/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * Ported from ArkCORE-NG (https://github.com/Arkania/ArkCORE-NG, src/server/scripts/Maelstrom/zone_kezan.cpp),
 * GPL-2.0, by the Arkania team (gpn39f, ladel and others). Copyright (C) 2011-2017 ArkCORE <http://www.arkania.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* Kezan (map 648, zone 4737), the Goblin starting zone up to Life Savings.
 *
 * Rewritten against AzerothCore's APIs. Differences from ArkCORE that are on purpose:
 * - Creatures whose TDB 434 SmartAI already does what ArkCORE's class did keep their SmartAI (trainers, the four
 *   "Do it Yourself" targets, Foreman Dampwick, Kaja'Cola balloon, Brute Enforcer, ...). Where a class replaces a
 *   SmartAI, every action of that SmartAI is kept here, with TDB's creature_text group ids (ArkCORE's text ids
 *   belonged to its own database).
 * - Lines that TDB has no creature_text for are sent by their broadcast text id (the vault game, the partygoers).
 * - Kezan's chapters come from phase_area (build_world_db.py goblin()); no phase is set here.
 * - The 447 fires are TDB spawns of chapter 7, so the gasbot no longer spawns copies and they are never deleted.
 */

#include "CellImpl.h"
#include "Chat.h"
#include "CombatAI.h"
#include "Containers.h"
#include "CreatureGroups.h"
#include "CreatureScript.h"
#include "DBCStores.h"
#include "GameObjectAI.h"
#include "GameObjectScript.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Random.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "SpellAuraEffects.h"
#include "Spell.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "SpellScriptLoader.h"
#include "TemporarySummon.h"
#include "MoveSplineInit.h"
#include "Vehicle.h"
#include <deque>
#include <unordered_map>
#include "WaypointMgr.h"
#include <array>

namespace Kezan
{

enum KezanQuests
{
    QUEST_TAKING_CARE_OF_BUSINESS   = 14138,
    QUEST_DO_IT_YOURSELF            = 14070,
    QUEST_ROLLING_WITH_MY_HOMIES    = 14071,
    QUEST_KAJA_COLA                 = 25473,
    QUEST_MEGS_IN_MARKETING         = 28349,
    QUEST_REPORT_FOR_TRYOUTS        = 24567,
    QUEST_THE_REPLACEMENTS          = 24488,
    QUEST_NECESSARY_ROUGHNESS       = 24502,
    QUEST_FOURTH_AND_GOAL           = 24503,
    QUEST_FOURTH_AND_GOAL_2         = 28414,
    QUEST_GIVE_SASSY_THE_NEWS       = 24520,
    QUEST_OFF_TO_THE_BANK_M         = 26711,
    QUEST_OFF_TO_THE_BANK_F         = 26712,
    QUEST_THE_NEW_YOU_M             = 14110,
    QUEST_THE_NEW_YOU_F             = 14109,
    QUEST_LIFE_OF_THE_PARTY_M       = 14153,
    QUEST_LIFE_OF_THE_PARTY_F       = 14113,
    QUEST_PIRATE_PARTY_CRASHERS     = 14115,
    QUEST_THE_UNINVITED_GUEST       = 14116,
    QUEST_BAZILLION_MACAROONS       = 14120,
    NPC_HOT_ROD_LIFE_SAVINGS        = 37676,
    QUEST_ROBBING_HOODS             = 14121,
    QUEST_THE_GREAT_BANK_HEIST      = 14122,
    QUEST_LIBERATE_THE_KAJAMITE     = 14124,
    QUEST_447                       = 14125,
    QUEST_LIFE_SAVINGS              = 14126,
    QUEST_FLASH_HEAL                = 14009,
};

enum KezanCreatures
{
    NPC_HOT_ROD                     = 34840,
    NPC_ACE                         = 34957,
    NPC_GOBBER                      = 34958,
    NPC_IZZY                        = 34959,
    NPC_KEZAN_CITIZEN               = 35063,
    NPC_KEZAN_CITIZEN_2             = 35075,
    NPC_KEZAN_PARTYGOER             = 35175,
    NPC_PIRATE_PARTY_CRASHER        = 35200,
    NPC_HIRED_LOOTER                = 35234,
    NPC_STEAMWHEEDLE_SHARK          = 37114,
    NPC_BILGEWATER_BUCCANEER        = 37179,
    NPC_FOURTH_AND_GOAL_TARGET      = 37203,
    NPC_GASBOT                      = 37598,
    NPC_SASSY_DRIVER                = 37680,
    NPC_NECESSARY_ROUGHNESS_CREDIT  = 48271,
    NPC_TRAINING_DUMMY              = 48304,
    NPC_DEATHWING                   = 48572,
    NPC_FLASH_HEAL_CREDIT           = 44175,
    NPC_DEATHWING_ATTACK_TARGET     = 42196,
};

enum KezanObjects
{
    GO_KAJAMITE_CHUNK               = 195492,
    SPELL_KABLOOEY_BOMBS            = 67682,
    GO_GASBOT_CONTROL_PANEL         = 201736,
    GO_GASBOT_PANEL_GLITTER         = 202850,
};

enum KezanItems
{
    ITEM_KEYS_TO_THE_HOT_ROD        = 46856,
    ITEM_STACK_OF_MACAROONS         = 47044,
};

enum KezanSpells
{
    // Generic Quest Invisibility Detection 4-7: what lets the player see the football field's actors
    SPELL_INVISIBILITY_DETECTION_4  = 90161,
    SPELL_INVISIBILITY_DETECTION_5  = 94566,
    SPELL_INVISIBILITY_DETECTION_6  = 94567,
    SPELL_INVISIBILITY_DETECTION_7  = 94568,
    SPELL_QUEST_INVISIBILITY_5      = 88195,
    SPELL_QUEST_INVISIBILITY_6      = 82358,
    SPELL_QUEST_INVISIBILITY_7      = 85096,

    SPELL_KEYS_TO_THE_HOT_ROD_AURA  = 91551,
    SPELL_HOT_ROD_KNOCKBACK         = 66301,
    SPELL_HOT_ROD_KNOCKBACK_2       = 70330,
    SPELL_RESUMMON_ACE              = 66644,
    SPELL_RESUMMON_GOBBER           = 66645,
    SPELL_RESUMMON_IZZY             = 66646,
    SPELL_SUMMON_ACE                = 66597,
    SPELL_SUMMON_GOBBER             = 66599,
    SPELL_SUMMON_IZZY               = 66600,

    SPELL_AWESOME_PARTY_ENSEMBLE    = 66908,
    SPELL_LOTP_HAPPY_PARTYGOER      = 66916,
    SPELL_OUTFIT_FEMALE             = 66927,
    SPELL_OUTFIT_MALE               = 66928,
    SPELL_OUTFIT_SECONDARY          = 66985,
    SPELL_PHASE_QUEST_ZONE_01       = 59073,
    SPELL_EARTHQUAKE_CAMERA_SHAKE   = 96274,

    SPELL_SUMMON_HOT_ROD_SAVINGS    = 70321,
    SPELL_SASSYS_DRIVING            = 66724,
    SPELL_HAND_OVER_THE_KEYS        = 70318,
    SPELL_HOT_ROD_KNOCKBACK_TRIGGER = 70329,
    SPELL_TELEPORT_TO_LOST_ISLES    = 74100,

    SPELL_FLASH_HEAL                = 2061,
    SPELL_SHOOT_GUN                 = 69509,
    SPELL_TORCH_TOSS                = 6257,
    ITEM_STOLEN_LOOT                = 47530,

    SPELL_SUMMON_STEAMWHEEDLE_SHARK = 69971,
    SPELL_THROW_FOOTBOMB            = 69993,
    SPELL_SUMMON_BUCCANEER_GOAL     = 70075,
    SPELL_SIGNAL_BACK_TO_SHREDDER   = 70062,
    SPELL_INCREASED_DETECT_RANGE    = 76651,
    SPELL_DEATHWINGS_ATTACK         = 66858,
    SPELL_DEATHWING_SOUND_4         = 69988,
    SPELL_CATACLYSM_EARTHQUAKE      = 90615,    // Fourth and Goal: Character Earthquake (ArkCORE used Duskhaven's 68707)

    SPELL_SUMMON_GASBOT             = 70252,
    SPELL_GASBOT_GAS_STREAM         = 70256,
    SPELL_GASBOT_EXPLOSION          = 70259,

    SPELL_SUMMON_VAULT_VEHICLE      = 67488,
};

enum KezanSounds
{
    SOUND_FOOTBALL_CROWD            = 17466,
    SOUND_FOOTBALL_CHEER            = 17467,
    SOUND_DEATHWING_ROAR            = 8574,
    SOUND_DISCO_BALL                = 12924,
    SOUND_SUBJECT_NINE              = 18178,
};

// Broadcast texts for lines that TDB 434 has no creature_text for
enum KezanBroadcastTexts
{
    BCT_KICK_THE_FOOTBOMB           = 37050,
    BCT_MOUNT_KAJARO                = 37041,
    BCT_KAJA_COLA_IDEA_FIRST        = 37326,
    BCT_KAJA_COLA_IDEA_LAST         = 37365,
};

// The players around an object (ArkCORE's FindNearestPlayers)
std::list<Player*> GetPlayersInRange(WorldObject* source, float range)
{
    std::list<Player*> players;
    Acore::AnyPlayerInObjectRangeCheck checker(source, range);
    Acore::PlayerListSearcher<Acore::AnyPlayerInObjectRangeCheck> searcher(source, players, checker);
    Cell::VisitObjects(source, searcher, range);
    return players;
}

// A knockback of the Hot Rod hits the citizens and looters; the caster is the car or its driver.
Player* GetHotRodDriver(Unit* caster)
{
    if (!caster)
        return nullptr;

    if (Player* player = caster->ToPlayer())
        return player;

    if (Vehicle* vehicle = caster->GetVehicleKit())
        if (Unit* passenger = vehicle->GetPassenger(0))
            return passenger->ToPlayer();

    return nullptr;
}

bool IsHotRodKnockback(uint32 spellId)
{
    return spellId == SPELL_HOT_ROD_KNOCKBACK || spellId == SPELL_HOT_ROD_KNOCKBACK_2;
}

/*######
## 34673 Bamm Megabomb (hunter trainer): shoots the training dummies
######*/

enum BammMegabomb
{
    EVENT_BAMM_PICK_DUMMY           = 1,
    EVENT_BAMM_SHOOT                = 2,
};

struct npc_kezan_bamm_megabomb : public ScriptedAI
{
    npc_kezan_bamm_megabomb(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        _events.Reset();
        _events.ScheduleEvent(EVENT_BAMM_PICK_DUMMY, 5s);
        _dummyGUID.Clear();
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_BAMM_PICK_DUMMY:
                {
                    std::list<Creature*> dummies;
                    me->GetCreatureListWithEntryInGrid(dummies, NPC_TRAINING_DUMMY, 15.0f);
                    if (!dummies.empty())
                    {
                        Creature* dummy = Acore::Containers::SelectRandomContainerElement(dummies);
                        _dummyGUID = dummy->GetGUID();
                        me->SetFacingToObject(dummy);
                    }
                    _events.ScheduleEvent(EVENT_BAMM_SHOOT, 1s);
                    break;
                }
                case EVENT_BAMM_SHOOT:
                    if (Creature* dummy = ObjectAccessor::GetCreature(*me, _dummyGUID))
                        DoCast(dummy, SPELL_SHOOT_GUN);
                    _events.ScheduleEvent(EVENT_BAMM_PICK_DUMMY, 7s, 10s);
                    break;
                default:
                    break;
            }
        }

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

private:
    EventMap _events;
    ObjectGuid _dummyGUID;
};

/*######
## 48305 Injured Employee: priests heal them with Flash Heal (quest 14009)
######*/

struct npc_kezan_injured_employee : public ScriptedAI
{
    npc_kezan_injured_employee(Creature* creature) : ScriptedAI(creature) { }

    void SpellHit(Unit* caster, SpellInfo const* spell) override
    {
        if (spell->Id != SPELL_FLASH_HEAL || !caster)
            return;

        if (Player* player = caster->ToPlayer())
            if (player->GetQuestStatus(QUEST_FLASH_HEAL) == QUEST_STATUS_INCOMPLETE)
            {
                player->KilledMonsterCredit(NPC_FLASH_HEAL_CREDIT);
                me->DespawnOrUnsummon(500ms);
            }
    }
};

/*######
## 34668 Sassy Hardwrench: quest talk (TDB's SmartAI), Life Savings' Hot Rod, the football and party auras
######*/

enum SassyHardwrench
{
    SAY_SASSY_TAKING_CARE           = 0,
    SAY_SASSY_KAJA_COLA_DONE        = 1,
    SAY_SASSY_DO_IT_YOURSELF        = 2,
    SAY_SASSY_PIRATES               = 3,
    SAY_SASSY_UNINVITED_GUEST       = 4,
    SAY_SASSY_BANK_HEIST            = 5,
    SAY_SASSY_447                   = 6,
    SAY_SASSY_LIFE_SAVINGS          = 7,
    GOSSIP_MENU_LIFE_SAVINGS        = 12494,
};

void PlayRandomGossipEmote(Creature* creature)
{
    creature->HandleEmoteCommand(RAND(EMOTE_ONESHOT_TALK, EMOTE_ONESHOT_WAVE, EMOTE_ONESHOT_EXCLAMATION,
        EMOTE_ONESHOT_QUESTION));
}

struct npc_kezan_sassy_hardwrench : public ScriptedAI
{
    npc_kezan_sassy_hardwrench(Creature* creature) : ScriptedAI(creature) { }

    void sGossipHello(Player* /*player*/) override
    {
        PlayRandomGossipEmote(me);
    }

    // On the Life Savings drive Sassy is at the wheel: the one on her crate at the headquarters is not seen by her
    // passenger (the one at the yacht is)
    bool CanBeSeen(Player const* seer) override
    {
        Unit const* car = seer->GetVehicleBase();
        return !(car && car->GetEntry() == NPC_HOT_ROD_LIFE_SAVINGS && me->GetPositionX() < -8300.0f);
    }

    // Life Savings: "Okay, Sassy, I'm ready to go." (menu 12494, shown while the quest is under way) and she drives the
    // player to the yacht, as on retail; taking the quest alone does not
    void sGossipSelect(Player* player, uint32 sender, uint32 action) override
    {
        // The quest has no objectives: taken, it is complete at once
        QuestStatus status = player->GetQuestStatus(QUEST_LIFE_SAVINGS);
        if (sender != GOSSIP_MENU_LIFE_SAVINGS || action != 0
            || (status != QUEST_STATUS_INCOMPLETE && status != QUEST_STATUS_COMPLETE))
            return;

        CloseGossipMenuFor(player);
        Talk(SAY_SASSY_LIFE_SAVINGS);
        player->DestroyItemCount(ITEM_KEYS_TO_THE_HOT_ROD, 1, true);
        player->CastSpell(player, SPELL_SUMMON_HOT_ROD_SAVINGS, true);
    }

    void sQuestAccept(Player* player, Quest const* quest) override
    {
        switch (quest->GetQuestId())
        {
            case QUEST_TAKING_CARE_OF_BUSINESS:
                Talk(SAY_SASSY_TAKING_CARE);
                break;
            case QUEST_DO_IT_YOURSELF:
                Talk(SAY_SASSY_DO_IT_YOURSELF, player);
                break;
            case QUEST_PIRATE_PARTY_CRASHERS:
                Talk(SAY_SASSY_PIRATES);
                player->CastSpell(player, SPELL_EARTHQUAKE_CAMERA_SHAKE, true);
                break;
            case QUEST_THE_UNINVITED_GUEST:
                Talk(SAY_SASSY_UNINVITED_GUEST);
                break;
            case QUEST_THE_GREAT_BANK_HEIST:
                Talk(SAY_SASSY_BANK_HEIST, player);
                break;
            case QUEST_447:
                Talk(SAY_SASSY_447);
                break;
            default:
                break;
        }
    }

    void sQuestReward(Player* player, Quest const* quest, uint32 /*opt*/) override
    {
        switch (quest->GetQuestId())
        {
            case QUEST_KAJA_COLA:
                Talk(SAY_SASSY_KAJA_COLA_DONE);
                break;
            case QUEST_LIFE_OF_THE_PARTY_F:
            case QUEST_LIFE_OF_THE_PARTY_M:
                for (uint32 spellId : { SPELL_AWESOME_PARTY_ENSEMBLE, SPELL_OUTFIT_FEMALE, SPELL_OUTFIT_MALE,
                                        SPELL_OUTFIT_SECONDARY, SPELL_LOTP_HAPPY_PARTYGOER })
                    player->RemoveAurasDueToSpell(spellId);
                if (quest->GetQuestId() == QUEST_LIFE_OF_THE_PARTY_M)
                    player->RemoveAurasDueToSpell(SPELL_PHASE_QUEST_ZONE_01);
                else
                    me->HandleEmoteCommand(EMOTE_ONESHOT_CHEER);
                break;
            case QUEST_GIVE_SASSY_THE_NEWS:
                player->RemoveAurasDueToSpell(SPELL_INVISIBILITY_DETECTION_4);
                me->HandleEmoteCommand(EMOTE_ONESHOT_CHEER);
                break;
            case QUEST_DO_IT_YOURSELF:
            case QUEST_PIRATE_PARTY_CRASHERS:
            case QUEST_THE_GREAT_BANK_HEIST:
            case QUEST_447:
                me->HandleEmoteCommand(EMOTE_ONESHOT_CHEER);
                break;
            default:
                break;
        }
    }

