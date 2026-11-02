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

#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "MoveSpline.h"
#include "ObjectAccessor.h"
#include "Pet.h"
#include "Player.h"
#include "SpellAuraEffects.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "SpellScriptLoader.h"
/*
 * Scripts for spells with SPELLFAMILY_HUNTER, SPELLFAMILY_PET and SPELLFAMILY_GENERIC spells used by hunter players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_hun_".
 *
 * 4.3.4: most scripts are ported from The Cataclysm Preservation Project (spell_hunter.cpp and, for the pet
 * scaling, spell_pet.cpp at b061f46) and rewritten against AzerothCore; the bindings come from TDB 434
 * (scripts/build_world_db.py). The shots' attack power shares are spell_bonus_data rows, applied by
 * Spell::EffectWeaponDmg (CPP computed them in its scripts).
 */

/// @todo: this import is not necessary for compilation and marked as unused by the IDE
//  however, for some reasons removing it would cause a damn linking issue
//  there is probably some underlying problem with imports which should properly addressed
//  see: https://github.com/azerothcore/azerothcore-wotlk/issues/9766
#include "GridNotifiersImpl.h"

enum HunterSpells
{
    SPELL_HUNTER_BESTIAL_WRATH                      = 19574,
    SPELL_HUNTER_CALL_PET_1                         = 883,
    SPELL_HUNTER_CALL_PET_2                         = 83242,
    SPELL_HUNTER_CALL_PET_3                         = 83243,
    SPELL_HUNTER_CALL_PET_4                         = 83244,
    SPELL_HUNTER_CALL_PET_5                         = 83245,
    SPELL_HUNTER_CAMOUFLAGE_DURATION                = 51755,
    SPELL_HUNTER_CAMOUFLAGE_PERIODIC                = 80326,
    SPELL_HUNTER_CAMOUFLAGE_PERIODIC_TRIGGERED      = 80325,
    SPELL_HUNTER_CHIMERA_SHOT_HEAL                  = 53353,
    SPELL_HUNTER_DETERRENCE                         = 19263,
    SPELL_HUNTER_DISENGAGE                          = 781,
    SPELL_HUNTER_FERVOR_PET_ENERGIZE                = 82728,
    SPELL_HUNTER_FIRE                               = 82926,
    SPELL_HUNTER_FOCUS_FIRE                         = 82692,
    SPELL_HUNTER_FOCUS_FIRE_DUMMY                   = 88843,
    SPELL_HUNTER_FOCUS_FIRE_ENERGIZE                = 83468,
    SPELL_HUNTER_FRENZY_EFFECT                      = 19615,
    SPELL_HUNTER_GENERIC_ENERGIZE_FOCUS             = 91954,
    SPELL_HUNTER_GLYPH_OF_KILL_SHOT_COOLDOWN        = 90967,
    SPELL_HUNTER_IMPROVED_MEND_PET                  = 24406,
    SPELL_HUNTER_IMPROVED_SERPENT_STING_DAMAGE      = 83077,
    SPELL_HUNTER_IMPROVED_STEADY_SHOT_TRIGGERED     = 53220,
    SPELL_HUNTER_HUNTERS_MARK                       = 1130,
    SPELL_HUNTER_INSANITY                           = 95809,
    SPELL_HUNTER_INVIGORATION_TRIGGERED             = 53398,
    SPELL_HUNTER_KILLING_STREAK_TRIGGERED_R1        = 94006,
    SPELL_HUNTER_KILLING_STREAK_TRIGGERED_R2        = 94007,
    SPELL_HUNTER_LOCK_AND_LOAD                      = 56453,
    SPELL_HUNTER_LOCK_AND_LOAD_MARKER               = 67544,
    SPELL_HUNTER_MARKED_FOR_DEATH_TRIGGERED         = 88691,
    SPELL_HUNTER_MASTERS_CALL_TRIGGERED             = 62305,
    SPELL_HUNTER_MISDIRECTION                       = 34477,
    SPELL_HUNTER_MISDIRECTION_PROC                  = 35079,
    SPELL_HUNTER_PET_CARRION_FEEDER_TRIGGERED       = 54045,
    SPELL_HUNTER_PET_HEART_OF_THE_PHOENIX_DEBUFF    = 55711,
    SPELL_HUNTER_PET_HEART_OF_THE_PHOENIX_TRIGGERED = 54114,
    SPELL_HUNTER_PET_LAST_STAND_TRIGGERED           = 53479,
    SPELL_HUNTER_PIERCING_SHOTS                     = 63468,
    SPELL_HUNTER_RAPID_RECUPERATION                 = 58883,
    SPELL_HUNTER_READINESS                          = 23989,
    SPELL_HUNTER_RESISTANCE_IS_FUTILE_BUFF          = 82897,
    SPELL_HUNTER_RESISTANCE_IS_FUTILE_ENERGIZE      = 86316,
    SPELL_HUNTER_SERPENT_SPREAD_STING_R1            = 88453,
    SPELL_HUNTER_SERPENT_SPREAD_STING_R2            = 88466,
    SPELL_HUNTER_SIC_EM_R1                          = 83359,
    SPELL_HUNTER_SIC_EM_R2                          = 89388,
    SPELL_HUNTER_SNIPER_TRAINING_BUFF_R1            = 64418,
    SPELL_HUNTER_SNIPER_TRAINING_R1                 = 53302,
    SPELL_HUNTER_STEADY_SHOT                        = 56641,
    SPELL_HUNTER_STEADY_SHOT_FOCUS                  = 77443,
    SPELL_HUNTER_T9_4P_GREATNESS                    = 68130,
    SPELL_HUNTER_THRILL_OF_THE_HUNT                 = 34720,
    SPELL_HUNTER_TRAP_LAUNCHER_LINKED               = 82946,
    SPELL_HUNTER_WILD_QUIVER_DAMAGE                 = 76663,
    SPELL_HUNTER_WYVERN_STING_DOT                   = 24131,

    // the pet's talent tree markers (pet scaling)
    SPELL_HUNTER_PET_CUNNING_MARKER                 = 87884,
    SPELL_HUNTER_PET_FEROCITY_MARKER                = 87887,
    SPELL_HUNTER_PET_TENACITY_MARKER                = 87891
};

enum HunterSpellIcons
{
    HUNTER_ICON_ID_GLYPH_OF_DAZZLED_PREY            = 2109,
    HUNTER_ICON_ID_IMPROVED_SERPENT_STING           = 536,
    HUNTER_ICON_ID_INVIGORATION                     = 3487,
    HUNTER_ICON_ID_TERMINATION                      = 2008
};

enum HunterMiscSpells
{
    SPELL_DRAENEI_GIFT_OF_THE_NAARU                 = 59543,
    SPELL_MAGE_TEMPORAL_DISPLACEMENT                = 80354,
    SPELL_SHAMAN_EXHAUSTION                         = 57723,
    SPELL_SHAMAN_SATED                              = 57724
};

constexpr std::array<uint32, MAX_ACTIVE_PETS> CallPetSpellIdBySlot =
{
    SPELL_HUNTER_CALL_PET_1,
    SPELL_HUNTER_CALL_PET_2,
    SPELL_HUNTER_CALL_PET_3,
    SPELL_HUNTER_CALL_PET_4,
    SPELL_HUNTER_CALL_PET_5
};

// Serpent Sting and Serpent Spread's stings
static Aura* GetSerpentStingAura(Unit* target, ObjectGuid casterGUID)
{
    if (AuraEffect* aurEff = target->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_HUNTER, 0x4000, 0, 0, casterGUID))
        return aurEff->GetBase();

    return nullptr;
}

// Steady Shot and Cobra Shot: their focus, more with Termination (target below its health) and the Glyph of
// Dazzled Prey (dazed target)
static void CastShotFocus(Unit* caster, Unit* target, uint32 energizeSpellId)
{
    int32 focus = sSpellMgr->AssertSpellInfo(energizeSpellId)->Effects[EFFECT_0].CalcValue();
    if (target)
    {
        if (AuraEffect const* termination = caster->GetDummyAuraEffect(SPELLFAMILY_HUNTER, HUNTER_ICON_ID_TERMINATION, EFFECT_0))
            if (target->GetHealthPct() < termination->GetSpellInfo()->Effects[EFFECT_1].CalcValue())
                focus += termination->GetAmount();

        if (AuraEffect const* dazzledPrey = caster->GetDummyAuraEffect(SPELLFAMILY_HUNTER, HUNTER_ICON_ID_GLYPH_OF_DAZZLED_PREY, EFFECT_0))
            if (target->HasAuraWithMechanic(1ULL << MECHANIC_DAZE))
                focus += dazzledPrey->GetAmount();
    }

    caster->CastCustomSpell(caster, energizeSpellId, &focus, nullptr, nullptr, true);
}

