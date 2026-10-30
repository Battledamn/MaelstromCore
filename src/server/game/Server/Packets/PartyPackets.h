/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * You may redistribute it and/or modify it under the terms of the GNU General Public License
 * version 2 or, at your option, any later version.
 */

#ifndef PartyPackets_h__
#define PartyPackets_h__

#include "ObjectGuid.h"
#include "Optional.h"
#include "Packet.h"
#include <string>
#include <vector>

class Player;

// Cataclysm 4.3.4 (build 15595) party packets, ported from The Cataclysm Preservation Project
// (src/server/game/Server/Packets/PartyPackets.* at b061f46).
namespace WorldPackets
{
    namespace Party
    {
        class PartyCommandResult final : public ServerPacket
        {
        public:
            PartyCommandResult() : ServerPacket(SMSG_PARTY_COMMAND_RESULT, 4 + 1 + 4 + 4 + 8) { }

            WorldPacket const* Write() override;

            std::string Name;
            uint32 Command = 0;
            uint32 Result = 0;
            uint32 ResultData = 0;
            ObjectGuid ResultGUID;
        };

        class PartyInviteClient final : public ClientPacket
        {
        public:
            PartyInviteClient(WorldPacket&& packet) : ClientPacket(CMSG_PARTY_INVITE, std::move(packet)) { }

            void Read() override;

            uint32 ProposedRoles = 0;
            uint32 TargetCfgRealmID = 0;
            std::string TargetName;
            std::string TargetRealm;
            ObjectGuid TargetGUID;
        };

        class PartyInvite final : public ServerPacket
        {
        public:
            PartyInvite() : ServerPacket(SMSG_PARTY_INVITE, 43) { }

            WorldPacket const* Write() override;

            bool IsXRealm = false;
            bool MustBeBNetFriend = false;
            bool CanAccept = false;
            uint32 InviterCfgRealmID = 0;
            uint32 LfgCompletedMask = 0;
            uint32 ProposedRoles = 0;
            uint32 Timestamp = 0;
            std::string InviterRealmName;
            std::string InviterName;
            std::vector<uint32> LfgSlots;
            ObjectGuid InviterGUID;
        };

        class PartyInviteResponse final : public ClientPacket
        {
        public:
            PartyInviteResponse(WorldPacket&& packet) : ClientPacket(CMSG_PARTY_INVITE_RESPONSE, std::move(packet)) { }

            void Read() override;

            bool Accept = false;
            Optional<uint32> RolesDesired;
        };

        class GroupDecline final : public ServerPacket
        {
        public:
            GroupDecline(std::string const& name) : ServerPacket(SMSG_GROUP_DECLINE, name.size() + 1), Name(name) { }

            WorldPacket const* Write() override;

            std::string Name;
        };

        struct PartyPlayerInfo
        {
            std::string Name;
            ObjectGuid Guid;
            uint8 Connected = 0;
            uint8 Subgroup = 0;
            uint8 Flags = 0;
            uint8 RolesAssigned = 0;
        };

        struct PartyLFGInfo
        {
            uint8 MyLfgFlags = 0;
            uint32 LfgSlot = 0;
            bool LfgAborted = false;
        };

        struct PartyLootSettings
        {
            uint8 LootMethod = 0;
            ObjectGuid LootMaster;
            uint8 LootThreshold = 0;
        };

        struct PartyDifficultySettings
        {
            uint8 DungeonDifficulty = 0;
            uint8 RaidDifficulty = 0;
        };

        // SMSG_PARTY_UPDATE (3.3.5a SMSG_GROUP_LIST): the 4.3.4 layout gained the LFG aborted flag and lost the
        // trailing dynamic raid difficulty byte.
        class PartyUpdate final : public ServerPacket
        {
        public:
            PartyUpdate() : ServerPacket(SMSG_PARTY_UPDATE, 50) { }

            WorldPacket const* Write() override;

