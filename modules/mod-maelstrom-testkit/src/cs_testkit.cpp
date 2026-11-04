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
 *   .testkit glyph <id> <1|0> [spells]  puts glyph <id> (GlyphProperties.dbc) in a slot of its type or takes it out, and the
 *                          known spells of the list on action buttons 1-5; id 0 empties every glyph slot; <2> instead checks
 *                          the glyph's modifiers: "Test kit: glyph check <id> ok|fail <details>" for the addon
 *   .testkit cast <spell> [triggered]  you cast it: at yourself if it is helpful, otherwise at your selection (.cast self makes the
 *                          selection cast it at itself, so a dummy would get your Metamorphosis)
 *   .testkit audit         every glyph of every class, no character needed: its modifiers reach a spell and the effect they
 *                          change exists (also from the console and SOAP)
 * Used by the MaelstromClassTest addon. Not 4.3.4 behaviour, so a module (docs/AZEROTHCORE_WAY.md).
 */

#include "Chat.h"
#include "CommandScript.h"
#include "DBCStores.h"
#include "Player.h"
#include "SpellMgr.h"
#include "SpellAuraEffects.h"
#include "SpellAuras.h"
#include "SpellInfo.h"
#include "StringConvert.h"
#include "TemporarySummon.h"
#include "Tokenize.h"

using namespace Acore::ChatCommands;

namespace
{
    // module permissions (2000+, docs/MODULARITY.md)
    rbac::RBACPermissions constexpr RBAC_PERM_COMMAND_TESTKIT_TREE  = rbac::RBACPermissions(2001);
    rbac::RBACPermissions constexpr RBAC_PERM_COMMAND_TESTKIT_READY = rbac::RBACPermissions(2002);
    rbac::RBACPermissions constexpr RBAC_PERM_COMMAND_TESTKIT_RESET = rbac::RBACPermissions(2003);
    rbac::RBACPermissions constexpr RBAC_PERM_COMMAND_TESTKIT_GLYPH = rbac::RBACPermissions(2004);
    rbac::RBACPermissions constexpr RBAC_PERM_COMMAND_TESTKIT_AUDIT = rbac::RBACPermissions(2005);

    uint32 constexpr NPC_TRAINING_DUMMY = 46647;    // level 85
    uint32 constexpr SPELL_FORBEARANCE  = 25771;
    uint32 constexpr DUMMY_COUNT        = 5;        // enough for "4 or more targets" effects
    uint32 constexpr DUMMY_DESPAWN_MS   = 30 * MINUTE * IN_MILLISECONDS;
    uint8 constexpr GLYPH_BAR_BUTTONS   = 5;
    uint8 constexpr GLYPH_ALL_SLOTS_LEVEL = 75;

    // The effect a glyph's modifier changes is there: a real effect, or a value only a script reads (Wild Growth's target count).
    // A correction that removes an effect with no value (Chains of Ice's damage, before it went) does not pass.
    bool HasTargetedEffect(SpellInfo const* info, int8 targeted)
    {
        return info->Effects[targeted].IsEffect() || info->Effects[targeted].BasePoints;
    }

    // The glyph's modifiers on the spells the character knows: its aura is on, they reach a spell, they change its value, and the
    // effect they name is still there (a spell correction removed Chains of Ice's damage effect, whatever its glyph added to it)
    void CheckGlyph(ChatHandler* handler, Player* player, uint32 glyphId, GlyphPropertiesEntry const* entry)
    {
        Aura const* aura = player->GetAura(entry->SpellId);
        if (!aura)
        {
            handler->PSendSysMessage("Test kit: glyph check {} fail its aura is not on you", glyphId);
            return;
        }

        std::string reached;
        std::string broken;
        bool effectFound = false;
        uint32 reachedCount = 0;
        uint32 changed = 0;
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            AuraEffect const* effect = aura->GetEffect(i);
            if (!effect || !effect->GetAmount()
                || (effect->GetAuraType() != SPELL_AURA_ADD_FLAT_MODIFIER && effect->GetAuraType() != SPELL_AURA_ADD_PCT_MODIFIER))
                continue;

            SpellModOp const op = SpellModOp(effect->GetMiscValue());
            int8 const targeted = op == SPELLMOD_EFFECT1 ? 0 : (op == SPELLMOD_EFFECT2 ? 1 : (op == SPELLMOD_EFFECT3 ? 2 : -1));
            auto const look = [&](SpellInfo const* info)
            {
                // a class mask can also match stray spells (Glyph of Flametongue Weapon's has "Frozen Rune Weapon 5")
                if (reachedCount < 3 && reached.find(info->SpellName[0]) == std::string::npos)
                {
                    reached += (reached.empty() ? "" : ", ") + std::string(info->SpellName[0]);
                    ++reachedCount;
                }

                if (targeted >= 0)
                {
                    if (HasTargetedEffect(info, targeted))
                        effectFound = true;
                    else
                        broken = info->SpellName[0];
                }

                int32 value = 1000;
                player->ApplySpellMod(info->Id, op, value);
                if (value != 1000)
                    ++changed;
            };

            bool found = false;
            for (auto const& [spellId, spell] : player->GetSpellMap())
            {
                SpellInfo const* info = sSpellMgr->GetSpellInfo(spellId);
                if (info && spell->State != PLAYERSPELL_REMOVED && spell->Active && effect->IsAffectedOnSpell(info))
                {
                    look(info);
                    found = true;
                }
            }

            // Many glyphs change a spell that never is in a spellbook: a talent's buff (Elemental Mastery's 64701), a weapon
            // imbue's passive or what a spell triggers (Fire Nova's 8349)
            if (!found)
                for (uint32 spellId = 0; spellId < sSpellMgr->GetSpellInfoStoreSize(); ++spellId)
                    if (SpellInfo const* info = sSpellMgr->GetSpellInfo(spellId))
                        if (effect->IsAffectedOnSpell(info))
                            look(info);
        }

