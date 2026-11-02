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
#include "Spell.h"
#include "SpellAuraEffects.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "SpellScriptLoader.h"
/*
 * Scripts for spells with SPELLFAMILY_WARRIOR and SPELLFAMILY_GENERIC spells used by warrior players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_warr_".
 *
 * 4.3.4: most scripts are ported from The Cataclysm Preservation Project (spell_warrior.cpp at b061f46)
 * and rewritten against AzerothCore; the bindings come from TDB 434 (scripts/build_world_db.py).
 */

enum WarriorSpells
{
    SPELL_WARRIOR_BATTLE_TRANCE_TRIGGERED           = 12964,
    SPELL_WARRIOR_BLADESTORM_PERIODIC_WHIRLWIND     = 50622,
    SPELL_WARRIOR_BLOOD_AND_THUNDER_R1              = 84614,
    SPELL_WARRIOR_BLOODSURGE_R1                     = 46913,
    SPELL_WARRIOR_BLOODSURGE_TRIGGERED              = 46916,
    SPELL_WARRIOR_BLOODTHIRST                       = 23885,
    SPELL_WARRIOR_BLOODTHIRST_DAMAGE                = 23881,
    SPELL_WARRIOR_CHARGE_ENERGIZE                   = 34846,
    SPELL_WARRIOR_CHARGE_STUN                       = 7922,
    SPELL_WARRIOR_COLOSSUS_SMASH                    = 86346,
    SPELL_WARRIOR_DEEP_WOUNDS_PERIODIC              = 12721,
    SPELL_WARRIOR_EXECUTE                           = 5308,
    SPELL_WARRIOR_GLYPH_OF_COLOSSUS_SMASH           = 89003,
    SPELL_WARRIOR_IMPENDING_VICTORY_R1              = 80128,
    SPELL_WARRIOR_INTERCEPT                         = 20252,
    SPELL_WARRIOR_INTERVENE_TRIGGER                 = 59667,
    SPELL_WARRIOR_JUGGERNAUT_CRIT_BONUS_BUFF        = 65156,
    SPELL_WARRIOR_JUGGERNAUT_CRIT_BONUS_TALENT      = 64976,
    SPELL_WARRIOR_LAST_STAND_TRIGGERED              = 12976,
    SPELL_WARRIOR_MORTAL_STRIKE                     = 12294,
    SPELL_WARRIOR_OPPORTUNITY_STRIKE                = 76858,
    SPELL_WARRIOR_RALLYING_CRY                      = 97463,
    SPELL_WARRIOR_REND                              = 94009,
    SPELL_WARRIOR_RETALIATION_DAMAGE                = 20240,
    SPELL_WARRIOR_SECOND_WIND_PROC_R1               = 29834,
    SPELL_WARRIOR_SECOND_WIND_PROC_R2               = 29838,
    SPELL_WARRIOR_SECOND_WIND_TRIGGER_R1            = 29841,
    SPELL_WARRIOR_SECOND_WIND_TRIGGER_R2            = 29842,
    SPELL_WARRIOR_SHIELD_SLAM                       = 23922,
    SPELL_WARRIOR_SINGLE_MINDED_FURY                = 81099,
    SPELL_WARRIOR_SLAM_MAIN_HAND                    = 50782,
    SPELL_WARRIOR_SLAM_OFF_HAND                     = 97992,
    SPELL_WARRIOR_SUDDEN_DEATH_R1                   = 29723,
    SPELL_WARRIOR_SUNDER_ARMOR                      = 58567,
    SPELL_WARRIOR_SWEEPING_STRIKES_EXTRA_ATTACK_1   = 12723,
    SPELL_WARRIOR_SWEEPING_STRIKES_EXTRA_ATTACK_2   = 26654,
    SPELL_WARRIOR_T10_PROT_4P_ABSORB                = 70845,
    SPELL_WARRIOR_TAUNT                             = 355,
    SPELL_WARRIOR_VENGEANCE                         = 76691,
    SPELL_WARRIOR_VICTORIOUS                        = 32216,
    SPELL_WARRIOR_VICTORIOUS_IMPENDING_VICTORY      = 82368,
    SPELL_WARRIOR_VIGILANCE_PROC                    = 50725,
    SPELL_WARRIOR_WARRIORS_WRATH                    = 21887,
    SPELL_WARRIOR_WHIRLWIND_OFF_HAND                = 44949
};

// spell_generic.cpp: Vengeance for the tank as if it had taken this damage itself
void AddVengeanceDamage(Unit* tank, uint32 damage);

// 16488, 16490, 16491 - Blood Craze
// Heals "${$m1/2}%" of the maximum health over the whole duration. The data's aura heals $m1 whole percent on
// every tick (ten times the tooltip), so each tick heals its share here. CPP hooked a periodic heal aura,
// which these are not, so its script never ran.
class spell_warr_blood_craze : public AuraScript
{
    PrepareAuraScript(spell_warr_blood_craze);

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        PreventDefaultAction();

        Unit* target = GetTarget();
        float const pct = GetSpellInfo()->Effects[EFFECT_0].BasePoints / 2.0f / std::max(aurEff->GetTotalTicks(), 1);
        HealInfo healInfo(target, target, uint32(CalculatePct(target->GetMaxHealth(), pct)), GetSpellInfo(), GetSpellInfo()->GetSchoolMask());
        target->HealBySpell(healInfo);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warr_blood_craze::HandlePeriodic, EFFECT_0, SPELL_AURA_OBS_MOD_HEALTH);
    }
};

