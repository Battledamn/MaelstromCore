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

#include "DynamicObject.h"
#include "GridNotifiers.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "SpellScriptLoader.h"
#include "TemporarySummon.h"
/*
 * Scripts for spells with SPELLFAMILY_PRIEST and SPELLFAMILY_GENERIC spells used by priest players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_pri_".
 *
 * 4.3.4: ported from The Cataclysm Preservation Project (spell_priest.cpp at b061f46), rewritten against
 * AzerothCore; the AzerothCore scripts that are still right for 4.3.4 are kept (docs/CLASS_SCRIPTS.md).
 */

enum PriestSpells
{
    SPELL_PRIEST_ABSOLUTION                         = 33167,
    SPELL_PRIEST_ARCHANGEL_DUMMY                    = 94709,
    SPELL_PRIEST_ARCHANGEL_TRIGGERED                = 81700,
    SPELL_PRIEST_ARCHANGEL_ENERGIZE                 = 87152,
    SPELL_PRIEST_ATONEMENT_TRIGGERED_LARGE          = 81751,
    SPELL_PRIEST_ATONEMENT_TRIGGERED_DEFAULT        = 94472,
    SPELL_PRIEST_BODY_AND_SOUL_DISPEL               = 64136,
    SPELL_PRIEST_BODY_AND_SOUL_SPEED                = 65081,
    SPELL_PRIEST_CHAKRA_SERENITY                    = 81208,
    SPELL_PRIEST_CHAKRA_SANCTUARY                   = 81206,
    SPELL_PRIEST_CHAKRA_SANCTUARY_LINKED            = 81207,
    SPELL_PRIEST_CHAKRA_CHASTISE                    = 81209,
    SPELL_PRIEST_CHAKRA_FLOW                        = 89912,
    SPELL_PRIEST_CURE_DISEASE                       = 528,
    SPELL_PRIEST_DARK_ARCHANGEL_TRIGGERED           = 87153,
    SPELL_PRIEST_DARK_EVANGELISM_TRIGGERED_1        = 87117,
    SPELL_PRIEST_DARK_EVANGELISM_TRIGGERED_2        = 87118,
    SPELL_PRIEST_DISPEL_MAGIC_FRIENDLY              = 97690,
    SPELL_PRIEST_DISPEL_MAGIC_HOSTILE               = 97691,
    SPELL_PRIEST_DIVINE_AEGIS                       = 47753,
    SPELL_PRIEST_DIVINE_TOUCH                       = 63544,
    SPELL_PRIEST_ECHO_OF_LIGHT                      = 77489,
    SPELL_PRIEST_EVANGELISM_ARCHANGEL_DUMMY         = 87154,
    SPELL_PRIEST_EVANGELISM_R1                      = 81659,
    SPELL_PRIEST_EVANGELISM_TRIGGERED_1             = 81660,
    SPELL_PRIEST_EVANGELISM_TRIGGERED_2             = 81661,
    SPELL_PRIEST_EMPOWERED_SHADOW                   = 95799,
    SPELL_PRIEST_GLYPH_OF_CIRCLE_OF_HEALING         = 55675,
    SPELL_PRIEST_GLYPH_OF_DISPEL_MAGIC              = 55677,
    SPELL_PRIEST_GLYPH_OF_DISPEL_MAGIC_HEAL         = 56131,
    SPELL_PRIEST_GLYPH_OF_LIGHTWELL                 = 55673,
    SPELL_PRIEST_GLYPH_OF_POWER_WORD_SHIELD         = 55672,
    SPELL_PRIEST_GLYPH_OF_POWER_WORD_SHIELD_HEAL    = 56160,
    SPELL_PRIEST_GLYPH_OF_PRAYER_OF_HEALING_HEAL    = 56161,
    SPELL_PRIEST_GLYPH_OF_SHADOW                    = 107906,
    SPELL_PRIEST_GLYPH_OF_SHADOW_WORD_DEATH         = 95652,
    SPELL_PRIEST_GUARDIAN_SPIRIT_HEAL               = 48153,
    SPELL_PRIEST_HOLY_WORD_CHASTISE                 = 88625,
    SPELL_PRIEST_HOLY_WORD_SANCTUARY                = 88686,
    SPELL_PRIEST_IMPROVED_DEVOURING_PLAGUE_DAMAGE   = 63675,
    SPELL_PRIEST_INDULGENCE_OF_THE_PENITENT         = 89913,
    SPELL_PRIEST_INNER_FOCUS                        = 89485,
    SPELL_PRIEST_ITEM_EFFICIENCY                    = 37595,
    SPELL_PRIEST_ITEM_T11_HEALER_4P_BONUS           = 89911,
    SPELL_PRIEST_LEAP_OF_FAITH_EFFECT               = 92832,
    SPELL_PRIEST_MANA_LEECH_PROC                    = 34650,
    SPELL_PRIEST_MIND_FLAY                          = 15407,
    SPELL_PRIEST_MIND_TRAUMA                        = 48301,
    SPELL_PRIEST_PENANCE_DAMAGE                     = 47758,
    SPELL_PRIEST_PENANCE_HEAL                       = 47757,
    SPELL_PRIEST_REFLECTIVE_SHIELD_R1               = 33201,
    SPELL_PRIEST_REFLECTIVE_SHIELD_TRIGGERED        = 33619,
    SPELL_PRIEST_RENEW                              = 139,
    SPELL_PRIEST_REVELATIONS                        = 88627,
    SPELL_PRIEST_SHADOW_ORB_MARKER                  = 93683,
    SPELL_PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH    = 107903,
    SPELL_PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH       = 107904,
    SPELL_PRIEST_SHADOW_WORD_DEATH                  = 32409,
    SPELL_PRIEST_SPIRIT_OF_REDEMPTION_TRIGGERED     = 27827,
    SPELL_PRIEST_STRENGTH_OF_SOUL_R1                = 89488,
    SPELL_PRIEST_STRENGTH_OF_SOUL_TRIGGERED_R1      = 96266,
    SPELL_PRIEST_STRENGTH_OF_SOUL_TRIGGERED_R2      = 96267,
    SPELL_PRIEST_TWIN_DISCIPLINES_RANK_1            = 47586,
    SPELL_PRIEST_T9_HEALING_2P                      = 67201,
    SPELL_PRIEST_VAMPIRIC_EMBRACE_HEAL              = 15290,
    SPELL_PRIEST_VAMPIRIC_TOUCH_DISPEL              = 64085,
    SPELL_PRIEST_WEAKENED_SOUL                      = 6788,
    SPELL_GEN_REPLENISHMENT                         = 57669,
    SPELL_GEN_INTERRUPT                             = 32747,

    // AzerothCore's kept scripts
    SPELL_PRIEST_LIGHTWELL_CHARGES                  = 59907,
    SPELL_PRIEST_T4_4P_FLEXIBILITY                  = 37565,
    SPELL_PRIEST_GLYPH_OF_SHADOWFIEND               = 58228,
    SPELL_PRIEST_GLYPH_OF_SHADOWFIEND_MANA          = 58227,
    SPELL_PRIEST_ORACULAR_HEAL                      = 26170,
    SPELL_PRIEST_DIVINE_BLESSING                    = 40440,
    SPELL_PRIEST_DIVINE_WRATH                       = 40441,
    SPELL_PRIEST_ARMOR_OF_FAITH                     = 28810,
    SPELL_PRIEST_BLESSED_HEALING                    = 70772
};

enum PriestSpellIcons
{
    PRIEST_ICON_ID_BORROWED_TIME                    = 2899,
    PRIEST_ICON_ID_DIVINE_TOUCH_TALENT              = 3021,
    PRIEST_ICON_ID_PAIN_AND_SUFFERING               = 2874,
    PRIEST_ICON_ID_SHIELD_DISCIPLINE                = 566,   // the mastery is SPELLFAMILY_HUNTER in the client data
    PRIEST_ICON_ID_HARNESSED_SHADOWS                = 554,
    PRIEST_ICON_ID_IMPROVED_MIND_BLAST              = 95,
    PRIEST_ICON_ID_MIND_MELT                        = 3139,
    PRIEST_ICON_ID_SHADOW_ORB                       = 4941,
    PRIEST_ICON_ID_GLYPH_OF_PSYCHIC_SCREAM          = 1488,
    PRIEST_ICON_ID_GLYPH_OF_SHADOW_WORD_DEATH       = 1980,
    PRIEST_ICON_ID_IMPROVED_DEVOURING_PLAGUE        = 3790
};

enum Mics
{
    PRIEST_LIGHTWELL_NPC_1                          = 31897,
    PRIEST_LIGHTWELL_NPC_2                          = 31896,
    PRIEST_LIGHTWELL_NPC_3                          = 31895,
    PRIEST_LIGHTWELL_NPC_4                          = 31894,
    PRIEST_LIGHTWELL_NPC_5                          = 31893,
    PRIEST_LIGHTWELL_NPC_6                          = 31883
};

// only units with the given power type
class PowerCheck
{
public:
    explicit PowerCheck(Powers const power) : _power(power) { }

    bool operator()(WorldObject* obj) const
    {
        if (Unit* target = obj->ToUnit())
            return target->getPowerType() != _power;
        return true;
    }

private:
    Powers const _power;
};

// only the caster's raid or party
class RaidCheck
{
public:
    explicit RaidCheck(Unit const* caster) : _caster(caster) { }

