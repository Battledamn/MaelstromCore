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

#include "Chat.h"
#include "DatabaseEnv.h"
#include "Group.h"
#include "GroupMgr.h"
#include "LFGMgr.h"
#include "Language.h"
#include "Log.h"
#include "MapMgr.h"
#include "MiscPackets.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "PartyPackets.h"
#include "Pet.h"
#include "Player.h"
#include "Realm.h"
#include "ScriptMgr.h"
#include "SocialMgr.h"
#include "SpellAuras.h"
#include "Util.h"
#include "Vehicle.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"

class Aura;

/* differeces from off:
    -you can uninvite yourself - is is useful
    -you can accept invitation even if leader went offline
*/
/* todo:
    -group_destroyed msg is sent but not shown
    -reduce xp gaining when in raid group
    -quest sharing has to be corrected
    -FIX sending PartyMemberStats
*/

void WorldSession::SendPartyResult(PartyOperation operation, std::string const& member, PartyResult res, uint32 val /* = 0 */)
{
    WorldPackets::Party::PartyCommandResult packet;
    packet.Command = uint32(operation);
    packet.Name = member;
    packet.Result = uint32(res);
    packet.ResultData = val; // LFD cooldown (ERR_PARTY_LFG_BOOT_COOLDOWN_S and ERR_PARTY_LFG_BOOT_NOT_ELIGIBLE_S)
    SendPacket(packet.Write());
}

namespace
{
    void SendPartyInvite(Player* invitingPlayer, Player* invitedPlayer, uint32 proposedRoles, bool canAccept)
    {
        WorldPackets::Party::PartyInvite partyInvite;
        partyInvite.CanAccept = canAccept;
        partyInvite.InviterName = invitingPlayer->GetName();
        partyInvite.InviterGUID = invitingPlayer->GetGUID();
        partyInvite.InviterRealmName = realm.Name;
        partyInvite.InviterCfgRealmID = realm.Id.Realm;
        partyInvite.Timestamp = getMSTime();
        partyInvite.ProposedRoles = proposedRoles;
        invitedPlayer->SendDirectMessage(partyInvite.Write());
    }
}