// 23881 - Bloodthirst
class spell_warr_bloodthirst : public SpellScript
{
    PrepareSpellScript(spell_warr_bloodthirst);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARRIOR_BLOODTHIRST });
    }

    // ${$AP*$m1/100}
    void CalculateDamage(SpellEffIndex /*effIndex*/)
    {
        SetEffectValue(int32(CalculatePct(GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK), GetEffectValue())));
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_WARRIOR_BLOODTHIRST, true);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_warr_bloodthirst::CalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnEffectHit += SpellEffectFn(spell_warr_bloodthirst::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
    }
};

// 23880 - Bloodthirst (heal)
// Bloodthirst's second effect is the heal in thousandths of a percent of the maximum health.
class spell_warr_bloodthirst_heal : public SpellScript
{
    PrepareSpellScript(spell_warr_bloodthirst_heal);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARRIOR_BLOODTHIRST_DAMAGE });
    }

    void HandleHeal(SpellEffIndex /*effIndex*/)
    {
        SpellInfo const* spellInfo = sSpellMgr->AssertSpellInfo(SPELL_WARRIOR_BLOODTHIRST_DAMAGE);
        float const pct = spellInfo->Effects[EFFECT_1].CalcValue(GetCaster()) / 1000.0f;
        SetEffectValue(int32(CalculatePct(GetCaster()->GetMaxHealth(), pct)));
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_warr_bloodthirst_heal::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
    }
};

// 100 - Charge
class spell_warr_charge : public SpellScript
{
    PrepareSpellScript(spell_warr_charge);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_WARRIOR_JUGGERNAUT_CRIT_BONUS_TALENT,
                SPELL_WARRIOR_JUGGERNAUT_CRIT_BONUS_BUFF,
                SPELL_WARRIOR_CHARGE_ENERGIZE,
                SPELL_WARRIOR_CHARGE_STUN
            });
    }

    void HandleStun(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_WARRIOR_CHARGE_STUN, true);
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        int32 rage = GetEffectValue();
        caster->CastCustomSpell(caster, SPELL_WARRIOR_CHARGE_ENERGIZE, &rage, nullptr, nullptr, true);

        // Juggernaut crit bonus
        if (caster->HasAura(SPELL_WARRIOR_JUGGERNAUT_CRIT_BONUS_TALENT))
            caster->CastSpell(caster, SPELL_WARRIOR_JUGGERNAUT_CRIT_BONUS_BUFF, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warr_charge::HandleStun, EFFECT_0, SPELL_EFFECT_CHARGE);
        OnEffectHitTarget += SpellEffectFn(spell_warr_charge::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
    }
};

// 86346 - Colossus Smash
class spell_warr_colossus_smash : public SpellScript
{
    PrepareSpellScript(spell_warr_colossus_smash);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARRIOR_GLYPH_OF_COLOSSUS_SMASH, SPELL_WARRIOR_SUNDER_ARMOR });
    }

    void HandleGlyphEffect(SpellEffIndex /*effIndex*/)
    {
        if (AuraEffect const* glyph = GetCaster()->GetAuraEffect(SPELL_WARRIOR_GLYPH_OF_COLOSSUS_SMASH, EFFECT_0))
            GetCaster()->CastSpell(GetHitUnit(), SPELL_WARRIOR_SUNDER_ARMOR, true, nullptr, glyph);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warr_colossus_smash::HandleGlyphEffect, EFFECT_0, SPELL_EFFECT_NORMALIZED_WEAPON_DMG);
    }
};

// 12809 - Concussion Blow
class spell_warr_concussion_blow : public SpellScript
{
    PrepareSpellScript(spell_warr_concussion_blow);

    // ${$m3/100*$AP}
    void CalculateDamage(SpellEffIndex /*effIndex*/)
    {
        int32 const pct = GetSpellInfo()->Effects[EFFECT_2].CalcValue(GetCaster());
        SetEffectValue(int32(CalculatePct(GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK), pct)));
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_warr_concussion_blow::CalculateDamage, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// -12834 - Deep Wounds
// Critical strikes make the target bleed for 16% per rank of the weapon's average damage over the bleed's
// duration (the hand that struck; the character sheet's weapon damage, attack power included).
class spell_warr_deep_wounds : public AuraScript
{
    PrepareAuraScript(spell_warr_deep_wounds);

    static constexpr float DAMAGE_PCT_PER_RANK = 16.0f;

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARRIOR_DEEP_WOUNDS_PERIODIC });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        Unit* warrior = GetTarget();
        Unit* target = eventInfo.GetActionTarget();
        if (!warrior->IsPlayer() || !target)
            return;

        float average;
        if (eventInfo.GetTypeMask() & PROC_FLAG_DONE_OFFHAND_ATTACK)
            average = (warrior->GetFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE) + warrior->GetFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE)) / 2.0f;
        else
            average = (warrior->GetFloatValue(UNIT_FIELD_MINDAMAGE) + warrior->GetFloatValue(UNIT_FIELD_MAXDAMAGE)) / 2.0f;

        int32 damage = int32(CalculatePct(average, DAMAGE_PCT_PER_RANK * GetSpellInfo()->GetRank()));
        if (uint32 ticks = sSpellMgr->AssertSpellInfo(SPELL_WARRIOR_DEEP_WOUNDS_PERIODIC)->GetMaxTicks())
            damage /= int32(ticks);

        if (damage > 0)
            warrior->CastCustomSpell(target, SPELL_WARRIOR_DEEP_WOUNDS_PERIODIC, &damage, nullptr, nullptr, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_warr_deep_wounds::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 20243 - Devastate
// The damage per Sunder Armor stack is added by Spell::EffectWeaponDmg (weapon effects are summed there).
class spell_warr_devastate : public SpellScript
{
    PrepareSpellScript(spell_warr_devastate);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARRIOR_SUNDER_ARMOR });
    }

    void HandleSunderArmor(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_WARRIOR_SUNDER_ARMOR, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warr_devastate::HandleSunderArmor, EFFECT_1, SPELL_EFFECT_NORMALIZED_WEAPON_DMG);
    }
};

