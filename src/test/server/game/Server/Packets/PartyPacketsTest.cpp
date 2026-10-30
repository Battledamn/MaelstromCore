/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * You may redistribute it and/or modify it under the terms of the GNU General Public License
 * version 2 or, at your option, any later version.
 */

#include "PartyPackets.h"
#include "Group.h"
#include "SpellAuraDefines.h"
#include "Util.h"
#include "gtest/gtest.h"
#include <span>

namespace
{
    std::string Hex(WorldPacket const* packet)
    {
        return ByteArrayToHexStr(std::span<uint8 const>(packet->contents(), packet->size()));
    }

    WorldPacket ClientPacket(OpcodeClient opcode, std::string const& hex)
    {
        std::vector<uint8> bytes(hex.size() / 2);
        Acore::Impl::HexStrToByteArray(hex, bytes.data(), bytes.size());
        WorldPacket packet(opcode, bytes.size());
        if (!bytes.empty())
            packet.append(bytes.data(), bytes.size());
        return packet;
    }
}

// The 3.3.5a group opcodes are unknown to build 15595: /invite arrived as 0x0513, which the fork had mapped to
// CMSG_DROP_NEW_CONNECTION, so nothing happened. Values from the pinned CPP Opcodes.h.
TEST(PartyPacketsTest, UsesCataclysmPartyOpcodes)
{
    EXPECT_EQ(uint16(CMSG_PARTY_INVITE), 0x0513);
    EXPECT_EQ(uint16(CMSG_PARTY_INVITE_RESPONSE), 0x0410);
    EXPECT_EQ(uint16(CMSG_SET_EVERYONE_IS_ASSISTANT), 0x2530);
    EXPECT_EQ(uint16(CMSG_SET_ROLE), 0x25B1);
    EXPECT_EQ(uint16(CMSG_ROLE_POLL_BEGIN), 0x0430);
    EXPECT_EQ(uint16(CMSG_GROUP_REQUEST_JOIN_UPDATES), 0x2583);
    EXPECT_EQ(uint16(CMSG_CLEAR_RAID_MARKER), 0x7300);
    EXPECT_EQ(uint16(CMSG_DROP_NEW_CONNECTION), 0x0000);
    EXPECT_EQ(uint16(SMSG_PARTY_INVITE), 0x31B2);
    EXPECT_EQ(uint16(SMSG_PARTY_UPDATE), 0x4C24);
    EXPECT_EQ(uint16(SMSG_PARTY_MEMBER_STATE), 0x2104);
    EXPECT_EQ(uint16(SMSG_PARTY_MEMBER_FULL_STATE), 0x0215);
    EXPECT_EQ(uint16(SMSG_PARTY_COMMAND_RESULT), 0x6E07);
    EXPECT_EQ(uint16(SMSG_ROLE_CHANGED_INFORM), 0x39A6);
    EXPECT_EQ(uint16(SMSG_ROLE_POLL_BEGIN), 0x70B0);

    // The 4.x member stats inserted WMO_GROUP_ID at 0x100, shifting every later flag.
    EXPECT_EQ(uint32(GROUP_UPDATE_FLAG_POSITION), 0x200u);
    EXPECT_EQ(uint32(GROUP_UPDATE_FLAG_AURAS), 0x400u);
    EXPECT_EQ(uint32(GROUP_UPDATE_FLAG_PET_AURAS), 0x80000u);
    EXPECT_EQ(uint32(GROUP_UPDATE_FLAG_VEHICLE_SEAT), 0x100000u);
    EXPECT_EQ(uint32(GROUP_UPDATE_FLAG_PHASE), 0x200000u);
    EXPECT_EQ(uint32(GROUP_UPDATE_PET) & uint32(GROUP_UPDATE_FULL), 0u);
}

TEST(PartyPacketsTest, ReadsBitPackedInviteByName)
{
    // realm id 1, roles 8, empty guid and realm, a 7-character name
    WorldPacket packet = ClientPacket(CMSG_PARTY_INVITE, "010000000800000000001C005361656C736961");
    WorldPackets::Party::PartyInviteClient invite(std::move(packet));
    invite.Read();
    EXPECT_EQ(invite.TargetCfgRealmID, 1u);
    EXPECT_EQ(invite.ProposedRoles, 8u);
    EXPECT_EQ(invite.TargetName, "Saelsia");
    EXPECT_EQ(invite.TargetRealm, "");
    EXPECT_TRUE(invite.TargetGUID.IsEmpty());

    // "/invite Elybrook" as captured from the real build 15595 client (server/logs/packets.pkt, 2026-10-30)
    WorldPackets::Party::PartyInviteClient captured(ClientPacket(CMSG_PARTY_INVITE,
        "00000000000000000000200045" "6C7962726F6F6B"));
    captured.Read();
    EXPECT_EQ(captured.TargetName, "Elybrook");
    EXPECT_EQ(captured.ProposedRoles, 0u);
}

