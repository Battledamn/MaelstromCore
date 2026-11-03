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
 * Scriptnames of files in this file should be prefixed with "npc_pet_mag_".
 */

#include "CombatAI.h"
#include "CreatureScript.h"
#include "MoveSplineInit.h"
#include "Pet.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "TemporarySummon.h"

enum MageSpells
{
    SPELL_MAGE_CLONE_ME                 = 45204,
    SPELL_MAGE_MASTERS_THREAT_LIST      = 58838,
    SPELL_PET_HIT_SCALING               = 61013,
    // the three images' summons: frost (58832), arcane (88091) and fire (88092, both with the Glyph of Mirror Image)
    SPELL_SUMMON_MIRROR_IMAGE1          = 58831,
    SPELL_SUMMON_MIRROR_IMAGE2          = 58833,
    SPELL_SUMMON_MIRROR_IMAGE3          = 58834,
    SPELL_SUMMON_ARCANE_IMAGE1          = 88085,
    SPELL_SUMMON_ARCANE_IMAGE2          = 88087,
    SPELL_SUMMON_ARCANE_IMAGE3          = 88089,
    SPELL_SUMMON_FIRE_IMAGE1            = 88086,
    SPELL_SUMMON_FIRE_IMAGE2            = 88088,
    SPELL_SUMMON_FIRE_IMAGE3            = 88090,
    SPELL_MAGE_MIRROR_IMAGE_FROSTBOLT   = 59638,
    SPELL_MAGE_MIRROR_IMAGE_FIREBALL    = 88082,
    SPELL_MAGE_MIRROR_IMAGE_ARCANE_BLAST = 88084,

    // Flame Orb, Frostfire Orb
    SPELL_FLAME_ORB_AURA                = 82690,
    SPELL_FROSTFIRE_ORB_AURA            = 84717,
    SPELL_FIRE_POWER_EXPLOSION          = 83619,
    SPELL_FIRE_POWER_R1                 = 18459
};

enum MageCreatures
{
    NPC_MIRROR_IMAGE_FROST              = 31216,
    NPC_MIRROR_IMAGE_ARCANE             = 47243,
    NPC_MIRROR_IMAGE_FIRE               = 47244,
    NPC_FLAME_ORB                       = 44214
};

class DeathEvent : public BasicEvent
{
public:
    DeathEvent(Creature& owner) : BasicEvent(), _owner(owner) { }

    bool Execute(uint64 /*eventTime*/, uint32 /*diff*/) override
    {
        Unit::Kill(&_owner, &_owner);
        return true;
    }

private:
    Creature& _owner;
};

struct npc_pet_mage_mirror_image : CasterAI
{
    npc_pet_mage_mirror_image(Creature* creature) : CasterAI(creature) { }

    uint32 selectionTimer;
    ObjectGuid _ebonGargoyleGUID;
    uint32 checktarget;
    uint32 dist = urand(1, 5);
    bool _delayAttack;

    // 4.3.4: each image casts its one spell (CPP); the templates list the WotLK Fire Blast or nothing
    static uint32 ImageSpell(uint32 entry)
    {
        switch (entry)
        {
            case NPC_MIRROR_IMAGE_ARCANE: return SPELL_MAGE_MIRROR_IMAGE_ARCANE_BLAST;
            case NPC_MIRROR_IMAGE_FIRE:   return SPELL_MAGE_MIRROR_IMAGE_FIREBALL;
            default:                      return SPELL_MAGE_MIRROR_IMAGE_FROSTBOLT;
        }
    }

    void AttackStart(Unit* who) override
    {
        AttackStartCaster(who, 20.0f);
    }

