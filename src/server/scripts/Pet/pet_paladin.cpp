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

/*
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "npc_pet_pal_".
 */

#include "CreatureScript.h"
#include "PetDefines.h"
#include "Player.h"
#include "ScriptedCreature.h"

enum GuardianOfAncientKings
{
    SPELL_PALADIN_ANCIENT_GUARDIAN              = 86657,

    NPC_GUARDIAN_OF_ANCIENT_KINGS_PROTECTION    = 46490,
    NPC_GUARDIAN_OF_ANCIENT_KINGS_RETRIBUTION   = 46506
};

// 46490, 46499, 46506 - Guardian of Ancient Kings (CPP npc_pet_pal_guardian_of_ancient_kings)
// The protection guardian stands still and shields the paladin, the holy one follows and heals through
// the paladin's Ancient Healer; the retribution one fights the paladin's target.
struct npc_pet_pal_guardian_of_ancient_kings : public ScriptedAI
{
    npc_pet_pal_guardian_of_ancient_kings(Creature* creature) : ScriptedAI(creature) { }

    void InitializeAI() override
    {
        me->SetReactState(IsRetribution() ? REACT_DEFENSIVE : REACT_PASSIVE);
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (me->GetEntry() != NPC_GUARDIAN_OF_ANCIENT_KINGS_PROTECTION)
            return;

        if (Unit* owner = summoner->ToUnit())
        {
            me->AddUnitState(UNIT_STATE_ROOT);
            DoCast(owner, SPELL_PALADIN_ANCIENT_GUARDIAN);
        }
    }

    void UpdateAI(uint32 /*diff*/) override
    {
        if (!IsRetribution())
            return;

        Player* owner = me->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!owner)
            return;

        // a player's summon is only engaged once it has hit, so the victim is kept here as PetAI does,
        // not through UpdateVictim
        Unit* victim = me->GetVictim();
        if (victim && (!victim->IsAlive() || !me->CanCreatureAttack(victim)))
        {
            me->AttackStop();
            victim = nullptr;
        }

        // follow the paladin's fight: its victim, or what it has targeted (when it summoned the guardian)
        if (!victim)
        {
            Unit* target = owner->GetVictim();
            if (!target)
                target = owner->GetSelectedUnit();

            if (target && target->IsAlive() && me->CanCreatureAttack(target))
            {
                AttackStart(target);
                victim = target;
            }
            else if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != FOLLOW_MOTION_TYPE)
                me->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, me->GetFollowAngle());
        }

        if (victim)
            DoMeleeAttackIfReady();
    }

private:
    bool IsRetribution() const { return me->GetEntry() == NPC_GUARDIAN_OF_ANCIENT_KINGS_RETRIBUTION; }
};

void AddSC_paladin_pet_scripts()
{
    RegisterCreatureAI(npc_pet_pal_guardian_of_ancient_kings);
}
