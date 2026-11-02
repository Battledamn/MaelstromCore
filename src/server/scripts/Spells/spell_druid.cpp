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

#include "Containers.h"
#include "DynamicObject.h"
#include "GridNotifiers.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "Spell.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "SpellScriptLoader.h"
#include "TemporarySummon.h"
#include <mutex>
/*
 * Scripts for spells with SPELLFAMILY_DRUID and SPELLFAMILY_GENERIC spells used by druid players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_dru_".
 */

enum DruidSpells
{
    SPELL_DRUID_ASTRAL_ALIGNMENT            = 90164,
    SPELL_DRUID_BARKSKIN                    = 22812,
    SPELL_DRUID_BLOOD_IN_THE_WATER_SCRIPT   = 80863,
    SPELL_DRUID_BLOOM                       = 90159,
    SPELL_DRUID_BRUTAL_IMPACT_R1            = 16940,
    SPELL_DRUID_EFFLORESCENCE_AOE           = 81262,
    SPELL_DRUID_EFFLORESCENCE_HEAL          = 81269,
    SPELL_DRUID_EMPOWERED_TOUCH_SCRIPT      = 88433,
    SPELL_DRUID_ENRAGE_MOD_DAMAGE           = 51185,
    SPELL_DRUID_ENRAGED_DEFENSE             = 70725,
    SPELL_DRUID_FERAL_CHARGE_BEAR           = 16979,
    SPELL_DRUID_FERAL_CHARGE_CAT            = 49376,
    SPELL_DRUID_FERAL_SWIFTNESS_R1          = 17002,
    SPELL_DRUID_FERAL_SWIFTNESS_R2          = 24866,
    SPELL_DRUID_FERAL_SWIFTNESS_PASSIVE_1   = 24867,
    SPELL_DRUID_FERAL_SWIFTNESS_PASSIVE_2   = 24864,
    SPELL_DRUID_FERAL_SWIFTNESS_CLEAR_ROAR  = 97993,
    SPELL_DRUID_FERAL_SWIFTNESS_CLEAR_CAT   = 97985,
    SPELL_DRUID_FIREBLOOM                   = 99017,
    SPELL_DRUID_FRENZIED_REGENERATION_HEAL  = 22845,
    SPELL_DRUID_FUNGAL_GROWTH_R1            = 78788,
    SPELL_DRUID_FUNGAL_GROWTH_R2            = 78789,
    SPELL_DRUID_FUNGAL_GROWTH_SUMMON_R1     = 81291,
    SPELL_DRUID_FUNGAL_GROWTH_SUMMON_R2     = 81283,
    SPELL_DRUID_GLYPH_OF_BARKSKIN_TRIGGER   = 63058,
    SPELL_DRUID_GLYPH_OF_FEROCIOUS_BITE     = 101024,
    SPELL_DRUID_GLYPH_OF_FRENZIED_REGEN     = 54810,
    SPELL_DRUID_GLYPH_OF_INNERVATE_MANA     = 54833,
    SPELL_DRUID_GLYPH_OF_STARFIRE           = 54846,
    SPELL_DRUID_GLYPH_OF_TYPHOON            = 62135,
    SPELL_DRUID_HARMONY                     = 100977,
    SPELL_DRUID_IDOL_OF_FERAL_SHADOWS       = 34241,
    SPELL_DRUID_IDOL_OF_WORSHIP             = 60774,
    SPELL_DRUID_INCREASED_MOONFIRE_DURATION = 38414,
    SPELL_DRUID_ITEM_T8_BALANCE_RELIC       = 64950,
    SPELL_DRUID_ITEM_T10_FERAL_4P_BONUS     = 70726,
    SPELL_DRUID_KING_OF_THE_JUNGLE          = 48492,
    SPELL_DRUID_LACERATE                    = 33745,
    SPELL_DRUID_LEADER_OF_THE_PACK_HEAL     = 34299,
    SPELL_DRUID_LEADER_OF_THE_PACK_MANA     = 68285,
    SPELL_DRUID_LIFEBLOOM                   = 33763,
    SPELL_DRUID_LIFEBLOOM_FINAL_HEAL        = 33778,
    SPELL_DRUID_LIVING_SEED_HEAL            = 48503,
    SPELL_DRUID_LIVING_SEED_PROC            = 48504,
    SPELL_DRUID_MAIM_INTERRUPT              = 32747,
    SPELL_DRUID_MOONGLADE_2P_BONUS          = 37286,
    SPELL_DRUID_NATURES_BOUNTY              = 96206,
    SPELL_DRUID_NURTURING_INSTINCT_R1       = 47179,
    SPELL_DRUID_NURTURING_INSTINCT_R2       = 47180,
    SPELL_DRUID_PULVERIZE_TRIGGERED         = 80951,
    SPELL_DRUID_REGROWTH_REFRESH            = 93036,
    SPELL_DRUID_REJUVENATION                = 774,
    SPELL_DRUID_REJUVENATION_DIRECT_HEAL    = 64801,
    SPELL_DRUID_REPLENISHMENT               = 57669,
    SPELL_DRUID_REVITALIZE_R1               = 48539,
    SPELL_DRUID_RIP                         = 1079,
    SPELL_DRUID_RIP_DURATION_LACERATE_DMG   = 60141,
    SPELL_DRUID_SAVAGE_DEFENDER             = 77494, // guardian mastery
    SPELL_DRUID_SAVAGE_ROAR                 = 62071,
    SPELL_DRUID_SHADOWMELD                  = 58984,
    SPELL_DRUID_SKULL_BASH_CHARGE           = 93983,
    SPELL_DRUID_SKULL_BASH_INTERRUPT        = 93985,
    SPELL_DRUID_SKULL_BASH_COST_INCREASE_R1 = 82364,
    SPELL_DRUID_SKULL_BASH_COST_INCREASE_R2 = 82365,
    SPELL_DRUID_SOLAR_BEAM_SILENCE          = 81261,
    SPELL_DRUID_STAMPEDE_R1                 = 78892,
    SPELL_DRUID_STAMPEDE_R2                 = 78893,
    SPELL_DRUID_STAMPEDE_BEAR_R1            = 81016,
    SPELL_DRUID_STAMPEDE_BEAR_R2            = 81017,
    SPELL_DRUID_STAMPEDE_CAT_R1             = 81021,
    SPELL_DRUID_STAMPEDE_CAT_R2             = 81022,
    SPELL_DRUID_STAMPEDE_RAVAGE_MARKER      = 109881,
    SPELL_DRUID_STARSURGE                   = 78674,
    SPELL_DRUID_STRENGTH_OF_THE_PANTHER     = 90166,
    SPELL_DRUID_SURVIVAL_INSTINCTS          = 50322,
    SPELL_DRUID_T13_FERAL_2P_BONUS          = 105725,
    SPELL_DRUID_TIGER_S_FURY_ENERGIZE       = 51178,
    SPELL_DRUID_TREE_OF_LIFE                = 33891,
    SPELL_DRUID_TREE_OF_LIFE_PASSIVE_1      = 5420,
    SPELL_DRUID_TREE_OF_LIFE_PASSIVE_2      = 81097,
    SPELL_DRUID_TREE_OF_LIFE_PASSIVE_3      = 81098,
    SPELL_DRUID_VENGEANCE                   = 76691,
    SPELL_DRUID_WILD_MUSHROOM               = 88747,
    SPELL_DRUID_WILD_MUSHROOM_DAMAGE        = 78777,
    SPELL_DRUID_WILD_MUSHROOM_SUICIDE       = 92853,
    SPELL_DRUID_WILD_MUSHROOM_VISUAL        = 92701,
    SPELL_DRUID_ECLIPSE_LUNAR               = 48518,
    SPELL_DRUID_ECLIPSE_SOLAR               = 48517,
    // Item set and relic bonuses
    SPELL_DRUID_T3_PROC_ENERGIZE_MANA       = 28722,
    SPELL_DRUID_T3_PROC_ENERGIZE_RAGE       = 28723,
    SPELL_DRUID_T3_PROC_ENERGIZE_ENERGY     = 28724,
    SPELL_DRUID_BLESSING_OF_THE_CLAW        = 28750,
    SPELL_DRUID_EXHILARATE                  = 28742,
    SPELL_DRUID_INFUSION                    = 37238,
    SPELL_DRUID_BLESSING_OF_REMULOS         = 40445,
    SPELL_DRUID_BLESSING_OF_ELUNE           = 40446,
    SPELL_DRUID_BLESSING_OF_CENARIUS        = 40452,
    SPELL_DRUID_REJUVENATION_T10_PROC       = 70691,
    SPELL_DRUID_BALANCE_T10_BONUS           = 70718,
    SPELL_DRUID_BALANCE_T10_BONUS_PROC      = 70721,
    SPELL_DRUID_LANGUISH                    = 71023,
    SPELL_DRUID_T9_FERAL_RELIC_BEAR         = 67354,
    SPELL_DRUID_T9_FERAL_RELIC_CAT          = 67355,
    SPELL_DRUID_RESTORATION_T10_2P_BONUS    = 70658
};

enum DruidIcons
{
    SPELL_ICON_REVITALIZE                   = 2862,
    SPELL_ICON_ID_GLYPH_OF_REGROWTH         = 197,
    SPELL_ICON_ID_DREAMSTATE                = 2255,
    SPELL_ICON_ID_GLYPH_OF_INNERVATE        = 62,
    SPELL_ICON_ID_GLYPH_OF_FEROCIOUS_BITE   = 1680,
    SPELL_ICON_ID_GIFT_OF_THE_EARTHMOTHER   = 3186
};

enum DruidMisc
{
    SPELL_CATEGORY_MANGLE_BEAR              = 971
};

// 50334 - Berserk
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): in 4.3.4 Berserk no longer
// removes Tiger's Fury; its effect 1 takes Mangle (Bear)'s cooldown away, so the cooldown running is reset.
class spell_dru_berserk : public AuraScript
{
    PrepareAuraScript(spell_dru_berserk);

    void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* player = GetTarget()->ToPlayer())
            player->RemoveCategoryCooldown(SPELL_CATEGORY_MANGLE_BEAR);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_dru_berserk::HandleEffectApply, EFFECT_1, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
    }
};

// 70723 - Item - Druid T10 Balance 4P Bonus
class spell_dru_t10_balance_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_dru_t10_balance_4p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_LANGUISH });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetDamage())
            return;

        SpellInfo const* triggeredSpell = sSpellMgr->GetSpellInfo(SPELL_DRUID_LANGUISH);

        int32 amount = CalculatePct(static_cast<int32>(damageInfo->GetDamage()), aurEff->GetAmount()) / triggeredSpell->GetMaxTicks();
        eventInfo.GetProcTarget()->CastDelayedSpellWithPeriodicAmount(GetTarget(), SPELL_DRUID_LANGUISH, SPELL_AURA_PERIODIC_DAMAGE, amount, EFFECT_0);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_t10_balance_4p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -33872 - Nurturing Instinct
class spell_dru_nurturing_instinct : public AuraScript
{
    PrepareAuraScript(spell_dru_nurturing_instinct);

    void AfterApply(AuraEffect const*  /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* player = GetTarget()->ToPlayer())
            player->addSpell(GetSpellInfo()->GetRank() == 1 ? SPELL_DRUID_NURTURING_INSTINCT_R1 : SPELL_DRUID_NURTURING_INSTINCT_R2, SPEC_MASK_ALL, false, true);
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* player = GetTarget()->ToPlayer())
            player->removeSpell(GetSpellInfo()->GetRank() == 1 ? SPELL_DRUID_NURTURING_INSTINCT_R1 : SPELL_DRUID_NURTURING_INSTINCT_R2, SPEC_MASK_ALL, true);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_dru_nurturing_instinct::AfterApply, EFFECT_0, SPELL_AURA_MOD_SPELL_HEALING_OF_STAT_PERCENT, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_dru_nurturing_instinct::AfterRemove, EFFECT_0, SPELL_AURA_MOD_SPELL_HEALING_OF_STAT_PERCENT, AURA_EFFECT_HANDLE_REAL);
    }
};

// 768 - Cat Form
// 5487 - Bear Form
// Feral Swiftness still grants dodge in Cat and Bear Form in 4.3.4 (24867, 24864); the speed and the
// root and snare removal come from the talent's own data and from Dash and Stampeding Roar.
class spell_dru_feral_swiftness : public AuraScript
{
    PrepareAuraScript(spell_dru_feral_swiftness);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_FERAL_SWIFTNESS_PASSIVE_1, SPELL_DRUID_FERAL_SWIFTNESS_PASSIVE_2 });
    }

    void AfterApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (Player* player = GetTarget()->ToPlayer())
            if (uint8 rank = player->HasTalent(SPELL_DRUID_FERAL_SWIFTNESS_R1, player->GetActiveSpec()) ? 1 : (player->HasTalent(SPELL_DRUID_FERAL_SWIFTNESS_R2, player->GetActiveSpec()) ? 2 : 0))
                player->CastSpell(player, rank == 1 ? SPELL_DRUID_FERAL_SWIFTNESS_PASSIVE_1 : SPELL_DRUID_FERAL_SWIFTNESS_PASSIVE_2, true, nullptr, aurEff, GetCasterGUID());
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_DRUID_FERAL_SWIFTNESS_PASSIVE_1);
        GetTarget()->RemoveAurasDueToSpell(SPELL_DRUID_FERAL_SWIFTNESS_PASSIVE_2);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_dru_feral_swiftness::AfterApply, EFFECT_0, SPELL_AURA_ANY, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_dru_feral_swiftness::AfterRemove, EFFECT_0, SPELL_AURA_ANY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 768 - Cat Form
// "Entering Cat Form will cancel this effect" (the druid's Vengeance passive, 84840)
class spell_dru_cat_form : public AuraScript
{
    PrepareAuraScript(spell_dru_cat_form);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_VENGEANCE });
    }

    void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_DRUID_VENGEANCE);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_dru_cat_form::AfterApply, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
    }
};

