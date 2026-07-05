#include "pch.h"
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <io.h>
#include <vector>
#include "FastFunctionHook.hpp"
#include "tsonic.h"

#define BIT(n) (1 << n)

#define UISCALE_ALIGN_CENTER BIT(0)
#define UISCALE_ALIGN_AUTO   BIT(1)

#define UISCALE_ALIGN_TOP    BIT(2)
#define UISCALE_ALIGN_BOTTOM BIT(3)

#define UISCALE_ALIGN_LEFT   BIT(4)
#define UISCALE_ALIGN_RIGHT  BIT(5)

Rt2dFont* __cdecl rt2dFontShowBitmap_r(Rt2dFont* a1, const RwChar* a2, RwReal scl, RwV2d* anchor, Rt2dBrush* brush);
Rt2dMaestro* __cdecl Rt2dMaestroRender_r(Rt2dMaestro* maestro);
RwBool __cdecl RwIm2DRenderPrimitive_r(RwPrimitiveType primType, RwIm2DVertex* vertices, RwInt32 numVertices);

FastFunctionHookPtr<decltype(&rt2dFontShowBitmap_r)> rt2dFontShowBitmap_h(0x6AA640);
FastFunctionHookPtr<decltype(&Rt2dMaestroRender_r)> Rt2dMaestroRender_h(0x6B4340);
FastFunctionHookPtr<decltype(&RwIm2DRenderPrimitive_r)> RwIm2DRenderPrimitive_h(0x64CA10);

namespace uiscale
{
    std::vector<int> anchor_stack;

    void push(int anchor)
    {
        anchor_stack.push_back(anchor);
    }

    void pop()
    {
        anchor_stack.pop_back();
    }

    int get()
    {
        return anchor_stack.size() ? anchor_stack.back() : UISCALE_ALIGN_CENTER;
    }

    // In maestro/font units
    bool get_offset(float* outx, float* outy)
    {
        int anchor = uiscale::get();

        if (anchor == UISCALE_ALIGN_CENTER)
        {
            return false;
        }

        if (pModeSwitch->PLAY_MODE)
        {
            return false; // Ignore 2P for now
        }

        float inner_aspect = 4.0f / 3.0f;
        float outer_aspect = (float)RsGlobal.maximumWidth / (float)RsGlobal.maximumHeight;

        float offset_x = 0.0f;
        float offset_y = 0.0f;

        if (outer_aspect >= inner_aspect)
        {
            float W = inner_aspect / outer_aspect;
            offset_x = (1.0f - W) / (2.0f * W);
            offset_x *= 1.2311481f; // ?
        }
        else
        {
            float H = outer_aspect / inner_aspect;
            offset_y = (1.0f - H) / (2.0f * H);
        }

        if (anchor & UISCALE_ALIGN_TOP)
        {
            offset_y = fabsf(offset_y);
        }
        else if (anchor & UISCALE_ALIGN_BOTTOM)
        {
            offset_y = -fabsf(offset_y);
        }
        else
        {
            offset_y = 0.0f;
        }

        if (anchor & UISCALE_ALIGN_LEFT)
        {
            offset_x = -fabsf(offset_x);
        }
        else if (anchor & UISCALE_ALIGN_RIGHT)
        {
            offset_x = fabsf(offset_x);
        }
        else
        {
            offset_x = 0.0f;
        }

        if (outx) *outx = offset_x;
        if (outy) *outy = offset_y;

        return true;
    }

    // In pixels
    bool get_offset2(float* outx, float* outy)
    {
        int anchor = uiscale::get();

        if (anchor == UISCALE_ALIGN_CENTER)
        {
            return false;
        }

        if (pModeSwitch->PLAY_MODE)
        {
            return false; // Ignore 2P for now
        }

        float window_w = (float)RsGlobal.maximumWidth;
        float window_h = (float)RsGlobal.maximumHeight;

        float box_w, box_h;

        float inner_aspect = 4.0f / 3.0f;
        float outer_aspect = window_w / window_h;

        float offset_x = 0.0f;
        float offset_y = 0.0f;

        if (outer_aspect >= inner_aspect)
        {
            box_h = window_h;
            box_w = box_h * inner_aspect;
        }
        else
        {
            box_w = window_w;
            box_h = box_w / inner_aspect;
        }

        offset_x = (window_w - box_w) * 0.5f;
        offset_y = (window_h - box_h) * 0.5f;

        if (anchor & UISCALE_ALIGN_TOP)
        {
            offset_y = -fabsf(offset_y);
        }
        else if (anchor & UISCALE_ALIGN_BOTTOM)
        {
            offset_y = fabsf(offset_y);
        }
        else
        {
            offset_y = 0.0f;
        }

        if (anchor & UISCALE_ALIGN_LEFT)
        {
            offset_x = -fabsf(offset_x);
        }
        else if (anchor & UISCALE_ALIGN_RIGHT)
        {
            offset_x = fabsf(offset_x);
        }
        else
        {
            offset_x = 0.0f;
        }

        if (outx) *outx = offset_x;
        if (outy) *outy = offset_y;

        return true;
    }
}

