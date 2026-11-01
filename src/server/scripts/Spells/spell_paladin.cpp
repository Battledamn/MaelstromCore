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

#include "GameTime.h"
#include "Group.h"
#include "Player.h"
#include "Spell.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "SpellScriptLoader.h"
#include "TemporarySummon.h"
#include "UnitAI.h"
/*
 * Scripts for spells with SPELLFAMILY_PALADIN and SPELLFAMILY_GENERIC spells used by paladin players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_pal_".
 */

enum PaladinSpells
{
    SPELL_PALADIN_DIVINE_PLEA                    = 54428,
    SPELL_PALADIN_BLESSING_OF_SANCTUARY_BUFF     = 67480,
    SPELL_PALADIN_BLESSING_OF_SANCTUARY_ENERGIZE = 57319,

    SPELL_PALADIN_HOLY_SHOCK_R1                  = 20473,
    SPELL_PALADIN_HOLY_SHOCK_R1_DAMAGE           = 25912,
    SPELL_PALADIN_HOLY_SHOCK_R1_HEALING          = 25914,

    SPELL_PALADIN_BLESSING_OF_LOWER_CITY_DRUID   = 37878,
    SPELL_PALADIN_BLESSING_OF_LOWER_CITY_PALADIN = 37879,
    SPELL_PALADIN_BLESSING_OF_LOWER_CITY_PRIEST  = 37880,
    SPELL_PALADIN_BLESSING_OF_LOWER_CITY_SHAMAN  = 37881,

    SPELL_PALADIN_DIVINE_STORM                   = 53385,
    SPELL_PALADIN_DIVINE_STORM_DUMMY             = 54171,
    SPELL_PALADIN_DIVINE_STORM_HEAL              = 54172,

    SPELL_PALADIN_EYE_FOR_AN_EYE_RANK_1          = 9799,
    SPELL_PALADIN_EYE_FOR_AN_EYE_DAMAGE          = 25997,
    SPELL_PALADIN_GUARDED_BY_THE_LIGHT           = 88063,
    SPELL_PALADIN_GUARDED_BY_THE_LIGHT_R2        = 85646,

    SPELL_PALADIN_FORBEARANCE                    = 25771,
    SPELL_PALADIN_AVENGING_WRATH_MARKER          = 61987,
    SPELL_PALADIN_IMMUNE_SHIELD_MARKER           = 61988,

    SPELL_PALADIN_HAND_OF_SACRIFICE              = 6940,
    SPELL_PALADIN_DIVINE_SACRIFICE               = 64205,

    SPELL_PALADIN_JUDGEMENT_DAMAGE               = 54158,
    SPELL_PALADIN_JUDGEMENT_OF_JUSTICE           = 20184,
    SPELL_PALADIN_JUDGEMENT_OF_LIGHT             = 20185,
    SPELL_PALADIN_JUDGEMENT_OF_WISDOM            = 20186,

    SPELL_PALADIN_GLYPH_OF_SALVATION             = 63225,

    SPELL_PALADIN_RIGHTEOUS_DEFENSE_TAUNT        = 31790,

    SPELL_PALADIN_SANCTIFIED_WRATH               = 57318,
    SPELL_PALADIN_SANCTIFIED_WRATH_TALENT_R1     = 53375,

    SPELL_PALADIN_SEAL_OF_RIGHTEOUSNESS          = 25742,

    SPELL_PALADIN_DEVOTION_AURA_R1               = 465,
    SPELL_PALADIN_RETRIBUTION_AURA_R1            = 7294,
    SPELL_PALADIN_CONCENTRACTION_AURA            = 19746,
    SPELL_PALADIN_SHADOW_RESISTANCE_AURA_R1      = 19876,
    SPELL_PALADIN_FROST_RESISTANCE_AURA_R1       = 19888,
    SPELL_PALADIN_FIRE_RESISTANCE_AURA_R1        = 19891,
    SPELL_PALADIN_CRUSADER_AURA                  = 32223,
    SPELL_PALADIN_SANCTIFIED_RETRIBUTION_R1      = 31869,
    SPELL_PALADIN_SWIFT_RETRIBUTION_R1           = 53379,

    SPELL_PALADIN_IMPROVED_CONCENTRACTION_AURA   = 63510,
    SPELL_PALADIN_IMPROVED_DEVOTION_AURA         = 63514,
    SPELL_PALADIN_SANCTIFIED_RETRIBUTION_AURA    = 63531,
    SPELL_PALADIN_AURA_MASTERY_IMMUNE            = 64364,

    SPELL_JUDGEMENTS_OF_THE_JUST                 = 68055,
    SPELL_JUDGEMENT_OF_VENGEANCE_EFFECT          = 31804,
    SPELL_HOLY_VENGEANCE                         = 31803,
    SPELL_JUDGEMENT_OF_CORRUPTION_EFFECT         = 53733,
    SPELL_BLOOD_CORRUPTION                       = 53742,

    SPELL_GENERIC_ARENA_DAMPENING                = 74410,
    SPELL_GENERIC_BATTLEGROUND_DAMPENING         = 74411,

    // Crystalforge Raiment - Tier 5 Holy 2 Set
    SPELL_IMPROVED_JUDGEMENT                     = 37188,
    SPELL_IMPROVED_JUDGEMENT_ENERGIZE            = 43838,

    SPELL_PALADIN_HOLY_VENGEANCE                 = 31803,
    SPELL_PALADIN_BLOOD_CORRUPTION               = 53742,
    SPELL_PALADIN_SEAL_OF_VENGEANCE_EFFECT       = 42463,
    SPELL_PALADIN_SEAL_OF_CORRUPTION_EFFECT      = 53739,

    SPELL_PALADIN_SEAL_OF_COMMAND                = 20375
};

enum PaladinSpellIcons
{
    PALADIN_ICON_ID_RETRIBUTION_AURA             = 555,
    PALADIN_ICON_JUDGEMENTS_OF_THE_JUST          = 3015,
    PALADIN_ICON_JUDGEMENTS_OF_THE_WISE          = 3017,
    PALADIN_ICON_HAMMER_OF_THE_RIGHTEOUS         = 3023,
    PALADIN_ICON_RIGHTEOUS_VENGEANCE             = 3025,
    PALADIN_ICON_SHEATH_OF_LIGHT                 = 3030
};

enum MiscSpellIcons
{
    SPELL_ICON_ID_STRENGTH_OF_WRYNN              = 1704,
    SPELL_ICON_ID_HELLSCREAM_WARSONG             = 937
};

// Proc system triggered spell IDs
enum PaladinProcSpells
{
    SPELL_PALADIN_ILLUMINATION_ENERGIZE          = 20272,
    SPELL_PALADIN_JUDGEMENTS_OF_THE_WISE_MANA    = 31930,
    SPELL_PALADIN_REPLENISHMENT                  = 57669,
    SPELL_PALADIN_RIGHTEOUS_VENGEANCE_DOT        = 61840,
    SPELL_PALADIN_SHEATH_OF_LIGHT_HOT            = 54203,
    SPELL_PALADIN_JUDGEMENT_OF_LIGHT_HEAL        = 20267,
    SPELL_PALADIN_JUDGEMENT_OF_WISDOM_MANA       = 20268,
    SPELL_PALADIN_SPIRITUAL_ATTUNEMENT_MANA      = 31786,
    SPELL_PALADIN_BEACON_OF_LIGHT_AURA           = 53563,
    SPELL_PALADIN_LIGHTS_BEACON                  = 53651,
    SPELL_PALADIN_BEACON_OF_LIGHT_HL             = 53652,
    SPELL_PALADIN_BEACON_OF_LIGHT_FOL            = 53653,
    SPELL_PALADIN_BEACON_OF_LIGHT_HS             = 53654,
    SPELL_PALADIN_LAY_ON_HANDS_R1                = 633,
    SPELL_PALADIN_HOLY_LIGHT_R1                  = 635,
    SPELL_PALADIN_FLASH_OF_LIGHT_R1              = 19750,
    SPELL_PALADIN_GLYPH_OF_HOLY_LIGHT_HEAL       = 54968,
    SPELL_PALADIN_SACRED_SHIELD                  = 53601,
    SPELL_PALADIN_T9_HOLY_4P_BONUS               = 67191,
    SPELL_PALADIN_FLASH_OF_LIGHT_PROC            = 66922,
    SPELL_PALADIN_ENDURING_LIGHT                 = 40471,
    SPELL_PALADIN_ENDURING_JUDGEMENT             = 40472,
    SPELL_PALADIN_HOLY_POWER_ARMOR               = 28790,
    SPELL_PALADIN_HOLY_POWER_ATTACK_POWER        = 28791,
    SPELL_PALADIN_HOLY_POWER_SPELL_POWER         = 28793,
    SPELL_PALADIN_HOLY_POWER_MP5                 = 28795,
    SPELL_PALADIN_HOLY_MENDING                   = 64891,
    SPELL_PALADIN_GLYPH_OF_DIVINITY              = 54939,
    SPELL_PALADIN_GLYPH_OF_DIVINITY_PROC         = 54986,
    SPELL_PALADIN_HEART_OF_THE_CRUSADER_EFF_R1   = 21183,
    SPELL_PALADIN_JUDGEMENTS_OF_THE_JUST_PROC    = 68055,
    SPELL_PALADIN_SACRED_SHIELD_TRIGGER          = 58597,
    SPELL_PALADIN_T8_HOLY_4P_BONUS               = 64895
};

// 85285 - Sacred Shield
// Ported from The Cataclysm Preservation Project (spell_paladin.cpp at b061f46): in 4.3.4 a hit that
// leaves the paladin below 30% health triggers the absorb (twice its base plus 280% attack power).
class spell_pal_sacred_shield : public AuraScript
{
    PrepareAuraScript(spell_pal_sacred_shield);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (DamageInfo* damage = eventInfo.GetDamageInfo())
            return GetTarget()->HealthBelowPctDamaged(30, damage->GetDamage());

        return false;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();