// 16864 - Omen of Clarity
class spell_dru_omen_of_clarity : public AuraScript
{
    PrepareAuraScript(spell_dru_omen_of_clarity);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_BALANCE_T10_BONUS, SPELL_DRUID_BALANCE_T10_BONUS_PROC });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo)
            return true;

        if (spellInfo->IsPassive())
            return false;

        if (spellInfo->HasEffect(SPELL_EFFECT_CREATE_ITEM))
            return false;

        // Reject energize spells (e.g. Furor) - they are not real casts
        if (spellInfo->HasEffect(SPELL_EFFECT_ENERGIZE))
            return false;

        if (eventInfo.GetTypeMask() & PROC_FLAG_DONE_SPELL_MELEE_DMG_CLASS)
            return spellInfo->HasAttribute(SPELL_ATTR0_ON_NEXT_SWING) || spellInfo->HasAttribute(SPELL_ATTR0_ON_NEXT_SWING_NO_DAMAGE);

        // Non-damaged/Non-healing spells - only druid abilities
        if (!spellInfo->HasAttribute(SpellCustomAttributes(SPELL_ATTR0_CU_DIRECT_DAMAGE | SPELL_ATTR0_CU_NO_INITIAL_THREAT)))
        {
            if (spellInfo->SpellFamilyName == SPELLFAMILY_DRUID)
                return !spellInfo->HasAura(SPELL_AURA_MOD_SHAPESHIFT);

            return false;
        }

        if (spellInfo->SpellIconID == SPELL_ICON_REVITALIZE)
            return false;

        return true;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        Unit* target = GetTarget();
        if (target->HasAura(SPELL_DRUID_BALANCE_T10_BONUS))
            target->CastSpell(nullptr, SPELL_DRUID_BALANCE_T10_BONUS_PROC, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dru_omen_of_clarity::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_dru_omen_of_clarity::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 22812 - Barkskin
class spell_dru_barkskin : public AuraScript
{
    PrepareAuraScript(spell_dru_barkskin);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_GLYPH_OF_BARKSKIN_TRIGGER });
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_DRUID_GLYPH_OF_BARKSKIN_TRIGGER);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_dru_barkskin::OnRemove, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
    }
};

// 63057 - Glyph of Barkskin
class spell_dru_glyph_of_barkskin : public AuraScript
{
    PrepareAuraScript(spell_dru_glyph_of_barkskin);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_GLYPH_OF_BARKSKIN_TRIGGER });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        eventInfo.GetActor()->CastSpell(eventInfo.GetActor(), SPELL_DRUID_GLYPH_OF_BARKSKIN_TRIGGER, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_glyph_of_barkskin::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

/* 35669 - Serverside - Druid Pet Scaling 01
   35670 - Serverside - Druid Pet Scaling 02
   35671 - Serverside - Druid Pet Scaling 03
   35672 - Serverside - Druid Pet Scaling 04 */
class spell_dru_treant_scaling : public AuraScript
{
    PrepareAuraScript(spell_dru_treant_scaling);

    void CalculateResistanceAmount(AuraEffect const* aurEff, int32& amount, bool& /*canBeRecalculated*/)
    {
        // xinef: treant inherits 40% of resistance from owner and 35% of armor (guessed)
        if (Unit* owner = GetUnitOwner()->GetOwner())
        {
            SpellSchoolMask schoolMask = SpellSchoolMask(aurEff->GetSpellInfo()->Effects[aurEff->GetEffIndex()].MiscValue);
            int32 modifier = schoolMask == SPELL_SCHOOL_MASK_NORMAL ? 35 : 40;
            amount = CalculatePct(std::max<int32>(0, owner->GetResistance(schoolMask)), modifier);
        }
    }

    void CalculateStatAmount(AuraEffect const* aurEff, int32& amount, bool& /*canBeRecalculated*/)
    {
        // xinef: treant inherits 30% of intellect / stamina (guessed)
        if (Unit* owner = GetUnitOwner()->GetOwner())
        {
            Stats stat = Stats(aurEff->GetSpellInfo()->Effects[aurEff->GetEffIndex()].MiscValue);
            amount = CalculatePct(std::max<int32>(0, owner->GetStat(stat)), 30);
        }
    }

    void CalculateAPAmount(AuraEffect const*  /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        // xinef: treant inherits 105% of SP as AP - 15% of damage increase per hit
        if (Unit* owner = GetUnitOwner()->GetOwner())
        {
            int32 nature = owner->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_NATURE);
            amount = CalculatePct(std::max<int32>(0, nature), 105);

            // xinef: brambles talent
            if (AuraEffect const* bramblesEff = owner->GetAuraEffect(SPELL_AURA_ADD_FLAT_MODIFIER, SPELLFAMILY_DRUID, 53, 2))
                AddPct(amount, bramblesEff->GetAmount());
        }
    }

    void CalculateSPAmount(AuraEffect const*  /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        // xinef: treant inherits 15% of SP
        if (Unit* owner = GetUnitOwner()->GetOwner())
        {
            int32 nature = owner->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_NATURE);
            amount = CalculatePct(std::max<int32>(0, nature), 15);

            // xinef: Update appropriate player field
            if (owner->IsPlayer())
                owner->SetUInt32Value(PLAYER_PET_SPELL_POWER, (uint32)amount);
        }
    }

    void HandleEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        GetUnitOwner()->ApplySpellImmune(0, IMMUNITY_STATE, aurEff->GetAuraType(), true, SPELL_BLOCK_TYPE_POSITIVE);
        if (aurEff->GetAuraType() == SPELL_AURA_MOD_ATTACK_POWER)
            GetUnitOwner()->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ATTACK_POWER_PCT, true, SPELL_BLOCK_TYPE_POSITIVE);
        else if (aurEff->GetAuraType() == SPELL_AURA_MOD_STAT)
            GetUnitOwner()->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE, true, SPELL_BLOCK_TYPE_POSITIVE);
    }

    void Register() override
    {
        if (m_scriptSpellId != 35669)
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_treant_scaling::CalculateResistanceAmount, EFFECT_ALL, SPELL_AURA_MOD_RESISTANCE);

        if (m_scriptSpellId == 35669 || m_scriptSpellId == 35670)
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_treant_scaling::CalculateStatAmount, EFFECT_ALL, SPELL_AURA_MOD_STAT);

        if (m_scriptSpellId == 35669)
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_treant_scaling::CalculateAPAmount, EFFECT_ALL, SPELL_AURA_MOD_ATTACK_POWER);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_treant_scaling::CalculateSPAmount, EFFECT_ALL, SPELL_AURA_MOD_DAMAGE_DONE);
        }

        OnEffectApply += AuraEffectApplyFn(spell_dru_treant_scaling::HandleEffectApply, EFFECT_ALL, SPELL_AURA_ANY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 1850 - Dash
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): Feral Swiftness gives Dash
// a chance (the talent's effect 1) to remove all roots and snares. The Cat Form requirement is the
// spell's own (its shapeshift mask), so the old cast check is gone.
class spell_dru_dash : public SpellScript
{
    PrepareSpellScript(spell_dru_dash);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_FERAL_SWIFTNESS_R1, SPELL_DRUID_FERAL_SWIFTNESS_CLEAR_CAT });
    }

    void HandleFeralSwiftness(SpellEffIndex /*effIndex*/)
    {
        Unit* target = GetHitUnit();
        if (Aura const* swiftness = target->GetAuraOfRankedSpell(SPELL_DRUID_FERAL_SWIFTNESS_R1, target->GetGUID()))
            if (roll_chance_i(swiftness->GetSpellInfo()->Effects[EFFECT_1].CalcValue()))
                target->CastSpell(target, SPELL_DRUID_FERAL_SWIFTNESS_CLEAR_CAT, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dru_dash::HandleFeralSwiftness, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// 1850 - Dash
class spell_dru_dash_aura : public AuraScript
{
    PrepareAuraScript(spell_dru_dash_aura);

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        // do not set speed if not in cat form
        if (GetUnitOwner()->GetShapeshiftForm() != FORM_CAT)
            amount = 0;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_dash_aura::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_INCREASE_SPEED);
    }
};

// 5229 - Enrage
class spell_dru_enrage : public AuraScript
{
    PrepareAuraScript(spell_dru_enrage);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_KING_OF_THE_JUNGLE, SPELL_DRUID_ENRAGE_MOD_DAMAGE, SPELL_DRUID_ENRAGED_DEFENSE, SPELL_DRUID_ITEM_T10_FERAL_4P_BONUS });
    }

    void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        if (AuraEffect const* aurEff = target->GetAuraEffectOfRankedSpell(SPELL_DRUID_KING_OF_THE_JUNGLE, EFFECT_0))
            target->CastCustomSpell(SPELL_DRUID_ENRAGE_MOD_DAMAGE, SPELLVALUE_BASE_POINT0, aurEff->GetAmount(), target, true, nullptr, aurEff);

        // Item - Druid T10 Feral 4P Bonus
        if (target->HasAura(SPELL_DRUID_ITEM_T10_FERAL_4P_BONUS))
            target->CastSpell(target, SPELL_DRUID_ENRAGED_DEFENSE, true);
    }

    void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_DRUID_ENRAGE_MOD_DAMAGE);
        GetTarget()->RemoveAurasDueToSpell(SPELL_DRUID_ENRAGED_DEFENSE);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_dru_enrage::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_ENERGIZE, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_dru_enrage::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_ENERGIZE, AURA_EFFECT_HANDLE_REAL);
    }
};

// 54846 - Glyph of Starfire
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): each Starfire adds 3 sec
// to Moonfire (or Sunfire), up to 9 sec; Nature's Splendor no longer exists in 4.3.4.
class spell_dru_glyph_of_starfire : public SpellScript
{
    PrepareSpellScript(spell_dru_glyph_of_starfire);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_INCREASED_MOONFIRE_DURATION });
    }

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (Unit* unitTarget = GetHitUnit())
            if (AuraEffect const* aurEff = unitTarget->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_DRUID, 0x00000002, 0, 0, caster->GetGUID()))
            {
                Aura* aura = aurEff->GetBase();

                uint32 countMin = aura->GetMaxDuration();
                uint32 countMax = aura->GetSpellInfo()->GetMaxDuration() + 9000;
                if (caster->HasAura(SPELL_DRUID_INCREASED_MOONFIRE_DURATION))
                    countMax += 3000;

                if (countMin < countMax)
                {
                    aura->SetDuration(uint32(aura->GetDuration() + 3000));
                    aura->SetMaxDuration(countMin + 3000);
                }
            }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dru_glyph_of_starfire::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 54845 - Glyph of Starfire
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46). (own) The proc row also
// lets Moonfire through; only Starfire casts the extension, and 54846 itself checks for the Moonfire.
class spell_dru_glyph_of_starfire_proc : public AuraScript
{
    PrepareAuraScript(spell_dru_glyph_of_starfire_proc);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_GLYPH_OF_STARFIRE });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        return spellInfo && (spellInfo->SpellFamilyFlags[0] & 0x00000004) && eventInfo.GetProcTarget();
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(eventInfo.GetProcTarget(), SPELL_DRUID_GLYPH_OF_STARFIRE, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dru_glyph_of_starfire_proc::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_dru_glyph_of_starfire_proc::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 34246 - Idol of the Emerald Queen
// 60779 - Idol of Lush Moss
class spell_dru_idol_lifebloom : public AuraScript
{
    PrepareAuraScript(spell_dru_idol_lifebloom);

    void HandleEffectCalcSpellMod(AuraEffect const* aurEff, SpellModifier*& spellMod)
    {
        if (!spellMod)
        {
            spellMod = new SpellModifier(GetAura());
            spellMod->op = SPELLMOD_DOT;
            spellMod->type = SPELLMOD_FLAT;
            spellMod->spellId = GetId();
            spellMod->mask = aurEff->GetSpellInfo()->Effects[aurEff->GetEffIndex()].SpellClassMask;
        }
        spellMod->value = aurEff->GetAmount() / 7;
    }

    void Register() override
    {
        DoEffectCalcSpellMod += AuraEffectCalcSpellModFn(spell_dru_idol_lifebloom::HandleEffectCalcSpellMod, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 29166 - Innervate
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): the target regains 5% of
// its own maximum mana ($m1/4), 20% plus Dreamstate when cast on self; cast on someone else, Glyph of
// Innervate gives the druid 54833 (1% of maximum mana per second). (own) The glyph is cast once when
// the aura is applied rather than from the amount calculation, which also runs on recalculation.
class spell_dru_innervate : public AuraScript
{
    PrepareAuraScript(spell_dru_innervate);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_GLYPH_OF_INNERVATE_MANA });
    }

    void CalculateAmount(AuraEffect const* aurEff, int32& amount, bool& /*canBeRecalculated*/)
    {
        Unit* target = GetUnitOwner();
        uint32 mana = target->GetMaxPower(POWER_MANA);
        if (!mana || !aurEff->GetTotalTicks())
        {
            amount = 0;
            return;
        }

        int32 percentage = amount / 4;

        // Cast on self: an additional 15%, and Dreamstate adds its amount
        Unit* caster = GetCaster();
        if (caster == target)
        {
            percentage += 15;
            if (AuraEffect const* dreamstate = caster->GetDummyAuraEffect(SPELLFAMILY_DRUID, SPELL_ICON_ID_DREAMSTATE, EFFECT_0))
                percentage += dreamstate->GetAmount();
        }

        amount = int32(CalculatePct(mana, percentage) / aurEff->GetTotalTicks());
    }

    void HandleGlyph(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();
        if (!caster || caster == GetTarget())
            return;

        if (caster->GetDummyAuraEffect(SPELLFAMILY_DRUID, SPELL_ICON_ID_GLYPH_OF_INNERVATE, EFFECT_0))
            caster->CastSpell(caster, SPELL_DRUID_GLYPH_OF_INNERVATE_MANA, true);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_innervate::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_ENERGIZE);
        AfterEffectApply += AuraEffectApplyFn(spell_dru_innervate::HandleGlyph, EFFECT_0, SPELL_AURA_PERIODIC_ENERGIZE, AURA_EFFECT_HANDLE_REAL);
    }
};

