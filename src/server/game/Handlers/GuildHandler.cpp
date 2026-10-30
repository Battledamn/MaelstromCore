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

#include "GameTime.h"
#include "Guild.h"
#include "GuildMgr.h"
#include "GuildPackets.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "SocialMgr.h"
#include "World.h"
#include "WorldSession.h"

void WorldSession::HandleGuildQueryOpcode(WorldPackets::Guild::QueryGuildInfo& query)
{
    LOG_DEBUG("guild", "CMSG_GUILD_QUERY [{}]: Guild: {} Target: {}", GetPlayerInfo(), query.GuildGuid.ToString(),
        query.PlayerGuid.ToString());

    // Also sent from the character list, before a player is logged in
    if (Guild* guild = sGuildMgr->GetGuildById(query.GuildGuid.GetCounter()))
        guild->HandleQuery(this);
}

void WorldSession::HandleGuildCreateOpcode(WorldPackets::Guild::GuildCreate& packet)
{
    LOG_ERROR("network.opcode", "CMSG_GUILD_CREATE: Possible hacking-attempt: {} tried to create a guild [Name: {}] using cheats", GetPlayerInfo(), packet.GuildName);
}

void WorldSession::HandleGuildInviteOpcode(WorldPackets::Guild::GuildInviteByName& packet)
{
    LOG_DEBUG("guild", "CMSG_GUILD_INVITE [{}]: Invited: {}", GetPlayerInfo(), packet.Name);
    if (normalizePlayerName(packet.Name))
        if (Guild* guild = GetPlayer()->GetGuild())
            guild->HandleInviteMember(this, packet.Name);
}

void WorldSession::HandleGuildRemoveOpcode(WorldPackets::Guild::GuildOfficerRemoveMember& packet)
{
    LOG_DEBUG("guild", "CMSG_GUILD_REMOVE [{}]: Target: {}", GetPlayerInfo(), packet.Removee.ToString());

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleRemoveMember(this, packet.Removee);
}

void WorldSession::HandleGuildAcceptOpcode(WorldPackets::Guild::AcceptGuildInvite& /*invite*/)
{
    LOG_DEBUG("guild", "CMSG_GUILD_ACCEPT [{}]", GetPlayer()->GetName());

    if (sWorld->getBoolConfig(CONFIG_TRIAL_RESTRICTION_GUILD) && IsTrialAccount())
    {
        Guild::SendCommandResult(this, GUILD_COMMAND_INVITE, ERR_GUILD_PERMISSIONS);
        GetPlayer()->SetGuildIdInvited(0);
        GetPlayer()->SetInGuild(0);
        return;
    }

    if (!GetPlayer()->GetGuildId())
        if (Guild* guild = sGuildMgr->GetGuildById(GetPlayer()->GetGuildIdInvited()))
            guild->HandleAcceptMember(this);
}

void WorldSession::HandleGuildDeclineOpcode(WorldPackets::Guild::GuildDeclineInvitation& /*decline*/)
{
    LOG_DEBUG("guild", "CMSG_GUILD_DECLINE [{}]", GetPlayerInfo());

    if (GetPlayer()->GetGuild())
    {
        return;
    }

    GetPlayer()->SetGuildIdInvited(0);
    GetPlayer()->SetInGuild(0);
}

void WorldSession::HandleGuildInfoOpcode(WorldPackets::Guild::GuildGetInfo& /*packet*/)
{
    LOG_DEBUG("guild", "CMSG_GUILD_INFO [{}]", GetPlayerInfo());

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->SendInfo(this);
}

void WorldSession::HandleGuildRosterOpcode(WorldPackets::Guild::GuildGetRoster& /*packet*/)
{
    LOG_DEBUG("guild", "CMSG_GUILD_ROSTER [{}]", GetPlayerInfo());

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleRoster(this);
    else
        Guild::SendCommandResult(this, GUILD_COMMAND_ROSTER, ERR_GUILD_PLAYER_NOT_IN_GUILD);
}

void WorldSession::HandleGuildPromoteOpcode(WorldPackets::Guild::GuildPromoteMember& promote)
{
    LOG_DEBUG("guild", "CMSG_GUILD_PROMOTE [{}]: Target: {}", GetPlayerInfo(), promote.Promotee.ToString());

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleUpdateMemberRank(this, promote.Promotee, false);
}

void WorldSession::HandleGuildDemoteOpcode(WorldPackets::Guild::GuildDemoteMember& demote)
{
    LOG_DEBUG("guild", "CMSG_GUILD_DEMOTE [{}]: Target: {}", GetPlayerInfo(), demote.Demotee.ToString());

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleUpdateMemberRank(this, demote.Demotee, true);
}