    bool operator()(WorldObject* obj) const
    {
        if (Unit* target = obj->ToUnit())
            return !_caster->IsInRaidWith(target);
        return true;
    }

private:
    Unit const* _caster;
};

// 87151 - Archangel
// Uses up Evangelism (healing done, mana) or Dark Evangelism (damage done, mana) stacks.
class spell_pri_archangel : public SpellScript
{
    PrepareSpellScript(spell_pri_archangel);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_ARCHANGEL_TRIGGERED, SPELL_PRIEST_ARCHANGEL_ENERGIZE, SPELL_PRIEST_DARK_ARCHANGEL_TRIGGERED,
            SPELL_PRIEST_EVANGELISM_TRIGGERED_1, SPELL_PRIEST_EVANGELISM_TRIGGERED_2, SPELL_PRIEST_DARK_EVANGELISM_TRIGGERED_1,
            SPELL_PRIEST_DARK_EVANGELISM_TRIGGERED_2 });
    }

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        uint32 spellId = 0;
        for (uint32 id : { SPELL_PRIEST_EVANGELISM_TRIGGERED_1, SPELL_PRIEST_EVANGELISM_TRIGGERED_2,
                           SPELL_PRIEST_DARK_EVANGELISM_TRIGGERED_1, SPELL_PRIEST_DARK_EVANGELISM_TRIGGERED_2 })
            if (caster->HasAura(id))
            {
                spellId = id;
                break;
            }

        Aura const* aura = spellId ? caster->GetAura(spellId) : nullptr;
        if (!aura)
            return;

        int32 const stacks = aura->GetStackAmount();
        if (spellId == SPELL_PRIEST_EVANGELISM_TRIGGERED_1 || spellId == SPELL_PRIEST_EVANGELISM_TRIGGERED_2)
        {
            int32 bp = sSpellMgr->AssertSpellInfo(SPELL_PRIEST_ARCHANGEL_TRIGGERED)->Effects[EFFECT_0].BasePoints * stacks;
            int32 mana = stacks;
            caster->CastCustomSpell(caster, SPELL_PRIEST_ARCHANGEL_TRIGGERED, &bp, nullptr, nullptr, true);
            caster->CastCustomSpell(caster, SPELL_PRIEST_ARCHANGEL_ENERGIZE, &mana, nullptr, nullptr, true);
        }
        else
        {
            SpellInfo const* dark = sSpellMgr->AssertSpellInfo(SPELL_PRIEST_DARK_ARCHANGEL_TRIGGERED);
            int32 bp0 = dark->Effects[EFFECT_0].BasePoints * stacks;
            int32 bp1 = dark->Effects[EFFECT_1].BasePoints * stacks;
            int32 mana = dark->Effects[EFFECT_2].BasePoints * stacks;
            caster->CastCustomSpell(caster, SPELL_PRIEST_DARK_ARCHANGEL_TRIGGERED, &bp0, &bp1, nullptr, true);
            caster->CastCustomSpell(caster, SPELL_PRIEST_ARCHANGEL_ENERGIZE, &mana, nullptr, nullptr, true);
        }

        caster->RemoveAurasDueToSpell(spellId);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pri_archangel::HandleHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// -14523 - Atonement
// Smite and Holy Fire heal the most injured ally near the target for a share of their damage.
class spell_pri_atonement : public AuraScript
{
    PrepareAuraScript(spell_pri_atonement);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_ATONEMENT_TRIGGERED_LARGE, SPELL_PRIEST_ATONEMENT_TRIGGERED_DEFAULT });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        DamageInfo* damage = eventInfo.GetDamageInfo();
        if (!damage || !damage->GetVictim())
            return;

        Unit* priest = GetTarget();
        Unit* target = damage->GetVictim();
        int32 bp = CalculatePct(int32(damage->GetDamage()), aurEff->GetAmount());

        // a large target: the heal reaches out from its edge
        if (target->GetCombatReach() >= 15.0f)
        {
            CustomSpellValues values;
            values.AddSpellMod(SPELLVALUE_BASE_POINT0, bp);
            values.AddSpellMod(SPELLVALUE_RADIUS_MOD, int32(target->GetCombatReach() + 15.0f) * 100);
            priest->CastCustomSpell(SPELL_PRIEST_ATONEMENT_TRIGGERED_LARGE, values, target, TRIGGERED_FULL_MASK, nullptr, aurEff);
        }
        else
            priest->CastCustomSpell(target, SPELL_PRIEST_ATONEMENT_TRIGGERED_DEFAULT, &bp, nullptr, nullptr, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_atonement::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 81751, 94472 - Atonement (heal): the most injured member of the priest's group
class spell_pri_atonement_heal : public SpellScript
{
    PrepareSpellScript(spell_pri_atonement_heal);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(RaidCheck(GetCaster()));
        if (targets.size() > 1)
        {
            targets.sort(Acore::HealthPctOrderPred());
            targets.resize(1);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_atonement_heal::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
    }
};

// -64127 - Body and Soul
// Power Word: Shield and Leap of Faith give speed; Cure Disease on yourself may also remove a poison.
class spell_pri_body_and_soul : public AuraScript
{
    PrepareAuraScript(spell_pri_body_and_soul);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_CURE_DISEASE, SPELL_PRIEST_BODY_AND_SOUL_DISPEL, SPELL_PRIEST_BODY_AND_SOUL_SPEED });
    }

    void HandleEffectSpeedProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        // Power Word: Shield or Leap of Faith
        if (!spellInfo || !eventInfo.GetProcTarget() || !(spellInfo->SpellFamilyFlags[0] & 0x1 || spellInfo->SpellFamilyFlags[2] & 0x80000))
            return;

        int32 speed = aurEff->GetAmount();
        GetTarget()->CastCustomSpell(eventInfo.GetProcTarget(), SPELL_PRIEST_BODY_AND_SOUL_SPEED, &speed, nullptr, nullptr, true, nullptr, aurEff);
    }

    void HandleEffectDispelProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        // Cure Disease on yourself
        if (!spellInfo || spellInfo->Id != SPELL_PRIEST_CURE_DISEASE || eventInfo.GetProcTarget() != GetTarget())
            return;

        if (roll_chance_i(aurEff->GetAmount()))
            GetTarget()->CastSpell(eventInfo.GetProcTarget(), SPELL_PRIEST_BODY_AND_SOUL_DISPEL, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_body_and_soul::HandleEffectSpeedProc, EFFECT_0, SPELL_AURA_DUMMY);
        OnEffectProc += AuraEffectProcFn(spell_pri_body_and_soul::HandleEffectDispelProc, EFFECT_1, SPELL_AURA_DUMMY);
    }
};

// 14751 - Chakra
// The next Heal, Flash Heal, Greater Heal or Binding Heal enters Serenity; Prayer of Healing or Prayer of Mending
// Sanctuary; Smite or Mind Spike Chastise. Holy Word: Chastise turns into the state's Holy Word with Revelations.
class spell_pri_chakra : public AuraScript
{
    PrepareAuraScript(spell_pri_chakra);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_CHAKRA_SERENITY, SPELL_PRIEST_CHAKRA_SANCTUARY, SPELL_PRIEST_CHAKRA_SANCTUARY_LINKED,
            SPELL_PRIEST_CHAKRA_CHASTISE, SPELL_PRIEST_CHAKRA_FLOW, SPELL_PRIEST_REVELATIONS, SPELL_PRIEST_HOLY_WORD_CHASTISE });
    }

    void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        target->RemoveAurasDueToSpell(SPELL_PRIEST_CHAKRA_SERENITY);
        target->RemoveAurasDueToSpell(SPELL_PRIEST_CHAKRA_SANCTUARY);
        target->RemoveAurasDueToSpell(SPELL_PRIEST_CHAKRA_CHASTISE);
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* caster = eventInfo.GetActor();
        SpellInfo const* spell = eventInfo.GetSpellInfo();
        if (!caster || !spell)
            return;

        // the state's Holy Word replaces Holy Word: Chastise (an action bar override, effect 2) with Revelations
        auto holyWord = [caster](uint32 stateId)
        {
            return caster->HasAura(SPELL_PRIEST_REVELATIONS) ? sSpellMgr->AssertSpellInfo(stateId)->Effects[EFFECT_2].BasePoints
                : int32(SPELL_PRIEST_HOLY_WORD_CHASTISE);
        };

        // Chakra: Serenity
        if (spell->SpellFamilyFlags.HasFlag(0x00000400) || spell->SpellFamilyFlags.HasFlag(0x00001000)
            || spell->SpellFamilyFlags.HasFlag(0x00000800) || spell->SpellFamilyFlags.HasFlag(0, 0x00000004))
        {
            CustomSpellValues values;
            values.AddSpellMod(SPELLVALUE_BASE_POINT2, holyWord(SPELL_PRIEST_CHAKRA_SERENITY));
            caster->CastCustomSpell(SPELL_PRIEST_CHAKRA_SERENITY, values, caster, TRIGGERED_FULL_MASK, nullptr, aurEff);
        }

        // Chakra: Sanctuary
        if (spell->SpellFamilyFlags.HasFlag(0x00000200) || spell->SpellFamilyFlags.HasFlag(0, 0x00000020))
        {
            CustomSpellValues values;
            values.AddSpellMod(SPELLVALUE_BASE_POINT2, holyWord(SPELL_PRIEST_CHAKRA_SANCTUARY));
            caster->CastCustomSpell(SPELL_PRIEST_CHAKRA_SANCTUARY, values, caster, TRIGGERED_FULL_MASK, nullptr, aurEff);
            caster->CastSpell(caster, SPELL_PRIEST_CHAKRA_SANCTUARY_LINKED, true, nullptr, aurEff);
        }

        // Chakra: Chastise
        if (spell->SpellFamilyFlags.HasFlag(0x00000080) || spell->SpellFamilyFlags.HasFlag(0, 0x00010000))
            caster->CastSpell(caster, SPELL_PRIEST_CHAKRA_CHASTISE, true, nullptr, aurEff);

        // T11 healer 4P
        if (caster->GetAuraEffect(SPELL_PRIEST_ITEM_T11_HEALER_4P_BONUS, EFFECT_0))
            caster->CastSpell(caster, SPELL_PRIEST_CHAKRA_FLOW, true);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_pri_chakra::HandleEffectApply, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        OnEffectProc += AuraEffectProcFn(spell_pri_chakra::HandleProc, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER);
    }
};

