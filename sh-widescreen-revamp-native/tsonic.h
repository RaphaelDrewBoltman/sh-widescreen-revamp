#pragma once

#include <stdint.h>

//////////////////////////////////// RW ////////////////////////////////////

enum RwOpCombineType : __int32
{
	rwCOMBINEREPLACE = 0x0,
	rwCOMBINEPRECONCAT = 0x1,
	rwCOMBINEPOSTCONCAT = 0x2,
};

enum RwPrimitiveType : __int32
{
	rwPRIMTYPENAPRIMTYPE = 0x0,
	rwPRIMTYPELINELIST = 0x1,
	rwPRIMTYPEPOLYLINE = 0x2,
	rwPRIMTYPETRILIST = 0x3,
	rwPRIMTYPETRISTRIP = 0x4,
	rwPRIMTYPETRIFAN = 0x5,
	rwPRIMTYPEPOINTLIST = 0x6,
};


typedef char RwChar;
typedef uint8_t RwUInt8;
typedef int32_t RwInt32;
typedef uint32_t RwUInt32;
typedef float_t RwReal;
typedef bool RwBool;

struct RwIm2DVertex
{
	RwReal x;
	RwReal y;
	RwReal z;
	RwReal rhw;
	RwUInt32 emissiveColor;
	RwReal u;
	RwReal v;
};

struct RwV2d
{
	RwReal x;
	RwReal y;
};

struct RwV3d
{
	RwReal x;
	RwReal y;
	RwReal z;
};

struct RwRGBA
{
	RwUInt8 red;
	RwUInt8 green;
	RwUInt8 blue;
	RwUInt8 alpha;
};

struct RwRGBAReal
{
	RwReal red;
	RwReal green;
	RwReal blue;
	RwReal alpha;
};

struct rt2dShadeParameters
{
	RwRGBAReal col;
	RwV2d uv;
};

struct Rt2dBrush
{
	rt2dShadeParameters top;
	rt2dShadeParameters dtop;
	rt2dShadeParameters bottom;
	rt2dShadeParameters dbottom;
	RwRGBA colorCache;
	RwInt32 flag;
	void* texture;
	void* material;
	RwReal halfwidth;
	RwInt32 refCount;
};

struct RwSList
{
	RwUInt8* listElements;
	RwInt32 numElementsFilled;
	RwInt32 numElementsAlloced;
	RwInt32 entrySize;
};

struct Rt2dFont
{
	char gap0[4];
	RwReal field_4;
	RwReal yShift__;
	RwReal width__;
	char gap10[280];
	RwSList* field_128;
	Rt2dFont* (*fRenderFunc)();
	char gap130[4];
	int field_134;
};

struct RwMatrix
{
	RwV3d right;
	int flags;
	RwV3d up;
	RwUInt32 pad1;
	RwV3d at;
	RwUInt32 pad2;
	RwV3d pos;
	RwUInt32 pad3;
};

struct _rt2dScene
{
	RwSList* objects;
	RwInt32 objectCount;
	RwSList* depths;
	RwBool isDirtyDepths;
};

union _rt2dObjectdata
{
	//_rt2dShape shape;
	_rt2dScene scene;
	//_rt2dPickRegion pickRegion;
	//_rt2dObjectString objectString;
};

struct Rt2dObject
{
	RwInt32 type;
	RwInt32 depth;
	RwInt32 flag;
	RwMatrix MTM;
	RwMatrix LTM;
	RwRGBAReal colorMult;
	RwRGBAReal colorOffs;
	_rt2dObjectdata data;
};

struct Rt2dBBox
{
	RwReal x;
	RwReal y;
	RwReal w;
	RwReal h;
};

struct __declspec(align(4)) Rt2dMaestro
{
	int flagsMaybe;
	char gap4[4];
	RwSList* animations;
	const char** stringLabels;
	void* buttons;
	char field_14;
	void* messageList;
	void* byteBucket__;
	void* messageQueue;
	Rt2dObject* scene;
	int(__cdecl* callbackMaybe)(int, int);
	float float_2C;
	float float_30;
	char gap34[4];
	RwV3d camRelated;
	int field_44;
	Rt2dBBox bbox;
	void* stateTransTable;
};