void WorldSession::HandleGuildAssignRankOpcode(WorldPackets::Guild::GuildAssignMemberRank& packet)
{
    LOG_DEBUG("guild", "CMSG_GUILD_ASSIGN_MEMBER_RANK [{}]: Target: {} Rank: {}, Issuer: {}", GetPlayerInfo(),
        packet.Member.ToString(), packet.RankID, packet.Setter.ToString());

    if (packet.RankID >= GUILD_RANKS_MAX_COUNT)
        return;

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleSetMemberRank(this, packet.Member, uint8(packet.RankID));
}

void WorldSession::HandleGuildLeaveOpcode(WorldPackets::Guild::GuildLeave& /*leave*/)
{
    LOG_DEBUG("guild", "CMSG_GUILD_LEAVE [{}]", GetPlayerInfo());

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleLeaveMember(this);
}

void WorldSession::HandleGuildDisbandOpcode(WorldPackets::Guild::GuildDelete& /*packet*/)
{
    LOG_DEBUG("guild", "CMSG_GUILD_DISBAND [{}]", GetPlayerInfo());

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleDisband(this);
}

void WorldSession::HandleGuildLeaderOpcode(WorldPackets::Guild::GuildSetGuildMaster& packet)
{
    LOG_DEBUG("guild", "CMSG_GUILD_SET_GUILD_MASTER [{}]: Target: {} Dethrone: {}", GetPlayerInfo(), packet.NewMasterName,
        packet.IsDethrone);

    // Dethroning an inactive guild master is not implemented: it needs the 90 day inactivity tracking
    if (packet.IsDethrone)
    {
        Guild::SendCommandResult(this, GUILD_COMMAND_CHANGE_LEADER, ERR_GUILD_PERMISSIONS);
        return;
    }

    Guild* guild = GetPlayer()->GetGuild();
    if (!guild)
        return;

    // /gleader without a name sends an empty one
    if (!normalizePlayerName(packet.NewMasterName))
    {
        Guild::SendCommandResult(this, GUILD_COMMAND_CHANGE_LEADER, ERR_GUILD_PLAYER_NOT_FOUND_S, packet.NewMasterName);
        return;
    }

    guild->HandleSetLeader(this, packet.NewMasterName);
}

void WorldSession::HandleGuildMOTDOpcode(WorldPackets::Guild::GuildUpdateMotdText& packet)
{
    LOG_DEBUG("guild", "CMSG_GUILD_MOTD [{}]: MOTD: {}", GetPlayerInfo(), packet.MotdText);

    if (packet.MotdText.size() > 128)
        packet.MotdText.resize(128);

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleSetMOTD(this, packet.MotdText);
}

void WorldSession::HandleGuildSetPublicNoteOpcode(WorldPackets::Guild::GuildSetMemberNote& packet)
{
    LOG_DEBUG("guild", "CMSG_GUILD_SET_NOTE [{}]: Target: {}, Note: {}, Public: {}", GetPlayerInfo(),
        packet.NoteeGUID.ToString(), packet.Note, packet.IsPublic);

    if (packet.Note.size() > 31)
        packet.Note.resize(31);

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleSetMemberNote(this, packet.NoteeGUID, packet.Note, packet.IsPublic);
}

void WorldSession::HandleGuildQueryRanksOpcode(WorldPackets::Guild::GuildQueryRanks& packet)
{
    LOG_DEBUG("guild", "CMSG_GUILD_QUERY_RANKS [{}]: Guild: {}", GetPlayerInfo(), packet.GuildGuid.ToString());

    if (Guild* guild = sGuildMgr->GetGuildById(packet.GuildGuid.GetCounter()))
        if (guild->GetMember(GetPlayer()->GetGUID()))
            guild->SendGuildRankInfo(this);
}

void WorldSession::HandleGuildRankOpcode(WorldPackets::Guild::GuildSetRankPermissions& packet)
{
    Guild* guild = GetPlayer()->GetGuild();
    if (!guild)
        return;

    if (packet.RankID >= GUILD_RANKS_MAX_COUNT)
        return;

    if (packet.RankName.size() > 15)
        packet.RankName.resize(15);

    // The packet always carries 8 tabs; this core has GUILD_BANK_MAX_TABS of them
    std::array<GuildBankRightsAndSlots, GUILD_BANK_MAX_TABS> rightsAndSlots;
    for (uint8 tabId = 0; tabId < GUILD_BANK_MAX_TABS; ++tabId)
        rightsAndSlots[tabId] = GuildBankRightsAndSlots(tabId, uint8(packet.TabFlags[tabId]), packet.TabWithdrawItemLimit[tabId]);

    LOG_DEBUG("guild", "CMSG_GUILD_SET_RANK_PERMISSIONS [{}]: Rank: {} ({})", GetPlayerInfo(), packet.RankName, packet.RankID);

    guild->HandleSetRankInfo(this, uint8(packet.RankID), packet.RankName, packet.Flags, packet.WithdrawGoldLimit, rightsAndSlots);
}