// 33763 - Lifebloom
// 94447 - Lifebloom (Tree of Life)
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): the bloom (effect 1) heals
// when Lifebloom expires or is dispelled, for the amount the druid's healing bonuses gave it, and casting
// or refreshing Lifebloom with Revitalize grants Replenishment. The WotLK mana return is gone. (own) The
// bonus is not divided by the stacks: AzerothCore multiplies the amount by the stacks after this hook.
class spell_dru_lifebloom : public AuraScript
{
    PrepareAuraScript(spell_dru_lifebloom);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_REVITALIZE_R1, SPELL_DRUID_LIFEBLOOM_FINAL_HEAL, SPELL_DRUID_REPLENISHMENT });
    }

    void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            if (caster->GetAuraOfRankedSpell(SPELL_DRUID_REVITALIZE_R1, caster->GetGUID()))
                caster->CastSpell(caster, SPELL_DRUID_REPLENISHMENT, true);
    }

    void AfterRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
        if (removeMode != AURA_REMOVE_BY_EXPIRE && removeMode != AURA_REMOVE_BY_ENEMY_SPELL)
            return;

        int32 healAmount = aurEff->GetAmount();
        if (healAmount <= 0)
            return;

        GetTarget()->CastCustomSpell(GetTarget(), SPELL_DRUID_LIFEBLOOM_FINAL_HEAL, &healAmount, nullptr, nullptr, true, nullptr, aurEff, GetCasterGUID());
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        if (Unit* caster = GetCaster())
            amount = int32(caster->SpellHealingBonusDone(GetUnitOwner(), GetSpellInfo(), std::max<int32>(amount, 0), HEAL, EFFECT_1));
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_dru_lifebloom::AfterApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        AfterEffectRemove += AuraEffectRemoveFn(spell_dru_lifebloom::AfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_lifebloom::CalculateAmount, EFFECT_1, SPELL_AURA_DUMMY);
    }
};

// -48496 - Living Seed
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): the seed holds a share of
// the effective heal, not of the overheal.
class spell_dru_living_seed : public AuraScript
{
    PrepareAuraScript(spell_dru_living_seed);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_LIVING_SEED_PROC });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        HealInfo* healInfo = eventInfo.GetHealInfo();
        if (!healInfo || !eventInfo.GetProcTarget())
            return;

        int32 amount = CalculatePct(int32(healInfo->GetEffectiveHeal()), aurEff->GetAmount());
        if (amount > 0)
            GetTarget()->CastCustomSpell(SPELL_DRUID_LIVING_SEED_PROC, SPELLVALUE_BASE_POINT0, amount, eventInfo.GetProcTarget(), true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_living_seed::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 48504 - Living Seed
class spell_dru_living_seed_proc : public AuraScript
{
    PrepareAuraScript(spell_dru_living_seed_proc);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_LIVING_SEED_HEAL });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastCustomSpell(SPELL_DRUID_LIVING_SEED_HEAL, SPELLVALUE_BASE_POINT0, aurEff->GetAmount(), GetTarget(), true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_living_seed_proc::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 1079 - Rip
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): each tick gains 2.07% of
// attack power per combo point (the tooltip's formula, so Rip needs no attack power bonus data).
class spell_dru_rip : public AuraScript
{
    PrepareAuraScript(spell_dru_rip);

    bool Load() override
    {
        Unit* caster = GetCaster();
        return caster && caster->IsPlayer();
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;

        if (Unit* caster = GetCaster())
        {
            uint8 cp = caster->GetComboPoints();

            // Idol of Feral Shadows. Can't be handled as SpellMod due its dependency from CPs
            if (AuraEffect const* idol = caster->GetAuraEffect(SPELL_DRUID_IDOL_OF_FERAL_SHADOWS, EFFECT_0))
                amount += cp * idol->GetAmount();
            // Idol of Worship. Can't be handled as SpellMod due its dependency from CPs
            else if (AuraEffect const* idol2 = caster->GetAuraEffect(SPELL_DRUID_IDOL_OF_WORSHIP, EFFECT_0))
                amount += cp * idol2->GetAmount();

            amount += int32(caster->GetTotalAttackPowerValue(BASE_ATTACK) * 0.0207f * cp);
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_rip::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 62600 - Savage Defense
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): a critical strike in Bear
// Form gives a single shield (62606) absorbing 35% of attack power, increased by the Savage Defender
// mastery. (own) The mastery multiplies the share ("increases the damage absorbed by X%") where CPP added
// the two percentages.
class spell_dru_savage_defense : public AuraScript
{
    PrepareAuraScript(spell_dru_savage_defense);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->Effects[EFFECT_0].TriggerSpell });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        Unit* target = GetTarget();

        float absorbPct = float(aurEff->GetAmount());
        if (AuraEffect const* mastery = target->GetAuraEffect(SPELL_DRUID_SAVAGE_DEFENDER, EFFECT_0))
            AddPct(absorbPct, mastery->GetAmount());

        int32 amount = int32(CalculatePct(target->GetTotalAttackPowerValue(BASE_ATTACK), absorbPct));
        target->CastCustomSpell(target, GetSpellInfo()->Effects[aurEff->GetEffIndex()].TriggerSpell, &amount, nullptr, nullptr, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_savage_defense::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 52610 - Savage Roar
class spell_dru_savage_roar : public SpellScript
{
    PrepareSpellScript(spell_dru_savage_roar);

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();
        if (caster->GetShapeshiftForm() != FORM_CAT)
            return SPELL_FAILED_ONLY_SHAPESHIFT;

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_dru_savage_roar::CheckCast);
    }
};

class spell_dru_savage_roar_aura : public AuraScript
{
    PrepareAuraScript(spell_dru_savage_roar_aura);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_SAVAGE_ROAR });
    }

    void AfterApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        target->CastSpell(target, SPELL_DRUID_SAVAGE_ROAR, true, nullptr, aurEff, GetCasterGUID());
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_DRUID_SAVAGE_ROAR);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_dru_savage_roar_aura::AfterApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_dru_savage_roar_aura::AfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 50286 - Starfall
class spell_dru_starfall_dummy : public SpellScript
{
    PrepareSpellScript(spell_dru_starfall_dummy);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        // Get caster object
        Unit* caster = GetCaster();

        // Remove targets if they are outside line of sight with respect to caster
        targets.remove_if([caster](WorldObject const* target)
          {
              if (target)
              {
                  if (!caster->IsWithinLOS(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ()))
                      return true;
              }
              return false;
          });

        // Take 2 random targets from remaining within line of sight targets
        Acore::Containers::RandomResize(targets, 2);
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        // Shapeshifting into an animal form or mounting cancels the effect
        if (caster->GetCreatureType() == CREATURE_TYPE_BEAST || caster->IsMounted())
        {
            if (SpellInfo const* spellInfo = GetTriggeringSpell())
                caster->RemoveAurasDueToSpell(spellInfo->Id);
            return;
        }

        // Any effect which causes you to lose control of your character will suppress the starfall effect.
        if (caster->HasUnitState(UNIT_STATE_CONTROLLED))
            return;

        caster->CastSpell(GetHitUnit(), uint32(GetEffectValue()), true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_starfall_dummy::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_dru_starfall_dummy::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 61336 - Survival Instincts
class spell_dru_survival_instincts : public SpellScript
{
    PrepareSpellScript(spell_dru_survival_instincts);

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();
        if (!caster->IsInFeralForm())
            return SPELL_FAILED_ONLY_SHAPESHIFT;

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_dru_survival_instincts::CheckCast);
    }
};

// 61336 - Survival Instincts
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): in 4.3.4 it reduces all
// damage taken by 50% (50322's own amount) instead of the WotLK maximum health increase.
class spell_dru_survival_instincts_aura : public AuraScript
{
    PrepareAuraScript(spell_dru_survival_instincts_aura);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_SURVIVAL_INSTINCTS });
    }

    void AfterApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->CastSpell(GetTarget(), SPELL_DRUID_SURVIVAL_INSTINCTS, true, nullptr, aurEff);
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_DRUID_SURVIVAL_INSTINCTS);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_dru_survival_instincts_aura::AfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
        AfterEffectRemove += AuraEffectRemoveFn(spell_dru_survival_instincts_aura::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
    }
};

// 40121 - Swift Flight Form (Passive)
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): 310% flying speed comes with
// Master Riding (riding skill 375) in 4.3.4, not with owning a 310% mount.
class spell_dru_swift_flight_passive : public AuraScript
{
    PrepareAuraScript(spell_dru_swift_flight_passive);

    bool Load() override
    {
        return GetCaster() && GetCaster()->IsPlayer();
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        if (Player* caster = GetCaster()->ToPlayer())
            if (caster->GetSkillValue(SKILL_RIDING) >= 375)
                amount = 310;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_swift_flight_passive::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED);
    }
};

// 33943 - Flight Form
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): no flight form while
// transformed into something that cannot mount. (own) Only a transform blocks it; CPP's check also
// refused the cast from Cat or Bear Form, which shift straight into Flight Form.
class spell_dru_flight_form : public SpellScript
{
    PrepareSpellScript(spell_dru_flight_form);

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();
        if (caster->getTransForm() && caster->IsInDisallowedMountForm())
            return SPELL_FAILED_NOT_SHAPESHIFT;

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_dru_flight_form::CheckCast);
    }
};

// 40120 - Swift Flight Form
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): sniffed shapeshift amounts.
class spell_dru_swift_flight_form : public AuraScript
{
    PrepareAuraScript(spell_dru_swift_flight_form);

    bool Load() override
    {
        return GetCaster() && GetCaster()->IsPlayer();
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        if (Player* caster = GetCaster()->ToPlayer())
            amount = caster->GetSkillValue(SKILL_RIDING) >= 375 ? 240 : 239;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_swift_flight_form::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT);
    }
};

// 5217 - Tiger's Fury
class spell_dru_tiger_s_fury : public SpellScript
{
    PrepareSpellScript(spell_dru_tiger_s_fury);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_TIGER_S_FURY_ENERGIZE });
    }

    void OnHit()
    {
        if (AuraEffect const* aurEff = GetHitUnit()->GetAuraEffectOfRankedSpell(SPELL_DRUID_KING_OF_THE_JUNGLE, EFFECT_1))
            GetHitUnit()->CastCustomSpell(SPELL_DRUID_TIGER_S_FURY_ENERGIZE, SPELLVALUE_BASE_POINT0, aurEff->GetAmount(), GetHitUnit(), true);
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_dru_tiger_s_fury::OnHit);
    }
};

// 61391 - Typhoon
class spell_dru_typhoon : public SpellScript
{
    PrepareSpellScript(spell_dru_typhoon);

    void HandleKnockBack(SpellEffIndex effIndex)
    {
        // Glyph of Typhoon
        if (GetCaster()->HasAura(SPELL_DRUID_GLYPH_OF_TYPHOON))
            PreventHitDefaultEffect(effIndex);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dru_typhoon::HandleKnockBack, EFFECT_0, SPELL_EFFECT_KNOCK_BACK);
    }
};

// 70691 - Rejuvenation
class spell_dru_t10_restoration_4p_bonus : public SpellScript
{
    PrepareSpellScript(spell_dru_t10_restoration_4p_bonus);

    bool Load() override
    {
        return GetCaster()->IsPlayer();
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (!GetCaster()->ToPlayer()->GetGroup())
        {
            targets.clear();
            targets.push_back(GetCaster());
        }
        else
        {
            targets.remove(GetExplTargetUnit());
            std::list<Unit*> tempTargets;
            for (std::list<WorldObject*>::const_iterator itr = targets.begin(); itr != targets.end(); ++itr)
                if ((*itr)->isType(TYPEMASK_UNIT | TYPEMASK_PLAYER) && GetCaster()->IsInRaidWith((*itr)->ToUnit()) && !(*itr)->ToUnit()->GetAuraEffect(SPELL_AURA_PERIODIC_HEAL, SPELLFAMILY_DRUID, 64, EFFECT_0))
                    tempTargets.push_back((*itr)->ToUnit());

            if (tempTargets.empty())
            {
                targets.clear();
                FinishCast(SPELL_FAILED_DONT_REPORT);
                return;
            }

            tempTargets.sort(Acore::HealthPctOrderPred());
            targets.clear();
            targets.push_back(tempTargets.front());
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_t10_restoration_4p_bonus::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
    }
};

// 48438 - Wild Growth
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): the most injured raid
// members up to effect 2's value (5, Glyph of Wild Growth adds one through its modifier), two more in
// Tree of Life.
class spell_dru_wild_growth : public SpellScript
{
    PrepareSpellScript(spell_dru_wild_growth);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_TREE_OF_LIFE });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(Acore::RaidCheck(GetCaster(), false));

        uint32 maxTargets = uint32(std::max<int32>(GetSpellInfo()->Effects[EFFECT_2].CalcValue(GetCaster()), 0));

        // Tree of Life: two more targets
        if (GetCaster()->HasAura(SPELL_DRUID_TREE_OF_LIFE))
            maxTargets += 2;

        if (targets.size() > maxTargets)
        {
            targets.sort(Acore::HealthPctOrderPred());
            targets.resize(maxTargets);
        }

        _targets = targets;
    }

    void SetTargets(std::list<WorldObject*>& targets)
    {
        targets = _targets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_wild_growth::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_wild_growth::SetTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ALLY);
    }

private:
    std::list<WorldObject*> _targets;
};

