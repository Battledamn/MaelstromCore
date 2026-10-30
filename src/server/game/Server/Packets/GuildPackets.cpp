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

#include "GuildPackets.h"

namespace
{
    // 4.3.4 sends guids as a bit mask plus the non-zero bytes xor 1, each in a packet specific order.
    void ReadGuidMask(ByteBuffer& data, ObjectGuid& guid, std::initializer_list<uint8> order)
    {
        for (uint8 index : order)
            guid[index] = data.ReadBit();
    }

    void ReadGuidBytes(ByteBuffer& data, ObjectGuid& guid, std::initializer_list<uint8> order)
    {
        for (uint8 index : order)
            data.ReadByteSeq(guid[index]);
    }

    void WriteGuidBytes(ByteBuffer& data, ObjectGuid const& guid, std::initializer_list<uint8> order)
    {
        for (uint8 index : order)
            data.WriteByteSeq(guid[index]);
    }
}

void WorldPackets::Guild::QueryGuildInfo::Read()
{
    _worldPacket >> GuildGuid;
    _worldPacket >> PlayerGuid;
}

WorldPacket const* WorldPackets::Guild::QueryGuildInfoResponse::Write()
{
    _worldPacket << GuildGuid;
    _worldPacket << GuildName;

    for (GuildInfoRank const& rank : Ranks)
        _worldPacket << rank.RankName;

    for (GuildInfoRank const& rank : Ranks)
        _worldPacket << uint32(rank.RankOrder);

    for (GuildInfoRank const& rank : Ranks)
        _worldPacket << uint32(rank.RankID);

    _worldPacket << uint32(EmblemStyle);
    _worldPacket << uint32(EmblemColor);
    _worldPacket << uint32(BorderStyle);
    _worldPacket << uint32(BorderColor);
    _worldPacket << uint32(BackgroundColor);
    _worldPacket << uint32(RankCount);

    return &_worldPacket;
}

void WorldPackets::Guild::GuildCreate::Read()
{
    _worldPacket >> GuildName;
}