void WorldSession::HandlePartyInviteOpcode(WorldPackets::Party::PartyInviteClient& packet)
{
    std::string membername = packet.TargetName;

    // attempt add selected player

    // cheating
    if (!normalizePlayerName(membername))
    {
        SendPartyResult(PARTY_OP_INVITE, membername, ERR_BAD_PLAYER_NAME_S);
        return;
    }

    Player* invitingPlayer = GetPlayer();
    Player* invitedPlayer = ObjectAccessor::FindPlayerByName(membername, false);

    // no player or cheat self-invite
    if (!invitedPlayer || invitedPlayer == invitingPlayer)
    {
        SendPartyResult(PARTY_OP_INVITE, membername, ERR_BAD_PLAYER_NAME_S);
        return;
    }

    if (!sScriptMgr->OnPlayerCanGroupInvite(invitingPlayer, membername))
        return;

    if (sWorld->getBoolConfig(CONFIG_TRIAL_RESTRICTION_PARTY) && IsTrialAccount())
    {
        SendPartyResult(PARTY_OP_INVITE, membername, ERR_INVITE_RESTRICTED);
        return;
    }

    if (invitingPlayer->IsSpectator() || invitedPlayer->IsSpectator())
    {
        SendPartyResult(PARTY_OP_INVITE, membername, ERR_INVITE_RESTRICTED);
        return;
    }

    // restrict invite to GMs
    if (!sWorld->getBoolConfig(CONFIG_ALLOW_GM_GROUP) && !invitingPlayer->IsGameMaster() && invitedPlayer->IsGameMaster())
    {
        SendPartyResult(PARTY_OP_INVITE, membername, ERR_BAD_PLAYER_NAME_S);
        return;
    }

    // target has opted out of receiving group invites
    if (!invitedPlayer->IsAcceptGroupInvites())
    {
        SendPartyResult(PARTY_OP_INVITE, membername, ERR_BAD_PLAYER_NAME_S);
        return;
    }

    // can't group with
    if (!invitingPlayer->IsGameMaster() && !sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GROUP) && invitingPlayer->GetTeamId() != invitedPlayer->GetTeamId())
    {
        SendPartyResult(PARTY_OP_INVITE, membername, ERR_PLAYER_WRONG_FACTION);
        return;
    }
    if (invitingPlayer->GetInstanceId() != 0 && invitedPlayer->GetInstanceId() != 0 && invitingPlayer->GetInstanceId() != invitedPlayer->GetInstanceId() && invitingPlayer->GetMapId() == invitedPlayer->GetMapId())
    {
        SendPartyResult(PARTY_OP_INVITE, membername, ERR_TARGET_NOT_IN_INSTANCE_S);
        return;
    }

    if (invitedPlayer->GetSocial()->HasIgnore(invitingPlayer->GetGUID()))
    {
        SendPartyResult(PARTY_OP_INVITE, membername, ERR_IGNORING_YOU_S);
        return;
    }

    if (!invitedPlayer->GetSocial()->HasFriend(invitingPlayer->GetGUID()) && invitingPlayer->GetLevel() < sWorld->getIntConfig(CONFIG_PARTY_LEVEL_REQ))
    {
        SendPartyResult(PARTY_OP_INVITE, invitedPlayer->GetName(), ERR_INVITE_RESTRICTED);
        return;
    }

    // Battlefield raids (e.g. Wintergrasp) have their composition managed by the BF
    // system based on queue and team balance. Letting raid members recruit outsiders
    // bypasses Battlefield::AddOrSetPlayerToCorrectBfGroup, which on WG entry then
    // refuses to add the invitee because they are already in a BF group.
    if (Group* invitingGroup = invitingPlayer->GetGroup(); invitingGroup && invitingGroup->isBFGroup())
    {
        SendPartyResult(PARTY_OP_INVITE, membername, ERR_NOT_LEADER);
        return;
    }

    Group* group = invitingPlayer->GetGroup();
    if (group && group->isBGGroup())
        group = invitingPlayer->GetOriginalGroup();
    if (!group)
        group = invitingPlayer->GetGroupInvite();

    Group* group2 = invitedPlayer->GetGroup();
    if (group2 && group2->isBGGroup())
        group2 = invitedPlayer->GetOriginalGroup();
    // player already in another group or invited
    if (group2 || invitedPlayer->GetGroupInvite())
    {
        SendPartyResult(PARTY_OP_INVITE, membername, ERR_ALREADY_IN_GROUP_S);

        if (group2)
        {
            // tell the player that they were invited but it failed as they were already in a group
            SendPartyInvite(invitingPlayer, invitedPlayer, packet.ProposedRoles, false);
        }

        return;
    }

    if (group)
    {
        // not have permissions for invite
        if (!group->IsLeader(invitingPlayer->GetGUID()) && !group->IsAssistant(invitingPlayer->GetGUID()))
        {
            SendPartyResult(PARTY_OP_INVITE, "", ERR_NOT_LEADER);
            return;
        }
        // not have place
        if (group->IsFull())
        {
            SendPartyResult(PARTY_OP_INVITE, "", ERR_GROUP_FULL);
            return;
        }
    }

    // xinef: if player has no group, check group invite
    if (!group && invitingPlayer->GetGroupInvite() && invitingPlayer->GetGroupInvite()->GetLeaderGUID() == invitingPlayer->GetGUID())
        group = invitingPlayer->GetGroupInvite();

    // ok, but group not exist, start a new group
    // but don't create and save the group to the DB until
    // at least one person joins
    if (!group)
    {
        group = new Group();
        // new group: if can't add then delete
        if (!group->AddLeaderInvite(invitingPlayer))
        {
            delete group;
            return;
        }
        if (!group->AddInvite(invitedPlayer))
        {
            delete group;
            return;
        }
    }
    else
    {
        // already existed group: if can't add then just leave
        if (!group->AddInvite(invitedPlayer))
        {
            return;
        }
    }

    // ok, we do it
    SendPartyInvite(invitingPlayer, invitedPlayer, packet.ProposedRoles, true);

    SendPartyResult(PARTY_OP_INVITE, membername, ERR_PARTY_RESULT_OK);
}