            uint8 PartyFlags = 0;
            uint8 Subgroup = 0;
            uint8 Flags = 0;
            uint8 RolesAssigned = 0;
            Optional<PartyLFGInfo> LfgInfo;
            ObjectGuid PartyGUID;
            uint32 SequenceNum = 0;
            std::vector<PartyPlayerInfo> PlayerList;
            ObjectGuid LeaderGUID;
            PartyLootSettings LootSettings;
            PartyDifficultySettings DifficultySettings;
        };

        struct PartyMemberAuraStates
        {
            uint32 SpellID = 0;
            uint16 Flags = 0;
            std::vector<int32> Points;
        };

        struct PartyMemberPetStats
        {
            ObjectGuid GUID;
            std::string Name;
            int16 ModelId = 0;
            int32 CurrentHealth = 0;
            int32 MaxHealth = 0;
            uint8 PowerType = 0;
            uint16 CurrentPower = 0;
            uint16 MaxPower = 0;
            uint64 AuraMask = 0;
            std::vector<PartyMemberAuraStates> Auras;
        };

        struct PartyMemberStats
        {
            uint16 Status = 0;
            int32 CurrentHealth = 0;
            int32 MaxHealth = 0;
            uint8 PowerType = 0;
            uint16 CurrentPower = 0;
            uint16 MaxPower = 0;
            uint16 Level = 0;
            uint16 ZoneID = 0;
            int16 WmoGroupID = 0;
            int16 PositionX = 0;
            int16 PositionY = 0;
            int16 PositionZ = 0;
            uint64 AuraMask = 0;
            std::vector<PartyMemberAuraStates> Auras;
            Optional<PartyMemberPetStats> PetStats;
            int32 VehicleSeat = 0;
            uint32 PhaseShiftFlags = 0;
            std::vector<uint16> Phases;
        };

        // SMSG_PARTY_MEMBER_STATE (partial) and SMSG_PARTY_MEMBER_FULL_STATE (3.3.5a SMSG_PARTY_MEMBER_STATS and
        // SMSG_PARTY_MEMBER_STATS_FULL). The change mask uses the 4.3.4 GroupUpdateFlags bit positions.
        class PartyMemberState final : public ServerPacket
        {
        public:
            PartyMemberState(bool fullUpdate = false)
                : ServerPacket(fullUpdate ? SMSG_PARTY_MEMBER_FULL_STATE : SMSG_PARTY_MEMBER_STATE, 80) { }

            WorldPacket const* Write() override;
            void Initialize(Player* player);

            bool ForEnemy = false;
            ObjectGuid MemberGuid;
            uint32 ChangeMask = 0;
            PartyMemberStats MemberStats;
        };

        class SetEveryoneIsAssistant final : public ClientPacket
        {
        public:
            SetEveryoneIsAssistant(WorldPacket&& packet)
                : ClientPacket(CMSG_SET_EVERYONE_IS_ASSISTANT, std::move(packet)) { }

            void Read() override;

            bool EveryoneIsAssistant = false;
        };

        class SetRole final : public ClientPacket
        {
        public:
            SetRole(WorldPacket&& packet) : ClientPacket(CMSG_SET_ROLE, std::move(packet)) { }

            void Read() override;

            ObjectGuid ChangedUnit;
            uint32 Role = 0;
        };

        class RoleChangedInform final : public ServerPacket
        {
        public:
            RoleChangedInform() : ServerPacket(SMSG_ROLE_CHANGED_INFORM, 8 + 8 + 4 + 4) { }

            WorldPacket const* Write() override;

            ObjectGuid ChangedUnit;
            ObjectGuid From;
            uint32 NewRole = 0;
            uint32 OldRole = 0;
        };

        class RolePollBegin final : public ServerPacket
        {
        public:
            RolePollBegin() : ServerPacket(SMSG_ROLE_POLL_BEGIN, 8) { }

            WorldPacket const* Write() override;

            ObjectGuid From;
        };
    }
}

#endif // PartyPackets_h__