WorldPacket const* WorldPackets::Guild::GuildInfoResponse::Write()
{
    _worldPacket << GuildName;
    _worldPacket.AppendPackedTime(CreateDate);
    _worldPacket << int32(NumMembers);
    _worldPacket << int32(NumAccounts);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Guild::GuildRoster::Write()
{
    _worldPacket.WriteBits(WelcomeText.length(), 11);
    _worldPacket.WriteBits(MemberData.size(), 18);

    for (GuildRosterMemberData const& member : MemberData)
    {
        _worldPacket.WriteBit(member.Guid[3]);
        _worldPacket.WriteBit(member.Guid[4]);
        _worldPacket.WriteBit(member.Authenticated);
        _worldPacket.WriteBit(member.SorEligible);
        _worldPacket.WriteBits(member.Note.length(), 8);
        _worldPacket.WriteBits(member.OfficerNote.length(), 8);
        _worldPacket.WriteBit(member.Guid[0]);
        _worldPacket.WriteBits(member.Name.length(), 7);
        _worldPacket.WriteBit(member.Guid[1]);
        _worldPacket.WriteBit(member.Guid[2]);
        _worldPacket.WriteBit(member.Guid[6]);
        _worldPacket.WriteBit(member.Guid[5]);
        _worldPacket.WriteBit(member.Guid[7]);
    }

    _worldPacket.WriteBits(InfoText.length(), 12);
    _worldPacket.FlushBits();

    for (GuildRosterMemberData const& member : MemberData)
    {
        _worldPacket << uint8(member.ClassID);
        _worldPacket << int32(member.GuildReputation);
        _worldPacket.WriteByteSeq(member.Guid[0]);
        _worldPacket << int64(member.WeeklyXP);
        _worldPacket << int32(member.RankID);
        _worldPacket << int32(member.PersonalAchievementPoints);

        for (GuildRosterProfessionData const& profession : member.Profession)
        {
            _worldPacket << int32(profession.Step);
            _worldPacket << int32(profession.Rank);
            _worldPacket << int32(profession.DbID);
        }

        _worldPacket.WriteByteSeq(member.Guid[2]);
        _worldPacket << uint8(member.Status);
        _worldPacket << int32(member.AreaID);
        _worldPacket << int64(member.TotalXP);
        _worldPacket.WriteByteSeq(member.Guid[7]);
        _worldPacket << int32(member.GuildRepToCap);
        _worldPacket.WriteString(member.Note);
        _worldPacket.WriteByteSeq(member.Guid[3]);
        _worldPacket << uint8(member.Level);
        _worldPacket << uint32(member.VirtualRealmAddress);
        _worldPacket.WriteByteSeq(member.Guid[5]);
        _worldPacket.WriteByteSeq(member.Guid[4]);
        _worldPacket << uint8(member.Gender);
        _worldPacket.WriteByteSeq(member.Guid[1]);
        _worldPacket << float(member.LastSave);
        _worldPacket.WriteString(member.OfficerNote);
        _worldPacket.WriteByteSeq(member.Guid[6]);
        _worldPacket.WriteString(member.Name);
    }

    _worldPacket.WriteString(InfoText);
    _worldPacket.WriteString(WelcomeText);
    _worldPacket << int32(NumAccounts);
    _worldPacket << int32(WeeklyRepCap);
    _worldPacket.AppendPackedTime(CreateDate);
    _worldPacket << int32(GuildFlags);

    return &_worldPacket;
}

void WorldPackets::Guild::GuildUpdateMotdText::Read()
{
    uint32 length = _worldPacket.ReadBits(11);
    MotdText = _worldPacket.ReadString(length);
}

WorldPacket const* WorldPackets::Guild::GuildCommandResult::Write()
{
    _worldPacket << int32(Command);
    _worldPacket << Name;
    _worldPacket << int32(Result);

    return &_worldPacket;
}

void WorldPackets::Guild::GuildInviteByName::Read()
{
    uint32 length = _worldPacket.ReadBits(7);
    Name = _worldPacket.ReadString(length);
}

WorldPacket const* WorldPackets::Guild::GuildInvite::Write()
{
    _worldPacket << uint32(GuildLevel);
    _worldPacket << uint32(BorderStyle);
    _worldPacket << uint32(BorderColor);
    _worldPacket << uint32(EmblemStyle);
    _worldPacket << uint32(BackgroundColor);
    _worldPacket << uint32(EmblemColor);

    _worldPacket.WriteBit(GuildGuid[3]);
    _worldPacket.WriteBit(GuildGuid[2]);
    _worldPacket.WriteBits(OldGuildName.length(), 8);
    _worldPacket.WriteBit(GuildGuid[1]);
    _worldPacket.WriteBit(OldGuildGuid[6]);
    _worldPacket.WriteBit(OldGuildGuid[4]);
    _worldPacket.WriteBit(OldGuildGuid[1]);
    _worldPacket.WriteBit(OldGuildGuid[5]);
    _worldPacket.WriteBit(OldGuildGuid[7]);
    _worldPacket.WriteBit(OldGuildGuid[2]);
    _worldPacket.WriteBit(GuildGuid[7]);
    _worldPacket.WriteBit(GuildGuid[0]);
    _worldPacket.WriteBit(GuildGuid[6]);
    _worldPacket.WriteBits(GuildName.length(), 8);
    _worldPacket.WriteBit(OldGuildGuid[3]);
    _worldPacket.WriteBit(OldGuildGuid[0]);
    _worldPacket.WriteBit(GuildGuid[5]);
    _worldPacket.WriteBits(InviterName.length(), 7);
    _worldPacket.WriteBit(GuildGuid[4]);
    _worldPacket.FlushBits();

    _worldPacket.WriteByteSeq(GuildGuid[1]);
    _worldPacket.WriteByteSeq(OldGuildGuid[3]);
    _worldPacket.WriteByteSeq(GuildGuid[6]);
    _worldPacket.WriteByteSeq(OldGuildGuid[2]);
    _worldPacket.WriteByteSeq(OldGuildGuid[1]);
    _worldPacket.WriteByteSeq(GuildGuid[0]);
    _worldPacket.WriteString(OldGuildName);
    _worldPacket.WriteByteSeq(GuildGuid[7]);
    _worldPacket.WriteByteSeq(GuildGuid[2]);
    _worldPacket.WriteString(InviterName);
    _worldPacket.WriteByteSeq(OldGuildGuid[7]);
    _worldPacket.WriteByteSeq(OldGuildGuid[6]);
    _worldPacket.WriteByteSeq(OldGuildGuid[5]);
    _worldPacket.WriteByteSeq(OldGuildGuid[0]);
    _worldPacket.WriteByteSeq(GuildGuid[4]);
    _worldPacket.WriteString(GuildName);
    _worldPacket.WriteByteSeq(GuildGuid[5]);
    _worldPacket.WriteByteSeq(GuildGuid[3]);
    _worldPacket.WriteByteSeq(OldGuildGuid[4]);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Guild::GuildEvent::Write()
{
    _worldPacket << uint8(Type);
    _worldPacket << uint8(Params.size());
    for (std::string_view param : Params)
        _worldPacket << param;

    // 4.3.4 always ends the event with a guid (empty for events about no particular player)
    _worldPacket << Guid;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Guild::GuildEventLogQueryResults::Write()
{
    _worldPacket.WriteBits(Entry.size(), 23);

    for (GuildEventEntry const& entry : Entry)
    {
        _worldPacket.WriteBit(entry.PlayerGUID[2]);
        _worldPacket.WriteBit(entry.PlayerGUID[4]);
        _worldPacket.WriteBit(entry.OtherGUID[7]);
        _worldPacket.WriteBit(entry.OtherGUID[6]);
        _worldPacket.WriteBit(entry.PlayerGUID[3]);
        _worldPacket.WriteBit(entry.OtherGUID[3]);
        _worldPacket.WriteBit(entry.OtherGUID[5]);
        _worldPacket.WriteBit(entry.PlayerGUID[7]);
        _worldPacket.WriteBit(entry.PlayerGUID[5]);
        _worldPacket.WriteBit(entry.PlayerGUID[0]);
        _worldPacket.WriteBit(entry.OtherGUID[4]);
        _worldPacket.WriteBit(entry.OtherGUID[2]);
        _worldPacket.WriteBit(entry.OtherGUID[0]);
        _worldPacket.WriteBit(entry.OtherGUID[1]);
        _worldPacket.WriteBit(entry.PlayerGUID[1]);
        _worldPacket.WriteBit(entry.PlayerGUID[6]);
    }

    _worldPacket.FlushBits();

    for (GuildEventEntry const& entry : Entry)
    {
        WriteGuidBytes(_worldPacket, entry.OtherGUID, { 3, 2, 5 });
        _worldPacket << uint8(entry.RankID);
        WriteGuidBytes(_worldPacket, entry.OtherGUID, { 4 });
        WriteGuidBytes(_worldPacket, entry.PlayerGUID, { 0, 4 });
        _worldPacket << uint32(entry.TransactionDate);
        WriteGuidBytes(_worldPacket, entry.PlayerGUID, { 7, 3 });
        WriteGuidBytes(_worldPacket, entry.OtherGUID, { 0, 6, 7 });
        WriteGuidBytes(_worldPacket, entry.PlayerGUID, { 5 });
        _worldPacket << uint8(entry.TransactionType);
        WriteGuidBytes(_worldPacket, entry.OtherGUID, { 1 });
        WriteGuidBytes(_worldPacket, entry.PlayerGUID, { 2, 6, 1 });
    }

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Guild::GuildPermissionsQueryResults::Write()
{
    _worldPacket << uint32(RankID);
    _worldPacket << int32(NumTabs);
    _worldPacket << int32(Flags);
    _worldPacket << int32(WithdrawGoldLimit);
    _worldPacket.WriteBits(Tab.size(), 23);
    _worldPacket.FlushBits();

    for (GuildRankTabPermissions const& tab : Tab)
    {
        _worldPacket << int32(tab.Flags);
        _worldPacket << int32(tab.WithdrawItemLimit);
    }

    return &_worldPacket;
}

void WorldPackets::Guild::GuildSetRankPermissions::Read()
{
    _worldPacket >> OldRankID;
    _worldPacket >> OldFlags;
    _worldPacket >> Flags;

    for (uint8 i = 0; i < GUILD_BANK_PACKET_TABS; ++i)
    {
        _worldPacket >> TabFlags[i];
        _worldPacket >> TabWithdrawItemLimit[i];
    }

    _worldPacket >> WithdrawGoldLimit;
    _worldPacket >> RankID;
    uint32 length = _worldPacket.ReadBits(7);
    RankName = _worldPacket.ReadString(length);
}

void WorldPackets::Guild::GuildAddRank::Read()
{
    _worldPacket >> RankID;
    uint32 length = _worldPacket.ReadBits(7);
    Name = _worldPacket.ReadString(length);
}

void WorldPackets::Guild::GuildDeleteRank::Read()
{
    _worldPacket >> RankID;
}

void WorldPackets::Guild::GuildUpdateInfoText::Read()
{
    uint32 length = _worldPacket.ReadBits(12);
    InfoText = _worldPacket.ReadString(length);
}

void WorldPackets::Guild::GuildSetMemberNote::Read()
{
    ReadGuidMask(_worldPacket, NoteeGUID, { 1, 4, 5, 3, 0, 7 });
    IsPublic = _worldPacket.ReadBit();
    NoteeGUID[6] = _worldPacket.ReadBit();
    uint32 length = _worldPacket.ReadBits(8);
    NoteeGUID[2] = _worldPacket.ReadBit();

    ReadGuidBytes(_worldPacket, NoteeGUID, { 4, 5, 0, 3, 1, 6, 7 });
    Note = _worldPacket.ReadString(length);
    ReadGuidBytes(_worldPacket, NoteeGUID, { 2 });
}

WorldPacket const* WorldPackets::Guild::GuildMemberUpdateNote::Write()
{
    _worldPacket.WriteBit(Member[7]);
    _worldPacket.WriteBit(Member[2]);
    _worldPacket.WriteBit(Member[3]);
    _worldPacket.WriteBits(Note.length(), 8);
    _worldPacket.WriteBit(Member[5]);
    _worldPacket.WriteBit(Member[0]);
    _worldPacket.WriteBit(Member[6]);
    _worldPacket.WriteBit(Member[4]);
    _worldPacket.WriteBit(IsPublic);
    _worldPacket.WriteBit(Member[1]);
    _worldPacket.FlushBits();

    WriteGuidBytes(_worldPacket, Member, { 3, 0, 2, 5 });
    _worldPacket.WriteString(Note);
    WriteGuidBytes(_worldPacket, Member, { 7, 6, 1, 4 });

    return &_worldPacket;
}

void WorldPackets::Guild::GuildDemoteMember::Read()
{
    ReadGuidMask(_worldPacket, Demotee, { 7, 1, 5, 6, 2, 3, 0, 4 });
    ReadGuidBytes(_worldPacket, Demotee, { 1, 2, 7, 5, 6, 0, 4, 3 });
}

void WorldPackets::Guild::GuildPromoteMember::Read()
{
    ReadGuidMask(_worldPacket, Promotee, { 7, 2, 5, 6, 1, 0, 3, 4 });
    ReadGuidBytes(_worldPacket, Promotee, { 0, 5, 2, 3, 6, 4, 1, 7 });
}

void WorldPackets::Guild::GuildOfficerRemoveMember::Read()
{
    ReadGuidMask(_worldPacket, Removee, { 6, 5, 4, 0, 1, 3, 7, 2 });
    ReadGuidBytes(_worldPacket, Removee, { 2, 6, 5, 7, 1, 4, 3, 0 });
}

void WorldPackets::Guild::GuildAssignMemberRank::Read()
{
    _worldPacket >> RankID;

    Member[1] = _worldPacket.ReadBit();
    Member[7] = _worldPacket.ReadBit();
    Setter[4] = _worldPacket.ReadBit();
    Setter[2] = _worldPacket.ReadBit();
    Member[4] = _worldPacket.ReadBit();
    Member[5] = _worldPacket.ReadBit();
    Member[6] = _worldPacket.ReadBit();
    Setter[1] = _worldPacket.ReadBit();
    Setter[7] = _worldPacket.ReadBit();
    Member[2] = _worldPacket.ReadBit();
    Member[3] = _worldPacket.ReadBit();
    Member[0] = _worldPacket.ReadBit();
    Setter[6] = _worldPacket.ReadBit();
    Setter[3] = _worldPacket.ReadBit();
    Setter[0] = _worldPacket.ReadBit();
    Setter[5] = _worldPacket.ReadBit();

    ReadGuidBytes(_worldPacket, Member, { 0 });
    ReadGuidBytes(_worldPacket, Setter, { 1, 3, 5 });
    ReadGuidBytes(_worldPacket, Member, { 7, 3 });
    ReadGuidBytes(_worldPacket, Setter, { 0 });
    ReadGuidBytes(_worldPacket, Member, { 1 });
    ReadGuidBytes(_worldPacket, Setter, { 6 });
    ReadGuidBytes(_worldPacket, Member, { 2, 5, 4 });
    ReadGuidBytes(_worldPacket, Setter, { 2, 4 });
    ReadGuidBytes(_worldPacket, Member, { 6 });
    ReadGuidBytes(_worldPacket, Setter, { 7 });
}

WorldPacket const* WorldPackets::Guild::GuildRanksUpdate::Write()
{
    _worldPacket.WriteBit(Officer[7]);
    _worldPacket.WriteBit(Officer[2]);
    _worldPacket.WriteBit(Other[2]);
    _worldPacket.WriteBit(Officer[1]);
    _worldPacket.WriteBit(Other[1]);
    _worldPacket.WriteBit(Other[7]);
    _worldPacket.WriteBit(Other[0]);
    _worldPacket.WriteBit(Other[5]);
    _worldPacket.WriteBit(Other[4]);
    _worldPacket.WriteBit(Promote);
    _worldPacket.WriteBit(Officer[5]);
    _worldPacket.WriteBit(Officer[0]);
    _worldPacket.WriteBit(Other[6]);
    _worldPacket.WriteBit(Officer[3]);
    _worldPacket.WriteBit(Officer[6]);
    _worldPacket.WriteBit(Other[3]);
    _worldPacket.WriteBit(Officer[4]);
    _worldPacket.FlushBits();

    _worldPacket << uint32(RankID);
    WriteGuidBytes(_worldPacket, Officer, { 3 });
    WriteGuidBytes(_worldPacket, Other, { 7 });
    WriteGuidBytes(_worldPacket, Officer, { 6, 2 });
    WriteGuidBytes(_worldPacket, Other, { 5, 0 });
    WriteGuidBytes(_worldPacket, Officer, { 7, 5 });
    WriteGuidBytes(_worldPacket, Other, { 2, 1 });
    WriteGuidBytes(_worldPacket, Officer, { 0, 4, 1 });
    WriteGuidBytes(_worldPacket, Other, { 3, 6, 4 });

    return &_worldPacket;
}

void WorldPackets::Guild::GuildQueryRanks::Read()
{
    ReadGuidMask(_worldPacket, GuildGuid, { 2, 3, 0, 6, 4, 7, 5, 1 });
    ReadGuidBytes(_worldPacket, GuildGuid, { 3, 4, 5, 7, 1, 0, 6, 2 });
}

WorldPacket const* WorldPackets::Guild::GuildRanks::Write()
{
    _worldPacket.WriteBits(Ranks.size(), 18);

    for (GuildRankData const& rank : Ranks)
        _worldPacket.WriteBits(rank.RankName.length(), 7);

    _worldPacket.FlushBits();

    for (GuildRankData const& rank : Ranks)
    {
        _worldPacket << int32(rank.RankID);

        for (uint8 i = 0; i < GUILD_BANK_PACKET_TABS; ++i)
        {
            _worldPacket << int32(rank.TabWithdrawItemLimit[i]);
            _worldPacket << int32(rank.TabFlags[i]);
        }

        _worldPacket << int32(rank.WithdrawGoldLimit);
        _worldPacket << int32(rank.Flags);
        _worldPacket.WriteString(rank.RankName);
        _worldPacket << int32(rank.RankOrder);
    }

    return &_worldPacket;
}

void WorldPackets::Guild::GuildQueryNews::Read()
{
    _worldPacket.read_skip<uint32>();
}

WorldPacket const* WorldPackets::Guild::GuildNews::Write()
{
    _worldPacket.WriteBits(0, 21);                         // news count: no guild news yet
    _worldPacket.FlushBits();

    return &_worldPacket;
}

void WorldPackets::Guild::GuildXPQuery::Read()
{
    ReadGuidMask(_worldPacket, GuildGuid, { 2, 1, 0, 5, 4, 7, 6, 3 });
    ReadGuidBytes(_worldPacket, GuildGuid, { 7, 2, 3, 6, 1, 5, 0, 4 });
}

WorldPacket const* WorldPackets::Guild::GuildXP::Write()
{
    _worldPacket << int64(TotalXP);
    _worldPacket << int64(GuildRemainingXP);
    _worldPacket << int64(GuildTodayXP);
    _worldPacket << int64(WeeklyXP);
    _worldPacket << int64(GuildCurrentXP);

    return &_worldPacket;
}

void WorldPackets::Guild::GuildRequestMaxDailyXP::Read()
{
    ReadGuidMask(_worldPacket, GuildGuid, { 0, 3, 5, 1, 4, 6, 7, 2 });
    ReadGuidBytes(_worldPacket, GuildGuid, { 7, 4, 3, 5, 1, 2, 6, 0 });
}

WorldPacket const* WorldPackets::Guild::GuildMaxDailyXP::Write()
{
    _worldPacket << uint64(Xp);

    return &_worldPacket;
}

void WorldPackets::Guild::RequestGuildRewardsList::Read()
{
    _worldPacket >> CurrentVersion;
}

WorldPacket const* WorldPackets::Guild::GuildRewardList::Write()
{
    _worldPacket.WriteBits(0, 21);                         // reward count: no guild rewards yet
    _worldPacket.FlushBits();
    _worldPacket << int32(Version);

    return &_worldPacket;
}

void WorldPackets::Guild::GuildRequestPartyState::Read()
{
    ReadGuidMask(_worldPacket, GuildGuid, { 0, 6, 7, 3, 5, 1, 2, 4 });
    ReadGuidBytes(_worldPacket, GuildGuid, { 6, 3, 2, 1, 5, 0, 7, 4 });
}

WorldPacket const* WorldPackets::Guild::GuildPartyState::Write()
{
    _worldPacket.WriteBit(InGuildParty);
    _worldPacket.FlushBits();
    _worldPacket << float(GuildXPEarnedMult);
    _worldPacket << uint32(NumMembers);
    _worldPacket << uint32(NumRequired);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Guild::GuildChallengeUpdate::Write()
{
    for (int32 value : Xp)
        _worldPacket << int32(value);

    for (int32 value : MaxLevelGold)
        _worldPacket << int32(value);

    for (int32 value : MaxCount)
        _worldPacket << int32(value);

    for (int32 value : Gold)
        _worldPacket << int32(value);

    for (int32 value : CurrentCount)
        _worldPacket << int32(value);

    return &_worldPacket;
}

void WorldPackets::Guild::AutoDeclineGuildInvites::Read()
{
    uint8 allow = 0;
    _worldPacket >> allow;
    Allow = allow != 0;
}

void WorldPackets::Guild::GuildBankActivate::Read()
{
    _worldPacket >> Banker;
    _worldPacket >> FullUpdate;
}

void WorldPackets::Guild::GuildBankBuyTab::Read()
{
    _worldPacket >> Banker;
    _worldPacket >> BankTab;
}

void WorldPackets::Guild::GuildBankUpdateTab::Read()
{
    _worldPacket >> Banker;
    _worldPacket >> BankTab;
    _worldPacket >> Name;
    _worldPacket >> Icon;
}

void WorldPackets::Guild::GuildBankDepositMoney::Read()
{
    _worldPacket >> Banker;
    _worldPacket >> Money;
}

void WorldPackets::Guild::GuildBankQueryTab::Read()
{
    _worldPacket >> Banker;
    _worldPacket >> Tab;
    _worldPacket >> FullUpdate;
}

WorldPacket const* WorldPackets::Guild::GuildBankRemainingWithdrawMoney::Write()
{
    _worldPacket << RemainingWithdrawMoney;

    return &_worldPacket;
}

void WorldPackets::Guild::GuildBankWithdrawMoney::Read()
{
    _worldPacket >> Banker;
    _worldPacket >> Money;
}

WorldPacket const* WorldPackets::Guild::GuildBankQueryResults::Write()
{
    _worldPacket << uint64(Money);
    _worldPacket << uint8(Tab);
    _withdrawalsRemainingPos = _worldPacket.wpos();
    _worldPacket << int32(WithdrawalsRemaining);
    _worldPacket << uint8(FullUpdate);

    if (!Tab && FullUpdate)
    {
        _worldPacket << uint8(TabInfo.size());
        for (GuildBankTabInfo const& tab : TabInfo)
        {
            _worldPacket << tab.Name;
            _worldPacket << tab.Icon;
        }
    }

    _worldPacket << uint8(ItemInfo.size());
    for (GuildBankItemInfo const& item : ItemInfo)
    {
        _worldPacket << uint8(item.Slot);
        _worldPacket << uint32(item.ItemID);
        if (item.ItemID)
        {
            _worldPacket << int32(item.Flags);
            _worldPacket << int32(item.RandomPropertiesID);
            if (item.RandomPropertiesID)
                _worldPacket << int32(item.RandomPropertiesSeed);

            _worldPacket << int32(item.Count);
            _worldPacket << int32(item.EnchantmentID);
            _worldPacket << uint8(item.Charges);
            _worldPacket << uint8(item.SocketEnchant.size());

            for (GuildBankSocketEnchant const& socketEnchant : item.SocketEnchant)
            {
                _worldPacket << uint8(socketEnchant.SocketIndex);
                _worldPacket << int32(socketEnchant.SocketEnchantID);
            }
        }
    }

    return &_worldPacket;
}

void WorldPackets::Guild::GuildBankQueryResults::SetWithdrawalsRemaining(int32 withdrawalsRemaining)
{
    WithdrawalsRemaining = withdrawalsRemaining;
    _worldPacket.put<int32>(_withdrawalsRemainingPos, withdrawalsRemaining);
}

void WorldPackets::Guild::GuildBankSwapItems::Read()
{
    _worldPacket >> Banker;
    _worldPacket >> BankOnly;

    if (BankOnly)
    {
        // dest
        _worldPacket >> BankTab;
        _worldPacket >> BankSlot;
        _worldPacket >> ItemID;

        // src
        _worldPacket >> BankTab1;
        _worldPacket >> BankSlot1;
        _worldPacket >> ItemID1;

        _worldPacket >> AutoStore;
        _worldPacket >> BankItemCount;
    }
    else
    {
        _worldPacket >> BankTab;
        _worldPacket >> BankSlot;
        _worldPacket >> ItemID;

        _worldPacket >> AutoStore;
        if (AutoStore)
        {
            _worldPacket >> BankItemCount;
            _worldPacket >> ToSlot;
            _worldPacket >> StackCount;
        }
        else
        {
            _worldPacket >> ContainerSlot;
            _worldPacket >> ContainerItemSlot;
            _worldPacket >> ToSlot;
            _worldPacket >> StackCount;
        }
    }
}

void WorldPackets::Guild::GuildBankLogQuery::Read()
{
    _worldPacket >> Tab;
}

WorldPacket const* WorldPackets::Guild::GuildBankLogQueryResults::Write()
{
    _worldPacket << uint8(Tab);
    _worldPacket << uint8(Entry.size());

    for (GuildBankLogEntry const& logEntry : Entry)
    {
        _worldPacket << int8(logEntry.EntryType);
        _worldPacket << logEntry.PlayerGUID;

        switch (logEntry.EntryType)
        {
            case GUILD_BANK_LOG_DEPOSIT_ITEM:
            case GUILD_BANK_LOG_WITHDRAW_ITEM:
                _worldPacket << uint32(logEntry.ItemID);
                _worldPacket << uint32(logEntry.Count);
                break;
            case GUILD_BANK_LOG_MOVE_ITEM:
            case GUILD_BANK_LOG_MOVE_ITEM2:
                _worldPacket << uint32(logEntry.ItemID);
                _worldPacket << uint32(logEntry.Count);
                _worldPacket << uint8(logEntry.OtherTab);
                break;
            default:
                _worldPacket << uint32(logEntry.Money);
                break;
        }

        _worldPacket << uint32(logEntry.TimeOffset);
    }

    return &_worldPacket;
}

void WorldPackets::Guild::GuildBankTextQuery::Read()
{
    _worldPacket >> Tab;
}

WorldPacket const* WorldPackets::Guild::GuildBankTextQueryResult::Write()
{
    _worldPacket << uint8(Tab);
    _worldPacket << Text;

    return &_worldPacket;
}

void WorldPackets::Guild::GuildBankSetTabText::Read()
{
    _worldPacket >> Tab;
    _worldPacket >> TabText;
}

void WorldPackets::Guild::GuildSetGuildMaster::Read()
{
    uint32 length = _worldPacket.ReadBits(7);
    IsDethrone = _worldPacket.ReadBit();
    NewMasterName = _worldPacket.ReadString(length);
}

void WorldPackets::Guild::SaveGuildEmblem::Read()
{
    _worldPacket >> Vendor;
    _worldPacket >> EStyle;
    _worldPacket >> EColor;
    _worldPacket >> BStyle;
    _worldPacket >> BColor;
    _worldPacket >> Bg;
}

WorldPacket const* WorldPackets::Guild::PlayerSaveGuildEmblem::Write()
{
    _worldPacket << int32(Error);

    return &_worldPacket;
}
