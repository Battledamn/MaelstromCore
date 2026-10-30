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
#ifndef GuildPackets_h__
#define GuildPackets_h__

#include "Guild.h"
#include "ObjectGuid.h"
#include "Packet.h"
#include "PacketUtilities.h"
#include <array>
#include <boost/container/static_vector.hpp>

namespace WorldPackets
{
    namespace Guild
    {
        // 4.3.4 layouts ported from The Cataclysm Preservation Project (GuildPackets, GuildHandler, Guild.cpp).
        // Our ByteBuffer does not flush pending bits on append, so every bit block ends in FlushBits().

        class QueryGuildInfo final : public ClientPacket
        {
        public:
            QueryGuildInfo(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_QUERY, std::move(packet)) { }

            void Read() override;

            ObjectGuid GuildGuid;
            ObjectGuid PlayerGuid;
        };

        struct GuildInfoRank
        {
            uint32 RankID = 0;
            uint32 RankOrder = 0;
            std::string RankName;
        };

        class QueryGuildInfoResponse final : public ServerPacket
        {
        public:
            QueryGuildInfoResponse() : ServerPacket(SMSG_GUILD_QUERY_RESPONSE) { }

            WorldPacket const* Write() override;

            ObjectGuid GuildGuid;
            std::string GuildName;
            std::array<GuildInfoRank, GUILD_RANKS_MAX_COUNT> Ranks;
            uint32 RankCount = 0;

            uint32 EmblemStyle = 0;
            uint32 EmblemColor = 0;
            uint32 BorderStyle = 0;
            uint32 BorderColor = 0;
            uint32 BackgroundColor = 0;
        };

        class GuildCreate final : public ClientPacket
        {
        public:
            GuildCreate(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_CREATE, std::move(packet)) { }

            void Read() override;

            std::string GuildName;
        };

        class GuildGetInfo final : ClientPacket
        {
        public:
            GuildGetInfo(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_INFO, std::move(packet)) { }

            void Read() override { }
        };

        class GuildInfoResponse final : ServerPacket
        {
        public:
            GuildInfoResponse() : ServerPacket(SMSG_GUILD_INFO, 123) { }

            WorldPacket const* Write() override;

            std::string GuildName;
            time_t CreateDate = time_t(0);
            int32 NumMembers = 0;
            int32 NumAccounts = 0;
        };

        class GuildGetRoster final : public ClientPacket
        {
        public:
            GuildGetRoster(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_ROSTER, std::move(packet)) { }

            void Read() override { }
        };

        struct GuildRosterProfessionData
        {
            int32 DbID = 0;
            int32 Rank = 0;
            int32 Step = 0;
        };

        struct GuildRosterMemberData
        {
            ObjectGuid Guid;
            int64 WeeklyXP = 0;
            int64 TotalXP = 0;
            int32 RankID = 0;
            int32 AreaID = 0;
            int32 PersonalAchievementPoints = 0;
            int32 GuildReputation = 0;
            int32 GuildRepToCap = 0;
            float LastSave = 0.0f;
            std::string Name;
            uint32 VirtualRealmAddress = 0;
            std::string Note;
            std::string OfficerNote;
            uint8 Status = 0;
            uint8 Level = 0;
            uint8 ClassID = 0;
            uint8 Gender = 0;
            bool Authenticated = false;
            bool SorEligible = false;
            std::array<GuildRosterProfessionData, GUILD_PROFESSION_COUNT> Profession = {};
        };

        class GuildRoster final : public ServerPacket
        {
        public:
            GuildRoster() : ServerPacket(SMSG_GUILD_ROSTER, 4 + 4 + 4 + 4) { }

            WorldPacket const* Write() override;

            std::vector<GuildRosterMemberData> MemberData;
            std::string WelcomeText;
            std::string InfoText;
            uint32 CreateDate = 0;
            int32 NumAccounts = 0;
            int32 GuildFlags = 0;
            int32 WeeklyRepCap = 0;
        };