Rt2dFont* __cdecl rt2dFontShowBitmap_r(Rt2dFont* a1, const RwChar* a2, RwReal scl, RwV2d* anchor, Rt2dBrush* brush)
{
    float shiftx, shifty;
    if (uiscale::get_offset(&shiftx, &shifty))
    {
        RwV2d newAnchor = *anchor;

        newAnchor.x += shiftx;
        newAnchor.y += shifty;

        RwV2d backup = newAnchor;

        Rt2dFont* res = rt2dFontShowBitmap_h.Original(a1, a2, scl, &newAnchor, brush);

        // Font system moves anchor
        anchor->x += newAnchor.x - backup.x;
        anchor->y += newAnchor.y - backup.y;

        return res;
    }
    else
    {
        return rt2dFontShowBitmap_h.Original(a1, a2, scl, anchor, brush);
    }
}

template<int align>
Rt2dFont* __cdecl rt2dFontShowBitmap_Anchor(Rt2dFont* a1, const RwChar* a2, RwReal scl, RwV2d* anchor, Rt2dBrush* brush)
{
    uiscale::push(align);
    Rt2dFont* res = rt2dFontShowBitmap_r(a1, a2, scl, anchor, brush);
    uiscale::pop();
    return res;
}

Rt2dFont* __cdecl  Rt2dFontShow_GDisp(Rt2dFont* a1, const RwChar* a2, RwReal scl, RwV2d* anchor, Rt2dBrush* brush)
{
    if (a2 == (char*)pTObjTeamManagerFor1P + 68 ||
        a2 == (char*)pTObjTeamManagerFor1P + 77 ||
        a2 == (char*)pTObjTeamManagerFor1P + 80)
    {
        return rt2dFontShowBitmap_Anchor<UISCALE_ALIGN_BOTTOM | UISCALE_ALIGN_RIGHT>(a1, a2, scl, anchor, brush); // Mission timer/counter
    }
    else if (a2 == TechniquePointsStrings[0] ||
        a2 == TechniquePointsStrings[1] ||
        a2 == TechniquePointsStrings[2] ||
        a2 == TechniquePointsStrings[3] ||
        a2 == TechniquePointsStrings[4] ||
        a2 == TechniquePointsStrings[5])
    {
        return rt2dFontShowBitmap_Anchor<UISCALE_ALIGN_TOP | UISCALE_ALIGN_LEFT>(a1, a2, scl, anchor, brush); // Score bonus
    }
    else if (!strcmp(a2, "1") || !strcmp(a2, "2") || !strcmp(a2, "3"))
    {
        return rt2dFontShowBitmap_Anchor<UISCALE_ALIGN_TOP | UISCALE_ALIGN_RIGHT>(a1, a2, scl, anchor, brush); // Level up
    }
    else
    {
        return rt2dFontShowBitmap_r(a1, a2, scl, anchor, brush);
    }
}

Rt2dFont* __cdecl  Rt2dFontShow_Timer(Rt2dFont* a1, const RwChar* a2, RwReal scl, RwV2d* anchor, Rt2dBrush* brush)
{
    if (ACTION__IsSpecialAndBonusStage(&Action))
    {
        return rt2dFontShowBitmap_Anchor<UISCALE_ALIGN_BOTTOM>(a1, a2, scl, anchor, brush);
    }
    else
    {
        return rt2dFontShowBitmap_Anchor<UISCALE_ALIGN_TOP | UISCALE_ALIGN_LEFT>(a1, a2, scl, anchor, brush);
    }
}

Rt2dMaestro* __cdecl Rt2dMaestroRender_r(Rt2dMaestro* maestro)
{
    float shiftx, shifty;
    if (uiscale::get_offset(&shiftx, &shifty))
    {
        RwMatrix mat = ((Rt2dObject*)(maestro->scene))->MTM;
        RwV3d vec = { shiftx, shifty };

        Rt2dCTMPush();
        Rt2dCTMSetIdentity();
        RwMatrixTranslate(&maestro->scene->MTM, &vec, rwCOMBINEPOSTCONCAT);
        Rt2dObjectMTMChanged((Rt2dObject*)maestro->scene);
        Rt2dCTMPop();

        Rt2dMaestro* res = Rt2dMaestroRender_h.Original(maestro);

        ((Rt2dObject*)(maestro->scene))->MTM = mat;

        return res;
    }
    else
    {
        return Rt2dMaestroRender_h.Original(maestro);
    }
}