// 90355 - Ancient Hysteria
class spell_hun_ancient_hysteria : public SpellScript
{
    PrepareSpellScript(spell_hun_ancient_hysteria);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_INSANITY, SPELL_MAGE_TEMPORAL_DISPLACEMENT, SPELL_SHAMAN_EXHAUSTION, SPELL_SHAMAN_SATED });
    }

    void RemoveInvalidTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([](WorldObject const* target)
        {
            Unit const* unit = target->ToUnit();
            return !unit || unit->HasAura(SPELL_HUNTER_INSANITY) || unit->HasAura(SPELL_MAGE_TEMPORAL_DISPLACEMENT)
                || unit->HasAura(SPELL_SHAMAN_EXHAUSTION) || unit->HasAura(SPELL_SHAMAN_SATED);
        });
    }

    void ApplyDebuff()
    {
        if (Unit* target = GetHitUnit())
            target->CastSpell(target, SPELL_HUNTER_INSANITY, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hun_ancient_hysteria::RemoveInvalidTargets, EFFECT_ALL, TARGET_UNIT_CASTER_AREA_RAID);
        AfterHit += SpellHitFn(spell_hun_ancient_hysteria::ApplyDebuff);
    }
};

// 19574 - Bestial Wrath
class spell_hun_bestial_wrath : public SpellScript
{
    PrepareSpellScript(spell_hun_bestial_wrath);

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();
        if (!caster || !caster->IsPlayer())
            return SPELL_FAILED_NO_VALID_TARGETS;

        Pet* pet = caster->ToPlayer()->GetPet();
        if (!pet)
            return SPELL_FAILED_NO_PET;

        if (!pet->IsAlive())
        {
            SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_PET_IS_DEAD);
            return SPELL_FAILED_CUSTOM_ERROR;
        }

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_hun_bestial_wrath::CheckCast);
    }
};

// 51753 - Camouflage
class spell_hun_camouflage : public SpellScript
{
    PrepareSpellScript(spell_hun_camouflage);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_CAMOUFLAGE_DURATION });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        caster->CastSpell(caster, SPELL_HUNTER_CAMOUFLAGE_DURATION, true);

        if (Player* player = caster->ToPlayer())
            if (Pet* pet = player->GetPet())
                pet->CastSpell(pet, SPELL_HUNTER_CAMOUFLAGE_DURATION, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_hun_camouflage::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 51755 - Camouflage
class spell_hun_camouflage_duration : public AuraScript
{
    PrepareAuraScript(spell_hun_camouflage_duration);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_CAMOUFLAGE_PERIODIC, SPELL_HUNTER_CAMOUFLAGE_PERIODIC_TRIGGERED });
    }

    void AfterApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->CastSpell(GetTarget(), SPELL_HUNTER_CAMOUFLAGE_PERIODIC, true, nullptr, aurEff);
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_HUNTER_CAMOUFLAGE_PERIODIC);
        GetTarget()->RemoveAurasDueToSpell(SPELL_HUNTER_CAMOUFLAGE_PERIODIC_TRIGGERED);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_hun_camouflage_duration::AfterApply, EFFECT_0, SPELL_AURA_INTERFERE_TARGETTING, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_hun_camouflage_duration::AfterRemove, EFFECT_0, SPELL_AURA_INTERFERE_TARGETTING, AURA_EFFECT_HANDLE_REAL);
    }
};

// 80325 - Camouflage
class spell_hun_camouflage_triggered : public AuraScript
{
    PrepareAuraScript(spell_hun_camouflage_triggered);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_CAMOUFLAGE_PERIODIC, SPELL_HUNTER_CAMOUFLAGE_DURATION });
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        target->RemoveAurasDueToSpell(SPELL_HUNTER_CAMOUFLAGE_PERIODIC);
        target->RemoveAurasDueToSpell(SPELL_HUNTER_CAMOUFLAGE_DURATION);

        if (Player* player = target->ToPlayer())
        {
            if (Pet* pet = player->GetPet())
            {
                pet->RemoveAurasDueToSpell(SPELL_HUNTER_CAMOUFLAGE_PERIODIC);
                pet->RemoveAurasDueToSpell(SPELL_HUNTER_CAMOUFLAGE_DURATION);
            }
        }
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_hun_camouflage_triggered::AfterRemove, EFFECT_0, SPELL_AURA_MOD_STEALTH, AURA_EFFECT_HANDLE_REAL);
    }
};

// -755 - Health Funnel, -136 - Mend Pet
class spell_hun_check_pet_los : public SpellScript
{
    PrepareSpellScript(spell_hun_check_pet_los);

    SpellCastResult CheckCast()
    {
        Unit* pet = GetCaster()->GetGuardianPet();
        if (!pet)
            pet = GetCaster()->GetCharm();

        if (!pet)
            return SPELL_FAILED_NO_PET;

        if (!pet->IsAlive())
        {
            SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_PET_IS_DEAD);
            return SPELL_FAILED_CUSTOM_ERROR;
        }

        if (!GetCaster()->IsWithinLOSInMap(pet))
            return SPELL_FAILED_LINE_OF_SIGHT;

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_hun_check_pet_los::CheckCast);
    }
};

// 53209 - Chimera Shot
// Heals the hunter and refreshes the Serpent Sting on the target (the attack power share is a
// spell_bonus_data row).
class spell_hun_chimera_shot : public SpellScript
{
    PrepareSpellScript(spell_hun_chimera_shot);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_CHIMERA_SHOT_HEAL });
    }

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        caster->CastSpell(caster, SPELL_HUNTER_CHIMERA_SHOT_HEAL, true);

        if (Aura* serpentSting = GetSerpentStingAura(GetHitUnit(), caster->GetGUID()))
            serpentSting->RefreshDuration();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_hun_chimera_shot::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// -53256 - Cobra Strikes (talent)
// The buff comes with all its stacks (the next 2 Basic Attacks).
class spell_hun_cobra_strikes : public AuraScript
{
    PrepareAuraScript(spell_hun_cobra_strikes);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->Effects[EFFECT_0].TriggerSpell });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();

        SpellInfo const* triggeredSpellInfo = sSpellMgr->GetSpellInfo(aurEff->GetSpellInfo()->Effects[aurEff->GetEffIndex()].TriggerSpell);
        if (!triggeredSpellInfo)
            return;

        GetTarget()->CastCustomSpell(triggeredSpellInfo->Id, SPELLVALUE_AURA_STACK, triggeredSpellInfo->StackAmount, GetTarget(), true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_hun_cobra_strikes::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 53257 - Cobra Strikes (the buff): one stack per Basic Attack
class spell_hun_cobra_strikes_triggered : public AuraScript
{
    PrepareAuraScript(spell_hun_cobra_strikes_triggered);

    void HandleStackDrop(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
    {
        ModStackAmount(-1);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_hun_cobra_strikes_triggered::HandleStackDrop, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER);
    }
};

// 77767 - Cobra Shot
// Focus, and $s2 more seconds on the Serpent Sting on the target.
class spell_hun_cobra_shot : public SpellScript
{
    PrepareSpellScript(spell_hun_cobra_shot);

    bool Load() override
    {
        return GetCaster()->IsPlayer();
    }

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_GENERIC_ENERGIZE_FOCUS });
    }

    void HandleLaunch(SpellEffIndex /*effIndex*/)
    {
        CastShotFocus(GetCaster(), GetExplTargetUnit(), SPELL_HUNTER_GENERIC_ENERGIZE_FOCUS);
    }

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        if (Aura* serpentSting = GetSerpentStingAura(GetHitUnit(), GetCaster()->GetGUID()))
        {
            int32 duration = serpentSting->GetDuration() + GetEffectValue() * IN_MILLISECONDS;
            serpentSting->SetDuration(std::min(duration, serpentSting->GetMaxDuration()), true);
        }
    }

    void Register() override
    {
        OnEffectLaunch += SpellEffectFn(spell_hun_cobra_shot::HandleLaunch, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
        OnEffectHitTarget += SpellEffectFn(spell_hun_cobra_shot::HandleScriptEffect, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 1742 - Cower
class spell_hun_cower : public AuraScript
{
    PrepareAuraScript(spell_hun_cower);

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        if (AuraEffect* aurEff = GetUnitOwner()->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_PET, GetSpellInfo()->SpellIconID, EFFECT_0))
            AddPct(amount, aurEff->GetAmount());
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hun_cower::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_DECREASE_SPEED);
    }
};

// -82898 - Crouching Tiger, Hidden Chimera
// Melee hits on you take time off Disengage, ranged hits off Deterrence.
class spell_hun_crouching_tiger_hidden_chimera : public AuraScript
{
    PrepareAuraScript(spell_hun_crouching_tiger_hidden_chimera);