        class GuildUpdateMotdText final : public ClientPacket
        {
        public:
            GuildUpdateMotdText(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_MOTD, std::move(packet)) { }

            void Read() override;

            std::string MotdText;
        };

        // The 4.3.4 client acts on SMSG_GUILD_COMMAND_RESULT_2; SMSG_GUILD_COMMAND_RESULT is a JAM packet
        // with another layout.
        class GuildCommandResult final : public ServerPacket
        {
        public:
            GuildCommandResult() : ServerPacket(SMSG_GUILD_COMMAND_RESULT_2, 9) { }

            WorldPacket const* Write() override;

            std::string Name;
            int32 Result = 0;
            int32 Command = 0;
        };

        class AcceptGuildInvite final : public ClientPacket
        {
        public:
            AcceptGuildInvite(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_ACCEPT, std::move(packet)) { }

            void Read() override { }
        };

        class GuildDeclineInvitation final : public ClientPacket
        {
        public:
            GuildDeclineInvitation(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_DECLINE, std::move(packet)) { }

            void Read() override { }
        };

        class GuildInviteByName final : public ClientPacket
        {
        public:
            GuildInviteByName(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_INVITE, std::move(packet)) { }

            void Read() override;

            std::string Name;
        };

        class GuildInvite final : public ServerPacket
        {
        public:
            GuildInvite() : ServerPacket(SMSG_GUILD_INVITE, 100) { }

            WorldPacket const* Write() override;

            ObjectGuid GuildGuid;
            ObjectGuid OldGuildGuid;
            std::string InviterName;
            std::string GuildName;
            std::string OldGuildName;
            uint32 GuildLevel = 0;
            uint32 EmblemStyle = 0;
            uint32 EmblemColor = 0;
            uint32 BorderStyle = 0;
            uint32 BorderColor = 0;
            uint32 BackgroundColor = 0;
        };

        class GuildEvent final : public ServerPacket
        {
        public:
            GuildEvent() : ServerPacket(SMSG_GUILD_EVENT) { }

            WorldPacket const* Write() override;

            uint8 Type = 0;
            boost::container::static_vector<std::string_view, 3> Params;
            ObjectGuid Guid;
        };

        struct GuildEventEntry
        {
            ObjectGuid PlayerGUID;
            ObjectGuid OtherGUID;
            uint8 TransactionType = 0;
            uint8 RankID = 0;
            uint32 TransactionDate = 0;
        };

        class GuildEventLogQuery final : public ClientPacket
        {
        public:
            GuildEventLogQuery(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_EVENT_LOG_QUERY, std::move(packet)) { }

            void Read() override { }
        };

        class GuildEventLogQueryResults final : public ServerPacket
        {
        public:
            GuildEventLogQueryResults() : ServerPacket(SMSG_GUILD_EVENT_LOG_QUERY_RESULTS, 4) { }

            WorldPacket const* Write() override;

            std::vector<GuildEventEntry> Entry;
        };

        class GuildPermissionsQuery final : public ClientPacket
        {
        public:
            GuildPermissionsQuery(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_PERMISSIONS_QUERY, std::move(packet)) { }

            void Read() override { }
        };

        class GuildPermissionsQueryResults final : public ServerPacket
        {
        public:
            struct GuildRankTabPermissions
            {
                int32 Flags = 0;
                int32 WithdrawItemLimit = 0;
            };

            GuildPermissionsQueryResults() : ServerPacket(SMSG_GUILD_PERMISSIONS_QUERY_RESULTS, 4 * 15 + 3) { }

            WorldPacket const* Write() override;

            uint32 RankID = 0;
            int32 NumTabs = 0;
            int32 Flags = 0;
            int32 WithdrawGoldLimit = 0;
            std::array<GuildRankTabPermissions, GUILD_BANK_PACKET_TABS> Tab = {};
        };

        class GuildSetRankPermissions final : public ClientPacket
        {
        public:
            GuildSetRankPermissions(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_RANK, std::move(packet)) { }

            void Read() override;

