/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "GuildPackets.h"
#include "gtest/gtest.h"

namespace
{
    void ExpectBytes(WorldPacket const* written, std::initializer_list<uint8> expected, std::size_t skipFrom = 0,
        std::size_t skipCount = 0)
    {
        ASSERT_EQ(written->size(), expected.size());
        std::size_t index = 0;
        for (uint8 byte : expected)
        {
            if (index < skipFrom || index >= skipFrom + skipCount)
                EXPECT_EQ(written->contents()[index], byte) << index;
            ++index;
        }
    }
}

TEST(GuildPacketsTest, UsesCataclysmOpcodesAndEventIds)
{
    // The 3.3.5 event ids (MOTD = 2, which is GE_DEMOTION in 4.3.4) crashed the client on .guild create
    EXPECT_EQ(int(GE_PROMOTION), 1);
    EXPECT_EQ(int(GE_MOTD), 3);
    EXPECT_EQ(int(GE_JOINED), 4);
    EXPECT_EQ(int(GE_FOUNDER), 15);
    EXPECT_EQ(int(GE_SIGNED_ON), 16);

    EXPECT_EQ(uint16(WorldPackets::Guild::GuildCommandResult().GetOpcode()), 0x2707);
    EXPECT_EQ(uint16(SMSG_GUILD_QUERY_RESPONSE), 0x0E06);
    EXPECT_EQ(uint16(CMSG_GUILD_ROSTER), 0x1226);
    EXPECT_EQ(uint16(SMSG_GUILD_RANKS), 0x30B4);
}

TEST(GuildPacketsTest, EventWritesParamsAndTrailingGuid)
{
    WorldPackets::Guild::GuildEvent packet;
    packet.Type = GE_SIGNED_ON;
    packet.Params.emplace_back("Ely");
    packet.Guid = ObjectGuid::Create<HighGuid::Player>(0x1F);

    ExpectBytes(packet.Write(), {
        0x10, 0x01,                             // GE_SIGNED_ON, one parameter
        0x45, 0x6C, 0x79, 0x00,                 // "Ely"
        0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    });
}

TEST(GuildPacketsTest, EventWithoutPlayerStillEndsWithGuid)
{
    WorldPackets::Guild::GuildEvent packet;
    packet.Type = GE_MOTD;
    packet.Params.emplace_back("Hi");

    ExpectBytes(packet.Write(), {
        0x03, 0x01, 0x48, 0x69, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    });
}

TEST(GuildPacketsTest, RosterWritesBitPackedMember)
{
    WorldPackets::Guild::GuildRoster packet;
    packet.WelcomeText = "Hi";
    packet.NumAccounts = 1;

    WorldPackets::Guild::GuildRosterMemberData& member = packet.MemberData.emplace_back();
    member.Guid = ObjectGuid::Create<HighGuid::Player>(0x1F);
    member.Name = "Ely";
    member.AreaID = 1657;
    member.Level = 85;
    member.ClassID = 1;
    member.Gender = 1;
    member.Status = GUILDMEMBER_STATUS_ONLINE;

    // Bit block: motd length (11), member count (18), the member's guid bits, flags and string lengths,
    // info length (12), padded to 10 bytes. The packed creation date depends on the time zone and is skipped.
    ExpectBytes(packet.Write(), {
        0x00, 0x40, 0x00, 0x08, 0x00, 0x00, 0x41, 0x80, 0x00, 0x00,
        0x01,                                   // class
        0x00, 0x00, 0x00, 0x00,                 // guild reputation
        0x1E,                                   // guid byte 0 (0x1F ^ 1)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // weekly xp
        0x00, 0x00, 0x00, 0x00,                 // rank
        0x00, 0x00, 0x00, 0x00,                 // achievement points
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // profession 1
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // profession 2
        0x01,                                   // status
        0x79, 0x06, 0x00, 0x00,                 // area 1657
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // total xp
        0x00, 0x00, 0x00, 0x00,                 // reputation to cap
        0x55,                                   // level 85
        0x00, 0x00, 0x00, 0x00,                 // virtual realm address
        0x01,                                   // gender
        0x00, 0x00, 0x00, 0x00,                 // last save
        0x45, 0x6C, 0x79,                       // name, no terminator
        0x48, 0x69,                             // motd
        0x01, 0x00, 0x00, 0x00,                 // accounts
        0x00, 0x00, 0x00, 0x00,                 // weekly reputation cap
        0x00, 0x00, 0x00, 0x00,                 // creation date (skipped)
        0x00, 0x00, 0x00, 0x00                  // guild flags
    }, 96, 4);
}

TEST(GuildPacketsTest, ReadsSetNoteWithGuidAndPublicBit)
{
    WorldPacket raw(CMSG_GUILD_SET_NOTE, 6);
    // Bits: guid 1, 4, 5, 3, 0 (set), 7, public (set), guid 6, note length 2 (8 bits), guid 2
    uint8 const payload[] = { 0x0A, 0x02, 0x00, 0x1E, 0x61, 0x62 };
    raw.append(payload, sizeof(payload));

    WorldPackets::Guild::GuildSetMemberNote packet(std::move(raw));
    packet.Read();

    EXPECT_EQ(packet.NoteeGUID, ObjectGuid::Create<HighGuid::Player>(0x1F));
    EXPECT_TRUE(packet.IsPublic);
    EXPECT_EQ(packet.Note, "ab");
    EXPECT_EQ(packet.GetRawPacket()->rpos(), packet.GetRawPacket()->size());
}

TEST(GuildPacketsTest, RanksWriteNameLengthsThenRankData)
{
    WorldPackets::Guild::GuildRanks packet;
    WorldPackets::Guild::GuildRankData& rank = packet.Ranks.emplace_back();
    rank.RankID = 0;
    rank.RankOrder = 0;
    rank.Flags = GR_RIGHT_ALL;
    rank.WithdrawGoldLimit = 0xFFFFFFFF;
    rank.RankName = "GM";

    WorldPacket const* written = packet.Write();
    // 18-bit count (1) and 7-bit name length (2) fit in four bytes after padding
    ASSERT_EQ(written->size(), 4 + 4 + GUILD_BANK_PACKET_TABS * 8 + 4 + 4 + 2 + 4);
    EXPECT_EQ(written->contents()[0], 0x00);
    EXPECT_EQ(written->contents()[1], 0x00);
    EXPECT_EQ(written->contents()[2], 0x41);
    EXPECT_EQ(written->contents()[3], 0x00);
    std::size_t const namePos = 4 + 4 + GUILD_BANK_PACKET_TABS * 8 + 4 + 4;
    EXPECT_EQ(written->contents()[namePos], 'G');
    EXPECT_EQ(written->contents()[namePos + 1], 'M');
}