// 81208 - Chakra: Serenity, 81209 - Chakra: Chastise
class spell_pri_chakra_flow_removal : public AuraScript
{
    PrepareAuraScript(spell_pri_chakra_flow_removal);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_CHAKRA_CHASTISE, SPELL_PRIEST_CHAKRA_SERENITY, SPELL_PRIEST_CHAKRA_FLOW });
    }

    void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_PRIEST_CHAKRA_FLOW);
    }

    void Register() override
    {
        if (m_scriptSpellId == SPELL_PRIEST_CHAKRA_SERENITY)
            AfterEffectRemove += AuraEffectRemoveFn(spell_pri_chakra_flow_removal::HandleEffectRemove, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        else if (m_scriptSpellId == SPELL_PRIEST_CHAKRA_CHASTISE)
            AfterEffectRemove += AuraEffectRemoveFn(spell_pri_chakra_flow_removal::HandleEffectRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }
};

// 81206 - Chakra: Sanctuary
class spell_pri_chakra_sanctuary : public AuraScript
{
    PrepareAuraScript(spell_pri_chakra_sanctuary);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_CHAKRA_SANCTUARY_LINKED, SPELL_PRIEST_CHAKRA_FLOW });
    }

    void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_PRIEST_CHAKRA_SANCTUARY_LINKED);
        GetTarget()->RemoveAurasDueToSpell(SPELL_PRIEST_CHAKRA_FLOW);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_pri_chakra_sanctuary::HandleEffectRemove, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }
};

// 81585 - Chakra: Serenity: the heal refreshes Renew on the target
class spell_pri_chakra_serenity_script : public SpellScript
{
    PrepareSpellScript(spell_pri_chakra_serenity_script);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_RENEW });
    }

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        if (Aura* renew = GetHitUnit()->GetAura(SPELL_PRIEST_RENEW, GetCaster()->GetGUID()))
            renew->RefreshDuration();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pri_chakra_serenity_script::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 34861 - Circle of Healing: the five (six with the glyph) most injured members of the priest's group
class spell_pri_circle_of_healing : public SpellScript
{
    PrepareSpellScript(spell_pri_circle_of_healing);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_GLYPH_OF_CIRCLE_OF_HEALING });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(RaidCheck(GetCaster()));

        uint32 const maxTargets = GetCaster()->HasAura(SPELL_PRIEST_GLYPH_OF_CIRCLE_OF_HEALING) ? 6 : 5;
        if (targets.size() > maxTargets)
        {
            targets.sort(Acore::HealthPctOrderPred());
            targets.resize(maxTargets);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_circle_of_healing::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
    }
};

// 2944 - Devouring Plague
// Improved Devouring Plague: damage at once, a share of the whole plague.
class spell_pri_devouring_plague : public AuraScript
{
    PrepareAuraScript(spell_pri_devouring_plague);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_IMPROVED_DEVOURING_PLAGUE_DAMAGE });
    }

    void AfterApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        if (AuraEffect const* improved = caster->GetDummyAuraEffect(SPELLFAMILY_PRIEST, PRIEST_ICON_ID_IMPROVED_DEVOURING_PLAGUE, EFFECT_0))
        {
            int32 bp = CalculatePct(aurEff->GetAmount() * int32(aurEff->GetTotalTicks()), improved->GetAmount());
            caster->CastCustomSpell(GetTarget(), SPELL_PRIEST_IMPROVED_DEVOURING_PLAGUE_DAMAGE, &bp, nullptr, nullptr, true);
        }
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_pri_devouring_plague::AfterApply, EFFECT_0, SPELL_AURA_PERIODIC_LEECH, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }
};

// 527 - Dispel Magic
// 4.3.4: on yourself (others with Absolution) or on an enemy; the glyph heals the friendly target.
class spell_pri_dispel_magic : public SpellScript
{
    PrepareSpellScript(spell_pri_dispel_magic);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_ABSOLUTION, SPELL_PRIEST_GLYPH_OF_DISPEL_MAGIC_HEAL, SPELL_PRIEST_GLYPH_OF_DISPEL_MAGIC,
            SPELL_PRIEST_DISPEL_MAGIC_FRIENDLY, SPELL_PRIEST_DISPEL_MAGIC_HOSTILE });
    }

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();
        Unit* target = GetExplTargetUnit();
        if (!target || (!caster->HasAura(SPELL_PRIEST_ABSOLUTION) && caster != target && target->IsFriendlyTo(caster)))
            return SPELL_FAILED_BAD_TARGETS;
        return SPELL_CAST_OK;
    }

    void HandleDispel(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        if (target->IsFriendlyTo(caster))
        {
            caster->CastSpell(target, SPELL_PRIEST_DISPEL_MAGIC_FRIENDLY, true);
            if (AuraEffect const* glyph = caster->GetAuraEffect(SPELL_PRIEST_GLYPH_OF_DISPEL_MAGIC, EFFECT_0))
            {
                int32 heal = target->CountPctFromMaxHealth(glyph->GetAmount());
                caster->CastCustomSpell(target, SPELL_PRIEST_GLYPH_OF_DISPEL_MAGIC_HEAL, &heal, nullptr, nullptr, true);
            }
        }
        else
            caster->CastSpell(target, SPELL_PRIEST_DISPEL_MAGIC_HOSTILE, true);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_pri_dispel_magic::CheckCast);
        OnEffectHitTarget += SpellEffectFn(spell_pri_dispel_magic::HandleDispel, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// -47509 - Divine Aegis
// Critical heals (and Prayer of Healing always) shield the target for a share of the heal, up to 40% of its health.
class spell_pri_divine_aegis : public AuraScript
{
    PrepareAuraScript(spell_pri_divine_aegis);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_DIVINE_AEGIS });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetProcTarget() || !eventInfo.GetSpellInfo() || !eventInfo.GetHealInfo())
            return false;

        // Prayer of Healing always
        if (eventInfo.GetSpellInfo()->SpellFamilyFlags[0] & 0x200)
            return true;

        return eventInfo.GetHitMask() & PROC_HIT_CRITICAL;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* target = eventInfo.GetProcTarget();
        int32 absorb = CalculatePct(int32(eventInfo.GetHealInfo()->GetEffectiveHeal()), aurEff->GetAmount());

        // the shields add up
        if (AuraEffect const* aegis = target->GetAuraEffect(SPELL_PRIEST_DIVINE_AEGIS, EFFECT_0))
            absorb += aegis->GetAmount();

        // Mastery: Shield Discipline
        if (AuraEffect const* shieldDiscipline = GetTarget()->GetDummyAuraEffect(SPELLFAMILY_HUNTER, PRIEST_ICON_ID_SHIELD_DISCIPLINE, EFFECT_0))
            AddPct(absorb, shieldDiscipline->GetAmount());

        absorb = std::min(absorb, int32(CalculatePct(target->GetMaxHealth(), 40)));
        if (absorb > 0)
            GetTarget()->CastCustomSpell(target, SPELL_PRIEST_DIVINE_AEGIS, &absorb, nullptr, nullptr, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pri_divine_aegis::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pri_divine_aegis::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 64844 - Divine Hymn: the three most injured members of the priest's group
class spell_pri_divine_hymn : public SpellScript
{
    PrepareSpellScript(spell_pri_divine_hymn);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(RaidCheck(GetCaster()));
        uint32 const maxTargets = 3;
        if (targets.size() > maxTargets)
        {
            targets.sort(Acore::HealthPctOrderPred());
            targets.resize(maxTargets);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_divine_hymn::FilterTargets, EFFECT_ALL, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// 77485 - Echo of Light (Holy mastery)
// Direct heals leave a heal over time for the mastery's share of the heal.
class spell_pri_echo_of_light : public AuraScript
{
    PrepareAuraScript(spell_pri_echo_of_light);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_ECHO_OF_LIGHT, SPELL_PRIEST_HOLY_WORD_SANCTUARY });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->Id != SPELL_PRIEST_HOLY_WORD_SANCTUARY && eventInfo.GetHealInfo();
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* caster = GetCaster();
        Unit* target = eventInfo.GetProcTarget();
        if (!caster || !target)
            return;

        uint32 const ticks = sSpellMgr->AssertSpellInfo(SPELL_PRIEST_ECHO_OF_LIGHT)->GetMaxTicks();
        int32 heal = int32(CalculatePct(eventInfo.GetHealInfo()->GetHeal(), aurEff->GetAmount()));
        if (heal > 0 && ticks)
        {
            int32 perTick = heal / int32(ticks);
            caster->CastCustomSpell(target, SPELL_PRIEST_ECHO_OF_LIGHT, &perTick, nullptr, nullptr, true, nullptr, aurEff);
        }
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pri_echo_of_light::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pri_echo_of_light::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -81659 - Evangelism
// Smite, Holy Fire and Penance build Evangelism; Mind Flay builds Dark Evangelism. Both enable Archangel.
class spell_pri_evangelism : public AuraScript
{
    PrepareAuraScript(spell_pri_evangelism);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_EVANGELISM_R1, SPELL_PRIEST_EVANGELISM_TRIGGERED_1, SPELL_PRIEST_EVANGELISM_TRIGGERED_2,
            SPELL_PRIEST_DARK_EVANGELISM_TRIGGERED_1, SPELL_PRIEST_DARK_EVANGELISM_TRIGGERED_2, SPELL_PRIEST_EVANGELISM_ARCHANGEL_DUMMY,
            SPELL_PRIEST_ARCHANGEL_DUMMY });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* target = GetTarget();
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo)
            return;

        uint32 spellId = GetSpellInfo()->Id == SPELL_PRIEST_EVANGELISM_R1 ? SPELL_PRIEST_EVANGELISM_TRIGGERED_1 : SPELL_PRIEST_EVANGELISM_TRIGGERED_2;
        if (spellInfo->Id == SPELL_PRIEST_MIND_FLAY)
            spellId = spellId == SPELL_PRIEST_EVANGELISM_TRIGGERED_1 ? SPELL_PRIEST_DARK_EVANGELISM_TRIGGERED_1 : SPELL_PRIEST_DARK_EVANGELISM_TRIGGERED_2;

        target->CastSpell(target, spellId, true, nullptr, aurEff);
        target->CastSpell(target, SPELL_PRIEST_EVANGELISM_ARCHANGEL_DUMMY, true, nullptr, aurEff);

        if (Aura* aura = target->GetAura(spellId))
            if (aura->GetStackAmount() == aura->GetSpellInfo()->StackAmount)
                target->CastSpell(target, SPELL_PRIEST_ARCHANGEL_DUMMY, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_evangelism::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 81660, 81661 - Evangelism, 87117, 87118 - Dark Evangelism (the stacks): Archangel's markers go with them
class spell_pri_evangelism_triggered : public AuraScript
{
    PrepareAuraScript(spell_pri_evangelism_triggered);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_EVANGELISM_ARCHANGEL_DUMMY, SPELL_PRIEST_ARCHANGEL_DUMMY });
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_PRIEST_EVANGELISM_ARCHANGEL_DUMMY);
        GetTarget()->RemoveAurasDueToSpell(SPELL_PRIEST_ARCHANGEL_DUMMY);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_pri_evangelism_triggered::AfterRemove, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
    }
};