TEST(PartyPacketsTest, ReadsInviteResponseAcceptBitAndOptionalRoles)
{
    WorldPackets::Party::PartyInviteResponse accept(ClientPacket(CMSG_PARTY_INVITE_RESPONSE, "C002000000"));
    accept.Read();
    EXPECT_TRUE(accept.Accept);
    ASSERT_TRUE(accept.RolesDesired.has_value());
    EXPECT_EQ(*accept.RolesDesired, 2u);

    WorldPackets::Party::PartyInviteResponse decline(ClientPacket(CMSG_PARTY_INVITE_RESPONSE, "00"));
    decline.Read();
    EXPECT_FALSE(decline.Accept);
    EXPECT_FALSE(decline.RolesDesired.has_value());
}

TEST(PartyPacketsTest, WritesBitPackedInvite)
{
    WorldPackets::Party::PartyInvite invite;
    invite.CanAccept = true;
    invite.InviterGUID = ObjectGuid(uint64(0x1F));
    invite.InviterName = "Ely";
    invite.InviterRealmName = "Mael";
    invite.InviterCfgRealmID = 1;
    invite.Timestamp = 0x11223344;
    invite.ProposedRoles = 2;

    EXPECT_EQ(Hex(invite.Write()),
        "48040300000000"                  // bits: can accept, guid byte 0, realm length 4, name length 3, no LFG slots
        "44332211" "02000000" "00000000"  // timestamp, proposed roles, LFG completed mask
        "1E"                              // guid byte 0 (xor 1)
        "4D61656C" "456C79"               // "Mael", "Ely" (no terminators)
        "01000000");                      // realm id
}

TEST(PartyPacketsTest, CommandResultEndsWithGuid)
{
    WorldPackets::Party::PartyCommandResult result;
    result.Command = 0;
    result.Name = "Ely";
    result.Result = 2;
    EXPECT_EQ(Hex(result.Write()), "00000000" "456C7900" "02000000" "00000000" "0000000000000000");
}

TEST(PartyPacketsTest, WritesPartyUpdateWithoutDynamicDifficultyByte)
{
    WorldPackets::Party::PartyUpdate update;
    update.Flags = MEMBER_FLAG_ASSISTANT;
    update.PartyGUID = ObjectGuid(uint64(0x0500000000000007));
    update.SequenceNum = 3;
    WorldPackets::Party::PartyPlayerInfo& member = update.PlayerList.emplace_back();
    member.Name = "Saelsia";
    member.Guid = ObjectGuid(uint64(0x12));
    member.Connected = MEMBER_STATUS_ONLINE;
    update.LeaderGUID = ObjectGuid(uint64(0x1F));
    update.LootSettings.LootMethod = GROUP_LOOT;
    update.LootSettings.LootThreshold = 2;

    EXPECT_EQ(Hex(update.Write()),
        "00000100"                                   // party flags, subgroup, member flags, roles
        "0700000000000005" "03000000" "01000000"     // party guid, sequence, member count
        "5361656C73696100" "1200000000000000" "01000000" // member name, guid, online, subgroup, flags, roles
        "1F00000000000000"                           // leader
        "03" "0000000000000000" "02" "00" "00");     // loot method, master, threshold, difficulties

    WorldPackets::Party::PartyUpdate destroyed;
    destroyed.PartyFlags = GROUPTYPE_DESTROYED;
    destroyed.PartyGUID = ObjectGuid(uint64(0x0500000000000007));
    destroyed.SequenceNum = 4;
    EXPECT_EQ(Hex(destroyed.Write()), "10000000" "0700000000000005" "04000000" "00000000" "0000000000000000");
}