    void UpdateAI(uint32 /*diff*/) override
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

/*######
## 34874 Megs Dreadshredder: the Hot Rod keys and the football field's detection aura
######*/

enum MegsDreadshredder
{
    SAY_MEGS_ROLLING_ACCEPT         = 0,
    SAY_MEGS_USE_THE_KEYS           = 1,
    SAY_MEGS_ROLLING_DONE           = 2,
    SAY_MEGS_ROBBING_HOODS          = 3,
};

struct npc_kezan_megs_dreadshredder : public ScriptedAI
{
    npc_kezan_megs_dreadshredder(Creature* creature) : ScriptedAI(creature) { }

    void sGossipHello(Player* player) override
    {
        PlayRandomGossipEmote(me);

        // Lost keys are replaced until Sassy takes them for Life Savings
        if (player->GetQuestStatus(QUEST_ROLLING_WITH_MY_HOMIES) == QUEST_STATUS_REWARDED
            && player->GetQuestStatus(QUEST_LIFE_SAVINGS) == QUEST_STATUS_NONE
            && !player->GetItemCount(ITEM_KEYS_TO_THE_HOT_ROD))
            player->AddItem(ITEM_KEYS_TO_THE_HOT_ROD, 1);
    }

    void sQuestAccept(Player* player, Quest const* quest) override
    {
        switch (quest->GetQuestId())
        {
            case QUEST_ROLLING_WITH_MY_HOMIES:
                Talk(SAY_MEGS_ROLLING_ACCEPT);
                DoCast(player, SPELL_KEYS_TO_THE_HOT_ROD_AURA, true);
                Talk(SAY_MEGS_USE_THE_KEYS, player);
                player->AddAura(SPELL_INVISIBILITY_DETECTION_4, player);
                break;
            case QUEST_REPORT_FOR_TRYOUTS:
                player->AddAura(SPELL_INVISIBILITY_DETECTION_4, player);
                break;
            case QUEST_ROBBING_HOODS:
                Talk(SAY_MEGS_ROBBING_HOODS, player);
                break;
            default:
                break;
        }
    }

    void sQuestReward(Player* player, Quest const* quest, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_ROLLING_WITH_MY_HOMIES)
        {
            Talk(SAY_MEGS_ROLLING_DONE);
            player->AddAura(SPELL_INVISIBILITY_DETECTION_4, player);
        }
    }

    void UpdateAI(uint32 /*diff*/) override
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

/*######
## 5497, 5498, 5499: Rolling with my Homies, the places where Ace, Gobber and Izzy wait
######*/

class at_kezan_homie : public AreaTriggerScript
{
public:
    at_kezan_homie(char const* name, uint32 homieEntry, uint32 summonSpell) : AreaTriggerScript(name),
        _homieEntry(homieEntry), _summonSpell(summonSpell) { }

    bool OnTrigger(Player* player, AreaTrigger const* /*trigger*/) override
    {
        if (player->GetQuestStatus(QUEST_ROLLING_WITH_MY_HOMIES) == QUEST_STATUS_INCOMPLETE
            && !player->GetReqKillOrCastCurrentCount(QUEST_ROLLING_WITH_MY_HOMIES, _homieEntry))
        {
            player->CastSpell(player, _summonSpell, true);
            player->KilledMonsterCredit(_homieEntry);
        }

        return true;
    }

private:
    uint32 _homieEntry;
    uint32 _summonSpell;
};

// Ace, Gobber and Izzy ride with the player from Rolling with my Homies until A Bazillion Macaroons?! is turned in
static bool HomiesGone(Player const* player)
{
    return player->GetQuestStatus(QUEST_BAZILLION_MACAROONS) == QUEST_STATUS_REWARDED;
}

static void DismissHomies(Player* player)
{
    std::vector<TempSummon*> homies;
    for (Unit* controlled : player->m_Controlled)
        if (controlled->GetEntry() == NPC_ACE || controlled->GetEntry() == NPC_GOBBER || controlled->GetEntry() == NPC_IZZY)
            if (TempSummon* summon = controlled->ToTempSummon())
                homies.push_back(summon);
    for (TempSummon* homie : homies)
        homie->UnSummon();
}

/*######
## 34840 Hot Rod (Rolling with my Homies, Robbing Hoods): picks up the homies and knocks citizens over
######*/

enum HotRod
{
    EVENT_HOT_ROD_CHECK_TARGETS     = 1,
    EVENT_HOT_ROD_SEAT_HOMIES       = 2,
    EVENT_HOT_ROD_EMOTE_IZZY        = 3,
    EVENT_HOT_ROD_EMOTE_GOBBER      = 4,
    EVENT_HOT_ROD_EMOTE_ACE         = 5,

    ACTION_HOMIE_ENTER_CAR          = 1,
    DATA_HOMIE_PLAYER               = 1,
    DATA_HOMIE_HOT_ROD              = 2,
};

bool IsHotRodTarget(uint32 entry)
{
    return entry == NPC_KEZAN_CITIZEN || entry == NPC_KEZAN_CITIZEN_2 || entry == NPC_HIRED_LOOTER;
}

struct npc_kezan_hot_rod : public VehicleAI
{
    npc_kezan_hot_rod(Creature* creature) : VehicleAI(creature) { }

    void Reset() override
    {
        _events.Reset();
        _events.ScheduleEvent(EVENT_HOT_ROD_CHECK_TARGETS, 1s);
        me->SetImmuneToNPC(true);
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        Player* player = summoner ? summoner->ToPlayer() : nullptr;
        if (!player)
            return;

        _playerGUID = player->GetGUID();
        // From A Bazillion Macaroons?! on, the player drives alone
        if (HomiesGone(player))
        {
            DismissHomies(player);
            return;
        }

        bool hasIzzy = false;
        bool hasGobber = false;
        bool hasAce = false;
        for (Unit* controlled : player->m_Controlled)
        {
            hasIzzy |= controlled->GetEntry() == NPC_IZZY;
            hasGobber |= controlled->GetEntry() == NPC_GOBBER;
            hasAce |= controlled->GetEntry() == NPC_ACE;
        }

        // Homies already picked up come back with the car
        QuestStatus status = player->GetQuestStatus(QUEST_ROLLING_WITH_MY_HOMIES);
        bool done = status == QUEST_STATUS_COMPLETE || status == QUEST_STATUS_REWARDED;
        auto resummon = [&](bool has, uint32 entry, uint32 spellId)
        {
            if (has)
            {
                if (status == QUEST_STATUS_INCOMPLETE
                    && !player->GetReqKillOrCastCurrentCount(QUEST_ROLLING_WITH_MY_HOMIES, entry))
                    player->KilledMonsterCredit(entry);
            }
            else if (done || (status == QUEST_STATUS_INCOMPLETE
                && player->GetReqKillOrCastCurrentCount(QUEST_ROLLING_WITH_MY_HOMIES, entry)))
                player->CastSpell(player, spellId, true);
        };
        resummon(hasIzzy, NPC_IZZY, SPELL_RESUMMON_IZZY);
        resummon(hasGobber, NPC_GOBBER, SPELL_RESUMMON_GOBBER);
        resummon(hasAce, NPC_ACE, SPELL_RESUMMON_ACE);

        _events.ScheduleEvent(EVENT_HOT_ROD_SEAT_HOMIES, 500ms);
    }

    void PassengerBoarded(Unit* passenger, int8 /*seatId*/, bool apply) override
    {
        if (!passenger)
            return;

        if (Player* player = passenger->ToPlayer())
        {
            if (apply)
            {
                if (!_playerGUID)
                    _playerGUID = player->GetGUID();
            }
            else
                me->DespawnOrUnsummon(1s);
            return;
        }

        Creature* homie = passenger->ToCreature();
        if (!homie)
            return;

        if (apply)
        {
            homie->SetImmuneToNPC(true);
            homie->SetReactState(REACT_PASSIVE);
            switch (homie->GetEntry())
            {
                case NPC_IZZY:
                    _events.RescheduleEvent(EVENT_HOT_ROD_EMOTE_IZZY, 2s, 10s);
                    break;
                case NPC_GOBBER:
                    _events.RescheduleEvent(EVENT_HOT_ROD_EMOTE_GOBBER, 2s, 10s);
                    break;
                case NPC_ACE:
                    _events.RescheduleEvent(EVENT_HOT_ROD_EMOTE_ACE, 2s, 10s);
                    break;
                default:
                    break;
            }
        }
        else
        {
            homie->SetImmuneToNPC(false);
            homie->SetReactState(REACT_DEFENSIVE);
            if (Player* player = ObjectAccessor::GetPlayer(*me, _playerGUID))
                homie->GetMotionMaster()->MoveFollow(player, frand(3.0f, 5.0f), frand(2.0f, 4.2f));
        }
    }

    void UpdateAI(uint32 diff) override
    {
        VehicleAI::UpdateAI(diff);
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_HOT_ROD_SEAT_HOMIES:
                    if (Player* player = ObjectAccessor::GetPlayer(*me, _playerGUID))
                        for (Unit* controlled : player->m_Controlled)
                            if (Creature* homie = controlled->ToCreature())
                                if (homie->GetEntry() == NPC_IZZY || homie->GetEntry() == NPC_GOBBER
                                    || homie->GetEntry() == NPC_ACE)
                                {
                                    homie->AI()->SetGUID(_playerGUID, DATA_HOMIE_PLAYER);
                                    homie->AI()->SetGUID(me->GetGUID(), DATA_HOMIE_HOT_ROD);
                                    homie->AI()->DoAction(ACTION_HOMIE_ENTER_CAR);
                                }
                    break;
                case EVENT_HOT_ROD_EMOTE_IZZY:
                    EmotePassenger(NPC_IZZY, RAND(1, 5, 6, 11, 25, 273, 274));
                    _events.Repeat(6s, 11s);
                    break;
                case EVENT_HOT_ROD_EMOTE_GOBBER:
                    EmotePassenger(NPC_GOBBER, RAND(1, 3, 11, 15, 20, 94, 440, 463));
                    _events.Repeat(8s, 13s);
                    break;
                case EVENT_HOT_ROD_EMOTE_ACE:
                    EmotePassenger(NPC_ACE, RAND(1, 5, 6, 11, 25, 273, 274));
                    _events.Repeat(7s, 12s);
                    break;
                case EVENT_HOT_ROD_CHECK_TARGETS:
                    if (HasTargetAhead())
                        DoCastSelf(SPELL_HOT_ROD_KNOCKBACK, true);
                    _events.Repeat(250ms);
                    break;
                default:
                    break;
            }
        }
    }

private:
    void EmotePassenger(uint32 entry, uint32 emote)
    {
        if (Vehicle* vehicle = me->GetVehicleKit())
            for (int8 seat = 1; seat < 4; ++seat)
                if (Unit* passenger = vehicle->GetPassenger(seat))
                    if (passenger->GetEntry() == entry)
                        passenger->HandleEmoteCommand(emote);
    }

    // A citizen or looter right in front of (or under) the car
    bool HasTargetAhead()
    {
        if (!me->GetVehicleKit() || !me->GetVehicleKit()->GetPassenger(0))
            return false;

        std::list<Creature*> creatures;
        me->GetCreatureListWithEntryInGrid(creatures, { NPC_KEZAN_CITIZEN, NPC_KEZAN_CITIZEN_2, NPC_HIRED_LOOTER },
            10.0f);
        for (Creature* creature : creatures)
        {
            if (!creature->IsAlive())
                continue;
            if (me->GetExactDist(creature) < 3.0f || me->isInFront(creature))
                return true;
        }
        return false;
    }

    EventMap _events;
    ObjectGuid _playerGUID;
};

/*######
## 34957 Ace, 34958 Gobber, 34959 Izzy: the homies get into the Hot Rod and follow the player
######*/

struct npc_kezan_homie : public ScriptedAI
{
    npc_kezan_homie(Creature* creature) : ScriptedAI(creature) { }

    enum
    {
        EVENT_ENTER_CAR = 1,
    };

    void IsSummonedBy(WorldObject* summoner) override
    {
        Player* player = summoner ? summoner->ToPlayer() : nullptr;
        if (!player)
            return;

        _playerGUID = player->GetGUID();
        if (Creature* car = player->GetVehicleCreatureBase())
            if (car->GetEntry() == NPC_HOT_ROD)
            {
                _hotRodGUID = car->GetGUID();
                _events.ScheduleEvent(EVENT_ENTER_CAR, 500ms);
            }
    }

    void SetGUID(ObjectGuid const& guid, int32 id) override
    {
        if (id == DATA_HOMIE_PLAYER)
            _playerGUID = guid;
        else if (id == DATA_HOMIE_HOT_ROD)
            _hotRodGUID = guid;
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_HOMIE_ENTER_CAR)
            _events.ScheduleEvent(EVENT_ENTER_CAR, 500ms);
    }

    void EnterEvadeMode(EvadeReason /*why*/) override
    {
        me->CombatStop(true);
        if (Player* player = ObjectAccessor::GetPlayer(*me, _playerGUID))
            me->GetMotionMaster()->MoveFollow(player, frand(3.0f, 5.0f), frand(2.0f, 4.2f));
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            if (eventId != EVENT_ENTER_CAR)
                continue;

            // The player drives in seat 0; Ace, Gobber and Izzy have seats 1, 2 and 3
            int8 seat = me->GetEntry() == NPC_ACE ? 1 : (me->GetEntry() == NPC_GOBBER ? 2 : 3);
            if (Creature* car = ObjectAccessor::GetCreature(*me, _hotRodGUID))
                if (Vehicle* hotRod = car->GetVehicleKit())
                    if (!hotRod->HasEmptySeat(0) && hotRod->HasEmptySeat(seat))
                        me->EnterVehicle(car, seat);
        }

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

private:
    EventMap _events;
    ObjectGuid _playerGUID;
    ObjectGuid _hotRodGUID;
};

/*######
## 35120 FBoK Bank Teller: TDB's SmartAI talk, and macaroons for The New You
######*/

enum BankTeller
{
    SAY_TELLER_OFF_TO_THE_BANK      = 0,
    SAY_TELLER_IDLE                 = 1,
    EVENT_TELLER_IDLE               = 1,
    MACAROONS_FOR_THE_NEW_YOU       = 10,
};

struct npc_kezan_fbok_bank_teller : public ScriptedAI
{
    npc_kezan_fbok_bank_teller(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        _events.Reset();
        _events.ScheduleEvent(EVENT_TELLER_IDLE, 15s, 60s);
    }

    void sGossipHello(Player* /*player*/) override
    {
        PlayRandomGossipEmote(me);
    }

    void sQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() != QUEST_THE_NEW_YOU_M && quest->GetQuestId() != QUEST_THE_NEW_YOU_F)
            return;

        // Off to the Bank gave ten; top them up if some were spent elsewhere
        uint32 count = player->GetItemCount(ITEM_STACK_OF_MACAROONS);
        if (count < MACAROONS_FOR_THE_NEW_YOU)
            player->AddItem(ITEM_STACK_OF_MACAROONS, MACAROONS_FOR_THE_NEW_YOU - count);
    }

    void sQuestReward(Player* /*player*/, Quest const* quest, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_OFF_TO_THE_BANK_M || quest->GetQuestId() == QUEST_OFF_TO_THE_BANK_F)
            Talk(SAY_TELLER_OFF_TO_THE_BANK);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!me->IsInCombat())
        {
            _events.Update(diff);
            if (_events.ExecuteEvent() == EVENT_TELLER_IDLE)
            {
                Talk(SAY_TELLER_IDLE);
                _events.ScheduleEvent(EVENT_TELLER_IDLE, 15s, 60s);
            }
        }

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

private:
    EventMap _events;
};

/*######
## 201591 Disco Ball
######*/

struct go_kezan_disco_ball : public GameObjectAI
{
    go_kezan_disco_ball(GameObject* go) : GameObjectAI(go) { }

    enum
    {
        EVENT_DISCO_SOUND = 1,
    };

    void Reset() override
    {
        _events.RescheduleEvent(EVENT_DISCO_SOUND, 3s);
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);
        while (uint32 eventId = _events.ExecuteEvent())
        {
            if (eventId != EVENT_DISCO_SOUND)
                continue;

            // A positional sound for players near the ball: as a direct sound everyone in view range, hundreds of
            // yards away, heard the disco music at full volume
            me->GetMap()->DoForAllPlayers([this](Player* player)
            {
                if (player->IsWithinDist(me, 40.0f))
                    me->PlayDistanceSound(SOUND_DISCO_BALL, player);
            });
            _events.ScheduleEvent(EVENT_DISCO_SOUND, 2600ms, 5200ms);
        }
    }

private:
    EventMap _events;
};

/*######
## 49150 Subject Nine
######*/

struct npc_kezan_subject_nine : public ScriptedAI
{
    npc_kezan_subject_nine(Creature* creature) : ScriptedAI(creature) { }

    enum
    {
        EVENT_SUBJECT_ANIMATION = 1,
    };

    void Reset() override
    {
        _events.Reset();
        _events.ScheduleEvent(EVENT_SUBJECT_ANIMATION, 10s);
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            if (eventId != EVENT_SUBJECT_ANIMATION)
                continue;

            // Positional, so it fades with distance (a direct sound reached everyone in view range at full volume)
            me->PlayDistanceSound(SOUND_SUBJECT_NINE);
            me->HandleEmoteCommand(RAND(33, 35, 377));
            _events.ScheduleEvent(EVENT_SUBJECT_ANIMATION, 15s, 18s);
        }

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

private:
    EventMap _events;
};

/*######
## 70478 Kaja'Cola: the drinker has an idea
######*/

class spell_kezan_kaja_cola : public AuraScript
{
    PrepareAuraScript(spell_kezan_kaja_cola);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetTarget())
            target->Talk(urand(BCT_KAJA_COLA_IDEA_FIRST, BCT_KAJA_COLA_IDEA_LAST), CHAT_MSG_SAY, 15.0f, target);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_kezan_kaja_cola::OnApply, EFFECT_0, SPELL_AURA_DUMMY,
            AURA_EFFECT_HANDLE_REAL);
    }
};