            uint32 OldRankID = 0;
            uint32 OldFlags = 0;
            uint32 Flags = 0;
            std::array<uint32, GUILD_BANK_PACKET_TABS> TabFlags = {};
            std::array<uint32, GUILD_BANK_PACKET_TABS> TabWithdrawItemLimit = {};
            uint32 WithdrawGoldLimit = 0;
            uint32 RankID = 0;
            std::string RankName;
        };

        class GuildAddRank final : public ClientPacket
        {
        public:
            GuildAddRank(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_ADD_RANK, std::move(packet)) { }

            void Read() override;

            uint32 RankID = 0;
            std::string Name;
        };

        class GuildDeleteRank final : public ClientPacket
        {
        public:
            GuildDeleteRank(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_DEL_RANK, std::move(packet)) { }

            void Read() override;

            uint32 RankID = 0;
        };

        class GuildUpdateInfoText final : public ClientPacket
        {
        public:
            GuildUpdateInfoText(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_INFO_TEXT, std::move(packet)) { }

            void Read() override;

            std::string InfoText;
        };

        class GuildSetMemberNote final : public ClientPacket
        {
        public:
            GuildSetMemberNote(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_SET_NOTE, std::move(packet)) { }

            void Read() override;

            ObjectGuid NoteeGUID;
            bool IsPublic = false;
            std::string Note;
        };

        class GuildMemberUpdateNote final : public ServerPacket
        {
        public:
            GuildMemberUpdateNote() : ServerPacket(SMSG_GUILD_MEMBER_UPDATE_NOTE, 1 + 1 + 1 + 8 + 32) { }

            WorldPacket const* Write() override;

            ObjectGuid Member;
            bool IsPublic = false;
            std::string Note;
        };

        class GuildDelete final : public ClientPacket
        {
        public:
            GuildDelete(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_DISBAND, std::move(packet)) { }

            void Read() override { }
        };

        class GuildDemoteMember final : public ClientPacket
        {
        public:
            GuildDemoteMember(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_DEMOTE, std::move(packet)) { }

            void Read() override;

            ObjectGuid Demotee;
        };

        class GuildPromoteMember final : public ClientPacket
        {
        public:
            GuildPromoteMember(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_PROMOTE, std::move(packet)) { }

            void Read() override;

            ObjectGuid Promotee;
        };

        class GuildOfficerRemoveMember : public ClientPacket
        {
        public:
            GuildOfficerRemoveMember(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_REMOVE, std::move(packet)) { }

            void Read() override;

            ObjectGuid Removee;
        };

        class GuildAssignMemberRank final : public ClientPacket
        {
        public:
            GuildAssignMemberRank(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_ASSIGN_MEMBER_RANK, std::move(packet)) { }

            void Read() override;

            ObjectGuid Member;
            ObjectGuid Setter;
            uint32 RankID = 0;
        };

        class GuildRanksUpdate final : public ServerPacket
        {
        public:
            GuildRanksUpdate() : ServerPacket(SMSG_GUILD_RANKS_UPDATE, 3 + 8 + 8 + 4) { }

            WorldPacket const* Write() override;

            ObjectGuid Officer;
            ObjectGuid Other;
            bool Promote = false;
            uint32 RankID = 0;
        };

        class GuildLeave final : public ClientPacket
        {
        public:
            GuildLeave(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_LEAVE, std::move(packet)) { }

            void Read() override { }
        };

        class GuildQueryRanks final : public ClientPacket
        {
        public:
            GuildQueryRanks(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_QUERY_RANKS, std::move(packet)) { }

            void Read() override;

            ObjectGuid GuildGuid;
        };

        struct GuildRankData
        {
            uint32 RankID = 0;
            int32 RankOrder = 0;
            uint32 Flags = 0;
            uint32 WithdrawGoldLimit = 0;
            std::string RankName;
            std::array<uint32, GUILD_BANK_PACKET_TABS> TabFlags = {};
            std::array<uint32, GUILD_BANK_PACKET_TABS> TabWithdrawItemLimit = {};
        };