template<int anchor>
Rt2dMaestro* __cdecl Rt2dMaestroRender_Anchor(Rt2dMaestro* maestro)
{
    uiscale::push(anchor);
    Rt2dMaestro* res = Rt2dMaestroRender_r(maestro);
    uiscale::pop();
    return res;
}

Rt2dMaestro* __cdecl Rt2dMaestroRender_GDisp(Rt2dMaestro* maestro)
{
    if (maestro == pMaestro_DispRing[0] ||
        maestro == pMaestro_DispSpKey ||
        maestro == pMaestro_DispSpKeyEffect)
    {
        return Rt2dMaestroRender_Anchor<UISCALE_ALIGN_TOP | UISCALE_ALIGN_LEFT>(maestro);
    }
    else if (maestro == pMaestro_DispChallenge)
    {
        return Rt2dMaestroRender_Anchor<UISCALE_ALIGN_BOTTOM | UISCALE_ALIGN_LEFT>(maestro);
    }
    else if (maestro == pMaestro_DispLeader[0] ||
        maestro == pMaestro_DispDeniedLeader[0] ||
        maestro == pMaestro_DispDeniedLeader[1] ||
        maestro == pMaestro_DispDeniedLeader[2] ||
        maestro == pMaestro_DispFlyGauge[0] ||
        maestro == pMaestro_DispSFAGauge[0] ||
        maestro == pMaestro_DispBButton[0] ||
        maestro == pMaestro_DispLevel[0] ||
        maestro == pMaestro_DispLevel[1] || 
        maestro == pMaestro_DispLevel[2])
    {
        return Rt2dMaestroRender_Anchor<UISCALE_ALIGN_TOP | UISCALE_ALIGN_RIGHT>(maestro);
    }
    else if (maestro == pMaestro_DispLeader[1] ||
        maestro == pMaestro_DispDeniedLeader[3] ||
        maestro == pMaestro_DispDeniedLeader[4] ||
        maestro == pMaestro_DispDeniedLeader[5] ||
        maestro == pMaestro_DispFlyGauge[1] ||
        maestro == pMaestro_DispSFAGauge[1] ||
        maestro == pMaestro_DispBButton[1] ||
        maestro == pMaestro_DispLevel[3] ||
        maestro == pMaestro_DispLevel[4] ||
        maestro == pMaestro_DispLevel[5])
    {
        return Rt2dMaestroRender_Anchor<UISCALE_ALIGN_TOP | UISCALE_ALIGN_RIGHT>(maestro);
    }
    else
    {
        return Rt2dMaestroRender_r(maestro);
    }
}

RwBool __cdecl RwIm2DRenderPrimitive_r(RwPrimitiveType primType, RwIm2DVertex* vertices, RwInt32 numVertices)
{
    float shiftx, shifty;
    if (uiscale::get_offset2(&shiftx, &shifty))
    {
        for (int i = 0; i < numVertices; ++i)
        {
            vertices[i].x += shiftx;
            vertices[i].y += shifty;
        }

        RwBool res = RwIm2DRenderPrimitive_h.Original(primType, vertices, numVertices);

        for (int i = 0; i < numVertices; ++i)
        {
            vertices[i].x -= shiftx;
            vertices[i].y -= shifty;
        }

        return res;
    }
    else
    {
        return RwIm2DRenderPrimitive_h.Original(primType, vertices, numVertices);
    }
}

template<int anchor>
RwBool __cdecl RwIm2DRenderPrimitive2_Anchor(RwPrimitiveType primType, RwIm2DVertex* vertices, RwInt32 numVertices)
{
    uiscale::push(anchor);
    RwBool res = RwIm2DRenderPrimitive_r(primType, vertices, numVertices);
    uiscale::pop();
    return res;
}

void InitializeOutput()
{
    // Get the handle to the current console output
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Redirect stdout to the console handle
    int fd = _open_osfhandle((intptr_t)hConsole, _O_TEXT);
    FILE* file = _fdopen(fd, "w");
    *stdout = *file;
    setvbuf(stdout, NULL, _IONBF, 0);

    // Also redirect std::cout to the console
    std::ios::sync_with_stdio();
    std::cout.clear();
}

