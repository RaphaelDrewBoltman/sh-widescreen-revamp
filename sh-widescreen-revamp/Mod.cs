using Reloaded.Hooks.ReloadedII.Interfaces;
using Reloaded.Memory;
using Reloaded.Memory.Interfaces;
using Reloaded.Mod.Interfaces;
using Reloaded.Universal.Redirector.Interfaces;
using sh_widescreen_revamp.Configuration;
using sh_widescreen_revamp.Template;
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Xml.Linq;
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

        [DllImport("sh-widescreen-revamp.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void InitMod(char* path);

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

            char* configpath = (char*)Marshal.StringToHGlobalAnsi(_configuration.FilePath).ToPointer();
            InitMod(configpath);
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