// 48438 - Wild Growth (AuraScript)
// 4.3.4 keeps the falling heal ("applied quickly at first, and slows down").
class spell_dru_wild_growth_aura : public AuraScript
{
    PrepareAuraScript(spell_dru_wild_growth_aura);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_RESTORATION_T10_2P_BONUS });
    }

    void SetTickHeal(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        _baseTick = amount;
        if (Unit* caster = GetCaster())
            if (AuraEffect const* bonus = caster->GetAuraEffect(SPELL_DRUID_RESTORATION_T10_2P_BONUS, EFFECT_0))
                AddPct(_baseReduction, -bonus->GetAmount());
    }

    void HandleTickUpdate(AuraEffect* aurEff)
    {
        // Wild Growth = first tick gains a 6% bonus, reduced by 2% each tick
        float reduction = _baseReduction;
        reduction *= (aurEff->GetTickNumber() - 1);

        float const bonus = 6.f - reduction;
        int32 const amount = int32(_baseTick + CalculatePct(_baseTick, bonus));
        aurEff->SetAmount(amount);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_wild_growth_aura::SetTickHeal, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
        OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_dru_wild_growth_aura::HandleTickUpdate, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
    }

    float _baseTick = 0.f;
    float _baseReduction = 2.f;
};

// 774 - Rejuvenation
class spell_dru_rejuvenation_moonglade_2_set : public AuraScript
{
    PrepareAuraScript(spell_dru_rejuvenation_moonglade_2_set);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_MOONGLADE_2P_BONUS });
    }

    bool Load() override
    {
        _casterGUID.Clear();
        return true;
    }

    void OnApply(AuraEffect const*  /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* caster = ObjectAccessor::FindPlayer(GetCasterGUID()))
            if (caster->HasAura(SPELL_DRUID_MOONGLADE_2P_BONUS))
                {
                    Player* target = GetTarget()->ToPlayer();
                    if (!target)
                        return;

                    _casterGUID = GetCasterGUID();
                    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(SPELL_DRUID_MOONGLADE_2P_BONUS);
                    target->ApplyRatingMod(CR_DODGE, spellInfo->Effects[EFFECT_0].CalcValue(), true); // 35 rating
                }
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (_casterGUID)
        {
            Player* target = GetTarget()->ToPlayer();
            if (!target)
                return;

            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(SPELL_DRUID_MOONGLADE_2P_BONUS);
            target->ApplyRatingMod(CR_DODGE, spellInfo->Effects[EFFECT_0].CalcValue(), false); // 35 rating
        }
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_dru_rejuvenation_moonglade_2_set::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_dru_rejuvenation_moonglade_2_set::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
    }

private:
    ObjectGuid _casterGUID;
};

// 17007 - Leader of the Pack
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): in 4.3.4 the talent itself
// procs on melee critical strikes in Cat or Bear Form (6 sec cooldown in spell_proc), healing 4% of
// maximum health (34299's amount) and restoring 8% of maximum mana (68285's amount).
class spell_dru_leader_of_the_pack : public AuraScript
{
    PrepareAuraScript(spell_dru_leader_of_the_pack);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_LEADER_OF_THE_PACK_HEAL, SPELL_DRUID_LEADER_OF_THE_PACK_MANA });
    }

    bool CheckProc(ProcEventInfo& /*eventInfo*/)
    {
        ShapeshiftForm form = GetTarget()->GetShapeshiftForm();
        return form == FORM_CAT || form == FORM_BEAR;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        Unit* target = GetTarget();

        int32 heal = 0;
        if (int32 pct = sSpellMgr->GetSpellInfo(SPELL_DRUID_LEADER_OF_THE_PACK_HEAL)->Effects[EFFECT_0].CalcValue())
            heal = int32(target->CountPctFromMaxHealth(pct));

        int32 mana = 0;
        if (int32 pct = sSpellMgr->GetSpellInfo(SPELL_DRUID_LEADER_OF_THE_PACK_MANA)->Effects[EFFECT_0].CalcValue())
            mana = CalculatePct(int32(target->GetMaxPower(POWER_MANA)), pct);

        if (heal > 0)
            target->CastCustomSpell(SPELL_DRUID_LEADER_OF_THE_PACK_HEAL, SPELLVALUE_BASE_POINT0, heal, target, true, nullptr, aurEff);

        if (mana > 0)
            target->CastCustomSpell(SPELL_DRUID_LEADER_OF_THE_PACK_MANA, SPELLVALUE_BASE_POINT0, mana, target, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dru_leader_of_the_pack::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_dru_leader_of_the_pack::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

enum DruidEclipseSpells
{
    SPELL_DRUID_STARSURGE_ENERGIZE          = 86605,
    SPELL_DRUID_ECLIPSE_GENERAL_ENERGIZE    = 89265,
    SPELL_DRUID_SOLAR_ECLIPSE_MARKER        = 67483, // points the eclipse bar arrow to the solar side
    SPELL_DRUID_LUNAR_ECLIPSE_MARKER        = 67484, // points the eclipse bar arrow to the lunar side
    SPELL_DRUID_SOLAR_ECLIPSE_SUNFIRE       = 94338,
    SPELL_DRUID_SUNFIRE                     = 93402,
    SPELL_DRUID_EUPHORIA_ENERGIZE           = 81069,
    SPELL_DRUID_EUPHORIA_MANA_ENERGIZE      = 81070,
    SPELL_DRUID_NATURES_GRACE               = 16880,
    SPELL_DRUID_LUNAR_SHOWER                = 33603,
    SPELL_DRUID_ITEM_T11_BALANCE_4P_BONUS   = 90163
};

enum DruidEclipseIcons
{
    SPELL_ICON_ID_EUPHORIA                  = 4431,
    SPELL_ICON_ID_SUNFIRE                   = 3262
};

// 48517 - Eclipse (Solar)
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46).
class spell_dru_eclipse : public AuraScript
{
    PrepareAuraScript(spell_dru_eclipse);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_ECLIPSE_SOLAR, SPELL_DRUID_SUNFIRE, SPELL_DRUID_SOLAR_ECLIPSE_SUNFIRE });
    }

    void ApplyEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();

        // Sunfire talent: Moonfire becomes Sunfire during Solar Eclipse
        if (GetSpellInfo()->Id == SPELL_DRUID_ECLIPSE_SOLAR && target->GetDummyAuraEffect(SPELLFAMILY_DRUID, SPELL_ICON_ID_SUNFIRE, EFFECT_0))
            if (!target->HasAura(SPELL_DRUID_SOLAR_ECLIPSE_SUNFIRE))
                target->CastCustomSpell(SPELL_DRUID_SOLAR_ECLIPSE_SUNFIRE, SPELLVALUE_BASE_POINT0, SPELL_DRUID_SUNFIRE, target, true);
    }

    void RemoveEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetSpellInfo()->Id == SPELL_DRUID_ECLIPSE_SOLAR)
            GetTarget()->RemoveAurasDueToSpell(SPELL_DRUID_SOLAR_ECLIPSE_SUNFIRE);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_dru_eclipse::ApplyEffect, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_dru_eclipse::RemoveEffect, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
    }
};

// 79577 - Eclipse Mastery Driver Passive
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46). Wrath moves the
// eclipse power towards -100 (lunar), Starfire towards +100 (solar); SPELL_AURA_PROC_ON_POWER_AMOUNT
// then casts the eclipse at +-100, and the marker flips so the bar runs back the other way.
class spell_dru_eclipse_mastery_driver_passive : public AuraScript
{
    PrepareAuraScript(spell_dru_eclipse_mastery_driver_passive);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_DRUID_ECLIPSE_LUNAR,
                SPELL_DRUID_LUNAR_ECLIPSE_MARKER,
                SPELL_DRUID_ECLIPSE_SOLAR,
                SPELL_DRUID_SOLAR_ECLIPSE_MARKER,
                SPELL_DRUID_EUPHORIA_ENERGIZE,
                SPELL_DRUID_EUPHORIA_MANA_ENERGIZE,
                SPELL_DRUID_STARSURGE_ENERGIZE,
                SPELL_DRUID_ECLIPSE_GENERAL_ENERGIZE,
                SPELL_DRUID_NATURES_GRACE,
                SPELL_DRUID_LUNAR_SHOWER
            });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Player* player = GetTarget()->ToPlayer();
        if (!player || player->GetPrimaryTalentTree(player->GetActiveSpec()) != TALENT_TREE_DRUID_BALANCE)
            return false;

        return eventInfo.GetSpellInfo() != nullptr;
    }

    bool CheckSolarEclipseProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        _eclipseMarker = GetEclipseMarker();
        if (_eclipseMarker && _eclipseMarker->GetId() != SPELL_DRUID_SOLAR_ECLIPSE_MARKER)
            return false;

        _energyAmount = GetEnergyGainFromSpellInfo(eventInfo.GetSpellInfo());
        return _energyAmount > 0;
    }

    bool CheckLunarEclipseProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        _eclipseMarker = GetEclipseMarker();
        if (_eclipseMarker && _eclipseMarker->GetId() != SPELL_DRUID_LUNAR_ECLIPSE_MARKER)
            return false;

        _energyAmount = GetEnergyGainFromSpellInfo(eventInfo.GetSpellInfo());
        return _energyAmount < 0;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        Unit* target = GetTarget();

        // Apply the eclipse marker if not present
        if (!_eclipseMarker)
            target->CastSpell(target, aurEff->GetEffIndex() == EFFECT_0 ? SPELL_DRUID_SOLAR_ECLIPSE_MARKER : SPELL_DRUID_LUNAR_ECLIPSE_MARKER, true);

        // Energy gain (negative values move the bar towards lunar)
        uint32 energizeSpell = (eventInfo.GetSpellInfo()->SpellFamilyFlags[2] & 0x2000000) != 0 ? SPELL_DRUID_STARSURGE_ENERGIZE : SPELL_DRUID_ECLIPSE_GENERAL_ENERGIZE;
        target->CastCustomSpell(energizeSpell, SPELLVALUE_BASE_POINT0, _energyAmount, target, true);

        // Euphoria bonus
        if (_euphoriaEnergyAmount)
            target->CastCustomSpell(SPELL_DRUID_EUPHORIA_ENERGIZE, SPELLVALUE_BASE_POINT0, _euphoriaEnergyAmount, target, true);
    }

    void HandleEclipseProc(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
    {
        Unit* target = GetTarget();
        uint32 newMarkerSpellId = 0;
        int32 eclipsePower = int32(target->GetPower(POWER_ECLIPSE));

        if (AuraEffect* eclipse = target->GetAuraEffect(SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, SPELLFAMILY_DRUID, 0x0, 0x0, 0x4000))
        {
            if (_eclipseMarker)
            {
                if (eclipse->GetId() == SPELL_DRUID_ECLIPSE_LUNAR && _eclipseMarker->GetId() == SPELL_DRUID_LUNAR_ECLIPSE_MARKER)
                    newMarkerSpellId = SPELL_DRUID_SOLAR_ECLIPSE_MARKER;
                else if (eclipse->GetId() == SPELL_DRUID_ECLIPSE_SOLAR && _eclipseMarker->GetId() == SPELL_DRUID_SOLAR_ECLIPSE_MARKER)
                    newMarkerSpellId = SPELL_DRUID_LUNAR_ECLIPSE_MARKER;
            }

            // An eclipse lasts until the bar crosses back over the middle
            if ((eclipse->GetId() == SPELL_DRUID_ECLIPSE_LUNAR && eclipsePower >= 0) ||
                (eclipse->GetId() == SPELL_DRUID_ECLIPSE_SOLAR && eclipsePower <= 0))
                eclipse->GetBase()->Remove();
        }

        if (!newMarkerSpellId)
            return;

        if (_eclipseMarker)
            _eclipseMarker->GetBase()->Remove();

        target->CastSpell(target, newMarkerSpellId, true);

        // Reset Nature's Grace proc cooldown
        if (Aura* aura = target->GetAuraOfRankedSpell(SPELL_DRUID_NATURES_GRACE))
            aura->ResetProcCooldown();

        // Euphoria mana bonus
        if (AuraEffect const* euphoria = target->GetDummyAuraEffect(SPELLFAMILY_DRUID, SPELL_ICON_ID_EUPHORIA, EFFECT_2))
            target->CastCustomSpell(SPELL_DRUID_EUPHORIA_MANA_ENERGIZE, SPELLVALUE_BASE_POINT0, euphoria->GetAmount(), target, true);

        // T11 Balance 4P bonus
        if (target->GetAuraEffect(SPELL_DRUID_ITEM_T11_BALANCE_4P_BONUS, EFFECT_0))
            target->CastCustomSpell(SPELL_DRUID_ASTRAL_ALIGNMENT, SPELLVALUE_AURA_STACK, 3, target, true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dru_eclipse_mastery_driver_passive::CheckProc);
        DoCheckEffectProc += AuraCheckEffectProcFn(spell_dru_eclipse_mastery_driver_passive::CheckSolarEclipseProc, EFFECT_0, SPELL_AURA_PROC_ON_POWER_AMOUNT);
        DoCheckEffectProc += AuraCheckEffectProcFn(spell_dru_eclipse_mastery_driver_passive::CheckLunarEclipseProc, EFFECT_1, SPELL_AURA_PROC_ON_POWER_AMOUNT);
        OnEffectProc += AuraEffectProcFn(spell_dru_eclipse_mastery_driver_passive::HandleProc, EFFECT_0, SPELL_AURA_PROC_ON_POWER_AMOUNT);
        OnEffectProc += AuraEffectProcFn(spell_dru_eclipse_mastery_driver_passive::HandleProc, EFFECT_1, SPELL_AURA_PROC_ON_POWER_AMOUNT);
        AfterEffectProc += AuraEffectProcFn(spell_dru_eclipse_mastery_driver_passive::HandleEclipseProc, EFFECT_0, SPELL_AURA_PROC_ON_POWER_AMOUNT);
        AfterEffectProc += AuraEffectProcFn(spell_dru_eclipse_mastery_driver_passive::HandleEclipseProc, EFFECT_1, SPELL_AURA_PROC_ON_POWER_AMOUNT);
    }

private:
    AuraEffect* GetEclipseMarker() const
    {
        return GetTarget()->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_DRUID, 0x0, 0x0, 0x100000, GetTarget()->GetGUID());
    }

    int32 GetEnergyGainFromSpellInfo(SpellInfo const* spell)
    {
        Unit* target = GetTarget();
        int32 energyAmount = 0;
        int32 bonusEnergyAmount = 0;

        // Wrath (towards lunar) and Starfire (towards solar)
        if ((spell->SpellFamilyFlags[0] & 0x1) != 0)
            energyAmount = -spell->Effects[EFFECT_1].CalcValue(target);
        else if ((spell->SpellFamilyFlags[0] & 0x4) != 0)
            energyAmount = spell->Effects[EFFECT_1].CalcValue(target);

        // Euphoria talent: chance to double the gain while no eclipse is active
        if (energyAmount != 0 && !target->GetAuraEffect(SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, SPELLFAMILY_DRUID, 0x0, 0x0, 0x4000))
            if (AuraEffect const* euphoria = target->GetDummyAuraEffect(SPELLFAMILY_DRUID, SPELL_ICON_ID_EUPHORIA, EFFECT_0))
                if (roll_chance_i(euphoria->GetAmount()))
                    bonusEnergyAmount = energyAmount;

        // Starsurge moves the bar in the marker's direction
        if ((spell->SpellFamilyFlags[2] & 0x2000000) != 0)
        {
            int32 bp = spell->Effects[EFFECT_1].CalcValue(target);
            if (_eclipseMarker)
                energyAmount += _eclipseMarker->GetId() == SPELL_DRUID_LUNAR_ECLIPSE_MARKER ? -bp : bp;
            else
                energyAmount -= bp;
        }

        // Lunar Shower bonus for Moonfire and Sunfire
        if ((spell->SpellFamilyFlags[0] & 0x2) != 0)
        {
            if (Aura const* lunarShower = target->GetAuraOfRankedSpell(SPELL_DRUID_LUNAR_SHOWER, target->GetGUID()))
            {
                if (target->HasAura(lunarShower->GetSpellInfo()->Effects[EFFECT_0].TriggerSpell))
                {
                    int32 bp = sSpellMgr->AssertSpellInfo(SPELL_DRUID_LUNAR_SHOWER)->Effects[EFFECT_2].CalcValue(target);
                    if (_eclipseMarker)
                        energyAmount += _eclipseMarker->GetId() == SPELL_DRUID_LUNAR_ECLIPSE_MARKER ? -bp : bp;
                    else
                        energyAmount -= bp;
                }
            }
        }

        _euphoriaEnergyAmount = bonusEnergyAmount;
        return energyAmount;
    }

    AuraEffect* _eclipseMarker = nullptr;
    int32 _energyAmount = 0;
    int32 _euphoriaEnergyAmount = 0;
};

