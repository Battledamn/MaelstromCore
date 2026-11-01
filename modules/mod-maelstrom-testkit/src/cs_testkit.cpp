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
 * MaelstromCore test kit: GM commands that set a character up for class tests.
 *   .testkit tree <1|2|3>  every talent of that talent tree at its highest rank (beyond the 41 points),
 *                          the tree as the active spec's specialization
 *   .testkit ready         full health and power, no cooldowns or Forbearance, training dummies
 *   .testkit reset         the active spec's talents and specialization reset
 * Used by the MaelstromClassTest addon. Not 4.3.4 behaviour, so a module (docs/AZEROTHCORE_WAY.md).
 */

#include "Chat.h"
#include "CommandScript.h"
#include "DBCStores.h"
#include "Player.h"
#include "TemporarySummon.h"

using namespace Acore::ChatCommands;

namespace
{
    // module permissions (2000+, docs/MODULARITY.md)
    rbac::RBACPermissions constexpr RBAC_PERM_COMMAND_TESTKIT_TREE  = rbac::RBACPermissions(2001);
    rbac::RBACPermissions constexpr RBAC_PERM_COMMAND_TESTKIT_READY = rbac::RBACPermissions(2002);
    rbac::RBACPermissions constexpr RBAC_PERM_COMMAND_TESTKIT_RESET = rbac::RBACPermissions(2003);

    uint32 constexpr NPC_TRAINING_DUMMY = 46647;    // level 85
    uint32 constexpr SPELL_FORBEARANCE  = 25771;
    uint32 constexpr DUMMY_COUNT        = 5;        // enough for "4 or more targets" effects
    uint32 constexpr DUMMY_DESPAWN_MS   = 30 * MINUTE * IN_MILLISECONDS;
}

class testkit_commandscript : public CommandScript
{
public:
    testkit_commandscript() : CommandScript("testkit_commandscript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable testkitCommandTable =
        {
            { "tree",  HandleTreeCommand,  RBAC_PERM_COMMAND_TESTKIT_TREE,  Console::No },
            { "ready", HandleReadyCommand, RBAC_PERM_COMMAND_TESTKIT_READY, Console::No },
            { "reset", HandleResetCommand, RBAC_PERM_COMMAND_TESTKIT_RESET, Console::No }
        };

        static ChatCommandTable commandTable =
        {
            { "testkit", testkitCommandTable }
        };
        return commandTable;
    }

    static bool HandleTreeCommand(ChatHandler* handler, uint8 treeNumber)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (treeNumber < 1 || treeNumber > MAX_TALENT_TABS || player->GetLevel() < 10)
        {
            handler->SendSysMessage("Usage: .testkit tree <1|2|3> (the talent window's tabs from the left), level 10 or higher.");
            return false;
        }

        uint32 const talentTabId = GetTalentTabPages(player->getClass())[treeNumber - 1];

        player->resetTalents(true);
        player->LearnPrimaryTalentSpecialization(treeNumber - 1);

        // every talent of the tree at its highest rank, row by row so that prerequisites come first
        std::vector<TalentEntry const*> talents;
        for (uint32 i = 0; i < sTalentStore.GetNumRows(); ++i)
            if (TalentEntry const* talent = sTalentStore.LookupEntry(i))
                if (talent->TalentTab == talentTabId)
                    talents.push_back(talent);

        std::ranges::sort(talents, [](TalentEntry const* a, TalentEntry const* b)
        {
            return a->Row != b->Row ? a->Row < b->Row : a->Col < b->Col;
        });

        uint32 learned = 0;
        for (TalentEntry const* talent : talents)
        {
            uint8 maxRank = 0;
            for (uint8 rank = 0; rank < MAX_TALENT_RANK; ++rank)
                if (talent->RankID[rank])
                    maxRank = rank;

            if (player->LearnTalent(talent->TalentID, maxRank, true))
                ++learned;
        }

        player->SendTalentsInfoData(false);
        handler->PSendSysMessage("Test kit: tree {} (talent tab {}): {} of {} talents learned at their highest rank.",
            treeNumber, talentTabId, learned, talents.size());
        return true;
    }

    static bool HandleReadyCommand(ChatHandler* handler)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player->IsAlive())
            player->ResurrectPlayer(1.0f);

        player->SetFullHealth();
        for (uint8 i = POWER_MANA; i < MAX_POWERS; ++i)
        {
            Powers power = Powers(i);
            if (power == POWER_HAPPINESS || power == POWER_RUNE || !player->GetMaxPower(power))
                continue;

            // rage and runic power start empty, the others full
            player->SetPower(power, (power == POWER_RAGE || power == POWER_RUNIC_POWER) ? 0 : player->GetMaxPower(power));
        }

        player->RemoveAllSpellCooldown();
        player->RemoveAurasDueToSpell(SPELL_FORBEARANCE);

        // a half circle of training dummies in front of the player
        for (uint32 i = 0; i < DUMMY_COUNT; ++i)
        {
            float const angle = player->GetOrientation() + (float(i) - float(DUMMY_COUNT - 1) / 2.0f) * (M_PI / 8.0f);
            float const x = player->GetPositionX() + 6.0f * std::cos(angle);
            float const y = player->GetPositionY() + 6.0f * std::sin(angle);
            float const z = player->GetMap()->GetHeight(player->GetPhaseMask(), x, y, player->GetPositionZ() + 2.0f);
            player->SummonCreature(NPC_TRAINING_DUMMY, x, y, z, angle + float(M_PI), TEMPSUMMON_TIMED_DESPAWN, DUMMY_DESPAWN_MS);
        }

        handler->SendSysMessage("Test kit: health and power full, cooldowns and Forbearance cleared, training dummies placed.");
        return true;
    }

    static bool HandleResetCommand(ChatHandler* handler)
    {
        Player* player = handler->GetSession()->GetPlayer();
        player->resetTalents(true);
        player->SendTalentsInfoData(false);
        handler->SendSysMessage("Test kit: the active spec's talents and specialization are reset.");
        return true;
    }
};

void AddSC_testkit_commandscript()
{
    new testkit_commandscript();
}