// 4.3.4 answers an invite with one opcode carrying the accept bit (3.3.5a CMSG_GROUP_ACCEPT / CMSG_GROUP_DECLINE).
void WorldSession::HandlePartyInviteResponseOpcode(WorldPackets::Party::PartyInviteResponse& packet)
{
    if (packet.Accept)
        AcceptPartyInvite();
    else
        DeclinePartyInvite();
}

void WorldSession::AcceptPartyInvite()
{
    Group* group = GetPlayer()->GetGroupInvite();

    if (!group)
        return;

    // Remove player from invitees in any case
    group->RemoveInvite(GetPlayer());

    if (GetPlayer()->IsSpectator())
    {
        SendPartyResult(PARTY_OP_INVITE, "", ERR_INVITE_RESTRICTED);
        return;
    }

    if (!sScriptMgr->OnPlayerCanGroupAccept(GetPlayer(), group))
        return;

    Player* leader = ObjectAccessor::FindConnectedPlayer(group->GetLeaderGUID());

    // Trial accounts cannot join a group whose existing members are above the trial level cap.
    if (sWorld->getBoolConfig(CONFIG_TRIAL_RESTRICTION_PARTY) && IsTrialAccount())
    {
        if (uint32 trialLevelCap = sWorld->getIntConfig(CONFIG_TRIAL_LEVEL_CAP))
        {
            uint8 leaderLevel = leader ? leader->GetLevel() : sCharacterCache->GetCharacterLevelByGuid(group->GetLeaderGUID());
            if (leaderLevel > trialLevelCap)
            {
                SendPartyResult(PARTY_OP_INVITE, "", ERR_INVITE_RESTRICTED);
                return;
            }

            for (auto const& slot : group->GetMemberSlots())
            {
                if (slot.guid == GetPlayer()->GetGUID())
                    continue;

                uint8 memberLevel = 0;
                if (Player* member = ObjectAccessor::FindConnectedPlayer(slot.guid))
                    memberLevel = member->GetLevel();
                else
                    memberLevel = sCharacterCache->GetCharacterLevelByGuid(slot.guid);

                if (memberLevel > trialLevelCap)
                {
                    SendPartyResult(PARTY_OP_INVITE, "", ERR_INVITE_RESTRICTED);
                    return;
                }
            }
        }
    }

    if (group->GetLeaderGUID() == GetPlayer()->GetGUID())
    {
        LOG_ERROR("network.opcode", "HandleGroupAcceptOpcode: player {} ({}) tried to accept an invite to his own group",
            GetPlayer()->GetName(), GetPlayer()->GetGUID().ToString());
        return;
    }

    // Group is full
    if (group->IsFull())
    {
        SendPartyResult(PARTY_OP_INVITE, "", ERR_GROUP_FULL);
        return;
    }

    // Forming a new group, create it
    if (!group->IsCreated())
    {
        // This can happen if the leader is zoning. To be removed once delayed actions for zoning are implemented
        if (!leader || leader->IsSpectator())
        {
            group->RemoveAllInvites();
            return;
        }

        // If we're about to create a group there really should be a leader present
        ASSERT(leader);
        group->RemoveInvite(leader);
        group->Create(leader);
        sGroupMgr->AddGroup(group);
    }

    // Everything is fine, do it, PLAYER'S GROUP IS SET IN ADDMEMBER!!!
    if (!group->AddMember(GetPlayer()))
        return;

    group->BroadcastGroupUpdate();
}

void WorldSession::DeclinePartyInvite()
{
    Group* group = GetPlayer()->GetGroupInvite();
    if (!group)
        return;

    // Remember leader if online (group pointer will be invalid if group gets disbanded)
    Player* leader = ObjectAccessor::FindConnectedPlayer(group->GetLeaderGUID());

    // uninvite, group can be deleted
    GetPlayer()->UninviteFromGroup();

    if (!leader)
        return;

    // report
    WorldPackets::Party::GroupDecline decline(GetPlayer()->GetName());
    leader->SendDirectMessage(decline.Write());
}

