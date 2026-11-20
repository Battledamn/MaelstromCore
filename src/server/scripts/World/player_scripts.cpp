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

#include "Player.h"
#include "PlayerScript.h"
#include "QuestPackets.h"

enum ApprenticeAnglerQuestEnum
{
    QUEST_APPRENTICE_ANGLER = 8194
};

class QuestApprenticeAnglerPlayerScript : public PlayerScript
{
public:
    QuestApprenticeAnglerPlayerScript() : PlayerScript("QuestApprenticeAnglerPlayerScript", {PLAYERHOOK_ON_PLAYER_COMPLETE_QUEST})
    {
    }

    void OnPlayerCompleteQuest(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_APPRENTICE_ANGLER)
        {
            uint32 level = player->GetLevel();
            int32 moneyRew = 0;
            if (level <= 10)
                moneyRew = 85;
            else if (level <= 60)
                moneyRew = 2300;
            else if (level <= 69)
                moneyRew = 9000;
            else if (level <= 70)
                moneyRew = 11200;
            else if (level <= 79)
                moneyRew = 12000;
            else
                moneyRew = 19000;

            player->ModifyMoney(moneyRew);
            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_MONEY_FROM_QUEST_REWARD, uint32(moneyRew));
            player->SaveToDB(false, false);

            // Send packet with money
            WorldPackets::Quest::QuestGiverQuestComplete questGiverQuestComplete;
            questGiverQuestComplete.QuestId = quest->GetQuestId();
            questGiverQuestComplete.RewardMoney = moneyRew;
            player->SendDirectMessage(questGiverQuestComplete.Write());
        }
    }
};

enum LearnJudgementCredit
{
    SPELL_JUDGEMENT             = 20271,
    NPC_LEARN_JUDGEMENT_CREDIT  = 44420
};

// Paladin starter quests ("Learn Spell: Judgement") credit 44420 when Judgement is learned,
// or on accept when it was learned before the quest was taken.
class player_learn_judgement_credit : public PlayerScript
{
public:
    player_learn_judgement_credit() : PlayerScript("player_learn_judgement_credit",
        { PLAYERHOOK_ON_LEARN_SPELL, PLAYERHOOK_ON_PLAYER_QUEST_ACCEPT }) { }

    void OnPlayerLearnSpell(Player* player, uint32 spellId) override
    {
        if (spellId == SPELL_JUDGEMENT)
            player->KilledMonsterCredit(NPC_LEARN_JUDGEMENT_CREDIT);
    }

    void OnPlayerQuestAccept(Player* player, Quest const* quest) override
    {
        if (!player->HasSpell(SPELL_JUDGEMENT))
            return;

        for (int32 required : quest->RequiredNpcOrGo)
        {
            if (required == NPC_LEARN_JUDGEMENT_CREDIT)
            {
                player->KilledMonsterCredit(NPC_LEARN_JUDGEMENT_CREDIT);
                return;
            }
        }
    }
};

void AddSC_player_scripts()
{
    new QuestApprenticeAnglerPlayerScript();
    new player_learn_judgement_credit();
}