extern "C" __declspec(dllexport) void InitMod(const char* path)
{
    InitializeOutput();

    // Adaptative HUD support
    rt2dFontShowBitmap_h.Hook(rt2dFontShowBitmap_r);
    Rt2dMaestroRender_h.Hook(Rt2dMaestroRender_r);
    RwIm2DRenderPrimitive_h.Hook(RwIm2DRenderPrimitive_r);

    WriteCall((void*)0x41E239, Rt2dMaestroRender_GDisp);

    WriteCall((void*)0x41E921, Rt2dFontShow_Timer);
    WriteCall((void*)0x41E37B, Rt2dFontShow_GDisp); // Queue system (level up number effect, chaotix timer/counter...)
    WriteCall((void*)0x41E585, rt2dFontShowBitmap_Anchor<UISCALE_ALIGN_TOP | UISCALE_ALIGN_LEFT>);
    WriteCall((void*)0x41EA2C, rt2dFontShowBitmap_Anchor<UISCALE_ALIGN_TOP | UISCALE_ALIGN_LEFT>);
    WriteCall((void*)0x41EA80, rt2dFontShowBitmap_Anchor<UISCALE_ALIGN_TOP | UISCALE_ALIGN_LEFT>);
    WriteCall((void*)0x41E641, rt2dFontShowBitmap_Anchor<UISCALE_ALIGN_BOTTOM | UISCALE_ALIGN_LEFT>);
    WriteCall((void*)0x4069FE, rt2dFontShowBitmap_Anchor<UISCALE_ALIGN_BOTTOM | UISCALE_ALIGN_RIGHT>); // Bobsled time/score
    WriteCall((void*)0x406A54, rt2dFontShowBitmap_Anchor<UISCALE_ALIGN_BOTTOM | UISCALE_ALIGN_RIGHT>); // Bobsled time/score
    WriteCall((void*)0x406AE4, rt2dFontShowBitmap_Anchor<UISCALE_ALIGN_BOTTOM | UISCALE_ALIGN_RIGHT>); // Bobsled time/score
    WriteCall((void*)0x406B24, rt2dFontShowBitmap_Anchor<UISCALE_ALIGN_BOTTOM | UISCALE_ALIGN_RIGHT>); // Bobsled time/score

    WriteCall((void*)0x52639C, RwIm2DRenderPrimitive2_Anchor<UISCALE_ALIGN_TOP | UISCALE_ALIGN_RIGHT>);
    WriteCall((void*)0x526538, RwIm2DRenderPrimitive2_Anchor<UISCALE_ALIGN_TOP | UISCALE_ALIGN_RIGHT>);
    WriteCall((void*)0x527191, RwIm2DRenderPrimitive2_Anchor<UISCALE_ALIGN_TOP | UISCALE_ALIGN_LEFT>);

    // Remove horizontal scaling (could also remove the function calls)
    WriteData((float*)0x406849, 1.0f); // Bobsled
    WriteData((float*)0x406CAF, 1.0f); // Bobsled
    WriteData((float*)0x41E131, 1.0f); // Goal
    WriteData((float*)0x41E26D, 1.0f); // Continue
    WriteData((float*)0x41E31C, 1.0f); // General UI
    WriteData((float*)0x78A394, 1.33f); // Subtitles

    // Remove reverse horizontal scaling (could also remove the function calls)
    WriteData((float*)0x8DCBFC, 1.0f);
    WriteData((float*)0x8DCC48, 1.0f);
    WriteData((float*)0x8DCC84, 1.0f);
    WriteData((float*)0x8DCC9C, 1.0f);
    WriteData((float*)0x8DCCB4, 1.0f);
    WriteData((float*)0x8DCCE0, 1.0f);
    WriteData((float*)0x8DCCF8, 1.0f);
    WriteData((float*)0x4C3CAE, 1.0f); // Slot Machine 2D Effects

    // Fix 4/3 hud positions after horizontal scaling changes
    WriteData((float*)0x7BB2F0, 0.081f); // GDISP_SCORE_X
    WriteData((float*)0x7BB2E8, 0.081f); // GDISP_TIME_X
    WriteData((float*)0x7BB2C8, 0.154f); // GDISP_RING_X
    WriteData((float*)0x7BB2CC, 0.785f); // GDISP_RING_Y
    WriteData((float*)0x7BB2D0, 0.154f); // GDISP_RING2P_X1
    WriteData((float*)0x7BB2D4, 0.860f); // GDISP_RING2P_Y1
    WriteData((float*)0x7BB2D8, 0.708f); // GDISP_RING2P_X2
    WriteData((float*)0x7BB2DC, 0.860f); // GDISP_RING2P_Y2
    WriteData((float*)0x5B4996, 0.081f); // BONUS_POINT_X
    WriteData((float*)0x78A3EC, 0.72f); // BONUS_POINT_Y
    WriteData((float*)0x781B30, 0.081f); // BONUS_POINT_SPEED
    WriteData((float*)0x7BB2E0, 0.189f); // GDISP_LIVE_X
    WriteData((float*)0x7BB2E4, 0.081f); // GDISP_LIVE_Y
    WriteData((float*)0x8DDCE4, 0.99f); // LVLUP_SPD_X
    WriteData((float*)0x8DDCE8, 0.69f); // LVLUP_SPD_Y
    WriteData((float*)0x8DDCEC, 0.94f); // LVLUP_FLY_X
    WriteData((float*)0x8DDCF4, 1.03f); // LVLUP_POW_X
    WriteData((float*)0x8DDCCC, 0.0007f); // LVLUP_SPD_TRAG_X
    WriteData((float*)0x8DDCD0, 0.0005f); // LVLUP_SPD_TRAG_Y
    WriteData((float*)0x8DDCD4, 0.00009f); // LVLUP_FLY_TRAG_X
    WriteData((float*)0x8DDCD8, 0.0025f); // LVLUP_FLY_TRAG_Y
    WriteData((float*)0x8DDCDC, 0.0015f); // LVLUP_POW_TRAG_X
    WriteData((float*)0x8DDCE0, 0.0025f); // LVLUP_POW_TRAG_Y
    WriteData((float*)0x4067C9, 0.874f); // BOBS_BEST_X
    WriteData((float*)0x4067B9, 0.874f); // BOBS_TIME_X
    WriteData((float*)0x5AA0E0, 0.768f); // EXTRA_TIME_X
    WriteData((float*)0x78A23C, 1.155f); // EXTRA_COUNT_X
    WriteData((float*)0x41E913, 0.481f); // SS_TIME_X
    WriteData((float*)0x527407, 458.0f); // SS_GAUGE_BASE_X
    WriteData((float*)0x527402, 30.5f); // SS_GAUGE_BASE_Y
    WriteData((float*)0x5274BF, 466.0f); // SS_GAUGE_LVL1_X
    WriteData((float*)0x5274B5, 506.0f); // SS_GAUGE_LVL2_X
    WriteData((float*)0x5274AB, 546.0f); // SS_GAUGE_LVL3_X
    WriteData((float*)0x52759D, 36.5f); // SS_GAUGE_LVL_Y
    WriteData((float*)0x527927, 474.0f); // SS_LEVEL!_X
    WriteData((float*)0x52798C, 33.0f); // SS_LEVEL!_Y
    WriteData((float*)0x52698C, 45.0f); // SS_SCORE01_X
    WriteData((float*)0x5269A2, 66.0f); // SS_SCORE02_X
    WriteData((float*)0x5269B1, 57.0f); // SS_SCORE03_X
    WriteData((float*)0x438217, 0.702f); // RSLA_SPDLV_X
    WriteData((float*)0x438227, 0.702f); // RSLA_FLYLV_X
    WriteData((float*)0x438237, 0.702f); // RSLA_POWLV_X
    WriteData((float*)0x438304, 0.659f); // RSLA_TIME_X
    WriteData((float*)0x438320, 0.889f); // RSLA_RING_X
    WriteData((float*)0x438330, 0.814f); // RSLA_BONUS_X
    WriteData((float*)0x4384FB, 0.687f); // RSLA_TOTAL_X
    WriteData((float*)0x438340, 1.005f); // RSLA_STAR_TIME_X
    WriteData((float*)0x438350, 1.005f); // RSLA_STAR_RING_X
    WriteData((float*)0x438509, 1.005f); // RSLA_STAR_TOTAL_X
    WriteData((float*)0x4385C7, 0.61f); // RSLB_TIME_X
    WriteData((float*)0x4385ED, 1.0f); // RSLB_STAR_TIME_X
    WriteData((float*)0x438779, 0.716f); // RSLS_SCORE_X
    WriteData((float*)0x438823, 0.6708f); // RSLS_TIME_X
    WriteData((float*)0x438833, 0.815f); // RSLS_TIMESCR_X
    WriteData((float*)0x438843, 0.82f); // RSLS_GAUGESCR_X
    WriteData((float*)0x4389ED, 0.6875f); // RSLS_TOTAL_X
}