// 28716 - Rejuvenation (T3 2P Bonus)
class spell_dru_t3_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_dru_t3_2p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({
            SPELL_DRUID_T3_PROC_ENERGIZE_MANA,
            SPELL_DRUID_T3_PROC_ENERGIZE_RAGE,
            SPELL_DRUID_T3_PROC_ENERGIZE_ENERGY
        });
    }

    bool CheckProc(ProcEventInfo& /*eventInfo*/)
    {
        return roll_chance_i(50);
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* target = eventInfo.GetActionTarget();
        uint32 spellId;

        switch (target->getPowerType())
        {
            case POWER_MANA:
                spellId = SPELL_DRUID_T3_PROC_ENERGIZE_MANA;
                break;
            case POWER_RAGE:
                spellId = SPELL_DRUID_T3_PROC_ENERGIZE_RAGE;
                break;
            case POWER_ENERGY:
                spellId = SPELL_DRUID_T3_PROC_ENERGIZE_ENERGY;
                break;
            default:
                return;
        }

        eventInfo.GetActor()->CastSpell(target, spellId, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dru_t3_2p_bonus::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_dru_t3_2p_bonus::HandleProc, EFFECT_0, SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
    }
};

// 28744 - Regrowth (T3 6P Bonus)
class spell_dru_t3_6p_bonus : public AuraScript
{
    PrepareAuraScript(spell_dru_t3_6p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_BLESSING_OF_THE_CLAW });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        eventInfo.GetActor()->CastSpell(eventInfo.GetActionTarget(), SPELL_DRUID_BLESSING_OF_THE_CLAW, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_t3_6p_bonus::HandleProc, EFFECT_0, SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
    }
};

// 28719 - Healing Touch (T3 8P Bonus)
class spell_dru_t3_8p_bonus : public AuraScript
{
    PrepareAuraScript(spell_dru_t3_8p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_EXHILARATE });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo)
            return;

        Unit* caster = eventInfo.GetActor();
        int32 amount = CalculatePct(spellInfo->CalcPowerCost(caster, spellInfo->GetSchoolMask()), aurEff->GetAmount());
        caster->CastCustomSpell(SPELL_DRUID_EXHILARATE, SPELLVALUE_BASE_POINT0, amount, caster, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_t3_8p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 37288, 37295 - Mana Restore (T4 2P Bonus)
class spell_dru_t4_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_dru_t4_2p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_INFUSION });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(nullptr, SPELL_DRUID_INFUSION, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_t4_2p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 40442 - Druid Tier 6 Trinket
class spell_dru_item_t6_trinket : public AuraScript
{
    PrepareAuraScript(spell_dru_item_t6_trinket);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({
            SPELL_DRUID_BLESSING_OF_REMULOS,
            SPELL_DRUID_BLESSING_OF_ELUNE,
            SPELL_DRUID_BLESSING_OF_CENARIUS
        });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo)
            return;

        uint32 spellId;
        int32 chance;

        // Starfire
        if (spellInfo->SpellFamilyFlags[0] & 0x00000004)
        {
            spellId = SPELL_DRUID_BLESSING_OF_REMULOS;
            chance = 25;
        }
        // Rejuvenation
        else if (spellInfo->SpellFamilyFlags[0] & 0x00000010)
        {
            spellId = SPELL_DRUID_BLESSING_OF_ELUNE;
            chance = 25;
        }
        // Mangle (Bear) and Mangle (Cat)
        else if (spellInfo->SpellFamilyFlags[1] & 0x00000440)
        {
            spellId = SPELL_DRUID_BLESSING_OF_CENARIUS;
            chance = 40;
        }
        else
            return;

        if (roll_chance_i(chance))
            eventInfo.GetActor()->CastSpell(nullptr, spellId, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_item_t6_trinket::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 54815 - Glyph of Bloodletting (Glyph of Shred in WotLK)
// Shred or Mangle extend the caster's Rip by 2 sec, up to 6 sec. (own) In 4.3.4 Glyph of Rip adds damage,
// not duration, so it no longer raises the cap; the T7 bonus (60141) still adds 4 sec to Rip.
class spell_dru_glyph_of_shred : public AuraScript
{
    PrepareAuraScript(spell_dru_glyph_of_shred);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_RIP_DURATION_LACERATE_DMG });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        Unit* caster = eventInfo.GetActor();
        Unit* target = eventInfo.GetActionTarget();
        if (!caster || !target)
            return;

        // Try to find Rip on the target
        if (AuraEffect const* rip = target->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_DRUID, 0x00800000, 0x0, 0x0, caster->GetGUID()))
        {
            // Rip's max duration, includes the extensions so far
            uint32 countMin = rip->GetBase()->GetMaxDuration();

            // Just Rip's max duration without other spells
            uint32 countMax = rip->GetSpellInfo()->GetMaxDuration();

            // Add the glyph's 3 extensions and the T7 bonus
            countMax += 3 * aurEff->GetAmount() * IN_MILLISECONDS;
            countMax += caster->HasAura(SPELL_DRUID_RIP_DURATION_LACERATE_DMG) ? 4 * IN_MILLISECONDS : 0;

            // If min < max the caster has not used up the extensions yet
            if (countMin < countMax)
            {
                rip->GetBase()->SetDuration(rip->GetBase()->GetDuration() + aurEff->GetAmount() * IN_MILLISECONDS);
                rip->GetBase()->SetMaxDuration(countMin + aurEff->GetAmount() * IN_MILLISECONDS);
            }
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_glyph_of_shred::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 70664 - Item - Druid T10 Restoration 4P Bonus (Rejuvenation)
class spell_dru_t10_restoration_4p_bonus_dummy : public AuraScript
{
    PrepareAuraScript(spell_dru_t10_restoration_4p_bonus_dummy);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_REJUVENATION_T10_PROC });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo || spellInfo->Id == SPELL_DRUID_REJUVENATION_T10_PROC)
            return false;

        HealInfo* healInfo = eventInfo.GetHealInfo();
        if (!healInfo || !healInfo->GetHeal())
            return false;

        Player* caster = eventInfo.GetActor()->ToPlayer();
        if (!caster)
            return false;

        return caster->GetGroup() || caster != eventInfo.GetActionTarget();
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        int32 amount = eventInfo.GetHealInfo()->GetHeal();
        eventInfo.GetActor()->CastCustomSpell(SPELL_DRUID_REJUVENATION_T10_PROC, SPELLVALUE_BASE_POINT0, amount, nullptr, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dru_t10_restoration_4p_bonus_dummy::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_dru_t10_restoration_4p_bonus_dummy::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 44835 - Maim Interrupt
class spell_dru_maim_interrupt : public AuraScript
{
    PrepareAuraScript(spell_dru_maim_interrupt);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_MAIM_INTERRUPT });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(GetTarget(), SPELL_DRUID_MAIM_INTERRUPT, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_maim_interrupt::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 67353 - T9 Feral Relic (Idol of Mutilation)
class spell_dru_t9_feral_relic : public AuraScript
{
    PrepareAuraScript(spell_dru_t9_feral_relic);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({
            SPELL_DRUID_T9_FERAL_RELIC_BEAR,
            SPELL_DRUID_T9_FERAL_RELIC_CAT
        });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Unit* target = eventInfo.GetActor();

        switch (target->GetShapeshiftForm())
        {
            case FORM_BEAR:
            case FORM_DIREBEAR:
            case FORM_CAT:
                return true;
            default:
                break;
        }

        return false;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        uint32 triggerspell = 0;

        Unit* target = eventInfo.GetActor();

        switch (target->GetShapeshiftForm())
        {
            case FORM_BEAR:
            case FORM_DIREBEAR:
                triggerspell = SPELL_DRUID_T9_FERAL_RELIC_BEAR;
                break;
            case FORM_CAT:
                triggerspell = SPELL_DRUID_T9_FERAL_RELIC_CAT;
                break;
            default:
                return;
        }

        target->CastSpell(target, triggerspell, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dru_t9_feral_relic::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_dru_t9_feral_relic::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 22842 - Frenzied Regeneration
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): raises health to 30% if it
// is lower, then converts up to 10 rage a second into 0.3% of maximum health per rage point; with the
// Glyph of Frenzied Regeneration (54810) it converts nothing and boosts healing taken instead.
class spell_dru_frenzied_regeneration : public AuraScript
{
    PrepareAuraScript(spell_dru_frenzied_regeneration);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_FRENZIED_REGENERATION_HEAL, SPELL_DRUID_GLYPH_OF_FRENZIED_REGEN });
    }

    void HandleHealthAfterApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        int32 pct = aurEff->GetAmount();
        if (target->GetHealthPct() < float(pct))
            target->SetHealth(target->CountPctFromMaxHealth(pct));
    }

    void HandleRegeneration(AuraEffect const* aurEff)
    {
        Unit* target = GetTarget();
        if (target->HasAura(SPELL_DRUID_GLYPH_OF_FRENZIED_REGEN))
            return;

        // rage is stored x10: up to 10 rage (100) a tick
        uint32 rage = std::min<uint32>(target->GetPower(POWER_RAGE), 100);
        if (!rage)
            return;

        float healthPct = aurEff->GetAmount() / 100.0f;
        int32 heal = int32(CalculatePct(float(target->GetMaxHealth()), healthPct) * (rage / 10));
        if (heal <= 0)
            return;

        target->ModifyPower(POWER_RAGE, -int32(rage));
        target->CastCustomSpell(SPELL_DRUID_FRENZIED_REGENERATION_HEAL, SPELLVALUE_BASE_POINT0, heal, target, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dru_frenzied_regeneration::HandleRegeneration, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        AfterEffectApply += AuraEffectApplyFn(spell_dru_frenzied_regeneration::HandleHealthAfterApply, EFFECT_1, SPELL_AURA_MOD_INCREASE_HEALTH_2, AURA_EFFECT_HANDLE_REAL);
    }
};

// 5570 - Insect Swarm
class spell_dru_insect_swarm : public AuraScript
{
    PrepareAuraScript(spell_dru_insect_swarm);

    void CalculateAmount(AuraEffect const* aurEff, int32& amount, bool& /*canBeRecalculated*/)
    {
        if (Unit* caster = GetCaster())
            if (AuraEffect const* relicAurEff = caster->GetAuraEffect(SPELL_DRUID_ITEM_T8_BALANCE_RELIC, EFFECT_0))
                amount += relicAurEff->GetAmount() / aurEff->GetTotalTicks();
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_insect_swarm::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 50464 - Nourish
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): 20% more healing when the
// druid's Rejuvenation, Regrowth, Lifebloom or Wild Growth is on the target (the WotLK Glyph of Nourish
// is Glyph of Starsurge in 4.3.4).
class spell_dru_nourish : public SpellScript
{
    PrepareSpellScript(spell_dru_nourish);

    void HandleHealBonus(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        if (!target)
            return;

        Unit::AuraEffectList const& periodicHeals = target->GetAuraEffectsByType(SPELL_AURA_PERIODIC_HEAL);
        bool applyBonus = std::any_of(periodicHeals.begin(), periodicHeals.end(), [guid = caster->GetGUID()](AuraEffect const* aurEff)
        {
            if (aurEff->GetCasterGUID() != guid || aurEff->GetSpellInfo()->SpellFamilyName != SPELLFAMILY_DRUID)
                return false;

            return bool(aurEff->GetSpellInfo()->SpellFamilyFlags.HasFlag(0x10 | 0x40, 0x10 | 0x4000000, 0));
        });

        if (applyBonus)
            SetEffectValue(int32(GetEffectValue() * 1.2f));
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_dru_nourish::HandleHealBonus, EFFECT_0, SPELL_EFFECT_HEAL);
    }
};