// 55680 - Glyph of Prayer of Healing: a share of the heal over time
class spell_pri_glyph_of_prayer_of_healing : public AuraScript
{
    PrepareAuraScript(spell_pri_glyph_of_prayer_of_healing);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_GLYPH_OF_PRAYER_OF_HEALING_HEAL });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        HealInfo* healInfo = eventInfo.GetHealInfo();
        if (!healInfo || !eventInfo.GetProcTarget())
            return;

        SpellInfo const* triggered = sSpellMgr->AssertSpellInfo(SPELL_PRIEST_GLYPH_OF_PRAYER_OF_HEALING_HEAL);
        if (uint32 ticks = triggered->GetMaxTicks())
        {
            int32 heal = int32(CalculatePct(int32(healInfo->GetHeal()), aurEff->GetAmount()) / int32(ticks));
            GetTarget()->CastCustomSpell(eventInfo.GetProcTarget(), SPELL_PRIEST_GLYPH_OF_PRAYER_OF_HEALING_HEAL, &heal, nullptr, nullptr, true, nullptr, aurEff);
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_glyph_of_prayer_of_healing::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 47788 - Guardian Spirit: a killing blow heals for $s2% of maximum health instead and ends the spirit
class spell_pri_guardian_spirit : public AuraScript
{
    PrepareAuraScript(spell_pri_guardian_spirit);

    uint32 healPct = 0;

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_GUARDIAN_SPIRIT_HEAL });
    }

    bool Load() override
    {
        healPct = GetSpellInfo()->Effects[EFFECT_1].CalcValue();
        return true;
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        // Set absorbtion amount to unlimited
        amount = -1;
    }

    void Absorb(AuraEffect* /*aurEff*/, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        Unit* target = GetTarget();
        if (dmgInfo.GetDamage() < target->GetHealth())
            return;

        int32 healAmount = int32(target->CountPctFromMaxHealth(healPct));
        // the healing bonus must not raise the following heal
        if (AuraEffect* bonus = GetEffect(EFFECT_0))
            bonus->SetAmount(0);
        target->CastCustomSpell(target, SPELL_PRIEST_GUARDIAN_SPIRIT_HEAL, &healAmount, nullptr, nullptr, true);
        absorbAmount = dmgInfo.GetDamage();
        Remove();
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_guardian_spirit::CalculateAmount, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_pri_guardian_spirit::Absorb, EFFECT_1);
    }
};

