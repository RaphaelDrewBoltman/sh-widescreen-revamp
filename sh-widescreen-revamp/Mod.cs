using Reloaded.Hooks.ReloadedII.Interfaces;
using Reloaded.Mod.Interfaces;
using Reloaded.Memory;
using Reloaded.Memory.Interfaces;
using Reloaded.Universal.Redirector.Interfaces;
using sh_widescreen_revamp.Configuration;
using sh_widescreen_revamp.Template;
using System;
using System.Xml.Linq;
using System.Runtime.CompilerServices;
using static System.Net.Mime.MediaTypeNames;

namespace sh_widescreen_revamp
{
    /// <summary>
    /// Your mod logic goes here.
    /// </summary>
    public unsafe class Mod : ModBase // <= Do not Remove.
    {
        /// <summary>
        /// Provides access to the mod loader API.
        /// </summary>
        private readonly IModLoader _modLoader;

        /// <summary>
        /// Provides access to the Reloaded.Hooks API.
        /// </summary>
        /// <remarks>This is null if you remove dependency on Reloaded.SharedLib.Hooks in your mod.</remarks>
        private readonly IReloadedHooks? _hooks;

        /// <summary>
        /// Provides access to the Reloaded.Universal.Redirector API.
        /// </summary>
        /// <remarks>This is null if you remove dependency on Reloaded.Universal.Redirector in your mod.</remarks>
        private readonly IRedirectorController? _redirector;

        /// <summary>
        /// Provides access to the Reloaded logger.
        /// </summary>
        private readonly ILogger _logger;

        /// <summary>
        /// Entry point into the mod, instance that created this class.
        /// </summary>
        private readonly IMod _owner;

        /// <summary>
        /// Provides access to this mod's configuration.
        /// </summary>
        private Config _configuration;

        /// <summary>
        /// The configuration of the currently executing mod.
        /// </summary>
        private readonly IModConfig _modConfig;

        struct RwV2d
        {
            public float x;
            public float y;
        };

        RwV2d* RingAnchor = (RwV2d*)0x7BB2C8;
        RwV2d* TimeAnchor = (RwV2d*)0x7BB2E8;
        RwV2d* ScoreAnchor = (RwV2d*)0x7BB2F0;
        RwV2d* LivesAnchor = (RwV2d*)0x7BB2E0;

        void WriteFloat(nuint memoryAddress, float value)
        {
            var data = BitConverter.GetBytes(value);
            Memory.Instance.SafeWrite(memoryAddress, data);
        }

        void WriteByte(nuint memoryAddress, byte value)
        {
            byte[] data = { value };
            Memory.Instance.SafeWrite(memoryAddress, data);
        }

        void WriteNop(nuint memoryAddress, int count)
        {
            var data = new byte[count];
            for (int i = 0; i < count; i++)
                data[i] = 0x90;
            Memory.Instance.SafeWrite(memoryAddress, data);
        }

        void AddAssetPath(string style)
        {
            string modpath = Path.GetFullPath(_modLoader.GetDirectoryForModId(_modConfig.ModId));
            _redirector?.AddRedirectFolder(modpath + "\\assets\\" + style, "/dvdroot");
        }