    void InitializeAI() override
    {
        CasterAI::InitializeAI();
        spells.assign(1, ImageSpell(me->GetEntry()));

        _delayAttack = true;
        me->m_Events.AddEventAtOffset([this]()
        {
            _delayAttack = false;
        }, 1200ms);

        Unit* owner = me->GetOwner();
        if (!owner)
            return;

        // Clone Me!
        owner->CastSpell(me, SPELL_MAGE_CLONE_ME, true);

        // the images keep the sides the client data summons them on: in front, on the right and on the left
        // (CPP follows at the spawn angle); WotLK's left, behind and right hid one behind the mage
        float angle = 0.0f;
        switch (me->GetUInt32Value(UNIT_CREATED_BY_SPELL))
        {
            case SPELL_SUMMON_MIRROR_IMAGE1:
            case SPELL_SUMMON_ARCANE_IMAGE1:
            case SPELL_SUMMON_FIRE_IMAGE1:
                angle = 0.0f;
                break;
            case SPELL_SUMMON_MIRROR_IMAGE2:
            case SPELL_SUMMON_ARCANE_IMAGE2:
            case SPELL_SUMMON_FIRE_IMAGE2:
                angle = 1.5f * M_PI;
                break;
            case SPELL_SUMMON_MIRROR_IMAGE3:
            case SPELL_SUMMON_ARCANE_IMAGE3:
            case SPELL_SUMMON_FIRE_IMAGE3:
                angle = 0.5f * M_PI;
                break;
        }

        ((Minion*)me)->SetFollowAngle(angle);
        // Spell::SummonGuardian gives every summon after the first the angle to its spawn point, which is the mage's
        // own spot here (0, so two images stood in one): keep this side once the summon is done
        me->m_Events.AddEventAtOffset([this, angle]()
        {
            ((Minion*)me)->SetFollowAngle(angle);
            if (Unit* imageOwner = me->GetOwner())
                if (!me->IsInCombat() && !me->GetVictim())
                    me->GetMotionMaster()->MoveFollow(imageOwner, PET_FOLLOW_DIST, angle, MOTION_SLOT_ACTIVE);
        }, 1ms);
        if (owner->IsInCombat())
            me->NearTeleportTo(me->GetPositionX() + cos(angle)*dist, me->GetPositionY() + std::sin(angle)*dist, me->GetPositionZ(), me->GetOrientation(), false, false, false, false);
        else
            me->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, me->GetFollowAngle(), MOTION_SLOT_ACTIVE);

        me->SetReactState(REACT_DEFENSIVE);

        // Xinef: Inherit Master's Threat List (not yet implemented)
        //owner->CastSpell((Unit*)nullptr, SPELL_MAGE_MASTERS_THREAT_LIST, true);
        for (auto const& pair : owner->GetThreatMgr().GetThreatenedByMeList())
        {
            if (Unit* unit = pair.second->GetOwner())
                unit->GetThreatMgr().AddThreat(me, pair.second->GetThreat());
        }

        _ebonGargoyleGUID.Clear();

        // Xinef: copy caster auras
        Unit::VisibleAuraMap const* visibleAuraMap = owner->GetVisibleAuras();
        for (Unit::VisibleAuraMap::const_iterator itr = visibleAuraMap->begin(); itr != visibleAuraMap->end(); ++itr)
            if (Aura* visAura = itr->second->GetBase())
            {
                // Ebon Gargoyle
                if (visAura->GetId() == 49206 && me->GetUInt32Value(UNIT_CREATED_BY_SPELL) == SPELL_SUMMON_MIRROR_IMAGE1)
                {
                    if (Unit* gargoyle = visAura->GetCaster())
                        _ebonGargoyleGUID = gargoyle->GetGUID();
                    continue;
                }
                SpellScriptsBounds bounds = sObjectMgr->GetSpellScriptsBounds(visAura->GetId());
                if (bounds.first != bounds.second)
                    continue;
                std::vector<int32> const* spellTriggered = sSpellMgr->GetSpellLinked(visAura->GetId() + SPELL_LINK_AURA);
                if (!spellTriggered || !spellTriggered->empty())
                    continue;
                if (Aura* newAura = me->AddAura(visAura->GetId(), me))
                    newAura->SetDuration(visAura->GetDuration());
            }