// -33191 - Harnessed Shadows: Shadow Orbs with the Shadow Orb Power mastery's bonus
class spell_pri_harnessed_shadows : public AuraScript
{
    PrepareAuraScript(spell_pri_harnessed_shadows);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->Effects[EFFECT_1].TriggerSpell });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        Unit* target = GetTarget();
        int32 bp = 10;
        if (AuraEffect const* mastery = target->GetAuraEffect(SPELL_AURA_PROC_TRIGGER_SPELL, SPELLFAMILY_PRIEST, 0, 0, 0x00100000))
            bp += mastery->GetAmount();

        target->CastCustomSpell(target, GetSpellInfo()->Effects[EFFECT_1].TriggerSpell, &bp, nullptr, nullptr, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_harnessed_shadows::HandleProc, EFFECT_1, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 88685 - Holy Word: Sanctuary: the area heal every tick at the ground spot
class spell_pri_holy_word_sanctuary : public AuraScript
{
    PrepareAuraScript(spell_pri_holy_word_sanctuary);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_HOLY_WORD_SANCTUARY });
    }

    void HandleDummyTick(AuraEffect const* aurEff)
    {
        if (DynamicObject* dyn = GetTarget()->GetDynObject(aurEff->GetId()))
        {
            SpellCastTargets targets;
            targets.SetDst(dyn->GetPositionX(), dyn->GetPositionY(), dyn->GetPositionZ(), dyn->GetOrientation());
            // procs stay possible (Echo of Light leaves Sanctuary out itself)
            GetTarget()->CastSpell(targets, sSpellMgr->AssertSpellInfo(SPELL_PRIEST_HOLY_WORD_SANCTUARY), nullptr,
                TriggerCastFlags(TRIGGERED_FULL_MASK & ~TRIGGERED_DISALLOW_PROC_EVENTS), nullptr, aurEff);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_pri_holy_word_sanctuary::HandleDummyTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 88686 - Holy Word: Sanctuary (heal): more than six targets share six targets' healing
class spell_pri_holy_word_sanctuary_triggered : public SpellScript
{
    PrepareSpellScript(spell_pri_holy_word_sanctuary_triggered);

    void FilterTargets(std::list<WorldObject*>& unitList)
    {
        _targets = unitList.size();
    }

    void HandleHeal(SpellEffIndex /*effIndex*/)
    {
        if (GetHitHeal() && _targets > 6)
            SetHitHeal(int32((GetHitHeal() * 6) / _targets));
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_holy_word_sanctuary_triggered::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
        OnEffectHitTarget += SpellEffectFn(spell_pri_holy_word_sanctuary_triggered::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
    }

private:
    uint32 _targets = 0;
};

// 64904 - Hymn of Hope: the three mana users of the priest's group lowest on mana
class spell_pri_hymn_of_hope : public SpellScript
{
    PrepareSpellScript(spell_pri_hymn_of_hope);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(PowerCheck(POWER_MANA));
        targets.remove_if(RaidCheck(GetCaster()));

        uint32 const maxTargets = 3;
        if (targets.size() > maxTargets)
        {
            targets.sort(Acore::PowerPctOrderPred(POWER_MANA));
            targets.resize(maxTargets);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_hymn_of_hope::FilterTargets, EFFECT_ALL, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// -14748 - Improved Power Word: Shield: its dummy is a percent modifier on the shield
class spell_pri_improved_power_word_shield : public AuraScript
{
    PrepareAuraScript(spell_pri_improved_power_word_shield);

    void HandleEffectCalcSpellMod(AuraEffect const* aurEff, SpellModifier*& spellMod)
    {
        if (!spellMod)
        {
            spellMod = new SpellModifier(GetAura());
            spellMod->op = SpellModOp(aurEff->GetMiscValue());
            spellMod->type = SPELLMOD_PCT;
            spellMod->spellId = GetId();
            spellMod->mask = GetSpellInfo()->Effects[aurEff->GetEffIndex()].SpellClassMask;
        }
        spellMod->value = aurEff->GetAmount();
    }

    void Register() override
    {
        DoEffectCalcSpellMod += AuraEffectCalcSpellModFn(spell_pri_improved_power_word_shield::HandleEffectCalcSpellMod, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 37594 - Greater Heal Refund (T5 healer 2P)
class spell_pri_item_greater_heal_refund : public AuraScript
{
    PrepareAuraScript(spell_pri_item_greater_heal_refund);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_ITEM_EFFICIENCY });
    }

    void OnProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(GetTarget(), SPELL_PRIEST_ITEM_EFFICIENCY, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_item_greater_heal_refund::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 92833 - Leap of Faith: the target is pulled to the priest
class spell_pri_leap_of_faith_effect_trigger : public SpellScript
{
    PrepareSpellScript(spell_pri_leap_of_faith_effect_trigger);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_LEAP_OF_FAITH_EFFECT });
    }

    void HandleEffectDummy(SpellEffIndex /*effIndex*/)
    {
        WorldLocation const* dest = GetHitDest();
        SpellInfo const* jump = sSpellMgr->GetSpellInfo(uint32(GetEffectValue()));
        if (!dest || !jump)
            return;

        SpellCastTargets targets;
        targets.SetDst(dest->GetPositionX(), dest->GetPositionY(), dest->GetPositionZ(), dest->GetOrientation());
        targets.SetUnitTarget(GetCaster());
        GetHitUnit()->CastSpell(targets, jump, nullptr, TRIGGERED_NONE);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pri_leap_of_faith_effect_trigger::HandleEffectDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 7001 - Lightwell Renew: Glyph of Lightwell
class spell_pri_lightwell_renew : public AuraScript
{
    PrepareAuraScript(spell_pri_lightwell_renew);

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        if (Unit* caster = GetCaster())
            if (AuraEffect const* glyph = caster->GetAuraEffect(SPELL_PRIEST_GLYPH_OF_LIGHTWELL, EFFECT_0))
                AddPct(amount, glyph->GetAmount());
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_lightwell_renew::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
    }
};

// 28305 - Mana Leech (Passive) (Priest Pet Aura)
class spell_pri_mana_leech : public AuraScript
{
    PrepareAuraScript(spell_pri_mana_leech);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_MANA_LEECH_PROC });
    }

    bool CheckProc(ProcEventInfo& /*eventInfo*/)
    {
        return GetTarget()->GetOwner() != nullptr;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        if (Unit* owner = GetTarget()->GetOwner())
            GetTarget()->CastSpell(owner, SPELL_PRIEST_MANA_LEECH_PROC, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pri_mana_leech::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pri_mana_leech::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -88994 - Masochism: Shadow Word: Death on yourself, or a hit for $s2% of your health
class spell_pri_masochism : public AuraScript
{
    PrepareAuraScript(spell_pri_masochism);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_SHADOW_WORD_DEATH });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        DamageInfo const* damage = eventInfo.GetDamageInfo();
        if (!damage)
            return false;

        if (eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->Id == SPELL_PRIEST_SHADOW_WORD_DEATH && eventInfo.GetActor() == GetTarget())
            return true;

        return damage->GetDamage() >= CalculatePct(GetTarget()->GetMaxHealth(), GetSpellInfo()->Effects[EFFECT_1].CalcValue());
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pri_masochism::CheckProc);
    }
};

// 8092 - Mind Blast: Shadow Orbs' bonus; Improved Mind Blast's Mind Trauma in Shadowform
class spell_pri_mind_blast : public SpellScript
{
    PrepareSpellScript(spell_pri_mind_blast);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_MIND_TRAUMA, SPELL_PRIEST_EMPOWERED_SHADOW });
    }

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (AuraEffect const* orbs = caster->GetDummyAuraEffect(SPELLFAMILY_GENERIC, PRIEST_ICON_ID_SHADOW_ORB, EFFECT_0))
        {
            int32 damage = GetHitDamage();
            AddPct(damage, orbs->GetAmount());
            SetHitDamage(damage);
        }

        if (caster->GetShapeshiftForm() == FORM_SHADOW)
            if (AuraEffect* improved = caster->GetAuraEffect(SPELL_AURA_ADD_FLAT_MODIFIER, SPELLFAMILY_PRIEST, PRIEST_ICON_ID_IMPROVED_MIND_BLAST, EFFECT_0))
                if (roll_chance_i(improved->GetSpellInfo()->Effects[EFFECT_1].CalcValue(caster)))
                    caster->CastSpell(GetHitUnit(), SPELL_PRIEST_MIND_TRAUMA, true, nullptr, improved);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pri_mind_blast::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 73510 - Mind Spike: Shadow Orbs' bonus
class spell_pri_mind_spike : public SpellScript
{
    PrepareSpellScript(spell_pri_mind_spike);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_EMPOWERED_SHADOW });
    }

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        if (AuraEffect const* orbs = GetCaster()->GetDummyAuraEffect(SPELLFAMILY_GENERIC, PRIEST_ICON_ID_SHADOW_ORB, EFFECT_0))
        {
            int32 damage = GetHitDamage();
            AddPct(damage, orbs->GetAmount());
            SetHitDamage(damage);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pri_mind_spike::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 47948 - Pain and Suffering (Proc): Mind Flay refreshes Shadow Word: Pain
class spell_pri_pain_and_suffering_proc : public SpellScript
{
    PrepareSpellScript(spell_pri_pain_and_suffering_proc);

    void HandleEffectScriptEffect(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (AuraEffect* pain = GetHitUnit()->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_PRIEST, 0x8000, 0, 0, caster->GetGUID()))
        {
            pain->RecalculateAmount(caster);
            pain->CalculatePeriodic(caster, false, false);
            pain->GetBase()->RefreshDuration();
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pri_pain_and_suffering_proc::HandleEffectScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 47540 - Penance
class spell_pri_penance : public SpellScript
{
    PrepareSpellScript(spell_pri_penance);

    bool Load() override
    {
        return GetCaster()->IsPlayer();
    }

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_PENANCE_DAMAGE, SPELL_PRIEST_PENANCE_HEAL, SPELL_PRIEST_ITEM_T11_HEALER_4P_BONUS,
            SPELL_PRIEST_INDULGENCE_OF_THE_PENITENT });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        if (!target || !target->IsAlive())
            return;

        if (caster->IsFriendlyTo(target))
        {
            caster->CastSpell(target, SPELL_PRIEST_PENANCE_HEAL, TRIGGERED_DISALLOW_PROC_EVENTS);
            // T11 healer 4P
            if (caster->GetAuraEffect(SPELL_PRIEST_ITEM_T11_HEALER_4P_BONUS, EFFECT_0))
                caster->CastSpell(caster, SPELL_PRIEST_INDULGENCE_OF_THE_PENITENT, true);
        }
        else
            caster->CastSpell(target, SPELL_PRIEST_PENANCE_DAMAGE, TRIGGERED_DISALLOW_PROC_EVENTS);
    }

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();
        if (Unit* target = GetExplTargetUnit())
            if (!caster->IsFriendlyTo(target))
            {
                if (!caster->IsValidAttackTarget(target))
                    return SPELL_FAILED_BAD_TARGETS;
                if (!caster->isInFront(target))
                    return SPELL_FAILED_UNIT_NOT_INFRONT;
            }
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_pri_penance::CheckCast);
        OnEffectHitTarget += SpellEffectFn(spell_pri_penance::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// -47569 - Phantasm: Fade may remove movement impairing effects
class spell_pri_phantasm : public AuraScript
{
    PrepareAuraScript(spell_pri_phantasm);

    bool CheckProc(ProcEventInfo& /*eventInfo*/)
    {
        return roll_chance_i(GetEffect(EFFECT_0)->GetAmount());
    }

    void HandleEffectProc(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->RemoveMovementImpairingAuras(true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pri_phantasm::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pri_phantasm::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 17 - Power Word: Shield: Weakened Soul
class spell_pri_power_word_shield : public SpellScript
{
    PrepareSpellScript(spell_pri_power_word_shield);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_WEAKENED_SOUL });
    }

    void HandleWeakenedSoul(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_PRIEST_WEAKENED_SOUL, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pri_power_word_shield::HandleWeakenedSoul, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// 17 - Power Word: Shield (aura)
// 87% of spell power (+ Borrowed Time), Twin Disciplines, Focused Power, Shield Discipline; the glyph's heal;
// Reflective Shield.
class spell_pri_power_word_shield_aura : public AuraScript
{
    PrepareAuraScript(spell_pri_power_word_shield_aura);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_REFLECTIVE_SHIELD_TRIGGERED, SPELL_PRIEST_REFLECTIVE_SHIELD_R1,
            SPELL_PRIEST_GLYPH_OF_POWER_WORD_SHIELD, SPELL_PRIEST_GLYPH_OF_POWER_WORD_SHIELD_HEAL });
    }

    void CalculateAmount(AuraEffect const* aurEff, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;
        Unit* caster = GetCaster();
        if (!caster)
            return;

        float bonus = 0.87f;
        // Borrowed Time
        if (AuraEffect const* borrowedTime = caster->GetDummyAuraEffect(SPELLFAMILY_PRIEST, PRIEST_ICON_ID_BORROWED_TIME, EFFECT_1))
            bonus += CalculatePct(1.0f, borrowedTime->GetAmount());

        bonus *= caster->SpellBaseHealingBonusDone(GetSpellInfo()->GetSchoolMask());
        // Improved Power Word: Shield is a modifier on all effects; it applies to the spell power share here
        bonus = caster->ApplyEffectModifiers(GetSpellInfo(), aurEff->GetEffIndex(), bonus);
        amount += int32(bonus);

        // Twin Disciplines
        if (AuraEffect const* twin = caster->GetAuraEffectOfRankedSpell(SPELL_PRIEST_TWIN_DISCIPLINES_RANK_1, EFFECT_1))
            AddPct(amount, twin->GetAmount());

        // Focused Power
        amount = int32(amount * caster->GetTotalAuraMultiplier(SPELL_AURA_MOD_HEALING_DONE_PERCENT));

        // Mastery: Shield Discipline
        if (AuraEffect const* shieldDiscipline = caster->GetDummyAuraEffect(SPELLFAMILY_HUNTER, PRIEST_ICON_ID_SHIELD_DISCIPLINE, EFFECT_0))
            AddPct(amount, shieldDiscipline->GetAmount());

        // Glyph of Power Word: Shield
        if (Unit* target = GetUnitOwner())
            if (AuraEffect const* glyph = caster->GetAuraEffect(SPELL_PRIEST_GLYPH_OF_POWER_WORD_SHIELD, EFFECT_0))
            {
                int32 heal = CalculatePct(amount, glyph->GetAmount());
                caster->CastCustomSpell(target, SPELL_PRIEST_GLYPH_OF_POWER_WORD_SHIELD_HEAL, &heal, nullptr, nullptr, true, nullptr, aurEff);
            }
    }

    void ReflectDamage(AuraEffect* aurEff, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        Unit* target = GetTarget();
        if (!dmgInfo.GetAttacker() || dmgInfo.GetAttacker() == target)
            return;

        if (AuraEffect const* talent = target->GetAuraEffectOfRankedSpell(SPELL_PRIEST_REFLECTIVE_SHIELD_R1, EFFECT_0))
        {
            int32 bp = CalculatePct(int32(absorbAmount), talent->GetAmount());
            target->CastCustomSpell(dmgInfo.GetAttacker(), SPELL_PRIEST_REFLECTIVE_SHIELD_TRIGGERED, &bp, nullptr, nullptr, true, nullptr, aurEff);
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_power_word_shield_aura::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        AfterEffectAbsorb += AuraEffectAbsorbFn(spell_pri_power_word_shield_aura::ReflectDamage, EFFECT_0);
    }
};

// 33110 - Prayer of Mending Heal: T9 healer 2P
class spell_pri_prayer_of_mending_heal : public SpellScript
{
    PrepareSpellScript(spell_pri_prayer_of_mending_heal);

    void HandleHeal(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetOriginalCaster())
            if (AuraEffect* t9 = caster->GetAuraEffect(SPELL_PRIEST_T9_HEALING_2P, EFFECT_0))
            {
                int32 heal = GetHitHeal();
                AddPct(heal, t9->GetAmount());
                SetHitHeal(heal);
            }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pri_prayer_of_mending_heal::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
    }
};