        Unit* target = GetTarget();
        SpellInfo const* spell = sSpellMgr->AssertSpellInfo(GetSpellInfo()->Effects[aurEff->GetEffIndex()].TriggerSpell);
        int32 absorb = int32(spell->Effects[EFFECT_0].CalcValue() * 2 + target->GetTotalAttackPowerValue(BASE_ATTACK) * 2.8f);
        target->CastCustomSpell(spell->Id, SPELLVALUE_BASE_POINT0, absorb, target, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pal_sacred_shield::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pal_sacred_shield::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 31850 - Ardent Defender
// Ported from The Cataclysm Preservation Project (spell_paladin.cpp at b061f46): in 4.3.4 it is a
// cooldown whose own aura reduces the damage taken; a blow that would kill is absorbed instead and
// heals for 15% of maximum health, which ends the aura.
class spell_pal_ardent_defender : public AuraScript
{
    PrepareAuraScript(spell_pal_ardent_defender);

    enum Spell
    {
        PAL_SPELL_ARDENT_DEFENDER_HEAL   = 66235
    };

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ PAL_SPELL_ARDENT_DEFENDER_HEAL });
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        // Set absorbtion amount to unlimited
        amount = -1;
    }

    void Absorb(AuraEffect* aurEff, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        absorbAmount = 0;
        Unit* target = GetTarget();
        if (dmgInfo.GetDamage() < target->GetHealth())
            return;

        absorbAmount = dmgInfo.GetDamage();
        int32 health = int32(target->CountPctFromMaxHealth(15));
        target->CastCustomSpell(PAL_SPELL_ARDENT_DEFENDER_HEAL, SPELLVALUE_BASE_POINT0, health, target, true, nullptr, aurEff);
        Remove();
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_ardent_defender::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_pal_ardent_defender::Absorb, EFFECT_0);
    }
};

// 31821 - Aura Mastery
class spell_pal_aura_mastery : public AuraScript
{
    PrepareAuraScript(spell_pal_aura_mastery);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_AURA_MASTERY_IMMUNE });
    }

    void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->CastSpell(GetTarget(), SPELL_PALADIN_AURA_MASTERY_IMMUNE, true);
    }

    void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveOwnedAura(SPELL_PALADIN_AURA_MASTERY_IMMUNE, GetCasterGUID());
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_pal_aura_mastery::HandleEffectApply, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_pal_aura_mastery::HandleEffectRemove, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
    }
};

// 64364 - Aura Mastery Immune
class spell_pal_aura_mastery_immune : public AuraScript
{
    PrepareAuraScript(spell_pal_aura_mastery_immune);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_CONCENTRACTION_AURA });
    }

    bool CheckAreaTarget(Unit* target)
    {
        return target->HasAura(SPELL_PALADIN_CONCENTRACTION_AURA, GetCasterGUID());
    }

    void Register() override
    {
        DoCheckAreaTarget += AuraCheckAreaTargetFn(spell_pal_aura_mastery_immune::CheckAreaTarget);
    }
};

// 53563 - Beacon of Light
class spell_pal_beacon_of_light : public AuraScript
{
    PrepareAuraScript(spell_pal_beacon_of_light);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->Effects[EFFECT_0].TriggerSpell });
    }

    void PeriodicTick(AuraEffect const* aurEff)
    {
        PreventDefaultAction();

        // the beacon target casts Light's Beacon on its party and raid members, as TrinityCore's periodic
        // trigger does (the caster is the beacon target, which spell_pal_lights_beacon relies on)
        Unit* owner = GetAura()->GetUnitOwner();
        uint32 triggerSpell = GetSpellInfo()->Effects[aurEff->GetEffIndex()].TriggerSpell;
        owner->CastSpell(GetTarget(), triggerSpell, true, nullptr, aurEff, owner->GetGUID());
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_pal_beacon_of_light::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// 31884 - Avenging Wrath
class spell_pal_avenging_wrath : public AuraScript
{
    PrepareAuraScript(spell_pal_avenging_wrath);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_SANCTIFIED_WRATH, SPELL_PALADIN_SANCTIFIED_WRATH_TALENT_R1 });
    }

    // 4.3.4 (CPP): any rank of the Sanctified Wrath talent brings its aura while Avenging Wrath lasts
    void HandleApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        if (target->GetAuraOfRankedSpell(SPELL_PALADIN_SANCTIFIED_WRATH_TALENT_R1))
            target->CastSpell(target, SPELL_PALADIN_SANCTIFIED_WRATH, true, nullptr, aurEff);
    }

    void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_PALADIN_SANCTIFIED_WRATH);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_pal_avenging_wrath::HandleApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_pal_avenging_wrath::HandleRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
    }
};

// 37877 - Blessing of Faith
class spell_pal_blessing_of_faith : public SpellScript
{
    PrepareSpellScript(spell_pal_blessing_of_faith);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_PALADIN_BLESSING_OF_LOWER_CITY_DRUID,
                SPELL_PALADIN_BLESSING_OF_LOWER_CITY_PALADIN,
                SPELL_PALADIN_BLESSING_OF_LOWER_CITY_PRIEST,
                SPELL_PALADIN_BLESSING_OF_LOWER_CITY_SHAMAN
            });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* unitTarget = GetHitUnit())
        {
            uint32 spell_id = 0;
            switch (unitTarget->getClass())
            {
                case CLASS_DRUID:
                    spell_id = SPELL_PALADIN_BLESSING_OF_LOWER_CITY_DRUID;
                    break;
                case CLASS_PALADIN:
                    spell_id = SPELL_PALADIN_BLESSING_OF_LOWER_CITY_PALADIN;
                    break;
                case CLASS_PRIEST:
                    spell_id = SPELL_PALADIN_BLESSING_OF_LOWER_CITY_PRIEST;
                    break;
                case CLASS_SHAMAN:
                    spell_id = SPELL_PALADIN_BLESSING_OF_LOWER_CITY_SHAMAN;
                    break;
                default:
                    return; // ignore for non-healing classes
            }
            Unit* caster = GetCaster();
            caster->CastSpell(caster, spell_id, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pal_blessing_of_faith::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 64205 - Divine Sacrifice
class spell_pal_divine_sacrifice : public AuraScript
{
    PrepareAuraScript(spell_pal_divine_sacrifice);

    uint32 groupSize, minHpPct;
    int32 remainingAmount;

    bool Load() override
    {
        if (Unit* caster = GetCaster())
        {
            if (caster->IsPlayer())
            {
                if (caster->ToPlayer()->GetGroup())
                    groupSize = caster->ToPlayer()->GetGroup()->GetMembersCount();
                else
                    groupSize = 1;
            }
            else
                return false;

            remainingAmount = (caster->CountPctFromMaxHealth(GetSpellInfo()->Effects[EFFECT_2].CalcValue(caster)) * groupSize);
            minHpPct = GetSpellInfo()->Effects[EFFECT_1].CalcValue(caster);
            return true;
        }
        return false;
    }

    void Split(AuraEffect* /*aurEff*/, DamageInfo& /*dmgInfo*/, uint32& splitAmount)
    {
        remainingAmount -= splitAmount;
        // break when absorbed everything it could, or if the casters hp drops below 20%
        if (Unit* caster = GetCaster())
            if (remainingAmount <= 0 || (caster->GetHealthPct() < minHpPct))
                caster->RemoveAura(SPELL_PALADIN_DIVINE_SACRIFICE);
    }

    void Register() override
    {
        OnEffectSplit += AuraEffectSplitFn(spell_pal_divine_sacrifice::Split, EFFECT_0);
    }
};

// 53385 - Divine Storm
// Ported from The Cataclysm Preservation Project (spell_paladin.cpp at b061f46): in 4.3.4 it grants a
// holy power charge (effect 0) only when it hits at least four enemies, and the heal goes to up to
// three party members.
class spell_pal_divine_storm : public SpellScript
{
    PrepareSpellScript(spell_pal_divine_storm);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_DIVINE_STORM_DUMMY });
    }

    void TriggerHeal()
    {
        Unit* caster = GetCaster();
        if (GetHitUnit() == caster)
            return;

        CustomSpellValues values;
        values.AddSpellMod(SPELLVALUE_BASE_POINT0, CalculatePct(GetHitDamage(), GetSpellInfo()->Effects[EFFECT_1].CalcValue(caster)));
        values.AddSpellMod(SPELLVALUE_MAX_TARGETS, 3);
        caster->CastCustomSpell(SPELL_PALADIN_DIVINE_STORM_DUMMY, values, caster, TRIGGERED_FULL_MASK);
    }

    void CountTargets(std::list<WorldObject*>& targets)
    {
        _targetCount = uint8(targets.size());
    }

    void HandleEnergize(SpellEffIndex effIndex)
    {
        if (_targetCount < 4)
            PreventHitDefaultEffect(effIndex);
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_pal_divine_storm::TriggerHeal);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_divine_storm::CountTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_pal_divine_storm::HandleEnergize, EFFECT_0, SPELL_EFFECT_ENERGIZE);
    }

private:
    uint8 _targetCount = 0;
};

// 54171 - Divine Storm (Dummy)
class spell_pal_divine_storm_dummy : public SpellScript
{
    PrepareSpellScript(spell_pal_divine_storm_dummy);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_DIVINE_STORM_HEAL });
    }

    void CountTargets(std::list<WorldObject*>& targetList)
    {
        Acore::Containers::RandomResize(targetList, GetSpellValue()->MaxAffectedTargets);
        _targetCount = targetList.size();
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (!_targetCount || ! GetHitUnit())
            return;

        int32 heal = GetEffectValue() / _targetCount;
        GetCaster()->CastCustomSpell(GetHitUnit(), SPELL_PALADIN_DIVINE_STORM_HEAL, &heal, nullptr, nullptr, true);
    }
private:
    uint32 _targetCount;

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pal_divine_storm_dummy::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_divine_storm_dummy::CountTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_RAID);
    }
};

// 33695 - Exorcism and Holy Wrath Damage
class spell_pal_exorcism_and_holy_wrath_damage : public AuraScript
{
    PrepareAuraScript(spell_pal_exorcism_and_holy_wrath_damage);

    void HandleEffectCalcSpellMod(AuraEffect const* aurEff, SpellModifier*& spellMod)
    {
        if (!spellMod)
        {
            spellMod = new SpellModifier(aurEff->GetBase());
            spellMod->op = SPELLMOD_DAMAGE;
            spellMod->type = SPELLMOD_FLAT;
            spellMod->spellId = GetId();
            spellMod->mask[1] = 0x200002;
        }

        spellMod->value = aurEff->GetAmount();
    }

    void Register() override
    {
        DoEffectCalcSpellMod += AuraEffectCalcSpellModFn(spell_pal_exorcism_and_holy_wrath_damage::HandleEffectCalcSpellMod, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -9799 - Eye for an Eye
class spell_pal_eye_for_an_eye : public AuraScript
{
    PrepareAuraScript(spell_pal_eye_for_an_eye);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_EYE_FOR_AN_EYE_DAMAGE });
    }

    void OnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        DamageInfo* damageInfo = eventInfo.GetDamageInfo();

        if (!damageInfo || !damageInfo->GetDamage())
        {
            return;
        }