// 90164 - Astral Alignment
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): each critical strike uses
// up one of the three stacks.
class spell_dru_astral_alignment : public AuraScript
{
    PrepareAuraScript(spell_dru_astral_alignment);

    void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetAura()->ModStackAmount(-1);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_astral_alignment::HandleProc, EFFECT_0, SPELL_AURA_MOD_SPELL_CRIT_CHANCE);
    }
};

// -80318 - Blood in the Water
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): Ferocious Bite on a target at
// or below 25% health (60% with the T13 feral 2P bonus) refreshes Rip, 50% chance per rank. (own) Rank 1's
// effect 0 is a proc trigger, rank 2's a dummy, so the handler takes either aura type.
class spell_dru_blood_in_the_water : public AuraScript
{
    PrepareAuraScript(spell_dru_blood_in_the_water);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_BLOOD_IN_THE_WATER_SCRIPT, SPELL_DRUID_T13_FERAL_2P_BONUS });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetVictim())
            return false;

        float healthPct = float(GetSpellInfo()->Effects[EFFECT_0].CalcValue());

        // T13 Bonus
        if (GetTarget()->HasAura(SPELL_DRUID_T13_FERAL_2P_BONUS))
            healthPct = float(sSpellMgr->GetSpellInfo(SPELL_DRUID_T13_FERAL_2P_BONUS)->Effects[EFFECT_1].CalcValue());

        return damageInfo->GetVictim()->GetHealthPct() <= healthPct && roll_chance_i(50 * GetSpellInfo()->GetRank());
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(eventInfo.GetProcTarget(), SPELL_DRUID_BLOOD_IN_THE_WATER_SCRIPT, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dru_blood_in_the_water::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_dru_blood_in_the_water::HandleProc, EFFECT_0, SPELL_AURA_ANY);
    }
};

// 80863 - Blood in the Water
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46).
class spell_dru_blood_in_the_water_script : public SpellScript
{
    PrepareSpellScript(spell_dru_blood_in_the_water_script);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_RIP });
    }

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        if (Aura* aura = GetHitUnit()->GetAura(SPELL_DRUID_RIP, GetCaster()->GetGUID()))
            aura->RefreshDuration();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dru_blood_in_the_water_script::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// -34151 - Efflorescence (proc aura)
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): Swiftmend leaves a patch
// (81262) whose effect 1 holds the talent's share of the effective heal.
class spell_dru_effloresence : public AuraScript
{
    PrepareAuraScript(spell_dru_effloresence);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_EFFLORESCENCE_AOE });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        HealInfo* healInfo = eventInfo.GetHealInfo();
        if (!healInfo || !healInfo->GetTarget())
            return;

        int32 healAmount = CalculatePct(int32(healInfo->GetEffectiveHeal()), aurEff->GetAmount());
        if (healAmount > 0)
            GetTarget()->CastCustomSpell(healInfo->GetTarget(), SPELL_DRUID_EFFLORESCENCE_AOE, nullptr, &healAmount, nullptr, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_effloresence::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 81262 - Efflorescence (area effect)
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46).
class spell_dru_effloresence_aoe : public AuraScript
{
    PrepareAuraScript(spell_dru_effloresence_aoe);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_EFFLORESCENCE_HEAL });
    }

    void HandleEffectPeriodic(AuraEffect const* aurEff)
    {
        if (Unit* caster = GetCaster())
            if (DynamicObject* dyn = caster->GetDynObject(aurEff->GetId()))
                caster->CastSpell(dyn->GetPositionX(), dyn->GetPositionY(), dyn->GetPositionZ(), SPELL_DRUID_EFFLORESCENCE_HEAL, true, nullptr, aurEff, caster->GetGUID());
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dru_effloresence_aoe::HandleEffectPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 81269 - Efflorescence (heal)
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): the three most injured
// allies are healed for the patch's amount.
class spell_dru_effloresence_heal : public SpellScript
{
    PrepareSpellScript(spell_dru_effloresence_heal);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_EFFLORESCENCE_AOE });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (targets.size() > 3)
        {
            targets.sort(Acore::HealthPctOrderPred());
            targets.resize(3);
        }
    }

    void HandleHeal(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Aura const* aura = caster->GetAura(SPELL_DRUID_EFFLORESCENCE_AOE, caster->GetGUID()))
                if (AuraEffect const* aurEff = aura->GetEffect(EFFECT_1))
                    if (int32 heal = aurEff->GetAmount())
                        SetHitHeal(heal);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_effloresence_heal::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
        OnEffectHitTarget += SpellEffectFn(spell_dru_effloresence_heal::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
    }
};

// -33879 - Empowered Touch
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): Healing Touch, Regrowth and
// Nourish have the talent's effect 1 chance to refresh the druid's Lifebloom on the target.
class spell_dru_empowered_touch : public AuraScript
{
    PrepareAuraScript(spell_dru_empowered_touch);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_EMPOWERED_TOUCH_SCRIPT });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        AuraEffect const* chance = GetEffect(EFFECT_1);
        return chance && eventInfo.GetProcTarget() && roll_chance_i(chance->GetAmount());
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(eventInfo.GetProcTarget(), SPELL_DRUID_EMPOWERED_TOUCH_SCRIPT, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dru_empowered_touch::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_dru_empowered_touch::HandleProc, EFFECT_1, SPELL_AURA_DUMMY);
    }
};

// 88433 - Empowered Touch
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): the refresh also grants
// Replenishment with Revitalize, like recasting Lifebloom.
class spell_dru_empowered_touch_script : public SpellScript
{
    PrepareSpellScript(spell_dru_empowered_touch_script);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_LIFEBLOOM, SPELL_DRUID_REVITALIZE_R1, SPELL_DRUID_REPLENISHMENT });
    }

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (Aura* aura = GetHitUnit()->GetAura(SPELL_DRUID_LIFEBLOOM, caster->GetGUID()))
        {
            aura->RefreshDuration();
            if (caster->GetAuraOfRankedSpell(SPELL_DRUID_REVITALIZE_R1, caster->GetGUID()))
                caster->CastSpell(caster, SPELL_DRUID_REPLENISHMENT, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dru_empowered_touch_script::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 339 - Entangling Roots
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): damage breaks the roots
// after four hits on a target of the caster's level, one more or fewer per two levels of difference (at
// least two), one more in Tree of Life.
class spell_dru_entangling_roots : public AuraScript
{
    PrepareAuraScript(spell_dru_entangling_roots);

    static constexpr int16 DEFAULT_PROCS = 4; // the allowed procs on a target of the caster's level
    static constexpr int16 MINIMUM_PROCS = 2;

    void CalculateProcAmount(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        int16 offset = (int16(caster->GetLevel()) - int16(GetTarget()->GetLevel())) / 2;
        _allowedProcs = uint8(std::max<int16>(MINIMUM_PROCS, DEFAULT_PROCS + offset));
        if (caster->GetShapeshiftForm() == FORM_TREE)
            ++_allowedProcs;
    }

    bool CheckProc(ProcEventInfo& /*eventInfo*/)
    {
        if (_allowedProcs != 0)
            --_allowedProcs;

        return _allowedProcs == 0;
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_dru_entangling_roots::CalculateProcAmount, EFFECT_0, SPELL_AURA_MOD_ROOT, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        DoCheckProc += AuraCheckProcFn(spell_dru_entangling_roots::CheckProc);
    }

private:
    uint8 _allowedProcs = 0;
};

// 16979 - Feral Charge (Bear Form)
// 49376 - Feral Charge (Cat Form)
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): with Stampede, the bear
// charge gives melee haste (81016/81017) and the cat charge a free Ravage! (81021/81022). (own) The rank is
// read from the talent spell itself: the triggered spells have no rank chain.
class spell_dru_feral_charge : public SpellScript
{
    PrepareSpellScript(spell_dru_feral_charge);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_DRUID_STAMPEDE_R1,
                SPELL_DRUID_STAMPEDE_R2,
                SPELL_DRUID_STAMPEDE_BEAR_R1,
                SPELL_DRUID_STAMPEDE_BEAR_R2,
                SPELL_DRUID_STAMPEDE_CAT_R1,
                SPELL_DRUID_STAMPEDE_CAT_R2
            });
    }

    void CastStampede(uint32 rank1SpellId, uint32 rank2SpellId)
    {
        Unit* caster = GetCaster();
        if (caster->HasAura(SPELL_DRUID_STAMPEDE_R2))
            caster->CastSpell(caster, rank2SpellId, true);
        else if (caster->HasAura(SPELL_DRUID_STAMPEDE_R1))
            caster->CastSpell(caster, rank1SpellId, true);
    }

    void HandleBearFormEffect(SpellEffIndex /*effIndex*/)
    {
        CastStampede(SPELL_DRUID_STAMPEDE_BEAR_R1, SPELL_DRUID_STAMPEDE_BEAR_R2);
    }

    void HandleCatFormEffect(SpellEffIndex /*effIndex*/)
    {
        CastStampede(SPELL_DRUID_STAMPEDE_CAT_R1, SPELL_DRUID_STAMPEDE_CAT_R2);
    }

    void Register() override
    {
        if (m_scriptSpellId == SPELL_DRUID_FERAL_CHARGE_BEAR)
            OnEffectLaunchTarget += SpellEffectFn(spell_dru_feral_charge::HandleBearFormEffect, EFFECT_0, SPELL_EFFECT_CHARGE);
        else if (m_scriptSpellId == SPELL_DRUID_FERAL_CHARGE_CAT)
            OnEffectLaunchTarget += SpellEffectFn(spell_dru_feral_charge::HandleCatFormEffect, EFFECT_0, SPELL_EFFECT_TRIGGER_SPELL);
    }
};

// 97985 - Feral Swiftness Clear (Dash, Stampeding Roar in Cat Form)
// 97993 - Feral Swiftness Clear (Stampeding Roar in Bear Form)
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): removes all roots and snares.
class spell_dru_feral_swiftness_clear : public SpellScript
{
    PrepareSpellScript(spell_dru_feral_swiftness_clear);

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        GetHitUnit()->RemoveAppliedAuras([](AuraApplication const* aurApp)
        {
            uint64 mechanicMask = aurApp->GetBase()->GetSpellInfo()->GetSpellMechanicMaskByEffectMask(MAX_EFFECT_MASK);
            return (mechanicMask & ((1ULL << MECHANIC_ROOT) | (1ULL << MECHANIC_SNARE))) != 0;
        });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dru_feral_swiftness_clear::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 22568 - Ferocious Bite
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): 10.9% of attack power per
// combo point, and up to 35 more energy doubling the damage; Glyph of Ferocious Bite heals per 10 energy
// spent. (own) The bonus goes into the effect's value at launch, so the whole hit gets the energy bonus.
class spell_dru_ferocious_bite : public SpellScript
{
    PrepareSpellScript(spell_dru_ferocious_bite);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_GLYPH_OF_FEROCIOUS_BITE });
    }

    bool Load() override
    {
        return GetCaster()->IsPlayer() && GetCaster()->getClass() == CLASS_DRUID;
    }

    void ChangeDamage(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();

        int32 damage = GetEffectValue();
        damage += int32(caster->GetComboPoints() * caster->GetTotalAttackPowerValue(BASE_ATTACK) * 0.109f);

        // converts each extra point of energy (up to 35 energy) into additional damage
        int32 energy = -caster->ModifyPower(POWER_ENERGY, -35);
        AddPct(damage, energy * 100.0f / 35.0f);
        SetEffectValue(damage);

        // Glyph of Ferocious Bite
        if (AuraEffect const* aurEff = caster->GetDummyAuraEffect(SPELLFAMILY_DRUID, SPELL_ICON_ID_GLYPH_OF_FEROCIOUS_BITE, EFFECT_0))
        {
            int32 bp = aurEff->GetAmount() * ((GetSpell()->GetPowerCost() + energy) / 10);
            if (bp > 0)
                caster->CastCustomSpell(caster, SPELL_DRUID_GLYPH_OF_FEROCIOUS_BITE, &bp, nullptr, nullptr, true);
        }
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_dru_ferocious_bite::ChangeDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 99017 - Firebloom
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): heals the most injured ally.
class spell_dru_firebloom : public SpellScript
{
    PrepareSpellScript(spell_dru_firebloom);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (targets.size() < 2)
            return;

        targets.sort(Acore::HealthPctOrderPred());
        targets.resize(1);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_firebloom::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
    }
};

// 77495 - Harmony (restoration mastery)
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): direct heals give the
// mastery's amount as a periodic healing bonus (100977).
class spell_dru_harmony : public AuraScript
{
    PrepareAuraScript(spell_dru_harmony);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_HARMONY });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastCustomSpell(SPELL_DRUID_HARMONY, SPELLVALUE_BASE_POINT0, aurEff->GetAmount(), GetTarget(), true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_harmony::HandleProc, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER);
    }
};

// 100977 - Harmony
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): Bloom (T12 restoration
// bonus) ends with Harmony.
class spell_dru_harmony_triggered : public AuraScript
{
    PrepareAuraScript(spell_dru_harmony_triggered);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_BLOOM });
    }

    void RemoveEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_DRUID_BLOOM);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_dru_harmony_triggered::RemoveEffect, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
    }
};

// 90165 - Item - Druid T11 Feral 4P Bonus
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): Mangle stacks Strength of
// the Panther.
class spell_dru_item_t11_feral_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_dru_item_t11_feral_4p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_STRENGTH_OF_THE_PANTHER });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(GetTarget(), SPELL_DRUID_STRENGTH_OF_THE_PANTHER, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_item_t11_feral_4p_bonus::HandleProc, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER);
    }
};