TEST(PartyPacketsTest, WritesPartialMemberStateInCataFieldOrder)
{
    WorldPackets::Party::PartyMemberState state;
    state.MemberGuid = ObjectGuid(uint64(0x12));
    state.ChangeMask = GROUP_UPDATE_FLAG_STATUS | GROUP_UPDATE_FLAG_CUR_HP | GROUP_UPDATE_FLAG_MAX_HP |
        GROUP_UPDATE_FLAG_POWER_TYPE | GROUP_UPDATE_FLAG_CUR_POWER | GROUP_UPDATE_FLAG_MAX_POWER |
        GROUP_UPDATE_FLAG_POSITION;
    state.MemberStats.Status = MEMBER_STATUS_ONLINE;
    state.MemberStats.CurrentHealth = 100;
    state.MemberStats.MaxHealth = 200;
    state.MemberStats.PowerType = 1;
    state.MemberStats.CurrentPower = 10;
    state.MemberStats.MaxPower = 100;
    state.MemberStats.PositionX = 10;
    state.MemberStats.PositionY = -20;
    state.MemberStats.PositionZ = 30;

    WorldPacket const* packet = state.Write();
    EXPECT_EQ(packet->GetOpcode(), uint16(SMSG_PARTY_MEMBER_STATE));
    EXPECT_EQ(Hex(packet),
        "0112" "3F020000"                 // packed guid, change mask
        "0100" "64000000" "C8000000"      // status, health, max health
        "01" "0A00" "6400"                // power type, power, max power
        "0A00" "ECFF" "1E00");            // position x, y, z
}

TEST(PartyPacketsTest, WritesFullMemberStateAurasPetAurasAndPhase)
{
    WorldPackets::Party::PartyMemberState state(true);
    state.MemberGuid = ObjectGuid(uint64(0x12));
    state.ChangeMask = GROUP_UPDATE_FLAG_AURAS | GROUP_UPDATE_FLAG_PET_GUID | GROUP_UPDATE_FLAG_PET_AURAS |
        GROUP_UPDATE_FLAG_PHASE;
    state.MemberStats.AuraMask = (uint64(1) << 2) | (uint64(1) << 5);
    WorldPackets::Party::PartyMemberAuraStates& plain = state.MemberStats.Auras.emplace_back();
    plain.SpellID = 1234;
    plain.Flags = AFLAG_EFF_INDEX_0 | AFLAG_POSITIVE;
    WorldPackets::Party::PartyMemberAuraStates& scalable = state.MemberStats.Auras.emplace_back();
    scalable.SpellID = 5;
    scalable.Flags = AFLAG_EFF_INDEX_0 | AFLAG_ANY_EFFECT_AMOUNT_SENT;
    scalable.Points = { 7, 0, 0 };
    state.MemberStats.PhaseShiftFlags = 0x08;

    WorldPacket const* packet = state.Write();
    EXPECT_EQ(packet->GetOpcode(), uint16(SMSG_PARTY_MEMBER_FULL_STATE));
    EXPECT_EQ(Hex(packet),
        "00" "0112" "000C2800"                                    // for enemy, packed guid, change mask
        "01" "2400000000000000" "40000000"                        // full, aura mask, MAX_AURAS_GROUP_UPDATE
        "D2040000" "1100" "05000000" "4100" "07000000" "00000000" "00000000"
        "0000000000000000"                                        // no pet: empty pet guid
        "01" "0000000000000000" "00000000"                        // no pet auras
        "08000000" "00000000");                                   // phase shift flags, no phases
}

TEST(PartyPacketsTest, WritesBitPackedRolePackets)
{
    WorldPackets::Party::SetRole setRole(ClientPacket(CMSG_SET_ROLE, "02000000" "02" "13"));
    setRole.Read();
    EXPECT_EQ(setRole.Role, 2u);
    EXPECT_EQ(setRole.ChangedUnit, ObjectGuid(uint64(0x12)));

    WorldPackets::Party::RolePollBegin poll;
    poll.From = ObjectGuid(uint64(0x1F));
    EXPECT_EQ(Hex(poll.Write()), "02" "1E");

    WorldPackets::Party::RoleChangedInform inform;
    inform.From = ObjectGuid(uint64(0x1F));
    inform.ChangedUnit = ObjectGuid(uint64(0x12));
    inform.OldRole = 0;
    inform.NewRole = 2;
    EXPECT_EQ(Hex(inform.Write()), "4001" "13" "02000000" "1E" "00000000");
}
