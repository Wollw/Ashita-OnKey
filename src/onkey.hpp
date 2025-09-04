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

#ifndef ONKEY_HPP_INCLUDED
#define ONKEY_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/**
 * Main Ashita SDK Include
 * 
 * In order to compile a proper Ashita plugin, this header is required to be included. This header
 * includes the various other generally used parts of the Ashita SDK, as well as defines the various
 * SDK interfaces, enumerations, and other type definitions required.
 * 
 * To help with uniformed pathing, Ashita bases all SDK includes from a parent path. You can set this
 * parent path via the following environment variable on your system: 
 * 
 * ASHITA4_SDK_PATH
 */
#include "Ashita.h"

namespace onkey
{
    
    class plugin final : public IPlugin
    {
        IAshitaCore* core_;        // Pointer to the main AshitaCore object.
        ILogManager* log_;         // Pointer to the main LogManager object.
        IDirect3DDevice8* device_; // Pointer to the games Direct3D device object.
        uint32_t id_;              // The plugins ids. (The plugins current base address.)

    public:
        plugin(void);
        ~plugin(void) override;

        // Properties (Plugin Information)
        auto GetName(void) const -> const char* override;
        auto GetAuthor(void) const -> const char* override;
        auto GetDescription(void) const -> const char* override;
        auto GetLink(void) const -> const char* override;
        auto GetVersion(void) const -> double override;
        auto GetInterfaceVersion(void) const -> double override;
        auto GetPriority(void) const -> int32_t override;
        auto GetFlags(void) const -> uint32_t override;

        // Methods
        auto Initialize(IAshitaCore* core, ILogManager* logger, uint32_t id) -> bool override;
        auto Release(void) -> void override;

        bool myOnKey(WPARAM wParam, LPARAM lParam, bool handled);
    };

} // namespace onkey

#endif // ONKEY_HPP_INCLUDED