void WorldSession::HandleGroupUninviteGuidOpcode(WorldPacket& recvData)
{
    ObjectGuid guid;
    std::string reason, name;
    recvData >> guid;
    recvData >> reason;

    //can't uninvite yourself
    if (guid == GetPlayer()->GetGUID())
    {
        LOG_ERROR("network.opcode", "WorldSession::HandleGroupUninviteGuidOpcode: leader {} ({}) tried to uninvite himself from the group.",
            GetPlayer()->GetName(), GetPlayer()->GetGUID().ToString());
        return;
    }

    sCharacterCache->GetCharacterNameByGuid(guid, name);

    PartyResult res = GetPlayer()->CanUninviteFromGroup(guid);
    if (res != ERR_PARTY_RESULT_OK)
    {
        if (res == ERR_PARTY_LFG_BOOT_NOT_ELIGIBLE_S)
        {
            if (Player* kickTarget = ObjectAccessor::FindConnectedPlayer(guid))
            {
                if (Aura* dungeonCooldownAura = kickTarget->GetAura(lfg::LFG_SPELL_DUNGEON_COOLDOWN))
                {
                    int32 elapsedTime = dungeonCooldownAura->GetMaxDuration() - dungeonCooldownAura->GetDuration();
                    if (static_cast<int32>(sWorld->getIntConfig(CONFIG_LFG_KICK_PREVENTION_TIMER)) > elapsedTime)
                    {
                        SendPartyResult(PARTY_OP_UNINVITE, name, res, (sWorld->getIntConfig(CONFIG_LFG_KICK_PREVENTION_TIMER) - elapsedTime) / 1000);
                    }
                }
            }
        } else
        {
            SendPartyResult(PARTY_OP_UNINVITE, name, res);
        }

        return;
    }

    Group* grp = GetPlayer()->GetGroup();
    if (!grp)
        return;

    // Xinef: do not allow to kick with empty reason, this will resend packet with given reason
    if (grp->isLFGGroup(true) && reason.empty())
    {
        SendPartyResult(PARTY_OP_UNINVITE, name, ERR_VOTE_KICK_REASON_NEEDED);
        return;
    }

    if (grp->IsLeader(guid) && !grp->isLFGGroup(true))
    {
        SendPartyResult(PARTY_OP_UNINVITE, name, ERR_NOT_LEADER);
        return;
    }

    if (grp->IsMember(guid))
    {
        Player::RemoveFromGroup(grp, guid, GROUP_REMOVEMETHOD_KICK, GetPlayer()->GetGUID(), reason.c_str());
        return;
    }

    if (Player* player = grp->GetInvited(guid))
    {
        player->UninviteFromGroup();
        return;
    }

    SendPartyResult(PARTY_OP_UNINVITE, name, ERR_TARGET_NOT_IN_GROUP_S);
}

void WorldSession::HandleGroupUninviteOpcode(WorldPacket& recvData)
{
    std::string membername;
    recvData >> membername;

    // player not found
    if (!normalizePlayerName(membername))
        return;

    // can't uninvite yourself
    if (GetPlayer()->GetName() == membername)
    {
        LOG_ERROR("network.opcode", "WorldSession::HandleGroupUninviteOpcode: leader {} ({}) tried to uninvite himself from the group.",
            GetPlayer()->GetName(), GetPlayer()->GetGUID().ToString());
        return;
    }

    Group* grp = GetPlayer()->GetGroup();
    if (!grp)
    {
        return;
    }

    PartyResult res = GetPlayer()->CanUninviteFromGroup(grp->GetMemberGUID(membername));
    if (res != ERR_PARTY_RESULT_OK)
    {
        SendPartyResult(PARTY_OP_UNINVITE, "", res);
        return;
    }

    if (ObjectGuid guid = grp->GetMemberGUID(membername))
    {
        Player::RemoveFromGroup(grp, guid, GROUP_REMOVEMETHOD_KICK, GetPlayer()->GetGUID());
        return;
    }

    if (Player* player = grp->GetInvited(membername))
    {
        player->UninviteFromGroup();
        return;
    }

    SendPartyResult(PARTY_OP_UNINVITE, membername, ERR_TARGET_NOT_IN_GROUP_S);
}