// 8122 - Psychic Scream: the third effect (the glyph's root) only with the glyph
class spell_pri_psychic_scream : public SpellScript
{
    PrepareSpellScript(spell_pri_psychic_scream);

    void FilterGlyphTargets(std::list<WorldObject*>& targets)
    {
        if (!GetCaster()->GetDummyAuraEffect(SPELLFAMILY_PRIEST, PRIEST_ICON_ID_GLYPH_OF_PSYCHIC_SCREAM, EFFECT_0))
            targets.clear();
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_psychic_scream::FilterGlyphTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// 139 - Renew: Divine Touch heals a share of the whole Renew at once
class spell_pri_renew : public AuraScript
{
    PrepareAuraScript(spell_pri_renew);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_DIVINE_TOUCH });
    }

    bool Load() override
    {
        return GetCaster() && GetCaster()->IsPlayer();
    }

    void HandleApplyEffect(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        if (AuraEffect const* divineTouch = caster->GetDummyAuraEffect(SPELLFAMILY_PRIEST, PRIEST_ICON_ID_DIVINE_TOUCH_TALENT, EFFECT_0))
        {
            // the tick amount already carries the healing bonuses (AuraEffect::CalculateAmount)
            int32 heal = CalculatePct(aurEff->GetAmount() * int32(aurEff->GetTotalTicks()), divineTouch->GetAmount());
            caster->CastCustomSpell(GetTarget(), SPELL_PRIEST_DIVINE_TOUCH, &heal, nullptr, nullptr, true, nullptr, aurEff);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_pri_renew::HandleApplyEffect, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }
};

// 77487 - Shadow Orb: Mind Blast and Mind Spike use the orbs up for Empowered Shadow
class spell_pri_shadow_orb : public AuraScript
{
    PrepareAuraScript(spell_pri_shadow_orb);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_SHADOW_ORB_MARKER, SPELL_PRIEST_EMPOWERED_SHADOW });
    }

    void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetStackAmount() == GetSpellInfo()->StackAmount)
            GetTarget()->CastSpell(GetTarget(), SPELL_PRIEST_SHADOW_ORB_MARKER, true);
    }

    void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_PRIEST_SHADOW_ORB_MARKER);
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        Unit* target = GetTarget();
        int32 bp = 10;
        if (AuraEffect const* mastery = target->GetAuraEffect(SPELL_AURA_PROC_TRIGGER_SPELL, SPELLFAMILY_PRIEST, 0, 0, 0x00100000))
            bp += mastery->GetAmount();

        target->CastCustomSpell(target, SPELL_PRIEST_EMPOWERED_SHADOW, &bp, &bp, nullptr, true, nullptr, aurEff);
        Remove();
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_pri_shadow_orb::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        AfterEffectRemove += AuraEffectRemoveFn(spell_pri_shadow_orb::HandleEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        OnEffectProc += AuraEffectProcFn(spell_pri_shadow_orb::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 77486 - Shadow Orb Power (Shadow mastery)
class spell_pri_shadow_orb_power : public AuraScript
{
    PrepareAuraScript(spell_pri_shadow_orb_power);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->Effects[EFFECT_0].TriggerSpell });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        int32 amount = 10 + aurEff->GetAmount();
        GetTarget()->CastCustomSpell(GetTarget(), GetSpellInfo()->Effects[EFFECT_0].TriggerSpell, &amount, nullptr, nullptr, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_shadow_orb_power::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 95740 - Shadow Orbs (Passive): 10% (+ Harnessed Shadows) per Shadow Word: Pain or Mind Flay tick
class spell_pri_shadow_orbs : public AuraScript
{
    PrepareAuraScript(spell_pri_shadow_orbs);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->Effects[EFFECT_0].TriggerSpell });
    }

    bool CheckProc(ProcEventInfo& /*eventInfo*/)
    {
        Unit* target = GetTarget();
        // the mastery's own proc takes over
        if (target->GetAuraEffect(SPELL_AURA_PROC_TRIGGER_SPELL, SPELLFAMILY_PRIEST, 0, 0, 0x00100000))
            return false;

        int32 chance = 10;
        if (AuraEffect const* harnessed = target->GetAuraEffect(SPELL_AURA_ADD_FLAT_MODIFIER, SPELLFAMILY_PRIEST, PRIEST_ICON_ID_HARNESSED_SHADOWS, EFFECT_0))
            chance += harnessed->GetAmount();
        return roll_chance_i(chance);
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        int32 bp = 10;
        GetTarget()->CastCustomSpell(GetTarget(), GetSpellInfo()->Effects[EFFECT_0].TriggerSpell, &bp, nullptr, nullptr, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pri_shadow_orbs::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pri_shadow_orbs::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 32379 - Shadow Word: Death
// Triple damage below 25% health (Mind Melt adds to it); what does not kill hurts the priest (Pain and Suffering
// lowers it); the glyph resets the cooldown once when the target survives at or below its health share.
class spell_pri_shadow_word_death : public SpellScript
{
    PrepareSpellScript(spell_pri_shadow_word_death);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_SHADOW_WORD_DEATH, SPELL_PRIEST_GLYPH_OF_SHADOW_WORD_DEATH });
    }

    void HandleDamage(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        int32 damage = GetHitDamage();

        if (target->GetHealthPct() < 25.0f)
        {
            damage *= 3;
            if (AuraEffect const* mindMelt = caster->GetDummyAuraEffect(SPELLFAMILY_PRIEST, PRIEST_ICON_ID_MIND_MELT, EFFECT_0))
                AddPct(damage, mindMelt->GetAmount());
        }

        SetHitDamage(damage);
        _launchHealthPct = target->GetHealthPct();

        if (target->GetHealth() > uint32(damage))
        {
            if (AuraEffect const* pain = caster->GetDummyAuraEffect(SPELLFAMILY_PRIEST, PRIEST_ICON_ID_PAIN_AND_SUFFERING, EFFECT_1))
                AddPct(damage, pain->GetAmount());
            caster->CastCustomSpell(caster, SPELL_PRIEST_SHADOW_WORD_DEATH, &damage, nullptr, nullptr, true);
        }
    }

    void HandleGlyph()
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        if (!target || caster == target)
            return;

        if (AuraEffect const* glyph = caster->GetDummyAuraEffect(SPELLFAMILY_PRIEST, PRIEST_ICON_ID_GLYPH_OF_SHADOW_WORD_DEATH, EFFECT_0))
            if (_launchHealthPct <= glyph->GetAmount() && target->IsAlive() && !caster->HasAura(SPELL_PRIEST_GLYPH_OF_SHADOW_WORD_DEATH))
            {
                if (Player* player = caster->ToPlayer())
                    player->RemoveSpellCooldown(GetSpellInfo()->Id, true);
                caster->CastSpell(caster, SPELL_PRIEST_GLYPH_OF_SHADOW_WORD_DEATH, true); // cooldown marker
            }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pri_shadow_word_death::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        AfterHit += SpellHitFn(spell_pri_shadow_word_death::HandleGlyph);
    }