FunctionPointer(Rt2dFont*, rt2dFontShowBitmap, (Rt2dFont* a1, const RwChar* a2, RwReal scl, RwReal* anchor, Rt2dBrush* brush), 0x6AA640);
FunctionPointer(Rt2dObject*, Rt2dObjectMTMTranslate, (Rt2dObject* object, RwReal x, RwReal y), 0x6ACE10);
FunctionPointer(Rt2dObject*, Rt2dObjectMTMScale, (Rt2dObject* object, RwReal x, RwReal y), 0x6ACDC0);
FunctionPointer(RwBool, Rt2dCTMTranslate, (RwReal x, RwReal y), 0x6AA330);
FunctionPointer(RwBool, Rt2dCTMSetIdentity, (), 0x6A9B60);
FunctionPointer(RwBool, Rt2dCTMPush, (), 0x6AA150);
FunctionPointer(RwBool, Rt2dCTMPop, (), 0x6AA2B0);
FunctionPointer(Rt2dObject*, Rt2dSceneUpdateLTM, (Rt2dObject* object), 0x6AD8B0);
FunctionPointer(Rt2dObject*, Rt2dObjectMTMChanged, (Rt2dObject* object), 0x06ACDB0);
FunctionPointer(Rt2dMaestro*, Rt2dMaestroUpdateAnimations, (Rt2dMaestro* maestro), 0x6B3FF0);
FunctionPointer(Rt2dMaestro*, Rt2dMaestroProcessMessages, (Rt2dMaestro* maestro), 0x6B4660);
FunctionPointer(RwBool, RwIm2DRenderPrimitive, (RwPrimitiveType primType, RwIm2DVertex* vertices, RwInt32 numVertices), 0x64CA10);
FunctionPointer(RwMatrix*, RwMatrixTranslate, (RwMatrix* matrix, RwV3d* translation, RwOpCombineType combineOp), 0x650E00);

//////////////////////////////////// GAME ////////////////////////////////////

struct RsGlobalType
{
	const RwChar* appName;
	RwInt32 maximumWidth;
	RwInt32 maximumHeight;
	RwInt32 maxFPS;
	RwBool quit;
	//psGlobalType* ps;
	//RsInputDevice keyboard;
	//RsInputDevice mouse;
	//RsInputDevice pad;
	//void* field_3C;
};

struct MODESWITCH
{
	char DEBUG;
	char DEBUG_COLLI;
	char DEBUG_C_COLLI;
	char DEBUG_PATH;
	char DEBUG_INFO;
	unsigned char EDITOR;
	unsigned char DEBUG_CAMERA;
	unsigned char DEBUG_LANDINFO;
	unsigned char DEBUG_BLOCKTEST;
	unsigned char DEBUG_PERFOMANCE;
	unsigned char DEBUG_SNAPSHOT;
	char DEBUG_SCORE;
	char DEBUG_ZAKORUSH;
	char DEBUG_CAMPATH;
	char DEBUG_BUKUCAM;
	char EDITION;
	char VERSION;
	char PALMode;
	char soundMode_HHC;
	char textLang;
	char speechLang_HHC;
	char field_15;
	char isPlayBGM;
	char isPlaySFX;
	unsigned char DEMO;
	char DEMO_STEP;
	char SIGN_DISP;
	char SCORE_DISP;
	char PAUSE_GAME_DISABLE;
	char PAUSE_GAME;
	unsigned char PLAY_MODE;
	unsigned char TIME_STOP;
	unsigned char SFA_SCREEN;
	char POSE_EFFECT;
	unsigned char GOAL;
	char NOTKILLED;
	unsigned char GAME_RULE;
	unsigned char MISS_RULE;
	char SPECIAL_KEY;
	unsigned char USE_EVENT;
	unsigned char MISSION;
	unsigned char SCREEN_2P_DIVIDE;
	char LAST_ADVMODE;
	char NOW_SAVING;
	int gamePlayTime;
	int frame;
	int timeStopframe;
	int systemMode;
	int field_3C;
	int eventNo;
};

DataPointer(int, Action, 0x8D66E8);
DataArray(const char*, TechniquePointsStrings, 0x8DDD14, 6);
DataArray(void*, teamTOp, 0xA4C268, 4);
DataArray(Rt2dMaestro*, pMaestro_DispRing, 0xA77710, 2);
DataPointer(Rt2dMaestro*, pMaestro_DispChallenge, 0xA77758);
DataArray(Rt2dMaestro*, pMaestro_DispLeader, 0xA77718, 2);
DataArray(Rt2dMaestro*, pMaestro_DispDeniedLeader, 0xA77738, 6);
DataArray(Rt2dMaestro*, pMaestro_DispFlyGauge, 0xA77750, 2);
DataArray(Rt2dMaestro*, pMaestro_DispSFAGauge, 0xA7775C, 2);
DataArray(Rt2dMaestro*, pMaestro_DispBButton, 0xA77764, 2);
DataPointer(Rt2dMaestro*, pMaestro_DispSpKey, 0xA7776C);
DataPointer(Rt2dMaestro*, pMaestro_DispSpKeyEffect, 0xA77770);
DataArray(Rt2dMaestro*, pMaestro_DispLevel, 0xA77720, 6);
DataPointer(MODESWITCH*, pModeSwitch, 0xA777E4);
DataPointer(RsGlobalType, RsGlobal, 0xA77938);
DataPointer(void*, pTObjTeamManagerFor1P, 0xA78AA8);

UsercallFunc(BOOL, ACTION__IsSpecialAndBonusStage, (void* _this), (_this), 0x404ED0, rEAX, rEAX);
FunctionPointer(void, InitMaestroObjectC, (Rt2dMaestro* maestro), 0x405AE0);
FunctionPointer(int, Rt2dDeviceGetStep, (RwV2d* xstep, RwV2d* ystep, RwV2d* origin), 0x6A9C50);