        // return damage % to attacker but < 50% own total health
        int32 damage = std::min(CalculatePct(static_cast<int32>(damageInfo->GetDamage()), aurEff->GetAmount()), static_cast<int32>(GetTarget()->GetMaxHealth()) / 2);
        GetTarget()->CastCustomSpell(SPELL_PALADIN_EYE_FOR_AN_EYE_DAMAGE, SPELLVALUE_BASE_POINT0, damage, eventInfo.GetProcTarget(), true, nullptr, aurEff);
    }

    void Register() override
    {
        // 4.3.4: rank 1's effect is a dummy, rank 2's a proc trigger (CPP)
        OnEffectProc += AuraEffectProcFn(spell_pal_eye_for_an_eye::OnProc, EFFECT_0,
            m_scriptSpellId == SPELL_PALADIN_EYE_FOR_AN_EYE_RANK_1 ? SPELL_AURA_DUMMY : SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 54968 - Glyph of Holy Light
class spell_pal_glyph_of_holy_light : public SpellScript
{
    PrepareSpellScript(spell_pal_glyph_of_holy_light);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove(GetExplTargetUnit());

        uint32 const maxTargets = GetSpellInfo()->MaxAffectedTargets;
        if (targets.size() > maxTargets)
        {
            targets.sort(Acore::HealthPctOrderPred());
            targets.resize(maxTargets);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_glyph_of_holy_light::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
    }
};

// -85639 - Guarded by the Light
// Ported from The Cataclysm Preservation Project (spell_paladin.cpp at b061f46): rank 2 turns the
// overhealing of the paladin's heals on themselves into an absorb (88063).
class spell_pal_guarded_by_the_light : public AuraScript
{
    PrepareAuraScript(spell_pal_guarded_by_the_light);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_GUARDED_BY_THE_LIGHT, SPELL_PALADIN_GUARDED_BY_THE_LIGHT_R2 });
    }

    void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        HealInfo* heal = eventInfo.GetHealInfo();
        if (!heal)
            return;

        int32 overheal = int32(heal->GetHeal()) - int32(heal->GetEffectiveHeal());
        if (overheal > 0)
            GetTarget()->CastCustomSpell(SPELL_PALADIN_GUARDED_BY_THE_LIGHT, SPELLVALUE_BASE_POINT0, overheal, GetTarget(), true);
    }

    void Register() override
    {
        if (m_scriptSpellId == SPELL_PALADIN_GUARDED_BY_THE_LIGHT_R2) // only rank 2 has this effect
            OnEffectProc += AuraEffectProcFn(spell_pal_guarded_by_the_light::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 6940 - Hand of Sacrifice
class spell_pal_hand_of_sacrifice_aura : public AuraScript
{
    PrepareAuraScript(spell_pal_hand_of_sacrifice_aura);

    int32 remainingAmount;

    bool Load() override
    {
        if (Unit* caster = GetCaster())
        {
            remainingAmount = caster->GetMaxHealth();
            return true;
        }
        return false;
    }

    void Split(AuraEffect* /*aurEff*/, DamageInfo& /*dmgInfo*/, uint32& splitAmount)
    {
        remainingAmount -= splitAmount;

        if (remainingAmount <= 0)
        {
            GetTarget()->RemoveAura(SPELL_PALADIN_HAND_OF_SACRIFICE);
        }
    }

    void Register() override
    {
        OnEffectSplit += AuraEffectSplitFn(spell_pal_hand_of_sacrifice_aura::Split, EFFECT_0);
    }
};

class spell_pal_hand_of_sacrifice : public SpellScript
{
    PrepareSpellScript(spell_pal_hand_of_sacrifice);

    SpellCastResult CheckTarget()
    {
        Unit* target = GetExplTargetUnit();
        if (!target || target->HasAura(SPELL_PALADIN_DIVINE_SACRIFICE))
            return SPELL_FAILED_TARGET_AURASTATE;

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_pal_hand_of_sacrifice::CheckTarget);
    }
};

// 20473 - Holy Shock
// 4.3.4 has one rank: the dummy casts the heal on a friend or the damage on an enemy (CPP).
class spell_pal_holy_shock : public SpellScript
{
    PrepareSpellScript(spell_pal_holy_shock);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_HOLY_SHOCK_R1_DAMAGE, SPELL_PALADIN_HOLY_SHOCK_R1_HEALING });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (Unit* unitTarget = GetHitUnit())
        {
            if (caster->IsFriendlyTo(unitTarget))
                caster->CastSpell(unitTarget, SPELL_PALADIN_HOLY_SHOCK_R1_HEALING, true);
            else
                caster->CastSpell(unitTarget, SPELL_PALADIN_HOLY_SHOCK_R1_DAMAGE, true);
        }
    }

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();
        if (Unit* target = GetExplTargetUnit())
        {
            if (!caster->IsFriendlyTo(target))
            {
                if (!caster->IsValidAttackTarget(target))
                    return SPELL_FAILED_BAD_TARGETS;

                if (!caster->isInFront(target))
                    return SPELL_FAILED_UNIT_NOT_INFRONT;
            }
        }
        else
            return SPELL_FAILED_BAD_TARGETS;
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_pal_holy_shock::CheckCast);
        OnEffectHitTarget += SpellEffectFn(spell_pal_holy_shock::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

enum PaladinCataSpells
{
    SPELL_PALADIN_CENSURE                               = 31803,
    SPELL_PALADIN_DIVINE_PURPOSE_PROC                   = 90174,
    SPELL_PALADIN_ETERNAL_GLORY_PROC                    = 88676,
    SPELL_PALADIN_ITEM_T11_RETRIBUTION_4P_BONUS         = 90299,
    SPELL_PALADIN_JUDGEMENT_DEFAULT                     = 54158,
    SPELL_PALADIN_JUDGEMENT_OF_RIGHTEOUSNESS            = 20187,
    SPELL_PALADIN_JUDGEMENT_OF_TRUTH                    = 31804,
    SPELL_PALADIN_LONG_ARM_OF_THE_LAW                   = 87173,
    SPELL_PALADIN_SEAL_OF_RIGHTEOUSNESS_DAMAGE          = 25742,
    SPELL_PALADIN_SEAL_OF_RIGHTEOUSNESS_DAMAGE_AOE      = 101423,
    SPELL_PALADIN_SEAL_OF_JUSTICE_EFFECT                = 20170,
    SPELL_PALADIN_TWO_HANDED_WEAPON_SPECIALIZATION      = 20113
};

enum PaladinCataSpellIcons
{
    PALADIN_ICON_ID_ETERNAL_GLORY                       = 2944,
    PALADIN_ICON_ID_GLYPH_OF_THE_LONG_WORD              = 4127,
    PALADIN_ICON_ID_GUARDED_BY_THE_LIGHT                = 3026,
    PALADIN_ICON_ID_LONG_ARM_OF_THE_LAW                 = 3013,
    PALADIN_ICON_ID_SEALS_OF_COMMAND                    = 561,
    PALADIN_ICON_ID_SELFLESS_HEALER                     = 3924,
    PALADIN_ICON_ID_GLYPH_OF_DIVINITY                   = 79,
    PALADIN_ICON_ID_GLYPH_OF_LIGHT_OF_DAWN              = 5154
};

// 20271 - Judgement
// Ported from The Cataclysm Preservation Project (spell_paladin.cpp at b061f46). In 4.3.4 effect 0 is a
// dummy: the damage spell comes from the active seal's effect 2 (Judgement of Righteousness for Seal of
// Righteousness, Judgement of Truth for Seal of Truth, the default Judgement otherwise).
class spell_pal_judgement : public SpellScript
{
    PrepareSpellScript(spell_pal_judgement);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_PALADIN_JUDGEMENT_DEFAULT,
                SPELL_PALADIN_JUDGEMENT_OF_TRUTH,
                SPELL_PALADIN_JUDGEMENT_OF_RIGHTEOUSNESS,
                SPELL_PALADIN_CENSURE,
                SPELL_PALADIN_TWO_HANDED_WEAPON_SPECIALIZATION,
                SPELL_PALADIN_LONG_ARM_OF_THE_LAW
            });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        if (!target)
            return;

        uint32 spellId = SPELL_PALADIN_JUDGEMENT_DEFAULT;
        float ap = caster->GetTotalAttackPowerValue(BASE_ATTACK);
        int32 holy = caster->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_HOLY);

        // Some seals have a dummy aura effect which contains the spell id of the judgement damage spell
        for (AuraEffect const* aurEff : caster->GetAuraEffectsByType(SPELL_AURA_DUMMY))
        {
            if (aurEff->GetSpellInfo()->GetSpellSpecific() != SPELL_SPECIFIC_SEAL || aurEff->GetEffIndex() != EFFECT_2)
                continue;

            spellId = aurEff->GetAmount();
        }

        int32 bp = 0;
        switch (spellId)
        {
            case SPELL_PALADIN_JUDGEMENT_OF_TRUTH:
                bp = int32(1 + (0.223f * holy + 0.142f * ap));
                if (Aura const* censure = target->GetAura(SPELL_PALADIN_CENSURE, caster->GetGUID()))
                    AddPct(bp, censure->GetStackAmount() * 20.f);
                break;
            case SPELL_PALADIN_JUDGEMENT_OF_RIGHTEOUSNESS:
                bp = int32(1 + (0.2f * ap + 0.32f * holy));
                break;
            case SPELL_PALADIN_JUDGEMENT_DEFAULT:
                bp = int32(1 + (0.25f * holy + ap * 0.16f));
                break;
            default:
                break;
        }

        // Judgement benefits from Two-Handed Weapon Specialization (logs and community research, per CPP)
        if (AuraEffect const* twoHandedSpec = caster->GetAuraEffect(SPELL_PALADIN_TWO_HANDED_WEAPON_SPECIALIZATION, EFFECT_0))
            AddPct(bp, twoHandedSpec->GetAmount());

        caster->CastCustomSpell(spellId, SPELLVALUE_BASE_POINT0, bp, target, true);

        // Long Arm of the Law
        if (AuraEffect const* aurEff = caster->GetDummyAuraEffect(SPELLFAMILY_PALADIN, PALADIN_ICON_ID_LONG_ARM_OF_THE_LAW, EFFECT_0))
            if (roll_chance_i(aurEff->GetAmount()) && caster->GetExactDist2d(target) > 15.f)
                caster->CastSpell(caster, SPELL_PALADIN_LONG_ARM_OF_THE_LAW, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pal_judgement::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 633 - Lay on Hands
// 4.3.4 (CPP): on yourself it brings Forbearance and the immune marker (no Avenging Wrath marker any
// more); the Glyph of Divinity gives mana on every use (54986).
class spell_pal_lay_on_hands : public SpellScript
{
    PrepareSpellScript(spell_pal_lay_on_hands);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_PALADIN_FORBEARANCE,
                SPELL_PALADIN_IMMUNE_SHIELD_MARKER,
                SPELL_PALADIN_GLYPH_OF_DIVINITY_PROC
            });
    }

    void HandleMaxHealthHeal(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetExplTargetUnit();

        if (!target || !caster)
            return;

        uint32 baseHeal = caster->GetMaxHealth();
        uint32 modifiedHeal = target->SpellHealingBonusTaken(caster, GetSpellInfo(), baseHeal, HEAL);

        // EffectHealMaxHealth() ignores healing modifiers, so we pre-apply the
        // difference here; this delta will be added on top of the raw heal.
        int64 healAdjustment = int64(modifiedHeal) - int64(baseHeal);

        SetHitHeal(healAdjustment);
    }

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();
        if (Unit* target = GetExplTargetUnit())
            if (caster == target)
                if (target->HasAnyAuras(SPELL_PALADIN_FORBEARANCE, SPELL_PALADIN_IMMUNE_SHIELD_MARKER))
                    return SPELL_FAILED_TARGET_AURASTATE;

        return SPELL_CAST_OK;
    }

    void HandleScript()
    {
        Unit* caster = GetCaster();
        if (caster == GetHitUnit())
        {
            caster->CastSpell(caster, SPELL_PALADIN_FORBEARANCE, true);
            caster->CastSpell(caster, SPELL_PALADIN_IMMUNE_SHIELD_MARKER, true);
        }

        if (AuraEffect const* aurEff = caster->GetDummyAuraEffect(SPELLFAMILY_PALADIN, PALADIN_ICON_ID_GLYPH_OF_DIVINITY, EFFECT_0))
            caster->CastSpell(caster, SPELL_PALADIN_GLYPH_OF_DIVINITY_PROC, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_pal_lay_on_hands::CheckCast);
        AfterHit += SpellHitFn(spell_pal_lay_on_hands::HandleScript);
        OnEffectHitTarget += SpellEffectFn(spell_pal_lay_on_hands::HandleMaxHealthHeal, EFFECT_0, SPELL_EFFECT_HEAL_MAX_HEALTH);
    }
};

