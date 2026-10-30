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

#include "Log.h"
#include "Opcodes.h"
#include "Player.h"
#include "SpellMgr.h"
#include "WorldPacket.h"
#include "WorldSession.h"

void WorldSession::HandleLearnTalentOpcode(WorldPacket& recvData)
{
    uint32 talent_id, requested_rank;
    recvData >> talent_id >> requested_rank;

    if (_player->LearnTalent(talent_id, requested_rank))
        _player->SendTalentsInfoData(false);
}

void WorldSession::HandleLearnPreviewTalents(WorldPacket& recvPacket)
{
    LOG_DEBUG("network", "CMSG_LEARN_PREVIEW_TALENTS");

    // 4.3.4 (CPP LearnPreviewTalents): int32 primary tree tab index (-1 when it is already chosen),
    // uint32 count, then count x (uint32 talent id, uint32 rank)
    int32 talentTab;
    uint32 talentsCount;
    recvPacket >> talentTab >> talentsCount;

    // Client has max 44 talents for tree for 3 trees, rounded up : 150
    uint32 const MaxTalentsCount = 150;
    if (talentsCount > MaxTalentsCount)
    {
        recvPacket.rfinish();
        return;
    }

    // choosing the primary talent tree (specialization) of the active spec, as CMSG_SET_PRIMARY_TALENT_TREE did
    if (talentTab >= 0 && !_player->LearnPrimaryTalentSpecialization(uint8(talentTab)))
    {
        recvPacket.rfinish();
        _player->SendTalentsInfoData(false);
        return;
    }

    for (uint32 i = 0; i < talentsCount; ++i)
    {
        uint32 talentId, talentRank;
        recvPacket >> talentId >> talentRank;

        if (!_player->LearnTalent(talentId, talentRank))
            break;
    }

    _player->SendTalentsInfoData(false);

    recvPacket.rfinish();
}

void WorldSession::HandleTalentWipeConfirmOpcode(WorldPacket& recvData)
{
    LOG_DEBUG("network", "MSG_TALENT_WIPE_CONFIRM");
    ObjectGuid guid;
    recvData >> guid;

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_TRAINER);
    if (!unit)
    {
        LOG_DEBUG("network", "WORLD: HandleTalentWipeConfirmOpcode - Unit ({}) not found or you can't interact with him.", guid.ToString());
        return;
    }

    if (!unit->CanResetTalents(_player))
        return;

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    if (!(_player->resetTalents()))
    {
        WorldPacket data(MSG_TALENT_WIPE_CONFIRM, 8 + 4);  //you have not any talent
        data << uint64(0);
        data << uint32(0);
        SendPacket(&data);
        return;
    }

    _player->SendTalentsInfoData(false);
    unit->CastSpell(_player, 14867, true);                  //spell: "Untalent Visual Effect"
}

void WorldSession::HandleUnlearnSkillOpcode(WorldPacket& recvData)
{
    uint32 skillId;
    recvData >> skillId;

    if (!IsPrimaryProfessionSkill(skillId))
        return;

    GetPlayer()->SetSkill(skillId, 0, 0, 0);
}