        me->m_Events.AddEventAtOffset(new DeathEvent(*me), 29500ms);
    }

    // Do not reload Creature templates on evade mode enter - prevent visual lost
    void EnterEvadeMode(EvadeReason /*why*/) override
    {
        if (me->IsInEvadeMode() || !me->IsAlive())
            return;

        Unit* owner = me->GetCharmerOrOwner();

        me->CombatStop(true);
        if (owner && !me->HasUnitState(UNIT_STATE_FOLLOW))
        {
            me->GetMotionMaster()->Clear(false);
            me->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, me->GetFollowAngle(), MOTION_SLOT_ACTIVE);
        }
    }

    void MySelectNextTarget()
    {
        if (_ebonGargoyleGUID)
        {
            Unit* gargoyle = ObjectAccessor::GetUnit(*me, _ebonGargoyleGUID);
            if (gargoyle && gargoyle->GetAI())
                gargoyle->GetAI()->AttackStart(me);
            _ebonGargoyleGUID.Clear();
        }
        Unit* owner = me->GetOwner();
        if (owner && owner->IsPlayer())
        {
            Unit* selection = owner->ToPlayer()->GetSelectedUnit();

            if (selection && me->CanSeeOrDetect(selection))
            {
                me->GetThreatMgr().ResetAllThreat();
                me->AddThreat(selection, 1000000.0f);

                if (owner->IsInCombat())
                    AttackStart(selection);
            }

            if (!owner->IsInCombat() && !me->GetVictim())
                EnterEvadeMode(EVADE_REASON_OTHER);
        }
    }

    void Reset() override
    {
        selectionTimer = 0;
        checktarget = 0;
    }

    void UpdateAI(uint32 diff) override
    {
        if (_delayAttack)
            return;

        events.Update(diff);

        if (!me->IsInCombat() || !me->GetVictim())
        {
            MySelectNextTarget();
            return;
        }

        // A dead target, or one we lost sight of, is invalid: drop it and reselect.
        // CanSeeOrDetect is comparatively expensive, so throttle the sight check to ~1s.
        bool lostTarget = !me->GetVictim()->IsAlive();

        checktarget += diff;
        if (checktarget >= 1000)
        {
            checktarget = 0;
            if (!me->CanSeeOrDetect(me->GetVictim()))
                lostTarget = true;
        }

        if (lostTarget)
        {
            MySelectNextTarget();
            me->InterruptNonMeleeSpells(false);
            return;
        }

        // A cast already in progress when the crowd control lands is allowed to finish (3.1.2),
        // except a Frostbolt on a target that is now Polymorphed: 3.2.0 cancels that cast so it
        // cannot break the Polymorph. Other breakable CC keeps the "let it finish" behaviour.
        if (me->HasUnitState(UNIT_STATE_CASTING))
        {
            if (me->GetVictim()->HasAuraWithMechanic(1ULL << MECHANIC_POLYMORPH)
                && me->FindCurrentSpellBySpellId(SPELL_MAGE_MIRROR_IMAGE_FROSTBOLT))
                me->InterruptNonMeleeSpells(false, SPELL_MAGE_MIRROR_IMAGE_FROSTBOLT);

            return;
        }

        // Never start a new cast on a target under a breakable-by-damage CC aura (Polymorph,
        // Dragon's Breath, ...) - that is what would break the crowd control.
        if (me->GetVictim()->HasBreakableByDamageCrowdControlAura(me))
            return;

        if (uint32 spellId = events.ExecuteEvent())
        {
            events.RescheduleEvent(spellId, 2500ms);
            me->CastSpell(me->GetVictim(), spellId, false);
        }
    }
};

enum MageOrbEvents
{
    EVENT_ORB_MOVE_FORWARD              = 1,
    EVENT_ORB_APPLY_PERIODIC_EFFECT     = 2,
    EVENT_ORB_EARLY_EXPLOSION           = 3,
    EVENT_ORB_EXPLODE                   = 4
};

// 44214 - Flame Orb, 45322 - Frostfire Orb (CPP's npc_mage_orb): moves forward from the mage for 15 seconds, hovering
// over the ground (its sniffed data: hover, 2 yards), hitting the closest enemy every second; Fire Power's explosion on
// its chance, early when it found nothing to hit. CPP moves it to a point with the path generator; with pathfinding off
// (STATUS.md) that was one straight line through the air to the point 100 yards ahead, so it follows the ground here.
struct npc_mage_orb : public ScriptedAI
{
    npc_mage_orb(Creature* creature) : ScriptedAI(creature) { }

