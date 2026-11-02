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
 * Scriptnames of files in this file should be prefixed with "npc_pet_pri_".
 */

#include "CreatureScript.h"
#include "ObjectAccessor.h"
#include "PetAI.h"
#include "ScriptedCreature.h"
#include "TotemAI.h"

enum PriestSpells
{
    SPELL_PRIEST_LIGHTWELL_CHARGES          = 59907,
    SPELL_PRIEST_SHADOWY_APPARITION_CLONE   = 87213,   // Summon Shadowy Apparition Spawn Effect: the priest's look
    SPELL_PRIEST_SHADOWY_APPARITION_VISUAL  = 87427,
    SPELL_PRIEST_SHADOWY_APPARITION_DEATH   = 87529,
    SPELL_PRIEST_SHADOWY_APPARITION_DAMAGE  = 87532,
};

struct npc_pet_pri_lightwell : public TotemAI
{
    npc_pet_pri_lightwell(Creature* creature) : TotemAI(creature) { }

    void InitializeAI() override
    {
        if (TempSummon* tempSummon = me->ToTempSummon())
        {
            if (Unit* owner = tempSummon->GetSummonerUnit())
            {
                uint32 hp = uint32(owner->GetMaxHealth() * 0.3f);
                me->SetMaxHealth(hp);
                me->SetHealth(hp);
                me->SetLevel(owner->GetLevel());
            }
        }

        me->CastSpell(me, SPELL_PRIEST_LIGHTWELL_CHARGES, false); // Spell for Lightwell Charges
        TotemAI::InitializeAI();
    }
};

// 46954 - Shadowy Apparition (ours): walks to the Shadow Word: Pain target spell_pri_shadowy_apparition gives it,
// cannot be targeted, and on touching the target hits it once with the priest's spell power, then fades
struct npc_pet_pri_shadowy_apparition : public ScriptedAI
{
    npc_pet_pri_shadowy_apparition(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(WorldObject* summoner) override
    {
        me->SetReactState(REACT_PASSIVE);
        me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        if (Unit* priest = summoner->ToUnit())
            priest->CastSpell(me, SPELL_PRIEST_SHADOWY_APPARITION_CLONE, true);
        DoCastSelf(SPELL_PRIEST_SHADOWY_APPARITION_VISUAL, true);
    }

    void SetGUID(ObjectGuid const& guid, int32 /*id*/) override
    {
        _targetGUID = guid;
    }

    void UpdateAI(uint32 diff) override
    {
        if (_done)
            return;

        Unit* target = ObjectAccessor::GetUnit(*me, _targetGUID);
        if (!target || !target->IsAlive())
        {
            Fade();
            return;
        }

        // it runs into the target: the hit comes when it reaches the target's body, not its melee range
        if (me->GetExactDist2d(target) <= std::max(target->GetObjectSize() * 0.5f, 0.5f))
        {
            // its own cast (87532 is a melee-range spell), with the priest's spell power and crit
            me->CastSpell(target, SPELL_PRIEST_SHADOWY_APPARITION_DAMAGE, true, nullptr, nullptr, me->GetOwnerGUID());
            Fade();
            return;
        }

        // "slowly move towards" the target: it walks, following the target's moves
        if (_moveTimer <= diff)
        {
            me->GetMotionMaster()->MovePoint(0, target->GetPosition(), FORCED_MOVEMENT_WALK);
            _moveTimer = 500;
        }
        else
            _moveTimer -= diff;
    }

private:
    void Fade()
    {
        _done = true;
        me->StopMoving();
        DoCastSelf(SPELL_PRIEST_SHADOWY_APPARITION_DEATH, true);
        me->DespawnOrUnsummon(1s);
    }

    ObjectGuid _targetGUID;
    uint32 _moveTimer = 0;
    bool _done = false;
};

void AddSC_priest_pet_scripts()
{
    RegisterCreatureAI(npc_pet_pri_lightwell);
    RegisterCreatureAI(npc_pet_pri_shadowy_apparition);
}