        public Mod(ModContext context)
        {
            _modLoader = context.ModLoader;
            _hooks = context.Hooks;
            _logger = context.Logger;
            _owner = context.Owner;
            _modConfig = context.ModConfig;
            _configuration = context.Configuration;
            _redirector = context.Redirector;

            var memory = Memory.Instance;

            // Force Life Icon
            if (_configuration.PermanentLifeIcon)
            {
                WriteByte(0x420E87, 0x5);
                WriteNop(0x420EB0, 2);
                WriteNop(0x420DFB, 2);
            }

            // Change HUD Buttons based on selected console
            switch (_configuration.HUDStyle)
            {
                case Config.Styles.XBOX:
                    AddAssetPath("XBOX");
                    break;
                case Config.Styles.GameCube:
                    AddAssetPath("GameCube");
                    break;
                case Config.Styles.PlayStation2:
                    AddAssetPath("PlayStation 2");
                    break;
                case Config.Styles.Dreamcast:
                    AddAssetPath("Dreamcast");
                    break;
            }

            // Score Bonus
            WriteFloat(0x781B30, -0.115f); // X
            WriteFloat(0x5B4996, -0.115f); // X
            WriteFloat(0x78A3EC, 0.72f);   // Y

            // Level UP Effects
            WriteFloat(0x8DDCE4, 1.2f);     // POS LEFT X
            WriteFloat(0x8DDCEC, 1.13f);    // POS BOTTOM X
            WriteFloat(0x8DDCF4, 1.25f);    // POS RIGHT X
            WriteFloat(0x8DDCCC, 0.00065f); // SPD LEFT X
            WriteFloat(0x8DDCD0, 0.0005f);  // SPD LEFT Y
            WriteFloat(0x8DDCD4, 0.00009f); // SPD BOTTOM X
            WriteFloat(0x8DDCD8, 0.0005f);  // SPD BOTTOM Y
            WriteFloat(0x8DDCDC, 0.0015f);  // SPD RIGHT X
            WriteFloat(0x8DDCE0, 0.0005f);  // SPD RIGHT Y

            // Special Stage Gauge
            float gaugex = 565.0f;
            float gaugey = 30.5f;
            WriteFloat(0x527407, gaugex);
            WriteFloat(0x5274BF, gaugex + 7.0f);
            WriteFloat(0x5274B5, gaugex + 47.0f);
            WriteFloat(0x5274AB, gaugex + 87.0f);
            WriteFloat(0x527402, gaugey);
            WriteFloat(0x52759D, gaugey + 6.0f);

            // Special Stage Level UP
            WriteFloat(0x527927, 580.0f);  // SP X
            WriteFloat(0x527941, 610.0f);  // MP X
            WriteFloat(0x5279B2, 32.25f);  // Y
            WriteFloat(0x78A2D8, 32.25f);  // Y
            WriteFloat(0x52798C, 32.25f);  // Y

            // Special Stage Score Bonus
            WriteFloat(0x52698C, -56.5f); // X
            WriteFloat(0x5269B1, -47.5f); // X
            WriteFloat(0x5269A2, -38.5f); // X

            // Extra Mission Information
            WriteFloat(0x5AA0E0, 0.974f); // Time Trial
            WriteFloat(0x78A23C, 1.359f); // Chaotix Item Count

            // Bobsled
            WriteFloat(0x4067C9, 1.08f);
            WriteFloat(0x4067B9, 1.08f);

            // Denied Leader 2P
            WriteFloat(0x8DCCC4, -0.5585f); // Levels 2P
            WriteFloat(0x8DCCAC, -0.56f);   // Denied Leader 2P

            // Now Loading
            nuint c_NowLoadingCoord = 0x743C38;
            for (int i = 0; i < 7; ++i)
            {
                WriteFloat(c_NowLoadingCoord, 507.0f);
                c_NowLoadingCoord += 8;
            }

            // Normal Results Screen
            WriteFloat(0x438217, 0.7025f); // Speed Level
            WriteFloat(0x438227, 0.7025f); // Fly Level
            WriteFloat(0x438237, 0.7025f); // Power Level
            WriteFloat(0x438304, 0.66f);   // Time
            WriteFloat(0x438320, 0.89f);   // Ring
            WriteFloat(0x438330, 0.815f);  // Time Bonus
            WriteFloat(0x4384FB, 0.681f);   // Total Score
            WriteFloat(0x438340, 1.005f); // Time Star
            WriteFloat(0x438350, 1.005f); // Ring Star
            WriteFloat(0x438509, 1.005f); // Total Star

            // Boss Results Screen
            WriteFloat(0x4385C7, 0.61f); // Time
            WriteFloat(0x4385ED, 1.0f); // Star

            // Special Stage Results Screen
            WriteFloat(0x438779, 0.716f);  // Score
            WriteFloat(0x438823, 0.6708f); // Time
            WriteFloat(0x438833, 0.815f);  // Time Score
            WriteFloat(0x438843, 0.82f);   // Gauge Score
            WriteFloat(0x4389ED, 0.6875f); // Total Score

            // Normal HUD
            RingAnchor->x = -0.0515f;
            RingAnchor->y = 0.785f;
            TimeAnchor->x = -0.115f;
            ScoreAnchor->x = -0.115f;
            LivesAnchor->x = -0.015f;
            LivesAnchor->y = 0.081f;
            WriteFloat(0x41E913, 0.482f); // Special Stage Related
            WriteFloat(0x41E7A9, 0.482f); // Special Stage Related

            // Fix Horizontal Scaling (could also remove the function calls)
            WriteFloat(0x406849, 1.0f); // Bobsled
            WriteFloat(0x406CAF, 1.0f); // Bobsled
            WriteFloat(0x41E131, 1.0f); // Goal
            WriteFloat(0x41E26D, 1.0f); // Continue
            WriteFloat(0x41E31C, 1.0f); // General UI
            WriteFloat(0x78A394, 1.33f); // Subtitles

            // Fix Inverse Horizontal Scaling (could also remove the function calls)
            WriteFloat(0x8DCBFC, 1.0f);
            WriteFloat(0x8DCC48, 1.0f);
            WriteFloat(0x8DCC84, 1.0f);
            WriteFloat(0x8DCC9C, 1.0f);
            WriteFloat(0x8DCCB4, 1.0f);
            WriteFloat(0x8DCCE0, 1.0f);
            WriteFloat(0x8DCCF8, 1.0f);
            WriteFloat(0x4C3CAE, 1.0f); // Slot Machine 2D Effects
        }

        #region Standard Overrides
        public override void ConfigurationUpdated(Config configuration)
        {
            if (_configuration.PermanentLifeIcon != configuration.PermanentLifeIcon)
            {
                if (configuration.PermanentLifeIcon)
                {
                    WriteByte(0x420E87, 0x5);
                    WriteNop(0x420EB0, 2);
                    WriteNop(0x420DFB, 2);
                }
                else
                {
                    WriteByte(0x420E87, 0x6);
                    WriteByte(0x420EB0, 0x7A);
                    WriteByte(0x420EB1, 0x7B);
                    WriteByte(0x420DFB, 0x7A);
                    WriteByte(0x420DFC, 0x1A);
                }
            }

            _configuration = configuration;
            _logger.WriteLine($"[{_modConfig.ModId}] Config Updated: Applying");
        }
        #endregion

        #region For Exports, Serialization etc.
#pragma warning disable CS8618 // Non-nullable field must contain a non-null value when exiting constructor. Consider declaring as nullable.
        public Mod() { }
#pragma warning restore CS8618
        #endregion
    }
}