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

#ifndef AuthenticationPackets_h__
#define AuthenticationPackets_h__

#include "Packet.h"
#include <array>
#include <optional>
#include <string>

namespace WorldPackets
{
    namespace Auth
    {
        class AuthChallenge final : public ServerPacket
        {
        public:
            AuthChallenge() : ServerPacket(SMSG_AUTH_CHALLENGE, 37) { }

            WorldPacket const* Write() override;

            std::array<uint8, 32> DosChallenge = {};
            std::array<uint8, 4> Challenge = {};
            uint8 DosZeroBits = 1;
        };

        class AuthSession final : public ClientPacket
        {
        public:
            explicit AuthSession(WorldPacket&& packet) : ClientPacket(CMSG_AUTH_SESSION, std::move(packet)) { }

            void Read() override;

            int32 LoginServerID = 0;
            uint32 BattlegroupID = 0;
            int8 LoginServerType = 0;
            uint64 DosResponse = 0;
            uint16 Build = 0;
            uint32 RealmID = 0;
            int8 BuildType = 0;
            std::array<uint8, 4> LocalChallenge = {};
            uint32 RegionID = 0;
            std::array<uint8, 20> Digest = {};
            ByteBuffer AddonInfo;
            bool UseIPv6 = false;
            std::string Account;
        };

        struct AuthSuccessInfo
        {
            uint32 TimeRemain = 0;
            uint8 ActiveExpansionLevel = 0;
            uint32 TimeSecondsUntilPCKick = 0;
            uint8 AccountExpansionLevel = 0;
            uint32 TimeRested = 0;
            uint8 TimeOptions = 0;
        };

        struct AuthWaitInfo
        {
            uint32 WaitCount = 0;
            bool HasFCM = false;
        };

        class AuthResponse final : public ServerPacket
        {
        public:
            explicit AuthResponse(uint8 result) : ServerPacket(SMSG_AUTH_RESPONSE, 21), Result(result) { }

            WorldPacket const* Write() override;

            std::optional<AuthSuccessInfo> SuccessInfo;
            std::optional<AuthWaitInfo> WaitInfo;
            uint8 Result;
        };

        // Build 15595's second (instance) world connection, ported from the Cataclysm Preservation
        // Project (pinned TrinityCore-Cata AuthenticationPackets).

        enum class ConnectToSerial : uint32
        {
            None            = 0,
            Realm           = 14,
            WorldAttempt1   = 17,
            WorldAttempt2   = 35,
            WorldAttempt3   = 53,
            WorldAttempt4   = 71,
            WorldAttempt5   = 89
        };

        // Sent on the realm socket: where to open the instance socket. The address payload is
        // HMAC-bound and RSA-signed; the client verifies it with the modulus scripts/patch_client.py
        // puts into Wow-64-maelstrom.exe (the Cataclysm Preservation Project launcher modulus).
        class ConnectTo final : public ServerPacket
        {
        public:
            static std::string const Haiku;
            static uint8 const PiDigits[142];

            ConnectTo() : ServerPacket(SMSG_CONNECT_TO, 8 + 4 + 256 + 1) { }

            WorldPacket const* Write() override;

            uint64 Key = 0;
            ConnectToSerial Serial = ConnectToSerial::None;
            std::array<uint8, 16> Address = {};     // IPv4 in the first four bytes
            bool AddressIsV6 = false;
            uint16 Port = 0;
            uint8 XorMagic = 0x2A;
            uint8 Con = 0;
        };

        // First packet on the instance socket, in place of CMSG_AUTH_SESSION.
        class AuthContinuedSession final : public ClientPacket
        {
        public:
            explicit AuthContinuedSession(WorldPacket&& packet) : ClientPacket(CMSG_AUTH_CONTINUED_SESSION, std::move(packet)) { }

            void Read() override;

            uint64 DosResponse = 0;
            uint64 Key = 0;
            std::array<uint8, 20> Digest = {};
        };

        class ConnectToFailed final : public ClientPacket
        {
        public:
            explicit ConnectToFailed(WorldPacket&& packet) : ClientPacket(CMSG_CONNECT_TO_FAILED, std::move(packet)) { }

            void Read() override;

            ConnectToSerial Serial = ConnectToSerial::None;
            uint8 Con = 0;
        };

        // Tells the client the instance socket is ready and the login continues on it.
        class ResumeComms final : public ServerPacket
        {
        public:
            ResumeComms() : ServerPacket(SMSG_RESUME_COMMS, 0) { }

            WorldPacket const* Write() override { return &_worldPacket; }
        };
    }
}

#endif // AuthenticationPackets_h__