void WorldSession::HandleGroupSetLeaderOpcode(WorldPacket& recvData)
{
    ObjectGuid guid;
    recvData >> guid;

    Player* player = ObjectAccessor::FindConnectedPlayer(guid);
    Group* group = GetPlayer()->GetGroup();

    if (!group || !player)
        return;

    if (!group->IsLeader(GetPlayer()->GetGUID()) || player->GetGroup() != group || guid == GetPlayer()->GetGUID())
        return;

    // Everything's fine, accepted.
    group->ChangeLeader(guid);
    group->SendUpdate();
}

void WorldSession::HandleGroupDisbandOpcode(WorldPacket& /*recvData*/)
{
    Group* grp = GetPlayer()->GetGroup();
    Group* grpInvite = GetPlayer()->GetGroupInvite();
    if (!grp && !grpInvite)
        return;

    if (_player->InBattleground()) // Do not leave group, give no error. Verified on TBC Classic
        return;

    /** error handling **/
    /********************/

    // everything's fine, do it
    if (grp)
    {
        SendPartyResult(PARTY_OP_LEAVE, GetPlayer()->GetName(), ERR_PARTY_RESULT_OK);
        GetPlayer()->RemoveFromGroup(GROUP_REMOVEMETHOD_LEAVE);
    }
    else if (grpInvite && grpInvite->GetLeaderGUID() == GetPlayer()->GetGUID())
    { // pending group creation being cancelled
        SendPartyResult(PARTY_OP_LEAVE, GetPlayer()->GetName(), ERR_PARTY_RESULT_OK);
        grpInvite->Disband();
    }
}

void WorldSession::HandleLootMethodOpcode(WorldPacket& recvData)
{
    uint32 lootMethod;
    ObjectGuid lootMaster;
    uint32 lootThreshold;
    recvData >> lootMethod >> lootMaster >> lootThreshold;

    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    /** error handling **/
    // Xinef: Check if group is LFG
    if (!group->IsLeader(GetPlayer()->GetGUID()) || group->isLFGGroup(true))
        return;

    if (lootMethod > NEED_BEFORE_GREED)
        return;

    if (lootThreshold < ITEM_QUALITY_UNCOMMON || lootThreshold > ITEM_QUALITY_ARTIFACT)
        return;

    if (lootMethod == MASTER_LOOT && !group->IsMember(lootMaster))
        return;
    /********************/

    // everything's fine, do it
    group->SetLootMethod((LootMethod)lootMethod);
    group->SetMasterLooterGuid(lootMaster);
    group->SetLootThreshold((ItemQualities)lootThreshold);
    group->SendUpdate();
}

void WorldSession::HandleLootRoll(WorldPacket& recvData)
{
    ObjectGuid guid;
    uint32 itemSlot;
    uint8  rollType;
    recvData >> guid;                  // guid of the item rolled
    recvData >> itemSlot;
    recvData >> rollType;              // 0: pass, 1: need, 2: greed

    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    if (GetPlayer()->HasPlayerFlag(PLAYER_FLAGS_NO_PLAY_TIME))
    {
        SendPlayTimeWarning(PTF_UNHEALTHY_TIME, 0);
        rollType = ROLL_PASS;
    }

    group->CountRollVote(GetPlayer()->GetGUID(), guid, rollType);

    switch (rollType)
    {
        case ROLL_NEED:
            GetPlayer()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_ROLL_NEED, 1);
            break;
        case ROLL_GREED:
            GetPlayer()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_ROLL_GREED, 1);
            break;
        case ROLL_DISENCHANT:
            GetPlayer()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_ROLL_DISENCHANT, 1);
            break;
    }
}

void WorldSession::HandleMinimapPingOpcode(WorldPackets::Misc::MinimapPingClient& packet)
{
    if (!sMapMgr->IsValidMapCoord(GetPlayer()->GetMap()->GetId(), packet.MapX, packet.MapY))
        return;

    Group* group = GetPlayer()->GetGroup();

    if (!group)
        return;

    group->DoMinimapPing(GetPlayer()->GetGUID(), packet.MapX, packet.MapY);
}

void WorldSession::HandleRandomRollOpcode(WorldPackets::Misc::RandomRollClient& packet)
{
    uint32 minimum, maximum;
    minimum = packet.Min;
    maximum = packet.Max;

    /** error handling **/
    if (minimum > maximum || maximum > sWorld->getIntConfig(CONFIG_RANDOM_ROLL_MAXIMUM))
        return;

    GetPlayer()->DoRandomRoll(minimum, maximum);
}