        class GuildRanks final : public ServerPacket
        {
        public:
            GuildRanks() : ServerPacket(SMSG_GUILD_RANKS, 4) { }

            WorldPacket const* Write() override;

            std::vector<GuildRankData> Ranks;
        };

        class GuildMemberDailyReset final : public ServerPacket
        {
        public:
            GuildMemberDailyReset() : ServerPacket(SMSG_GUILD_MEMBER_DAILY_RESET, 0) { }

            WorldPacket const* Write() override { return &_worldPacket; }
        };

        // Cataclysm-only queries. They are answered with an empty or level 1 state until guild levels,
        // news, rewards and challenges are implemented.
        class GuildQueryNews final : public ClientPacket
        {
        public:
            GuildQueryNews(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_QUERY_NEWS, std::move(packet)) { }

            void Read() override;
        };

        class GuildNews final : public ServerPacket
        {
        public:
            GuildNews() : ServerPacket(SMSG_GUILD_NEWS, 3) { }

            WorldPacket const* Write() override;
        };

        class GuildXPQuery final : public ClientPacket
        {
        public:
            GuildXPQuery(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_XP_QUERY, std::move(packet)) { }

            void Read() override;

            ObjectGuid GuildGuid;
        };

        class GuildXP final : public ServerPacket
        {
        public:
            GuildXP() : ServerPacket(SMSG_GUILD_XP, 40) { }

            WorldPacket const* Write() override;

            int64 WeeklyXP = 0;
            int64 TotalXP = 0;
            int64 GuildTodayXP = 0;
            int64 GuildRemainingXP = 0;
            int64 GuildCurrentXP = 0;
        };

        class GuildRequestMaxDailyXP final : public ClientPacket
        {
        public:
            GuildRequestMaxDailyXP(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_REQUEST_MAX_DAILY_XP, std::move(packet)) { }

            void Read() override;

            ObjectGuid GuildGuid;
        };

        class GuildMaxDailyXP final : public ServerPacket
        {
        public:
            GuildMaxDailyXP() : ServerPacket(SMSG_GUILD_MAX_DAILY_XP, 8) { }

            WorldPacket const* Write() override;

            uint64 Xp = 0;
        };

        class RequestGuildRewardsList final : public ClientPacket
        {
        public:
            RequestGuildRewardsList(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_GUILD_REWARDS_LIST, std::move(packet)) { }

            void Read() override;

            uint32 CurrentVersion = 0;
        };

        class GuildRewardList final : public ServerPacket
        {
        public:
            GuildRewardList() : ServerPacket(SMSG_GUILD_REWARD_LIST, 8) { }

            WorldPacket const* Write() override;

            int32 Version = 0;
        };

        class GuildRequestPartyState final : public ClientPacket
        {
        public:
            GuildRequestPartyState(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_REQUEST_PARTY_STATE, std::move(packet)) { }

            void Read() override;

            ObjectGuid GuildGuid;
        };

        class GuildPartyState final : public ServerPacket
        {
        public:
            GuildPartyState() : ServerPacket(SMSG_GUILD_PARTY_STATE_RESPONSE, 13) { }

            WorldPacket const* Write() override;

            bool InGuildParty = false;
            float GuildXPEarnedMult = 0.0f;
            uint32 NumMembers = 0;
            uint32 NumRequired = 0;
        };

        class GuildChallengeUpdateRequest final : public ClientPacket
        {
        public:
            GuildChallengeUpdateRequest(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_REQUEST_CHALLENGE_UPDATE, std::move(packet)) { }

            void Read() override { }
        };

        class GuildChallengeUpdate final : public ServerPacket
        {
        public:
            GuildChallengeUpdate() : ServerPacket(SMSG_GUILD_CHALLENGE_UPDATE, 80) { }

            WorldPacket const* Write() override;

