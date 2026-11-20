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

#include "AreaTriggerScript.h"
#include "CreatureScript.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "ScriptedGossip.h"
#include "SpellScript.h"
#include "SpellScriptLoader.h"
#include "TemporarySummon.h"
#include <array>

/*######
## npc_daphne_stilwell
######*/

enum DaphneStilwell
{
    // Yells
    SAY_DS_START        = 0,
    SAY_DS_DOWN_1       = 1,
    SAY_DS_DOWN_2       = 2,
    SAY_DS_DOWN_3       = 3,
    SAY_DS_PROLOGUE     = 4,

    // Spells
    SPELL_SHOOT         = 6660,

    // Quests
    QUEST_TOME_VALOR    = 1651,

    // Creatures
    NPC_DEFIAS_RAIDER   = 6180,

    // Equips
    EQUIP_ID_RIFLE      = 2511
};

class npc_daphne_stilwell : public CreatureScript
{
public:
    npc_daphne_stilwell() : CreatureScript("npc_daphne_stilwell") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_TOME_VALOR)
        {
            creature->AI()->Talk(SAY_DS_START);
            if (npc_escortAI* pEscortAI = CAST_AI(npc_daphne_stilwell::npc_daphne_stilwellAI, creature->AI()))
                pEscortAI->Start(true, player->GetGUID());
        }

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_daphne_stilwellAI(creature);
    }

    struct npc_daphne_stilwellAI : public npc_escortAI
    {
        npc_daphne_stilwellAI(Creature* creature) : npc_escortAI(creature), summons(me) { }

        SummonList summons;
        uint8 textCounter;

        void Reset() override
        {
            summons.DespawnAll();
            textCounter = SAY_DS_DOWN_1;
        }

        using CreatureAI::WaypointReached;
        void WaypointReached(uint32 waypointId) override
        {
            Player* player = GetPlayerForEscort();

            if (!player)
                return;

            switch (waypointId)
            {
                case 4:
                    SetEquipmentSlots(false, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE, EQUIP_ID_RIFLE);
                    me->SetSheath(SHEATH_STATE_RANGED);
                    me->HandleEmoteCommand(EMOTE_STATE_USE_STANDING_NO_SHEATHE);
                    break;
                case 7:
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11435.52f, 1601.26f, 68.06f, 4.1f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11440.96f, 1599.69f, 66.35f, 4.09f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11433.44f, 1594.24f, 66.99f, 4.05f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                    break;
                case 8:
                    me->SetSheath(SHEATH_STATE_RANGED);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11435.52f, 1601.26f, 68.06f, 4.1f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11440.96f, 1599.69f, 66.35f, 4.09f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11433.44f, 1594.24f, 66.99f, 4.05f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11428.29f, 1598.37f, 70.90f, 3.9f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                    break;
                case 9:
                    me->SetSheath(SHEATH_STATE_RANGED);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11435.52f, 1601.26f, 68.06f, 4.1f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11440.96f, 1599.69f, 66.35f, 4.09f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11433.44f, 1594.24f, 66.99f, 4.05f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11428.29f, 1598.37f, 70.90f, 3.9f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11438.14f, 1607.6f, 70.94f, 4.38f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                    break;
                case 10:
                    me->SetWalk(true);
                    break;
                case 11:
                    Talk(SAY_DS_PROLOGUE);
                    break;
                case 13:
                    SetEquipmentSlots(true);
                    me->SetSheath(SHEATH_STATE_UNARMED);
                    me->HandleEmoteCommand(EMOTE_STATE_USE_STANDING_NO_SHEATHE);
                    break;
                case 17:
                    SetEscortPaused(true);
                    player->GroupEventHappens(QUEST_TOME_VALOR, me);
                    me->SetNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                    me->DespawnOrUnsummon(1s, 1s);
                    break;
            }
        }

        void AttackStart(Unit* who) override
        {
            if (me->Attack(who, false))
            {
                me->SetInCombatWith(who);
                who->SetInCombatWith(me);
            }
        }

        void JustSummoned(Creature* creature) override
        {
            creature->SetHomePosition(me->GetHomePosition());
            creature->GetMotionMaster()->MoveChase(me);
            creature->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
            creature->AI()->AttackStart(me);
            creature->AddThreat(me, 0.0f);
            summons.Summon(creature);
        }

        void SummonedCreatureDies(Creature* creature, Unit*) override
        {
            summons.Despawn(creature);
            if (summons.empty())
                Talk(textCounter++, GetPlayerForEscort());
        }

        void Update(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;

            if (me->isAttackReady(BASE_ATTACK))
            {
                if (!me->IsWithinDist(me->GetVictim(), ATTACK_DISTANCE))
                    DoCastVictim(SPELL_SHOOT, true);
                else
                {
                    me->SetSheath(SHEATH_STATE_MELEE);
                    me->AttackerStateUpdate(me->GetVictim());
                }

                me->resetAttackTimer();
            }
        }
    };
};