// 55694 - Enraged Regeneration
class spell_warr_enraged_regeneration : public SpellScript
{
    PrepareSpellScript(spell_warr_enraged_regeneration);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARRIOR_VENGEANCE });
    }

    // Vengeance also gives the enrage aura state, but it is no enrage effect
    SpellCastResult CheckCast()
    {
        for (auto const& [id, aurApp] : GetCaster()->GetAppliedAuras())
        {
            SpellInfo const* spellInfo = aurApp->GetBase()->GetSpellInfo();
            if (spellInfo->Id == SPELL_WARRIOR_VENGEANCE)
                continue;

            if (spellInfo->Mechanic == MECHANIC_ENRAGED)
                return SPELL_CAST_OK;

            for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                if (aurApp->HasEffect(i) && spellInfo->Effects[i].Mechanic == MECHANIC_ENRAGED)
                    return SPELL_CAST_OK;
        }

        return SPELL_FAILED_CASTER_AURASTATE;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_warr_enraged_regeneration::CheckCast);
    }
};

// No new enrage effects while it lasts
class spell_warr_enraged_regeneration_aura : public AuraScript
{
    PrepareAuraScript(spell_warr_enraged_regeneration_aura);

    void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_ENRAGED, true);
    }

    void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_ENRAGED, false);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_warr_enraged_regeneration_aura::HandleApply, EFFECT_0, SPELL_AURA_OBS_MOD_HEALTH, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_warr_enraged_regeneration_aura::HandleRemove, EFFECT_0, SPELL_AURA_OBS_MOD_HEALTH, AURA_EFFECT_HANDLE_REAL);
    }
};

// 5308 - Execute
// ${10+$AP*0.437*$m1/100} damage, and up to $m2 more rage for up to ${$ap*0.874*$m1/100-1} more damage.
class spell_warr_execute : public SpellScript
{
    PrepareSpellScript(spell_warr_execute);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARRIOR_SUDDEN_DEATH_R1 });
    }

    void CalculateDamage(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        float const ap = caster->GetTotalAttackPowerValue(BASE_ATTACK);
        int32 const apPct = GetEffectValue();
        int32 damage = int32(10 + CalculatePct(ap * 0.437f, apPct));

        // the cost is already taken here, so what is left is the extra rage (CPP subtracted the cost again)
        int32 const extraRageCap = GetSpellInfo()->Effects[EFFECT_1].CalcValue(caster) * 10;
        _extraRage = std::min<int32>(caster->GetPower(POWER_RAGE), extraRageCap);
        _powerBefore = caster->GetPower(POWER_RAGE) + GetSpell()->GetPowerCost();
        if (extraRageCap > 0 && _extraRage > 0)
        {
            int32 const bonus = std::max<int32>(0, int32(CalculatePct(ap * 0.874f, apPct)) - 1);
            damage += int32(bonus * float(_extraRage) / extraRageCap);
        }

        SetEffectValue(damage);
    }

    // Sudden Death: at least $s1 rage stays after Execute
    void ConsumeExtraRage()
    {
        Unit* caster = GetCaster();
        int32 rage = caster->GetPower(POWER_RAGE) - _extraRage;
        if (AuraEffect const* suddenDeath = caster->GetAuraEffectOfRankedSpell(SPELL_WARRIOR_SUDDEN_DEATH_R1, EFFECT_0))
            rage = std::max(rage, std::min(suddenDeath->GetAmount() * 10, _powerBefore));

        caster->SetPower(POWER_RAGE, std::max(rage, 0));
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_warr_execute::CalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        AfterCast += SpellCastFn(spell_warr_execute::ConsumeExtraRage);
    }

private:
    int32 _extraRage = 0;
    int32 _powerBefore = 0;
};

// 58387 - Glyph of Sunder Armor
class spell_warr_glyph_of_sunder_armor : public AuraScript
{
    PrepareAuraScript(spell_warr_glyph_of_sunder_armor);

    void HandleEffectCalcSpellMod(AuraEffect const* aurEff, SpellModifier*& spellMod)
    {
        if (!spellMod)
        {
            spellMod = new SpellModifier(aurEff->GetBase());
            spellMod->op = SpellModOp(aurEff->GetMiscValue());
            spellMod->type = SPELLMOD_FLAT;
            spellMod->spellId = GetId();
            spellMod->mask = GetSpellInfo()->Effects[aurEff->GetEffIndex()].SpellClassMask;
        }

        spellMod->value = aurEff->GetAmount();
    }

