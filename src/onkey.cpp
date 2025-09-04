/**
 * This file is part of OnKey.
 *
 * OnKey is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OnKey is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with OnKey.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "onkey.hpp"
#include "vkcodes.hpp"
#include <winuser.h>
#include <string>
#include <sstream>
#include <unordered_set>

namespace onkey
{
    auto pluginName = "onkey";

    bool keyShift   = false;
    bool keyApps    = false;
    bool keyLWin    = false;
    bool keyRWin    = false;
    bool keyAlt     = false;
    bool keyControl = false;

    plugin* myPlugin = NULL;

    std::unordered_set<std::string> handledKeys;

    bool plugin::myOnKey(WPARAM wParam, LPARAM lParam, bool handled)
    {
        WORD vkCode        = LOWORD(wParam);
        WORD keyFlags      = HIWORD(lParam);
        WORD scanCode      = LOBYTE(keyFlags);
        BOOL isExtendedKey = (keyFlags & KF_EXTENDED) == KF_EXTENDED;
        if (isExtendedKey)
            scanCode = MAKEWORD(scanCode, 0xE0);
        BOOL wasKeyDown    = (keyFlags & KF_REPEAT) == KF_REPEAT;
        WORD repeatCount   = LOWORD(lParam);
        BOOL isKeyReleased = (keyFlags & KF_UP) == KF_UP;

        // Keep track of modifier key states.
        if (isKeyReleased) {
            switch (vkCode) { 
                case VK_APPS:
                    keyApps = false;
                    break;
                case VK_SHIFT:
                    keyShift = false;
                    break;
                case VK_LWIN:
                    keyLWin = false;
                    break;
                case VK_RWIN:
                    keyRWin = false;
                    break;
                case VK_MENU:
                    keyAlt = false;
                    break;
                case VK_CONTROL:
                    keyControl = false;
                    break;
                default:
                    break;
            }
        }
        else if (!wasKeyDown) {
            switch (vkCode) {
                case VK_APPS:
                    keyApps = true;
                    break;
                case VK_SHIFT:
                    keyShift = true;
                    break;
                case VK_LWIN:
                    keyLWin = true;
                    break;
                case VK_RWIN:
                    keyRWin = true;
                    break;
                case VK_MENU:
                    keyAlt = true;
                    break;
                case VK_CONTROL:
                    keyControl = true;
                    break;
                default:
                    break;
            }

            // Setup the basic alias format.
            std::string command = "/onkey_";
            auto vkCodeString = keyboard_get_virtual_key_code_name(vkCode);

            // If the VK Code is enabled in the configuration...
            if (handledKeys.contains(vkCodeString)) {

                // ..add its name to the alias command...
                command.append(vkCodeString);
                
                // ...along with any modifier keys...
                if (keyApps) {command.append("_apps");}
                if (keyControl) {command.append("_ctrl");}
                //if (keyAlt) {command.append("_alt");} // can get stuck after alt tabbing
                if (keyShift) {command.append("_shift");}
                if (keyLWin || keyRWin) {command.append("_meta");}

                // ...and finally run the alias.
                this->core_->GetChatManager()->QueueCommand(1, command.c_str());
                return true;
            }
        }
        return false;
    }

    BOOL __stdcall OnKey(WPARAM wParam, LPARAM lParam, bool handled)
    {
        return myPlugin->myOnKey(wParam, lParam, handled);
    }

    /**
     * Constructor and Deconstructor
     */
    plugin::plugin(void)
        : core_{nullptr}
        , log_{nullptr}
        , device_{nullptr}
        , id_{0}
    {}
    plugin::~plugin(void)
    {}

    /**
     * Returns the plugins name.
     *
     * @return {const char*} The plugins name.
     */
    auto plugin::GetName(void) const -> const char*
    {
        return pluginName;
    }

    /**
     * Returns the plugins author.
     *
     * @return {const char*} The plugins author.
     */
    auto plugin::GetAuthor(void) const -> const char*
    {
        return "wolliw";
    }

    /**
     * Returns the plugins description.
     *
     * @return {const char*} The plugins description.
     */
    auto plugin::GetDescription(void) const -> const char*
    {
        return "OnKey is a plugin used to bind unsupported keys to aliases associated with arbitrary VK codes.";
    }

    /**
     * Returns the plugins homepage link.
     *
     * @return {const char*} The plugins homepage link.
     */
    auto plugin::GetLink(void) const -> const char*
    {
        return "https://none";
    }

    /**
     * Returns the plugins version.
     *
     * @return {double} The plugins version.
     */
    auto plugin::GetVersion(void) const -> double
    {
        return 0.1f;
    }

    /**
     * Returns the plugins Ashita SDK interface version it was compiled against.
     *
     * @return {double} The plugins interface version.
     */
    auto plugin::GetInterfaceVersion(void) const -> double
    {
        return ASHITA_INTERFACE_VERSION;
    }

    /**
     * Returns the plugins execution priority.
     *
     * @return {int32_t} The plugins execution priority.
     * @notes
     *
     *      The priority value is used to determine the execution order of the plugins loaded in Ashita. When an event is fired, Ashita
     *      will loop through each of the loaded plugins and allow them to attempt to handle the event. The order that the plugins are
     *      passed the event is determined by this priority. By default, 0 will load a plugin 'next' in the list of plugins who also are
     *      set to 0. Meaning if other plugins are already loaded at 0, and your plugin is also 0 your plugin will be last.
     *
     *      Plugins can force themselves sooner or later into the execution order by using this value (both negative and position).
     *      Negative values will place your plugin sooner in the execution order, while positive values will place it later.
     *
     *      Plugins should use 0 by default if order does not matter to their purpose.
     */
    auto plugin::GetPriority(void) const -> int32_t
    {
        return 0;
    }

    /**
     * Returns the plugins flags.
     *
     * @return {uint32_t} The plugins flags.
     * @notes
     *
     *      Ashita offers a various number of events that can be invoked inside of a plugin, allowing plugins to have a lot of control
     *      over the flow of the game. However, this can create a lot of overhead, more specifically on lower end machines. Plugins can
     *      use this property to specify the exact events it plans to make use of, allowing Ashita to only invoke ones it needs.
     *
     *      Plugins should explicitly state which flags they plan to make use of and avoid using 'All' flags.
     *      This can help with the performance of the game for lower end machines.
     *
     *      See the 'Ashita::PluginFlags' enumeration for more information on what each flag does.
     */
    auto plugin::GetFlags(void) const -> uint32_t
    {
        return (uint32_t)Ashita::PluginFlags::None;
    }

    /**
     * Event invoked when the plugin is being loaded and initialized.
     *
     * @param {IAshitaCore*} core - The main Ashita core instance used to interact with the Ashita hook.
     * @param {ILogManager*} logger - The log manager instance used to write to the debug log files.
     * @param {uint32_t} id - The plugins unique id. (Matches the plugins module base address.)
     * @return {bool} True on success, false otherwise.
     *
     * @notes
     *
     *      Plugins must return true from this function in order to be considered valid and continue to load.
     *      If your plugin fails to meet any requirements you feel are manditory for it to run, you should return false here and prevent it
     *      from loading further.
     */
    auto plugin::Initialize(IAshitaCore* core, ILogManager* logger, const uint32_t id) -> bool
    {
        // Store the incoming parameters for later use..
        this->core_ = core;
        this->log_  = logger;
        this->id_   = id;

        myPlugin = this;

        // Read (or create) the configuration file.
        auto config = false;
        config = core->GetConfigurationManager()->Load(pluginName, pluginName);
        if (config) {

            char buffer[10000]; // This is bad, but I'm being lazy.
            
            // Read and insert the configured keys into the set of handled keys.
            auto result = core->GetConfigurationManager()->GetSectionKeys(pluginName, "Keys", buffer, sizeof(buffer));
            std::stringstream ss(buffer);
            std::string token;
            while (std::getline(ss, token, '\n')) {
                if (core->GetConfigurationManager()->GetBool(pluginName, "Keys", token.c_str(), false)) {
                    handledKeys.insert(token.c_str());
                }
            }

        } else {
            // Create a dummy config file if one didn't exist.
            core->GetConfigurationManager()->SetValue(pluginName, "Keys", "VK_F13", "false");
            core->GetConfigurationManager()->SetValue(pluginName, "Keys", "VK_F14", "false");
            core->GetConfigurationManager()->SetValue(pluginName, "Keys", "VK_F15", "false");
            core->GetConfigurationManager()->SetValue(pluginName, "Keys", "VK_F16", "false");
            core->GetConfigurationManager()->Save(pluginName, pluginName);
        }


        core->GetInputManager()->GetKeyboard()->AddCallback(pluginName, NULL, NULL, OnKey);

        return true;
    }

    /**
     * Event invoked when the plugin is being unloaded.
     *
     * @notes
     *
     *      Plugins should use this event to cleanup all resources they created or used during their lifespan.
     *      (ie. Fonts, primitives, textures, Direct3D related resources, memory allocations, etc.)
     */
    auto plugin::Release(void) -> void
    {
        this->core_->GetInputManager()->GetKeyboard()->RemoveCallback(pluginName);
    }

} // namespace onkey

/**
 * Returns an instance of this plugins main class.
 *
 * @param {const char*} args - The arguments passed to the plugin when it was loaded.
 * @return {IPlugin*} The plugins class object instance.
 * @notes
 *
 *      This export must be present for a plugin to load properly!
 *
 *      The args parameter can be used to prepare the plugin in any special manner for specific properties based on the arguments passed.
 *      For example, if you wish to have a password required to load the plugin, this can be used for that kind of thing. (Not recommended
 *      as this is a weak form of authentication, just an example.)
 */
__declspec(dllexport) auto __stdcall expCreatePlugin(const char* args) -> IPlugin*
{
    UNREFERENCED_PARAMETER(args);

    return new onkey::plugin();
}

/**
 * Returns the plugins Ashita interface version it was compiled against.
 *
 * @return {double} The plugins interface version.
 * @notes
 *
 *      This export must be present for a plugin to load properly!
 */
__declspec(dllexport) auto __stdcall expGetInterfaceVersion(void) -> double
{
    return ASHITA_INTERFACE_VERSION;
}