/*######
## npc_westfall_hobo (Transient 42383, West Plains Drifter 42391)
######*/

enum WestfallHobo
{
    // Texts: groups 0-3 are clues #1-#4, the others are the hobo turning on the player
    SAY_HOBO_CLUE_1             = 0,
    SAY_HOBO_ANGRY_BRIBED       = 4,
    SAY_HOBO_ANGRY_ASKED        = 5,

    // Gossip (11635 options show only while the quest is in progress, see conditions)
    GOSSIP_MENU_HOBO            = 11635,
    GOSSIP_OPTION_HOBO_ASK      = 0,
    GOSSIP_OPTION_HOBO_BRIBE    = 1,
    HOBO_BRIBE_COPPER           = 2,    // gossip_menu_option.BoxMoney of the bribe option

    QUEST_MURDER_WAS_THE_CASE   = 26209,
    NPC_FURLBROW_MURDER_INFO_1  = 42414 // the four clue credits are 42414-42417
};

// Murder Was The Case That They Gave Me: asking a hobo outright makes him attack; bribing him
// gives the player's first missing clue, unless he takes offence (half the time) and attacks.
struct npc_westfall_hobo : public ScriptedAI
{
    npc_westfall_hobo(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        me->SetImmuneToPC(true);
    }

    void sGossipSelect(Player* player, uint32 menuId, uint32 gossipListId) override
    {
        if (menuId != GOSSIP_MENU_HOBO || me->IsInCombat())
            return;

        if (gossipListId == GOSSIP_OPTION_HOBO_ASK)
        {
            CloseGossipMenuFor(player);
            TurnOn(player, SAY_HOBO_ANGRY_ASKED);
            return;
        }

        if (gossipListId != GOSSIP_OPTION_HOBO_BRIBE)
            return;

        // The engine only charges BoxMoney after this hook, and not at all once the menu is closed
        CloseGossipMenuFor(player);
        if (!player->HasEnoughMoney(uint32(HOBO_BRIBE_COPPER)))
            return;

        player->ModifyMoney(-int32(HOBO_BRIBE_COPPER));

        if (roll_chance_i(50))
        {
            TurnOn(player, SAY_HOBO_ANGRY_BRIBED);
            return;
        }

        for (uint8 clue = 0; clue < 4; ++clue)
        {
            uint32 const credit = NPC_FURLBROW_MURDER_INFO_1 + clue;
            if (player->GetReqKillOrCastCurrentCount(QUEST_MURDER_WAS_THE_CASE, credit))
                continue;

            player->KilledMonsterCredit(credit);
            Talk(SAY_HOBO_CLUE_1 + clue, player);
            return;
        }
    }

private:
    void TurnOn(Player* player, uint8 textGroup)
    {
        Talk(textGroup, player);
        me->SetImmuneToPC(false);
        AttackStart(player);
    }
};

/*######
## Livin' the Life (26228): Two-Shoed Lou's Old House (item 57761) and the Jangolode Mine scene
######*/

enum LivinTheLife
{
    NPC_JANGOLODE_MINE_GLUBTOK          = 42492,
    NPC_JANGOLODE_MINE_FIGURE           = 42515,

    SPELL_JANGOLODE_MINE_SUMMON_GLUBTOK = 79263,
    SPELL_JANGOLODE_MINE_SUMMON_FIGURE  = 79265
};

// The scene's actors are summoned per player; the ping and despawn spells must reach only the caster's own
static Creature* FindOwnJangolodeActor(Player* player, uint32 entry)
{
    std::list<Creature*> actors;
    player->GetCreatureListWithEntryInGrid(actors, entry, 50.0f);
    for (Creature* actor : actors)
        if (actor->IsAlive() && actor->GetOwnerGUID() == player->GetGUID())
            return actor;

    return nullptr;
}