// 31789 - Righteous Defense
class spell_pal_righteous_defense : public SpellScript
{
    PrepareSpellScript(spell_pal_righteous_defense);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_RIGHTEOUS_DEFENSE_TAUNT });
    }

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();
        if (!caster->IsPlayer())
            return SPELL_FAILED_DONT_REPORT;

        if (Unit* target = GetExplTargetUnit())
        {
            if (!target->IsFriendlyTo(caster) || target->getAttackers().empty())
                return SPELL_FAILED_BAD_TARGETS;
        }
        else
            return SPELL_FAILED_BAD_TARGETS;

        return SPELL_CAST_OK;
    }

    void HandleTriggerSpellLaunch(SpellEffIndex effIndex)
    {
        PreventHitDefaultEffect(effIndex);
    }

    void HandleTriggerSpellHit(SpellEffIndex effIndex)
    {
        PreventHitDefaultEffect(effIndex);
        if (Unit* target = GetHitUnit())
            GetCaster()->CastSpell(target, SPELL_PALADIN_RIGHTEOUS_DEFENSE_TAUNT, true);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_pal_righteous_defense::CheckCast);
        //! WORKAROUND
        //! target select will be executed in hitphase of effect 0
        //! so we must handle trigger spell also in hit phase (default execution in launch phase)
        //! see issue #3718
        OnEffectLaunchTarget += SpellEffectFn(spell_pal_righteous_defense::HandleTriggerSpellLaunch, EFFECT_1, SPELL_EFFECT_TRIGGER_SPELL);
        OnEffectHitTarget += SpellEffectFn(spell_pal_righteous_defense::HandleTriggerSpellHit, EFFECT_1, SPELL_EFFECT_TRIGGER_SPELL);
    }
};

// 20154 - Seal of Righteousness - melee proc dummy (addition ${$MWS*(0.011*$AP+0.022*$SPH)} damage)
// Ported from The Cataclysm Preservation Project (spell_paladin.cpp at b061f46, updated for 4.3.4).
class spell_pal_seal_of_righteousness : public AuraScript
{
    PrepareAuraScript(spell_pal_seal_of_righteousness);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_SEAL_OF_RIGHTEOUSNESS_DAMAGE, SPELL_PALADIN_SEAL_OF_RIGHTEOUSNESS_DAMAGE_AOE });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetProcTarget() != nullptr;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* target = GetTarget();

        float ap = target->GetTotalAttackPowerValue(BASE_ATTACK);
        int32 holy = target->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_HOLY);
        // GetAttackTime is the unhasted weapon speed (CPP: GetBaseAttackTime)
        int32 bp = int32((ap * 0.011f + 0.022f * holy) * target->GetAttackTime(BASE_ATTACK) / 1000);

        // Seals of Command: the proc hits up to three nearby enemies
        if (target->GetDummyAuraEffect(SPELLFAMILY_PALADIN, PALADIN_ICON_ID_SEALS_OF_COMMAND, EFFECT_1))
            target->CastCustomSpell(SPELL_PALADIN_SEAL_OF_RIGHTEOUSNESS_DAMAGE_AOE, SPELLVALUE_BASE_POINT0, bp, target, true, nullptr, aurEff);
        else
            target->CastCustomSpell(SPELL_PALADIN_SEAL_OF_RIGHTEOUSNESS_DAMAGE, SPELLVALUE_BASE_POINT0, bp, eventInfo.GetProcTarget(), true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pal_seal_of_righteousness::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pal_seal_of_righteousness::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 20164 - Seal of Justice - single-target melee hits snare the target and deal
// ${$MWS*(0.005*$AP+0.01*$SPH)} Holy damage (20170's effect 1 is a placeholder of 1)
class spell_pal_seal_of_justice : public AuraScript
{
    PrepareAuraScript(spell_pal_seal_of_justice);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_SEAL_OF_JUSTICE_EFFECT });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetProcTarget() && (!eventInfo.GetSpellInfo() || !eventInfo.GetSpellInfo()->IsAffectingArea());
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* target = GetTarget();

        float ap = target->GetTotalAttackPowerValue(BASE_ATTACK);
        int32 holy = target->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_HOLY);
        int32 bp1 = int32((ap * 0.005f + 0.01f * holy) * target->GetAttackTime(BASE_ATTACK) / 1000);
        target->CastCustomSpell(eventInfo.GetProcTarget(), SPELL_PALADIN_SEAL_OF_JUSTICE_EFFECT, nullptr, &bp1, nullptr, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pal_seal_of_justice::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pal_seal_of_justice::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 1022 - Hand of Protection
class spell_pal_hand_of_protection : public SpellScript
{
    PrepareSpellScript(spell_pal_hand_of_protection);

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();

        if (!caster->GetTarget() || caster->GetTarget() == caster->GetGUID())
            return SPELL_CAST_OK;

        if (caster->HasStunAura())
            return SPELL_FAILED_STUNNED;

        if (caster->HasConfuseAura())
            return SPELL_FAILED_CONFUSED;

        if (caster->GetUnitFlags() & UNIT_FLAG_FLEEING)
            return SPELL_FAILED_FLEEING;

        return SPELL_CAST_OK;
    }

    // 4.3.4 (CPP): Forbearance and the server-side immune marker on the target
    void HandleForbearance(SpellEffIndex /*effIndex*/)
    {
        Unit* target = GetHitUnit();
        target->AddAura(SPELL_PALADIN_IMMUNE_SHIELD_MARKER, target);
        GetCaster()->CastSpell(target, SPELL_PALADIN_FORBEARANCE, true);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_pal_hand_of_protection::CheckCast);
        OnEffectHitTarget += SpellEffectFn(spell_pal_hand_of_protection::HandleForbearance, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// -85117 - Divine Purpose
// Ported from The Cataclysm Preservation Project (spell_paladin.cpp at b061f46): in 4.3.4 the talent
// gives a chance on holy power abilities to make the next one free (90174); the free cast itself does
// not roll again.
class spell_pal_divine_purpose : public AuraScript
{
    PrepareAuraScript(spell_pal_divine_purpose);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_DIVINE_PURPOSE_PROC });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (AuraEffect const* divineEff = GetTarget()->GetAuraEffect(SPELL_PALADIN_DIVINE_PURPOSE_PROC, EFFECT_0))
            if (divineEff->IsAffectedOnSpell(eventInfo.GetSpellInfo()))
                return false;

        if (AuraEffect const* aurEff = GetEffect(EFFECT_0))
            return roll_chance_i(aurEff->GetAmount());

        return false;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(GetTarget(), SPELL_PALADIN_DIVINE_PURPOSE_PROC, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pal_divine_purpose::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pal_divine_purpose::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 40470 - Paladin Tier 6 Trinket
class spell_pal_item_t6_trinket : public AuraScript
{
    PrepareAuraScript(spell_pal_item_t6_trinket);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_PALADIN_ENDURING_LIGHT,
                SPELL_PALADIN_ENDURING_JUDGEMENT
            });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo)
            return false;

        // Holy Light & Flash of Light
        if (spellInfo->SpellFamilyFlags[0] & 0xC0000000)
        {
            if (!roll_chance_i(15))
                return false;

            _triggeredSpellId = SPELL_PALADIN_ENDURING_LIGHT;
            return true;
        }
        // Judgements
        else if (spellInfo->SpellFamilyFlags[0] & 0x00800000)
        {
            if (!roll_chance_i(50))
                return false;

            _triggeredSpellId = SPELL_PALADIN_ENDURING_JUDGEMENT;
            return true;
        }

        return false;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        eventInfo.GetActor()->CastSpell(eventInfo.GetActionTarget(), _triggeredSpellId, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pal_item_t6_trinket::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pal_item_t6_trinket::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }

private:
    uint32 _triggeredSpellId = 0;
};