/*######
## 35234 Hired Looter (Robbing Hoods): run over by the Hot Rod, it drops its loot
######*/

enum HiredLooter
{
    EVENT_LOOTER_TORCH_TOSS         = 1,
    EVENT_LOOTER_RUN_OVER           = 2,
};

struct npc_kezan_hired_looter : public ScriptedAI
{
    npc_kezan_hired_looter(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        _events.Reset();
        _playerGUID.Clear();
        _runOver = false;
        me->SetImmuneToAll(false);
        me->ClearUnitState(UNIT_STATE_STUNNED);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        _events.ScheduleEvent(EVENT_LOOTER_TORCH_TOSS, 5s, 11s);
    }

    void SpellHit(Unit* caster, SpellInfo const* spell) override
    {
        if (_runOver || spell->Id != SPELL_HOT_ROD_KNOCKBACK)
            return;

        if (Player* player = GetHotRodDriver(caster))
        {
            // Run over: the loot goes straight to the driver and the looter dies under the car. A second later, as
            // before, it died in mid-air from the knockback and its cast of the loot (67041) did not reach the player.
            _runOver = true;
            _events.Reset();
            // Given directly: the create spell (67041) cast by the driver, seated in the car, made nothing
            if (player->GetQuestStatus(QUEST_ROBBING_HOODS) == QUEST_STATUS_INCOMPLETE)
                player->AddItem(ITEM_STOLEN_LOOT, 1);
            Unit::Kill(player, me);
            // Its Stolen Loot is already in the bag; the corpse holds nothing more
            me->loot.clear();
            me->RemoveDynamicFlag(UNIT_DYNFLAG_LOOTABLE);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_LOOTER_TORCH_TOSS:
                    if (me->IsInCombat())
                    {
                        DoCastVictim(SPELL_TORCH_TOSS);
                        _events.Repeat(5s, 11s);
                    }
                    break;
                default:
                    break;
            }
        }

        if (_runOver || !UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

private:
    EventMap _events;
    ObjectGuid _playerGUID;
    bool _runOver = false;
};

/*######
## 35063, 35075 Kezan Citizen: knocked aside by the Hot Rod, they walk back home
######*/

struct npc_kezan_citizen : public ScriptedAI
{
    npc_kezan_citizen(Creature* creature) : ScriptedAI(creature) { }

    enum
    {
        EVENT_CITIZEN_MOVE_HOME = 1,
    };

    void SpellHit(Unit* caster, SpellInfo const* spell) override
    {
        if (!IsHotRodKnockback(spell->Id) || !GetHotRodDriver(caster))
            return;

        // TDB 434 has no creature_text for the citizens, so ArkCORE's complaint is left out
        _events.RescheduleEvent(EVENT_CITIZEN_MOVE_HOME, 10s);
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);
        if (_events.ExecuteEvent() == EVENT_CITIZEN_MOVE_HOME && !me->IsInCombat())
            me->GetMotionMaster()->MoveTargetedHome();

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

private:
    EventMap _events;
};

/*######
## 35175, 35186 Kezan Partygoer (Life of the Party): each wants one thing, the player's party spell gives it
######*/

enum Partygoer
{
    SPELL_WANT_BUBBLY               = 75042,
    SPELL_WANT_BUCKET               = 75044,
    SPELL_WANT_DANCE                = 75046,
    SPELL_WANT_FIREWORKS            = 75048,
    SPELL_WANT_HORS_DOEUVRES        = 75050,
    SPELL_COSMETIC_STUN             = 46957,
    SPELL_COSMETIC_DRUNK            = 55664,

    SPELL_PLAYER_BUBBLY             = 66909,
    SPELL_PLAYER_BUCKET             = 66910,
    SPELL_PLAYER_DANCE              = 66911,
    SPELL_PLAYER_FIREWORKS          = 66912,
    SPELL_PLAYER_HORS_DOEUVRES      = 66913,

    SPELL_LOTP_BUBBLY               = 75122,
    SPELL_LOTP_SUMMON_BUCKET        = 66931,
    SPELL_LOTP_DANCE                = 75123,
    SPELL_LOTP_SUMMON_DISCO_BALL    = 66930,
    SPELL_LOTP_FIREWORKS_BLUE       = 66917,
    SPELL_LOTP_FIREWORKS_GREEN      = 66918,
    SPELL_LOTP_FIREWORKS_RED        = 66919,
    SPELL_LOTP_HORS_DOEUVRES        = 75124,

    EVENT_PARTYGOER_ASK             = 1,
    EVENT_PARTYGOER_WANT_AGAIN      = 2,
};

struct PartyWish
{
    uint32 wantAura;
    uint32 playerSpell;
    uint32 askText;                 // broadcast text: what the partygoer wants
    std::array<uint32, 5> thanks;   // broadcast texts: thanks (0 ends the list)
};

// The texts are TDB's creature_text of 35186 (35175 has none), by broadcast id
std::array<PartyWish, 5> const PartyWishes =
{ {
    { SPELL_WANT_BUBBLY,        SPELL_PLAYER_BUBBLY,        40222, { 35417, 35418, 35419, 35420, 35421 } },
    { SPELL_WANT_BUCKET,        SPELL_PLAYER_BUCKET,        40223, { 35422, 35423, 35424, 0, 0 } },
    { SPELL_WANT_DANCE,         SPELL_PLAYER_DANCE,         40224, { 35425, 35426, 35427, 35428, 0 } },
    { SPELL_WANT_FIREWORKS,     SPELL_PLAYER_FIREWORKS,     40225, { 35429, 35430, 35431, 35432, 0 } },
    { SPELL_WANT_HORS_DOEUVRES, SPELL_PLAYER_HORS_DOEUVRES, 40226, { 35433, 35434, 35435, 35436, 0 } },
} };

bool IsOnLifeOfTheParty(Player* player)
{
    return player->GetQuestStatus(QUEST_LIFE_OF_THE_PARTY_F) == QUEST_STATUS_INCOMPLETE
        || player->GetQuestStatus(QUEST_LIFE_OF_THE_PARTY_M) == QUEST_STATUS_INCOMPLETE;
}

struct npc_kezan_partygoer : public ScriptedAI
{
    npc_kezan_partygoer(Creature* creature) : ScriptedAI(creature)
    {
        _wish = uint8(urand(0, uint32(PartyWishes.size() - 1)));
    }

    void Reset() override
    {
        _events.Reset();
        _happy = false;
        SyncWish();
        ApplyWish();
        _events.ScheduleEvent(EVENT_PARTYGOER_ASK, 1s);
    }

    void SpellHit(Unit* caster, SpellInfo const* spell) override
    {
        Player* player = caster ? caster->ToPlayer() : nullptr;
        if (!player || _happy || !IsOnLifeOfTheParty(player))
            return;

        PartyWish const& wish = PartyWishes[_wish];
        if (spell->Id != wish.playerSpell)
            return;

        switch (wish.playerSpell)
        {
            case SPELL_PLAYER_BUBBLY:
                DoCast(player, SPELL_LOTP_BUBBLY, true);
                break;
            case SPELL_PLAYER_BUCKET:
                DoCastSelf(SPELL_LOTP_SUMMON_BUCKET, true);
                break;
            case SPELL_PLAYER_DANCE:
                DoCast(player, SPELL_LOTP_DANCE, true);
                DoCastSelf(SPELL_LOTP_SUMMON_DISCO_BALL, true);
                break;
            case SPELL_PLAYER_FIREWORKS:
                DoCastSelf(RAND(SPELL_LOTP_FIREWORKS_BLUE, SPELL_LOTP_FIREWORKS_GREEN, SPELL_LOTP_FIREWORKS_RED), true);
                break;
            case SPELL_PLAYER_HORS_DOEUVRES:
                DoCast(player, SPELL_LOTP_HORS_DOEUVRES, true);
                break;
            default:
                break;
        }

        std::vector<uint32> thanks;
        for (uint32 text : wish.thanks)
            if (text)
                thanks.push_back(text);
        me->Say(Acore::Containers::SelectRandomContainerElement(thanks), player);

        player->KilledMonsterCredit(NPC_KEZAN_PARTYGOER);
        _happy = true;
        me->RemoveAurasDueToSpell(wish.wantAura);
        me->RemoveAurasDueToSpell(SPELL_COSMETIC_STUN);
        me->RemoveAurasDueToSpell(SPELL_COSMETIC_DRUNK);
        me->AddAura(SPELL_LOTP_HAPPY_PARTYGOER, me);
        _events.CancelEvent(EVENT_PARTYGOER_ASK);
        _events.ScheduleEvent(EVENT_PARTYGOER_WANT_AGAIN, 2min);
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_PARTYGOER_ASK:
                    SyncWish();
                    if (Player* player = me->SelectNearestPlayer(15.0f))
                        if (IsOnLifeOfTheParty(player) && roll_chance_i(30))
                        {
                            me->Say(PartyWishes[_wish].askText, player);
                            _events.ScheduleEvent(EVENT_PARTYGOER_ASK, 30s, 60s);
                            break;
                        }
                    _events.ScheduleEvent(EVENT_PARTYGOER_ASK, 1s);
                    break;
                case EVENT_PARTYGOER_WANT_AGAIN:
                    _happy = false;
                    me->RemoveAurasDueToSpell(SPELL_LOTP_HAPPY_PARTYGOER);
                    ApplyWish();
                    _events.ScheduleEvent(EVENT_PARTYGOER_ASK, 1s);
                    break;
                default:
                    break;
            }
        }

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

private:
    // TDB gives each spawn its wish as an aura (spawn addon); ArkCORE rolled one, kept for spawns without one
    void SyncWish()
    {
        for (uint8 i = 0; i < PartyWishes.size(); ++i)
            if (me->HasAura(PartyWishes[i].wantAura))
            {
                _wish = i;
                return;
            }
    }

    void ApplyWish()
    {
        PartyWish const& wish = PartyWishes[_wish];
        if (!me->HasAura(wish.wantAura))
            me->AddAura(wish.wantAura, me);
        if (wish.wantAura == SPELL_WANT_BUCKET)
        {
            me->AddAura(SPELL_COSMETIC_STUN, me);
            me->AddAura(SPELL_COSMETIC_DRUNK, me);
        }
    }

    EventMap _events;
    uint8 _wish;
    bool _happy = false;
};

/*######
## 35486 First Bank of Kezan Vault (The Great Bank Heist): use the tool the vault asks for
######*/

enum BankVault
{
    SPELL_POWER_CORRECT             = 67493,
    SPELL_POWER_INCORRECT           = 67494,
    SPELL_DRILL_AURA                = 67495,
    SPELL_EXPLOSIVES_AURA           = 67496,
    SPELL_LISTEN_AURA               = 67497,
    SPELL_LOCKPICK_AURA             = 67498,
    SPELL_G_RAY_AURA                = 67499,
    SPELL_VAULT_TIMER               = 67502,
    SPELL_VAULT_CRACKED             = 67492,
    SPELL_VAULT_CRACKING_TOOLSET    = 67476,
    SPELL_VAULT_EXIT                = 67579,
    ITEM_PERSONAL_RICHES            = 46858,

    SPELL_TOOL_G_RAY                = 67526,
    SPELL_TOOL_LOCKPICK             = 67525,
    SPELL_TOOL_EAR_O_SCOPE          = 67524,
    SPELL_TOOL_BLASTCRACKERS        = 67508,
    SPELL_TOOL_DRILL                = 67522,

    SOUND_VAULT_CORRECT             = 11595,
    SOUND_VAULT_INCORRECT           = 11596,

    // Broadcast texts (TDB 434 has no creature_text for the vault)
    BCT_VAULT_INTRO_1               = 35566,
    BCT_VAULT_INTRO_2               = 35567,
    BCT_VAULT_INTRO_3               = 35568,
    BCT_VAULT_GOOD_LUCK             = 35569,
    BCT_VAULT_USE_DRILL             = 35576,
    BCT_VAULT_USE_BLASTCRACKERS     = 35577,
    BCT_VAULT_USE_EAR_O_SCOPE       = 35578,
    BCT_VAULT_USE_LOCKPICK          = 35579,
    BCT_VAULT_USE_G_RAY             = 35700,
    BCT_VAULT_CORRECT               = 35702,
    BCT_VAULT_INCORRECT             = 35704,
    BCT_VAULT_TIMED_OUT             = 35773,
    BCT_VAULT_SUCCESS               = 35852,

    EVENT_VAULT_INTRO               = 1,
    EVENT_VAULT_ASK                 = 2,
    EVENT_VAULT_CHECK_TIMEOUT       = 3,
    EVENT_VAULT_EXIT                = 4,
    EVENT_VAULT_GIVE_UP             = 5,

    VAULT_POWER_DONE                = 100,
};

// Seconds on screen: an introduction line until the next one (6 s apart), a request for the 5 seconds of its timer
// (spell 67502), an answer for the shortest the client allows (1 s held, 3 fading)
constexpr float VAULT_LINE_INTRO = 6.0f;
constexpr float VAULT_LINE_ASK = 5.0f;
constexpr float VAULT_LINE_ANSWER = 4.0f;

struct VaultTool
{
    uint32 askAura;
    uint32 toolSpell;
    uint32 askText;
};

std::array<VaultTool, 5> const VaultTools =
{ {
    { SPELL_DRILL_AURA,      SPELL_TOOL_DRILL,         BCT_VAULT_USE_DRILL },
    { SPELL_EXPLOSIVES_AURA, SPELL_TOOL_BLASTCRACKERS, BCT_VAULT_USE_BLASTCRACKERS },
    { SPELL_LISTEN_AURA,     SPELL_TOOL_EAR_O_SCOPE,   BCT_VAULT_USE_EAR_O_SCOPE },
    { SPELL_LOCKPICK_AURA,   SPELL_TOOL_LOCKPICK,      BCT_VAULT_USE_LOCKPICK },
    { SPELL_G_RAY_AURA,      SPELL_TOOL_G_RAY,         BCT_VAULT_USE_G_RAY },
} };

std::array<uint32, 4> const VaultIntroTexts = { BCT_VAULT_INTRO_1, BCT_VAULT_INTRO_2, BCT_VAULT_INTRO_3,
    BCT_VAULT_GOOD_LUCK };

// The vault's lines go to the client's center-screen boss frame, two slots: a new line takes the top slot when it is
// free, otherwise the bottom one, moving a newer bottom line up first. Sent with the default display time (10 seconds
// held, 3 fading) the lines piled up and, moved up, were drawn over one another (recording, 2026-11-08). Each line
// now lasts about as long as its part of the game: the client holds it for the time given less the fade, at least 1.
static void VaultWhisper(Creature* vault, uint32 textId, Player* player, float seconds)
{
    BroadcastText const* bct = sObjectMgr->GetBroadcastText(textId);
    if (!bct || !player)
        return;

    LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
    WorldPacket data;
    ChatHandler::BuildChatPacket(data, CHAT_MSG_RAID_BOSS_WHISPER, LANG_UNIVERSAL, vault, player,
        bct->GetText(locale, vault->getGender()), 0, "", locale);
    // The packet ends with the display time (float) and the keep-out-of-chat flag (uint8)
    data.put<float>(data.wpos() - sizeof(uint8) - sizeof(float), seconds);
    player->SendDirectMessage(&data);
}

struct npc_kezan_bank_vault : public VehicleAI
{
    npc_kezan_bank_vault(Creature* creature) : VehicleAI(creature) { }

    void Reset() override
    {
        _events.Reset();
        _introLine = 0;
        _asked = -1;
        // The Vault Breaking progress bar (mana, power display 82) fills only with right answers. Owned by the player,
        // the vault refilled it by itself during the introduction and was "cracked" before the first question.
        me->SetRegeneratingPower(false);
        me->SetMaxPower(POWER_MANA, VAULT_POWER_DONE);
        me->SetPower(POWER_MANA, 0);
        _events.ScheduleEvent(EVENT_VAULT_GIVE_UP, 2min);
    }

    void PassengerBoarded(Unit* passenger, int8 /*seatId*/, bool apply) override
    {
        Player* player = passenger ? passenger->ToPlayer() : nullptr;
        if (!player)
            return;

        if (apply)
        {
            _playerGUID = player->GetGUID();
            player->KilledMonsterCredit(me->GetEntry());
            me->SetRegeneratingPower(false);
            me->SetMaxPower(POWER_MANA, VAULT_POWER_DONE);
            me->SetPower(POWER_MANA, 0);
            _events.ScheduleEvent(EVENT_VAULT_INTRO, 100ms);
        }
        else
        {
            player->RemoveAurasDueToSpell(SPELL_VAULT_CRACKING_TOOLSET);
            player->RemoveAurasDueToSpell(SPELL_VAULT_TIMER);
            _playerGUID.Clear();
            _events.Reset();
            me->DespawnOrUnsummon(1s);
        }
    }