// 79262 - Summon Lou's House (effect 0 summons the crate and seats the player, effect 1 summons the actors)
class spell_westfall_summon_lous_house : public SpellScript
{
    PrepareSpellScript(spell_westfall_summon_lous_house);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_JANGOLODE_MINE_SUMMON_FIGURE, SPELL_JANGOLODE_MINE_SUMMON_GLUBTOK });
    }

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        caster->CastSpell(caster, SPELL_JANGOLODE_MINE_SUMMON_FIGURE, true);
        caster->CastSpell(caster, SPELL_JANGOLODE_MINE_SUMMON_GLUBTOK, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(
            spell_westfall_summon_lous_house::HandleScriptEffect, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 79275 - Quest Credit: Jangolode Event (effect 0 completes the quest for the figure's owner)
class spell_westfall_quest_credit_jangolode_event : public SpellScript
{
    PrepareSpellScript(spell_westfall_quest_credit_jangolode_event);

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        GetHitUnit()->ExitVehicle();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(
            spell_westfall_quest_credit_jangolode_event::HandleScriptEffect, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 79290 - General Trigger 1: Glubtok
// 79292 - General Trigger 2: Glubtok
// 79294 - General Trigger 3: Glubtok
// 79297 - General Trigger 4: Glubtok
class spell_westfall_livin_the_life_ping_glubtok : public SpellScript
{
    PrepareSpellScript(spell_westfall_livin_the_life_ping_glubtok);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.clear();
        if (Player* player = GetCaster()->ToPlayer())
            if (Creature* glubtok = FindOwnJangolodeActor(player, NPC_JANGOLODE_MINE_GLUBTOK))
                targets.push_back(glubtok);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(
            spell_westfall_livin_the_life_ping_glubtok::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// 79279 - General Trigger 1: Figure
// 79283 - General Trigger 2: Figure
// 79284 - General Trigger 3: Figure
// 79287 - General Trigger 4: Figure
class spell_westfall_livin_the_life_ping_figure : public SpellScript
{
    PrepareSpellScript(spell_westfall_livin_the_life_ping_figure);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.clear();
        if (Player* player = GetCaster()->ToPlayer())
            if (Creature* figure = FindOwnJangolodeActor(player, NPC_JANGOLODE_MINE_FIGURE))
                targets.push_back(figure);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(
            spell_westfall_livin_the_life_ping_figure::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// 79273 - Despawn Jangolode Actors (forced on the player by the crate when they leave it)
class spell_westfall_despawn_jangolode_actor : public SpellScript
{
    PrepareSpellScript(spell_westfall_despawn_jangolode_actor);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.clear();
        Player* player = GetCaster()->ToPlayer();
        if (!player)
            return;

        if (Creature* glubtok = FindOwnJangolodeActor(player, NPC_JANGOLODE_MINE_GLUBTOK))
            targets.push_back(glubtok);

        if (Creature* figure = FindOwnJangolodeActor(player, NPC_JANGOLODE_MINE_FIGURE))
            targets.push_back(figure);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(
            spell_westfall_despawn_jangolode_actor::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

/*######
## Lou's Parting Thoughts (26232): the thugs behind Two-Shoed Lou's house
######*/

enum LousPartingThoughts
{
    NPC_THUG                            = 42387,
    NPC_LOUS_PARTING_THOUGHTS_TRIGGER   = 42562,

    QUEST_LOUS_PARTING_THOUGHTS         = 26232,

    SPELL_HOBO_INFORMATION_4            = 79184,    // kill credit 42417 ("Eavesdrop on Thugs")

    SOUND_SHOOTING                      = 15071,
    SOUND_SCREAM                        = 17852,

    // Thug texts (the trigger's own group 0 is the closing warning)
    SAY_THUG_MEET_HER                   = 0,
    SAY_THUG_FOR_REAL                   = 1,
    SAY_THUG_PILE_OF_GOLD               = 2,
    SAY_THUG_HER_FACE                   = 3,
    SAY_THUG_EAVESDROPPER               = 4,
    SAY_THUG_ONLY_ONE_THING             = 5,
    SAY_THUG_DIE                        = 6,
    SAY_TRIGGER_HURRY_BACK              = 0,

    ACTION_THUG_RESET                   = 1,
    DATA_THUG_DIED                      = 1,

    EVENT_SUMMON_THUGS                  = 1,
    EVENT_THUG_MEET_HER,
    EVENT_THUG_FOR_REAL,
    EVENT_THUG_PILE_OF_GOLD,
    EVENT_THUG_HER_FACE,
    EVENT_THUG_EAVESDROPPER,
    EVENT_THUG_ONLY_ONE_THING,
    EVENT_THUG_DIE,
    EVENT_THUG_CREDIT,
    EVENT_THUG_SHOOT_1,
    EVENT_THUG_SHOOT_2,
    EVENT_THUG_SCREAM,
    EVENT_THUG_WARNING
};

static constexpr uint8 THUG_COUNT = 4;

Position const ThugPos[THUG_COUNT] =
{
    { -9859.36f, 1332.42f, 41.9859f, 2.49582f  },
    { -9862.52f, 1332.08f, 41.9859f, 0.855211f },
    { -9863.49f, 1335.49f, 41.9859f, 5.63741f  },
    { -9860.43f, 1335.46f, 41.9859f, 4.11898f  }
};

// 5988 - the thugs' meeting spot: the first player on the quest starts their conversation
class at_westfall_two_shoed_lou_thugs : public AreaTriggerScript
{
public:
    at_westfall_two_shoed_lou_thugs() : AreaTriggerScript("at_westfall_two_shoed_lou_thugs") { }

    bool OnTrigger(Player* player, AreaTrigger const* /*trigger*/) override
    {
        if (!player->IsAlive() || player->GetQuestStatus(QUEST_LOUS_PARTING_THOUGHTS) != QUEST_STATUS_INCOMPLETE)
            return false;

        if (Creature* trigger = player->FindNearestCreature(NPC_LOUS_PARTING_THOUGHTS_TRIGGER, 50.0f))
            trigger->AI()->SetGUID(player->GetGUID());

        return false;
    }
};

// 42387 - Thug: reports its death to the trigger, and a thug that loses its target resets the scene
struct npc_westfall_thug : public ScriptedAI
{
    npc_westfall_thug(Creature* creature) : ScriptedAI(creature) { }

    void EnterEvadeMode(EvadeReason /*why*/) override
    {
        if (TempSummon* summon = me->ToTempSummon())
            if (Creature* trigger = summon->GetSummonerCreatureBase())
                trigger->AI()->DoAction(ACTION_THUG_RESET);

        me->DespawnOrUnsummon();
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (TempSummon* summon = me->ToTempSummon())
            if (Creature* trigger = summon->GetSummonerCreatureBase())
                trigger->AI()->SetData(DATA_THUG_DIED, DATA_THUG_DIED);
    }
};

// 42562 - Lou's Parting Thoughts Trigger: keeps the four thugs, plays their conversation for one player at a
// time, turns them on that player and gives the credit once all four are dead.
struct npc_westfall_lous_parting_thoughts_trigger : public ScriptedAI
{
    npc_westfall_lous_parting_thoughts_trigger(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        _events.Reset();
        _events.ScheduleEvent(EVENT_SUMMON_THUGS, 0ms);
    }

    void SetGUID(ObjectGuid const& guid, int32 /*id*/) override
    {
        if (!_invokerGUID.IsEmpty())
            return;

        _invokerGUID = guid;
        _events.ScheduleEvent(EVENT_THUG_MEET_HER, 0ms);
    }

    void SetData(uint32 type, uint32 /*value*/) override
    {
        if (type != DATA_THUG_DIED || ++_thugDeaths < THUG_COUNT)
            return;

        _events.ScheduleEvent(EVENT_THUG_CREDIT, 0ms);
        DoAction(ACTION_THUG_RESET);
    }

    void DoAction(int32 action) override
    {
        // every thug evading reports this; one respawn is enough
        if (action == ACTION_THUG_RESET)
            _events.RescheduleEvent(EVENT_SUMMON_THUGS, 60s);
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            Player* invoker = ObjectAccessor::GetPlayer(*me, _invokerGUID);

            switch (eventId)
            {
                case EVENT_SUMMON_THUGS:
                    SummonThugs();
                    break;
                case EVENT_THUG_MEET_HER:
                    ThugTalk(0, SAY_THUG_MEET_HER, invoker);
                    _events.ScheduleEvent(EVENT_THUG_FOR_REAL, 5s);
                    break;
                case EVENT_THUG_FOR_REAL:
                    ThugTalk(1, SAY_THUG_FOR_REAL, invoker);
                    _events.ScheduleEvent(EVENT_THUG_PILE_OF_GOLD, 5s);
                    break;
                case EVENT_THUG_PILE_OF_GOLD:
                    ThugTalk(1, SAY_THUG_PILE_OF_GOLD, invoker);
                    _events.ScheduleEvent(EVENT_THUG_HER_FACE, 8500ms);
                    break;
                case EVENT_THUG_HER_FACE:
                    ThugTalk(2, SAY_THUG_HER_FACE, invoker);
                    _events.ScheduleEvent(EVENT_THUG_EAVESDROPPER, 5s);
                    break;
                case EVENT_THUG_EAVESDROPPER:
                    ThugTalk(0, SAY_THUG_EAVESDROPPER, invoker);
                    if (invoker)
                        for (ObjectGuid const& guid : _thugGUIDs)
                            if (Creature* thug = ObjectAccessor::GetCreature(*me, guid))
                                thug->SetFacingToObject(invoker);
                    _events.ScheduleEvent(EVENT_THUG_ONLY_ONE_THING, 8500ms);
                    break;
                case EVENT_THUG_ONLY_ONE_THING:
                    ThugTalk(0, SAY_THUG_ONLY_ONE_THING, invoker);
                    _events.ScheduleEvent(EVENT_THUG_DIE, 5s);
                    break;
                case EVENT_THUG_DIE:
                    // the eavesdropper left or died during the talk: nobody to attack, start over
                    if (!invoker || !invoker->IsAlive())
                    {
                        DoAction(ACTION_THUG_RESET);
                        break;
                    }

                    ThugTalk(0, SAY_THUG_DIE, invoker);
                    for (ObjectGuid const& guid : _thugGUIDs)
                    {
                        if (Creature* thug = ObjectAccessor::GetCreature(*me, guid))
                        {
                            thug->SetImmuneToAll(false);
                            thug->AI()->AttackStart(invoker);
                        }
                    }
                    break;
                case EVENT_THUG_CREDIT:
                    // the credit spell is a group kill credit, so the invoker's party gets it too
                    if (invoker)
                        invoker->CastSpell(invoker, SPELL_HOBO_INFORMATION_4, true);
                    _events.ScheduleEvent(EVENT_THUG_SHOOT_1, 1500ms);
                    break;
                case EVENT_THUG_SHOOT_1:
                    if (invoker)
                        me->PlayDistanceSound(SOUND_SHOOTING, invoker);
                    _events.ScheduleEvent(EVENT_THUG_SHOOT_2, 1200ms);
                    break;
                case EVENT_THUG_SHOOT_2:
                    if (invoker)
                        me->PlayDistanceSound(SOUND_SHOOTING, invoker);
                    _events.ScheduleEvent(EVENT_THUG_SCREAM, 1200ms);
                    break;
                case EVENT_THUG_SCREAM:
                    if (invoker)
                        me->PlayDistanceSound(SOUND_SCREAM, invoker);
                    _events.ScheduleEvent(EVENT_THUG_WARNING, 2500ms);
                    break;
                case EVENT_THUG_WARNING:
                    if (invoker)
                        Talk(SAY_TRIGGER_HURRY_BACK, invoker);
                    break;
                default:
                    break;
            }
        }
    }

private:
    void SummonThugs()
    {
        for (uint8 i = 0; i < THUG_COUNT; ++i)
        {
            if (Creature* old = ObjectAccessor::GetCreature(*me, _thugGUIDs[i]))
                old->DespawnOrUnsummon();

            _thugGUIDs[i].Clear();
            if (Creature* thug = me->SummonCreature(NPC_THUG, ThugPos[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000))
                _thugGUIDs[i] = thug->GetGUID();
        }

        _invokerGUID.Clear();
        _thugDeaths = 0;
    }

    void ThugTalk(uint8 thugIndex, uint8 textGroup, Player* invoker)
    {
        if (!invoker)
            return;

        if (Creature* thug = ObjectAccessor::GetCreature(*me, _thugGUIDs[thugIndex]))
            thug->AI()->Talk(textGroup, invoker);
    }

    EventMap _events;
    ObjectGuid _invokerGUID;
    std::array<ObjectGuid, THUG_COUNT> _thugGUIDs;
    uint8 _thugDeaths = 0;
};

void AddSC_westfall()
{
    new npc_daphne_stilwell();
    RegisterCreatureAI(npc_westfall_hobo);
    RegisterSpellScript(spell_westfall_summon_lous_house);
    RegisterSpellScript(spell_westfall_quest_credit_jangolode_event);
    RegisterSpellScript(spell_westfall_livin_the_life_ping_glubtok);
    RegisterSpellScript(spell_westfall_livin_the_life_ping_figure);
    RegisterSpellScript(spell_westfall_despawn_jangolode_actor);
    RegisterCreatureAI(npc_westfall_thug);
    RegisterCreatureAI(npc_westfall_lous_parting_thoughts_trigger);
    new at_westfall_two_shoed_lou_thugs();
}