// 28789 - Holy Power (T3 6P Bonus)
class spell_pal_t3_6p_bonus : public AuraScript
{
    PrepareAuraScript(spell_pal_t3_6p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_PALADIN_HOLY_POWER_ARMOR,
                SPELL_PALADIN_HOLY_POWER_ATTACK_POWER,
                SPELL_PALADIN_HOLY_POWER_SPELL_POWER,
                SPELL_PALADIN_HOLY_POWER_MP5
            });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        uint32 spellId;
        Unit* caster = eventInfo.GetActor();
        Unit* target = eventInfo.GetActionTarget();
        if (!target)
            return;

        switch (target->getClass())
        {
            case CLASS_PALADIN:
            case CLASS_PRIEST:
            case CLASS_SHAMAN:
            case CLASS_DRUID:
                spellId = SPELL_PALADIN_HOLY_POWER_MP5;
                break;
            case CLASS_MAGE:
            case CLASS_WARLOCK:
                spellId = SPELL_PALADIN_HOLY_POWER_SPELL_POWER;
                break;
            case CLASS_HUNTER:
            case CLASS_ROGUE:
                spellId = SPELL_PALADIN_HOLY_POWER_ATTACK_POWER;
                break;
            case CLASS_WARRIOR:
                spellId = SPELL_PALADIN_HOLY_POWER_ARMOR;
                break;
            default:
                return;
        }

        caster->CastSpell(target, spellId, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pal_t3_6p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 64890 - Item - Paladin T8 Holy 2P Bonus
class spell_pal_t8_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_pal_t8_2p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_HOLY_MENDING });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        HealInfo* healInfo = eventInfo.GetHealInfo();
        return healInfo && healInfo->GetHeal();
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        Unit* caster = eventInfo.GetActor();
        Unit* target = eventInfo.GetActionTarget();
        if (!target)
            return;

        SpellInfo const* spellInfo = sSpellMgr->AssertSpellInfo(SPELL_PALADIN_HOLY_MENDING);
        int32 amount = CalculatePct(static_cast<int32>(eventInfo.GetHealInfo()->GetHeal()), aurEff->GetAmount());

        int32 maxTicks = spellInfo->GetMaxTicks();
        if (maxTicks <= 0)
            return;

        amount /= maxTicks;
        caster->CastCustomSpell(SPELL_PALADIN_HOLY_MENDING, SPELLVALUE_BASE_POINT0, amount, target, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pal_t8_2p_bonus::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pal_t8_2p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -53695 - Judgements of the Just
class spell_pal_judgements_of_the_just : public AuraScript
{
    PrepareAuraScript(spell_pal_judgements_of_the_just);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_JUDGEMENTS_OF_THE_JUST_PROC });
    }

    void OnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(eventInfo.GetActionTarget(), SPELL_PALADIN_JUDGEMENTS_OF_THE_JUST_PROC, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pal_judgements_of_the_just::OnProc, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER);
    }
};

// 63510 - Improved Concentration Aura (requires Concentration Aura on the target)
// 63514 - Improved Devotion Aura (requires Devotion Aura on the target)
class spell_pal_improved_aura_effect : public AuraScript
{
    PrepareAuraScript(spell_pal_improved_aura_effect);

public:
    spell_pal_improved_aura_effect(uint32 auraSpellId) : AuraScript(), _auraSpellId(auraSpellId) { }

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ _auraSpellId });
    }

    bool CheckAreaTarget(Unit* target)
    {
        return target->GetAuraOfRankedSpell(_auraSpellId, GetCasterGUID());
    }

    void Register() override
    {
        DoCheckAreaTarget += AuraCheckAreaTargetFn(spell_pal_improved_aura_effect::CheckAreaTarget);
    }

private:
    uint32 _auraSpellId;
};

// 63531 - Sanctified Retribution
// "Targets affected by any of your auras" - shared effect of Sanctified Retribution and Swift Retribution
class spell_pal_sanctified_retribution_effect : public AuraScript
{
    PrepareAuraScript(spell_pal_sanctified_retribution_effect);

    // 4.x merged the shadow, frost and fire resistance auras into Resistance Aura (19891)
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_PALADIN_DEVOTION_AURA_R1,
            SPELL_PALADIN_RETRIBUTION_AURA_R1,
            SPELL_PALADIN_CONCENTRACTION_AURA,
            SPELL_PALADIN_FIRE_RESISTANCE_AURA_R1,
            SPELL_PALADIN_CRUSADER_AURA
        });
    }

    bool CheckAreaTarget(Unit* target)
    {
        for (uint32 auraSpellId : { SPELL_PALADIN_DEVOTION_AURA_R1, SPELL_PALADIN_RETRIBUTION_AURA_R1, SPELL_PALADIN_CONCENTRACTION_AURA,
            SPELL_PALADIN_FIRE_RESISTANCE_AURA_R1, SPELL_PALADIN_CRUSADER_AURA })
            if (target->GetAuraOfRankedSpell(auraSpellId, GetCasterGUID()))
                return true;

        return false;
    }

    void Register() override
    {
        DoCheckAreaTarget += AuraCheckAreaTargetFn(spell_pal_sanctified_retribution_effect::CheckAreaTarget);
    }
};

// 53651 - Light's Beacon
// Ported from The Cataclysm Preservation Project (spell_paladin.cpp at b061f46): in 4.3.4 every heal
// of the paladin copies to the beacon target through one spell (53652), 50% of it (100% for Holy
// Light), instead of WotLK's copy spell per heal.
class spell_pal_lights_beacon : public AuraScript
{
    PrepareAuraScript(spell_pal_lights_beacon);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_BEACON_OF_LIGHT_AURA, SPELL_PALADIN_BEACON_OF_LIGHT_HL, SPELL_PALADIN_HOLY_LIGHT_R1 });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        // Light's Beacon is cast by the beacon target, which carries it too: a heal on the beacon target
        // itself is not copied to it (own; CPP copied it)
        return eventInfo.GetHealInfo() != nullptr && GetTarget() != GetCaster();
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        HealInfo* heal = eventInfo.GetHealInfo();
        // the share of the whole heal, overhealing included: a heal on a target at full health copies too
        // (own; CPP took the effective heal only)
        int32 bp = CalculatePct(int32(heal->GetHeal()), aurEff->GetAmount());
        if (!bp)
            return;

        if (heal->GetSpellInfo() && heal->GetSpellInfo()->Id == SPELL_PALADIN_HOLY_LIGHT_R1)
            bp *= 2;

        // a party or raid member carrying Light's Beacon (cast by the beacon target) was healed: the paladin
        // whose Beacon of Light is on that target heals it for the share (CPP)
        if (Unit* caster = GetCaster())
            if (Aura* aura = caster->GetAura(SPELL_PALADIN_BEACON_OF_LIGHT_AURA))
                if (Unit* originalCaster = aura->GetCaster())
                    originalCaster->CastCustomSpell(SPELL_PALADIN_BEACON_OF_LIGHT_HL, SPELLVALUE_BASE_POINT0, bp, caster, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pal_lights_beacon::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pal_lights_beacon::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

enum PaladinExorcism
{
    PALADIN_ICON_ID_GLYPH_OF_EXORCISM = 292
};

// 879 - Exorcism
// Ported from The Cataclysm Preservation Project (spell_paladin.cpp at b061f46): the damage gains 34.4%
// of the higher of attack power and holy spell power, and the damage over time (effect 1) only comes
// with the Glyph of Exorcism, ticking for 6.88% of the hit.
class spell_pal_exorcism : public SpellScript
{
    PrepareSpellScript(spell_pal_exorcism);

    void HandleDamage(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        int32 bonus = int32(std::max<float>(caster->GetTotalAttackPowerValue(BASE_ATTACK),
            float(caster->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_HOLY))) * 0.344f);
        SetEffectValue(GetEffectValue() + bonus);
    }

    void HandleGlyph(SpellEffIndex effIndex)
    {
        if (GetCaster()->GetDummyAuraEffect(SPELLFAMILY_PALADIN, PALADIN_ICON_ID_GLYPH_OF_EXORCISM, EFFECT_0))
            return;

        PreventHitDefaultEffect(effIndex);
        PreventHitAura();
    }

    void ApplyDotBonus()
    {
        Unit* target = GetHitUnit();
        if (!target)
            return;

        if (AuraEffect* exorcismAuraEff = target->GetAuraEffect(GetSpellInfo()->Id, EFFECT_1, GetCaster()->GetGUID()))
            exorcismAuraEff->ChangeAmount(int32(GetHitDamage() * 0.0688f));
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_pal_exorcism::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnEffectHitTarget += SpellEffectFn(spell_pal_exorcism::HandleGlyph, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
        AfterHit += SpellHitFn(spell_pal_exorcism::ApplyDotBonus);
    }
};

// Holy power spenders. Spell::TakePower takes the base cost of one charge before the effects launch and
// Spell::HandleHolyPower spends the rest once the cast is done, so GetPower(POWER_HOLY_POWER) + 1 is
// the number of charges a spender consumes (pinned TrinityCore-Cata / CPP).
static uint8 GetHolyPowerSpent(Unit const* caster)
{
    if (caster->HasAura(SPELL_PALADIN_DIVINE_PURPOSE_PROC))
        return 3;

    return uint8(std::min<uint32>(caster->GetPower(POWER_HOLY_POWER) + 1, 3));
}

// 85673 - Word of Glory
// Ported from The Cataclysm Preservation Project (spell_paladin.cpp at b061f46).
class spell_pal_word_of_glory : public SpellScript
{
    PrepareSpellScript(spell_pal_word_of_glory);

    bool Load() override
    {
        return GetCaster()->IsPlayer() && GetCaster()->getClass() == CLASS_PALADIN;
    }

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_DIVINE_PURPOSE_PROC, SPELL_PALADIN_ETERNAL_GLORY_PROC });
    }

    void HandleHeal(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        if (!target)
            return;

        int32 heal = int32(GetEffectValue() + caster->GetTotalAttackPowerValue(BASE_ATTACK) * 0.198f);
        heal *= GetHolyPowerSpent(caster);

        if (caster != target)
        {
            if (AuraEffect const* aurEff = caster->GetDummyAuraEffect(SPELLFAMILY_PALADIN, PALADIN_ICON_ID_SELFLESS_HEALER, EFFECT_0))
                AddPct(heal, aurEff->GetAmount());
        }
        else if (AuraEffect const* aurEff = caster->GetDummyAuraEffect(SPELLFAMILY_PALADIN, PALADIN_ICON_ID_GUARDED_BY_THE_LIGHT, EFFECT_0))
            AddPct(heal, aurEff->GetAmount());

        SetEffectValue(heal);
    }

    // The heal over time (effect 1) only comes with the Glyph of Long Word
    void HandleHotAura(SpellEffIndex effIndex)
    {
        if (GetCaster()->GetDummyAuraEffect(SPELLFAMILY_PALADIN, PALADIN_ICON_ID_GLYPH_OF_THE_LONG_WORD, EFFECT_1))
            return;

        PreventHitDefaultEffect(effIndex);
        PreventHitAura();
    }

    void HandleEternalGlory()
    {
        Unit* caster = GetCaster();
        if (AuraEffect const* aurEff = caster->GetDummyAuraEffect(SPELLFAMILY_PALADIN, PALADIN_ICON_ID_ETERNAL_GLORY, EFFECT_0))
        {
            if (roll_chance_i(aurEff->GetAmount()))
            {
                int32 powerCost = 1 + GetSpell()->GetPowerCost();
                caster->CastCustomSpell(SPELL_PALADIN_ETERNAL_GLORY_PROC, SPELLVALUE_BASE_POINT0, powerCost, caster, true, nullptr, aurEff);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pal_word_of_glory::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
        OnEffectHitTarget += SpellEffectFn(spell_pal_word_of_glory::HandleHotAura, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
        AfterCast += SpellCastFn(spell_pal_word_of_glory::HandleEternalGlory);
    }
};

class spell_pal_word_of_glory_aura : public AuraScript
{
    PrepareAuraScript(spell_pal_word_of_glory_aura);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_DIVINE_PURPOSE_PROC });
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        amount *= GetHolyPowerSpent(caster);

        if (caster != GetUnitOwner())
            if (AuraEffect const* aurEff = caster->GetDummyAuraEffect(SPELLFAMILY_PALADIN, PALADIN_ICON_ID_SELFLESS_HEALER, EFFECT_0))
                AddPct(amount, aurEff->GetAmount());
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_word_of_glory_aura::CalculateAmount, EFFECT_1, SPELL_AURA_PERIODIC_HEAL);
    }
};

