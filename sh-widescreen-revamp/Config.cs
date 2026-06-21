using Reloaded.Mod.Interfaces.Structs;
using sh_widescreen_revamp.Template.Configuration;
using System.ComponentModel;
using System.ComponentModel.DataAnnotations;

namespace sh_widescreen_revamp.Configuration
{
    public class Config : Configurable<Config>
    {
        /*
            User Properties:
                - Please put all of your configurable properties here.
    
            By default, configuration saves as "Config.json" in mod user config folder.    
            Need more config files/classes? See Configuration.cs
    
            Available Attributes:
            - Category
            - DisplayName
            - Description
            - DefaultValue

            // Technically Supported but not Useful
            - Browsable
            - Localizable

            The `DefaultValue` attribute is used as part of the `Reset` button in Reloaded-Launcher.
        */

        [DisplayName("Permanent Life Icon")]
        [Description("Always show the life icon in the bottom left corner.")]
        [DefaultValue(true)]
        public bool PermanentLifeIcon { get; set; } = true;


        [DisplayName("HUD Style")]
        [Description("Change the game's button style.")]
        [DefaultValue("Windows")]
        public Styles HUDStyle { get; set; } = Styles.Windows;

        public enum Styles
        {
            [Display(Name = "Windows")]
            Windows,
            [Display(Name = "GameCube")]
            GameCube,
            [Display(Name = "XBOX")]
            XBOX,
            [Display(Name = "Dreamcast")]
            Dreamcast,
            [Display(Name = "PlayStation 2")]
            PlayStation2,
        }
    }

    /// <summary>
    /// Allows you to override certain aspects of the configuration creation process (e.g. create multiple configurations).
    /// Override elements in <see cref="ConfiguratorMixinBase"/> for finer control.
    /// </summary>
    public class ConfiguratorMixin : ConfiguratorMixinBase
    {
        // 
    }
}