    void Register() override
    {
        DoEffectCalcSpellMod += AuraEffectCalcSpellModFn(spell_warr_glyph_of_sunder_armor::HandleEffectCalcSpellMod, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 60970 - Heroic Fury
class spell_warr_heroic_fury : public SpellScript
{
    PrepareSpellScript(spell_warr_heroic_fury);

    bool Load() override
    {
        return GetCaster()->IsPlayer();
    }

    void HandleCooldownReset(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->ToPlayer()->RemoveSpellCooldown(SPELL_WARRIOR_INTERCEPT, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warr_heroic_fury::HandleCooldownReset, EFFECT_2, SPELL_EFFECT_APPLY_AURA);
    }
};

// 6544 - Heroic Leap
class spell_warr_heroic_leap : public SpellScript
{
    PrepareSpellScript(spell_warr_heroic_leap);

    // The client shows a spot as unreachable once it is 4 yards above the warrior
    SpellCastResult CheckLocation()
    {
        WorldLocation const* dest = GetExplTargetDest();
        if (!dest || dest->GetPositionZ() - GetCaster()->GetPositionZ() > 4.0f)
            return SPELL_FAILED_NOPATH;

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_warr_heroic_leap::CheckLocation);
    }
};

// 78 - Heroic Strike
class spell_warr_heroic_strike : public SpellScript
{
    PrepareSpellScript(spell_warr_heroic_strike);

    // ${8+$ap*60/100}: the share of attack power is the second effect
    void CalculateDamage(SpellEffIndex /*effIndex*/)
    {
        int32 const pct = GetSpellInfo()->Effects[EFFECT_1].CalcValue(GetCaster());
        SetEffectValue(int32(8 + CalculatePct(GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK), pct)));
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_warr_heroic_strike::CalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 3411 - Intervene
// Lowers the protected ally's threat ($59667s1%); the client data has no trigger for it.
class spell_warr_intervene : public SpellScript
{
    PrepareSpellScript(spell_warr_intervene);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARRIOR_INTERVENE_TRIGGER });
    }

    void HandleApplyAura(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            target->CastSpell((Unit*)nullptr, SPELL_WARRIOR_INTERVENE_TRIGGER, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warr_intervene::HandleApplyAura, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
    }
};

// 5246 - Intimidating Shout
class spell_warr_intimidating_shout : public SpellScript
{
    PrepareSpellScript(spell_warr_intimidating_shout);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove(GetExplTargetWorldObject());
        uint32 maxTargets = GetSpellInfo()->MaxAffectedTargets;
        if (maxTargets && targets.size() > maxTargets)
            targets.resize(maxTargets);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warr_intimidating_shout::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warr_intimidating_shout::FilterTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// 70844 - Item - Warrior T10 Protection 4P Bonus
class spell_warr_item_t10_prot_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_warr_item_t10_prot_4p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARRIOR_T10_PROT_4P_ABSORB });
    }

    void HandleProc(ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();

        Unit* target = GetTarget();
        int32 absorb = CalculatePct(static_cast<int32>(target->GetMaxHealth()), GetSpellInfo()->Effects[EFFECT_1].CalcValue());
        target->CastCustomSpell(target, SPELL_WARRIOR_T10_PROT_4P_ABSORB, &absorb, nullptr, nullptr, true, nullptr, GetEffect(EFFECT_0));
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_warr_item_t10_prot_4p_bonus::HandleProc);
    }
};

// -84583 - Lambs to the Slaughter
// Mortal Strike refreshes the warrior's Rend on the target (the proc's own trigger gives Slaughter).
class spell_warr_lambs_to_the_slaughter : public AuraScript
{
    PrepareAuraScript(spell_warr_lambs_to_the_slaughter);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARRIOR_MORTAL_STRIKE, SPELL_WARRIOR_REND });
    }

    void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        if (Unit* target = eventInfo.GetActionTarget())
            if (Aura* rend = target->GetAura(SPELL_WARRIOR_REND, GetTarget()->GetGUID()))
                rend->RefreshDuration();
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_warr_lambs_to_the_slaughter::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 12975 - Last Stand
class spell_warr_last_stand : public SpellScript
{
    PrepareSpellScript(spell_warr_last_stand);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARRIOR_LAST_STAND_TRIGGERED });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        int32 healthModSpellBasePoints0 = int32(caster->CountPctFromMaxHealth(GetEffectValue()));
        caster->CastCustomSpell(caster, SPELL_WARRIOR_LAST_STAND_TRIGGERED, &healthModSpellBasePoints0, nullptr, nullptr, true, nullptr);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_warr_last_stand::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 97462 - Rallying Cry