// 85256 - Templar's Verdict
// Ported from The Cataclysm Preservation Project (spell_paladin.cpp at b061f46): 30% weapon damage
// for one charge, 90% for two (x3) and 235% for three (x7.83).
class spell_pal_templar_s_verdict : public SpellScript
{
    PrepareSpellScript(spell_pal_templar_s_verdict);

    bool Load() override
    {
        return GetCaster()->IsPlayer() && GetCaster()->getClass() == CLASS_PALADIN;
    }

    void HandleDamageBonus(SpellEffIndex /*effIndex*/)
    {
        float damageMod = 1.f;

        if (GetCaster()->HasAura(SPELL_PALADIN_DIVINE_PURPOSE_PROC))
            damageMod = 7.83334f; // 235%
        else
        {
            uint32 holyPower = GetCaster()->GetPower(POWER_HOLY_POWER);
            if (holyPower >= 1)
                damageMod = holyPower > 1 ? 7.83334f : 3.f;
        }

        SetEffectValue(int32(GetEffectValue() * damageMod));
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_pal_templar_s_verdict::HandleDamageBonus, EFFECT_0, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
    }
};

// 84963 - Inquisition
// Ported from The Cataclysm Preservation Project (spell_paladin.cpp at b061f46): the duration grows
// with every holy power charge spent.
class spell_pal_inquisition : public SpellScript
{
    PrepareSpellScript(spell_pal_inquisition);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_DIVINE_PURPOSE_PROC, SPELL_PALADIN_ITEM_T11_RETRIBUTION_4P_BONUS });
    }

    void ChangeDuration()
    {
        Unit* caster = GetCaster();
        if (Aura* aura = caster->GetAura(GetSpellInfo()->Id))
        {
            uint32 power = caster->HasAura(SPELL_PALADIN_DIVINE_PURPOSE_PROC) ? 2 : caster->GetPower(POWER_HOLY_POWER);

            // T11 bonus
            if (caster->GetAuraEffect(SPELL_PALADIN_ITEM_T11_RETRIBUTION_4P_BONUS, EFFECT_0, caster->GetGUID()))
                power = std::min<uint32>(caster->GetPower(POWER_HOLY_POWER) + 1, caster->GetMaxPower(POWER_HOLY_POWER));

            int32 duration = aura->GetDuration();
            duration += duration * int32(power);
            aura->SetDuration(duration);
        }
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_pal_inquisition::ChangeDuration);
    }
};

/*
 * Cataclysm (4.3.4) paladin scripts, ported from The Cataclysm Preservation Project (spell_paladin.cpp
 * at b061f46) to AzerothCore's script API. Bound in the world DB by scripts/build_world_db.py.
 */

enum PaladinCataScriptSpells
{
    SPELL_PALADIN_ANCIENT_HEALER                        = 86674,
    SPELL_PALADIN_ANCIENT_CRUSADER                      = 86701,
    SPELL_PALADIN_ANCIENT_CRUSADER_GUARDIAN             = 86703,
    SPELL_PALADIN_ANCIENT_FURY                          = 86704,
    SPELL_PALADIN_ANCIENT_POWER                         = 86700,
    SPELL_PALADIN_AVENGERS_SHIELD                       = 31935,
    SPELL_PALADIN_CONSECRATION_TRIGGERED                = 81297,
    SPELL_PALADIN_CRUSADER_STRIKE                       = 35395,
    SPELL_PALADIN_GUARDIAN_OF_ANCIENT_KINGS_HOLY        = 86669,
    SPELL_PALADIN_GUARDIAN_OF_ANCIENT_KINGS_PROTECTION  = 86659,
    SPELL_PALADIN_GUARDIAN_OF_ANCIENT_KINGS_RETRIBUTION = 86698,
    SPELL_PALADIN_HAND_OF_LIGHT                         = 96172,
    SPELL_PALADIN_HOLY_POWER_TALENT_MARKER              = 32733, // server-side
    SPELL_PALADIN_HOLY_RADIANCE_TRIGGERED               = 86452,
    SPELL_PALADIN_ILLUMINATED_HEALING                   = 86273,
    SPELL_PALADIN_ITEM_HEALING_TRANCE                   = 37706,
    SPELL_PALADIN_JUDGEMENTS_OF_THE_BOLD                = 89906,
    SPELL_PALADIN_JUDGEMENTS_OF_THE_WISE_PASSIVE        = 31878,
    SPELL_PALADIN_LIGHT_OF_THE_ANCIENT_KINGS            = 86678,
    SPELL_PALADIN_SPEED_OF_LIGHT                        = 85497,
    SPELL_PALADIN_TEMPLARS_VERDICT                      = 85256
};

// 26573 - Consecration
// The periodic dummy reaches every enemy in the area; only the caster's own tick casts the damage, at
// the spot Consecration was put down.
class spell_pal_consecration : public AuraScript
{
    PrepareAuraScript(spell_pal_consecration);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_CONSECRATION_TRIGGERED });
    }

    bool Load() override
    {
        if (Unit* caster = GetCaster())
            _castPos = caster->GetPosition();
        return true;
    }

    void HandleEffectPeriodic(AuraEffect const* aurEff)
    {
        Unit* caster = GetCaster();
        if (!caster || GetTarget() != caster)
            return;

        caster->CastSpell(_castPos.GetPositionX(), _castPos.GetPositionY(), _castPos.GetPositionZ(),
            SPELL_PALADIN_CONSECRATION_TRIGGERED, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_pal_consecration::HandleEffectPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }

private:
    Position _castPos;
};