    void AttackStart(Unit* /*target*/) override
    {
        // moving on again applies the self snare's speed change
        if (me->isMoving())
            MoveForward();
    }

    // the orbs fly at their template's hover height (2 yards) instead of hovering (world DB): lifted here, before the
    // client first sees them, they start at the height they travel at
    void InitializeAI() override
    {
        ScriptedAI::InitializeAI();
        me->SetDisableGravity(true);
        float ground = me->GetMapHeight(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 5.0f);
        if (ground > INVALID_HEIGHT)
            me->Relocate(me->GetPositionX(), me->GetPositionY(), ground + me->GetFloatValue(UNIT_FIELD_HOVERHEIGHT), me->GetOrientation());
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        _pos = summoner->GetPosition();
        summoner->MovePositionToFirstCollision(_pos, 100.0f, 0.0f);
        _events.ScheduleEvent(EVENT_ORB_MOVE_FORWARD, 1ms);
        _events.ScheduleEvent(EVENT_ORB_APPLY_PERIODIC_EFFECT, 400ms);
        _events.ScheduleEvent(EVENT_ORB_EARLY_EXPLOSION, 5s);
        _events.ScheduleEvent(EVENT_ORB_EXPLODE, 15s + 400ms);
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        switch (_events.ExecuteEvent())
        {
            case EVENT_ORB_MOVE_FORWARD:
                MoveForward();
                break;
            case EVENT_ORB_APPLY_PERIODIC_EFFECT:
                DoCastSelf(me->GetEntry() == NPC_FLAME_ORB ? SPELL_FLAME_ORB_AURA : SPELL_FROSTFIRE_ORB_AURA, true);
                break;
            case EVENT_ORB_EARLY_EXPLOSION:
                if (!me->IsInCombat())
                    TryExplode();
                break;
            case EVENT_ORB_EXPLODE:
                TryExplode();
                break;
            default:
                break;
        }
    }

private:
    // straight on to the end point, the ground's height every 3 yards plus the template's hover height
    void MoveForward()
    {
        float hover = me->GetFloatValue(UNIT_FIELD_HOVERHEIGHT);
        float x = me->GetPositionX(), y = me->GetPositionY(), z = me->GetPositionZ() - hover;
        float dist = me->GetExactDist2d(_pos.GetPositionX(), _pos.GetPositionY());
        uint32 steps = std::max<uint32>(1, uint32(dist / 3.0f));

        Movement::PointsArray path;
        path.emplace_back(x, y, me->GetPositionZ());
        for (uint32 i = 1; i <= steps; ++i)
        {
            float px = x + (_pos.GetPositionX() - x) * i / steps;
            float py = y + (_pos.GetPositionY() - y) * i / steps;
            float ground = me->GetMapHeight(px, py, z + 5.0f);
            if (ground > INVALID_HEIGHT)
                z = ground;
            path.emplace_back(px, py, z + hover);
        }

        me->GetMotionMaster()->Clear();
        me->GetMotionMaster()->MoveSplinePath(&path, FORCED_MOVEMENT_RUN);
    }

    void TryExplode()
    {
        TempSummon* orb = me->ToTempSummon();
        Unit* summoner = orb ? orb->GetSummonerUnit() : nullptr;
        if (!summoner)
            return;

        if (Aura* aura = summoner->GetAuraOfRankedSpell(SPELL_FIRE_POWER_R1))
            if (roll_chance_i(aura->GetSpellInfo()->ProcChance))
            {
                float z = me->GetPositionZ() - me->GetFloatValue(UNIT_FIELD_HOVERHEIGHT);
                summoner->CastSpell(me->GetPositionX(), me->GetPositionY(), z, SPELL_FIRE_POWER_EXPLOSION, true);
                me->DespawnOrUnsummon();
            }
    }

    EventMap _events;
    Position _pos;
};

void AddSC_mage_pet_scripts()
{
    RegisterCreatureAI(npc_pet_mage_mirror_image);
    RegisterCreatureAI(npc_mage_orb);
}