    bool Load() override
    {
        return GetUnitOwner()->IsPlayer();
    }

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_DISENGAGE, SPELL_HUNTER_DETERRENCE });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetDamageInfo() != nullptr;
    }

    void HandleDisengageProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        WeaponAttackType attackType = eventInfo.GetDamageInfo()->GetAttackType();
        if (attackType == BASE_ATTACK || attackType == OFF_ATTACK)
            GetTarget()->ToPlayer()->ModifySpellCooldown(SPELL_HUNTER_DISENGAGE, -aurEff->GetAmount());
    }

    void HandleDeterrenceProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        if (eventInfo.GetDamageInfo()->GetAttackType() == RANGED_ATTACK)
            GetTarget()->ToPlayer()->ModifySpellCooldown(SPELL_HUNTER_DETERRENCE, -aurEff->GetAmount());
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_hun_crouching_tiger_hidden_chimera::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_hun_crouching_tiger_hidden_chimera::HandleDisengageProc, EFFECT_0, SPELL_AURA_DUMMY);
        OnEffectProc += AuraEffectProcFn(spell_hun_crouching_tiger_hidden_chimera::HandleDeterrenceProc, EFFECT_1, SPELL_AURA_DUMMY);
    }
};

// 781 - Disengage
class spell_hun_disengage : public SpellScript
{
    PrepareSpellScript(spell_hun_disengage);

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();
        if (caster->IsPlayer() && !caster->IsInCombat())
            return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_hun_disengage::CheckCast);
    }
};

// 82726 - Fervor
class spell_hun_fervor : public SpellScript
{
    PrepareSpellScript(spell_hun_fervor);

    bool Load() override
    {
        return GetCaster()->IsPlayer();
    }

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_FERVOR_PET_ENERGIZE });
    }

    void HandleEnergize(SpellEffIndex /*effIndex*/)
    {
        Player* player = GetCaster()->ToPlayer();
        if (Pet* pet = player->GetPet())
            player->CastSpell(pet, SPELL_HUNTER_FERVOR_PET_ENERGIZE, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_hun_fervor::HandleEnergize, EFFECT_0, SPELL_EFFECT_ENERGIZE);
    }
};

// 82692 - Focus Fire
// Only with Frenzy Effect on the pet; consumes its stacks for ranged haste and pet focus.
class spell_hun_focus_fire : public SpellScript
{
    PrepareSpellScript(spell_hun_focus_fire);

    bool Load() override
    {
        return GetCaster()->IsPlayer();
    }

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_FRENZY_EFFECT, SPELL_HUNTER_FOCUS_FIRE_ENERGIZE });
    }

    SpellCastResult CheckCast()
    {
        Pet* pet = GetCaster()->ToPlayer()->GetPet();
        if (!pet)
            return SPELL_FAILED_NO_PET;

        if (!pet->HasAura(SPELL_HUNTER_FRENZY_EFFECT))
            return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_hun_focus_fire::CheckCast);
    }
};

class spell_hun_focus_fire_aura : public AuraScript
{
    PrepareAuraScript(spell_hun_focus_fire_aura);

    void CalculateAmount(AuraEffect const* aurEff, int32& amount, bool& /*canBeRecalculated*/)
    {
        amount = 0;
        Player* player = GetUnitOwner()->ToPlayer();
        Pet* pet = player ? player->GetPet() : nullptr;
        if (!pet)
            return;

        if (Aura* frenzy = pet->GetAura(SPELL_HUNTER_FRENZY_EFFECT, pet->GetGUID()))
        {
            _consumedStacks = frenzy->GetStackAmount();
            frenzy->Remove();
            amount = GetSpellInfo()->Effects[aurEff->GetEffIndex()].CalcValue() * _consumedStacks;
        }
    }

    void AfterApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (!_consumedStacks)
            return;

        int32 focus = _consumedStacks * aurEff->GetAmount();
        GetTarget()->CastCustomSpell(SPELL_HUNTER_FOCUS_FIRE_ENERGIZE, SPELLVALUE_BASE_POINT0, focus, nullptr, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hun_focus_fire_aura::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_RANGED_HASTE);
        AfterEffectApply += AuraEffectApplyFn(spell_hun_focus_fire_aura::AfterApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }

private:
    uint8 _consumedStacks = 0;
};

// 19615 - Frenzy Effect
// At full stacks the hunter's Focus Fire lights up.
class spell_hun_frenzy_effect : public AuraScript
{
    PrepareAuraScript(spell_hun_frenzy_effect);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_FOCUS_FIRE_DUMMY, SPELL_HUNTER_FOCUS_FIRE });
    }

    void AfterApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (GetStackAmount() == GetSpellInfo()->StackAmount)
            if (Unit* owner = GetTarget()->GetOwner())
                if (owner->HasSpell(SPELL_HUNTER_FOCUS_FIRE))
                    owner->CastSpell(owner, SPELL_HUNTER_FOCUS_FIRE_DUMMY, true, nullptr, aurEff);
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetTarget()->GetOwner())
            owner->RemoveAurasDueToSpell(SPELL_HUNTER_FOCUS_FIRE_DUMMY, owner->GetGUID());
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_hun_frenzy_effect::AfterApply, EFFECT_0, SPELL_AURA_MOD_MELEE_HASTE_3, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        AfterEffectRemove += AuraEffectRemoveFn(spell_hun_frenzy_effect::AfterRemove, EFFECT_0, SPELL_AURA_MOD_MELEE_HASTE_3, AURA_EFFECT_HANDLE_REAL);
    }
};

// 63067 - Glyph of Kill Shot
// A Kill Shot that does not kill can be used again at once (once every 6 s).
class spell_hun_glyph_of_kill_shot : public AuraScript
{
    PrepareAuraScript(spell_hun_glyph_of_kill_shot);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_GLYPH_OF_KILL_SHOT_COOLDOWN });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return GetTarget()->IsPlayer() && !GetTarget()->HasAura(SPELL_HUNTER_GLYPH_OF_KILL_SHOT_COOLDOWN)
            && eventInfo.GetDamageInfo() && eventInfo.GetSpellInfo();
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        Unit* victim = damageInfo->GetVictim();
        if (!victim || damageInfo->GetDamage() >= victim->GetHealth())
            return;

        Player* player = GetTarget()->ToPlayer();
        player->RemoveSpellCooldown(eventInfo.GetSpellInfo()->Id, true);
        player->CastSpell(player, SPELL_HUNTER_GLYPH_OF_KILL_SHOT_COOLDOWN, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_hun_glyph_of_kill_shot::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_hun_glyph_of_kill_shot::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -19572 - Improved Mend Pet
class spell_hun_improved_mend_pet : public AuraScript
{
    PrepareAuraScript(spell_hun_improved_mend_pet);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_IMPROVED_MEND_PET });
    }

    bool CheckProc(ProcEventInfo& /*eventInfo*/)
    {
        return roll_chance_i(GetEffect(EFFECT_0)->GetAmount());
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(GetTarget(), SPELL_HUNTER_IMPROVED_MEND_PET, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_hun_improved_mend_pet::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_hun_improved_mend_pet::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -53221 - Improved Steady Shot
// Two Steady Shots in a row give the haste buff.
class spell_hun_improved_steady_shot : public AuraScript
{
    PrepareAuraScript(spell_hun_improved_steady_shot);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_STEADY_SHOT, SPELL_HUNTER_IMPROVED_STEADY_SHOT_TRIGGERED });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo || spellInfo->Id != SPELL_HUNTER_STEADY_SHOT)
        {
            _steadyShotCounter = 0;
            return;
        }

        if (++_steadyShotCounter == 2)
        {
            int32 haste = aurEff->GetAmount();
            GetTarget()->CastCustomSpell(GetTarget(), SPELL_HUNTER_IMPROVED_STEADY_SHOT_TRIGGERED, &haste, nullptr, nullptr, true, nullptr, aurEff);
            _steadyShotCounter = 0;
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_hun_improved_steady_shot::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }

private:
    uint8 _steadyShotCounter = 0;
};

// 19577 - Intimidation
class spell_hun_intimidation : public AuraScript
{
    PrepareAuraScript(spell_hun_intimidation);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (SpellInfo const* spellInfo = eventInfo.GetSpellInfo())
            return !spellInfo->IsPositive();

        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_hun_intimidation::CheckProc);
    }
};