void WorldSession::HandleRaidTargetUpdateOpcode(WorldPacket& recvData)
{
    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    uint8  x;
    recvData >> x;

    /** error handling **/
    /********************/

    // everything's fine, do it
    if (x == 0xFF)                                           // target icon request
    {
        group->SendTargetIconList(this);
    }
    else                                                    // target icon update
    {
        if (group->isRaidGroup() && !group->IsLeader(GetPlayer()->GetGUID()) && !group->IsAssistant(GetPlayer()->GetGUID()))
            return;

        ObjectGuid guid;
        recvData >> guid;

        if (guid.IsPlayer())
        {
            Player* target = ObjectAccessor::FindConnectedPlayer(guid);

            if (!target || target->IsHostileTo(GetPlayer()))
                return;
        }

        group->SetTargetIcon(x, _player->GetGUID(), guid);
    }
}

void WorldSession::HandleGroupRaidConvertOpcode(WorldPacket& recvData)
{
    // 4.x: the client says which way to convert; a raid of five or fewer can become a party again.
    bool toRaid = recvData.read<uint8>() != 0;

    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    if (_player->InBattleground())
        return;

    /** error handling **/
    if (toRaid && group->CheckLevelForRaid())
    {
        SendPartyResult(PARTY_OP_INVITE, "", ERR_RAID_DISALLOWED_BY_LEVEL);
        return;
    }

    if (!group->IsLeader(GetPlayer()->GetGUID()) || group->GetMembersCount() < 2 || group->isLFGGroup()) // pussywizard: not allowed for lfg groups, it is either raid from the beginning or not!
        return;
    /********************/

    // everything's fine, do it (is it 0 (PARTY_OP_INVITE) correct code)
    SendPartyResult(PARTY_OP_INVITE, "", ERR_PARTY_RESULT_OK);
    if (toRaid)
        group->ConvertToRaid();
    else
        group->ConvertToGroup();
}

void WorldSession::HandleGroupChangeSubGroupOpcode(WorldPacket& recvData)
{
    // we will get correct pointer for group here, so we don't have to check if group is BG raid
    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    std::string name;
    uint8 groupNr;
    recvData >> name;
    recvData >> groupNr;

    if (groupNr >= MAX_RAID_SUBGROUPS)
        return;

    ObjectGuid senderGuid = GetPlayer()->GetGUID();
    if (!group->IsLeader(senderGuid) && !group->IsAssistant(senderGuid))
        return;

    if (!group->HasFreeSlotSubGroup(groupNr))
        return;

    Player* movedPlayer = ObjectAccessor::FindPlayerByName(name, false);
    ObjectGuid guid;
    if (movedPlayer)
    {
        guid = movedPlayer->GetGUID();
    }
    else
    {
        CharacterDatabase.EscapeString(name);
        guid = sCharacterCache->GetCharacterGuidByName(name);
    }

    group->ChangeMembersGroup(guid, groupNr);
}

void WorldSession::HandleGroupAssistantLeaderOpcode(WorldPacket& recvData)
{
    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    if (!group->IsLeader(GetPlayer()->GetGUID()))
        return;

    ObjectGuid guid;
    bool apply;
    recvData >> guid;
    recvData >> apply;

    group->SetGroupMemberFlag(guid, apply, MEMBER_FLAG_ASSISTANT);

    group->SendUpdate();
}

void WorldSession::HandlePartyAssignmentOpcode(WorldPacket& recvData)
{
    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    ObjectGuid senderGuid = GetPlayer()->GetGUID();
    if (!group->IsLeader(senderGuid) && !group->IsAssistant(senderGuid))
        return;

    uint8 assignment;
    bool apply;
    ObjectGuid guid;
    recvData >> assignment >> apply;
    recvData >> guid;

    switch (assignment)
    {
        case GROUP_ASSIGN_MAINASSIST:
            group->RemoveUniqueGroupMemberFlag(MEMBER_FLAG_MAINASSIST);
            group->SetGroupMemberFlag(guid, apply, MEMBER_FLAG_MAINASSIST);
            break;
        case GROUP_ASSIGN_MAINTANK:
            group->RemoveUniqueGroupMemberFlag(MEMBER_FLAG_MAINTANK);           // Remove main assist flag from current if any.
            group->SetGroupMemberFlag(guid, apply, MEMBER_FLAG_MAINTANK);
        default:
            break;
    }

    group->SendUpdate();
}