void WorldSession::HandleGuildAddRankOpcode(WorldPackets::Guild::GuildAddRank& packet)
{
    LOG_DEBUG("guild", "CMSG_GUILD_ADD_RANK [{}]: Rank: {}", GetPlayerInfo(), packet.Name);

    if (packet.Name.size() > 15)
        packet.Name.resize(15);

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleAddNewRank(this, packet.Name);
}

void WorldSession::HandleGuildDelRankOpcode(WorldPackets::Guild::GuildDeleteRank& packet)
{
    LOG_DEBUG("guild", "CMSG_GUILD_DEL_RANK [{}]: Rank: {}", GetPlayerInfo(), packet.RankID);

    if (packet.RankID >= GUILD_RANKS_MAX_COUNT)
        return;

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleRemoveRank(this, uint8(packet.RankID));
}

void WorldSession::HandleGuildChangeInfoTextOpcode(WorldPackets::Guild::GuildUpdateInfoText& packet)
{
    LOG_DEBUG("guild", "CMSG_GUILD_INFO_TEXT [{}]: {}", GetPlayerInfo(), packet.InfoText);

    if (packet.InfoText.size() > 500)
        packet.InfoText.resize(500);

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleSetInfo(this, packet.InfoText);
}

void WorldSession::HandleSaveGuildEmblemOpcode(WorldPackets::Guild::SaveGuildEmblem& packet)
{
    EmblemInfo emblemInfo;
    emblemInfo.ReadPacket(packet);

    LOG_DEBUG("guild", "MSG_SAVE_GUILD_EMBLEM [{}]: Guid: [{}] Style: {}, Color: {}, BorderStyle: {}, BorderColor: {}, BackgroundColor: {}"
                   , GetPlayerInfo(), packet.Vendor.ToString(), emblemInfo.GetStyle()
                   , emblemInfo.GetColor(), emblemInfo.GetBorderStyle()
                   , emblemInfo.GetBorderColor(), emblemInfo.GetBackgroundColor());
    if (GetPlayer()->GetNPCIfCanInteractWith(packet.Vendor, UNIT_NPC_FLAG_TABARDDESIGNER))
    {
        // Remove fake death
        if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
            GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

        if (Guild* guild = GetPlayer()->GetGuild())
            guild->HandleSetEmblem(this, emblemInfo);
        else
            Guild::SendSaveEmblemResult(this, ERR_GUILDEMBLEM_NOGUILD); // "You are not part of a guild!";
    }
    else
        Guild::SendSaveEmblemResult(this, ERR_GUILDEMBLEM_INVALIDVENDOR); // "That's not an emblem vendor!"
}

void WorldSession::HandleGuildEventLogQueryOpcode(WorldPackets::Guild::GuildEventLogQuery& /*packet*/)
{
    LOG_DEBUG("guild", "CMSG_GUILD_EVENT_LOG_QUERY [{}]", GetPlayerInfo());

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->SendEventLog(this);
}

void WorldSession::HandleGuildBankMoneyWithdrawn(WorldPackets::Guild::GuildBankRemainingWithdrawMoneyQuery& /*packet*/)
{
    LOG_DEBUG("guild", "CMSG_GUILD_BANK_REMAINING_WITHDRAW_MONEY_QUERY [{}]", GetPlayerInfo());

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->SendMoneyInfo(this);
}

void WorldSession::HandleGuildPermissions(WorldPackets::Guild::GuildPermissionsQuery& /* packet */)
{
    if (Guild* guild = GetPlayer()->GetGuild())
        guild->SendPermissions(this);
}

// Called when clicking on Guild bank gameobject
void WorldSession::HandleGuildBankerActivate(WorldPackets::Guild::GuildBankActivate& packet)
{
    LOG_DEBUG("guild", "CMSG_GUILD_BANKER_ACTIVATE [{}]: Go: [{}] AllSlots: {}"
    , GetPlayerInfo(), packet.Banker.ToString(), packet.FullUpdate);

    GameObject const* const go = GetPlayer()->GetGameObjectIfCanInteractWith(packet.Banker, GAMEOBJECT_TYPE_GUILD_BANK);
    if (!go)
        return;

    Guild* const guild = GetPlayer()->GetGuild();
    if (!guild)
    {
        Guild::SendCommandResult(this, GUILD_COMMAND_VIEW_TAB, ERR_GUILD_PLAYER_NOT_IN_GUILD);
        return;
    }

    guild->SendBankTabsInfo(this, packet.FullUpdate);
}