class spell_warr_rallying_cry : public SpellScript
{
    PrepareSpellScript(spell_warr_rallying_cry);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARRIOR_RALLYING_CRY });
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        int32 health = int32(GetHitUnit()->CountPctFromMaxHealth(GetEffectValue()));
        GetCaster()->CastCustomSpell(GetHitUnit(), SPELL_WARRIOR_RALLYING_CRY, &health, nullptr, nullptr, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warr_rallying_cry::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 94009 - Rend (772 triggers this bleed)
class spell_warr_rend : public AuraScript
{
    PrepareAuraScript(spell_warr_rend);

    void CalculateAmount(AuraEffect const* aurEff, int32& amount, bool& canBeRecalculated)
    {
        if (Unit* caster = GetCaster())
        {
            canBeRecalculated = false;

            // $0.25 * (($MWB + $mwb) / 2 + $AP / 14 * $MWS) bonus per tick, unhasted weapon speed
            float ap = caster->GetTotalAttackPowerValue(BASE_ATTACK);
            int32 mws = caster->GetAttackTime(BASE_ATTACK);
            if (Player* player = caster->ToPlayer())
            {
                Item const* weapon = player->GetWeaponForAttack(BASE_ATTACK, true);
                mws = weapon ? weapon->GetTemplate()->Delay : BASE_ATTACK_TIME;
            }
            float mwbMin = 0.f;
            float mwbMax = 0.f;
            for (uint8 i = 0; i < MAX_ITEM_PROTO_DAMAGES; ++i)
            {
                mwbMin += caster->GetWeaponDamageRange(BASE_ATTACK, MINDAMAGE, i);
                mwbMax += caster->GetWeaponDamageRange(BASE_ATTACK, MAXDAMAGE, i);
            }

            float mwb = ((mwbMin + mwbMax) / 2 + ap * mws / 14000) * 0.25f;
            amount += int32(caster->ApplyEffectModifiers(GetSpellInfo(), aurEff->GetEffIndex(), mwb));
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_rend::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 20230 - Retaliation
class spell_warr_retaliation : public AuraScript
{
    PrepareAuraScript(spell_warr_retaliation);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARRIOR_RETALIATION_DAMAGE });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        // Prevent counterattacking yourself on activation
        if (eventInfo.GetActor() == eventInfo.GetActionTarget())
            return false;

        // check attack comes not from behind and warrior is not stunned
        return eventInfo.GetActionTarget()->isInFront(eventInfo.GetActor(), float(M_PI)) && !GetTarget()->HasUnitState(UNIT_STATE_STUNNED);
    }

    void HandleEffectProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        eventInfo.GetActionTarget()->CastSpell(eventInfo.GetActor(), SPELL_WARRIOR_RETALIATION_DAMAGE, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warr_retaliation::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_warr_retaliation::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -29834 - Second Wind
// Stuns and roots from others start the rank's rage and health regeneration.
class spell_warr_second_wind_proc : public AuraScript
{
    PrepareAuraScript(spell_warr_second_wind_proc);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_WARRIOR_SECOND_WIND_PROC_R1,
                SPELL_WARRIOR_SECOND_WIND_PROC_R2,
                SPELL_WARRIOR_SECOND_WIND_TRIGGER_R1,
                SPELL_WARRIOR_SECOND_WIND_TRIGGER_R2
            });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetActor() == GetTarget())
            return false;

        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        return spellInfo && (spellInfo->GetAllEffectsMechanicMask() & ((1ULL << MECHANIC_ROOT) | (1ULL << MECHANIC_STUN)));
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();

        uint32 spellId = GetId() == SPELL_WARRIOR_SECOND_WIND_PROC_R2 ? SPELL_WARRIOR_SECOND_WIND_TRIGGER_R2 : SPELL_WARRIOR_SECOND_WIND_TRIGGER_R1;
        GetTarget()->CastSpell(GetTarget(), spellId, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warr_second_wind_proc::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_warr_second_wind_proc::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 29841, 29842 - Second Wind (the regeneration)
// "$29841s2% of your total health over $29841d": the percentage is the total, split over the ticks
// (CPP healed it every tick).
class spell_warr_second_wind_trigger : public AuraScript
{
    PrepareAuraScript(spell_warr_second_wind_trigger);

    void CalculateAmount(AuraEffect const* aurEff, int32& amount, bool& /*canBeRecalculated*/)
    {
        amount = int32(GetUnitOwner()->CountPctFromMaxHealth(amount) / std::max(aurEff->GetTotalTicks(), 1));
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_second_wind_trigger::CalculateAmount, EFFECT_1, SPELL_AURA_PERIODIC_HEAL);
    }
};

// 64380, 65941 - Shattering Throw
class spell_warr_shattering_throw : public SpellScript
{
    PrepareSpellScript(spell_warr_shattering_throw);

    void HandleScript(SpellEffIndex effIndex)
    {
        PreventHitDefaultEffect(effIndex);

        // remove shields, will still display immune to damage part
        if (Unit* target = GetHitUnit())
            target->RemoveAurasWithMechanic(1ULL << MECHANIC_IMMUNE_SHIELD, AURA_REMOVE_BY_ENEMY_SPELL);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warr_shattering_throw::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// -12298 - Shield Specialization
// A reflected spell gives four times the rage of a block.
class spell_warr_shield_specialization : public AuraScript
{
    PrepareAuraScript(spell_warr_shield_specialization);

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        if (!(eventInfo.GetHitMask() & PROC_HIT_REFLECT))
            return;

        PreventDefaultAction();
        if (SpellInfo const* energize = sSpellMgr->GetSpellInfo(GetSpellInfo()->Effects[aurEff->GetEffIndex()].TriggerSpell))
        {
            int32 rage = energize->Effects[EFFECT_0].CalcValue() * 4;
            GetTarget()->CastCustomSpell(GetTarget(), energize->Id, &rage, nullptr, nullptr, true, nullptr, aurEff);
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_warr_shield_specialization::HandleProc, EFFECT_1, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 46968 - Shockwave
class spell_warr_shockwave : public SpellScript
{
    PrepareSpellScript(spell_warr_shockwave);

    // ${$m3/100*$AP}
    void CalculateDamage(SpellEffIndex /*effIndex*/)
    {
        int32 const pct = GetSpellInfo()->Effects[EFFECT_2].CalcValue(GetCaster());
        SetEffectValue(int32(CalculatePct(GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK), pct)));
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_warr_shockwave::CalculateDamage, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 1464 - Slam
// The hit is 50782 (and 97992 with the off hand under Single-Minded Fury).
class spell_warr_slam : public SpellScript
{
    PrepareSpellScript(spell_warr_slam);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_WARRIOR_SLAM_MAIN_HAND,
                SPELL_WARRIOR_SLAM_OFF_HAND,
                SPELL_WARRIOR_BATTLE_TRANCE_TRIGGERED,
                SPELL_WARRIOR_BLOODSURGE_TRIGGERED,
                SPELL_WARRIOR_SINGLE_MINDED_FURY
            });
    }

    void HandleBeforeCast()
    {
        _affectedByBloodsurge = GetCaster()->HasAura(SPELL_WARRIOR_BLOODSURGE_TRIGGERED);
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        caster->CastSpell(target, SPELL_WARRIOR_SLAM_MAIN_HAND, true);

        if (caster->HasAura(SPELL_WARRIOR_SINGLE_MINDED_FURY) && caster->haveOffhandWeapon())
            caster->CastSpell(target, SPELL_WARRIOR_SLAM_OFF_HAND, true);

        // a Slam made free by Bloodsurge does not use up Battle Trance
        if (!_affectedByBloodsurge)
            caster->RemoveAurasDueToSpell(SPELL_WARRIOR_BATTLE_TRANCE_TRIGGERED, caster->GetGUID());
    }

    void Register() override
    {
        BeforeCast += SpellCastFn(spell_warr_slam::HandleBeforeCast);
        OnEffectLaunchTarget += SpellEffectFn(spell_warr_slam::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }

private:
    bool _affectedByBloodsurge = false;
};

// 50782, 97992 - Slam (the hits)
// Bloodsurge's damage bonus. The weapon effects are summed by the engine, so the bonus multiplies the hit.
class spell_warr_slam_triggered : public SpellScript
{
    PrepareSpellScript(spell_warr_slam_triggered);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARRIOR_BLOODSURGE_R1, SPELL_WARRIOR_BLOODSURGE_TRIGGERED });
    }

    // cast from Slam's launch, before Slam uses up Bloodsurge
    void HandleBeforeCast()
    {
        _affectedByBloodsurge = GetCaster()->HasAura(SPELL_WARRIOR_BLOODSURGE_TRIGGERED);
    }

    void HandleDamage(SpellEffIndex /*effIndex*/)
    {
        if (!_affectedByBloodsurge)
            return;

        if (AuraEffect const* bloodsurge = GetCaster()->GetAuraEffectOfRankedSpell(SPELL_WARRIOR_BLOODSURGE_R1, EFFECT_0))
        {
            int32 damage = GetHitDamage();
            AddPct(damage, bloodsurge->GetAmount());
            SetHitDamage(damage);
        }
    }

    void Register() override
    {
        BeforeCast += SpellCastFn(spell_warr_slam_triggered::HandleBeforeCast);
        OnEffectHitTarget += SpellEffectFn(spell_warr_slam_triggered::HandleDamage, EFFECT_1, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
    }

private:
    bool _affectedByBloodsurge = false;
};

// 76838 - Strikes of Opportunity (arms mastery)
class spell_warr_strikes_of_opportunity : public AuraScript
{
    PrepareAuraScript(spell_warr_strikes_of_opportunity);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARRIOR_OPPORTUNITY_STRIKE });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        // no strike from its own strikes
        if (SpellInfo const* spellInfo = eventInfo.GetSpellInfo())
            if (spellInfo->Id == SPELL_WARRIOR_OPPORTUNITY_STRIKE)
                return false;

        return roll_chance_i(GetEffect(EFFECT_0)->GetAmount());
    }

    void HandleEffectProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        if (Unit* target = eventInfo.GetActionTarget())
            GetTarget()->CastSpell(target, SPELL_WARRIOR_OPPORTUNITY_STRIKE, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warr_strikes_of_opportunity::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_warr_strikes_of_opportunity::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 52437 - Sudden Death
class spell_warr_sudden_death : public AuraScript
{
    PrepareAuraScript(spell_warr_sudden_death);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARRIOR_COLOSSUS_SMASH });
    }

    void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        // Remove cooldown on Colossus Smash
        if (Player* player = GetTarget()->ToPlayer())
            player->RemoveSpellCooldown(SPELL_WARRIOR_COLOSSUS_SMASH, true);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_warr_sudden_death::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 12328, 18765, 35429 - Sweeping Strikes