void WorldSession::HandleRaidReadyCheckOpcode(WorldPacket& recvData)
{
    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    if (recvData.empty())                                   // request
    {
        /** error handling **/
        if (!group->IsLeader(GetPlayer()->GetGUID()) && !group->IsAssistant(GetPlayer()->GetGUID()))
            return;
        /********************/

        // Check if Ready Check in BG is enabled
        if (sWorld->getBoolConfig(CONFIG_BATTLEGROUND_DISABLE_READY_CHECK_IN_BG))
        {
            // Check if player is in BG
            if (_player->InBattleground())
            {
                ChatHandler(_player->GetSession()).SendNotification(LANG_BG_READY_CHECK_ERROR);
                return;
            }
        }

        // everything's fine, do it
        WorldPacket data(MSG_RAID_READY_CHECK, 8);
        data << GetPlayer()->GetGUID();
        group->BroadcastPacket(&data, false, -1);

        group->OfflineReadyCheck();
    }
    else                                                    // answer
    {
        uint8 state;
        recvData >> state;

        // everything's fine, do it
        WorldPacket data(MSG_RAID_READY_CHECK_CONFIRM, 9);
        data << GetPlayer()->GetGUID();
        data << uint8(state);
        group->BroadcastReadyCheck(&data);
    }
}

void WorldSession::HandleRaidReadyCheckFinishedOpcode(WorldPacket& /*recvData*/)
{
    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    if (!group->IsLeader(GetPlayer()->GetGUID()) && !group->IsAssistant(GetPlayer()->GetGUID()))
        return;

    WorldPacket data(MSG_RAID_READY_CHECK_FINISHED);
    group->BroadcastPacket(&data, true, -1);
}

/*this procedure handles clients CMSG_REQUEST_PARTY_MEMBER_STATS request*/
void WorldSession::HandleRequestPartyMemberStatsOpcode(WorldPacket& recvData)
{
    ObjectGuid Guid;
    recvData >> Guid;

    WorldPackets::Party::PartyMemberState packet(true);
    Player* player = HashMapHolder<Player>::Find(Guid);
    if (!player || !player->IsInSameRaidWith(_player))
    {
        packet.MemberGuid = Guid;
        packet.ChangeMask = GROUP_UPDATE_FLAG_STATUS;
        packet.MemberStats.Status = MEMBER_STATUS_OFFLINE;
    }
    else
        packet.Initialize(player);

    SendPacket(packet.Write());
}

void WorldSession::HandleRequestRaidInfoOpcode(WorldPacket& /*recvData*/)
{
    // every time the player checks the character screen
    _player->SendRaidInfo();
}

void WorldSession::HandleOptOutOfLootOpcode(WorldPacket& recvData)
{
    bool passOnLoot = recvData.read<uint8>() != 0; // 1 always pass, 0 do not pass (4.3.4 sends one byte)

    if (!GetPlayer())
        return;

    GetPlayer()->SetPassOnGroupLoot(passOnLoot);
}