// 53412 - Invigoration
// The pet's critical Basic Attacks give the hunter the talent's focus.
class spell_hun_invigoration : public SpellScript
{
    PrepareSpellScript(spell_hun_invigoration);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_INVIGORATION_TRIGGERED });
    }

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        Unit* target = GetHitUnit();
        if (AuraEffect const* aurEff = target->GetDummyAuraEffect(SPELLFAMILY_HUNTER, HUNTER_ICON_ID_INVIGORATION, EFFECT_0))
        {
            int32 focus = aurEff->GetAmount();
            target->CastCustomSpell(target, SPELL_HUNTER_INVIGORATION_TRIGGERED, &focus, nullptr, nullptr, true, nullptr, aurEff);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_hun_invigoration::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 34026 - Kill Command
// The pet attacks its victim with the spell in effect 1 at once.
class spell_hun_kill_command : public SpellScript
{
    PrepareSpellScript(spell_hun_kill_command);

    bool Load() override
    {
        return GetCaster()->IsPlayer();
    }

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ uint32(spellInfo->Effects[EFFECT_1].BasePoints) });
    }

    SpellCastResult CheckCast()
    {
        // no pet: SPELL_FAILED_NO_PET from the effect's TARGET_UNIT_PET
        Pet* pet = GetCaster()->ToPlayer()->GetPet();
        if (!pet)
            return SPELL_FAILED_NO_PET;

        Unit* victim = pet->GetVictim();
        if (!victim)
        {
            SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_PET_MUST_BE_ATTACKING);
            return SPELL_FAILED_CUSTOM_ERROR;
        }

        // the pet's 83381 is a melee range (5 yard) attack
        if (!pet->IsWithinMeleeRange(victim))
            return SPELL_FAILED_OUT_OF_RANGE;

        return SPELL_CAST_OK;
    }

    void HandleScriptEffect(SpellEffIndex effIndex)
    {
        if (Pet* pet = GetCaster()->ToPlayer()->GetPet())
            if (Unit* victim = pet->GetVictim())
                pet->CastSpell(victim, uint32(GetSpellInfo()->Effects[effIndex].BasePoints), false);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_hun_kill_command::CheckCast);
        OnEffectHitTarget += SpellEffectFn(spell_hun_kill_command::HandleScriptEffect, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// -82748 - Killing Streak
// Two critical Kill Commands in a row give the rank's buff.
class spell_hun_killing_streak : public AuraScript
{
    PrepareAuraScript(spell_hun_killing_streak);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_KILLING_STREAK_TRIGGERED_R1, SPELL_HUNTER_KILLING_STREAK_TRIGGERED_R2 });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        if (!(eventInfo.GetHitMask() & PROC_HIT_CRITICAL))
        {
            _firstCrit = false;
            return;
        }

        if (!_firstCrit)
        {
            _firstCrit = true;
            return;
        }

        uint32 spellId = GetSpellInfo()->GetRank() > 1 ? SPELL_HUNTER_KILLING_STREAK_TRIGGERED_R2 : SPELL_HUNTER_KILLING_STREAK_TRIGGERED_R1;
        GetTarget()->CastSpell(GetTarget(), spellId, true, nullptr, aurEff);
        _firstCrit = false;
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_hun_killing_streak::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }

private:
    bool _firstCrit = false;
};

// 53478 - Last Stand Pet
class spell_hun_last_stand_pet : public SpellScript
{
    PrepareSpellScript(spell_hun_last_stand_pet);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_PET_LAST_STAND_TRIGGERED });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        int32 healthModSpellBasePoints0 = int32(caster->CountPctFromMaxHealth(30));
        caster->CastCustomSpell(caster, SPELL_HUNTER_PET_LAST_STAND_TRIGGERED, &healthModSpellBasePoints0, nullptr, nullptr, true, nullptr);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_hun_last_stand_pet::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// -56342 - Lock and Load
// The talent's chance, at most once in the marker's time.
class spell_hun_lock_and_load : public AuraScript
{
    PrepareAuraScript(spell_hun_lock_and_load);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_LOCK_AND_LOAD, SPELL_HUNTER_LOCK_AND_LOAD_MARKER });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetActor() && !eventInfo.GetActor()->HasAura(SPELL_HUNTER_LOCK_AND_LOAD_MARKER);
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        if (!roll_chance_i(aurEff->GetAmount()))
            return;

        Unit* actor = eventInfo.GetActor();
        actor->CastSpell(actor, GetSpellInfo()->Effects[aurEff->GetEffIndex()].TriggerSpell, true, nullptr, aurEff);
        actor->CastSpell(actor, SPELL_HUNTER_LOCK_AND_LOAD_MARKER, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_hun_lock_and_load::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_hun_lock_and_load::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// -53241 - Marked for Death
class spell_hun_marked_for_death : public AuraScript
{
    PrepareAuraScript(spell_hun_marked_for_death);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_MARKED_FOR_DEATH_TRIGGERED });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetProcTarget() && roll_chance_i(GetEffect(EFFECT_0)->GetAmount());
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(eventInfo.GetProcTarget(), SPELL_HUNTER_MARKED_FOR_DEATH_TRIGGERED, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_hun_marked_for_death::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_hun_marked_for_death::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// Resistance is Futile: the unit carrying one of the hunter's marks among the units the hunter fights
static Unit* GetHunterMarkedTarget(Unit* hunter)
{
    Unit* candidates[3] = { hunter->GetVictim(), ObjectAccessor::GetUnit(*hunter, hunter->GetTarget()), nullptr };
    if (Player* player = hunter->ToPlayer())
        if (Pet* pet = player->GetPet())
            candidates[2] = pet->GetVictim();

    for (Unit* unit : candidates)
        if (unit && unit->IsAlive() && (unit->HasAura(SPELL_HUNTER_HUNTERS_MARK, hunter->GetGUID()) ||
            unit->HasAura(SPELL_HUNTER_MARKED_FOR_DEATH_TRIGGERED, hunter->GetGUID())))
            return unit;
    return nullptr;
}

// -82893 - Resistance is Futile
// 4.3.4 has no event for a marked target that moves; the talent checks once a second instead, and a marked
// target that is moving gives the talent's chance to make the next Kill Command free.
class spell_hun_resistance_is_futile : public AuraScript
{
    PrepareAuraScript(spell_hun_resistance_is_futile);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_RESISTANCE_IS_FUTILE_BUFF, SPELL_HUNTER_HUNTERS_MARK,
            SPELL_HUNTER_MARKED_FOR_DEATH_TRIGGERED });
    }

    void CalcPeriodic(AuraEffect const* /*aurEff*/, bool& isPeriodic, int32& amplitude)
    {
        isPeriodic = true;
        amplitude = 1 * IN_MILLISECONDS;
    }

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        Unit* hunter = GetTarget();
        if (!hunter->IsInCombat() || hunter->HasAura(SPELL_HUNTER_RESISTANCE_IS_FUTILE_BUFF))
            return;

        Unit* marked = GetHunterMarkedTarget(hunter);
        if (!marked || (!marked->isMoving() && marked->movespline->Finalized()))
            return;

        if (roll_chance_i(aurEff->GetAmount()))
            hunter->CastSpell(hunter, SPELL_HUNTER_RESISTANCE_IS_FUTILE_BUFF, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_hun_resistance_is_futile::CalcPeriodic, EFFECT_0, SPELL_AURA_DUMMY);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_hun_resistance_is_futile::HandlePeriodic, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 82897 - Resistance is Futile!
class spell_hun_resistance_is_futile_buff : public AuraScript
{
    PrepareAuraScript(spell_hun_resistance_is_futile_buff);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_RESISTANCE_IS_FUTILE_ENERGIZE });
    }

    // only a Kill Command on the marked target (the one the pet attacks) is refunded
    bool CheckProc(ProcEventInfo& /*eventInfo*/)
    {
        Player* hunter = GetTarget()->ToPlayer();
        Pet* pet = hunter ? hunter->GetPet() : nullptr;
        Unit* victim = pet ? pet->GetVictim() : nullptr;
        return victim && (victim->HasAura(SPELL_HUNTER_HUNTERS_MARK, hunter->GetGUID()) ||
            victim->HasAura(SPELL_HUNTER_MARKED_FOR_DEATH_TRIGGERED, hunter->GetGUID()));
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(GetTarget(), SPELL_HUNTER_RESISTANCE_IS_FUTILE_ENERGIZE, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_hun_resistance_is_futile_buff::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_hun_resistance_is_futile_buff::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 53271 - Masters Call
class spell_hun_masters_call : public SpellScript
{
    PrepareSpellScript(spell_hun_masters_call);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_MASTERS_CALL_TRIGGERED });
    }

    SpellCastResult DoCheckCast()
    {
        Pet* pet = GetCaster()->ToPlayer()->GetPet();
        if (!pet || !pet->IsAlive())
            return SPELL_FAILED_NO_PET;

        // Do a mini Spell::CheckCasterAuras on the pet, no other way of doing this
        SpellCastResult result = SPELL_CAST_OK;
        uint32 const unitflag = pet->GetUnitFlags();
        if (pet->GetCharmerGUID())
            result = SPELL_FAILED_CHARMED;
        else if (unitflag & UNIT_FLAG_STUNNED)
            result = SPELL_FAILED_STUNNED;
        else if (unitflag & UNIT_FLAG_FLEEING)
            result = SPELL_FAILED_FLEEING;
        else if (unitflag & UNIT_FLAG_CONFUSED)
            result = SPELL_FAILED_CONFUSED;

        if (result != SPELL_CAST_OK)
            return result;

        Unit* target = GetExplTargetUnit();
        if (!target)
            return SPELL_FAILED_BAD_TARGETS;

        if (!pet->IsWithinLOSInMap(target))
            return SPELL_FAILED_LINE_OF_SIGHT;

        return SPELL_CAST_OK;
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->ToPlayer()->GetPet()->CastSpell(GetHitUnit(), GetEffectValue(), true);
    }

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        GetHitUnit()->CastSpell((Unit*)nullptr, SPELL_HUNTER_MASTERS_CALL_TRIGGERED, true);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_hun_masters_call::DoCheckCast);

        OnEffectHitTarget += SpellEffectFn(spell_hun_masters_call::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnEffectHitTarget += SpellEffectFn(spell_hun_masters_call::HandleScriptEffect, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 34477 - Misdirection
class spell_hun_misdirection : public AuraScript
{
    PrepareAuraScript(spell_hun_misdirection);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_MISDIRECTION_PROC });
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_DEFAULT || !GetTarget()->HasAura(SPELL_HUNTER_MISDIRECTION_PROC))
            GetTarget()->GetThreatMgr().UnregisterRedirectThreat(SPELL_HUNTER_MISDIRECTION);
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        // Do not trigger from Mend Pet
        if ((eventInfo.GetProcSpell() && (eventInfo.GetProcSpell()->GetSpellInfo()->SpellFamilyFlags[0] & 0x800000)) || (eventInfo.GetHealInfo() && (eventInfo.GetHealInfo()->GetSpellInfo()->SpellFamilyFlags[0] & 0x800000)))
            return false;

        return GetTarget()->GetThreatMgr().HasRedirects();
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(GetTarget(), SPELL_HUNTER_MISDIRECTION_PROC, true, nullptr, aurEff);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_hun_misdirection::OnRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        DoCheckProc += AuraCheckProcFn(spell_hun_misdirection::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_hun_misdirection::HandleProc, EFFECT_1, SPELL_AURA_DUMMY);
    }
};