    void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
    {
        if (_asked < 0)
            return;

        bool isTool = false;
        for (VaultTool const& tool : VaultTools)
            isTool |= tool.toolSpell == spell->Id;
        if (!isTool)
            return;

        Player* player = ObjectAccessor::GetPlayer(*me, _playerGUID);
        if (!player)
            return;

        if (VaultTools[_asked].toolSpell == spell->Id)
        {
            me->PlayDirectSound(SOUND_VAULT_CORRECT, player);
            DoCastSelf(SPELL_POWER_CORRECT, true);
            VaultWhisper(me, BCT_VAULT_CORRECT, player, VAULT_LINE_ANSWER);
        }
        else
        {
            me->PlayDirectSound(SOUND_VAULT_INCORRECT, player);
            DoCastSelf(SPELL_POWER_INCORRECT, true);
            VaultWhisper(me, BCT_VAULT_INCORRECT, player, VAULT_LINE_ANSWER);
        }

        // One answer per request (ArkCORE kept the timeout running and could ask twice)
        ClearRequest(player);
        _events.CancelEvent(EVENT_VAULT_CHECK_TIMEOUT);
        _events.RescheduleEvent(EVENT_VAULT_ASK, 1s);
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            Player* player = ObjectAccessor::GetPlayer(*me, _playerGUID);
            switch (eventId)
            {
                case EVENT_VAULT_INTRO:
                    if (player)
                        VaultWhisper(me, VaultIntroTexts[_introLine], player, VAULT_LINE_INTRO);
                    if (++_introLine < VaultIntroTexts.size())
                        _events.ScheduleEvent(EVENT_VAULT_INTRO, 6s);
                    else
                    {
                        _events.ScheduleEvent(EVENT_VAULT_ASK, 6s);
                        _events.RescheduleEvent(EVENT_VAULT_GIVE_UP, 3min);
                    }
                    break;
                case EVENT_VAULT_ASK:
                    if (!player)
                        break;
                    if (me->GetPower(POWER_MANA) >= VAULT_POWER_DONE)
                    {
                        VaultWhisper(me, BCT_VAULT_SUCCESS, player, VAULT_LINE_INTRO);
                        // Vault Cracked! creates the Personal Riches for the vault's driver: the vault casts it
                        DoCastSelf(SPELL_VAULT_CRACKED, true);
                        if (!player->HasItemCount(ITEM_PERSONAL_RICHES))
                            player->AddItem(ITEM_PERSONAL_RICHES, 1);
                        _events.Reset();
                        _events.ScheduleEvent(EVENT_VAULT_EXIT, 2s);
                        break;
                    }
                    _asked = int8(urand(0, uint32(VaultTools.size() - 1)));
                    DoCastSelf(VaultTools[_asked].askAura, true);
                    DoCastSelf(SPELL_VAULT_TIMER, true);
                    me->AddAura(SPELL_VAULT_TIMER, player);
                    VaultWhisper(me, VaultTools[_asked].askText, player, VAULT_LINE_ASK);
                    _events.ScheduleEvent(EVENT_VAULT_CHECK_TIMEOUT, 2s);
                    break;
                case EVENT_VAULT_CHECK_TIMEOUT:
                    if (!player)
                        break;
                    if (player->HasAura(SPELL_VAULT_TIMER))
                        _events.Repeat(250ms);
                    else
                    {
                        // Too slow: "Timed Out!" (retail recording, 2026-11-08), and it counts as a wrong tool
                        me->PlayDirectSound(SOUND_VAULT_INCORRECT, player);
                        DoCastSelf(SPELL_POWER_INCORRECT, true);
                        VaultWhisper(me, BCT_VAULT_TIMED_OUT, player, VAULT_LINE_ANSWER);
                        ClearRequest(player);
                        _events.ScheduleEvent(EVENT_VAULT_ASK, 1s);
                    }
                    break;
                case EVENT_VAULT_GIVE_UP:
                    if (player)
                        player->CastSpell(player, SPELL_VAULT_EXIT, true);
                    [[fallthrough]];
                case EVENT_VAULT_EXIT:
                    if (player)
                        player->ExitVehicle();
                    break;
                default:
                    break;
            }
        }
    }

private:
    void ClearRequest(Player* player)
    {
        if (_asked >= 0)
            me->RemoveAurasDueToSpell(VaultTools[_asked].askAura);
        me->RemoveAurasDueToSpell(SPELL_VAULT_TIMER);
        player->RemoveAurasDueToSpell(SPELL_VAULT_TIMER);
        _asked = -1;
    }

    EventMap _events;
    ObjectGuid _playerGUID;
    uint8 _introLine = 0;
    int8 _asked = -1;
};

/*######
## 67555 The Great Bank Heist: Vault Interact, 70253 447: AICast Gasbot Master
## The game object's spell makes the player cast the real one (the vault vehicle, the gasbot)
######*/

class spell_kezan_cast_on_hit_unit : public SpellScript
{
    PrepareSpellScript(spell_kezan_cast_on_hit_unit);

public:
    spell_kezan_cast_on_hit_unit(uint32 spellId) : SpellScript(), _spellId(spellId) { }

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ _spellId });
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            target->CastSpell(target, _spellId, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_kezan_cast_on_hit_unit::HandleScript, EFFECT_0,
            SPELL_EFFECT_SCRIPT_EFFECT);
    }

private:
    uint32 _spellId;
};

/*######
## 37106 Coach Crosscheck: TDB's SmartAI talk, the football detection auras, the Fourth and Goal shredder
######*/

enum CoachCrosscheck
{
    SAY_COACH_TRYOUTS_DONE          = 0,
    SAY_COACH_REPLACEMENTS          = 1,
    SAY_COACH_GIVE_SASSY_THE_NEWS   = 2,
};

struct npc_kezan_coach_crosscheck : public ScriptedAI
{
    npc_kezan_coach_crosscheck(Creature* creature) : ScriptedAI(creature) { }

    void sGossipHello(Player* /*player*/) override
    {
        PlayRandomGossipEmote(me);
    }

    void sQuestAccept(Player* player, Quest const* quest) override
    {
        player->AddAura(SPELL_INVISIBILITY_DETECTION_4, player);
        switch (quest->GetQuestId())
        {
            case QUEST_THE_REPLACEMENTS:
                Talk(SAY_COACH_REPLACEMENTS);
                break;
            case QUEST_NECESSARY_ROUGHNESS:
                player->RemoveAurasDueToSpell(SPELL_INVISIBILITY_DETECTION_6);
                player->RemoveAurasDueToSpell(SPELL_INVISIBILITY_DETECTION_7);
                break;
            case QUEST_FOURTH_AND_GOAL:
            case QUEST_FOURTH_AND_GOAL_2:
                player->RemoveAurasDueToSpell(SPELL_INVISIBILITY_DETECTION_5);
                player->RemoveAurasDueToSpell(SPELL_INVISIBILITY_DETECTION_6);
                player->RemoveAurasDueToSpell(SPELL_INVISIBILITY_DETECTION_7);
                // The kicking shredder is boarded at the field shredder (48526), not summoned on the player here
                break;
            case QUEST_GIVE_SASSY_THE_NEWS:
                Talk(SAY_COACH_GIVE_SASSY_THE_NEWS);
                player->RemoveAurasDueToSpell(SPELL_INVISIBILITY_DETECTION_6);
                player->RemoveAurasDueToSpell(SPELL_INVISIBILITY_DETECTION_7);
                break;
            default:
                break;
        }
    }

    void sQuestReward(Player* player, Quest const* quest, uint32 /*opt*/) override
    {
        switch (quest->GetQuestId())
        {
            case QUEST_NECESSARY_ROUGHNESS:
                player->ExitVehicle();
                player->AddAura(SPELL_INVISIBILITY_DETECTION_4, player);
                break;
            case QUEST_REPORT_FOR_TRYOUTS:
                Talk(SAY_COACH_TRYOUTS_DONE);
                player->AddAura(SPELL_INVISIBILITY_DETECTION_4, player);
                break;
            case QUEST_THE_REPLACEMENTS:
            case QUEST_FOURTH_AND_GOAL:
            case QUEST_FOURTH_AND_GOAL_2:
                player->AddAura(SPELL_INVISIBILITY_DETECTION_4, player);
                break;
            default:
                break;
        }
    }

    void UpdateAI(uint32 /*diff*/) override
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

/*######
## 48526 Bilgewater Buccaneer (on the field): clickable while a nearby player plays Necessary Roughness
######*/

struct npc_kezan_bilgewater_buccaneer_field : public ScriptedAI
{
    npc_kezan_bilgewater_buccaneer_field(Creature* creature) : ScriptedAI(creature) { }

    enum
    {
        EVENT_CHECK_PLAYERS = 1,
    };

    void Reset() override
    {
        _events.RescheduleEvent(EVENT_CHECK_PLAYERS, 1s);
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            if (eventId != EVENT_CHECK_PLAYERS)
                continue;

            bool playing = false;
            for (Player* player : GetPlayersInRange(me, 30.0f))
                playing |= player->GetQuestStatus(QUEST_NECESSARY_ROUGHNESS) == QUEST_STATUS_INCOMPLETE
                    || IsOnFourthAndGoal(player);

            if (playing)
                me->SetNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
            else
                me->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);

            _events.ScheduleEvent(EVENT_CHECK_PLAYERS, 2500ms);
        }

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

    // Its spellclick (70015) gives the Necessary Roughness shredder. On Fourth and Goal, after leaving or losing
    // the kicking shredder, the same click puts the player back into one instead. (A spellclick condition would
    // hide the click flag from players on Fourth and Goal, so this is decided here.)
    bool BeforeSpellClick(Unit* clicker) override
    {
        Player* player = clicker ? clicker->ToPlayer() : nullptr;
        if (!player || !IsOnFourthAndGoal(player))
            return true;

        if (!player->GetVehicle())
        {
            player->RemoveAurasDueToSpell(SPELL_INVISIBILITY_DETECTION_5);
            player->RemoveAurasDueToSpell(SPELL_INVISIBILITY_DETECTION_6);
            player->CastSpell(player, SPELL_SUMMON_BUCCANEER_GOAL, true);
        }
        return false;
    }

private:
    static bool IsOnFourthAndGoal(Player* player)
    {
        return player->GetQuestStatus(QUEST_FOURTH_AND_GOAL) == QUEST_STATUS_INCOMPLETE
            || player->GetQuestStatus(QUEST_FOURTH_AND_GOAL_2) == QUEST_STATUS_INCOMPLETE;
    }

    EventMap _events;
};

/*######
## 37179 Bilgewater Buccaneer (Necessary Roughness): throw footbombs at the Steamwheedle Sharks
######*/

enum NecessaryRoughness
{
    EVENT_ROUGHNESS_CROWD_SOUND     = 1,
    EVENT_ROUGHNESS_START           = 2,
    EVENT_ROUGHNESS_CHEERING        = 3,
    EVENT_ROUGHNESS_CHECK           = 4,
    EVENT_SHARK_ADVANCE             = 6,
    EVENT_SHARK_BLADE               = 7,

    SPELL_SHARKBLADE                = 70027,

    SUMMON_PROPERTIES_SHARK         = 942,

    DATA_SHARK_PLAYER               = 1,
};

// The sharks' summon spells are 69971 and 69976-69982
std::array<std::pair<uint32, Position>, 8> const SharkSummons =
{ {
    { SPELL_SUMMON_STEAMWHEEDLE_SHARK,      { -8291.62f, 1479.97f, 43.8908f } },
    { SPELL_SUMMON_STEAMWHEEDLE_SHARK + 5,  { -8285.75f, 1484.46f, 42.9395f } },
    { SPELL_SUMMON_STEAMWHEEDLE_SHARK + 6,  { -8291.08f, 1487.72f, 43.8463f } },
    { SPELL_SUMMON_STEAMWHEEDLE_SHARK + 7,  { -8288.04f, 1477.49f, 43.3046f } },
    { SPELL_SUMMON_STEAMWHEEDLE_SHARK + 8,  { -8288.33f, 1490.41f, 43.4756f } },
    { SPELL_SUMMON_STEAMWHEEDLE_SHARK + 9,  { -8295.10f, 1484.91f, 44.3231f } },
    { SPELL_SUMMON_STEAMWHEEDLE_SHARK + 10, { -8294.66f, 1474.68f, 44.2946f } },
    { SPELL_SUMMON_STEAMWHEEDLE_SHARK + 11, { -8294.61f, 1493.67f, 44.6239f } },
} };

struct npc_kezan_bilgewater_buccaneer_roughness : public VehicleAI
{
    npc_kezan_bilgewater_buccaneer_roughness(Creature* creature) : VehicleAI(creature) { }

    void Reset() override
    {
        me->AddAura(SPELL_QUEST_INVISIBILITY_5, me);
        me->SetFacingTo(3.19f);
    }

    void JustSummoned(Creature* summon) override
    {
        if (summon->GetEntry() != NPC_STEAMWHEEDLE_SHARK)
            return;

        _sharks.push_back(summon->GetGUID());
        summon->AI()->SetGUID(_playerGUID, DATA_SHARK_PLAYER);
    }

    // A footbombed shark stays down on the field; the reinforcements keep the crowd up
    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
    {
        _sharkSpot.erase(summon->GetGUID());
    }

    void PassengerBoarded(Unit* passenger, int8 /*seatId*/, bool apply) override
    {
        Player* player = passenger ? passenger->ToPlayer() : nullptr;
        if (!player)
            return;

        if (apply)
        {
            me->SetFacingTo(3.14f);
            _playerGUID = player->GetGUID();
            player->RemoveAurasDueToSpell(SPELL_INVISIBILITY_DETECTION_4);
            player->AddAura(SPELL_INVISIBILITY_DETECTION_5, player);
            if (player->GetQuestStatus(QUEST_NECESSARY_ROUGHNESS) == QUEST_STATUS_INCOMPLETE)
            {
                player->KilledMonsterCredit(NPC_NECESSARY_ROUGHNESS_CREDIT);
                _events.ScheduleEvent(EVENT_ROUGHNESS_CROWD_SOUND, 1s);
                _events.ScheduleEvent(EVENT_ROUGHNESS_START, 4s);
                _events.ScheduleEvent(EVENT_ROUGHNESS_CHEERING, 250ms);
                _events.ScheduleEvent(EVENT_ROUGHNESS_CHECK, 1s);
            }
        }
        else
        {
            player->RemoveAurasDueToSpell(SPELL_INVISIBILITY_DETECTION_5);
            player->AddAura(SPELL_INVISIBILITY_DETECTION_4, player);
            StopGame();
            me->DespawnOrUnsummon(1s);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        VehicleAI::UpdateAI(diff);
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_ROUGHNESS_CROWD_SOUND:
                    if (Player* player = ObjectAccessor::GetPlayer(*me, _playerGUID))
                    {
                        player->PlayDirectSound(SOUND_FOOTBALL_CROWD, player);
                        _events.Repeat(1s);
                    }
                    break;
                case EVENT_ROUGHNESS_START:
                    // The summon spells aim at TARGET_DEST_NEARBY_ENTRY, which finds nothing here and lands on
                    // the shredder itself: the sharks spawned inside it and failed the quest at once. Summon
                    // them on their field positions instead, facing the shredder (JustSummoned registers them).
                    for (uint8 spot = 0; spot < SharkSummons.size(); ++spot)
                        SummonShark(spot);
                    break;
                case EVENT_ROUGHNESS_CHEERING:
                {
                    uint32 cheers = urand(3, 10);
                    for (uint32 i = 0; i < cheers; ++i)
                        if (Creature* citizen = GetRandomObserver())
                            citizen->HandleEmoteCommand(RAND(1, 4, 5, 6, 11, 14, 15, 25, 92, 94, 274));
                    _events.Repeat(500ms);
                    break;
                }
                case EVENT_ROUGHNESS_CHECK:
                    if (Player* player = ObjectAccessor::GetPlayer(*me, _playerGUID))
                    {
                        // Necessary Roughness pops up its own turn-in. The player stays in the shredder: Fourth
                        // and Goal (24503, no NPC giver) is offered right after it, in the same shredder.
                        QuestStatus status = player->GetQuestStatus(QUEST_NECESSARY_ROUGHNESS);
                        if (status == QUEST_STATUS_COMPLETE || player->GetQuestRewardStatus(QUEST_NECESSARY_ROUGHNESS))
                            DespawnSharks();

                        // Fourth and Goal accepted: the kick needs its own shredder (37213, its Kick Footbomb
                        // action bar), so the player moves straight into it
                        if (player->GetQuestStatus(QUEST_FOURTH_AND_GOAL) == QUEST_STATUS_INCOMPLETE
                            || player->GetQuestStatus(QUEST_FOURTH_AND_GOAL_2) == QUEST_STATUS_INCOMPLETE)
                        {
                            player->RemoveAurasDueToSpell(SPELL_INVISIBILITY_DETECTION_6);
                            if (Vehicle* vehicle = me->GetVehicleKit())
                                vehicle->RemoveAllPassengers();
                            player->RemoveAurasDueToSpell(SPELL_INVISIBILITY_DETECTION_5);
                            player->CastSpell(player, SPELL_SUMMON_BUCCANEER_GOAL, true);
                            break;
                        }
                        // ArkCORE failed the quest when a shark reached the shredder; in the original they
                        // crowd around it without that
                    }
                    _events.Repeat(500ms);
                    break;
                default:
                    break;
            }
        }
    }