            std::array<int32, GUILD_CHALLENGE_TYPE_COUNT> Xp = {};
            std::array<int32, GUILD_CHALLENGE_TYPE_COUNT> CurrentCount = {};
            std::array<int32, GUILD_CHALLENGE_TYPE_COUNT> MaxCount = {};
            std::array<int32, GUILD_CHALLENGE_TYPE_COUNT> Gold = {};
            std::array<int32, GUILD_CHALLENGE_TYPE_COUNT> MaxLevelGold = {};
        };

        class AutoDeclineGuildInvites final : public ClientPacket
        {
        public:
            AutoDeclineGuildInvites(WorldPacket&& packet) : ClientPacket(CMSG_AUTO_DECLINE_GUILD_INVITES, std::move(packet)) { }

            void Read() override;

            bool Allow = false;
        };

        class GuildBankActivate final : public ClientPacket
        {
        public:
            GuildBankActivate(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_BANKER_ACTIVATE, std::move(packet)) { }

            void Read() override;

            ObjectGuid Banker;
            bool FullUpdate = false;
        };

        class GuildBankBuyTab final : public ClientPacket
        {
        public:
            GuildBankBuyTab(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_BANK_BUY_TAB, std::move(packet)) { }

            void Read() override;

            ObjectGuid Banker;
            uint8 BankTab = 0;
        };

        class GuildBankUpdateTab final : public ClientPacket
        {
        public:
            GuildBankUpdateTab(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_BANK_UPDATE_TAB, std::move(packet)) { }

            void Read() override;

            ObjectGuid Banker;
            uint8 BankTab = 0;
            String<16, Strings::NoHyperlinks> Name;
            String<100> Icon;
        };

        class GuildBankDepositMoney final : public ClientPacket
        {
        public:
            GuildBankDepositMoney(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_BANK_DEPOSIT_MONEY, std::move(packet)) { }

            void Read() override;

            ObjectGuid Banker;
            uint32 Money = 0;
        };

        class GuildBankQueryTab final : public ClientPacket
        {
        public:
            GuildBankQueryTab(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_BANK_QUERY_TAB, std::move(packet)) { }

            void Read() override;

            ObjectGuid Banker;
            uint8 Tab = 0;
            bool FullUpdate = false;
        };

        class GuildBankRemainingWithdrawMoneyQuery final : public ClientPacket
        {
        public:
            GuildBankRemainingWithdrawMoneyQuery(WorldPacket&& packet)
                : ClientPacket(CMSG_GUILD_BANK_REMAINING_WITHDRAW_MONEY_QUERY, std::move(packet)) { }

            void Read() override { }
        };

        class GuildBankRemainingWithdrawMoney final : public ServerPacket
        {
        public:
            GuildBankRemainingWithdrawMoney() : ServerPacket(SMSG_GUILD_BANK_MONEY_WITHDRAWN, 8) { }

            WorldPacket const* Write() override;

            int64 RemainingWithdrawMoney = 0;
        };

        class GuildBankWithdrawMoney final : public ClientPacket
        {
        public:
            GuildBankWithdrawMoney(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_BANK_WITHDRAW_MONEY, std::move(packet)) { }

            void Read() override;

            ObjectGuid Banker;
            uint32 Money = 0;
        };

        struct GuildBankSocketEnchant
        {
            uint8 SocketIndex = 0;
            int32 SocketEnchantID = 0;
        };

        struct GuildBankItemInfo
        {
            uint32 ItemID = 0;
            int32 RandomPropertiesSeed = 0;
            int32 RandomPropertiesID = 0;
            uint8 Slot = 0;
            int32 Count = 0;
            int32 EnchantmentID = 0;
            int32 Charges = 0;
            int32 Flags = 0;
            std::vector<GuildBankSocketEnchant> SocketEnchant;
        };

        struct GuildBankTabInfo
        {
            std::string Name;
            std::string Icon;
        };

        class GuildBankQueryResults final : public ServerPacket
        {
        public:
            GuildBankQueryResults() : ServerPacket(SMSG_GUILD_BANK_LIST, 25) { }

            WorldPacket const* Write() override;