void WorldSession::HandleGroupSwapSubGroupOpcode(WorldPacket& recv_data)
{
    std::string playerName1, playerName2;

    // first = moved from, second = moved to
    recv_data >> playerName1;
    recv_data >> playerName2;

    if (!normalizePlayerName(playerName1))
    {
        SendPartyResult(PARTY_OP_SWAP, playerName1, ERR_GROUP_SWAP_FAILED);
        return;
    }
    if (!normalizePlayerName(playerName2))
    {
        SendPartyResult(PARTY_OP_SWAP, playerName1, ERR_GROUP_SWAP_FAILED);
        return;
    }

    Group* group = GetPlayer()->GetGroup();
    if (!group || !group->isRaidGroup())
    {
        return;
    }

    if (!group->IsLeader(GetPlayer()->GetGUID()) && !group->IsAssistant(GetPlayer()->GetGUID()))
    {
        return;
    }

    //get guid, member may be offline
    auto getGuid = [&group](std::string const& playerName)
    {
        // no player, cheating?
        if (!group->GetMemberGUID(playerName))
        {
            return ObjectGuid::Empty;
        }

        if (Player* player = ObjectAccessor::FindPlayerByName(playerName.c_str()))
        {
            return player->GetGUID();
        }
        else
        {
            if (ObjectGuid guid = sCharacterCache->GetCharacterGuidByName(playerName))
            {
                return guid;
            }
            else
            {
                return ObjectGuid::Empty; // no player - again, cheating?
            }
        }
    };

    ObjectGuid guid1 = getGuid(playerName1);
    ObjectGuid guid2 = getGuid(playerName2);

    if (!guid1 || !guid2)
    {
        SendPartyResult(PARTY_OP_SWAP, playerName1, ERR_GROUP_SWAP_FAILED);
        return;
    }

    uint8 groupId1 = group->GetMemberGroup(guid1);
    uint8 groupId2 = group->GetMemberGroup(guid2);

    if (groupId1 == MAX_RAID_SUBGROUPS + 1 || groupId2 == MAX_RAID_SUBGROUPS + 1)
    {
        return;
    }

    if (groupId1 == groupId2)
    {
        return;
    }

    group->ChangeMembersGroup(guid1, groupId2);
    group->ChangeMembersGroup(guid2, groupId1);
}

void WorldSession::HandleSetEveryoneIsAssistant(WorldPackets::Party::SetEveryoneIsAssistant& packet)
{
    Group* group = GetPlayer()->GetGroup();
    if (!group || !group->IsLeader(GetPlayer()->GetGUID()))
        return;

    group->SetEveryoneIsAssistant(packet.EveryoneIsAssistant);
}

// The party frame role icons (tank, healer, damage); the Dungeon Finder role check has its own opcodes.
void WorldSession::HandleSetRoleOpcode(WorldPackets::Party::SetRole& packet)
{
    Group* group = GetPlayer()->GetGroup();
    if (group && group->isLFGGroup())
        return;

    ObjectGuid const senderGuid = GetPlayer()->GetGUID();
    bool canAssign = group && (group->IsLeader(senderGuid) || group->IsAssistant(senderGuid));
    if (packet.ChangedUnit != senderGuid && !canAssign)
        return;

    if (group && !group->IsMember(packet.ChangedUnit))
        return;

    uint8 oldRole = group ? group->GetLfgRoles(packet.ChangedUnit) : 0;
    if (oldRole == packet.Role)
        return;

    WorldPackets::Party::RoleChangedInform roleChangedInform;
    roleChangedInform.From = senderGuid;
    roleChangedInform.ChangedUnit = packet.ChangedUnit;
    roleChangedInform.OldRole = oldRole;
    roleChangedInform.NewRole = packet.Role;

    if (group)
    {
        group->BroadcastPacket(roleChangedInform.Write(), false);
        group->SetLfgRoles(packet.ChangedUnit, uint8(packet.Role));
    }
    else
        SendPacket(roleChangedInform.Write());
}

void WorldSession::HandleRolePollBeginOpcode(WorldPacket& recvData)
{
    // CPP answers only the empty request; a trailing party index is ignored.
    recvData.rfinish();

    Group* group = GetPlayer()->GetGroup();
    if (!group || group->isLFGGroup())
        return;

    if (!group->IsLeader(GetPlayer()->GetGUID()) && !group->IsAssistant(GetPlayer()->GetGUID()))
        return;

    WorldPackets::Party::RolePollBegin rolePollBegin;
    rolePollBegin.From = GetPlayer()->GetGUID();
    group->BroadcastPacket(rolePollBegin.Write(), true);
}

void WorldSession::HandleGroupRequestJoinUpdates(WorldPacket& /*recvData*/)
{
    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    WorldPacket data(SMSG_REAL_GROUP_UPDATE, 1 + 4 + 8);
    data << uint8(group->GetGroupType());
    data << uint32(group->GetMembersCount() - 1);
    data << group->GetLeaderGUID();
    SendPacket(&data);
}