private:
    // The summon spells aim at TARGET_DEST_NEARBY_ENTRY, which finds nothing here and lands on the shredder
    // itself (the sharks spawned inside it), so the sharks are summoned on their spots, at ground height
    void SummonShark(uint8 spot)
    {
        Position const& pos = SharkSummons[spot].second;
        float x = pos.GetPositionX() + frand(-2.0f, 2.0f);
        float y = pos.GetPositionY() + frand(-2.0f, 2.0f);
        float z = me->GetMapHeight(x, y, pos.GetPositionZ() + 5.0f);
        if (z <= INVALID_HEIGHT)
            z = pos.GetPositionZ();

        // In the original the sharks are not enemies (no red names). Their summon properties (942) are an ally
        // summon of no type, which changes no faction, so they get a friendly one here. The footbomb picks them
        // by entry (TARGET_CHECK_ENTRY), not by hostility.
        if (TempSummon* shark = me->SummonCreature(NPC_STEAMWHEEDLE_SHARK, x, y, z,
            0.0f, TEMPSUMMON_TIMED_DESPAWN, 180000, sSummonPropertiesStore.LookupEntry(SUMMON_PROPERTIES_SHARK)))
        {
            shark->SetFaction(FACTION_FRIENDLY);
            _sharkSpot[shark->GetGUID()] = spot;
        }
    }

    void DespawnSharks()
    {
        for (ObjectGuid const& guid : _sharks)
            if (Creature* shark = ObjectAccessor::GetCreature(*me, guid))
                shark->DespawnOrUnsummon(1ms);
        _sharks.clear();
        _sharkSpot.clear();
    }

    void StopGame()
    {
        DespawnSharks();
        _playerGUID.Clear();
        _events.Reset();
    }

    Creature* GetRandomObserver()
    {
        if (_observers.empty())
        {
            std::list<Creature*> citizens;
            me->GetCreatureListWithEntryInGrid(citizens, NPC_KEZAN_CITIZEN, 50.0f);
            for (Creature* citizen : citizens)
                _observers.push_back(citizen->GetGUID());
            return nullptr;
        }

        return ObjectAccessor::GetCreature(*me, Acore::Containers::SelectRandomContainerElement(_observers));
    }

    EventMap _events;
    ObjectGuid _playerGUID;
    GuidVector _sharks;
    GuidVector _observers;
    std::unordered_map<ObjectGuid, uint8> _sharkSpot;
};

/*######
## 37114 Steamwheedle Shark: hit by a footbomb, it goes down
######*/

struct npc_kezan_steamwheedle_shark : public ScriptedAI
{
    npc_kezan_steamwheedle_shark(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(WorldObject* summoner) override
    {
        // The sharks walk at the shredder, stop in a crowd in front of it and throw Sharkblades at it
        me->SetReactState(REACT_PASSIVE);
        if (summoner)
        {
            _shredderGUID = summoner->GetGUID();
            _events.ScheduleEvent(EVENT_SHARK_ADVANCE, 500ms);
        }
    }

    void SetGUID(ObjectGuid const& guid, int32 id) override
    {
        if (id == DATA_SHARK_PLAYER)
            _playerGUID = guid;
    }

    void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
    {
        // ArkCORE took any spell; only the footbomb counts
        if (spell->Id != SPELL_THROW_FOOTBOMB || !me->IsAlive())
            return;

        _events.Reset();
        me->GetMotionMaster()->Clear();
        me->GetMotionMaster()->MoveIdle();
        // The credit is given by hand, then the shark goes down where it stood
        if (Player* player = ObjectAccessor::GetPlayer(*me, _playerGUID))
            if (player->GetQuestStatus(QUEST_NECESSARY_ROUGHNESS) == QUEST_STATUS_INCOMPLETE)
                player->KilledMonsterCredit(NPC_STEAMWHEEDLE_SHARK);
        me->KillSelf();
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type == POINT_MOTION_TYPE && id == 0)
            _events.ScheduleEvent(EVENT_SHARK_BLADE, 500ms, 2500ms);
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);
        while (uint32 eventId = _events.ExecuteEvent())
        {
            Creature* shredder = ObjectAccessor::GetCreature(*me, _shredderGUID);
            if (!shredder || !shredder->IsAlive())
                continue;

            switch (eventId)
            {
                case EVENT_SHARK_ADVANCE:
                {
                    // A spot 9-14 yards in front of the shredder, on the shark's side of it (retail screenshots)
                    float angle = shredder->GetAngle(me) + frand(-0.45f, 0.45f);
                    float dist = frand(9.0f, 14.0f);
                    float x = shredder->GetPositionX() + dist * std::cos(angle);
                    float y = shredder->GetPositionY() + dist * std::sin(angle);
                    float z = me->GetMapHeight(x, y, shredder->GetPositionZ() + 5.0f);
                    if (z <= INVALID_HEIGHT)
                        z = shredder->GetPositionZ();
                    me->SetWalk(true);
                    me->GetMotionMaster()->MovePoint(0, x, y, z);
                    break;
                }
                case EVENT_SHARK_BLADE:
                    me->SetFacingToObject(shredder);
                    me->CastSpell(shredder, SPELL_SHARKBLADE, true);
                    _events.Repeat(2500ms, 4500ms);
                    break;
                default:
                    break;
            }
        }
    }

private:
    EventMap _events;
    ObjectGuid _playerGUID;
    ObjectGuid _shredderGUID;
};

/*######
## 37213 Bilgewater Buccaneer (Fourth and Goal): kick the footbomb between the smokestacks, then Deathwing comes
######*/

enum FourthAndGoal
{
    EVENT_GOAL_ENTER_VEHICLE        = 1,
    EVENT_GOAL_CROWD_SOUND          = 2,
    EVENT_GOAL_CHEER_SOUND          = 3,
    EVENT_GOAL_SUMMON_DEATHWING     = 4,
    EVENT_GOAL_KICK_PROMPT          = 5,
};

// He appears at the kick over Bilgewater Port, behind the player (.gps), and flies straight on over the field
Position const DeathwingSummonPos = { -8126.102f, 1434.897f, 78.79f, 3.04f };
Position const MountKajaroSummit = { -8821.0f, 1483.0f, 470.0f, 0.0f };

// The center-screen yellow lines of Fourth and Goal are boss emotes. Sent straight to the player: a boss emote
// only reaches players near its speaker, and Deathwing is far up in the sky.
static void SendBossEmoteTo(Unit* sender, Player* player, uint32 textId)
{
    BroadcastText const* text = sObjectMgr->GetBroadcastText(textId);
    if (!text || !player)
        return;

    LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
    WorldPacket data;
    ChatHandler::BuildChatPacket(data, CHAT_MSG_RAID_BOSS_EMOTE, LANG_UNIVERSAL, sender, player,
        text->GetText(locale, sender->getGender()), 0, "", locale);
    player->SendDirectMessage(&data);
}

struct npc_kezan_bilgewater_buccaneer_goal : public VehicleAI
{
    npc_kezan_bilgewater_buccaneer_goal(Creature* creature) : VehicleAI(creature) { }

    void Reset() override
    {
        me->AddAura(SPELL_QUEST_INVISIBILITY_6, me);
        me->AddAura(SPELL_QUEST_INVISIBILITY_7, me);
        me->SetFacingTo(3.14f);
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        Player* player = summoner ? summoner->ToPlayer() : nullptr;
        if (!player)
            return;

        _playerGUID = player->GetGUID();
        player->RemoveAurasDueToSpell(SPELL_INVISIBILITY_DETECTION_4);
        player->AddAura(SPELL_INVISIBILITY_DETECTION_7, player);
        // Boarded on the next update: the summon is not on the map yet
        _events.ScheduleEvent(EVENT_GOAL_ENTER_VEHICLE, 500ms);
        _events.ScheduleEvent(EVENT_GOAL_CROWD_SOUND, 1s);
    }

    void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
    {
        if (_kicked || spell->Id != SPELL_SIGNAL_BACK_TO_SHREDDER)
            return;

        // The goal counts and Deathwing appears at once (far behind the player; he reaches the field ~5.5 s later)
        if (Player* player = ObjectAccessor::GetPlayer(*me, _playerGUID))
        {
            _kicked = true;
            player->KilledMonsterCredit(NPC_FOURTH_AND_GOAL_TARGET);
            _events.CancelEvent(EVENT_GOAL_CROWD_SOUND);
            player->PlayDirectSound(SOUND_FOOTBALL_CHEER, player);
            _events.ScheduleEvent(EVENT_GOAL_CHEER_SOUND, 3s);
            _events.ScheduleEvent(EVENT_GOAL_SUMMON_DEATHWING, 100ms);
        }
    }

    void PassengerBoarded(Unit* passenger, int8 /*seatId*/, bool apply) override
    {
        Player* player = passenger ? passenger->ToPlayer() : nullptr;
        if (apply)
        {
            // In the original the aim is fixed on the smokestacks: the shredder faces them and cannot turn
            me->SetFacingTo(3.14f);
            me->SetUnitFlag2(UNIT_FLAG2_CANNOT_TURN);
            DoCastSelf(SPELL_INCREASED_DETECT_RANGE, true);
            // The center-screen prompt, a boss whisper from the shredder, a moment after boarding: sent at once it
            // reached the client before the shredder did and was dropped
            if (player)
                _events.ScheduleEvent(EVENT_GOAL_KICK_PROMPT, 1s);
        }
        else
        {
            if (player)
            {
                player->RemoveAurasDueToSpell(SPELL_INVISIBILITY_DETECTION_7);
                player->AddAura(SPELL_INVISIBILITY_DETECTION_4, player);
            }
            me->RemoveAurasDueToSpell(SPELL_INCREASED_DETECT_RANGE);
            // After the goal Deathwing comes whether the player stays in the shredder or not: the shredder,
            // which summons him, stays until he has
            me->DespawnOrUnsummon(_kicked && !_deathwingSummoned ? 6s : 100ms);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        VehicleAI::UpdateAI(diff);
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            Player* player = ObjectAccessor::GetPlayer(*me, _playerGUID);
            switch (eventId)
            {
                case EVENT_GOAL_ENTER_VEHICLE:
                    if (player)
                        player->EnterVehicle(me, 0);
                    break;
                case EVENT_GOAL_CROWD_SOUND:
                    if (player)
                        player->PlayDirectSound(SOUND_FOOTBALL_CROWD, player);
                    _events.Repeat(1s);
                    break;
                case EVENT_GOAL_CHEER_SOUND:
                    if (player)
                        player->PlayDirectSound(RAND(14556, 14557, 14558), player);
                    if (++_cheers < 3)
                        _events.Repeat(3s);
                    break;
                case EVENT_GOAL_KICK_PROMPT:
                    if (player && !_kicked && player->GetVehicleBase() == me)
                        SendBossEmoteTo(me, player, BCT_KICK_THE_FOOTBOMB);
                    break;
                case EVENT_GOAL_SUMMON_DEATHWING:
                    if (_deathwingSummoned || !player)
                        break;
                    _deathwingSummoned = true;
                    player->RemoveAurasDueToSpell(SPELL_INVISIBILITY_DETECTION_7);
                    player->AddAura(SPELL_INVISIBILITY_DETECTION_6, player);
                    player->SummonCreature(NPC_DEATHWING, DeathwingSummonPos, TEMPSUMMON_MANUAL_DESPAWN);
                    player->PlayDirectSound(SOUND_DEATHWING_ROAR, player);
                    break;
                default:
                    break;
            }
        }
    }

private:
    EventMap _events;
    ObjectGuid _playerGUID;
    bool _deathwingSummoned = false;
    bool _kicked = false;
    uint8 _cheers = 0;
};

/*######
## 70052 Kick Footbomb: a kick between the smokestacks signals the shredder
######*/

class spell_kezan_kick_footbomb : public SpellScript
{
    PrepareSpellScript(spell_kezan_kick_footbomb);

    void CheckDest()
    {
        // The aim is fixed on the smokestacks (the shredder cannot turn), so every kick goes between them;
        // the client's pitch range on vehicle 579 (30-45 degrees) only changes the arc
        GetCaster()->CastSpell(GetCaster(), SPELL_SIGNAL_BACK_TO_SHREDDER, true);
    }

    void Register() override
    {
        BeforeCast += SpellCastFn(spell_kezan_kick_footbomb::CheckDest);
    }
};

/*######
## 48572 Deathwing (Fourth and Goal): flies over the field and burns Mount Kajaro
######*/

enum DeathwingKezan
{
    EVENT_DEATHWING_FLY             = 1,
    EVENT_DEATHWING_YELL,
    EVENT_DEATHWING_ATTACK,
    EVENT_DEATHWING_KAJARO,
    EVENT_DEATHWING_EXIT_VEHICLE,
    EVENT_DEATHWING_EARTHQUAKE,
    EVENT_DEATHWING_ARRIVE,

    SOUND_DEATHWING_1               = 23227,
    SOUND_DEATHWING_2               = 23228,
    SOUND_DEATHWING_3               = 23229,
    SOUND_DEATHWING_4               = 23230,
    SOUND_FOOTBALL_DISMAY           = 14559,

    BCT_DEATHWING_TWILIGHT          = 48816,
    BCT_WHAT_WAS_THAT_THING         = 48862,
    BCT_GROUND_SHAKING              = 48861,
    NPC_KEZAN_CITIZEN_FEMALE        = 35075,

};

// Deathwing's flight over the football field, rebuilt from a retail recording seen from the kicking shredder,
// which faces west from (-8251, 1484), so the right of the screen is north. Positions come from where he is on
// screen and how big he looks; times are from the kick (he is summoned at about +4.5 s).
enum DeathwingArrival
{
    DW_ARRIVE_NONE,
    DW_ARRIVE_BREATH,           // faces Mount Kajaro and breathes fire at it
    DW_ARRIVE_KAJARO,           // "What did that dragon do to Mount Kajaro?!!!"
    DW_ARRIVE_DESPAWN,
};

struct DeathwingLeg
{
    Position pos;
    float speed;                // yards per second
    DeathwingArrival arrival;
    uint32 pauseMs;             // hover time after arriving
};

std::array<DeathwingLeg, 4> const DeathwingFlight =
{ {
    { { -8671.15f, 1514.410f, 378.85f },  40.0f, DW_ARRIVE_BREATH, 1500 },  // straight on, stops here, burns the beam
    { { -8306.265f, 1502.209f, 92.54f },  35.0f, DW_ARRIVE_KAJARO, 2300 },  // curves down to the field: holds
    { { -8126.102f, 1434.897f, 78.79f },  35.0f, DW_ARRIVE_NONE,   0 },     // off toward Bilgewater Port
    { { -7900.0f, 1460.0f, 90.0f },       40.0f, DW_ARRIVE_DESPAWN, 0 },    // on east over the port
} };

// From the fire back to the field he comes down in one curve out to the right: a cubic Bezier between the two
// points, through these controls, sampled into a smooth spline
uint32 const DEATHWING_CURVE_LEG = 1;
G3D::Vector3 const DeathwingCurveControl1 = { -8610.0f, 1650.0f, 300.0f };
G3D::Vector3 const DeathwingCurveControl2 = { -8390.0f, 1660.0f, 150.0f };

struct npc_kezan_deathwing : public ScriptedAI
{
    npc_kezan_deathwing(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        me->AddAura(SPELL_QUEST_INVISIBILITY_6, me);
        // A scene, not a fight: in the original he cannot be targeted. Full size: he fills the screen.
        me->SetUnitFlag(UNIT_FLAG_NOT_SELECTABLE);
        // Flying (with his flight row in creature_template_movement): every move is sent as a flight
        me->SetCanFly(true);
        me->SetDisableGravity(true);
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner ? summoner->ToPlayer() : nullptr)
            _playerGUID = player->GetGUID();