// 35079 - Misdirection (Proc)
class spell_hun_misdirection_proc : public AuraScript
{
    PrepareAuraScript(spell_hun_misdirection_proc);

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->GetThreatMgr().UnregisterRedirectThreat(SPELL_HUNTER_MISDIRECTION);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_hun_misdirection_proc::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 54044 - Pet Carrion Feeder
class spell_hun_pet_carrion_feeder : public SpellScript
{
    PrepareSpellScript(spell_hun_pet_carrion_feeder);

    bool Load() override
    {
        return GetCaster()->IsPet();
    }

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_PET_CARRION_FEEDER_TRIGGERED });
    }

    SpellCastResult CheckIfCorpseNear()
    {
        Unit* caster = GetCaster();
        float max_range = GetSpellInfo()->GetMaxRange(false);
        WorldObject* result = nullptr;
        // search for nearby enemy corpse in range
        Acore::AnyDeadUnitSpellTargetInRangeCheck check(caster, max_range, GetSpellInfo(), TARGET_CHECK_ENEMY);
        Acore::WorldObjectSearcher<Acore::AnyDeadUnitSpellTargetInRangeCheck> searcher(caster, result, check);
        Cell::VisitObjects(caster, searcher, max_range);
        if (!result)
            return SPELL_FAILED_NO_EDIBLE_CORPSES;

        return SPELL_CAST_OK;
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        caster->CastSpell(caster, SPELL_HUNTER_PET_CARRION_FEEDER_TRIGGERED, false);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_hun_pet_carrion_feeder::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnCheckCast += SpellCheckCastFn(spell_hun_pet_carrion_feeder::CheckIfCorpseNear);
    }
};

// 55709 - Pet Heart of the Phoenix
class spell_hun_pet_heart_of_the_phoenix : public SpellScript
{
    PrepareSpellScript(spell_hun_pet_heart_of_the_phoenix);

    bool Load() override
    {
        return GetCaster()->IsPet();
    }

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_PET_HEART_OF_THE_PHOENIX_TRIGGERED, SPELL_HUNTER_PET_HEART_OF_THE_PHOENIX_DEBUFF });
    }

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();
        if (caster->IsAlive())
            return SPELL_FAILED_TARGET_NOT_DEAD;
        if (caster->HasAura(SPELL_HUNTER_PET_HEART_OF_THE_PHOENIX_DEBUFF))
            return SPELL_FAILED_CASTER_AURASTATE;
        return SPELL_CAST_OK;
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (Unit* owner = caster->GetOwner())
            if (!caster->HasAura(SPELL_HUNTER_PET_HEART_OF_THE_PHOENIX_DEBUFF))
            {
                owner->CastCustomSpell(SPELL_HUNTER_PET_HEART_OF_THE_PHOENIX_TRIGGERED, SPELLVALUE_BASE_POINT0, 100, caster, true);
                caster->CastSpell(caster, SPELL_HUNTER_PET_HEART_OF_THE_PHOENIX_DEBUFF, true);
            }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_hun_pet_heart_of_the_phoenix::CheckCast);
        OnEffectHitTarget += SpellEffectFn(spell_hun_pet_heart_of_the_phoenix::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

/*
 * Hunter pet scaling (CPP's spell_pet.cpp): the pet's share of the hunter's stats, through the five scaling
 * auras every hunter pet learns from its family skill. AzerothCore has no pet scaling update on the owner's
 * stat changes, so each aura recalculates every 2 seconds (as AzerothCore's spell_hun_generic_scaling did).
 */
static void UpdatePetScaling(AuraEffect const* aurEff)
{
    AuraEffect* effect = aurEff->GetBase()->GetEffect(aurEff->GetEffIndex());
    Unit* pet = aurEff->GetBase()->GetUnitOwner();
    if (!effect || !pet)
        return;

    // a new maximum health keeps the current health, as far as it fits
    uint32 const health = pet->GetHealth();
    effect->RecalculateAmount();
    if (aurEff->GetAuraType() == SPELL_AURA_230)
        pet->SetHealth(std::min<uint32>(pet->GetMaxHealth(), health));
}

static Player* GetHunterPetOwner(Unit* unit)
{
    Pet* pet = unit->ToPet();
    return pet ? pet->GetOwner() : nullptr;
}

// 34902 - Hunter Pet Scaling 01: health from the hunter's Stamina by pet type, attack power and spell damage
// from the hunter's ranged attack power
class spell_hun_pet_scaling_01 : public AuraScript
{
    PrepareAuraScript(spell_hun_pet_scaling_01);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_PET_FEROCITY_MARKER, SPELL_HUNTER_PET_TENACITY_MARKER, SPELL_HUNTER_PET_CUNNING_MARKER });
    }

    void CalculateHealthAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = true;
        Unit* pet = GetUnitOwner();
        if (Player* owner = GetHunterPetOwner(pet))
        {
            float mod = 0.0f;
            if (pet->HasAura(SPELL_HUNTER_PET_FEROCITY_MARKER))
                mod = 0.67f;
            else if (pet->HasAura(SPELL_HUNTER_PET_TENACITY_MARKER))
                mod = 0.78f;
            else if (pet->HasAura(SPELL_HUNTER_PET_CUNNING_MARKER))
                mod = 0.725f;

            amount = int32(owner->GetHealthBonusFromStamina() * mod);
        }
    }

    void CalculateAttackPowerAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = true;
        if (Player* owner = GetHunterPetOwner(GetUnitOwner()))
            amount = int32(owner->GetTotalAttackPowerValue(RANGED_ATTACK) * 0.22f);
    }

    // 12.87% of the hunter's ranged attack power as spell damage
    void CalculateDamageDoneAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = true;
        if (Player* owner = GetHunterPetOwner(GetUnitOwner()))
        {
            amount = int32(owner->GetTotalAttackPowerValue(RANGED_ATTACK) * 0.1287f);
            owner->SetUInt32Value(PLAYER_PET_SPELL_POWER, uint32(amount));
        }
    }

    void CalcPeriodic(AuraEffect const* /*aurEff*/, bool& isPeriodic, int32& amplitude)
    {
        isPeriodic = true;
        amplitude = 2 * IN_MILLISECONDS;
    }

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        PreventDefaultAction();
        UpdatePetScaling(aurEff);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hun_pet_scaling_01::CalculateHealthAmount, EFFECT_0, SPELL_AURA_230);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hun_pet_scaling_01::CalculateAttackPowerAmount, EFFECT_1, SPELL_AURA_MOD_ATTACK_POWER);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hun_pet_scaling_01::CalculateDamageDoneAmount, EFFECT_2, SPELL_AURA_MOD_DAMAGE_DONE);
        DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_hun_pet_scaling_01::CalcPeriodic, EFFECT_ALL, SPELL_AURA_ANY);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_hun_pet_scaling_01::HandlePeriodic, EFFECT_ALL, SPELL_AURA_ANY);
    }
};