class spell_warr_sweeping_strikes : public AuraScript
{
    PrepareAuraScript(spell_warr_sweeping_strikes);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARRIOR_SWEEPING_STRIKES_EXTRA_ATTACK_1, SPELL_WARRIOR_SWEEPING_STRIKES_EXTRA_ATTACK_2 });
    }

    bool Load() override
    {
        _procTargetGUID.Clear();
        return true;
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Unit* actor = eventInfo.GetActor();
        if (!actor)
            return false;

        if (SpellInfo const* spellInfo = eventInfo.GetSpellInfo())
        {
            switch (spellInfo->Id)
            {
                case SPELL_WARRIOR_SWEEPING_STRIKES_EXTRA_ATTACK_1:
                case SPELL_WARRIOR_SWEEPING_STRIKES_EXTRA_ATTACK_2:
                case SPELL_WARRIOR_WHIRLWIND_OFF_HAND:
                    return false;
                default:
                    break;
            }
        }

        Unit* procTarget = actor->SelectNearbyNoTotemTarget(eventInfo.GetProcTarget());
        if (procTarget)
            _procTargetGUID = procTarget->GetGUID();

        return procTarget != nullptr;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        Unit* procTarget = ObjectAccessor::GetUnit(*GetTarget(), _procTargetGUID);
        if (!procTarget)
            return;

        if (DamageInfo* damageInfo = eventInfo.GetDamageInfo())
        {
            SpellInfo const* spellInfo = damageInfo->GetSpellInfo();
            // Bladestorm's whirlwinds, and Execute while the second target is above 20% health, strike with
            // normalized weapon damage instead of copying the hit
            if (spellInfo && (spellInfo->Id == SPELL_WARRIOR_BLADESTORM_PERIODIC_WHIRLWIND
                || (spellInfo->Id == SPELL_WARRIOR_EXECUTE && !procTarget->HasAuraState(AURA_STATE_HEALTHLESS_20_PERCENT))))
                GetTarget()->CastSpell(procTarget, SPELL_WARRIOR_SWEEPING_STRIKES_EXTRA_ATTACK_2, true, nullptr, aurEff);
            else
            {
                int32 damage = static_cast<int32>(damageInfo->GetUnmitigatedDamage());
                GetTarget()->CastCustomSpell(procTarget, SPELL_WARRIOR_SWEEPING_STRIKES_EXTRA_ATTACK_1, &damage, nullptr, nullptr, true, nullptr, aurEff);
            }
        }
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warr_sweeping_strikes::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_warr_sweeping_strikes::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }

private:
    ObjectGuid _procTargetGUID;
};

// -46951 - Sword and Board
class spell_warr_sword_and_board : public AuraScript
{
    PrepareAuraScript(spell_warr_sword_and_board);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARRIOR_SHIELD_SLAM });
    }

    void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
    {
        // Remove cooldown on Shield Slam
        if (Player* player = GetTarget()->ToPlayer())
            player->RemoveSpellCooldown(SPELL_WARRIOR_SHIELD_SLAM, true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_warr_sword_and_board::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 28845 - Cheat Death
class spell_warr_t3_prot_8p_bonus : public AuraScript
{
    PrepareAuraScript(spell_warr_t3_prot_8p_bonus);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetActionTarget()->HealthBelowPct(20))
            return true;

        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (damageInfo && damageInfo->GetDamage())
            if (GetTarget()->HealthBelowPctDamaged(20, damageInfo->GetDamage()))
                return true;

        return false;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warr_t3_prot_8p_bonus::CheckProc);
    }
};

// 6343 - Thunder Clap
// Blood and Thunder: when a target has the warrior's Rend, a roll spreads Rend to every target hit.
class spell_warr_thunder_clap : public SpellScript
{
    PrepareSpellScript(spell_warr_thunder_clap);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARRIOR_REND, SPELL_WARRIOR_BLOOD_AND_THUNDER_R1 });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        Unit* caster = GetCaster();
        AuraEffect const* bloodAndThunder = caster->GetAuraEffectOfRankedSpell(SPELL_WARRIOR_BLOOD_AND_THUNDER_R1, EFFECT_0);
        if (!bloodAndThunder || !roll_chance_i(bloodAndThunder->GetAmount()))
            return;

        _spreadRend = std::any_of(targets.begin(), targets.end(), [caster](WorldObject const* target)
        {
            return target->IsUnit() && target->ToUnit()->HasAura(SPELL_WARRIOR_REND, caster->GetGUID());
        });
    }

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        if (_spreadRend)
            GetCaster()->CastSpell(GetHitUnit(), SPELL_WARRIOR_REND, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warr_thunder_clap::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_warr_thunder_clap::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }

private:
    bool _spreadRend = false;
};

// 34428 - Victory Rush
class spell_warr_victory_rush : public SpellScript
{
    PrepareSpellScript(spell_warr_victory_rush);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_WARRIOR_VICTORIOUS,
                SPELL_WARRIOR_VICTORIOUS_IMPENDING_VICTORY,
                SPELL_WARRIOR_IMPENDING_VICTORY_R1
            });
    }

    // ${$AP*$m1/100}
    void CalculateDamage(SpellEffIndex /*effIndex*/)
    {
        SetEffectValue(int32(CalculatePct(GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK), GetEffectValue())));
    }

    // Enabled by Impending Victory, it heals only the talent's $s1% (CPP read the enabling aura's 0)
    void CalculateHeal(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (caster->HasAura(SPELL_WARRIOR_VICTORIOUS_IMPENDING_VICTORY) && !caster->HasAura(SPELL_WARRIOR_VICTORIOUS))
            if (AuraEffect const* impendingVictory = caster->GetAuraEffectOfRankedSpell(SPELL_WARRIOR_IMPENDING_VICTORY_R1, EFFECT_0))
                SetEffectValue(impendingVictory->GetAmount());
    }

    void RemoveVictorious()
    {
        GetCaster()->RemoveAurasDueToSpell(SPELL_WARRIOR_VICTORIOUS);
        GetCaster()->RemoveAurasDueToSpell(SPELL_WARRIOR_VICTORIOUS_IMPENDING_VICTORY);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_warr_victory_rush::CalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnEffectLaunchTarget += SpellEffectFn(spell_warr_victory_rush::CalculateHeal, EFFECT_2, SPELL_EFFECT_HEAL_PCT);
        AfterCast += SpellCastFn(spell_warr_victory_rush::RemoveVictorious);
    }
};