// Called when opening guild bank tab only
void WorldSession::HandleGuildBankQueryTab(WorldPackets::Guild::GuildBankQueryTab& packet)
{
    LOG_DEBUG("guild", "CMSG_GUILD_BANK_QUERY_TAB [{}]: Go: [{}], TabId: {}, ShowTabs: {}"
    , GetPlayerInfo(), packet.Banker.ToString(), packet.Tab, packet.FullUpdate);

    if (GetPlayer()->GetGameObjectIfCanInteractWith(packet.Banker, GAMEOBJECT_TYPE_GUILD_BANK))
        if (Guild* guild = GetPlayer()->GetGuild())
            guild->SendBankTabData(this, packet.Tab, packet.FullUpdate);
}

void WorldSession::HandleGuildBankDepositMoney(WorldPackets::Guild::GuildBankDepositMoney& packet)
{
    LOG_DEBUG("guild", "CMSG_GUILD_BANK_DEPOSIT_MONEY [{}]: Go: [{}], money: {}",
              GetPlayerInfo(), packet.Banker.ToString(), packet.Money);

    if (GetPlayer()->GetGameObjectIfCanInteractWith(packet.Banker, GAMEOBJECT_TYPE_GUILD_BANK))
        if (packet.Money && GetPlayer()->HasEnoughMoney(packet.Money))
            if (Guild* guild = GetPlayer()->GetGuild())
                guild->HandleMemberDepositMoney(this, packet.Money);
}

void WorldSession::HandleGuildBankWithdrawMoney(WorldPackets::Guild::GuildBankWithdrawMoney& packet)
{
    LOG_DEBUG("guild", "CMSG_GUILD_BANK_WITHDRAW_MONEY [{}]: Go: [{}], money: {}",
                   GetPlayerInfo(), packet.Banker.ToString(), packet.Money);
    if (packet.Money && GetPlayer()->GetGameObjectIfCanInteractWith(packet.Banker, GAMEOBJECT_TYPE_GUILD_BANK))
        if (Guild* guild = GetPlayer()->GetGuild())
            guild->HandleMemberWithdrawMoney(this, packet.Money);
}

void WorldSession::HandleGuildBankSwapItems(WorldPackets::Guild::GuildBankSwapItems& packet)
{
    if (!GetPlayer()->GetGameObjectIfCanInteractWith(packet.Banker, GAMEOBJECT_TYPE_GUILD_BANK))
    {
        return;
    }

    Guild* guild = GetPlayer()->GetGuild();
    if (!guild)
    {
        return;
    }

    if (packet.BankOnly)
        guild->SwapItems(GetPlayer(), packet.BankTab1, packet.BankSlot1, packet.BankTab, packet.BankSlot, packet.BankItemCount);
    else
    {
        uint8 playerBag = NULL_BAG;
        uint8 playerSlotId = NULL_SLOT;
        uint8 toChar = 1;
        uint32 splitedAmount = 0;

        if (!packet.AutoStore)
        {
            playerBag = packet.ContainerSlot;
            playerSlotId = packet.ContainerItemSlot;
            toChar = packet.ToSlot;
            splitedAmount = packet.StackCount;
        }

        // Player <-> Bank
        // Allow to work with inventory only
        if (!Player::IsInventoryPos(playerBag, playerSlotId) && !(playerBag == NULL_BAG && playerSlotId == NULL_SLOT))
            GetPlayer()->SendEquipError(EQUIP_ERR_NONE, nullptr);
        else
            guild->SwapItemsWithInventory(GetPlayer(), toChar != 0, packet.BankTab, packet.BankSlot, playerBag, playerSlotId, splitedAmount);
    }
}

void WorldSession::HandleGuildBankBuyTab(WorldPackets::Guild::GuildBankBuyTab& packet)
{
    LOG_DEBUG("guild", "CMSG_GUILD_BANK_BUY_TAB [{}]: [{}[, TabId: {}", GetPlayerInfo(), packet.Banker .ToString(), packet.BankTab);

    if (GetPlayer()->GetGameObjectIfCanInteractWith(packet.Banker, GAMEOBJECT_TYPE_GUILD_BANK))
        if (Guild* guild = GetPlayer()->GetGuild())
            guild->HandleBuyBankTab(this, packet.BankTab);
}