// 34903 - Hunter Pet Scaling 02, 34904 - Hunter Pet Scaling 03: 40% of the hunter's resistances; the armor
// (34904's last effect) by pet type
class spell_hun_pet_scaling_resistance : public AuraScript
{
    PrepareAuraScript(spell_hun_pet_scaling_resistance);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_PET_FEROCITY_MARKER, SPELL_HUNTER_PET_TENACITY_MARKER, SPELL_HUNTER_PET_CUNNING_MARKER });
    }

    void CalculateResistanceAmount(AuraEffect const* aurEff, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = true;
        Unit* pet = GetUnitOwner();
        Player* owner = GetHunterPetOwner(pet);
        if (!owner)
            return;

        SpellSchoolMask schoolMask = SpellSchoolMask(GetSpellInfo()->Effects[aurEff->GetEffIndex()].MiscValue);
        if (schoolMask != SPELL_SCHOOL_MASK_NORMAL)
        {
            amount = int32(owner->GetResistance(schoolMask) * 0.4f);
            return;
        }

        float mod = 0.0f;
        if (pet->HasAura(SPELL_HUNTER_PET_FEROCITY_MARKER))
            mod = 0.5f;
        else if (pet->HasAura(SPELL_HUNTER_PET_TENACITY_MARKER))
            mod = 0.6f;
        else if (pet->HasAura(SPELL_HUNTER_PET_CUNNING_MARKER))
            mod = 0.7f;

        amount = int32(owner->GetArmor() * mod);
    }

    void CalcPeriodic(AuraEffect const* /*aurEff*/, bool& isPeriodic, int32& amplitude)
    {
        isPeriodic = true;
        amplitude = 2 * IN_MILLISECONDS;
    }

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        PreventDefaultAction();
        UpdatePetScaling(aurEff);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hun_pet_scaling_resistance::CalculateResistanceAmount, EFFECT_ALL, SPELL_AURA_MOD_RESISTANCE);
        DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_hun_pet_scaling_resistance::CalcPeriodic, EFFECT_ALL, SPELL_AURA_ANY);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_hun_pet_scaling_resistance::HandlePeriodic, EFFECT_ALL, SPELL_AURA_ANY);
    }
};

// 61017 - Hunter Pet Scaling 04: the hunter's hit and expertise
class spell_hun_pet_scaling_04 : public AuraScript
{
    PrepareAuraScript(spell_hun_pet_scaling_04);

    void CalculateMeleeHitAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = true;
        if (Player* owner = GetHunterPetOwner(GetUnitOwner()))
            amount = int32(owner->GetTotalAuraModifier(SPELL_AURA_MOD_HIT_CHANCE) + owner->GetRatingBonusValue(CR_HIT_MELEE));
    }

    void CalculateSpellHitAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = true;
        if (Player* owner = GetHunterPetOwner(GetUnitOwner()))
            amount = int32(owner->GetTotalAuraModifier(SPELL_AURA_MOD_SPELL_HIT_CHANCE) + owner->GetRatingBonusValue(CR_HIT_SPELL));
    }

    void CalculateExpertiseAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = true;
        if (Player* owner = GetHunterPetOwner(GetUnitOwner()))
            amount = int32(owner->GetTotalAuraModifier(SPELL_AURA_MOD_EXPERTISE) + owner->GetRatingBonusValue(CR_EXPERTISE));
    }

    void CalcPeriodic(AuraEffect const* /*aurEff*/, bool& isPeriodic, int32& amplitude)
    {
        isPeriodic = true;
        amplitude = 2 * IN_MILLISECONDS;
    }

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        PreventDefaultAction();
        UpdatePetScaling(aurEff);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hun_pet_scaling_04::CalculateMeleeHitAmount, EFFECT_0, SPELL_AURA_MOD_HIT_CHANCE);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hun_pet_scaling_04::CalculateSpellHitAmount, EFFECT_1, SPELL_AURA_MOD_SPELL_HIT_CHANCE);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hun_pet_scaling_04::CalculateExpertiseAmount, EFFECT_2, SPELL_AURA_MOD_EXPERTISE);
        DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_hun_pet_scaling_04::CalcPeriodic, EFFECT_ALL, SPELL_AURA_ANY);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_hun_pet_scaling_04::HandlePeriodic, EFFECT_ALL, SPELL_AURA_ANY);
    }
};

// 89446 - Hunter Pet Scaling 05: the hunter's ranged crit chance and melee haste (CPP put the haste into the
// crit effect too, its crit function was never registered)
class spell_hun_pet_scaling_05 : public AuraScript
{
    PrepareAuraScript(spell_hun_pet_scaling_05);

    void CalculateCritAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = true;
        if (Player* owner = GetHunterPetOwner(GetUnitOwner()))
            amount = int32(owner->GetFloatValue(PLAYER_RANGED_CRIT_PERCENTAGE));
    }

    void CalculateHasteAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = true;
        if (Player* owner = GetHunterPetOwner(GetUnitOwner()))
            amount = int32((1.0f - owner->m_modAttackSpeedPct[BASE_ATTACK]) * 100.0f);
    }

    void CalcPeriodic(AuraEffect const* /*aurEff*/, bool& isPeriodic, int32& amplitude)
    {
        isPeriodic = true;
        amplitude = 2 * IN_MILLISECONDS;
    }

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        PreventDefaultAction();
        UpdatePetScaling(aurEff);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hun_pet_scaling_05::CalculateCritAmount, EFFECT_0, SPELL_AURA_MOD_CRIT_PCT);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hun_pet_scaling_05::CalculateHasteAmount, EFFECT_1, SPELL_AURA_MELEE_SLOW);
        DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_hun_pet_scaling_05::CalcPeriodic, EFFECT_ALL, SPELL_AURA_ANY);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_hun_pet_scaling_05::HandlePeriodic, EFFECT_ALL, SPELL_AURA_ANY);
    }
};

// -53234 - Piercing Shots
class spell_hun_piercing_shots : public AuraScript
{
    PrepareAuraScript(spell_hun_piercing_shots);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_PIERCING_SHOTS });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetActionTarget() != nullptr;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        DamageInfo* dmgInfo = eventInfo.GetDamageInfo();
        if (!dmgInfo || !dmgInfo->GetDamage())
            return;

        Unit* caster = eventInfo.GetActor();
        Unit* target = eventInfo.GetActionTarget();

        SpellInfo const* piercingShots = sSpellMgr->AssertSpellInfo(SPELL_HUNTER_PIERCING_SHOTS);
        int32 bp = CalculatePct(static_cast<int32>(dmgInfo->GetDamage()), aurEff->GetAmount());
        bp /= std::max<int32>(piercingShots->GetMaxTicks(), 1);

        target->CastDelayedSpellWithPeriodicAmount(caster, SPELL_HUNTER_PIERCING_SHOTS, SPELL_AURA_PERIODIC_DAMAGE, bp);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_hun_piercing_shots::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_hun_piercing_shots::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// -53228 - Rapid Recuperation
// Its haste part only from Rapid Fire; its focus part only from Rapid Killing.
class spell_hun_rapid_recuperation : public AuraScript
{
    PrepareAuraScript(spell_hun_rapid_recuperation);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_RAPID_RECUPERATION });
    }

    void HandleAbilityCast(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo || !(spellInfo->SpellFamilyFlags[0] & 0x20))
            PreventDefaultAction();
    }

    void HandleFocusRegen(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo || !(spellInfo->SpellFamilyFlags[1] & 0x01000000))
            return;

        int32 focus = aurEff->GetAmount();
        GetTarget()->CastCustomSpell(GetTarget(), SPELL_HUNTER_RAPID_RECUPERATION, &focus, nullptr, nullptr, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_hun_rapid_recuperation::HandleAbilityCast, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
        OnEffectProc += AuraEffectProcFn(spell_hun_rapid_recuperation::HandleFocusRegen, EFFECT_1, SPELL_AURA_DUMMY);
    }
};

// 23989 - Readiness
class spell_hun_readiness : public SpellScript
{
    PrepareSpellScript(spell_hun_readiness);

    bool Load() override
    {
        return GetCaster()->IsPlayer();
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Player* caster = GetCaster()->ToPlayer();
        // immediately finishes the cooldown on your other Hunter abilities except Bestial Wrath
        SpellCooldowns& cooldowns = caster->GetSpellCooldownMap();

        std::set<std::pair<uint32, bool>> spellsToRemove;
        std::set<uint32> categoriesToRemove;

        for (auto const& [spellId, cooldown] : cooldowns)
        {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
            if (spellInfo
            && spellInfo->SpellFamilyName == SPELLFAMILY_HUNTER
            && spellInfo->Id != SPELL_HUNTER_READINESS
            && spellInfo->Id != SPELL_HUNTER_BESTIAL_WRATH
            && spellInfo->Id != SPELL_DRAENEI_GIFT_OF_THE_NAARU)
            {
                if (spellInfo->RecoveryTime > 0)
                    spellsToRemove.insert(std::make_pair(spellInfo->Id, cooldown.needSendToClient));

                if (spellInfo->CategoryRecoveryTime > 0)
                    categoriesToRemove.insert(spellInfo->GetCategory());
            }
        }

        // we can't remove spell cooldowns while iterating.
        for (auto const& [spellId, sendToClient] : spellsToRemove)
            caster->RemoveSpellCooldown(spellId, sendToClient);
        for (auto const& category : categoriesToRemove)
            caster->RemoveCategoryCooldown(category);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_hun_readiness::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 82925 - Ready, Set, Aim...
// At five stacks: Fire! (the next Aimed Shot instant).
class spell_hun_ready_set_aim : public AuraScript
{
    PrepareAuraScript(spell_hun_ready_set_aim);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_FIRE });
    }

    void AfterApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (GetStackAmount() == GetSpellInfo()->StackAmount)
        {
            GetTarget()->CastSpell(GetTarget(), SPELL_HUNTER_FIRE, true, nullptr, aurEff);
            Remove();
        }
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_hun_ready_set_aim::AfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }
};