// -33881 - Nature's Ward (own: neither CPP nor TrinityCore script it). A hit that leaves the druid below 50%
// health casts a free Rejuvenation on them, with the talent's chance (effect 0: 50% at rank 1, 100% at rank 2).
class spell_dru_natures_ward : public AuraScript
{
    PrepareAuraScript(spell_dru_natures_ward);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_REJUVENATION });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetDamage())
            return false;

        return GetTarget()->HealthBelowPctDamaged(50, damageInfo->GetDamage()) && roll_chance_i(GetEffect(EFFECT_0)->GetAmount());
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(GetTarget(), SPELL_DRUID_REJUVENATION, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dru_natures_ward::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_dru_natures_ward::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 6807 - Maul
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): the tooltip's
// 35 + 19% of attack power - 1 replaces the effect's base points.
class spell_dru_maul : public SpellScript
{
    PrepareSpellScript(spell_dru_maul);

    void HandleDamage(SpellEffIndex /*effIndex*/)
    {
        SetEffectValue(int32(35 + GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) * 0.19f - 1));
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_dru_maul::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 16913 - Moonfury
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): losing the balance talent
// (respec) clears the eclipse markers and auras and empties the eclipse bar.
class spell_dru_moonfire : public AuraScript
{
    PrepareAuraScript(spell_dru_moonfire);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_DRUID_LUNAR_ECLIPSE_MARKER,
                SPELL_DRUID_SOLAR_ECLIPSE_MARKER,
                SPELL_DRUID_ECLIPSE_SOLAR,
                SPELL_DRUID_ECLIPSE_LUNAR,
                SPELL_DRUID_SOLAR_ECLIPSE_SUNFIRE
            });
    }

    void RemoveEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        target->RemoveAurasDueToSpell(SPELL_DRUID_SOLAR_ECLIPSE_SUNFIRE);
        target->RemoveAurasDueToSpell(SPELL_DRUID_LUNAR_ECLIPSE_MARKER);
        target->RemoveAurasDueToSpell(SPELL_DRUID_SOLAR_ECLIPSE_MARKER);
        target->RemoveAurasDueToSpell(SPELL_DRUID_ECLIPSE_SOLAR);
        target->RemoveAurasDueToSpell(SPELL_DRUID_ECLIPSE_LUNAR);
        target->SetPower(POWER_ECLIPSE, 0);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_dru_moonfire::RemoveEffect, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
    }
};

// -17074 - Nature's Bounty
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): with the druid's
// Rejuvenation on three or more targets, Nourish casts faster (96206); a 1 sec tick drops the targets
// whose Rejuvenation ended. (own) The cast time reduction is passed negative, as 96206's data has it.
class spell_dru_natures_bounty : public AuraScript
{
    PrepareAuraScript(spell_dru_natures_bounty);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_NATURES_BOUNTY, SPELL_DRUID_REJUVENATION });
    }

    void CalcPeriodic(AuraEffect const* /*aurEff*/, bool& isPeriodic, int32& period)
    {
        isPeriodic = true;
        period = 1 * IN_MILLISECONDS;
    }

    void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        if (Unit* target = eventInfo.GetProcTarget())
            if (std::find(_rejuvenationTargets.begin(), _rejuvenationTargets.end(), target->GetGUID()) == _rejuvenationTargets.end())
                _rejuvenationTargets.push_back(target->GetGUID());

        if (_rejuvenationTargets.size() >= 3 && !_bountyActive)
        {
            int32 bp = -GetSpellInfo()->Effects[EFFECT_1].CalcValue();
            GetTarget()->CastCustomSpell(SPELL_DRUID_NATURES_BOUNTY, SPELLVALUE_BASE_POINT0, bp, GetTarget(), true);
            _bountyActive = true;
        }
    }

    void HandlePeriodic(AuraEffect const* /*aurEff*/)
    {
        Unit* target = GetTarget();
        for (auto itr = _rejuvenationTargets.begin(); itr != _rejuvenationTargets.end();)
        {
            Unit* healTarget = ObjectAccessor::GetUnit(*target, *itr);
            if (!healTarget || !healTarget->HasAura(SPELL_DRUID_REJUVENATION, target->GetGUID()))
                itr = _rejuvenationTargets.erase(itr);
            else
                ++itr;
        }

        if (_bountyActive && _rejuvenationTargets.size() < 3)
        {
            target->RemoveAurasDueToSpell(SPELL_DRUID_NATURES_BOUNTY, target->GetGUID());
            _bountyActive = false;
        }
    }

    void Register() override
    {
        DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_dru_natures_bounty::CalcPeriodic, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER);
        OnEffectProc += AuraEffectProcFn(spell_dru_natures_bounty::HandleProc, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dru_natures_bounty::HandlePeriodic, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER);
    }

private:
    std::vector<ObjectGuid> _rejuvenationTargets;
    bool _bountyActive = false;
};

// 9005 - Pounce
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): Shadowmeld's stealth does
// not allow it, only Prowl.
class spell_dru_pounce : public SpellScript
{
    PrepareSpellScript(spell_dru_pounce);

    SpellCastResult CheckCast()
    {
        if (GetCaster()->HasAura(SPELL_DRUID_SHADOWMELD))
            return SPELL_FAILED_TARGET_AURASTATE;

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_dru_pounce::CheckCast);
    }
};

// 6785 - Ravage
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): as Pounce.
class spell_dru_ravage : public SpellScript
{
    PrepareSpellScript(spell_dru_ravage);

    SpellCastResult CheckCast()
    {
        if (GetCaster()->HasAura(SPELL_DRUID_SHADOWMELD))
            return SPELL_FAILED_TARGET_AURASTATE;

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_dru_ravage::CheckCast);
    }
};

// 80313 - Pulverize
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): the normalized weapon
// damage bonus (effect 2) counts once per Lacerate stack, and consuming the stacks gives 3% melee critical
// strike chance each (80951). AzerothCore sums all weapon effects in one handler, so effect 2's value is
// set before it runs.
class spell_dru_pulverize : public SpellScript
{
    PrepareSpellScript(spell_dru_pulverize);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_PULVERIZE_TRIGGERED, SPELL_DRUID_LACERATE });
    }

    void HandleLacerateBonus(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        uint8 stacks = 0;
        if (Aura const* lacerate = GetHitUnit()->GetAura(SPELL_DRUID_LACERATE, caster->GetGUID()))
            stacks = lacerate->GetStackAmount();

        GetSpell()->SetSpellValue(SPELLVALUE_BASE_POINT2, GetSpellInfo()->Effects[EFFECT_2].CalcValue(caster) * stacks);
    }

    void HandleLacerateRemoval(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (Aura* lacerate = GetHitUnit()->GetAura(SPELL_DRUID_LACERATE, caster->GetGUID()))
        {
            int32 critPerStack = sSpellMgr->GetSpellInfo(SPELL_DRUID_PULVERIZE_TRIGGERED)->Effects[EFFECT_0].CalcValue(caster);
            caster->CastCustomSpell(SPELL_DRUID_PULVERIZE_TRIGGERED, SPELLVALUE_BASE_POINT0, critPerStack * lacerate->GetStackAmount(), caster, TRIGGERED_FULL_MASK);
            lacerate->Remove();
        }
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_dru_pulverize::HandleLacerateBonus, EFFECT_0, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
        OnEffectHitTarget += SpellEffectFn(spell_dru_pulverize::HandleLacerateRemoval, EFFECT_0, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
    }
};

// 8936 - Regrowth
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): with the Glyph of Regrowth
// the last tick refreshes Regrowth on a target at or below the glyph's health percentage.
class spell_dru_regrowth : public AuraScript
{
    PrepareAuraScript(spell_dru_regrowth);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_REGROWTH_REFRESH });
    }

    void HandleGlyphEffect(AuraEffect const* aurEff)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        if (aurEff->GetTickNumber() < aurEff->GetTotalTicks() - 1)
            return;

        if (AuraEffect const* glyphEffect = caster->GetDummyAuraEffect(SPELLFAMILY_DRUID, SPELL_ICON_ID_GLYPH_OF_REGROWTH, EFFECT_0))
            if (GetTarget()->GetHealthPct() <= float(glyphEffect->GetAmount()))
                caster->CastSpell(GetTarget(), SPELL_DRUID_REGROWTH_REFRESH, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dru_regrowth::HandleGlyphEffect, EFFECT_1, SPELL_AURA_PERIODIC_HEAL);
    }
};

// 93036 - Regrowth Refresh
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): a plain refresh with rolling
// ticks (sniffed), not the 6 sec the script effect suggests.
class spell_dru_regrowth_refresh : public SpellScript
{
    PrepareSpellScript(spell_dru_regrowth_refresh);

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        if (AuraEffect* aurEff = GetHitUnit()->GetAuraEffect(SPELL_AURA_PERIODIC_HEAL, SPELLFAMILY_DRUID, 0x40, 0, 0, GetCaster()->GetGUID()))
            aurEff->GetBase()->RefreshDuration();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dru_regrowth_refresh::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 774 - Rejuvenation
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): Gift of the Earthmother
// heals at once for its share of Rejuvenation's whole periodic healing.
class spell_dru_rejuvenation : public AuraScript
{
    PrepareAuraScript(spell_dru_rejuvenation);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_REJUVENATION_DIRECT_HEAL });
    }

    void HandleGiftOfTheEarthmother(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        if (AuraEffect const* giftEffect = caster->GetDummyAuraEffect(SPELLFAMILY_DRUID, SPELL_ICON_ID_GIFT_OF_THE_EARTHMOTHER, EFFECT_1))
        {
            int32 bp = CalculatePct(aurEff->GetAmount() * int32(aurEff->GetTotalTicks()), giftEffect->GetAmount());
            if (bp > 0)
                caster->CastCustomSpell(GetTarget(), SPELL_DRUID_REJUVENATION_DIRECT_HEAL, &bp, nullptr, nullptr, true, nullptr, aurEff);
        }
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_dru_rejuvenation::HandleGiftOfTheEarthmother, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }
};

// 93400 - Shooting Stars
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): resets Starsurge.
class spell_dru_shooting_stars : public SpellScript
{
    PrepareSpellScript(spell_dru_shooting_stars);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_STARSURGE });
    }

    void HandleCooldownReset(SpellEffIndex /*effIndex*/)
    {
        if (Player* player = GetHitUnit()->ToPlayer())
            player->RemoveSpellCooldown(SPELL_DRUID_STARSURGE, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dru_shooting_stars::HandleCooldownReset, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// 80964 - Skull Bash (Bear Form)
// 80965 - Skull Bash (Cat Form)
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): charge, interrupt, and with
// Brutal Impact a cost increase on the target's spells. (own) The three are cast triggered.
class spell_dru_skull_bash : public SpellScript
{
    PrepareSpellScript(spell_dru_skull_bash);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_DRUID_SKULL_BASH_CHARGE,
                SPELL_DRUID_SKULL_BASH_INTERRUPT,
                SPELL_DRUID_SKULL_BASH_COST_INCREASE_R1,
                SPELL_DRUID_SKULL_BASH_COST_INCREASE_R2,
                SPELL_DRUID_BRUTAL_IMPACT_R1
            });
    }

    void HandleDummyEffect(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        caster->CastSpell(target, SPELL_DRUID_SKULL_BASH_CHARGE, true);
        caster->CastSpell(target, SPELL_DRUID_SKULL_BASH_INTERRUPT, true);
        if (Aura const* aura = caster->GetAuraOfRankedSpell(SPELL_DRUID_BRUTAL_IMPACT_R1, caster->GetGUID()))
            caster->CastSpell(target, aura->GetSpellInfo()->GetRank() == 1 ? SPELL_DRUID_SKULL_BASH_COST_INCREASE_R1 : SPELL_DRUID_SKULL_BASH_COST_INCREASE_R2, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dru_skull_bash::HandleDummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 78675 - Solar Beam
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): every half second the beam
// silences the enemies in it.
class spell_dru_solar_beam : public AuraScript
{
    PrepareAuraScript(spell_dru_solar_beam);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_SOLAR_BEAM_SILENCE });
    }

    void HandleEffectPeriodic(AuraEffect const* aurEff)
    {
        if (DynamicObject* dyn = GetTarget()->GetDynObject(aurEff->GetId()))
            GetTarget()->CastSpell(dyn->GetPositionX(), dyn->GetPositionY(), dyn->GetPositionZ(), SPELL_DRUID_SOLAR_BEAM_SILENCE, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dru_solar_beam::HandleEffectPeriodic, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 81021 - Stampede (Ravage!, rank 1)
// 81022 - Stampede (Ravage!, rank 2)
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): the Ravage marker lasts as
// long as the free Ravage!.
class spell_dru_stampede_triggered : public AuraScript
{
    PrepareAuraScript(spell_dru_stampede_triggered);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_STAMPEDE_RAVAGE_MARKER });
    }

    void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->CastSpell(GetTarget(), SPELL_DRUID_STAMPEDE_RAVAGE_MARKER, true);
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_DRUID_STAMPEDE_RAVAGE_MARKER, GetTarget()->GetGUID());
    }

    void Register() override
    {
        // SPELL_AURA_ANY: the two ranks use different aura types on effect 0
        AfterEffectApply += AuraEffectApplyFn(spell_dru_stampede_triggered::AfterApply, EFFECT_0, SPELL_AURA_ANY, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_dru_stampede_triggered::AfterRemove, EFFECT_0, SPELL_AURA_ANY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 77761 - Stampeding Roar (Bear Form)
// 77764 - Stampeding Roar (Cat Form)
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): Feral Swiftness gives the
// roar its chance to clear roots and snares from everyone it hits.
class spell_dru_stampeding_roar : public SpellScript
{
    PrepareSpellScript(spell_dru_stampeding_roar);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_FERAL_SWIFTNESS_R1, SPELL_DRUID_FERAL_SWIFTNESS_CLEAR_ROAR });
    }

    void HandleFeralSwiftness(SpellEffIndex /*effIndex*/)
    {
        Unit* target = GetHitUnit();
        if (GetCaster() != target)
            return;

        if (Aura const* swiftness = target->GetAuraOfRankedSpell(SPELL_DRUID_FERAL_SWIFTNESS_R1, target->GetGUID()))
            if (roll_chance_i(swiftness->GetSpellInfo()->Effects[EFFECT_1].CalcValue()))
                target->CastSpell(target, SPELL_DRUID_FERAL_SWIFTNESS_CLEAR_ROAR, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dru_stampeding_roar::HandleFeralSwiftness, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// 99015 - Item - Druid T12 Restoration 4P Bonus
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): Swiftmend's effective heal
// is repeated on an injured ally nearby (Firebloom).
class spell_dru_t12_restoration_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_dru_t12_restoration_4p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_FIREBLOOM });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        HealInfo* healInfo = eventInfo.GetHealInfo();
        if (!healInfo || !eventInfo.GetProcTarget())
            return;

        int32 heal = int32(healInfo->GetEffectiveHeal());
        if (heal > 0)
            GetTarget()->CastCustomSpell(SPELL_DRUID_FIREBLOOM, SPELLVALUE_BASE_POINT0, heal, eventInfo.GetProcTarget(), true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_t12_restoration_4p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 33891 - Tree of Life
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): the form's three passives.
class spell_dru_tree_of_life : public AuraScript
{
    PrepareAuraScript(spell_dru_tree_of_life);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_DRUID_TREE_OF_LIFE_PASSIVE_1,
                SPELL_DRUID_TREE_OF_LIFE_PASSIVE_2,
                SPELL_DRUID_TREE_OF_LIFE_PASSIVE_3
            });
    }

    void AfterApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        target->CastSpell(target, SPELL_DRUID_TREE_OF_LIFE_PASSIVE_1, true, nullptr, aurEff);
        target->CastSpell(target, SPELL_DRUID_TREE_OF_LIFE_PASSIVE_2, true, nullptr, aurEff);
        target->CastSpell(target, SPELL_DRUID_TREE_OF_LIFE_PASSIVE_3, true, nullptr, aurEff);
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        target->RemoveAurasDueToSpell(SPELL_DRUID_TREE_OF_LIFE_PASSIVE_1);
        target->RemoveAurasDueToSpell(SPELL_DRUID_TREE_OF_LIFE_PASSIVE_2);
        target->RemoveAurasDueToSpell(SPELL_DRUID_TREE_OF_LIFE_PASSIVE_3);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_dru_tree_of_life::AfterApply, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_dru_tree_of_life::AfterRemove, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
    }
};