void WorldSession::HandleGuildBankUpdateTab(WorldPackets::Guild::GuildBankUpdateTab& packet)
{
    LOG_DEBUG("guild", "CMSG_GUILD_BANK_UPDATE_TAB [{}]: Go: [{}], TabId: {}, Name: {}, Icon: {}"
    , GetPlayerInfo(), packet.Banker.ToString(), packet.BankTab, packet.Name, packet.Icon);

    if (!packet.Name.empty() && !packet.Icon.empty())
        if (GetPlayer()->GetGameObjectIfCanInteractWith(packet.Banker, GAMEOBJECT_TYPE_GUILD_BANK))
            if (Guild* guild = GetPlayer()->GetGuild())
                guild->HandleSetBankTabInfo(this, packet.BankTab, packet.Name, packet.Icon);
}

void WorldSession::HandleGuildBankLogQuery(WorldPackets::Guild::GuildBankLogQuery& packet)
{
    LOG_DEBUG("guild", "MSG_GUILD_BANK_LOG_QUERY [{}]: TabId: {}", GetPlayerInfo(), packet.Tab);

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->SendBankLog(this, packet.Tab);
}

void WorldSession::HandleQueryGuildBankTabText(WorldPackets::Guild::GuildBankTextQuery& packet)
{
    LOG_DEBUG("guild", "MSG_QUERY_GUILD_BANK_TEXT [{}]: TabId: {}", GetPlayerInfo(), packet.Tab);

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->SendBankTabText(this, packet.Tab);
}

void WorldSession::HandleSetGuildBankTabText(WorldPackets::Guild::GuildBankSetTabText& packet)
{
    LOG_DEBUG("guild", "CMSG_SET_GUILD_BANK_TEXT [{}]: TabId: {}, Text: {}", GetPlayerInfo(), packet.Tab, packet.TabText);

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->SetBankTabText(packet.Tab, packet.TabText);
}

// Cataclysm guild features that are not implemented yet (guild levels, news, rewards, challenges):
// answer with an empty or level 1 state so the guild window works.
void WorldSession::HandleGuildQueryNewsOpcode(WorldPackets::Guild::GuildQueryNews& /*packet*/)
{
    if (GetPlayer()->GetGuild())
        SendPacket(WorldPackets::Guild::GuildNews().Write());
}

void WorldSession::HandleGuildXPQueryOpcode(WorldPackets::Guild::GuildXPQuery& packet)
{
    if (Guild* guild = sGuildMgr->GetGuildById(packet.GuildGuid.GetCounter()))
        if (guild->GetMember(GetPlayer()->GetGUID()))
            SendPacket(WorldPackets::Guild::GuildXP().Write());
}

void WorldSession::HandleGuildRequestMaxDailyXP(WorldPackets::Guild::GuildRequestMaxDailyXP& packet)
{
    if (Guild* guild = sGuildMgr->GetGuildById(packet.GuildGuid.GetCounter()))
        if (guild->GetMember(GetPlayer()->GetGUID()))
            SendPacket(WorldPackets::Guild::GuildMaxDailyXP().Write());
}

void WorldSession::HandleRequestGuildRewardsList(WorldPackets::Guild::RequestGuildRewardsList& /*packet*/)
{
    if (!GetPlayer()->GetGuild())
        return;

    WorldPackets::Guild::GuildRewardList rewardList;
    rewardList.Version = int32(GameTime::GetGameTime().count());
    SendPacket(rewardList.Write());
}

void WorldSession::HandleGuildRequestPartyState(WorldPackets::Guild::GuildRequestPartyState& packet)
{
    Guild* guild = sGuildMgr->GetGuildById(packet.GuildGuid.GetCounter());
    if (!guild || !guild->GetMember(GetPlayer()->GetGUID()))
        return;

    // Guild groups only matter for guild experience and reputation bonuses, which are not implemented
    SendPacket(WorldPackets::Guild::GuildPartyState().Write());
}

void WorldSession::HandleGuildRequestChallengeUpdate(WorldPackets::Guild::GuildChallengeUpdateRequest& /*packet*/)
{
    if (GetPlayer()->GetGuild())
        SendPacket(WorldPackets::Guild::GuildChallengeUpdate().Write());
}

void WorldSession::HandleAutoDeclineGuildInvites(WorldPackets::Guild::AutoDeclineGuildInvites& packet)
{
    if (packet.Allow)
        GetPlayer()->SetPlayerFlag(PLAYER_FLAGS_AUTO_DECLINE_GUILD);
    else
        GetPlayer()->RemovePlayerFlag(PLAYER_FLAGS_AUTO_DECLINE_GUILD);
}