        // Flying animation: anim tier 3 (also in his creature_template_addon), set again now he is on the map
        me->SetAnimTier(AnimTier::Fly);
        // The marker on Mount Kajaro's summit he flies toward and burns: its light beam (78715, its template aura)
        // shines from the moment he appears. Kezan has no spawn of it, so he places it for the scene.
        if (Creature* target = me->SummonCreature(NPC_DEATHWING_ATTACK_TARGET, MountKajaroSummit,
            TEMPSUMMON_TIMED_DESPAWN, 45000))
        {
            // Seen from the field it is ~490 yards away, beyond the visibility range: the client did not know
            // the target, so it drew only the start of his fire, not the fireball flying to it
            target->SetVisibilityDistanceOverride(VisibilityDistanceType::Infinite);
            _targetGUID = target->GetGUID();
        }
        // Reset runs before the summon is on the map, where a move order is lost: the flight starts here
        _events.ScheduleEvent(EVENT_DEATHWING_FLY, 200ms);
        _events.ScheduleEvent(EVENT_DEATHWING_YELL, 4s);
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            Player* player = ObjectAccessor::GetPlayer(*me, _playerGUID);
            switch (eventId)
            {
                case EVENT_DEATHWING_FLY:
                {
                    if (_leg >= DeathwingFlight.size())
                        break;
                    DeathwingLeg const& leg = DeathwingFlight[_leg];
                    Movement::MoveSplineInit init(me);
                    if (_leg == DEATHWING_CURVE_LEG)
                    {
                        // The first point is replaced by where he is; the rest is the curve, smoothed
                        G3D::Vector3 const start(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
                        G3D::Vector3 const end(leg.pos.GetPositionX(), leg.pos.GetPositionY(), leg.pos.GetPositionZ());
                        Movement::PointsArray path = { start };
                        for (uint32 i = 1; i <= 10; ++i)
                        {
                            float t = i / 10.0f, u = 1.0f - t;
                            path.push_back(start * (u * u * u) + DeathwingCurveControl1 * (3 * u * u * t)
                                + DeathwingCurveControl2 * (3 * u * t * t) + end * (t * t * t));
                        }
                        init.MovebyPath(path);
                        // As with taxi flights: Flying + Catmull-Rom with the full point list curves the route;
                        // SetSmooth then SetFly cleared the curve and he flew corners (core d48ca6be1)
                        init.SetSmoothFlying();
                        init.SetUncompressed();
                    }
                    else
                    {
                        init.MoveTo(leg.pos.GetPositionX(), leg.pos.GetPositionY(), leg.pos.GetPositionZ(), false, true);
                        init.SetFly();
                    }
                    init.SetVelocity(leg.speed);
                    int32 duration = init.Launch();
                    _events.ScheduleEvent(EVENT_DEATHWING_ARRIVE, Milliseconds(std::max<int32>(duration, 100)));
                    break;
                }
                case EVENT_DEATHWING_ARRIVE:
                {
                    DeathwingLeg const& leg = DeathwingFlight[_leg];
                    switch (leg.arrival)
                    {
                        case DW_ARRIVE_BREATH:
                            me->SetFacingTo(me->GetAngle(MountKajaroSummit.GetPositionX(), MountKajaroSummit.GetPositionY()));
                            _events.ScheduleEvent(EVENT_DEATHWING_ATTACK, 300ms);
                            break;
                        case DW_ARRIVE_KAJARO:
                            // The Mount Kajaro line: he hesitates a moment, hovering, then flies off; the player
                            // drops out of the shredder as he goes
                            // He holds a second, then the line comes
                            _events.ScheduleEvent(EVENT_DEATHWING_KAJARO, 1s);
                            _events.ScheduleEvent(EVENT_DEATHWING_EXIT_VEHICLE, 2500ms);
                            _events.ScheduleEvent(EVENT_DEATHWING_EARTHQUAKE, 4500ms);
                            break;
                        case DW_ARRIVE_DESPAWN:
                            // The panicked citizens (90636, invisibility type 14) are seen through the detection his
                            // arrival gave; they calm down, out of sight, as he leaves
                            if (player)
                            {
                                player->RemoveAurasDueToSpell(SPELL_INVISIBILITY_DETECTION_6);
                                if (!player->HasAura(SPELL_INVISIBILITY_DETECTION_4))
                                    player->AddAura(SPELL_INVISIBILITY_DETECTION_4, player);
                            }
                            me->DespawnOrUnsummon(1s);
                            return;
                        default:
                            break;
                    }
                    ++_leg;
                    _events.ScheduleEvent(EVENT_DEATHWING_FLY, Milliseconds(leg.pauseMs + 10));
                    break;
                }
                case EVENT_DEATHWING_YELL:
                    // His yell over the field (broadcast text 48816, as in the original)
                    me->Yell(BCT_DEATHWING_TWILIGHT, player);
                    // His voice for it: "Fourth and Goal: Deathwing Sound 2" (vo_qe_deathwing_event05); the text
                    // (48816) carries no sound of its own
                    if (player)
                        player->PlayDirectSound(SOUND_DEATHWING_2, player);
                    break;
                case EVENT_DEATHWING_ATTACK:
                    // He breathes fire at the marker on Mount Kajaro's summit (the attack's range is "anywhere")
                    if (Creature* target = ObjectAccessor::GetCreature(*me, _targetGUID))
                        DoCast(target, SPELL_DEATHWINGS_ATTACK, true);
                    if (player)
                        player->PlayDirectSound(SOUND_DEATHWING_3, player);
                    break;
                case EVENT_DEATHWING_KAJARO:
                    DoCastSelf(SPELL_DEATHWING_SOUND_4, true);
                    if (player)
                    {
                        player->PlayDirectSound(SOUND_DEATHWING_4, player);
                        SendBossEmoteTo(me, player, BCT_MOUNT_KAJARO);
                    }
                    break;
                case EVENT_DEATHWING_EXIT_VEHICLE:
                    if (player)
                    {
                        player->ExitVehicle();
                        player->PlayDirectSound(SOUND_FOOTBALL_DISMAY, player);
                    }
                    break;
                case EVENT_DEATHWING_EARTHQUAKE:
                    // Kezan's own earthquake (90615); ArkCORE used Duskhaven's (68707)
                    if (player)
                    {
                        player->CastSpell(player, SPELL_CATACLYSM_EARTHQUAKE, true);
                        // The citizens on the stands react (a male and a female Kezan Citizen)
                        if (Creature* citizen = player->FindNearestCreature(NPC_KEZAN_CITIZEN, 40.0f))
                            citizen->Say(BCT_WHAT_WAS_THAT_THING, player);
                        if (Creature* citizen = player->FindNearestCreature(NPC_KEZAN_CITIZEN_FEMALE, 40.0f))
                            citizen->Say(BCT_GROUND_SHAKING, player);
                    }
                    break;
                default:
                    break;
            }
        }
    }

private:
    EventMap _events;
    ObjectGuid _playerGUID;
    ObjectGuid _targetGUID;
    uint32 _leg = 0;
};

/*######
## 195488 Kaja'mite Deposit (Liberate the Kaja'mite): breaks into chunks
######*/

struct go_kezan_kajamite_deposit : public GameObjectAI
{
    go_kezan_kajamite_deposit(GameObject* go) : GameObjectAI(go) { }

    // The Kablooey Bombs (67682) open the deposit; the core tells the script of an opened object only if it closes by
    // itself again, which this one does not, so the bomb's hit is what breaks it
    void SpellHit(Unit* caster, SpellInfo const* spellInfo) override
    {
        if (!me->isSpawned() || !caster || spellInfo->Id != SPELL_KABLOOEY_BOMBS)
            return;

        Player* player = caster->ToPlayer();
        if (!player || player->GetQuestStatus(QUEST_LIBERATE_THE_KAJAMITE) != QUEST_STATUS_INCOMPLETE)
            return;

        // Two or three chunks thrown onto the ground around it
        uint8 chunks = urand(2, 3);
        for (uint8 i = 0; i < chunks; ++i)
        {
            float angle = frand(0.0f, 2 * float(M_PI));
            float dist = frand(1.5f, 3.0f);
            float x = me->GetPositionX() + dist * std::cos(angle);
            float y = me->GetPositionY() + dist * std::sin(angle);
            float z = me->GetMapHeight(x, y, me->GetPositionZ() + 2.0f);
            if (z <= INVALID_HEIGHT)
                z = me->GetPositionZ();
            me->SummonGameObject(GO_KAJAMITE_CHUNK, x, y, z, angle, 0.0f, 0.0f, 0.0f, 0.0f, 60);
        }

        // ArkCORE deleted the spawn for good; it respawns on its own timer instead
        me->DespawnOrUnsummon(1ms);
    }
};

/*######
## 35202 Kezan Partygoer (Pirate Party Crashers): stays passive until a pirate hits it
######*/

struct npc_kezan_partygoer_passive : public ScriptedAI
{
    npc_kezan_partygoer_passive(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        me->SetReactState(REACT_PASSIVE);
    }

    void DamageTaken(Unit* attacker, uint32& /*damage*/, DamageEffectType /*damagetype*/,
        SpellSchoolMask /*damageSchoolMask*/) override
    {
        if (attacker && attacker->GetEntry() == NPC_PIRATE_PARTY_CRASHER)
            me->SetReactState(REACT_AGGRESSIVE);
    }
};

/*######
## 37598 Gasbot (447): walks into the house with its gas and sets it on fire
######*/

enum Gasbot
{
    POINT_GASBOT_1                  = 2001,
    POINT_GASBOT_2,
    POINT_GASBOT_3,
    POINT_GASBOT_4,

    EVENT_GASBOT_START              = 1,
    EVENT_GASBOT_MOVE_2,
    EVENT_GASBOT_MOVE_3,
    EVENT_GASBOT_MOVE_4,

    GASBOT_PASSENGERS               = 4,

    NPC_447_FIREWORKS_BUNNY         = 37682,
    SPELL_447_EXPLOSION_FIRE        = 70262,
};

struct npc_kezan_gasbot : public VehicleAI
{
    npc_kezan_gasbot(Creature* creature) : VehicleAI(creature) { }

    void Reset() override
    {
        _passengers = 0;
        me->SetWalk(true);
    }

    void PassengerBoarded(Unit* /*passenger*/, int8 /*seatId*/, bool apply) override
    {
        // The four gas targets are its accessories
        if (apply && ++_passengers == GASBOT_PASSENGERS)
            _events.ScheduleEvent(EVENT_GASBOT_START, 100ms);
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        switch (id)
        {
            case POINT_GASBOT_1:
                _events.ScheduleEvent(EVENT_GASBOT_MOVE_2, 100ms);
                break;
            case POINT_GASBOT_2:
                _events.ScheduleEvent(EVENT_GASBOT_MOVE_3, 100ms);
                break;
            case POINT_GASBOT_3:
                _events.ScheduleEvent(EVENT_GASBOT_MOVE_4, 100ms);
                break;
            case POINT_GASBOT_4:
                me->DespawnOrUnsummon(10ms);
                break;
            default:
                break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        VehicleAI::UpdateAI(diff);
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_GASBOT_START:
                    DoCastSelf(SPELL_GASBOT_GAS_STREAM, true);
                    me->GetMotionMaster()->MovePoint(POINT_GASBOT_1, -8424.21f, 1342.95f, 102.41f);
                    break;
                case EVENT_GASBOT_MOVE_2:
                    me->GetMotionMaster()->MovePoint(POINT_GASBOT_2, -8424.27f, 1346.94f, 104.66f);
                    break;
                case EVENT_GASBOT_MOVE_3:
                    me->GetMotionMaster()->MovePoint(POINT_GASBOT_3, -8423.88f, 1365.72f, 104.68f);
                    break;
                case EVENT_GASBOT_MOVE_4:
                    // In the house: it blows up and the player who sent it gets the quest's gasbot credit; the fireworks
                    // set about the house go off (447: Explosion + Fire)
                    DoCastSelf(SPELL_GASBOT_EXPLOSION, true);
                    {
                        std::list<Creature*> bunnies;
                        me->GetCreatureListWithEntryInGrid(bunnies, NPC_447_FIREWORKS_BUNNY, 40.0f);
                        for (Creature* bunny : bunnies)
                            bunny->AddAura(SPELL_447_EXPLOSION_FIRE, bunny);
                    }
                    if (TempSummon* summon = me->ToTempSummon())
                        if (Unit* summoner = summon->GetSummonerUnit())
                            if (Player* player = summoner->ToPlayer())
                                if (player->GetQuestStatus(QUEST_447) == QUEST_STATUS_INCOMPLETE)
                                {
                                    player->KilledMonsterCredit(NPC_GASBOT);
                                    // The headquarters' fires, the Claims Adjuster and his shredder are seen from now on
                                    player->UpdateObjectVisibility();
                                }
                    me->GetMotionMaster()->MovePoint(POINT_GASBOT_4, -8412.07f, 1364.56f, 104.71f);
                    break;
                default:
                    break;
            }
        }
    }

private:
    EventMap _events;
    uint8 _passengers = 0;
};

/*######
## 201736 Gasbot Control Panel, 201734 Flammable Bed (447): a lit bed makes the panel glitter for that player
######*/

enum GasbotPanel
{
    DATA_PANEL_PLAYER               = 1,
    DATA_PANEL_GLITTER              = 2,

    GO_DEFECTIVE_GENERATOR          = 201735,
    GO_LEAKY_STOVE                  = 201733,
    GO_FLAMMABLE_BED                = 201734,
    SPELL_LEAKY_STOVE               = 70238,
    SPELL_FLAMMABLE_BED             = 70245,
    SPELL_GASBOT_PANEL              = 70253,

    // What a target shows once used: an unseen helper with the visual aura (the "Master" spells' summons, which
    // nothing cast)
    NPC_OVERLOADED_GENERATOR        = 37561,
    NPC_STOVE_LEAK                  = 37590,
    NPC_SMOLDERING_BED              = 37594,
    SPELL_OVERLOADED_GENERATOR_VISUAL = 70226,
    SPELL_STOVE_LEAK_VISUAL         = 70236,
    SPELL_SMOLDERING_BED_VISUAL     = 70250,
};

// Used in its turn (the quest under way, the target's own step not yet done): the sparks, the leak or the smoulder
// stay on it for five minutes
static void Show447Visual(GameObject* target, Player* player, bool reportUse, uint32 npc, uint32 visual)
{
    if (reportUse || player->GetQuestStatus(QUEST_447) != QUEST_STATUS_INCOMPLETE
        || player->GetReqKillOrCastCurrentCount(QUEST_447, -int32(target->GetEntry())) > 0)
        return;

    if (Creature* helper = target->SummonCreature(npc, target->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 300000))
        helper->CastSpell(helper, visual, true);
}

// 447 goes in order: the generator, the stove, the bed, then the gasbot's panel. Used out of order, a target answers
// with the client's own error (SPELL_FAILED_CUSTOM_ERROR_80: "First, Overload the Defective Generator, Activate the
// Leaky Stove, and Drop a Cigar on the Flammable Bed."), as on retail.
// A click arrives twice, as the use and as the client's report of it: both are refused, the error is sent once.
static bool Refuse447OutOfOrder(Player* player, uint32 previousTarget, uint32 spellId, bool reportUse)
{
    if (player->GetQuestStatus(QUEST_447) != QUEST_STATUS_INCOMPLETE)
        return false;
    if (player->GetReqKillOrCastCurrentCount(QUEST_447, -int32(previousTarget)) > 0)
        return false;

    if (!reportUse)
        if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId))
            Spell::SendCastResult(player, spellInfo, 0, SPELL_FAILED_CUSTOM_ERROR, SPELL_CUSTOM_ERROR_80);
    return true;
}

// 201735 Defective Generator: the first
struct go_kezan_defective_generator : public GameObjectAI
{
    go_kezan_defective_generator(GameObject* go) : GameObjectAI(go) { }

    bool GossipHello(Player* player, bool reportUse) override
    {
        Show447Visual(me, player, reportUse, NPC_OVERLOADED_GENERATOR, SPELL_OVERLOADED_GENERATOR_VISUAL);
        return false;
    }
};

// 201733 Leaky Stove: after the generator
struct go_kezan_leaky_stove : public GameObjectAI
{
    go_kezan_leaky_stove(GameObject* go) : GameObjectAI(go) { }

    bool GossipHello(Player* player, bool reportUse) override
    {
        if (Refuse447OutOfOrder(player, GO_DEFECTIVE_GENERATOR, SPELL_LEAKY_STOVE, reportUse))
            return true;
        Show447Visual(me, player, reportUse, NPC_STOVE_LEAK, SPELL_STOVE_LEAK_VISUAL);
        return false;
    }
};

struct go_kezan_gasbot_control_panel : public GameObjectAI
{
    go_kezan_gasbot_control_panel(GameObject* go) : GameObjectAI(go) { }

    bool GossipHello(Player* player, bool reportUse) override
    {
        // The last of the four: after the bed
        if (Refuse447OutOfOrder(player, GO_FLAMMABLE_BED, SPELL_GASBOT_PANEL, reportUse))
            return true;

        if (player->GetGUID() != _playerGUID)
            return false;

        if (GameObject* glitter = ObjectAccessor::GetGameObject(*me, _glitterGUID))
            glitter->DespawnOrUnsummon();
        else if (GameObject* nearGlitter = me->FindNearestGameObject(GO_GASBOT_PANEL_GLITTER, 2.0f))
            nearGlitter->DespawnOrUnsummon();
        _glitterGUID.Clear();

        // Not handled: the panel's own spell (70253) goes on and summons the gasbot
        return false;
    }

    void SetGUID(ObjectGuid const& guid, int32 id) override
    {
        if (id == DATA_PANEL_PLAYER)
            _playerGUID = guid;
        else if (id == DATA_PANEL_GLITTER)
            _glitterGUID = guid;
    }

private:
    ObjectGuid _playerGUID;
    ObjectGuid _glitterGUID;
};

struct go_kezan_flammable_bed : public GameObjectAI
{
    go_kezan_flammable_bed(GameObject* go) : GameObjectAI(go) { }

    // After the stove
    bool GossipHello(Player* player, bool reportUse) override
    {
        if (Refuse447OutOfOrder(player, GO_LEAKY_STOVE, SPELL_FLAMMABLE_BED, reportUse))
            return true;
        Show447Visual(me, player, reportUse, NPC_SMOLDERING_BED, SPELL_SMOLDERING_BED_VISUAL);
        return false;
    }

    void OnStateChanged(uint32 state, Unit* unit) override
    {
        if (state != GO_ACTIVATED || !unit || !unit->IsPlayer())
            return;

        if (GameObject* panel = me->FindNearestGameObject(GO_GASBOT_CONTROL_PANEL, 50.0f))
            if (GameObject* glitter = panel->SummonGameObject(GO_GASBOT_PANEL_GLITTER, panel->GetPositionX(),
                panel->GetPositionY(), panel->GetPositionZ(), panel->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, 300))
            {
                panel->AI()->SetGUID(unit->GetGUID(), DATA_PANEL_PLAYER);
                panel->AI()->SetGUID(glitter->GetGUID(), DATA_PANEL_GLITTER);
            }
    }
};

/*######
## 37602 Claims Adjuster, 207751 his shredder (447): they come to the headquarters once the player has done all of 447
## (retail, 2026-11-08: they appear with the last credit; TDB's SmartAI took a "Phase - Quest Zone-Specific 05" aura
## from the player at the turn-in). Seen by a player whose 447 is complete or turned in.
######*/

static bool Sees447Adjuster(Player const* seer)
{
    if (seer->IsGameMaster())
        return true;
    QuestStatus status = seer->GetQuestStatus(QUEST_447);
    return status == QUEST_STATUS_COMPLETE || status == QUEST_STATUS_REWARDED;
}

struct npc_kezan_claims_adjuster : public ScriptedAI
{
    npc_kezan_claims_adjuster(Creature* creature) : ScriptedAI(creature) { }

    enum
    {
        SAY_CLAIMS_ADJUSTER_447 = 0,
    };

    bool CanBeSeen(Player const* seer) override
    {
        return Sees447Adjuster(seer);
    }

    void sGossipHello(Player* /*player*/) override
    {
        PlayRandomGossipEmote(me);
    }

    void sQuestReward(Player* player, Quest const* quest, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_447)
            Talk(SAY_CLAIMS_ADJUSTER_447, player);
    }
};

// 201745 447 Fire: the headquarters burns for a player whose 447 is done (complete or turned in), not before; dropping
// the quest puts it out again
struct go_kezan_447_fire : public GameObjectAI
{
    go_kezan_447_fire(GameObject* go) : GameObjectAI(go) { }

    bool CanBeSeen(Player const* seer) override
    {
        return Sees447Adjuster(seer);
    }
};