// -75806 - Grand Crusader
class spell_pal_grand_crusader : public AuraScript
{
    PrepareAuraScript(spell_pal_grand_crusader);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_AVENGERS_SHIELD });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Spell const* procSpell = eventInfo.GetProcSpell();
        return procSpell && eventInfo.GetProcTarget() && eventInfo.GetProcTarget() == procSpell->m_targets.GetUnitTarget();
    }

    void HandleEffectProc(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
    {
        if (Player* player = GetTarget()->ToPlayer())
            player->RemoveSpellCooldown(SPELL_PALADIN_AVENGERS_SHIELD, true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pal_grand_crusader::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pal_grand_crusader::HandleEffectProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 37705 - Healing Discount
class spell_pal_item_healing_discount : public AuraScript
{
    PrepareAuraScript(spell_pal_item_healing_discount);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_ITEM_HEALING_TRANCE });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(GetTarget(), SPELL_PALADIN_ITEM_HEALING_TRANCE, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pal_item_healing_discount::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 53600 - Shield of the Righteous
// The holy power multiplies the final damage: x1 for one charge, x3 for two, x6 for three. One charge
// is taken before the hit, so the power left is 0, 1 or 2.
class spell_pal_shield_of_the_righteous : public SpellScript
{
    PrepareSpellScript(spell_pal_shield_of_the_righteous);

    bool Load() override
    {
        return GetCaster()->IsPlayer() && GetCaster()->getClass() == CLASS_PALADIN;
    }

    void ChangeDamage(SpellEffIndex /*effIndex*/)
    {
        uint32 holyPower = GetCaster()->GetPower(POWER_HOLY_POWER);
        float damageMod = 1.f;
        if (holyPower >= 1)
            damageMod = holyPower > 1 ? 6.f : 3.f;

        SetHitDamage(int32(GetHitDamage() * damageMod));
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pal_shield_of_the_righteous::ChangeDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 76669 - Illuminated Healing (holy mastery)
// Heals leave an absorb of the mastery's share of the effective heal, stacking up to a third of the
// paladin's maximum health.
class spell_pal_illuminated_healing : public AuraScript
{
    PrepareAuraScript(spell_pal_illuminated_healing);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_ILLUMINATED_HEALING });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* caster = GetCaster();
        Unit* target = eventInfo.GetProcTarget();
        HealInfo* heal = eventInfo.GetHealInfo();
        if (!caster || !target || !heal)
            return;

        int32 shieldAmount = CalculatePct(int32(heal->GetEffectiveHeal()), aurEff->GetAmount());
        if (Aura* aura = target->GetAura(SPELL_PALADIN_ILLUMINATED_HEALING, caster->GetGUID()))
        {
            if (AuraEffect* eff = aura->GetEffect(EFFECT_0))
                eff->SetAmount(std::min<int32>(CalculatePct(int32(caster->GetMaxHealth()), 33), eff->GetAmount() + shieldAmount));

            aura->RefreshDuration();
        }
        else
            caster->CastCustomSpell(SPELL_PALADIN_ILLUMINATED_HEALING, SPELLVALUE_BASE_POINT0, shieldAmount, target, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pal_illuminated_healing::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 76672 - Hand of Light (retribution mastery)
// Crusader Strike, Templar's Verdict and Divine Storm deal the mastery's share again as holy damage.
class spell_pal_hand_of_light : public AuraScript
{
    PrepareAuraScript(spell_pal_hand_of_light);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_HAND_OF_LIGHT, SPELL_PALADIN_TEMPLARS_VERDICT, SPELL_PALADIN_CRUSADER_STRIKE,
            SPELL_PALADIN_DIVINE_STORM });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        return spellInfo && eventInfo.GetDamageInfo() && (spellInfo->Id == SPELL_PALADIN_TEMPLARS_VERDICT ||
            spellInfo->Id == SPELL_PALADIN_CRUSADER_STRIKE || spellInfo->Id == SPELL_PALADIN_DIVINE_STORM);
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* caster = GetCaster();
        Unit* target = eventInfo.GetProcTarget();
        if (!caster || !target)
            return;

        int32 damage = CalculatePct(int32(eventInfo.GetDamageInfo()->GetDamage()), aurEff->GetAmount());
        caster->CastCustomSpell(SPELL_PALADIN_HAND_OF_LIGHT, SPELLVALUE_BASE_POINT0, damage, target, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pal_hand_of_light::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pal_hand_of_light::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 31878 - Judgements of the Wise (Passive), 89901 - Judgements of the Bold (Passive)
// A judgement restores base mana over time: the effect's percentage of base mana, spread over its ticks.
class spell_pal_judgements : public AuraScript
{
    PrepareAuraScript(spell_pal_judgements);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_JUDGEMENTS_OF_THE_WISE_MANA, SPELL_PALADIN_JUDGEMENTS_OF_THE_BOLD });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        uint32 spellId = GetSpellInfo()->Id == SPELL_PALADIN_JUDGEMENTS_OF_THE_WISE_PASSIVE ?
            SPELL_PALADIN_JUDGEMENTS_OF_THE_WISE_MANA : SPELL_PALADIN_JUDGEMENTS_OF_THE_BOLD;

        Unit* actor = eventInfo.GetActor();
        SpellInfo const* spell = sSpellMgr->GetSpellInfo(spellId);
        if (!actor || !spell)
            return;

        int32 bp = 0;
        if (uint32 mana = actor->GetCreateMana())
            bp = int32(CalculatePct(CalculatePct(mana, spell->Effects[EFFECT_0].BasePoints), spell->GetMaxTicks()));

        actor->CastCustomSpell(spellId, SPELLVALUE_BASE_POINT0, bp, actor, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pal_judgements::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 31801 - Seal of Truth
// Melee hits stack Censure (a holy damage over time from attack and spell power); at five stacks they
// also deal the seal's damage.
class spell_pal_seal_of_truth : public AuraScript
{
    PrepareAuraScript(spell_pal_seal_of_truth);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_CENSURE, SPELL_PALADIN_SEAL_OF_VENGEANCE_EFFECT });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* caster = GetTarget();
        Unit* target = eventInfo.GetProcTarget();
        if (!target)
            return;

        if (Aura* aura = target->GetAura(SPELL_PALADIN_CENSURE, caster->GetGUID()))
            if (aura->GetStackAmount() == aura->GetSpellInfo()->StackAmount)
                caster->CastSpell(target, SPELL_PALADIN_SEAL_OF_VENGEANCE_EFFECT, true, nullptr, aurEff);

        float ap = caster->GetTotalAttackPowerValue(BASE_ATTACK) * 0.0270f;
        float holy = caster->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_HOLY) * 0.01f;
        caster->CastCustomSpell(SPELL_PALADIN_CENSURE, SPELLVALUE_BASE_POINT0, int32(ap + holy), target, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pal_seal_of_truth::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 31876 - Communion
class spell_pal_communion : public AuraScript
{
    PrepareAuraScript(spell_pal_communion);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_REPLENISHMENT });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(GetTarget(), SPELL_PALADIN_REPLENISHMENT, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pal_communion::HandleProc, EFFECT_1, SPELL_AURA_ADD_FLAT_MODIFIER);
    }
};

// -85803 - Selfless Healer
// Healing someone else raises the paladin's damage, more for each holy power charge.
class spell_pal_selfless_healer : public AuraScript
{
    PrepareAuraScript(spell_pal_selfless_healer);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->Effects[EFFECT_1].TriggerSpell });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        HealInfo* heal = eventInfo.GetHealInfo();
        return heal && heal->GetTarget() != GetTarget();
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        Unit* target = GetTarget();
        uint32 spellId = GetSpellInfo()->Effects[aurEff->GetEffIndex()].TriggerSpell;
        int32 bp = aurEff->GetAmount() * int32(1 + target->GetPower(POWER_HOLY_POWER));
        target->CastCustomSpell(spellId, SPELLVALUE_BASE_POINT0, bp, target, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pal_selfless_healer::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pal_selfless_healer::HandleProc, EFFECT_1, SPELL_AURA_DUMMY);
    }
};

// 86150 - Guardian of Ancient Kings
// Summons the guardian of the paladin's specialization (holy, protection, retribution) and its aura.
class spell_pal_guardian_of_ancient_kings : public SpellScript
{
    PrepareSpellScript(spell_pal_guardian_of_ancient_kings);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_GUARDIAN_OF_ANCIENT_KINGS_HOLY, SPELL_PALADIN_GUARDIAN_OF_ANCIENT_KINGS_PROTECTION,
            SPELL_PALADIN_GUARDIAN_OF_ANCIENT_KINGS_RETRIBUTION, SPELL_PALADIN_ANCIENT_HEALER, SPELL_PALADIN_ANCIENT_CRUSADER });
    }

    void HandleDummyEffect()
    {
        Player* caster = GetCaster()->ToPlayer();
        if (!caster)
            return;

        switch (caster->GetPrimaryTalentTree(caster->GetActiveSpec()))
        {
            case TALENT_TREE_PALADIN_HOLY:
                caster->CastSpell(caster, SPELL_PALADIN_GUARDIAN_OF_ANCIENT_KINGS_HOLY, true);
                caster->CastSpell(caster, SPELL_PALADIN_ANCIENT_HEALER, true);
                break;
            case TALENT_TREE_PALADIN_PROTECTION:
                caster->CastSpell(caster, SPELL_PALADIN_GUARDIAN_OF_ANCIENT_KINGS_PROTECTION, true);
                break;
            case TALENT_TREE_PALADIN_RETRIBUTION:
                caster->CastSpell(caster, SPELL_PALADIN_GUARDIAN_OF_ANCIENT_KINGS_RETRIBUTION, true);
                caster->CastSpell(caster, SPELL_PALADIN_ANCIENT_CRUSADER, true);
                break;
            default:
                break;
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_pal_guardian_of_ancient_kings::HandleDummyEffect);
    }
};

// 86674 - Ancient Healer
// The holy guardian repeats the paladin's next five heals (Light of the Ancient Kings: the heal, and a
// tenth of it to the group around the target).
class spell_pal_ancient_healer : public AuraScript
{
    PrepareAuraScript(spell_pal_ancient_healer);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_LIGHT_OF_THE_ANCIENT_KINGS, SPELL_PALADIN_GUARDIAN_OF_ANCIENT_KINGS_HOLY });
    }

    bool CheckProc(ProcEventInfo& /*eventInfo*/)
    {
        return _procCount < 5;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        HealInfo* heal = eventInfo.GetHealInfo();
        if (!heal)
            return;

        int32 bp0 = int32(heal->GetEffectiveHeal());
        int32 bp1 = CalculatePct(bp0, 10);

        for (Unit* guardian : GetTarget()->m_Controlled)
            if (guardian->GetUInt32Value(UNIT_CREATED_BY_SPELL) == SPELL_PALADIN_GUARDIAN_OF_ANCIENT_KINGS_HOLY)
                guardian->CastCustomSpell(heal->GetTarget(), SPELL_PALADIN_LIGHT_OF_THE_ANCIENT_KINGS, &bp0, &bp1, nullptr, true, nullptr, aurEff);

        ++_procCount;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pal_ancient_healer::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pal_ancient_healer::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }

private:
    uint8 _procCount = 0;
};

// 86701 - Ancient Crusader (paladin), 86703 - Ancient Crusader (guardian)
// Every hit of the paladin or the retribution guardian adds a stack of Ancient Power; when the guardian
// leaves, the stacks explode as Ancient Fury.
class spell_pal_ancient_crusader : public AuraScript
{
    PrepareAuraScript(spell_pal_ancient_crusader);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_ANCIENT_POWER, SPELL_PALADIN_ANCIENT_CRUSADER_GUARDIAN, SPELL_PALADIN_ANCIENT_FURY });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        if (m_scriptSpellId == SPELL_PALADIN_ANCIENT_CRUSADER_GUARDIAN)
        {
            if (TempSummon* summon = GetTarget()->ToTempSummon())
                if (Unit* summoner = summon->GetSummonerUnit())
                    summoner->CastSpell(summoner, SPELL_PALADIN_ANCIENT_POWER, true, nullptr, aurEff);
        }
        else
            GetTarget()->CastSpell(GetTarget(), SPELL_PALADIN_ANCIENT_POWER, true, nullptr, aurEff);
    }

    void AfterRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (m_scriptSpellId != SPELL_PALADIN_ANCIENT_CRUSADER)
            return;

        Unit* target = GetTarget();
        if (Aura* aura = target->GetAura(SPELL_PALADIN_ANCIENT_POWER))
        {
            target->CastSpell(target, SPELL_PALADIN_ANCIENT_FURY, true, nullptr, aurEff);
            aura->Remove();
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pal_ancient_crusader::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
        AfterEffectRemove += AuraEffectRemoveFn(spell_pal_ancient_crusader::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 86704 - Ancient Fury
// The damage grows with the Ancient Power stacks and is split between the targets.
class spell_pal_ancient_fury : public SpellScript
{
    PrepareSpellScript(spell_pal_ancient_fury);

    void CountTargets(std::list<WorldObject*>& targets)
    {
        _targetCount = uint32(targets.size());
    }

    void CalculateDamage()
    {
        Unit* caster = GetCaster();
        if (!_targetCount || !caster)
            return;

        if (Aura* powerAura = caster->GetAura(SPELL_PALADIN_ANCIENT_POWER))
            SetHitDamage(int32(GetHitDamage() * powerAura->GetStackAmount() / _targetCount));
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_ancient_fury::CountTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnHit += SpellHitFn(spell_pal_ancient_fury::CalculateDamage);
    }

private:
    uint32 _targetCount = 0;
};

// 85222 - Light of Dawn
// Heals the four (six with the glyph) most injured allies in front, more for each holy power charge;
// effect 1 is not used.
class spell_pal_light_of_dawn : public SpellScript
{
    PrepareSpellScript(spell_pal_light_of_dawn);

    bool Load() override
    {
        return GetCaster()->IsPlayer() && GetCaster()->getClass() == CLASS_PALADIN;
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        uint32 const maxTargets = GetCaster()->GetDummyAuraEffect(SPELLFAMILY_PALADIN, PALADIN_ICON_ID_GLYPH_OF_LIGHT_OF_DAWN, EFFECT_0) ? 6 : 4;
        if (targets.size() > maxTargets)
        {
            targets.sort(Acore::HealthPctOrderPred());
            targets.resize(maxTargets);
        }
    }

    void HandleHeal(SpellEffIndex /*effIndex*/)
    {
        int32 heal = GetHitHeal();
        heal += heal * int32(GetCaster()->GetPower(POWER_HOLY_POWER));
        SetHitHeal(heal);
    }

    void BlockHeal(SpellEffIndex effIndex)
    {
        PreventHitDefaultEffect(effIndex);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pal_light_of_dawn::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
        OnEffectLaunchTarget += SpellEffectFn(spell_pal_light_of_dawn::BlockHeal, EFFECT_1, SPELL_EFFECT_HEAL);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_light_of_dawn::FilterTargets, EFFECT_0, TARGET_UNIT_CONE_ALLY);
    }
};

// 82327 - Holy Radiance
// The heal is split when it reaches more than six allies; the aura repeats it every tick.
class spell_pal_holy_radiance : public SpellScript
{
    PrepareSpellScript(spell_pal_holy_radiance);

    bool Load() override
    {
        return GetCaster()->IsPlayer() && GetCaster()->getClass() == CLASS_PALADIN;
    }

    void CountTargets(std::list<WorldObject*>& targets)
    {
        _targetCount = uint32(targets.size());
    }

    void HandleHeal(SpellEffIndex /*effIndex*/)
    {
        if (int32 heal = GetHitHeal())
            if (_targetCount > 6)
                SetHitHeal(heal * 6 / int32(_targetCount));
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pal_holy_radiance::HandleHeal, EFFECT_1, SPELL_EFFECT_HEAL);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_holy_radiance::CountTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ALLY);
    }