private:
    float _launchHealthPct = 0.0f;
};

// 15473 - Shadowform: the visual, with or without the Glyph of Shadow
class spell_pri_shadowform : public AuraScript
{
    PrepareAuraScript(spell_pri_shadowform);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH, SPELL_PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH });
    }

    void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->CastSpell(GetTarget(), GetTarget()->HasAura(SPELL_PRIEST_GLYPH_OF_SHADOW) ?
            SPELL_PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH : SPELL_PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH, true);
    }

    void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH);
        GetTarget()->RemoveAurasDueToSpell(SPELL_PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_pri_shadowform::HandleEffectApply, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        AfterEffectRemove += AuraEffectRemoveFn(spell_pri_shadowform::HandleEffectRemove, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }
};

// 15487 - Silence: also interrupts a creature's cast
class spell_pri_silence : public SpellScript
{
    PrepareSpellScript(spell_pri_silence);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_GEN_INTERRUPT });
    }

    void HandleCreatureInterrupt(SpellEffIndex /*effIndex*/)
    {
        if (Creature* target = GetHitCreature())
            GetCaster()->CastSpell(target, SPELL_GEN_INTERRUPT, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pri_silence::HandleCreatureInterrupt, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// 20711 - Spirit of Redemption: a killing blow turns the priest into the spirit
class spell_pri_spirit_of_redemption : public AuraScript
{
    PrepareAuraScript(spell_pri_spirit_of_redemption);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_SPIRIT_OF_REDEMPTION_TRIGGERED });
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        amount = -1;
    }

    void Absorb(AuraEffect* aurEff, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        Unit* target = GetTarget();
        if (dmgInfo.GetDamage() >= target->GetHealth() && target->GetAuraEffectsByType(SPELL_AURA_SPIRIT_OF_REDEMPTION).empty())
        {
            absorbAmount = dmgInfo.GetDamage();
            target->CastSpell(target, SPELL_PRIEST_SPIRIT_OF_REDEMPTION_TRIGGERED, true, nullptr, aurEff);
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_spirit_of_redemption::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_pri_spirit_of_redemption::Absorb, EFFECT_0);
    }
};

// -89488 - Strength of Soul: Inner Focus' buff; heals shorten Weakened Soul
class spell_pri_strength_of_soul : public AuraScript
{
    PrepareAuraScript(spell_pri_strength_of_soul);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_INNER_FOCUS, SPELL_PRIEST_STRENGTH_OF_SOUL_R1, SPELL_PRIEST_STRENGTH_OF_SOUL_TRIGGERED_R1,
            SPELL_PRIEST_STRENGTH_OF_SOUL_TRIGGERED_R2 });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* target = GetTarget();
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo)
            return;

        if (spellInfo->Id == SPELL_PRIEST_INNER_FOCUS)
            target->CastSpell(target, aurEff->GetId() == SPELL_PRIEST_STRENGTH_OF_SOUL_R1 ?
                SPELL_PRIEST_STRENGTH_OF_SOUL_TRIGGERED_R1 : SPELL_PRIEST_STRENGTH_OF_SOUL_TRIGGERED_R2, true, nullptr, aurEff);
        else if (Unit* procTarget = eventInfo.GetProcTarget())
        {
            int32 bp = aurEff->GetAmount() * IN_MILLISECONDS;
            target->CastCustomSpell(procTarget, GetSpellInfo()->Effects[EFFECT_0].TriggerSpell, &bp, nullptr, nullptr, true, nullptr, aurEff);
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_strength_of_soul::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 89490 - Strength of Soul (Weakened Soul shortened by the effect's milliseconds)
class spell_pri_strength_of_soul_script : public SpellScript
{
    PrepareSpellScript(spell_pri_strength_of_soul_script);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_WEAKENED_SOUL });
    }

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        if (Aura* weakenedSoul = GetHitUnit()->GetAura(SPELL_PRIEST_WEAKENED_SOUL))
            weakenedSoul->SetDuration(std::max(0, weakenedSoul->GetDuration() - GetEffectValue()));
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pri_strength_of_soul_script::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 15286 - Vampiric Embrace: shadow damage heals the priest and (half as much) the group
class spell_pri_vampiric_embrace : public AuraScript
{
    PrepareAuraScript(spell_pri_vampiric_embrace);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_VAMPIRIC_EMBRACE_HEAL });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetDamage() || !damageInfo->GetSpellInfo())
            return false;
        // not Mind Sear
        return !(damageInfo->GetSpellInfo()->SpellFamilyFlags[1] & 0x80000);
    }

    void HandleEffectProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        int32 self = int32(CalculatePct(eventInfo.GetDamageInfo()->GetDamage(), aurEff->GetAmount()));
        int32 team = int32(CalculatePct(eventInfo.GetDamageInfo()->GetDamage(), aurEff->GetAmount() / 2));
        GetTarget()->CastCustomSpell(GetTarget(), SPELL_PRIEST_VAMPIRIC_EMBRACE_HEAL, &team, &self, nullptr, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pri_vampiric_embrace::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pri_vampiric_embrace::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 15290 - Vampiric Embrace (heal): the group part leaves the priest out
class spell_pri_vampiric_embrace_target : public SpellScript
{
    PrepareSpellScript(spell_pri_vampiric_embrace_target);

    void FilterTargets(std::list<WorldObject*>& unitList)
    {
        unitList.remove(GetCaster());
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_vampiric_embrace_target::FilterTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_PARTY);
    }
};

// 34914 - Vampiric Touch: dispelling it hurts the dispeller; Mind Blast on the target gives Replenishment
class spell_pri_vampiric_touch : public AuraScript
{
    PrepareAuraScript(spell_pri_vampiric_touch);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_VAMPIRIC_TOUCH_DISPEL, SPELL_GEN_REPLENISHMENT });
    }

    void HandleDispel(DispelInfo* /*dispelInfo*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetUnitOwner())
                if (AuraEffect const* aurEff = GetEffect(EFFECT_1))
                {
                    int32 damage = aurEff->GetAmount() * 8;
                    caster->CastCustomSpell(target, SPELL_PRIEST_VAMPIRIC_TOUCH_DISPEL, &damage, nullptr, nullptr, true, nullptr, aurEff);
                }
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetProcTarget() == GetCaster();
    }

    void HandleEffectProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        eventInfo.GetProcTarget()->CastSpell(eventInfo.GetProcTarget(), SPELL_GEN_REPLENISHMENT, true, nullptr, aurEff);
    }

    void Register() override
    {
        AfterDispel += AuraDispelFn(spell_pri_vampiric_touch::HandleDispel);
        DoCheckProc += AuraCheckProcFn(spell_pri_vampiric_touch::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pri_vampiric_touch::HandleEffectProc, EFFECT_2, SPELL_AURA_DUMMY);
    }
};

// 60123 - Lightwell
class spell_pri_lightwell : public SpellScript
{
    PrepareSpellScript(spell_pri_lightwell);

    bool Load() override
    {
        return GetCaster()->IsCreature();
    }

    void HandleScriptEffect(SpellEffIndex /* effIndex */)
    {
        Creature* caster = GetCaster()->ToCreature();
        if (!caster || !caster->IsSummon())
            return;

        uint32 lightwellRenew = 0;
        switch (caster->GetEntry())
        {
            case PRIEST_LIGHTWELL_NPC_1: lightwellRenew = 7001; break;
            case PRIEST_LIGHTWELL_NPC_2: lightwellRenew = 27873; break;
            case PRIEST_LIGHTWELL_NPC_3: lightwellRenew = 27874; break;
            case PRIEST_LIGHTWELL_NPC_4: lightwellRenew = 28276; break;
            case PRIEST_LIGHTWELL_NPC_5: lightwellRenew = 48084; break;
            case PRIEST_LIGHTWELL_NPC_6: lightwellRenew = 48085; break;
        }

        // proc a spellcast
        if (Aura* chargesAura = caster->GetAura(SPELL_PRIEST_LIGHTWELL_CHARGES))
        {
            caster->CastSpell(GetHitUnit(), lightwellRenew, true, nullptr, nullptr,
                caster->ToTempSummon()->GetSummonerGUID());
            if (chargesAura->ModCharges(-1))
                caster->ToTempSummon()->UnSummon();
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pri_lightwell::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 8129 - Mana Burn
class spell_pri_mana_burn : public SpellScript
{
    PrepareSpellScript(spell_pri_mana_burn);

    void HandleAfterHit()
    {
        if (Unit* unitTarget = GetHitUnit())
            unitTarget->RemoveAurasWithMechanic((1ULL << MECHANIC_FEAR) | (1ULL << MECHANIC_POLYMORPH));
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_pri_mana_burn::HandleAfterHit);
    }
};

// 605 - Mind Control
class spell_pri_mind_control : public AuraScript
{
    PrepareAuraScript(spell_pri_mind_control);

    void HandleApplyEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* target = GetTarget())
            {
                caster->SetInCombatWith(target);
                target->SetInCombatWith(caster);
            }
        }
    }

    void HandleRemoveEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* target = GetTarget())
            {
                caster->SetCombatTimer(0);
                target->SetCombatTimer(0);
            }
        }
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_pri_mind_control::HandleApplyEffect, EFFECT_0, SPELL_AURA_MOD_POSSESS, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_pri_mind_control::HandleRemoveEffect, EFFECT_0, SPELL_AURA_MOD_POSSESS, AURA_EFFECT_HANDLE_REAL);
    }
};