// 37506 - Scatter Shot
class spell_hun_scatter_shot : public SpellScript
{
    PrepareSpellScript(spell_hun_scatter_shot);

    bool Load() override
    {
        return GetCaster()->IsPlayer();
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Player* caster = GetCaster()->ToPlayer();
        // break Auto Shot and autohit
        caster->InterruptSpell(CURRENT_AUTOREPEAT_SPELL);
        caster->AttackStop();
        caster->SendAttackSwingCancelAttack();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_hun_scatter_shot::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 1978 - Serpent Sting, 88453, 88466 - Serpent Sting (Serpent Spread)
// Improved Serpent Sting: the talent's share of the whole sting at once. The attack power share of the ticks
// is a spell_bonus_data row.
class spell_hun_serpent_sting : public AuraScript
{
    PrepareAuraScript(spell_hun_serpent_sting);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_IMPROVED_SERPENT_STING_DAMAGE });
    }

    void HandleImprovedSerpentSting(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        if (AuraEffect const* improved = caster->GetDummyAuraEffect(SPELLFAMILY_HUNTER, HUNTER_ICON_ID_IMPROVED_SERPENT_STING, EFFECT_0))
        {
            // the whole sting with its attack power share (own; CPP took the base tick only)
            uint32 tick = caster->SpellDamageBonusDone(GetTarget(), GetSpellInfo(), uint32(std::max(aurEff->GetAmount(), 0)), DOT, aurEff->GetEffIndex());
            int32 damage = CalculatePct(int32(tick) * aurEff->GetTotalTicks(), improved->GetAmount());
            if (damage > 0)
                caster->CastCustomSpell(GetTarget(), SPELL_HUNTER_IMPROVED_SERPENT_STING_DAMAGE, &damage, nullptr, nullptr, true, nullptr, improved);
        }
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_hun_serpent_sting::HandleImprovedSerpentSting, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }
};

// 2643 - Multi-Shot
// Serpent Spread: the targets hit get the talent rank's Serpent Sting. The talent's own effect is a dummy
// effect, not an aura, so nothing can proc from it (CPP hooked an aura proc that never ran). Ours.
class spell_hun_multi_shot : public SpellScript
{
    PrepareSpellScript(spell_hun_multi_shot);

    static constexpr uint32 SPELL_HUNTER_SERPENT_SPREAD_R1 = 87934;
    static constexpr uint32 SPELL_HUNTER_SERPENT_SPREAD_R2 = 87935;

    bool Load() override
    {
        return GetCaster()->IsPlayer();
    }

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_SERPENT_SPREAD_STING_R1, SPELL_HUNTER_SERPENT_SPREAD_STING_R2 });
    }

    void HandleSerpentSpread(SpellEffIndex /*effIndex*/)
    {
        Player* player = GetCaster()->ToPlayer();
        uint8 const spec = player->GetActiveSpec();
        uint32 spellId = 0;
        if (player->HasTalent(SPELL_HUNTER_SERPENT_SPREAD_R2, spec))
            spellId = SPELL_HUNTER_SERPENT_SPREAD_STING_R2;
        else if (player->HasTalent(SPELL_HUNTER_SERPENT_SPREAD_R1, spec))
            spellId = SPELL_HUNTER_SERPENT_SPREAD_STING_R1;

        if (spellId)
            player->CastSpell(GetHitUnit(), spellId, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_hun_multi_shot::HandleSerpentSpread, EFFECT_1, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
    }
};

// -83340 - Sic 'Em!
// The pet's next Basic Attack costs less. The rank's buff goes on the hunter, as its data says (self target): a
// pet's spell costs take the owner's spell modifiers, and a modifier aura on the pet itself does nothing.
class spell_hun_sic_em : public AuraScript
{
    PrepareAuraScript(spell_hun_sic_em);

    bool Load() override
    {
        return GetUnitOwner()->IsPlayer();
    }

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_SIC_EM_R1, SPELL_HUNTER_SIC_EM_R2 });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(GetTarget(), GetSpellInfo()->GetRank() > 1 ? SPELL_HUNTER_SIC_EM_R2 : SPELL_HUNTER_SIC_EM_R1, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_hun_sic_em::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -53302 - Sniper Training
class spell_hun_sniper_training : public AuraScript
{
    PrepareAuraScript(spell_hun_sniper_training);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_SNIPER_TRAINING_R1, SPELL_HUNTER_SNIPER_TRAINING_BUFF_R1 });
    }

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        PreventDefaultAction();
        if (aurEff->GetAmount() <= 0)
        {
            Unit* target = GetTarget();
            uint32 spellId = SPELL_HUNTER_SNIPER_TRAINING_BUFF_R1 + GetSpellInfo()->GetRank() - 1;
            target->CastSpell(target, spellId, true, nullptr, aurEff);

            if (Player* playerTarget = GetUnitOwner()->ToPlayer())
            {
                int32 baseAmount = aurEff->GetBaseAmount();
                int32 amount = playerTarget->CalculateSpellDamage(playerTarget, GetSpellInfo(), aurEff->GetEffIndex(), &baseAmount);
                GetEffect(EFFECT_0)->SetAmount(amount);
            }
        }
    }

    void HandleUpdatePeriodic(AuraEffect* aurEff)
    {
        if (Player* playerTarget = GetUnitOwner()->ToPlayer())
        {
            int32 baseAmount = aurEff->GetBaseAmount();
            int32 amount = playerTarget->isMoving() ?
                            playerTarget->CalculateSpellDamage(playerTarget, GetSpellInfo(), aurEff->GetEffIndex(), &baseAmount) :
                            aurEff->GetAmount() - 1;
            aurEff->SetAmount(amount);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_hun_sniper_training::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_hun_sniper_training::HandleUpdatePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// 56641 - Steady Shot
// Its focus (the damage, with its attack power share, is the weapon damage effects and spell_bonus_data).
class spell_hun_steady_shot : public SpellScript
{
    PrepareSpellScript(spell_hun_steady_shot);

    bool Load() override
    {
        return GetCaster()->IsPlayer();
    }

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_STEADY_SHOT_FOCUS });
    }

    void HandleLaunch(SpellEffIndex /*effIndex*/)
    {
        CastShotFocus(GetCaster(), GetExplTargetUnit(), SPELL_HUNTER_STEADY_SHOT_FOCUS);
    }

    void Register() override
    {
        OnEffectLaunch += SpellEffectFn(spell_hun_steady_shot::HandleLaunch, EFFECT_0, SPELL_EFFECT_NORMALIZED_WEAPON_DMG);
    }
};

// 67151 - Item - Hunter T9 4P Bonus (Steady Shot)
class spell_hun_t9_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_hun_t9_4p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_T9_4P_GREATNESS });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Unit* actor = eventInfo.GetActor();
        return actor && actor->IsPlayer() && actor->ToPlayer()->GetPet();
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* caster = eventInfo.GetActor();
        caster->CastSpell(caster->ToPlayer()->GetPet(), SPELL_HUNTER_T9_4P_GREATNESS, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_hun_t9_4p_bonus::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_hun_t9_4p_bonus::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 1515 - Tame Beast
// 4.3.4 checks from CPP: the tamed pet needs a free active slot whose Call Pet the hunter knows.
class spell_hun_tame_beast : public SpellScript
{
    PrepareSpellScript(spell_hun_tame_beast);

    SpellCastResult SendTameFailure(PetTameFailure reason)
    {
        GetCaster()->ToPlayer()->SendTameFailure(reason);
        return SPELL_FAILED_DONT_REPORT;
    }

    SpellCastResult CheckCast()
    {
        Player* player = GetCaster()->ToPlayer();
        if (!player)
            return SPELL_FAILED_DONT_REPORT;

        if (!GetExplTargetUnit())
            return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

        if (!player->IsClass(CLASS_HUNTER, CLASS_CONTEXT_PET))
            return SendTameFailure(PET_TAME_UNITS_CANT_TAME);

        PetStable const* petStable = player->GetPetStable();
        Optional<uint8> freeSlot = PET_SLOT_FIRST_ACTIVE_SLOT;
        if (petStable)
            freeSlot = petStable->GetFirstFreeActiveSlot();
        if (!freeSlot)
            return SendTameFailure(PET_TAME_TOO_MANY);

        if (!player->HasSpell(CallPetSpellIdBySlot[*freeSlot]))
            return SendTameFailure(PET_TAME_SLOT_LOCKED);

        Creature* target = GetExplTargetUnit()->ToCreature();
        if (!target)
            return SendTameFailure(PET_TAME_INVALID_CREATURE);

        if (target->GetLevel() > player->GetLevel())
            return SendTameFailure(PET_TAME_TOO_HIGHLEVEL);

        if (target->GetCreatureTemplate()->IsExotic() && !player->CanTameExoticPets())
            return SendTameFailure(PET_TAME_CANT_CONTROL_EXOTIC);

        if (!target->GetCreatureTemplate()->IsTameable(player->CanTameExoticPets()))
            return SendTameFailure(PET_TAME_NOT_TAMEABLE);

        // a pet out, or one only unsummoned for a moment (mounted)
        if (player->GetPetGUID() || (petStable && petStable->CurrentPet))
            return SendTameFailure(PET_TAME_ANOTHER_SUMMON_ACTIVE);

        if (player->GetCharmGUID())
            return SendTameFailure(PET_TAME_ANOTHER_SUMMON_ACTIVE);

        if (target->GetOwnerGUID())
            return SendTameFailure(PET_TAME_CREATURE_ALREADY_OWNED);

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_hun_tame_beast::CheckCast);
    }
};