private:
    uint32 _targetCount = 0;
};

class spell_pal_holy_radiance_aura : public AuraScript
{
    PrepareAuraScript(spell_pal_holy_radiance_aura);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_HOLY_RADIANCE_TRIGGERED });
    }

    void HandleEffectPeriodic(AuraEffect const* aurEff)
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(GetTarget(), SPELL_PALADIN_HOLY_RADIANCE_TRIGGERED, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_pal_holy_radiance_aura::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 642 - Divine Shield
// Forbearance and the server-side immune marker on the target.
class spell_pal_divine_shield : public SpellScript
{
    PrepareSpellScript(spell_pal_divine_shield);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_FORBEARANCE, SPELL_PALADIN_IMMUNE_SHIELD_MARKER });
    }

    void HandleForbearance(SpellEffIndex /*effIndex*/)
    {
        Unit* target = GetHitUnit();
        target->AddAura(SPELL_PALADIN_IMMUNE_SHIELD_MARKER, target);
        GetCaster()->CastSpell(target, SPELL_PALADIN_FORBEARANCE, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pal_divine_shield::HandleForbearance, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// -20138 - Protector of the Innocent
// Procs only on heals of someone else.
class spell_pal_protector_of_the_innocent : public AuraScript
{
    PrepareAuraScript(spell_pal_protector_of_the_innocent);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetHealInfo() && eventInfo.GetProcTarget() != GetTarget();
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pal_protector_of_the_innocent::CheckProc);
    }
};

// -84800 - Tower of Radiance
// Procs only on heals of the paladin's own Beacon of Light target.
class spell_pal_tower_of_radiance : public AuraScript
{
    PrepareAuraScript(spell_pal_tower_of_radiance);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_BEACON_OF_LIGHT_AURA });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetProcTarget() && eventInfo.GetProcTarget()->HasAura(SPELL_PALADIN_BEACON_OF_LIGHT_AURA, GetTarget()->GetGUID());
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pal_tower_of_radiance::CheckProc);
    }
};

// 20165 - Seal of Insight
// Single-target hits heal (15% of attack power and holy spell power) and restore 4% of base mana.
class spell_pal_seal_of_insight : public AuraScript
{
    PrepareAuraScript(spell_pal_seal_of_insight);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return !eventInfo.GetSpellInfo() || !eventInfo.GetSpellInfo()->IsAffectingArea();
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        Unit* target = GetTarget();
        int32 bp0 = int32(CalculatePct(target->GetTotalAttackPowerValue(BASE_ATTACK), 15.f) +
            CalculatePct(float(target->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_HOLY)), 15.f));
        int32 bp1 = int32(CalculatePct(float(target->GetCreateMana()), 4.f));
        uint32 spellId = GetSpellInfo()->Effects[aurEff->GetEffIndex()].TriggerSpell;
        target->CastCustomSpell(target, spellId, &bp0, &bp1, nullptr, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pal_seal_of_insight::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pal_seal_of_insight::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 20066 - Repentance
// Damage breaks it, except Censure's.
class spell_pal_repentance : public AuraScript
{
    PrepareAuraScript(spell_pal_repentance);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo)
            return false;

        return !(spellInfo->SpellFamilyName == SPELLFAMILY_PALADIN && (spellInfo->SpellFamilyFlags[0] & 0x20000000));
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pal_repentance::CheckProc);
    }
};

// -85495 - Speed of Light
class spell_pal_speed_of_light : public AuraScript
{
    PrepareAuraScript(spell_pal_speed_of_light);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_SPEED_OF_LIGHT });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastCustomSpell(SPELL_PALADIN_SPEED_OF_LIGHT, SPELLVALUE_BASE_POINT0, aurEff->GetAmount(), GetTarget(), true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pal_speed_of_light::HandleProc, EFFECT_1, SPELL_AURA_DUMMY);
    }
};

// -31828 - Blessed Life
// At most one holy power charge from it while the server-side cooldown marker is up.
class spell_pal_blessed_life : public AuraScript
{
    PrepareAuraScript(spell_pal_blessed_life);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_HOLY_POWER_TALENT_MARKER });
    }

    bool CheckProcCooldown(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
    {
        return !GetTarget()->HasAura(SPELL_PALADIN_HOLY_POWER_TALENT_MARKER, GetCasterGUID());
    }

    void Register() override
    {
        DoCheckEffectProc += AuraCheckEffectProcFn(spell_pal_blessed_life::CheckProcCooldown, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

void AddSC_paladin_spell_scripts()
{
    RegisterSpellScript(spell_pal_divine_purpose);
    RegisterSpellScript(spell_pal_sacred_shield);
    RegisterSpellScript(spell_pal_ardent_defender);
    RegisterSpellScript(spell_pal_aura_mastery);
    RegisterSpellScript(spell_pal_aura_mastery_immune);
    RegisterSpellScript(spell_pal_beacon_of_light);
    RegisterSpellScript(spell_pal_avenging_wrath);
    RegisterSpellScript(spell_pal_blessing_of_faith);
    RegisterSpellScript(spell_pal_divine_sacrifice);
    RegisterSpellScript(spell_pal_divine_storm);
    RegisterSpellScript(spell_pal_divine_storm_dummy);
    RegisterSpellScript(spell_pal_exorcism_and_holy_wrath_damage);
    RegisterSpellScript(spell_pal_eye_for_an_eye);
    RegisterSpellScript(spell_pal_glyph_of_holy_light);
    RegisterSpellScript(spell_pal_guarded_by_the_light);
    RegisterSpellAndAuraScriptPair(spell_pal_hand_of_sacrifice, spell_pal_hand_of_sacrifice_aura);
    RegisterSpellScript(spell_pal_holy_shock);
    RegisterSpellScript(spell_pal_item_t6_trinket);
    RegisterSpellScript(spell_pal_judgement);
    RegisterSpellScript(spell_pal_lay_on_hands);
    RegisterSpellScript(spell_pal_righteous_defense);
    RegisterSpellScript(spell_pal_seal_of_justice);
    RegisterSpellScript(spell_pal_seal_of_righteousness);
    RegisterSpellAndAuraScriptPair(spell_pal_word_of_glory, spell_pal_word_of_glory_aura);
    RegisterSpellScript(spell_pal_templar_s_verdict);
    RegisterSpellScript(spell_pal_inquisition);
    RegisterSpellScript(spell_pal_exorcism);
    RegisterSpellScript(spell_pal_hand_of_protection);
    RegisterSpellScript(spell_pal_t3_6p_bonus);
    RegisterSpellScript(spell_pal_t8_2p_bonus);
    RegisterSpellScript(spell_pal_judgements_of_the_just);
    RegisterSpellScriptWithArgs(spell_pal_improved_aura_effect, "spell_pal_improved_concentraction_aura_effect", SPELL_PALADIN_CONCENTRACTION_AURA);
    RegisterSpellScriptWithArgs(spell_pal_improved_aura_effect, "spell_pal_improved_devotion_aura_effect", SPELL_PALADIN_DEVOTION_AURA_R1);
    RegisterSpellScript(spell_pal_sanctified_retribution_effect);
    RegisterSpellScript(spell_pal_lights_beacon);
    // Cataclysm (4.3.4), from CPP
    RegisterSpellScript(spell_pal_consecration);
    RegisterSpellScript(spell_pal_grand_crusader);
    RegisterSpellScript(spell_pal_item_healing_discount);
    RegisterSpellScript(spell_pal_shield_of_the_righteous);
    RegisterSpellScript(spell_pal_illuminated_healing);
    RegisterSpellScript(spell_pal_hand_of_light);
    RegisterSpellScript(spell_pal_judgements);
    RegisterSpellScript(spell_pal_seal_of_truth);
    RegisterSpellScript(spell_pal_communion);
    RegisterSpellScript(spell_pal_selfless_healer);
    RegisterSpellScript(spell_pal_guardian_of_ancient_kings);
    RegisterSpellScript(spell_pal_ancient_healer);
    RegisterSpellScript(spell_pal_ancient_crusader);
    RegisterSpellScript(spell_pal_ancient_fury);
    RegisterSpellScript(spell_pal_light_of_dawn);
    RegisterSpellAndAuraScriptPair(spell_pal_holy_radiance, spell_pal_holy_radiance_aura);
    RegisterSpellScript(spell_pal_divine_shield);
    RegisterSpellScript(spell_pal_protector_of_the_innocent);
    RegisterSpellScript(spell_pal_tower_of_radiance);
    RegisterSpellScript(spell_pal_seal_of_insight);
    RegisterSpellScript(spell_pal_repentance);
    RegisterSpellScript(spell_pal_speed_of_light);
    RegisterSpellScript(spell_pal_blessed_life);
}
