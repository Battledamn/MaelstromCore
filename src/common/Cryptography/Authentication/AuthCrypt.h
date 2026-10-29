/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _AUTHCRYPT_H
#define _AUTHCRYPT_H

#include "ARC4.h"
#include "AuthDefines.h"
#include <array>

class AC_COMMON_API AuthCrypt
{
public:
    AuthCrypt() = default;

    void Init(SessionKey const& K);
    // Build 15595 instance connection: keyed with the 16-byte seeds the socket sent in its
    // SMSG_AUTH_CHALLENGE instead of the fixed realm keys (pinned TrinityCore-Cata WorldPacketCrypt).
    void Init(SessionKey const& K, std::array<uint8, 16> const& serverKey, std::array<uint8, 16> const& clientKey);
    void DecryptRecv(uint8* data, std::size_t len);
    void EncryptSend(uint8* data, std::size_t len);

    bool IsInitialized() const { return _initialized; }

private:
    Acore::Crypto::ARC4 _clientDecrypt;
    Acore::Crypto::ARC4 _serverEncrypt;
    bool _initialized{ false };
};
#endif