// Taming the Beast quests (despawn creature after dismiss)
class spell_hun_taming_the_beast : public AuraScript
{
    PrepareAuraScript(spell_hun_taming_the_beast);

    void HandleOnEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* creature = GetTarget()->ToCreature())
        {
            creature->GetThreatMgr().ClearAllThreat();
            creature->GetCombatManager().EndAllCombat();
        }
    }

    void HandleOnEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* creature = GetTarget()->ToCreature())
            creature->DespawnOrUnsummon(1ms);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_hun_taming_the_beast::HandleOnEffectApply, EFFECT_0, SPELL_AURA_MOD_CHARM, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_hun_taming_the_beast::HandleOnEffectRemove, EFFECT_0, SPELL_AURA_MOD_CHARM, AURA_EFFECT_HANDLE_REAL);
    }
};

// 53434 - Call of the Wild
class spell_hun_target_only_pet_and_owner : public SpellScript
{
    PrepareSpellScript(spell_hun_target_only_pet_and_owner);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.clear();
        targets.push_back(GetCaster());
        if (Unit* owner = GetCaster()->GetOwner())
            targets.push_back(owner);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hun_target_only_pet_and_owner::FilterTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_PARTY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hun_target_only_pet_and_owner::FilterTargets, EFFECT_1, TARGET_UNIT_CASTER_AREA_PARTY);
    }
};

// -34497 - Thrill of the Hunt
// A share of the shot's focus cost back.
class spell_hun_thrill_of_the_hunt : public AuraScript
{
    PrepareAuraScript(spell_hun_thrill_of_the_hunt);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_THRILL_OF_THE_HUNT });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo)
            return;

        // "of the base Focus cost": the shot's own cost, also when Lock and Load made it free
        int32 baseCost = spellInfo->ManaCost ? int32(spellInfo->ManaCost) : spellInfo->CalcPowerCost(GetTarget(), spellInfo->GetSchoolMask());
        int32 focus = CalculatePct(baseCost, aurEff->GetAmount());
        if (focus > 0)
            GetTarget()->CastCustomSpell(GetTarget(), SPELL_HUNTER_THRILL_OF_THE_HUNT, &focus, nullptr, nullptr, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_hun_thrill_of_the_hunt::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -56333 - T.N.T.
class spell_hun_tnt : public AuraScript
{
    PrepareAuraScript(spell_hun_tnt);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_LOCK_AND_LOAD });
    }

    bool CheckProc(ProcEventInfo& /*eventInfo*/)
    {
        return roll_chance_i(GetEffect(EFFECT_0)->GetAmount());
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(GetTarget(), SPELL_HUNTER_LOCK_AND_LOAD, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_hun_tnt::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_hun_tnt::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 77769 - Trap Launcher
// The linked aura while the traps can be thrown (CPP cast Improved Steady Shot's buff here instead).
class spell_hun_trap_launcher : public AuraScript
{
    PrepareAuraScript(spell_hun_trap_launcher);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_TRAP_LAUNCHER_LINKED });
    }

    void AfterApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->CastSpell(GetTarget(), SPELL_HUNTER_TRAP_LAUNCHER_LINKED, true, nullptr, aurEff);
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_HUNTER_TRAP_LAUNCHER_LINKED);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_hun_trap_launcher::AfterApply, EFFECT_0, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS_TRIGGERED, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_hun_trap_launcher::AfterRemove, EFFECT_0, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS_TRIGGERED, AURA_EFFECT_HANDLE_REAL);
    }
};

// 76659 - Wild Quiver (marksmanship mastery)
class spell_hun_wild_quiver : public AuraScript
{
    PrepareAuraScript(spell_hun_wild_quiver);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_WILD_QUIVER_DAMAGE });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        // no extra shot from its own shots
        if (SpellInfo const* spellInfo = eventInfo.GetSpellInfo())
            if (spellInfo->Id == SPELL_HUNTER_WILD_QUIVER_DAMAGE)
                return false;

        return roll_chance_i(GetEffect(EFFECT_0)->GetAmount());
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        if (Unit* target = eventInfo.GetActionTarget())
            GetTarget()->CastSpell(target, SPELL_HUNTER_WILD_QUIVER_DAMAGE, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_hun_wild_quiver::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_hun_wild_quiver::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -19386 - Wyvern Sting
class spell_hun_wyvern_sting : public AuraScript
{
    PrepareAuraScript(spell_hun_wyvern_sting)

    void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(GetTarget(), sSpellMgr->GetSpellWithRank(SPELL_HUNTER_WYVERN_STING_DOT, GetSpellInfo()->GetRank()), true);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_hun_wyvern_sting::HandleEffectRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
    }
};

void AddSC_hunter_spell_scripts()
{
    RegisterSpellScript(spell_hun_ancient_hysteria);
    RegisterSpellScript(spell_hun_bestial_wrath);
    RegisterSpellScript(spell_hun_camouflage);
    RegisterSpellScript(spell_hun_camouflage_duration);
    RegisterSpellScript(spell_hun_camouflage_triggered);
    RegisterSpellScript(spell_hun_check_pet_los);
    RegisterSpellScript(spell_hun_chimera_shot);
    RegisterSpellScript(spell_hun_cobra_shot);
    RegisterSpellScript(spell_hun_cobra_strikes);
    RegisterSpellScript(spell_hun_cobra_strikes_triggered);
    RegisterSpellScript(spell_hun_cower);
    RegisterSpellScript(spell_hun_crouching_tiger_hidden_chimera);
    RegisterSpellScript(spell_hun_disengage);
    RegisterSpellScript(spell_hun_fervor);
    RegisterSpellAndAuraScriptPair(spell_hun_focus_fire, spell_hun_focus_fire_aura);
    RegisterSpellScript(spell_hun_frenzy_effect);
    RegisterSpellScript(spell_hun_glyph_of_kill_shot);
    RegisterSpellScript(spell_hun_improved_mend_pet);
    RegisterSpellScript(spell_hun_improved_steady_shot);
    RegisterSpellScript(spell_hun_intimidation);
    RegisterSpellScript(spell_hun_invigoration);
    RegisterSpellScript(spell_hun_kill_command);
    RegisterSpellScript(spell_hun_killing_streak);
    RegisterSpellScript(spell_hun_last_stand_pet);
    RegisterSpellScript(spell_hun_lock_and_load);
    RegisterSpellScript(spell_hun_marked_for_death);
    RegisterSpellScript(spell_hun_masters_call);
    RegisterSpellScript(spell_hun_misdirection);
    RegisterSpellScript(spell_hun_misdirection_proc);
    RegisterSpellScript(spell_hun_multi_shot);
    RegisterSpellScript(spell_hun_pet_carrion_feeder);
    RegisterSpellScript(spell_hun_pet_heart_of_the_phoenix);
    RegisterSpellScript(spell_hun_pet_scaling_01);
    RegisterSpellScript(spell_hun_pet_scaling_04);
    RegisterSpellScript(spell_hun_pet_scaling_05);
    RegisterSpellScript(spell_hun_pet_scaling_resistance);
    RegisterSpellScript(spell_hun_piercing_shots);
    RegisterSpellScript(spell_hun_rapid_recuperation);
    RegisterSpellScript(spell_hun_readiness);
    RegisterSpellScript(spell_hun_ready_set_aim);
    RegisterSpellScript(spell_hun_resistance_is_futile);
    RegisterSpellScript(spell_hun_resistance_is_futile_buff);
    RegisterSpellScript(spell_hun_scatter_shot);
    RegisterSpellScript(spell_hun_serpent_sting);
    RegisterSpellScript(spell_hun_sic_em);
    RegisterSpellScript(spell_hun_sniper_training);
    RegisterSpellScript(spell_hun_steady_shot);
    RegisterSpellScript(spell_hun_t9_4p_bonus);
    RegisterSpellScript(spell_hun_tame_beast);
    RegisterSpellScript(spell_hun_taming_the_beast);
    RegisterSpellScript(spell_hun_target_only_pet_and_owner);
    RegisterSpellScript(spell_hun_thrill_of_the_hunt);
    RegisterSpellScript(spell_hun_tnt);
    RegisterSpellScript(spell_hun_trap_launcher);
    RegisterSpellScript(spell_hun_wild_quiver);
    RegisterSpellScript(spell_hun_wyvern_sting);
}