            std::vector<GuildBankItemInfo> ItemInfo;
            std::vector<GuildBankTabInfo> TabInfo;
            int32 WithdrawalsRemaining = 0;
            uint8 Tab = 0;
            uint64 Money = 0;
            bool FullUpdate = false;

            void SetWithdrawalsRemaining(int32 withdrawalsRemaining);

        private:
            std::size_t _withdrawalsRemainingPos = 0;
        };

        class GuildBankSwapItems final : public ClientPacket
        {
        public:
            GuildBankSwapItems(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_BANK_SWAP_ITEMS, std::move(packet)) { }

            void Read() override;

            ObjectGuid Banker;
            int32 StackCount = 0;
            int32 BankItemCount = 0;
            uint32 ItemID = 0;
            uint32 ItemID1 = 0;
            uint8 ToSlot = 0;
            uint8 BankSlot = 0;
            uint8 BankSlot1 = 0;
            uint8 BankTab = 0;
            uint8 BankTab1 = 0;
            uint8 ContainerSlot = 0;
            uint8 ContainerItemSlot = 0;
            bool AutoStore = false;
            bool BankOnly = false;
        };

        class GuildBankLogQuery final : public ClientPacket
        {
        public:
            GuildBankLogQuery(WorldPacket&& packet) : ClientPacket(MSG_GUILD_BANK_LOG_QUERY, std::move(packet)) { }

            void Read() override;

            uint8 Tab = 0;
        };

        struct GuildBankLogEntry
        {
            ObjectGuid PlayerGUID;
            uint32 TimeOffset = 0;
            int8 EntryType = 0;
            uint32 Money = 0;
            int32 ItemID = 0;
            int32 Count = 0;
            int8 OtherTab = 0;
        };

        class GuildBankLogQueryResults final : public ServerPacket
        {
        public:
            GuildBankLogQueryResults() : ServerPacket(MSG_GUILD_BANK_LOG_QUERY_SERVER, 25) { }

            WorldPacket const* Write() override;

            uint8 Tab = 0;
            std::vector<GuildBankLogEntry> Entry;
        };

        class GuildBankTextQuery final : public ClientPacket
        {
        public:
            GuildBankTextQuery(WorldPacket&& packet) : ClientPacket(MSG_QUERY_GUILD_BANK_TEXT, std::move(packet)) { }

            void Read() override;

            uint8 Tab = 0;
        };

        class GuildBankTextQueryResult : public ServerPacket
        {
        public:
            GuildBankTextQueryResult() : ServerPacket(MSG_QUERY_GUILD_BANK_TEXT_SERVER, 4 + 2) { }

            WorldPacket const* Write() override;

            uint8 Tab = 0;
            std::string Text;
        };

        class GuildBankSetTabText final : public ClientPacket
        {
        public:
            GuildBankSetTabText(WorldPacket&& packet) : ClientPacket(CMSG_SET_GUILD_BANK_TEXT, std::move(packet)) { }

            void Read() override;

            uint8 Tab = 0;
            String<500, Strings::NoHyperlinks> TabText;
        };

        class GuildSetGuildMaster final : public ClientPacket
        {
        public:
            GuildSetGuildMaster(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_LEADER, std::move(packet)) { }

            void Read() override;

            std::string NewMasterName;
            bool IsDethrone = false;
        };

        class SaveGuildEmblem final : public ClientPacket
        {
        public:
            SaveGuildEmblem(WorldPacket&& packet) : ClientPacket(MSG_SAVE_GUILD_EMBLEM, std::move(packet)) { }

            void Read() override;

            ObjectGuid Vendor;
            int32 BStyle = 0;
            int32 EStyle = 0;
            int32 BColor = 0;
            int32 EColor = 0;
            int32 Bg = 0;
        };

        class PlayerSaveGuildEmblem final : public ServerPacket
        {
        public:
            PlayerSaveGuildEmblem() : ServerPacket(MSG_SAVE_GUILD_EMBLEM_SERVER, 4) { }

            WorldPacket const* Write() override;

            int32 Error = 0;
        };
    }
}

#endif // GuildPackets_h__