// -49821 - Mind Sear
class spell_pri_mind_sear : public SpellScript
{
    PrepareSpellScript(spell_pri_mind_sear);

    void FilterTargets(std::list<WorldObject*>& unitList)
    {
        unitList.remove_if(Acore::ObjectGUIDCheck(GetCaster()->GetGuidValue(UNIT_FIELD_CHANNEL_OBJECT), true));
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_mind_sear::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

// 57989 - Shadowfiend Death
class spell_pri_shadowfiend_death : public AuraScript
{
    PrepareAuraScript(spell_pri_shadowfiend_death);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_GLYPH_OF_SHADOWFIEND_MANA });
    }

    bool AfterCheckProc(ProcEventInfo& eventInfo, bool isTriggeredAtSpellProcEvent)
    {
        if (!isTriggeredAtSpellProcEvent)
            return false;
        return eventInfo.GetTypeMask() & PROC_FLAG_KILLED;
    }

    void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        if (Unit* owner = GetTarget()->GetOwner())
            if (owner->HasAura(SPELL_PRIEST_GLYPH_OF_SHADOWFIEND))
                owner->CastSpell(owner, SPELL_PRIEST_GLYPH_OF_SHADOWFIEND_MANA, true);
    }

    void Register() override
    {
        DoAfterCheckProc += AuraAfterCheckProcFn(spell_pri_shadowfiend_death::AfterCheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pri_shadowfiend_death::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 26169 - AQ 3P Bonus
class spell_pri_aq_3p_bonus : public AuraScript
{
    PrepareAuraScript(spell_pri_aq_3p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_ORACULAR_HEAL });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetActor() == eventInfo.GetActionTarget())
            return false;

        HealInfo* healInfo = eventInfo.GetHealInfo();
        return healInfo && healInfo->GetHeal();
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        int32 bp0 = CalculatePct(eventInfo.GetHealInfo()->GetHeal(), 10);
        eventInfo.GetActor()->CastCustomSpell(SPELL_PRIEST_ORACULAR_HEAL, SPELLVALUE_BASE_POINT0, bp0, eventInfo.GetActor(), true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pri_aq_3p_bonus::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pri_aq_3p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 40438 - Priest Tier 6 Trinket
class spell_pri_item_t6_trinket : public AuraScript
{
    PrepareAuraScript(spell_pri_item_t6_trinket);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_PRIEST_DIVINE_BLESSING,
            SPELL_PRIEST_DIVINE_WRATH
        });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* caster = eventInfo.GetActor();
        if (eventInfo.GetSpellTypeMask() & PROC_SPELL_TYPE_HEAL)
            caster->CastSpell(caster, SPELL_PRIEST_DIVINE_BLESSING, true, nullptr, aurEff);

        if (eventInfo.GetSpellTypeMask() & PROC_SPELL_TYPE_DAMAGE)
            caster->CastSpell(caster, SPELL_PRIEST_DIVINE_WRATH, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_item_t6_trinket::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 28809 - T3 4P Bonus
class spell_pri_t3_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_pri_t3_4p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_ARMOR_OF_FAITH });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        eventInfo.GetActor()->CastSpell(eventInfo.GetActionTarget(), SPELL_PRIEST_ARMOR_OF_FAITH, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_t3_4p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 37565 - Flexibility | Item - Priest T4 Holy/Discipline 4P Bonus
class spell_pri_t4_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_pri_t4_4p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_T4_4P_FLEXIBILITY });
    }

    void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->RemoveAurasDueToSpell(SPELL_PRIEST_T4_4P_FLEXIBILITY);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_t4_4p_bonus::HandleProc, EFFECT_ALL, SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
    }
};

// 70770 - Item - Priest T10 Healer 2P Bonus
class spell_pri_t10_heal_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_pri_t10_heal_2p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_BLESSED_HEALING });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        HealInfo* healInfo = eventInfo.GetHealInfo();
        return healInfo && healInfo->GetHeal();
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        SpellInfo const* spellInfo = sSpellMgr->AssertSpellInfo(SPELL_PRIEST_BLESSED_HEALING);
        int32 amount = CalculatePct(static_cast<int32>(eventInfo.GetHealInfo()->GetHeal()), aurEff->GetAmount());

        ASSERT(spellInfo->GetMaxTicks() > 0);
        amount /= spellInfo->GetMaxTicks();

        Unit* caster = eventInfo.GetActor();
        Unit* target = eventInfo.GetActionTarget();

        caster->CastCustomSpell(target, SPELL_PRIEST_BLESSED_HEALING, &amount, nullptr, nullptr, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pri_t10_heal_2p_bonus::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pri_t10_heal_2p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

void AddSC_priest_spell_scripts()
{
    RegisterSpellScript(spell_pri_archangel);
    RegisterSpellScript(spell_pri_atonement);
    RegisterSpellScript(spell_pri_atonement_heal);
    RegisterSpellScript(spell_pri_body_and_soul);
    RegisterSpellScript(spell_pri_chakra);
    RegisterSpellScript(spell_pri_chakra_flow_removal);
    RegisterSpellScript(spell_pri_chakra_sanctuary);
    RegisterSpellScript(spell_pri_chakra_serenity_script);
    RegisterSpellScript(spell_pri_circle_of_healing);
    RegisterSpellScript(spell_pri_devouring_plague);
    RegisterSpellScript(spell_pri_dispel_magic);
    RegisterSpellScript(spell_pri_divine_aegis);
    RegisterSpellScript(spell_pri_divine_hymn);
    RegisterSpellScript(spell_pri_echo_of_light);
    RegisterSpellScript(spell_pri_evangelism);
    RegisterSpellScript(spell_pri_evangelism_triggered);
    RegisterSpellScript(spell_pri_glyph_of_prayer_of_healing);
    RegisterSpellScript(spell_pri_guardian_spirit);
    RegisterSpellScript(spell_pri_harnessed_shadows);
    RegisterSpellScript(spell_pri_holy_word_sanctuary);
    RegisterSpellScript(spell_pri_holy_word_sanctuary_triggered);
    RegisterSpellScript(spell_pri_hymn_of_hope);
    RegisterSpellScript(spell_pri_improved_power_word_shield);
    RegisterSpellScript(spell_pri_item_greater_heal_refund);
    RegisterSpellScript(spell_pri_leap_of_faith_effect_trigger);
    RegisterSpellScript(spell_pri_lightwell_renew);
    RegisterSpellScript(spell_pri_mana_leech);
    RegisterSpellScript(spell_pri_masochism);
    RegisterSpellScript(spell_pri_mind_blast);
    RegisterSpellScript(spell_pri_mind_spike);
    RegisterSpellScript(spell_pri_pain_and_suffering_proc);
    RegisterSpellScript(spell_pri_penance);
    RegisterSpellScript(spell_pri_phantasm);
    RegisterSpellAndAuraScriptPair(spell_pri_power_word_shield, spell_pri_power_word_shield_aura);
    RegisterSpellScript(spell_pri_prayer_of_mending_heal);
    RegisterSpellScript(spell_pri_psychic_scream);
    RegisterSpellScript(spell_pri_renew);
    RegisterSpellScript(spell_pri_shadow_orb);
    RegisterSpellScript(spell_pri_shadow_orb_power);
    RegisterSpellScript(spell_pri_shadow_orbs);
    RegisterSpellScript(spell_pri_shadow_word_death);
    RegisterSpellScript(spell_pri_shadowform);
    RegisterSpellScript(spell_pri_silence);
    RegisterSpellScript(spell_pri_spirit_of_redemption);
    RegisterSpellScript(spell_pri_strength_of_soul);
    RegisterSpellScript(spell_pri_strength_of_soul_script);
    RegisterSpellScript(spell_pri_vampiric_embrace);
    RegisterSpellScript(spell_pri_vampiric_embrace_target);
    RegisterSpellScript(spell_pri_vampiric_touch);
    // AzerothCore's, still right for 4.3.4
    RegisterSpellScript(spell_pri_lightwell);
    RegisterSpellScript(spell_pri_mana_burn);
    RegisterSpellScript(spell_pri_mind_control);
    RegisterSpellScript(spell_pri_mind_sear);
    RegisterSpellScript(spell_pri_shadowfiend_death);
    RegisterSpellScript(spell_pri_aq_3p_bonus);
    RegisterSpellScript(spell_pri_item_t6_trinket);
    RegisterSpellScript(spell_pri_t3_4p_bonus);
    RegisterSpellScript(spell_pri_t4_4p_bonus);
    RegisterSpellScript(spell_pri_t10_heal_2p_bonus);
}