// Wild Mushrooms are summoned as plain allied summons (SummonProperties 3069: no slot, no minion title), which
// neither AzerothCore nor CPP registers on the caster (CPP's GetAllMinionsByEntry never finds them). Each druid's
// mushrooms are kept by guid here, at any distance on the druid's map; a new one is picked up right after the
// cast, where it stands next to its target spot.
static constexpr float WILD_MUSHROOM_PICKUP_RANGE = 100.0f;
static std::unordered_map<ObjectGuid, std::vector<ObjectGuid>> sWildMushrooms;
static std::mutex sWildMushroomsLock;   // maps update on several threads

static std::list<TempSummon*> GetWildMushrooms(Unit* caster)
{
    std::list<TempSummon*> mushrooms;
    std::lock_guard<std::mutex> guard(sWildMushroomsLock);
    auto it = sWildMushrooms.find(caster->GetGUID());
    if (it == sWildMushrooms.end())
        return mushrooms;

    // drop the ones that despawned, died, were detonated or are on another map
    std::vector<ObjectGuid>& guids = it->second;
    for (auto guid = guids.begin(); guid != guids.end();)
    {
        Creature* creature = ObjectAccessor::GetCreature(*caster, *guid);
        TempSummon* summon = creature ? creature->ToTempSummon() : nullptr;
        if (summon && summon->IsAlive() && summon->GetSummonerGUID() == caster->GetGUID())
        {
            mushrooms.push_back(summon);
            ++guid;
        }
        else
            guid = guids.erase(guid);
    }

    if (guids.empty())
        sWildMushrooms.erase(it);
    return mushrooms;
}

static void ForgetWildMushroom(Unit* caster, ObjectGuid const& mushroom)
{
    std::lock_guard<std::mutex> guard(sWildMushroomsLock);
    auto it = sWildMushrooms.find(caster->GetGUID());
    if (it == sWildMushrooms.end())
        return;
    std::erase(it->second, mushroom);
    if (it->second.empty())
        sWildMushrooms.erase(it);
}

static void PickUpNewWildMushrooms(Unit* caster)
{
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(SPELL_DRUID_WILD_MUSHROOM);
    if (!spellInfo)
        return;

    std::list<Creature*> creatures;
    caster->GetCreatureListWithEntryInGrid(creatures, uint32(spellInfo->Effects[EFFECT_0].MiscValue), WILD_MUSHROOM_PICKUP_RANGE);
    std::lock_guard<std::mutex> guard(sWildMushroomsLock);
    std::vector<ObjectGuid>& guids = sWildMushrooms[caster->GetGUID()];
    for (Creature* creature : creatures)
        if (TempSummon* summon = creature->ToTempSummon())
            if (summon->IsAlive() && summon->GetSummonerGUID() == caster->GetGUID() &&
                std::find(guids.begin(), guids.end(), summon->GetGUID()) == guids.end())
                guids.push_back(summon->GetGUID());
}

// 88747 - Wild Mushroom
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): at most three mushrooms
// (effect 0's value); a fourth replaces the oldest.
class spell_dru_wild_mushroom : public SpellScript
{
    PrepareSpellScript(spell_dru_wild_mushroom);

    void HandleSummon()
    {
        PickUpNewWildMushrooms(GetCaster());
        std::list<TempSummon*> mushrooms = GetWildMushrooms(GetCaster());
        if (mushrooms.size() <= uint32(std::max<int32>(GetSpellInfo()->Effects[EFFECT_0].BasePoints, 0)))
            return;

        TempSummon* oldestMushroom = nullptr;
        for (TempSummon* mushroom : mushrooms)
            if (!oldestMushroom || mushroom->GetTimer() < oldestMushroom->GetTimer())
                oldestMushroom = mushroom;

        if (oldestMushroom)
        {
            ForgetWildMushroom(GetCaster(), oldestMushroom->GetGUID());
            oldestMushroom->UnSummon();
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_dru_wild_mushroom::HandleSummon);
    }
};

// 88751 - Wild Mushroom: Detonate
// Ported from The Cataclysm Preservation Project (spell_druid.cpp at b061f46): every mushroom explodes,
// leaving Fungal Growth with the talent. (own) The damage and Fungal Growth are cast at the mushroom's
// position (both are destination spells).
class spell_dru_wild_mushroom_detonate : public SpellScript
{
    PrepareSpellScript(spell_dru_wild_mushroom_detonate);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_DRUID_WILD_MUSHROOM,
                SPELL_DRUID_WILD_MUSHROOM_DAMAGE,
                SPELL_DRUID_WILD_MUSHROOM_SUICIDE,
                SPELL_DRUID_WILD_MUSHROOM_VISUAL,
                SPELL_DRUID_FUNGAL_GROWTH_R1,
                SPELL_DRUID_FUNGAL_GROWTH_R2,
                SPELL_DRUID_FUNGAL_GROWTH_SUMMON_R1,
                SPELL_DRUID_FUNGAL_GROWTH_SUMMON_R2
            });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        for (TempSummon* mushroom : GetWildMushrooms(caster))
        {
            float x = mushroom->GetPositionX();
            float y = mushroom->GetPositionY();
            float z = mushroom->GetPositionZ();

            if (caster->HasAura(SPELL_DRUID_FUNGAL_GROWTH_R1))
                caster->CastSpell(x, y, z, SPELL_DRUID_FUNGAL_GROWTH_SUMMON_R1, true);
            else if (caster->HasAura(SPELL_DRUID_FUNGAL_GROWTH_R2))
                caster->CastSpell(x, y, z, SPELL_DRUID_FUNGAL_GROWTH_SUMMON_R2, true);

            caster->CastSpell(x, y, z, SPELL_DRUID_WILD_MUSHROOM_DAMAGE, true);
            mushroom->CastSpell(mushroom, SPELL_DRUID_WILD_MUSHROOM_VISUAL, true);
            mushroom->CastSpell(mushroom, SPELL_DRUID_WILD_MUSHROOM_SUICIDE, true);
            // it stays for its visual; a second Detonate must not find it again
            ForgetWildMushroom(caster, mushroom->GetGUID());
            mushroom->UnSummon(1500ms);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dru_wild_mushroom_detonate::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

void AddSC_druid_spell_scripts()
{
    RegisterSpellScript(spell_dru_t10_balance_4p_bonus);
    RegisterSpellScript(spell_dru_nurturing_instinct);
    RegisterSpellScript(spell_dru_feral_swiftness);
    RegisterSpellScript(spell_dru_cat_form);
    RegisterSpellScript(spell_dru_omen_of_clarity);
    RegisterSpellScript(spell_dru_barkskin);
    RegisterSpellScript(spell_dru_glyph_of_barkskin);
    RegisterSpellScript(spell_dru_treant_scaling);
    RegisterSpellScript(spell_dru_berserk);
    RegisterSpellAndAuraScriptPair(spell_dru_dash, spell_dru_dash_aura);
    RegisterSpellScript(spell_dru_enrage);
    RegisterSpellScript(spell_dru_glyph_of_starfire);
    RegisterSpellScript(spell_dru_glyph_of_starfire_proc);
    RegisterSpellScript(spell_dru_idol_lifebloom);
    RegisterSpellScript(spell_dru_innervate);
    RegisterSpellScript(spell_dru_lifebloom);
    RegisterSpellScript(spell_dru_living_seed);
    RegisterSpellScript(spell_dru_living_seed_proc);
    RegisterSpellScript(spell_dru_rip);
    RegisterSpellScript(spell_dru_savage_defense);
    RegisterSpellAndAuraScriptPair(spell_dru_savage_roar, spell_dru_savage_roar_aura);
    RegisterSpellScript(spell_dru_starfall_dummy);
    RegisterSpellAndAuraScriptPair(spell_dru_survival_instincts, spell_dru_survival_instincts_aura);
    RegisterSpellScript(spell_dru_swift_flight_passive);
    RegisterSpellScript(spell_dru_tiger_s_fury);
    RegisterSpellScript(spell_dru_typhoon);
    RegisterSpellScript(spell_dru_t10_restoration_4p_bonus);
    RegisterSpellAndAuraScriptPair(spell_dru_wild_growth, spell_dru_wild_growth_aura);
    RegisterSpellScript(spell_dru_rejuvenation_moonglade_2_set);
    RegisterSpellScript(spell_dru_leader_of_the_pack);
    RegisterSpellScript(spell_dru_eclipse);
    RegisterSpellScript(spell_dru_eclipse_mastery_driver_passive);
    RegisterSpellScript(spell_dru_t3_2p_bonus);
    RegisterSpellScript(spell_dru_t3_6p_bonus);
    RegisterSpellScript(spell_dru_t3_8p_bonus);
    RegisterSpellScript(spell_dru_t4_2p_bonus);
    RegisterSpellScript(spell_dru_item_t6_trinket);
    RegisterSpellScript(spell_dru_glyph_of_shred);
    RegisterSpellScript(spell_dru_t10_restoration_4p_bonus_dummy);
    RegisterSpellScript(spell_dru_maim_interrupt);
    RegisterSpellScript(spell_dru_t9_feral_relic);
    RegisterSpellScript(spell_dru_frenzied_regeneration);
    RegisterSpellScript(spell_dru_insect_swarm);
    RegisterSpellScript(spell_dru_nourish);
    // Cataclysm (4.3.4), from CPP
    RegisterSpellScript(spell_dru_astral_alignment);
    RegisterSpellScript(spell_dru_blood_in_the_water);
    RegisterSpellScript(spell_dru_blood_in_the_water_script);
    RegisterSpellScript(spell_dru_effloresence);
    RegisterSpellScript(spell_dru_effloresence_aoe);
    RegisterSpellScript(spell_dru_effloresence_heal);
    RegisterSpellScript(spell_dru_empowered_touch);
    RegisterSpellScript(spell_dru_empowered_touch_script);
    RegisterSpellScript(spell_dru_entangling_roots);
    RegisterSpellScript(spell_dru_feral_charge);
    RegisterSpellScript(spell_dru_feral_swiftness_clear);
    RegisterSpellScript(spell_dru_ferocious_bite);
    RegisterSpellScript(spell_dru_firebloom);
    RegisterSpellScript(spell_dru_flight_form);
    RegisterSpellScript(spell_dru_harmony);
    RegisterSpellScript(spell_dru_harmony_triggered);
    RegisterSpellScript(spell_dru_item_t11_feral_4p_bonus);
    RegisterSpellScript(spell_dru_maul);
    RegisterSpellScript(spell_dru_natures_ward);
    RegisterSpellScript(spell_dru_moonfire);
    RegisterSpellScript(spell_dru_natures_bounty);
    RegisterSpellScript(spell_dru_pounce);
    RegisterSpellScript(spell_dru_pulverize);
    RegisterSpellScript(spell_dru_ravage);
    RegisterSpellScript(spell_dru_regrowth);
    RegisterSpellScript(spell_dru_regrowth_refresh);
    RegisterSpellScript(spell_dru_rejuvenation);
    RegisterSpellScript(spell_dru_shooting_stars);
    RegisterSpellScript(spell_dru_skull_bash);
    RegisterSpellScript(spell_dru_solar_beam);
    RegisterSpellScript(spell_dru_stampede_triggered);
    RegisterSpellScript(spell_dru_stampeding_roar);
    RegisterSpellScript(spell_dru_swift_flight_form);
    RegisterSpellScript(spell_dru_t12_restoration_4p_bonus);
    RegisterSpellScript(spell_dru_tree_of_life);
    RegisterSpellScript(spell_dru_wild_mushroom);
    RegisterSpellScript(spell_dru_wild_mushroom_detonate);
}