struct go_kezan_claims_adjuster_shredder : public GameObjectAI
{
    go_kezan_claims_adjuster_shredder(GameObject* go) : GameObjectAI(go) { }

    bool CanBeSeen(Player const* seer) override
    {
        return Sees447Adjuster(seer);
    }
};

/*######
## 37676 Hot Rod (Life Savings): Sassy drives the player to the Trade Prince's yacht
######*/

enum HotRodLifeSavings
{
    POINT_DRIVE_BASE                = 100,      // + the route point's index
    POINT_JUMP_ONTO_THE_YACHT       = 99,
    ROUTE_GO_AROUND                 = 12,       // the detour north of the football field
    ROUTE_MAIN_STREET               = 16,       // the long dash up the street
    ROUTE_SHORE                     = 30,       // the knockback ends and the car jumps onto the yacht
    ROUTE_ON_DECK                   = 32,       // where it lands
    SAY_BREAKUP_AT_THE_DOCK         = 3,        // Candy Cane to a goblin man, Chip Endale to a goblin woman

    // Sassy at the wheel (37680; broadcast texts 37256-37259, which TDB gave her no creature_text for)
    SAY_SASSY_PEDAL_TO_THE_METAL    = 0,
    SAY_SASSY_GO_AROUND             = 1,
    SAY_SASSY_OUT_OF_THE_WAY        = 2,
    SAY_SASSY_DOUBLE_CROSSED        = 3,

    EVENT_HOT_ROD_SUMMON_SASSY      = 1,
    EVENT_HOT_ROD_ARRIVED,
    EVENT_HOT_ROD_DRIVE_ON,
};

// Sassy's drive from the headquarters through town to the Trade Prince's yacht: ArkCORE-NG's path 37676 (GPL-2.0, by
// Madman, 2017-03-05; NomadicWow sql/old/world/2016/10/2016_10_05_04_world.sql), its points 31 and 34 repeating 30
// and 33. The car jumps from 30, on the jetty, onto the yacht at 32 (it flew there, circling over the water, as a
// flyer it is not). Driven point by point: the core
// refuses a move shorter than a tenth of a yard (MoveSplineInitArgs::_checkPathLengths), and the path repeats two of
// its points and starts where the car appears, so as a waypoint path it stalled at once.
std::array<Position, 35> const LifeSavingsDrive =
{ {
    { -8425.034f, 1324.767f, 101.918f, 1.625f }, // 0
    { -8419.421f, 1333.294f, 102.392f, 0.121f }, // 1
    { -8400.216f, 1340.02f, 101.996f, 0.337f }, // 2
    { -8374.318f, 1349.09f, 101.71f, 0.337f }, // 3
    { -8356.793f, 1343.94f, 97.13f, 6.07f }, // 4
    { -8334.182f, 1331.252f, 87.026f, 5.772f }, // 5
    { -8312.973f, 1313.47f, 85.942f, 5.854f }, // 6
    { -8251.665f, 1291.138f, 44.664f, 6.102f }, // 7
    { -8129.597f, 1297.265f, 20.099f, 0.031f }, // 8
    { -8124.546f, 1350.449f, 17.529f, 1.546f }, // 9
    { -8121.129f, 1384.686f, 23.695f, 1.476f }, // 10
    { -8109.652f, 1442.592f, 10.324f, 1.35f }, // 11
    { -8106.67f, 1465.324f, 10.091f, 1.472f }, // 12
    { -8118.039f, 1477.232f, 10.28f, 2.316f }, // 13
    { -8117.421f, 1485.756f, 10.336f, 1.484f }, // 14
    { -8103.097f, 1500.302f, 9.948f, 0.796f }, // 15
    { -8096.165f, 1650.852f, 10.135f, 1.625f }, // 16
    { -8097.564f, 1682.969f, 14.167f, 1.586f }, // 17
    { -8124.712f, 1710.631f, 27.031f, 2.218f }, // 18
    { -8131.997f, 1720.4f, 31.389f, 2.167f }, // 19
    { -8136.745f, 1735.666f, 34.389f, 1.872f }, // 20
    { -8136.068f, 1754.767f, 34.431f, 1.464f }, // 21
    { -8120.522f, 1778.391f, 29.24f, 0.989f }, // 22
    { -8108.241f, 1788.452f, 29.472f, 0.686f }, // 23
    { -8095.225f, 1804.285f, 33.631f, 0.883f }, // 24
    { -8082.288f, 1826.98f, 41.07f, 1.056f }, // 25
    { -8068.063f, 1822.022f, 42.927f, 5.956f }, // 26
    { -8040.964f, 1824.189f, 41.814f, 6.141f }, // 27
    { -7994.833f, 1822.505f, 22.093f, 0.36f }, // 28
    { -7956.281f, 1836.39f, 9.489f, 0.266f }, // 29
    { -7886.333f, 1834.303f, 4.354f, 6.267f }, // 30
    { -7886.333f, 1834.303f, 4.354f, 0.172f }, // 31
    { -7859.835f, 1840.59f, 7.685f, 6.263f }, // 32
    { -7849.395f, 1840.313f, 7.588f, 6.255f }, // 33
    { -7849.395f, 1840.313f, 7.588f, 6.255f }, // 34
} };

struct npc_kezan_hot_rod_life_savings : public VehicleAI
{
    npc_kezan_hot_rod_life_savings(Creature* creature) : VehicleAI(creature) { }

    void Reset() override
    {
        me->SetSpeed(MOVE_RUN, 2.4f);
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner ? summoner->ToPlayer() : nullptr)
            _playerGUID = player->GetGUID();
        // Sassy drives (seat 0); ArkCORE's database put her in as the car's accessory
        _events.ScheduleEvent(EVENT_HOT_ROD_SUMMON_SASSY, 500ms);
    }

    void PassengerBoarded(Unit* passenger, int8 /*seatId*/, bool apply) override
    {
        if (!passenger)
            return;

        if (!apply)
        {
            if (Player* player = passenger->ToPlayer())
            {
                if (Creature* sassy = ObjectAccessor::GetCreature(*me, _sassyGUID))
                    sassy->DespawnOrUnsummon(1s);
                me->DespawnOrUnsummon(1s);
                player->UpdateObjectVisibility();       // Sassy is back on her crate for them
            }
            return;
        }

        if (Player* player = passenger->ToPlayer())
        {
            _playerGUID = player->GetGUID();
            player->CastSpell(player, SPELL_HAND_OVER_THE_KEYS, true);
            player->CastSpell(player, SPELL_HOT_ROD_KNOCKBACK_TRIGGER, true);
            player->UpdateObjectVisibility();           // Sassy drives: her crate is empty for them
        }
        else if (passenger->GetEntry() == NPC_SASSY_DRIVER)
            _sassyGUID = passenger->GetGUID();

        if (!_started && _playerGUID && _sassyGUID)
        {
            _started = true;
            _next = 0;
            SassySays(SAY_SASSY_PEDAL_TO_THE_METAL);
            DriveOn();
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type == EFFECT_MOTION_TYPE && id == POINT_JUMP_ONTO_THE_YACHT)
        {
            // On deck: the Trade Prince's welcome is not what Sassy hoped for
            SassySays(SAY_SASSY_DOUBLE_CROSSED);
            _next = ROUTE_ON_DECK + 1;
            _events.ScheduleEvent(EVENT_HOT_ROD_DRIVE_ON, 1ms);
            return;
        }

        if (type != POINT_MOTION_TYPE || id < POINT_DRIVE_BASE)
            return;

        uint32 index = id - POINT_DRIVE_BASE;
        if (index == ROUTE_SHORE)
        {
            // The knockback ends by the shore and the car jumps from the jetty onto the yacht
            if (Player* player = ObjectAccessor::GetPlayer(*me, _playerGUID))
                player->RemoveAurasDueToSpell(SPELL_HOT_ROD_KNOCKBACK_TRIGGER);
            me->GetMotionMaster()->MoveJump(LifeSavingsDrive[ROUTE_ON_DECK], 20.0f, 12.0f, POINT_JUMP_ONTO_THE_YACHT);
            return;
        }

        Reached(index);
        _next = index + 1;
        _events.ScheduleEvent(EVENT_HOT_ROD_DRIVE_ON, 1ms);
    }

    void UpdateAI(uint32 diff) override
    {
        VehicleAI::UpdateAI(diff);
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_HOT_ROD_SUMMON_SASSY:
                    if (TempSummon* sassy = me->SummonCreature(NPC_SASSY_DRIVER, me->GetPosition(),
                        TEMPSUMMON_MANUAL_DESPAWN))
                        sassy->EnterVehicle(me, 0);
                    break;
                case EVENT_HOT_ROD_DRIVE_ON:
                    DriveOn();
                    break;
                case EVENT_HOT_ROD_ARRIVED:
                    // At the yacht: the player's sweetheart breaks up with them (ArkCORE-NG), and they get out
                    if (Player* player = ObjectAccessor::GetPlayer(*me, _playerGUID))
                        if (Creature* ex = me->FindNearestCreature(player->getGender() == GENDER_MALE ? 35053u : 35054u,
                            25.0f))
                            ex->AI()->Talk(SAY_BREAKUP_AT_THE_DOCK, player);
                    if (Vehicle* vehicle = me->GetVehicleKit())
                        vehicle->RemoveAllPassengers();
                    break;
                default:
                    break;
            }
        }
    }

private:
    // To the next point more than a yard and a half away (passing the ones the car already stands on)
    void DriveOn()
    {
        while (_next < LifeSavingsDrive.size() && _next != ROUTE_SHORE
            && me->GetExactDist(&LifeSavingsDrive[_next]) < 1.5f)
            Reached(_next++);

        if (_next >= LifeSavingsDrive.size())
        {
            if (!_arrived)
            {
                _arrived = true;
                _events.ScheduleEvent(EVENT_HOT_ROD_ARRIVED, 3500ms);
            }
            return;
        }

        Position const& point = LifeSavingsDrive[_next];
        me->GetMotionMaster()->MovePoint(POINT_DRIVE_BASE + _next, point, false);
    }

    void Reached(uint32 index)
    {
        if (index == ROUTE_GO_AROUND)
            SassySays(SAY_SASSY_GO_AROUND);
        else if (index == ROUTE_MAIN_STREET)
            SassySays(SAY_SASSY_OUT_OF_THE_WAY);
    }

    void SassySays(uint8 group)
    {
        if (Creature* sassy = ObjectAccessor::GetCreature(*me, _sassyGUID))
            sassy->AI()->Talk(group, ObjectAccessor::GetPlayer(*me, _playerGUID));
    }

    EventMap _events;
    ObjectGuid _playerGUID;
    ObjectGuid _sassyGUID;
    uint32 _next = 0;
    bool _started = false;
    bool _arrived = false;
};

/*######
## 66301, 70330 Hot Rod - Knockback: only the citizens and looters close in front of the car
######*/

class spell_kezan_hot_rod_knockback : public SpellScript
{
    PrepareSpellScript(spell_kezan_hot_rod_knockback);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        // Rolling with my Homies picks its targets within 10 yards; Life Savings' trigger within 5
        float range = GetSpellInfo()->Id == SPELL_HOT_ROD_KNOCKBACK ? 10.0f : 5.0f;
        Unit* caster = GetCaster();
        targets.remove_if([caster, range](WorldObject* target)
        {
            Creature* creature = target->ToCreature();
            return !creature || !creature->IsAlive() || !IsHotRodTarget(creature->GetEntry())
                || caster->GetExactDist(creature) > range;
        });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_kezan_hot_rod_knockback::FilterTargets,
            EFFECT_0, TARGET_UNIT_CONE_ENTRY);
    }
};

/*######
## A Bazillion Macaroons?!: the bombardment. 43359 ELM General Purpose Bunny Infinite Hide Body (Kezan's sky anchors)
## and 37748 A Bazillion Macaroons Fiery Boulder Caster
######*/

// While A Bazillion Macaroons?! is under way (phase 382), fiery boulders from the erupting Mount Kajaro streak across
// the sky and crash all over the town (retail screenshots, 2026-11-08). TDB has twenty of these bunnies high over the
// west side of town in that phase, with no script. They are anchors: once a player nearby has the quest, each places
// a flying boulder caster at itself, which stays and throws a boulder (70097) every 25 seconds. The anchors' spawns
// carry their script (creature.ScriptName), not the template, which other zones share. The same caster entry throws
// Gallywix's one boulder at his window.
// The boulders strike all over the town, at the retail positions of the other hidden helpers of this entry in Kezan
// (map positions converted with the client's WorldMapArea 605; the same set places TDB's sky bunnies to a yard).
// Heights are not recorded: the boulder lands on the highest surface below z 150 (roof or ground). Orientation holds
// the spread in yards.
std::array<Position, 51> const BoulderImpactAreas =
{ {
    { -8377.32f, 1312.29f, 109.06f, 3.0f },  // where a retail boulder struck (.gps 2026-11-08)
    { -8099.3f, 1619.4f, 150.0f, 3.0f },
    { -8099.3f, 1550.5f, 150.0f, 3.0f },
    { -8099.3f, 1480.2f, 150.0f, 3.0f },
    { -8100.2f, 1409.9f, 150.0f, 3.0f },
    { -8100.2f, 1374.7f, 150.0f, 3.0f },
    { -8133.5f, 1664.0f, 150.0f, 3.0f },
    { -8149.7f, 1342.3f, 150.0f, 3.0f },
    { -8201.9f, 1531.5f, 150.0f, 3.0f },
    { -8203.7f, 1461.2f, 150.0f, 3.0f },
    { -8203.7f, 1662.7f, 150.0f, 3.0f },
    { -8206.4f, 1392.3f, 150.0f, 3.0f },
    { -8219.9f, 1345.0f, 150.0f, 3.0f },
    { -8238.8f, 1662.7f, 150.0f, 3.0f },
    { -8309.0f, 1661.3f, 150.0f, 3.0f },
    { -8323.4f, 1350.4f, 150.0f, 3.0f },
    { -8353.1f, 1404.4f, 150.0f, 3.0f },
    { -8356.7f, 1473.4f, 150.0f, 3.0f },
    { -8361.2f, 1545.1f, 150.0f, 3.0f },
    { -8363.9f, 1596.4f, 150.0f, 3.0f },
    { -8367.5f, 1651.9f, 150.0f, 3.0f },
    { -8379.2f, 1660.0f, 150.0f, 3.0f },
    { -8392.7f, 1353.1f, 150.0f, 3.0f },
    { -8420.6f, 1322.0f, 150.0f, 3.0f },
    { -8497.1f, 1358.5f, 150.0f, 3.0f },
    { -8102.9f, 1891.2f, 150.0f, 3.0f },
    { -8118.2f, 1946.6f, 150.0f, 3.0f },
    { -8072.3f, 1926.4f, 150.0f, 3.0f },
    { -8068.7f, 1865.5f, 150.0f, 3.0f },
    { -8108.3f, 1861.5f, 150.0f, 3.0f },
    { -8112.8f, 1923.6f, 150.0f, 3.0f },
    { -8300.0f, 1785.7f, 150.0f, 3.0f },
    { -8297.3f, 1716.8f, 150.0f, 3.0f },
    { -8293.7f, 1612.7f, 150.0f, 3.0f },
    { -8291.0f, 1543.7f, 150.0f, 3.0f },
    { -8287.4f, 1441.0f, 150.0f, 3.0f },
    { -8282.9f, 1336.8f, 150.0f, 3.0f },
    { -8279.3f, 1232.7f, 150.0f, 3.0f },
    { -8278.4f, 1219.2f, 150.0f, 3.0f },
    { -8312.6f, 1221.9f, 150.0f, 3.0f },
    { -8416.1f, 1227.3f, 150.0f, 3.0f },
    { -8484.5f, 1231.4f, 150.0f, 3.0f },
    { -8526.8f, 1178.7f, 150.0f, 3.0f },
    { -8428.7f, 1154.3f, 150.0f, 3.0f },
    { -8363.0f, 1139.4f, 150.0f, 3.0f },
    { -8297.3f, 1123.2f, 150.0f, 3.0f },
    { -8455.7f, 1335.5f, 150.0f, 3.0f },
    { -8421.5f, 1335.5f, 150.0f, 3.0f },
    { -8352.2f, 1334.1f, 150.0f, 3.0f },
    { -8283.8f, 1332.8f, 150.0f, 3.0f },
    { -8214.5f, 1332.8f, 150.0f, 3.0f },
} };

// Whether someone in Kezan is under the bombardment: it starts with A Bazillion Macaroons?! and goes on, once the
// quest is turned in, until the goblins leave Kezan
static bool AnyoneOnMacaroonsNear(Creature const* creature)
{
    for (MapReference const& ref : creature->GetMap()->GetPlayers())
    {
        Player* player = ref.GetSource();
        if (!player || player->GetZoneId() != 4737)
            continue;

        QuestStatus status = player->GetQuestStatus(14120);
        if (status == QUEST_STATUS_INCOMPLETE || status == QUEST_STATUS_COMPLETE || player->GetQuestRewardStatus(14120))
            return true;
    }
    return false;
}

struct npc_kezan_fiery_boulder_caster : public ScriptedAI
{
    npc_kezan_fiery_boulder_caster(Creature* creature) : ScriptedAI(creature) { }

    enum
    {
        EVENT_THROW_BOULDER = 1,
        EVENT_OWN_BOULDER = 2,
        SPELL_SKY_FIERY_BOULDER = 70097,
    };