        // only when no spell it reaches has the effect (Chains of Ice before its correction went)
        if (!broken.empty() && !effectFound)
            handler->PSendSysMessage("Test kit: glyph check {} fail the effect it changes is missing on {}", glyphId, broken);
        else if (reached.empty())
            handler->PSendSysMessage("Test kit: glyph check {} fail no spell at all has what it changes", glyphId);
        else if (!changed)
            handler->PSendSysMessage("Test kit: glyph check {} fail it reaches {} but does not change it", glyphId, reached);
        else
            handler->PSendSysMessage("Test kit: glyph check {} ok {}", glyphId, reached);
    }

    // as WorldSession::HandleRemoveGlyph
    void RemoveGlyphFromSlot(Player* player, uint8 slot)
    {
        GlyphPropertiesEntry const* entry = sGlyphPropertiesStore.LookupEntry(player->GetGlyph(slot));
        if (!entry)
            return;

        player->RemoveAurasDueToSpell(entry->SpellId);
        Unit::AuraMap& ownedAuras = player->GetOwnedAuras();
        for (Unit::AuraMap::iterator iter = ownedAuras.begin(); iter != ownedAuras.end();)
        {
            SpellInfo const* triggeredBy = iter->second->GetTriggeredByAuraSpellInfo();
            if (triggeredBy && triggeredBy->Id == entry->SpellId)
                player->RemoveOwnedAura(iter);
            else
                ++iter;
        }

        player->SendLearnPacket(entry->SpellId, false);
        player->SetGlyph(slot, 0, true);
    }
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
            { "reset", HandleResetCommand, RBAC_PERM_COMMAND_TESTKIT_RESET, Console::No },
            { "glyph", HandleGlyphCommand, RBAC_PERM_COMMAND_TESTKIT_GLYPH, Console::No },
            { "cast",  HandleCastCommand,  RBAC_PERM_COMMAND_TESTKIT_GLYPH, Console::No },
            { "audit", HandleAuditCommand, RBAC_PERM_COMMAND_TESTKIT_AUDIT, Console::Yes }
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

    static bool HandleCastCommand(ChatHandler* handler, SpellInfo const* spell, Optional<std::string> triggered)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!spell)
            return false;

        Unit* selected = handler->getSelectedUnit();
        Unit* target = (spell->IsPositive() || !selected) ? player : selected;
        player->CastSpell(target, spell->Id, triggered ? TRIGGERED_FULL_MASK : TRIGGERED_NONE);
        return true;
    }

    static bool HandleAuditCommand(ChatHandler* handler)
    {
        uint32 glyphs = 0;
        uint32 modifiers = 0;
        uint32 problems = 0;
        for (uint32 id = 0; id < sGlyphPropertiesStore.GetNumRows(); ++id)
        {
            GlyphPropertiesEntry const* entry = sGlyphPropertiesStore.LookupEntry(id);
            SpellInfo const* glyph = entry ? sSpellMgr->GetSpellInfo(entry->SpellId) : nullptr;
            if (!glyph)
                continue;

            ++glyphs;
            std::string const glyphName = glyph->SpellName[0];
            for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
            {
                SpellEffectInfo const& effect = glyph->Effects[i];
                if (!effect.IsAura(SPELL_AURA_ADD_FLAT_MODIFIER) && !effect.IsAura(SPELL_AURA_ADD_PCT_MODIFIER))
                    continue;

                if (!effect.BasePoints)
                    continue;

                ++modifiers;
                int32 const op = effect.MiscValue;
                int8 const targeted = op == SPELLMOD_EFFECT1 ? 0 : (op == SPELLMOD_EFFECT2 ? 1 : (op == SPELLMOD_EFFECT3 ? 2 : -1));
                uint32 reached = 0;
                bool effectFound = false;
                bool namedFound = false;
                std::string missingOn;
                for (uint32 spellId = 0; spellId < sSpellMgr->GetSpellInfoStoreSize(); ++spellId)
                {
                    SpellInfo const* info = sSpellMgr->GetSpellInfo(spellId);
                    if (!info || info == glyph || !info->IsAffected(glyph->SpellFamilyName, effect.SpellClassMask))
                        continue;

                    ++reached;
                    if (targeted >= 0)
                    {
                        // the spell the glyph is named after counts by itself: a stray match must not hide it, though a
                        // helper of the same name may lack the effect (Water Shield's mana gain, 52128)
                        bool const named = glyphName == std::string("Glyph of ") + info->SpellName[0];
                        if (HasTargetedEffect(info, targeted))
                        {
                            effectFound = true;
                            namedFound = namedFound || named;
                        }
                        else if (named || missingOn.empty())
                            missingOn = info->SpellName[0];
                    }
                }

                if (!reached)
                {
                    handler->PSendSysMessage("Audit: {} ({}) effect {}: its modifier reaches no spell", glyphName, glyph->Id, i);
                    ++problems;
                }
                else if (!missingOn.empty() && (!effectFound || (glyphName == "Glyph of " + missingOn && !namedFound)))
                {
                    handler->PSendSysMessage("Audit: {} ({}) effect {}: changes effect {} of {}, which is missing", glyphName, glyph->Id, i, targeted, missingOn);
                    ++problems;
                }
            }
        }

        handler->PSendSysMessage("Audit: {} glyphs, {} modifiers, {} problems.", glyphs, modifiers, problems);
        return true;
    }

    static bool HandleGlyphCommand(ChatHandler* handler, uint32 glyphId, uint8 apply, Tail spells)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (player->GetLevel() < GLYPH_ALL_SLOTS_LEVEL)
        {
            handler->SendSysMessage("Test kit: glyphs need level 75, when every slot is open.");
            return false;
        }

        if (!glyphId)
        {
            for (uint8 slot = 0; slot < MAX_GLYPH_SLOT_INDEX; ++slot)
                RemoveGlyphFromSlot(player, slot);
            player->SendTalentsInfoData(false);
            handler->SendSysMessage("Test kit: glyph slots emptied.");
            return true;
        }

        GlyphPropertiesEntry const* entry = sGlyphPropertiesStore.LookupEntry(glyphId);
        if (!entry)
        {
            handler->PSendSysMessage("Test kit: no glyph {}.", glyphId);
            return false;
        }

        if (apply == 2)
        {
            CheckGlyph(handler, player, glyphId, entry);
            return true;
        }

        // the glyph's slot if it has one; otherwise an empty slot of its type, or the first of its type
        int16 held = -1;
        int16 empty = -1;
        int16 first = -1;
        for (uint8 slot = 0; slot < MAX_GLYPH_SLOT_INDEX; ++slot)
        {
            GlyphSlotEntry const* slotEntry = sGlyphSlotStore.LookupEntry(player->GetGlyphSlot(slot));
            if (!slotEntry || slotEntry->TypeFlags != entry->TypeFlags)
                continue;

            if (player->GetGlyph(slot) == glyphId)
                held = slot;
            else if (!player->GetGlyph(slot) && empty < 0)
                empty = slot;
            if (first < 0)
                first = slot;
        }

        if (apply && held < 0)
        {
            int16 slot = empty >= 0 ? empty : first;
            if (slot < 0)
            {
                handler->PSendSysMessage("Test kit: no slot for glyph {}.", glyphId);
                return false;
            }

            RemoveGlyphFromSlot(player, uint8(slot));
            player->SendLearnPacket(entry->SpellId, true);
            player->CastSpell(player, entry->SpellId, TriggerCastFlags(TRIGGERED_FULL_MASK & ~(TRIGGERED_IGNORE_SHAPESHIFT | TRIGGERED_IGNORE_CASTER_AURASTATE)));
            player->SetGlyph(uint8(slot), glyphId, true);
        }
        else if (!apply && held >= 0)
            RemoveGlyphFromSlot(player, uint8(held));
        player->SendTalentsInfoData(false);

        uint8 placed = 0;
        if (!spells.empty())
        {
            for (uint8 button = 0; button < GLYPH_BAR_BUTTONS; ++button)
                player->removeActionButton(button);

            for (std::string_view token : Acore::Tokenize(spells, ' ', false))
            {
                Optional<uint32> spellId = Acore::StringTo<uint32>(token);
                if (spellId && placed < GLYPH_BAR_BUTTONS && player->HasSpell(*spellId)
                    && player->addActionButton(placed, *spellId, ACTION_BUTTON_SPELL))
                    ++placed;
            }
            player->SendActionButtons(1);
        }

        handler->PSendSysMessage("Test kit: glyph {} {}, {} spells on action buttons 1-5.", glyphId, apply ? "on" : "off", placed);
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