// 50720 - Vigilance
// Each hit on the protected ally resets the warrior's Taunt and gives the warrior Vengeance as if $s2% of the
// damage had hit the warrior (4.3.4: no damage reduction or threat transfer any more).
class spell_warr_vigilance : public AuraScript
{
    PrepareAuraScript(spell_warr_vigilance);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARRIOR_VIGILANCE_PROC });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return GetCaster() && eventInfo.GetDamageInfo();
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        Unit* warrior = GetCaster();
        GetTarget()->CastSpell(warrior, SPELL_WARRIOR_VIGILANCE_PROC, true, nullptr, aurEff);

        uint32 share = CalculatePct(eventInfo.GetDamageInfo()->GetDamage(), GetSpellInfo()->Effects[EFFECT_1].CalcValue());
        if (share && eventInfo.GetActor() && !eventInfo.GetActor()->IsPlayer())
            AddVengeanceDamage(warrior, share);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warr_vigilance::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_warr_vigilance::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 50725 - Vigilance
class spell_warr_vigilance_trigger : public SpellScript
{
    PrepareSpellScript(spell_warr_vigilance_trigger);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARRIOR_TAUNT });
    }

    void HandleScript(SpellEffIndex effIndex)
    {
        PreventHitDefaultEffect(effIndex);

        // Remove Taunt cooldown
        if (Player* target = GetHitPlayer())
            target->RemoveSpellCooldown(SPELL_WARRIOR_TAUNT, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warr_vigilance_trigger::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 21977 - Warrior's Wrath (T3 8P Bonus)
class spell_warr_warriors_wrath : public SpellScript
{
    PrepareSpellScript(spell_warr_warriors_wrath);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARRIOR_WARRIORS_WRATH });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_WARRIOR_WARRIORS_WRATH, true);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_warr_warriors_wrath::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 1680 - Whirlwind
// Main-hand hits on four or more targets take $m3 seconds off its cooldown.
class spell_warr_whirlwind : public SpellScript
{
    PrepareSpellScript(spell_warr_whirlwind);

    void CountTargets(SpellEffIndex /*effIndex*/)
    {
        ++_hitTargets;
    }

    void HandleCooldownReduction()
    {
        if (_hitTargets < 4)
            return;

        if (Player* player = GetCaster()->ToPlayer())
            player->ModifySpellCooldown(GetSpellInfo()->Id, -GetSpellInfo()->Effects[EFFECT_2].CalcValue(player) * IN_MILLISECONDS);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warr_whirlwind::CountTargets, EFFECT_0, SPELL_EFFECT_NORMALIZED_WEAPON_DMG);
        AfterCast += SpellCastFn(spell_warr_whirlwind::HandleCooldownReduction);
    }

private:
    uint8 _hitTargets = 0;
};

void AddSC_warrior_spell_scripts()
{
    RegisterSpellScript(spell_warr_blood_craze);
    RegisterSpellScript(spell_warr_bloodthirst);
    RegisterSpellScript(spell_warr_bloodthirst_heal);
    RegisterSpellScript(spell_warr_charge);
    RegisterSpellScript(spell_warr_colossus_smash);
    RegisterSpellScript(spell_warr_concussion_blow);
    RegisterSpellScript(spell_warr_deep_wounds);
    RegisterSpellScript(spell_warr_devastate);
    RegisterSpellAndAuraScriptPair(spell_warr_enraged_regeneration, spell_warr_enraged_regeneration_aura);
    RegisterSpellScript(spell_warr_execute);
    RegisterSpellScript(spell_warr_glyph_of_sunder_armor);
    RegisterSpellScript(spell_warr_heroic_fury);
    RegisterSpellScript(spell_warr_heroic_leap);
    RegisterSpellScript(spell_warr_heroic_strike);
    RegisterSpellScript(spell_warr_intervene);
    RegisterSpellScript(spell_warr_intimidating_shout);
    RegisterSpellScript(spell_warr_item_t10_prot_4p_bonus);
    RegisterSpellScript(spell_warr_lambs_to_the_slaughter);
    RegisterSpellScript(spell_warr_last_stand);
    RegisterSpellScript(spell_warr_rallying_cry);
    RegisterSpellScript(spell_warr_rend);
    RegisterSpellScript(spell_warr_retaliation);
    RegisterSpellScript(spell_warr_second_wind_proc);
    RegisterSpellScript(spell_warr_second_wind_trigger);
    RegisterSpellScript(spell_warr_shattering_throw);
    RegisterSpellScript(spell_warr_shield_specialization);
    RegisterSpellScript(spell_warr_shockwave);
    RegisterSpellScript(spell_warr_slam);
    RegisterSpellScript(spell_warr_slam_triggered);
    RegisterSpellScript(spell_warr_strikes_of_opportunity);
    RegisterSpellScript(spell_warr_sudden_death);
    RegisterSpellScript(spell_warr_sweeping_strikes);
    RegisterSpellScript(spell_warr_sword_and_board);
    RegisterSpellScript(spell_warr_t3_prot_8p_bonus);
    RegisterSpellScript(spell_warr_thunder_clap);
    RegisterSpellScript(spell_warr_victory_rush);
    RegisterSpellScript(spell_warr_vigilance);
    RegisterSpellScript(spell_warr_vigilance_trigger);
    RegisterSpellScript(spell_warr_warriors_wrath);
    RegisterSpellScript(spell_warr_whirlwind);
}