    void IsSummonedBy(WorldObject* summoner) override
    {
        // Seen from any distance: a boulder from a caster the client does not know is not drawn
        me->SetVisibilityDistanceOverride(VisibilityDistanceType::Infinite);
        Creature const* anchor = summoner ? summoner->ToCreature() : nullptr;
        if (anchor && anchor->GetScriptName() == "npc_kezan_balcony_boulder_bunny")
        {
            // The balcony's: one fiery boulder (70097, the bombardment's) on its own spot, the helper's, a second
            // after it appears (the client must know it first), then it goes
            _target.Relocate(anchor);
            _events.ScheduleEvent(EVENT_OWN_BOULDER, 1s);
            me->DespawnOrUnsummon(10s);
            return;
        }

        // A bombardment anchor's: every 25 seconds on its own clock, started at a random moment, so the twenty stay
        // out of step
        _bombards = true;
        _events.ScheduleEvent(EVENT_THROW_BOULDER, Milliseconds(urand(1000, 25000)));
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);
        uint32 eventId = _events.ExecuteEvent();
        if (eventId == EVENT_OWN_BOULDER)
        {
            SpellCastResult res = me->CastSpell(_target.GetPositionX(), _target.GetPositionY(), _target.GetPositionZ(),
                SPELL_SKY_FIERY_BOULDER, true);
            LOG_ERROR("scripts", "[kezan-debug] balcony caster at {} {} {} throws 70097 at {} {} {}: result {}",
                me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), _target.GetPositionX(),
                _target.GetPositionY(), _target.GetPositionZ(), uint32(res));
            return;
        }
        if (!_bombards || eventId != EVENT_THROW_BOULDER)
            return;

        // Once nobody in Kezan is under the bombardment any more, it goes; its anchor places a new one for the next
        // player
        if (!AnyoneOnMacaroonsNear(me))
        {
            me->DespawnOrUnsummon(1ms);
            return;
        }

        Position const& area = Acore::Containers::SelectRandomContainerElement(BoulderImpactAreas);
        float angle = frand(0.0f, 2.0f * float(M_PI));
        float dist = frand(0.0f, area.GetOrientation());
        float x = area.GetPositionX() + dist * std::cos(angle);
        float y = area.GetPositionY() + dist * std::sin(angle);
        float z = me->GetMapHeight(x, y, area.GetPositionZ() + 15.0f);
        if (z <= INVALID_HEIGHT)
            z = area.GetPositionZ();
        me->CastSpell(x, y, z, SPELL_SKY_FIERY_BOULDER, true);
        _events.ScheduleEvent(EVENT_THROW_BOULDER, 25s);
    }

private:
    EventMap _events;
    bool _bombards = false;
    Position _target;
};

struct npc_kezan_sky_boulder_bunny : public ScriptedAI
{
    npc_kezan_sky_boulder_bunny(Creature* creature) : ScriptedAI(creature) { }

    enum
    {
        EVENT_CHECK_QUEST = 1,
        NPC_BOULDER_CASTER = 37748,     // A Bazillion Macaroons Fiery Boulder Caster: invisible, flies
    };

    void Reset() override
    {
        _events.Reset();
        _events.ScheduleEvent(EVENT_CHECK_QUEST, 2s);
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);
        if (_events.ExecuteEvent() != EVENT_CHECK_QUEST)
            return;

        // The anchors stand in chapter 5 (phase 382); the bombardment goes on in chapters 6 and 7 (383, 384), so their
        // casters, which take their phases, are seen there too
        if (!_phased)
        {
            me->AddPhase(383);
            me->AddPhase(384);
            _phased = true;
        }

        // An anchor: once someone in Kezan is under the bombardment, it places its flying boulder caster here, which
        // stays and throws (npc_kezan_fiery_boulder_caster)
        if (AnyoneOnMacaroonsNear(me) && !ObjectAccessor::GetCreature(*me, _casterGUID))
            if (Creature* caster = me->SummonCreature(NPC_BOULDER_CASTER, me->GetPositionX(), me->GetPositionY(),
                me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
                _casterGUID = caster->GetGUID();
        _events.ScheduleEvent(EVENT_CHECK_QUEST, 3s);
    }

private:
    EventMap _events;
    ObjectGuid _casterGUID;
    bool _phased = false;
};

/*######
## 43359 the balcony helper: a fiery boulder crashes onto Gallywix's balcony as the player takes A Bazillion
## Macaroons?! (retail recording, 2026-11-08). Its own spawn (MaelstromCore) in phase 382; nothing in TDB did it.
######*/

struct npc_kezan_balcony_boulder_bunny : public ScriptedAI
{
    npc_kezan_balcony_boulder_bunny(Creature* creature) : ScriptedAI(creature) { }

    enum
    {
        EVENT_CHECK_QUEST = 1,
        QUEST_A_BAZILLION_MACAROONS = 14120,
        NPC_BOULDER_CASTER = 37748,
    };

    void Reset() override
    {
        _events.Reset();
        _events.ScheduleEvent(EVENT_CHECK_QUEST, 1s);
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);
        if (_events.ExecuteEvent() != EVENT_CHECK_QUEST)
            return;

        // Each time a player nearby takes the quest (again, after abandoning it), it places the boulder caster on
        // itself, as the bombardment's anchors do, which throws once
        std::list<Player*> players;
        Acore::AnyPlayerInObjectRangeCheck check(me, 100.0f);
        Acore::PlayerListSearcher<Acore::AnyPlayerInObjectRangeCheck> searcher(me, players, check);
        Cell::VisitObjects(me, searcher, 100.0f);
        GuidSet onQuest;
        for (Player* player : players)
            if (player->GetQuestStatus(QUEST_A_BAZILLION_MACAROONS) == QUEST_STATUS_INCOMPLETE
                || player->GetQuestStatus(QUEST_A_BAZILLION_MACAROONS) == QUEST_STATUS_COMPLETE)
                onQuest.insert(player->GetGUID());

        bool fresh = false;
        for (ObjectGuid const& guid : onQuest)
            if (!_seen.count(guid))
                fresh = true;
        _seen = std::move(onQuest);
        if (fresh)
        {
            Creature* caster = me->SummonCreature(NPC_BOULDER_CASTER, me->GetPositionX(), me->GetPositionY(),
                me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN);
            LOG_ERROR("scripts", "[kezan-debug] balcony helper {} sees 14120 taken: caster {}", me->GetSpawnId(),
                caster ? caster->GetGUID().ToString() : "NONE");
        }
        _events.ScheduleEvent(EVENT_CHECK_QUEST, 1s);
    }

private:
    EventMap _events;
    GuidSet _seen;
};

/*######
## 67435 Hobart's Ingenious Cap of Mook Foolery (Waltz Right In): the mook's see-through silhouette around the player
## The cap makes the player a vehicle (1362) and triggers 70467 every 5 seconds, which the client lacks; on retail the
## Mook Disguise (48925, its translucence the aura 75098) rides in that seat. Nothing placed it: the cap does now.
######*/

class spell_kezan_mook_foolery : public AuraScript
{
    PrepareAuraScript(spell_kezan_mook_foolery);

    enum
    {
        NPC_MOOK_DISGUISE = 48925,
    };

    void PlaceDisguise()
    {
        Unit* target = GetTarget();
        Vehicle* kit = target ? target->GetVehicleKit() : nullptr;
        if (!kit || kit->GetPassenger(0))
            return;

        if (Creature* disguise = target->SummonCreature(NPC_MOOK_DISGUISE, target->GetPosition(),
            TEMPSUMMON_MANUAL_DESPAWN))
        {
            _disguiseGUID = disguise->GetGUID();
            disguise->EnterVehicle(target, 0);
        }
    }

    void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        // Applied while the client still loads (logging in, a loading screen), the vehicle and its silhouette were set
        // up unseen and the silhouette stood idle, not mirroring the player. The cap goes again at once and comes back
        // when the client has the world loaded (CMSG_SET_ACTIVE_MOVER checks the area's auras), as on entering the villa.
        if (Player* player = GetTarget()->ToPlayer())
            if (player->GetSession()->PlayerLoading() || player->IsBeingTeleported())
            {
                GetAura()->SetDuration(1);
                return;
            }
        PlaceDisguise();
    }

    // The missing 70467: the silhouette is put back if it ever left the seat
    void HandlePeriodic(AuraEffect const* /*aurEff*/)
    {
        PreventDefaultAction();
        PlaceDisguise();
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetTarget())
            if (Creature* disguise = ObjectAccessor::GetCreature(*target, _disguiseGUID))
                disguise->DespawnOrUnsummon();
        _disguiseGUID.Clear();
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_kezan_mook_foolery::AfterApply, EFFECT_1,
            SPELL_AURA_SET_VEHICLE_ID, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_kezan_mook_foolery::HandlePeriodic, EFFECT_2,
            SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_kezan_mook_foolery::AfterRemove, EFFECT_1,
            SPELL_AURA_SET_VEHICLE_ID, AURA_EFFECT_HANDLE_REAL);
    }

private:
    ObjectGuid _disguiseGUID;
};

// Logging out takes the cap off while the player is still in the world, as leaving the villa does: the silhouette goes
// with it instead of being thrown out of its seat and left standing. A fight takes the cap off too (an attack breaks
// the disguise); once it is over the area's auras are checked again and the cap comes back, as on retail.
class player_kezan_mook_foolery : public PlayerScript
{
public:
    player_kezan_mook_foolery() : PlayerScript("player_kezan_mook_foolery",
        { PLAYERHOOK_ON_LOGOUT, PLAYERHOOK_ON_PLAYER_LEAVE_COMBAT, PLAYERHOOK_ON_QUEST_ABANDON }) { }

    // 447 dropped: its fires, the Claims Adjuster and his shredder are no longer seen
    void OnPlayerQuestAbandon(Player* player, uint32 questId) override
    {
        if (questId == QUEST_447)
            player->UpdateObjectVisibility();
    }

    void OnPlayerLogout(Player* player) override
    {
        player->RemoveAurasDueToSpell(67435);
    }

    void OnPlayerLeaveCombat(Player* player) override
    {
        if (player->IsInWorld() && player->IsAlive())
            player->UpdateAreaDependentAuras(player->GetAreaId());
    }
};

// 48925 Mook Disguise: exists only in the cap's seat. Thrown off (the player logged out, the seat went), it goes at
// the next update rather than stand about on its own; the cap seats a new one when the player is back. One that never
// got seated goes after a moment.
struct npc_kezan_mook_disguise : public ScriptedAI
{
    npc_kezan_mook_disguise(Creature* creature) : ScriptedAI(creature) { }

    void UpdateAI(uint32 diff) override
    {
        if (me->GetVehicle())
        {
            _seated = true;
            return;
        }

        _unseated += diff;
        if (_seated || _unseated >= 1500)
            me->DespawnOrUnsummon();
    }

private:
    bool _seated = false;
    uint32 _unseated = 0;
};

/*######
## 35613 Keensnout Potbelly (Waltz Right In): Gallywix's pigs sniff through Hobart's cap. Each walks at the side of
## its handler, the mook leading its formation on the patrol round the pool (builder: KEZAN_PIG_PATROLS), who therefore
## stays hostile though the cap makes the other mooks neutral (retail).
######*/

struct npc_kezan_keensnout_potbelly : public ScriptedAI
{
    npc_kezan_keensnout_potbelly(Creature* creature) : ScriptedAI(creature) { }

    enum
    {
        SPELL_BOAR_CHARGE = 3385,
        FACTION_PIG_HANDLER = 16,                       // the pigs' own, hostile; Hobart's cap fools only 960
        EVENT_FIND_HANDLER = 1,
    };

    void Reset() override
    {
        _events.Reset();
        _events.ScheduleEvent(EVENT_FIND_HANDLER, 1s);
    }

    void JustEngagedWith(Unit* who) override
    {
        DoCast(who, SPELL_BOAR_CHARGE);
        if (Creature* handler = ObjectAccessor::GetCreature(*me, _handlerGUID))
            if (handler->IsAlive() && !handler->IsInCombat())
                handler->AI()->AttackStart(who);
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);
        if (_events.ExecuteEvent() == EVENT_FIND_HANDLER)
        {
            // The formation (leader and pig, following, fighting and evading together) moves it; the handler is its
            // leader, kept hostile, also after a respawn
            Creature* handler = nullptr;
            if (CreatureGroup* group = me->GetFormation())
                handler = group->GetLeader();
            if (handler && handler != me && handler->IsAlive())
            {
                _handlerGUID = handler->GetGUID();
                if (handler->GetFaction() != FACTION_PIG_HANDLER)
                    handler->SetFaction(FACTION_PIG_HANDLER);
            }
            _events.ScheduleEvent(EVENT_FIND_HANDLER, 5s);
        }

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

private:
    EventMap _events;
    ObjectGuid _handlerGUID;
};

/*######
## 35222 Trade Prince Gallywix: TDB's SmartAI yells, and Life Savings sends the player to the Lost Isles
######*/

struct npc_kezan_trade_prince_gallywix : public ScriptedAI
{
    npc_kezan_trade_prince_gallywix(Creature* creature) : ScriptedAI(creature) { }

    enum
    {
        SAY_GALLYWIX_IDLE = 0,
        EVENT_GALLYWIX_IDLE = 1,
    };

    void Reset() override
    {
        _events.Reset();
        _events.ScheduleEvent(EVENT_GALLYWIX_IDLE, 35s, 60s);
    }

    void sQuestReward(Player* player, Quest const* quest, uint32 /*opt*/) override
    {
        // The quest's reward spell (91847) plays the movie of Kezan's end; this one teleports and binds the
        // player to the Lost Isles (ArkCORE teleported by hand to the same spot)
        if (quest->GetQuestId() == QUEST_LIFE_SAVINGS)
            player->CastSpell(player, SPELL_TELEPORT_TO_LOST_ISLES, true);
        // The homies go their own way once the macaroons are delivered
        else if (quest->GetQuestId() == QUEST_BAZILLION_MACAROONS)
            DismissHomies(player);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!me->IsInCombat())
        {
            _events.Update(diff);
            while (uint32 eventId = _events.ExecuteEvent())
            {
                if (eventId == EVENT_GALLYWIX_IDLE)
                {
                    // "...Especially you, $n!": the line names a nearby player, so it waits for one
                    if (Player* player = me->SelectNearestPlayer(60.0f))
                        Talk(SAY_GALLYWIX_IDLE, player);
                    _events.ScheduleEvent(EVENT_GALLYWIX_IDLE, 35s, 60s);
                }
            }
        }

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

private:
    EventMap _events;
};

} // namespace Kezan

void AddSC_zone_kezan()
{
    using namespace Kezan;
    RegisterCreatureAI(npc_kezan_bamm_megabomb);
    RegisterCreatureAI(npc_kezan_injured_employee);
    RegisterCreatureAI(npc_kezan_sassy_hardwrench);
    RegisterCreatureAI(npc_kezan_megs_dreadshredder);
    new at_kezan_homie("at_kezan_ace", NPC_ACE, SPELL_SUMMON_ACE);
    new at_kezan_homie("at_kezan_gobber", NPC_GOBBER, SPELL_SUMMON_GOBBER);
    new at_kezan_homie("at_kezan_izzy", NPC_IZZY, SPELL_SUMMON_IZZY);
    RegisterCreatureAI(npc_kezan_hot_rod);
    RegisterCreatureAI(npc_kezan_homie);
    RegisterCreatureAI(npc_kezan_fbok_bank_teller);
    RegisterGameObjectAI(go_kezan_disco_ball);
    RegisterCreatureAI(npc_kezan_subject_nine);
    RegisterSpellScript(spell_kezan_kaja_cola);
    RegisterSpellScript(spell_kezan_mook_foolery);
    new player_kezan_mook_foolery();
    RegisterCreatureAI(npc_kezan_hired_looter);
    RegisterCreatureAI(npc_kezan_citizen);
    RegisterCreatureAI(npc_kezan_partygoer);
    RegisterCreatureAI(npc_kezan_bank_vault);
    RegisterSpellScriptWithArgs(spell_kezan_cast_on_hit_unit, "spell_kezan_vault_interact", SPELL_SUMMON_VAULT_VEHICLE);
    RegisterSpellScriptWithArgs(spell_kezan_cast_on_hit_unit, "spell_kezan_aicast_gasbot_master", SPELL_SUMMON_GASBOT);
    RegisterCreatureAI(npc_kezan_coach_crosscheck);
    RegisterCreatureAI(npc_kezan_bilgewater_buccaneer_field);
    RegisterCreatureAI(npc_kezan_bilgewater_buccaneer_roughness);
    RegisterCreatureAI(npc_kezan_steamwheedle_shark);
    RegisterCreatureAI(npc_kezan_bilgewater_buccaneer_goal);
    RegisterSpellScript(spell_kezan_kick_footbomb);
    RegisterCreatureAI(npc_kezan_deathwing);
    RegisterGameObjectAI(go_kezan_kajamite_deposit);
    RegisterCreatureAI(npc_kezan_partygoer_passive);
    RegisterCreatureAI(npc_kezan_gasbot);
    RegisterGameObjectAI(go_kezan_gasbot_control_panel);
    RegisterGameObjectAI(go_kezan_flammable_bed);
    RegisterGameObjectAI(go_kezan_leaky_stove);
    RegisterGameObjectAI(go_kezan_defective_generator);
    RegisterGameObjectAI(go_kezan_claims_adjuster_shredder);
    RegisterGameObjectAI(go_kezan_447_fire);
    RegisterCreatureAI(npc_kezan_claims_adjuster);
    RegisterCreatureAI(npc_kezan_hot_rod_life_savings);
    RegisterSpellScript(spell_kezan_hot_rod_knockback);
    RegisterCreatureAI(npc_kezan_trade_prince_gallywix);
    RegisterCreatureAI(npc_kezan_fiery_boulder_caster);
    RegisterCreatureAI(npc_kezan_sky_boulder_bunny);
    RegisterCreatureAI(npc_kezan_balcony_boulder_bunny);
    RegisterCreatureAI(npc_kezan_keensnout_potbelly);
    RegisterCreatureAI(npc_kezan_mook_disguise);
}
