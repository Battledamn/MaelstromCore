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

#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Group.h"
#include "Item.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "SpellScriptLoader.h"
#include "TemporarySummon.h"
#include "Unit.h"
/*
 * Scripts for spells with SPELLFAMILY_SHAMAN and SPELLFAMILY_GENERIC spells used by shaman players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_sha_".
 *
 * 4.3.4: the scripts of The Cataclysm Preservation Project (spell_shaman.cpp and the shaman pet scaling of
 * spell_pet.cpp at b061f46) in AzerothCore's script API, with AzerothCore's still right in 4.3.4.
 */

enum ShamanSpells
{
    SPELL_SHAMAN_ANCESTRAL_AWAKENING            = 52759,
    SPELL_SHAMAN_ANCESTRAL_AWAKENING_PROC       = 52752,
    SPELL_SHAMAN_ANCESTRAL_VIGOR                = 105284,
    SPELL_SHAMAN_SEASONED_WINDS_R1              = 16086,
    SPELL_SHAMAN_SPIRIT_LINK_TOTEM_AURA         = 98007,
    SPELL_SHAMAN_SEASONED_WINDS_FIRE            = 97618,
    SPELL_SHAMAN_SEASONED_WINDS_FROST           = 97619,
    SPELL_SHAMAN_SEASONED_WINDS_NATURE          = 97620,
    SPELL_SHAMAN_SEASONED_WINDS_ARCANE          = 97621,
    SPELL_SHAMAN_SEASONED_WINDS_SHADOW          = 97622,
    SPELL_SHAMAN_BLESSING_OF_THE_ETERNALS_R1    = 51554,
    SPELL_SHAMAN_CHAIN_HEAL                     = 1064,
    SPELL_SHAMAN_CHAIN_LIGHTNING                = 421,
    SPELL_SHAMAN_CHAIN_LIGHTNING_TRIGGERED      = 45297,
    SPELL_SHAMAN_CLEANSING_WATERS_DUMMY_R1      = 86959,
    SPELL_SHAMAN_CLEANSING_WATERS_DUMMY_R2      = 86962,
    SPELL_SHAMAN_CLEANSING_WATERS_HEAL_R1       = 86961,
    SPELL_SHAMAN_CLEANSING_WATERS_HEAL_R2       = 86958,
    SPELL_SHAMAN_EARTH_SHIELD_HEAL              = 379,
    SPELL_SHAMAN_EARTHQUAKE_DAMAGE              = 77478,
    SPELL_SHAMAN_EARTHQUAKE_STUN                = 77505,
    SPELL_SHAMAN_ELECTRIFIED                    = 64930,
    SPELL_SHAMAN_ELEMENTAL_MASTERY              = 16166,
    SPELL_SHAMAN_ELEMENTAL_OATH                 = 51466,
    SPELL_SHAMAN_ENERGY_SURGE                   = 40465,
    SPELL_SHAMAN_EXHAUSTION                     = 57723,
    SPELL_SHAMAN_FIRE_NOVA_DAMAGE               = 8349,
    SPELL_SHAMAN_FLAME_SHOCK                    = 8050,
    SPELL_SHAMAN_FLAMETONGUE_ATTACK             = 10444,
    SPELL_SHAMAN_FLAMETONGUE_WEAPON             = 8024,
    SPELL_SHAMAN_FOCUSED_INSIGHT                = 77800,
    SPELL_SHAMAN_FREEZE                         = 63685,
    SPELL_SHAMAN_FULMINATION                    = 88766,
    SPELL_SHAMAN_FULMINATION_DAMAGE             = 88767,
    SPELL_SHAMAN_FULMINATION_PROC               = 95774,
    SPELL_SHAMAN_GLYPH_OF_EARTH_SHIELD          = 63279,
    SPELL_SHAMAN_GLYPH_OF_HEALING_WAVE          = 55533,
    SPELL_SHAMAN_GLYPH_OF_LIGHTNING_SHIELD      = 55448,
    SPELL_SHAMAN_GLYPH_OF_STONECLAW_TOTEM       = 63298,
    SPELL_SHAMAN_GLYPH_OF_THUNDERSTORM          = 62132,
    SPELL_SHAMAN_HEALING_RAIN_TRIGGERED         = 73921,
    SPELL_SHAMAN_HEALING_SURGE                  = 8004,
    SPELL_SHAMAN_IMPROVED_LAVA_LASH_R1          = 77700,
    SPELL_SHAMAN_ITEM_LIGHTNING_SHIELD          = 23552,
    SPELL_SHAMAN_ITEM_LIGHTNING_SHIELD_DAMAGE   = 27635,
    SPELL_SHAMAN_ITEM_MANA_SURGE                = 23571,
    SPELL_SHAMAN_ITEM_SHAMAN_T12_ELEMENTAL_4P   = 99206,
    SPELL_SHAMAN_LAVA_BURST                     = 51505,
    SPELL_SHAMAN_LAVA_BURST_BONUS_DAMAGE        = 71824,
    SPELL_SHAMAN_LAVA_BURST_TRIGGERED           = 77451,
    SPELL_SHAMAN_LAVA_FLOWS_R1                  = 51480,
    SPELL_SHAMAN_LAVA_FLOWS_TRIGGERED_R1        = 65264,
    SPELL_SHAMAN_LAVA_LASH_SPREAD_FLAME_SHOCK   = 105792,
    SPELL_SHAMAN_LAVA_SURGE                     = 77762,
    SPELL_SHAMAN_LIGHTNING_BOLT                 = 403,
    SPELL_SHAMAN_LIGHTNING_BOLT_OVERLOAD        = 45284,
    SPELL_SHAMAN_LIGHTNING_SHIELD               = 324,
    SPELL_SHAMAN_LIGHTNING_SHIELD_DAMAGE        = 26364,
    SPELL_SHAMAN_MAELSTROM_DUMMY                = 60349,
    SPELL_SHAMAN_MAELSTROM_POWER                = 70831,
    SPELL_SHAMAN_NATURE_GUARDIAN                = 31616,
    SPELL_SHAMAN_POWER_SURGE                    = 40466,
    SPELL_SHAMAN_RESURGENCE_ENERGIZE            = 101033,
    SPELL_SHAMAN_RIPTIDE                        = 61295,
    SPELL_SHAMAN_SATED                          = 57724,
    SPELL_SHAMAN_SEARING_FLAMES_DAMAGE          = 77661,
    SPELL_SHAMAN_SEARING_FLAMES_R1              = 77655,
    SPELL_SHAMAN_SOOTHING_RAINS_R1              = 16187,
    SPELL_SHAMAN_SPIRIT_HUNT_HEAL               = 58879,
    SPELL_SHAMAN_STONECLAW_TOTEM                = 55277,
    SPELL_SHAMAN_STORM_EARTH_AND_FIRE           = 51483,
    SPELL_SHAMAN_T10_ENHANCEMENT_4P_BONUS       = 70832,
    SPELL_SHAMAN_TELLURIC_CURRENTS              = 82987,
    SPELL_SHAMAN_THUNDERSTORM_SLOW              = 100955,
    SPELL_SHAMAN_TIDAL_FORCE_CRIT               = 55166,
    SPELL_SHAMAN_TIDAL_WAVES                    = 53390,
    SPELL_SHAMAN_TOTEM_EARTHBIND_EARTHGRAB      = 64695,
    SPELL_SHAMAN_TOTEM_EARTHBIND_TOTEM          = 6474,
    SPELL_SHAMAN_TOTEM_EARTHEN_POWER            = 59566,
    SPELL_SHAMAN_TOTEM_HEALING_STREAM_HEAL      = 52042,
    SPELL_SHAMAN_TOTEMIC_MASTERY                = 38437,
    SPELL_SHAMAN_TOTEMIC_POWER_MP5              = 28824,
    SPELL_SHAMAN_TOTEMIC_POWER_SPELL_POWER      = 28825,
    SPELL_SHAMAN_TOTEMIC_POWER_ATTACK_POWER     = 28826,
    SPELL_SHAMAN_TOTEMIC_POWER_ARMOR            = 28827,
    SPELL_SHAMAN_UNLEASH_EARTH                  = 73684,
    SPELL_SHAMAN_UNLEASH_FLAME                  = 73683,
    SPELL_SHAMAN_UNLEASH_FROST                  = 73682,
    SPELL_SHAMAN_UNLEASH_LIFE                   = 73685,
    SPELL_SHAMAN_UNLEASH_WIND                   = 73681,
    SPELL_SHAMAN_VOLCANO                        = 99207,
    SPELL_SHAMAN_WATER_SHIELD                   = 52127,
    SPELL_SHAMAN_WINDFURY_ATTACK_MAINHAND       = 25504,
    SPELL_SHAMAN_WINDFURY_ATTACK_OFFHAND        = 33750,
    SPELL_SHAMAN_WINDFURY_WEAPON                = 8232,

    SPELL_HUNTER_INSANITY                       = 95809,
    SPELL_MAGE_TEMPORAL_DISPLACEMENT            = 80354,

    // AzerothCore's names, used by the scripts kept from it
    SPELL_SHAMAN_LAVA_LASH                      = 60103,
    SPELL_SHAMAN_STORMSTRIKE                    = 17364
};

enum ShamanSpellCategories
{
    SHAMAN_SPELL_CATEGORY_SHOCK_SPELLS          = 19
};

enum ShamanEnchantmentIds
{
    SHAMAN_ENCHANTMENT_ID_FLAMETONGUE           = 5,
    SHAMAN_ENCHANTMENT_ID_EARTHLIVING           = 3345,
    SHAMAN_ENCHANTMENT_ID_FROSTBRAND            = 2,
    SHAMAN_ENCHANTMENT_ID_ROCKBITER             = 3021,
    SHAMAN_ENCHANTMENT_ID_WINDFURY              = 283
};

// -51556 - Ancestral Awakening
class spell_sha_ancestral_awakening : public AuraScript
{
    PrepareAuraScript(spell_sha_ancestral_awakening);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_ANCESTRAL_AWAKENING });
    }

    void HandleEffectProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        HealInfo* healInfo = eventInfo.GetHealInfo();
        if (!healInfo)
            return;

        int32 heal = int32(CalculatePct(healInfo->GetEffectiveHeal(), aurEff->GetAmount()));
        if (heal)
            GetTarget()->CastCustomSpell(SPELL_SHAMAN_ANCESTRAL_AWAKENING, SPELLVALUE_BASE_POINT0, heal, nullptr, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_ancestral_awakening::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -16176 - Ancestral Healing: Ancestral Fortitude on critical heals, Ancestral Vigor (up to 10% of the target's
// health) on every heal
class spell_sha_ancestral_healing : public AuraScript
{
    PrepareAuraScript(spell_sha_ancestral_healing);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_ANCESTRAL_VIGOR });
    }

    void HandleAncestralFortitude(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        if (!(eventInfo.GetHitMask() & PROC_HIT_CRITICAL))
            PreventDefaultAction();
    }

    void HandleAncestralVigor(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        HealInfo* healInfo = eventInfo.GetHealInfo();
        if (!healInfo)
            return;

        Unit* target = healInfo->GetTarget();
        if (!target)
            return;

        int32 healBonus = CalculatePct(int32(healInfo->GetEffectiveHeal()), aurEff->GetAmount());
        int32 healthCap = CalculatePct(int32(target->GetMaxHealth()), 10);

        if (Aura* oldVigor = target->GetAura(SPELL_SHAMAN_ANCESTRAL_VIGOR, GetTarget()->GetGUID()))
            if (AuraEffect const* oldEff = oldVigor->GetEffect(EFFECT_0))
                healBonus += oldEff->GetAmount();

        healBonus = std::min(healBonus, healthCap);
        if (healBonus)
            GetTarget()->CastCustomSpell(SPELL_SHAMAN_ANCESTRAL_VIGOR, SPELLVALUE_BASE_POINT0, healBonus, target, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_ancestral_healing::HandleAncestralFortitude, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
        OnEffectProc += AuraEffectProcFn(spell_sha_ancestral_healing::HandleAncestralVigor, EFFECT_1, SPELL_AURA_DUMMY);
    }
};

// -77829 - Ancestral Resolve: less damage taken while casting
class spell_sha_ancestral_resolve : public AuraScript
{
    PrepareAuraScript(spell_sha_ancestral_resolve);

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        amount = -1;
    }

    void Absorb(AuraEffect* aurEff, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        if (GetTarget()->HasUnitState(UNIT_STATE_CASTING))
            absorbAmount = CalculatePct(dmgInfo.GetDamage(), aurEff->GetBaseAmount());
        else
            absorbAmount = 0;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_ancestral_resolve::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_sha_ancestral_resolve::Absorb, EFFECT_0);
    }
};

// 2825 - Bloodlust: not on targets that had one of the 4.3.4 haste buffs (Sated, Exhaustion, Insanity, Temporal
// Displacement)
class spell_sha_bloodlust : public SpellScript
{
    PrepareSpellScript(spell_sha_bloodlust);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_SATED, SPELL_SHAMAN_EXHAUSTION, SPELL_HUNTER_INSANITY, SPELL_MAGE_TEMPORAL_DISPLACEMENT });
    }

    void RemoveInvalidTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(Acore::UnitAuraCheck(true, SPELL_SHAMAN_SATED));
        targets.remove_if(Acore::UnitAuraCheck(true, SPELL_SHAMAN_EXHAUSTION));
        targets.remove_if(Acore::UnitAuraCheck(true, SPELL_HUNTER_INSANITY));
        targets.remove_if(Acore::UnitAuraCheck(true, SPELL_MAGE_TEMPORAL_DISPLACEMENT));
    }

    void ApplyDebuff()
    {
        if (Unit* target = GetHitUnit())
            target->CastSpell(target, SPELL_SHAMAN_SATED, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_bloodlust::RemoveInvalidTargets, EFFECT_ALL, TARGET_UNIT_CASTER_AREA_RAID);
        AfterHit += SpellHitFn(spell_sha_bloodlust::ApplyDebuff);
    }
};

// 1064 - Chain Heal: consumes the first target's Riptide for Riptide's effect 2 more healing on every target
class spell_sha_chain_heal : public SpellScript
{
    PrepareSpellScript(spell_sha_chain_heal);

    void HandleHeal(SpellEffIndex /*effIndex*/)
    {
        if (_firstHeal)
        {
            if (AuraEffect* aurEff = GetHitUnit()->GetAuraEffect(SPELL_AURA_PERIODIC_HEAL, SPELLFAMILY_SHAMAN, 0, 0, 0x10, GetCaster()->GetGUID()))
            {
                _riptide = true;
                _amount = aurEff->GetSpellInfo()->Effects[EFFECT_2].CalcValue();
                GetHitUnit()->RemoveAura(aurEff->GetBase());
            }
            _firstHeal = false;
        }

        if (_riptide)
            SetHitHeal(GetHitHeal() + CalculatePct(GetHitHeal(), _amount));
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_chain_heal::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
    }

private:
    bool _firstHeal = true;
    bool _riptide = false;
    int32 _amount = 0;
};

// 51886 - Cleanse Spirit: Cleansing Waters' heal, once every 6 s. AzerothCore has no hook for a successful
// dispel (CPP's OnEffectSuccessfulDispel); the cast already needs something to dispel.
class spell_sha_cleanse_spirit : public SpellScript
{
    PrepareSpellScript(spell_sha_cleanse_spirit);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_CLEANSING_WATERS_HEAL_R1, SPELL_SHAMAN_CLEANSING_WATERS_HEAL_R2 });
    }

    void HandleDispel(SpellEffIndex /*effIndex*/)
    {
        Player* player = GetCaster()->ToPlayer();
        if (!player || player->HasSpellCooldown(SPELL_SHAMAN_CLEANSING_WATERS_HEAL_R1))
            return;

        uint32 healId = 0;
        if (player->HasAura(SPELL_SHAMAN_CLEANSING_WATERS_DUMMY_R2))
            healId = SPELL_SHAMAN_CLEANSING_WATERS_HEAL_R2;
        else if (player->HasAura(SPELL_SHAMAN_CLEANSING_WATERS_DUMMY_R1))
            healId = SPELL_SHAMAN_CLEANSING_WATERS_HEAL_R1;
        else
            return;

        player->CastSpell(GetHitUnit(), healId, true);
        player->AddSpellCooldown(SPELL_SHAMAN_CLEANSING_WATERS_HEAL_R1, 0, 6 * IN_MILLISECONDS);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_cleanse_spirit::HandleDispel, EFFECT_0, SPELL_EFFECT_DISPEL);
    }
};

// 974 - Earth Shield: the heal with the shaman's healing bonus at the time of the proc, and the Glyph of Earth
// Shield (the 3.5 s between heals is spell_proc's)
class spell_sha_earth_shield : public AuraScript
{
    PrepareAuraScript(spell_sha_earth_shield);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_EARTH_SHIELD_HEAL, SPELL_SHAMAN_GLYPH_OF_EARTH_SHIELD });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        Unit* caster = GetCaster();
        if (!caster)
            return;

        int32 bp = caster->SpellHealingBonusDone(GetTarget(), GetSpellInfo(), aurEff->GetAmount(), HEAL, aurEff->GetEffIndex());
        if (AuraEffect const* glyphEff = caster->GetAuraEffect(SPELL_SHAMAN_GLYPH_OF_EARTH_SHIELD, EFFECT_0))
            AddPct(bp, glyphEff->GetAmount());

        GetTarget()->CastCustomSpell(SPELL_SHAMAN_EARTH_SHIELD_HEAL, SPELLVALUE_BASE_POINT0, bp, GetTarget(), true, nullptr, aurEff, GetCasterGUID());
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_earth_shield::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 8042 - Earth Shock: Fulmination releases the Lightning Shield charges above its amount
class spell_sha_earth_shock : public SpellScript
{
    PrepareSpellScript(spell_sha_earth_shock);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_LIGHTNING_SHIELD,
            SPELL_SHAMAN_FULMINATION,
            SPELL_SHAMAN_FULMINATION_DAMAGE,
            SPELL_SHAMAN_FULMINATION_PROC,
            SPELL_SHAMAN_LIGHTNING_SHIELD_DAMAGE
        });
    }

    void HandleFulmination(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        if (!target)
            return;

        AuraEffect const* fulmination = caster->GetAuraEffect(SPELL_SHAMAN_FULMINATION, EFFECT_0);
        Aura* lightningShield = caster->GetAura(SPELL_SHAMAN_LIGHTNING_SHIELD, caster->GetGUID());
        if (!fulmination || !lightningShield)
            return;

        int32 surplusCharges = int32(lightningShield->GetCharges()) - fulmination->GetAmount();
        if (surplusCharges <= 0)
            return;

        lightningShield->SetCharges(fulmination->GetAmount());

        SpellInfo const* lightningShieldDamage = sSpellMgr->AssertSpellInfo(SPELL_SHAMAN_LIGHTNING_SHIELD_DAMAGE);
        int32 bp = lightningShieldDamage->Effects[EFFECT_0].CalcValue(caster, nullptr, target);
        bp = caster->SpellDamageBonusDone(target, lightningShieldDamage, bp, SPELL_DIRECT_DAMAGE, EFFECT_0);
        bp *= surplusCharges;

        caster->CastCustomSpell(SPELL_SHAMAN_FULMINATION_DAMAGE, SPELLVALUE_BASE_POINT0, bp, target, true);
        caster->RemoveAurasDueToSpell(SPELL_SHAMAN_FULMINATION_PROC);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_sha_earth_shock::HandleFulmination, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 52007 - Earthliving Weapon (Passive): 20%, plus Blessing of the Eternals' chance on targets below 35% health
class spell_sha_earthliving_weapon : public AuraScript
{
    PrepareAuraScript(spell_sha_earthliving_weapon);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        int32 chance = 20;
        HealInfo* healInfo = eventInfo.GetHealInfo();
        if (healInfo && healInfo->GetTarget() && healInfo->GetTarget()->HealthBelowPct(35))
            if (AuraEffect const* aurEff = GetUnitOwner()->GetAuraEffectOfRankedSpell(SPELL_SHAMAN_BLESSING_OF_THE_ETERNALS_R1, EFFECT_1))
                chance += aurEff->GetBaseAmount();

        return roll_chance_i(chance);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_earthliving_weapon::CheckProc);
    }
};

// 61882 - Earthquake
class spell_sha_earthquake : public AuraScript
{
    PrepareAuraScript(spell_sha_earthquake);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_EARTHQUAKE_DAMAGE });
    }

    void HandleDummyTick(AuraEffect const* aurEff)
    {
        if (DynamicObject* dyn = GetTarget()->GetDynObject(aurEff->GetId()))
            GetTarget()->CastSpell(dyn->GetPositionX(), dyn->GetPositionY(), dyn->GetPositionZ(), SPELL_SHAMAN_EARTHQUAKE_DAMAGE, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_earthquake::HandleDummyTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 77478 - Earthquake: its chance to knock down
class spell_sha_earthquake_damage : public SpellScript
{
    PrepareSpellScript(spell_sha_earthquake_damage);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_EARTHQUAKE_STUN });
    }

    void HandleKnockdown(SpellEffIndex effIndex)
    {
        if (roll_chance_i(GetSpellInfo()->Effects[effIndex].CalcValue()))
            GetCaster()->CastSpell(GetHitUnit(), SPELL_SHAMAN_EARTHQUAKE_STUN, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_earthquake_damage::HandleKnockdown, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 77222 - Elemental Overload: the Elemental mastery's copies of Lightning Bolt, Chain Lightning (a third of the
// chance) and Lava Burst
class spell_sha_elemental_overload : public AuraScript
{
    PrepareAuraScript(spell_sha_elemental_overload);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_LIGHTNING_BOLT,
            SPELL_SHAMAN_LIGHTNING_BOLT_OVERLOAD,
            SPELL_SHAMAN_CHAIN_LIGHTNING,
            SPELL_SHAMAN_CHAIN_LIGHTNING_TRIGGERED,
            SPELL_SHAMAN_LAVA_BURST,
            SPELL_SHAMAN_LAVA_BURST_TRIGGERED
        });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        AuraEffect const* aurEff = GetEffect(EFFECT_0);
        if (!spellInfo || !aurEff)
            return false;

        if (spellInfo->Id == SPELL_SHAMAN_CHAIN_LIGHTNING)
            return roll_chance_i(CalculatePct(aurEff->GetAmount(), 33));

        return roll_chance_i(aurEff->GetAmount());
    }

    void HandleEffectProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* target = eventInfo.GetProcTarget();
        if (!target)
            return;

        uint32 spellId = 0;
        switch (eventInfo.GetSpellInfo()->Id)
        {
            case SPELL_SHAMAN_LIGHTNING_BOLT:
                spellId = SPELL_SHAMAN_LIGHTNING_BOLT_OVERLOAD;
                break;
            case SPELL_SHAMAN_CHAIN_LIGHTNING:
                spellId = SPELL_SHAMAN_CHAIN_LIGHTNING_TRIGGERED;
                break;
            case SPELL_SHAMAN_LAVA_BURST:
                spellId = SPELL_SHAMAN_LAVA_BURST_TRIGGERED;
                break;
            default:
                return;
        }

        GetTarget()->CastSpell(target, spellId, true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_elemental_overload::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_elemental_overload::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -86183 - Feedback: lightning spells shorten Elemental Mastery's cooldown
class spell_sha_feedback : public AuraScript
{
    PrepareAuraScript(spell_sha_feedback);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_ELEMENTAL_MASTERY });
    }

    void HandleEffectProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        if (Player* target = GetTarget()->ToPlayer())
            target->ModifySpellCooldown(SPELL_SHAMAN_ELEMENTAL_MASTERY, aurEff->GetBaseAmount());
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_feedback::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 1535 - Fire Nova: explodes the caster's Flame Shocks
class spell_sha_fire_nova : public SpellScript
{
    PrepareSpellScript(spell_sha_fire_nova);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_FLAME_SHOCK, SPELL_SHAMAN_FIRE_NOVA_DAMAGE });
    }

    // CPP checks after target selection; in AzerothCore the cast is checked before it is paid for
    SpellCastResult CheckFlameShock()
    {
        Unit* caster = GetCaster();
        float radius = GetSpellInfo()->Effects[EFFECT_0].CalcRadius(caster);
        std::list<Unit*> targets;
        Acore::AnyUnfriendlyUnitInObjectRangeCheck check(caster, caster, radius);
        Acore::UnitListSearcher<Acore::AnyUnfriendlyUnitInObjectRangeCheck> searcher(caster, targets, check);
        Cell::VisitObjects(caster, searcher, radius);

        for (Unit* target : targets)
            if (target->HasAura(SPELL_SHAMAN_FLAME_SHOCK, caster->GetGUID()))
                return SPELL_CAST_OK;

        SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_FLAME_SHOCK_NOT_ACTIVE);
        return SPELL_FAILED_CUSTOM_ERROR;
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        ObjectGuid casterGUID = GetCaster()->GetGUID();
        targets.remove_if([casterGUID](WorldObject const* target)
        {
            return !target->ToUnit() || !target->ToUnit()->HasAura(SPELL_SHAMAN_FLAME_SHOCK, casterGUID);
        });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_SHAMAN_FIRE_NOVA_DAMAGE, true);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_sha_fire_nova::CheckFlameShock);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_fire_nova::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_sha_fire_nova::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 8349 - Fire Nova: not on the Flame Shocked target it explodes from
class spell_sha_fire_nova_damage : public SpellScript
{
    PrepareSpellScript(spell_sha_fire_nova_damage);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        Unit const* center = GetExplTargetUnit();
        targets.remove_if([center](WorldObject const* target) { return target == center; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_fire_nova_damage::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_fire_nova_damage::FilterTargets, EFFECT_2, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

// 8050 - Flame Shock: Lava Flows' haste when it is dispelled
class spell_sha_flame_shock : public AuraScript
{
    PrepareAuraScript(spell_sha_flame_shock);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_LAVA_FLOWS_R1, SPELL_SHAMAN_LAVA_FLOWS_TRIGGERED_R1 });
    }

    void HandleDispel(DispelInfo* /*dispelInfo*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        if (AuraEffect const* aurEff = caster->GetAuraEffectOfRankedSpell(SPELL_SHAMAN_LAVA_FLOWS_R1, EFFECT_0))
            caster->CastCustomSpell(SPELL_SHAMAN_LAVA_FLOWS_TRIGGERED_R1, SPELLVALUE_BASE_POINT0, aurEff->GetAmount(), caster, true);
    }

    void Register() override
    {
        AfterDispel += AuraDispelFn(spell_sha_flame_shock::HandleDispel);
    }
};

// 10400 - Flametongue Weapon (Passive): the 4.3.4 damage, from Flametongue Weapon's effect 1
class spell_sha_flametongue_weapon : public AuraScript
{
    PrepareAuraScript(spell_sha_flametongue_weapon);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_FLAMETONGUE_ATTACK, SPELL_SHAMAN_FLAMETONGUE_WEAPON });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Player* player = eventInfo.GetActor()->ToPlayer();
        if (!player || !eventInfo.GetActionTarget())
            return false;

        Item* item = player->GetItemByGuid(GetAura()->GetCastItemGUID());
        if (!item || !item->IsEquipped())
            return false;

        WeaponAttackType attType = Player::GetAttackBySlot(item->GetSlot());
        if (attType != BASE_ATTACK && attType != OFF_ATTACK)
            return false;

        if (((attType == BASE_ATTACK) && !(eventInfo.GetTypeMask() & PROC_FLAG_DONE_MAINHAND_ATTACK)) ||
            ((attType == OFF_ATTACK) && !(eventInfo.GetTypeMask() & PROC_FLAG_DONE_OFFHAND_ATTACK)))
            return false;

        return true;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        Player* player = eventInfo.GetActor()->ToPlayer();
        Unit* target = eventInfo.GetActionTarget();
        WeaponAttackType attType = (eventInfo.GetTypeMask() & PROC_FLAG_DONE_OFFHAND_ATTACK) ? OFF_ATTACK : BASE_ATTACK;

        Item* item = player->GetWeaponForAttack(attType);
        if (!item)
            return;

        float basePoints = sSpellMgr->AssertSpellInfo(SPELL_SHAMAN_FLAMETONGUE_WEAPON)->Effects[EFFECT_1].CalcValue(player);

        // normalized to a 4.0 speed weapon, between BasePoints / 77 and BasePoints / 25 as the tooltip says
        float attackSpeed = player->GetAttackTime(attType) / 1000.f;
        float fireDamage = basePoints / 100.0f * attackSpeed;
        RoundToInterval(fireDamage, basePoints / 77.0f, basePoints / 25.0f);

        float spellPowerBonus = player->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_FIRE);
        spellPowerBonus += target->GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_DAMAGE_TAKEN, SPELL_SCHOOL_MASK_FIRE);
        spellPowerBonus *= 0.03811f * attackSpeed;

        player->CastCustomSpell(SPELL_SHAMAN_FLAMETONGUE_ATTACK, SPELLVALUE_BASE_POINT0, int32(fireDamage + spellPowerBonus), target, true, item, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_flametongue_weapon::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_flametongue_weapon::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -77794 - Focused Insight: shocks lower the next heal's cost and raise its healing
class spell_sha_focused_insight : public AuraScript
{
    PrepareAuraScript(spell_sha_focused_insight);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_FOCUSED_INSIGHT });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->GetCategory() == SHAMAN_SPELL_CATEGORY_SHOCK_SPELLS;
    }

    void HandleEffectProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        SpellInfo const* shock = eventInfo.GetSpellInfo();
        int32 bp0 = -CalculatePct(shock->CalcPowerCost(GetTarget(), shock->GetSchoolMask()), aurEff->GetAmount());
        int32 bp1 = GetSpellInfo()->Effects[EFFECT_1].CalcValue();

        GetTarget()->CastCustomSpell(GetTarget(), SPELL_SHAMAN_FOCUSED_INSIGHT, &bp0, &bp1, &bp1, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_focused_insight::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_focused_insight::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -63373 - Frozen Power: Frost Shock roots targets at least 15 yards away
class spell_sha_frozen_power : public AuraScript
{
    PrepareAuraScript(spell_sha_frozen_power);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_FREEZE });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        if (!roll_chance_i(aurEff->GetAmount()))
            return;

        Unit* caster = eventInfo.GetActor();
        Unit* target = eventInfo.GetProcTarget();
        if (!target)
            return;

        float minDistance = float(sSpellMgr->AssertSpellInfo(SPELL_SHAMAN_FREEZE)->Effects[EFFECT_0].CalcValue(caster));
        if (caster->GetDistance(target) < minDistance)
            return;

        caster->CastSpell(target, SPELL_SHAMAN_FREEZE, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_frozen_power::HandleProc, EFFECT_1, SPELL_AURA_DUMMY);
    }
};

// 55440 - Glyph of Healing Wave
class spell_sha_glyph_of_healing_wave : public AuraScript
{
    PrepareAuraScript(spell_sha_glyph_of_healing_wave);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_GLYPH_OF_HEALING_WAVE });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        // not from heals on the shaman
        return GetTarget() != eventInfo.GetProcTarget();
    }

    void HandleEffectProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        HealInfo* healInfo = eventInfo.GetHealInfo();
        if (!healInfo)
            return;

        int32 heal = CalculatePct(int32(healInfo->GetEffectiveHeal()), aurEff->GetAmount());
        if (heal)
            GetTarget()->CastCustomSpell(SPELL_SHAMAN_GLYPH_OF_HEALING_WAVE, SPELLVALUE_BASE_POINT0, heal, GetTarget(), true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_glyph_of_healing_wave::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_glyph_of_healing_wave::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 73920 - Healing Rain
class spell_sha_healing_rain : public AuraScript
{
    PrepareAuraScript(spell_sha_healing_rain);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_HEALING_RAIN_TRIGGERED });
    }

    void HandleEffectPeriodic(AuraEffect const* aurEff)
    {
        if (DynamicObject* dyn = GetTarget()->GetDynObject(aurEff->GetId()))
            GetTarget()->CastSpell(dyn->GetPositionX(), dyn->GetPositionY(), dyn->GetPositionZ(), SPELL_SHAMAN_HEALING_RAIN_TRIGGERED, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_healing_rain::HandleEffectPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 73921 - Healing Rain: the heal is shared out above six targets
class spell_sha_healing_rain_triggered : public SpellScript
{
    PrepareSpellScript(spell_sha_healing_rain_triggered);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        _targets = targets.size();
    }

    void HandleHeal(SpellEffIndex /*effIndex*/)
    {
        if (GetHitHeal() && _targets > 6)
            SetHitHeal(int32(GetHitHeal() * 6 / _targets));
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_healing_rain_triggered::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
        OnEffectHitTarget += SpellEffectFn(spell_sha_healing_rain_triggered::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
    }

private:
    std::size_t _targets = 0;
};

// 52041 - Healing Stream Totem: the totem's heal from its effect 0, with Soothing Rains. The heal itself (52042)
// carries the 4.3.4 healing coefficient (the totem's healing bonus is the shaman's).
class spell_sha_healing_stream_totem : public SpellScript
{
    PrepareSpellScript(spell_sha_healing_stream_totem);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_TOTEM_HEALING_STREAM_HEAL, SPELL_SHAMAN_SOOTHING_RAINS_R1 });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        if (!target)
            return;

        int32 bp = GetEffectValue();
        if (Unit* owner = caster->GetOwner())
            if (AuraEffect const* soothingRains = owner->GetAuraEffectOfRankedSpell(SPELL_SHAMAN_SOOTHING_RAINS_R1, EFFECT_0))
                AddPct(bp, soothingRains->GetAmount());

        caster->CastCustomSpell(target, SPELL_SHAMAN_TOTEM_HEALING_STREAM_HEAL, &bp, nullptr, nullptr, true, nullptr, nullptr, caster->GetOwnerGUID());
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_healing_stream_totem::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 32182 - Heroism: not on targets that had one of the 4.3.4 haste buffs (Sated, Exhaustion, Insanity, Temporal
// Displacement)
class spell_sha_heroism : public SpellScript
{
    PrepareSpellScript(spell_sha_heroism);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_SATED, SPELL_SHAMAN_EXHAUSTION, SPELL_HUNTER_INSANITY, SPELL_MAGE_TEMPORAL_DISPLACEMENT });
    }

    void RemoveInvalidTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(Acore::UnitAuraCheck(true, SPELL_SHAMAN_SATED));
        targets.remove_if(Acore::UnitAuraCheck(true, SPELL_SHAMAN_EXHAUSTION));
        targets.remove_if(Acore::UnitAuraCheck(true, SPELL_HUNTER_INSANITY));
        targets.remove_if(Acore::UnitAuraCheck(true, SPELL_MAGE_TEMPORAL_DISPLACEMENT));
    }

    void ApplyDebuff()
    {
        if (Unit* target = GetHitUnit())
            target->CastSpell(target, SPELL_SHAMAN_EXHAUSTION, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_heroism::RemoveInvalidTargets, EFFECT_ALL, TARGET_UNIT_CASTER_AREA_RAID);
        AfterHit += SpellHitFn(spell_sha_heroism::ApplyDebuff);
    }
};

// 60103 - Lava Lash: Flametongue on the off-hand and Improved Lava Lash (Searing Flames' stacks, Flame Shock spread)
class spell_sha_lava_lash : public SpellScript
{
    PrepareSpellScript(spell_sha_lava_lash);

    bool Load() override
    {
        return GetCaster()->IsPlayer();
    }

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_SEARING_FLAMES_DAMAGE,
            SPELL_SHAMAN_LAVA_LASH_SPREAD_FLAME_SHOCK,
            SPELL_SHAMAN_FLAME_SHOCK,
            SPELL_SHAMAN_IMPROVED_LAVA_LASH_R1
        });
    }

    // AzerothCore takes the weapon percentage from the spell data at the hit (CPP raised effect 0's value by the
    // bonus percentages); the bonuses multiply the damage instead, as one sum
    void HandleDamageBonus()
    {
        Player* caster = GetCaster()->ToPlayer();
        Unit* target = GetHitUnit();
        if (!target)
            return;

        int32 bonusPct = 0;

        // 40% more with Flametongue on the off-hand weapon
        if (Item* offhand = caster->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
            if (offhand->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT) == SHAMAN_ENCHANTMENT_ID_FLAMETONGUE)
                bonusPct += GetSpellInfo()->Effects[EFFECT_1].CalcValue(caster);

        if (AuraEffect const* improvedLavaLash = caster->GetAuraEffectOfRankedSpell(SPELL_SHAMAN_IMPROVED_LAVA_LASH_R1, EFFECT_1))
        {
            if (Aura* searingFlames = target->GetAura(SPELL_SHAMAN_SEARING_FLAMES_DAMAGE, caster->GetGUID()))
            {
                bonusPct += improvedLavaLash->GetAmount() * searingFlames->GetStackAmount();
                searingFlames->Remove();
            }

            if (target->HasAura(SPELL_SHAMAN_FLAME_SHOCK, caster->GetGUID()))
                caster->CastCustomSpell(SPELL_SHAMAN_LAVA_LASH_SPREAD_FLAME_SHOCK, SPELLVALUE_MAX_TARGETS, 4, target, true, nullptr, improvedLavaLash);
        }

        if (bonusPct)
        {
            int32 damage = GetHitDamage();
            AddPct(damage, bonusPct);
            SetHitDamage(damage);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_sha_lava_lash::HandleDamageBonus);
    }
};

// 105792 - Lava Lash: spreads the main target's Flame Shock to up to four enemies without it (none in crowd
// control)
class spell_sha_lava_lash_script : public SpellScript
{
    PrepareSpellScript(spell_sha_lava_lash_script);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_FLAME_SHOCK });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        Unit const* center = GetExplTargetUnit();
        ObjectGuid casterGUID = GetCaster()->GetGUID();
        targets.remove_if([center, casterGUID](WorldObject const* obj)
        {
            Unit const* target = obj->ToUnit();
            if (!target || obj == center)
                return true;

            if (target->HasAura(SPELL_SHAMAN_FLAME_SHOCK, casterGUID))
                return true;

            return target->HasBreakableByDamageCrowdControlAura();
        });

        if (targets.size() > 4)
            targets.resize(4);
    }

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* center = GetExplTargetUnit();
        if (!center)
            return;

        if (Aura* flameShock = center->GetAura(SPELL_SHAMAN_FLAME_SHOCK, caster->GetGUID()))
            if (Aura* spread = caster->AddAura(SPELL_SHAMAN_FLAME_SHOCK, GetHitUnit()))
                spread->SetDuration(flameShock->GetDuration());
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_lava_lash_script::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_sha_lava_lash_script::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// -77755 - Lava Surge
class spell_sha_lava_surge : public AuraScript
{
    PrepareAuraScript(spell_sha_lava_surge);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_LAVA_SURGE });
    }

    void HandleEffectProc(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(GetTarget(), SPELL_SHAMAN_LAVA_SURGE, true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_lava_surge::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 77762 - Lava Surge: resets Lava Burst; Volcano with the T12 elemental 4P bonus
class spell_sha_lava_surge_proc : public SpellScript
{
    PrepareSpellScript(spell_sha_lava_surge_proc);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_LAVA_BURST, SPELL_SHAMAN_VOLCANO, SPELL_SHAMAN_ITEM_SHAMAN_T12_ELEMENTAL_4P });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* target = GetHitUnit();
        if (Player* player = target->ToPlayer())
            player->RemoveSpellCooldown(SPELL_SHAMAN_LAVA_BURST, true);

        if (target->HasAura(SPELL_SHAMAN_ITEM_SHAMAN_T12_ELEMENTAL_4P))
            target->CastSpell(target, SPELL_SHAMAN_VOLCANO, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_lava_surge_proc::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 324 - Lightning Shield: the Glyph of Lightning Shield keeps three charges
class spell_sha_lightning_shield : public AuraScript
{
    PrepareAuraScript(spell_sha_lightning_shield);

    void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
    {
        if (GetTarget()->HasAura(SPELL_SHAMAN_GLYPH_OF_LIGHTNING_SHIELD) && GetCharges() < 3)
            SetCharges(3);
    }

    void Register() override
    {
        AfterEffectProc += AuraEffectProcFn(spell_sha_lightning_shield::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 53817 - Maelstrom Weapon: the full-stack glow (CPP) and the T10 enhancement 4P bonus (AzerothCore's)
class spell_sha_maelstrom_weapon : public AuraScript
{
    PrepareAuraScript(spell_sha_maelstrom_weapon);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_MAELSTROM_DUMMY, SPELL_SHAMAN_MAELSTROM_POWER, SPELL_SHAMAN_T10_ENHANCEMENT_4P_BONUS });
    }

    void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetStackAmount() < int32(GetSpellInfo()->StackAmount))
            return;

        Unit* target = GetTarget();
        target->CastSpell(target, SPELL_SHAMAN_MAELSTROM_DUMMY, true);

        if (AuraEffect const* aurEff = target->GetAuraEffect(SPELL_SHAMAN_T10_ENHANCEMENT_4P_BONUS, EFFECT_0))
            if (roll_chance_i(aurEff->GetAmount()))
                target->CastSpell(target, SPELL_SHAMAN_MAELSTROM_POWER, true, nullptr, aurEff);
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_SHAMAN_MAELSTROM_DUMMY, GetTarget()->GetGUID());
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_sha_maelstrom_weapon::AfterApply, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        AfterEffectRemove += AuraEffectRemoveFn(spell_sha_maelstrom_weapon::AfterRemove, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
    }
};

// 16191 - Mana Tide: the totem gives its share of the shaman's Spirit (4.3.4; 3.3.5a gave mana)
class spell_sha_mana_tide_totem : public AuraScript
{
    PrepareAuraScript(spell_sha_mana_tide_totem);

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        Unit* totem = GetCaster();
        Unit* shaman = totem ? totem->GetOwner() : nullptr;
        amount = shaman ? int32(CalculatePct(shaman->GetStat(STAT_SPIRIT), amount)) : 0;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_mana_tide_totem::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_STAT);
    }
};

// -30881 - Nature's Guardian: below 30% health, heals and lowers the attacker's threat
class spell_sha_nature_guardian : public AuraScript
{
    PrepareAuraScript(spell_sha_nature_guardian);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_NATURE_GUARDIAN });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        return damageInfo && GetTarget()->HealthBelowPctDamaged(GetSpellInfo()->Effects[EFFECT_1].CalcValue(), damageInfo->GetDamage());
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* target = GetTarget();
        int32 bp = int32(target->CountPctFromMaxHealth(aurEff->GetAmount()));
        target->CastCustomSpell(SPELL_SHAMAN_NATURE_GUARDIAN, SPELLVALUE_BASE_POINT0, bp, target, true, nullptr, aurEff);

        if (Unit* attacker = eventInfo.GetActor())
            if (attacker != target && attacker->IsAlive())
                attacker->GetThreatMgr().ModifyThreatByPercent(target, -10);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_nature_guardian::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_nature_guardian::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 61783 - Feral Spirit Pet Scaling 04: the shaman's melee and spell hit
class spell_sha_pet_scaling_04 : public AuraScript
{
    PrepareAuraScript(spell_sha_pet_scaling_04);

    bool Load() override
    {
        return GetCaster() && GetCaster()->GetOwner() && GetCaster()->GetOwner()->IsPlayer();
    }

    void CalculateAmountMeleeHit(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = true;
        if (Player* owner = GetCaster()->GetOwner()->ToPlayer())
            amount += int32(owner->GetTotalAuraModifier(SPELL_AURA_MOD_HIT_CHANCE) + owner->GetRatingBonusValue(CR_HIT_MELEE));
    }

    void CalculateAmountSpellHit(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = true;
        if (Player* owner = GetCaster()->GetOwner()->ToPlayer())
            amount += int32(owner->GetTotalAuraModifier(SPELL_AURA_MOD_SPELL_HIT_CHANCE) + owner->GetRatingBonusValue(CR_HIT_SPELL));
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_pet_scaling_04::CalculateAmountMeleeHit, EFFECT_0, SPELL_AURA_MOD_HIT_CHANCE);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_pet_scaling_04::CalculateAmountSpellHit, EFFECT_1, SPELL_AURA_MOD_SPELL_HIT_CHANCE);
    }
};

// -16180 - Resurgence: mana from critical heals while Water Shield is up, by heal
class spell_sha_resurgence : public AuraScript
{
    PrepareAuraScript(spell_sha_resurgence);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_RESURGENCE_ENERGIZE, SPELL_SHAMAN_WATER_SHIELD });
    }

    bool CheckProc(ProcEventInfo& /*eventInfo*/)
    {
        return GetTarget()->HasAura(SPELL_SHAMAN_WATER_SHIELD, GetTarget()->GetGUID());
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!aurEff->GetAmount() || !spellInfo)
            return;

        int32 baseAmount = sSpellMgr->AssertSpellInfo(SPELL_SHAMAN_RESURGENCE_ENERGIZE)->Effects[EFFECT_0].CalcValue(GetTarget());

        float modifier = 1.f;
        switch (spellInfo->Id)
        {
            case SPELL_SHAMAN_HEALING_SURGE:
            case SPELL_SHAMAN_RIPTIDE:
            case SPELL_SHAMAN_UNLEASH_LIFE:
                modifier = 0.6f;
                break;
            case SPELL_SHAMAN_CHAIN_HEAL:
                modifier = 0.333f;
                break;
            default:
                break;
        }

        int32 bp = int32(baseAmount * modifier * (aurEff->GetAmount() / 100.f));
        GetTarget()->EnergizeBySpell(GetTarget(), SPELL_SHAMAN_RESURGENCE_ENERGIZE, bp, POWER_MANA);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_resurgence::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_resurgence::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -88756 - Rolling Thunder: Lightning Shield charges from lightning spells (the mana is the aura's own trigger)
class spell_sha_rolling_thunder : public AuraScript
{
    PrepareAuraScript(spell_sha_rolling_thunder);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_LIGHTNING_SHIELD, SPELL_SHAMAN_FULMINATION, SPELL_SHAMAN_FULMINATION_PROC });
    }

    bool CheckProc(ProcEventInfo& /*eventInfo*/)
    {
        return GetTarget()->HasAura(SPELL_SHAMAN_LIGHTNING_SHIELD);
    }

    void HandleEffectProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        Unit* target = GetTarget();
        Aura* lightningShield = target->GetAura(SPELL_SHAMAN_LIGHTNING_SHIELD);
        if (!lightningShield)
            return;

        uint8 charges = uint8(std::min<int32>(lightningShield->GetCharges() + 1, aurEff->GetAmount()));
        lightningShield->SetCharges(charges);
        lightningShield->RefreshDuration();

        // Fulmination's glow at full charges
        if (charges == aurEff->GetAmount() && target->HasAura(SPELL_SHAMAN_FULMINATION))
            target->CastSpell(target, SPELL_SHAMAN_FULMINATION_PROC, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_rolling_thunder::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_rolling_thunder::HandleEffectProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 3606 - Searing Bolt: the Searing Totem's bolts apply the shaman's Searing Flames
class spell_sha_searing_bolt : public SpellScript
{
    PrepareSpellScript(spell_sha_searing_bolt);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_SEARING_FLAMES_DAMAGE, SPELL_SHAMAN_SEARING_FLAMES_R1 });
    }

    void HandleSearingFlames(SpellEffIndex /*effIndex*/)
    {
        if (Unit* owner = GetCaster()->GetOwner())
            if (AuraEffect const* searingFlames = owner->GetAuraEffectOfRankedSpell(SPELL_SHAMAN_SEARING_FLAMES_R1, EFFECT_0))
                if (roll_chance_i(searingFlames->GetAmount()))
                    owner->CastSpell(GetHitUnit(), SPELL_SHAMAN_SEARING_FLAMES_DAMAGE, true, nullptr, searingFlames);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_searing_bolt::HandleSearingFlames, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 98020 - Spirit Link (the totem's pulse every second): the group members under the totem's aura end up at the same
// health percentage. Ours: not in CPP. Set directly, so mitigation can't leave them unequal.
class spell_sha_spirit_link : public SpellScript
{
    PrepareSpellScript(spell_sha_spirit_link);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_SPIRIT_LINK_TOTEM_AURA });
    }

    void HandlePulse(SpellEffIndex /*effIndex*/)
    {
        Unit* totem = GetCaster();
        Unit* owner = totem->GetOwner();
        Player* shaman = owner ? owner->ToPlayer() : nullptr;
        Group* group = shaman ? shaman->GetGroup() : nullptr;
        if (!group)
            return;

        std::list<Player*> linked;
        float totalPct = 0.0f;
        for (GroupReference* itr = group->GetFirstMember(); itr; itr = itr->next())
        {
            Player* member = itr->GetSource();
            if (member && member->IsAlive() && member->IsInMap(totem) && member->HasAura(SPELL_SHAMAN_SPIRIT_LINK_TOTEM_AURA, totem->GetGUID()))
            {
                linked.push_back(member);
                totalPct += member->GetHealthPct();
            }
        }

        if (linked.size() < 2)
            return;

        float pct = totalPct / linked.size();
        for (Player* member : linked)
            member->SetHealth(std::max<uint32>(1, member->CountPctFromMaxHealth(pct)));
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_sha_spirit_link::HandlePulse, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// -51525 - Static Shock: Lightning Shield's damage without using a charge (4.3.4)
class spell_sha_static_shock : public AuraScript
{
    PrepareAuraScript(spell_sha_static_shock);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_LIGHTNING_SHIELD, SPELL_SHAMAN_LIGHTNING_SHIELD_DAMAGE });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetActor()->HasAura(SPELL_SHAMAN_LIGHTNING_SHIELD) && eventInfo.GetProcTarget();
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        eventInfo.GetActor()->CastSpell(eventInfo.GetProcTarget(), SPELL_SHAMAN_LIGHTNING_SHIELD_DAMAGE, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_static_shock::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_static_shock::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -82984 - Telluric Currents: mana from a share of Lightning Bolt's damage
class spell_sha_telluric_currents : public AuraScript
{
    PrepareAuraScript(spell_sha_telluric_currents);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_TELLURIC_CURRENTS });
    }

    void HandleEffectProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetDamage())
            return;

        int32 bp = CalculatePct(int32(damageInfo->GetDamage()), aurEff->GetAmount());
        GetTarget()->CastCustomSpell(SPELL_SHAMAN_TELLURIC_CURRENTS, SPELLVALUE_BASE_POINT0, bp, GetTarget(), true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_telluric_currents::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 51490 - Thunderstorm: the 4.3.4 slow; the Glyph of Thunderstorm takes away the knockback only
class spell_sha_thunderstorm : public SpellScript
{
    PrepareSpellScript(spell_sha_thunderstorm);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_THUNDERSTORM_SLOW });
    }

    void HandleKnockBack(SpellEffIndex effIndex)
    {
        Unit* caster = GetCaster();
        if (caster->HasAura(SPELL_SHAMAN_GLYPH_OF_THUNDERSTORM))
            PreventHitDefaultEffect(effIndex);

        caster->CastSpell(GetHitUnit(), SPELL_SHAMAN_THUNDERSTORM_SLOW, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_thunderstorm::HandleKnockBack, EFFECT_2, SPELL_EFFECT_KNOCK_BACK);
    }
};

// -51562 - Tidal Waves
class spell_sha_tidal_waves : public AuraScript
{
    PrepareAuraScript(spell_sha_tidal_waves);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_TIDAL_WAVES });
    }

    void HandleEffectProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        int32 bp0 = -aurEff->GetAmount();
        int32 bp1 = aurEff->GetAmount();
        GetTarget()->CastCustomSpell(GetTarget(), SPELL_SHAMAN_TIDAL_WAVES, &bp0, &bp1, nullptr, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_tidal_waves::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 73680 - Unleash Elements: one spell per imbued weapon (two only when they differ)
class spell_sha_unleash_elements : public SpellScript
{
    PrepareSpellScript(spell_sha_unleash_elements);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_UNLEASH_LIFE,
            SPELL_SHAMAN_UNLEASH_WIND,
            SPELL_SHAMAN_UNLEASH_FROST,
            SPELL_SHAMAN_UNLEASH_FLAME,
            SPELL_SHAMAN_UNLEASH_EARTH
        });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Player* player = GetCaster()->ToPlayer();
        Unit* explTarget = GetExplTargetUnit();
        if (!player || !explTarget)
            return;

        uint32 lastSpellId = 0;
        for (uint8 slot : { EQUIPMENT_SLOT_MAINHAND, EQUIPMENT_SLOT_OFFHAND })
        {
            Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
            if (!item)
                continue;

            uint32 spellId = 0;
            switch (item->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT))
            {
                case SHAMAN_ENCHANTMENT_ID_FLAMETONGUE: spellId = SPELL_SHAMAN_UNLEASH_FLAME; break;
                case SHAMAN_ENCHANTMENT_ID_FROSTBRAND:  spellId = SPELL_SHAMAN_UNLEASH_FROST; break;
                case SHAMAN_ENCHANTMENT_ID_WINDFURY:    spellId = SPELL_SHAMAN_UNLEASH_WIND; break;
                case SHAMAN_ENCHANTMENT_ID_ROCKBITER:   spellId = SPELL_SHAMAN_UNLEASH_EARTH; break;
                case SHAMAN_ENCHANTMENT_ID_EARTHLIVING: spellId = SPELL_SHAMAN_UNLEASH_LIFE; break;
                default: break;
            }

            if (spellId && spellId != lastSpellId)
            {
                Unit* target = explTarget;
                if (spellId == SPELL_SHAMAN_UNLEASH_LIFE && !target->IsFriendlyTo(player))
                    target = player;

                player->CastSpell(target, spellId, true);
                lastSpellId = spellId;
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_unleash_elements::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 57994 - Wind Shear: Seasoned Winds gives resistance to the school of the spell it interrupted, by the tooltip's level
// formula (half at rank 1). Ours: not in CPP. The cast is read at launch, before Spell::EffectInterruptCast (which
// runs at launch) ends it, and it is gone after the hit.
class spell_sha_wind_shear : public SpellScript
{
    PrepareSpellScript(spell_sha_wind_shear);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_SEASONED_WINDS_R1, SPELL_SHAMAN_SEASONED_WINDS_FIRE, SPELL_SHAMAN_SEASONED_WINDS_FROST,
            SPELL_SHAMAN_SEASONED_WINDS_NATURE, SPELL_SHAMAN_SEASONED_WINDS_ARCANE, SPELL_SHAMAN_SEASONED_WINDS_SHADOW });
    }

    void RememberCast(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
        {
            _generic = target->GetCurrentSpell(CURRENT_GENERIC_SPELL);
            _channeled = target->GetCurrentSpell(CURRENT_CHANNELED_SPELL);
            _genericSchool = _generic ? _generic->m_spellInfo->GetSchoolMask() : SPELL_SCHOOL_MASK_NONE;
            _channeledSchool = _channeled ? _channeled->m_spellInfo->GetSchoolMask() : SPELL_SCHOOL_MASK_NONE;
        }
    }

    void HandleAfterHit()
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        Aura const* talent = caster->GetAuraOfRankedSpell(SPELL_SHAMAN_SEASONED_WINDS_R1);
        if (!talent || !target)
            return;

        SpellSchoolMask school = SPELL_SCHOOL_MASK_NONE;
        if (_generic && target->GetCurrentSpell(CURRENT_GENERIC_SPELL) != _generic)
            school = _genericSchool;
        else if (_channeled && target->GetCurrentSpell(CURRENT_CHANNELED_SPELL) != _channeled)
            school = _channeledSchool;

        uint32 spellId = 0;
        if (school & SPELL_SCHOOL_MASK_FIRE)
            spellId = SPELL_SHAMAN_SEASONED_WINDS_FIRE;
        else if (school & SPELL_SCHOOL_MASK_FROST)
            spellId = SPELL_SHAMAN_SEASONED_WINDS_FROST;
        else if (school & SPELL_SCHOOL_MASK_NATURE)
            spellId = SPELL_SHAMAN_SEASONED_WINDS_NATURE;
        else if (school & SPELL_SCHOOL_MASK_ARCANE)
            spellId = SPELL_SHAMAN_SEASONED_WINDS_ARCANE;
        else if (school & SPELL_SCHOOL_MASK_SHADOW)
            spellId = SPELL_SHAMAN_SEASONED_WINDS_SHADOW;
        if (!spellId)
            return;

        int32 level = caster->GetLevel();
        int32 resistance = level;
        if (level > 70)
            resistance += (std::min(level, 80) - 70) * 5;
        if (level > 80)
            resistance += (level - 80) * 7;
        if (talent->GetSpellInfo()->GetRank() == 1)
            resistance /= 2;

        caster->CastCustomSpell(spellId, SPELLVALUE_BASE_POINT0, resistance, caster, true);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_sha_wind_shear::RememberCast, EFFECT_0, SPELL_EFFECT_INTERRUPT_CAST);
        AfterHit += SpellHitFn(spell_sha_wind_shear::HandleAfterHit);
    }

private:
    Spell const* _generic = nullptr;
    Spell const* _channeled = nullptr;
    SpellSchoolMask _genericSchool = SPELL_SCHOOL_MASK_NONE;
    SpellSchoolMask _channeledSchool = SPELL_SCHOOL_MASK_NONE;
};

// 33757 - Windfury Weapon (Passive): three extra attacks in 4.3.4 (3.3.5a two), each with effect 1's attack power
class spell_sha_windfury_weapon : public AuraScript
{
    PrepareAuraScript(spell_sha_windfury_weapon);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_WINDFURY_WEAPON, SPELL_SHAMAN_WINDFURY_ATTACK_MAINHAND, SPELL_SHAMAN_WINDFURY_ATTACK_OFFHAND });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Player* player = eventInfo.GetActor()->ToPlayer();
        if (!player || !eventInfo.GetActionTarget())
            return false;

        Item* item = player->GetItemByGuid(GetAura()->GetCastItemGUID());
        if (!item || !item->IsEquipped())
            return false;

        WeaponAttackType attType = Player::GetAttackBySlot(item->GetSlot());
        if (attType != BASE_ATTACK && attType != OFF_ATTACK)
            return false;

        if (((attType == BASE_ATTACK) && !(eventInfo.GetTypeMask() & PROC_FLAG_DONE_MAINHAND_ATTACK)) ||
            ((attType == OFF_ATTACK) && !(eventInfo.GetTypeMask() & PROC_FLAG_DONE_OFFHAND_ATTACK)))
            return false;

        return true;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        Player* player = eventInfo.GetActor()->ToPlayer();
        Unit* target = eventInfo.GetActionTarget();
        WeaponAttackType attType = (eventInfo.GetTypeMask() & PROC_FLAG_DONE_OFFHAND_ATTACK) ? OFF_ATTACK : BASE_ATTACK;

        Item* item = player->GetWeaponForAttack(attType);
        if (!item)
            return;

        int32 attackPower = sSpellMgr->AssertSpellInfo(SPELL_SHAMAN_WINDFURY_WEAPON)->Effects[EFFECT_1].CalcValue(player);
        int32 amount = int32(attackPower / 14.f * player->GetAttackTime(attType) / 1000.f);
        uint32 spellId = attType == BASE_ATTACK ? SPELL_SHAMAN_WINDFURY_ATTACK_MAINHAND : SPELL_SHAMAN_WINDFURY_ATTACK_OFFHAND;

        for (uint8 i = 0; i < 3; ++i)
            player->CastCustomSpell(spellId, SPELLVALUE_BASE_POINT0, amount, target, true, item, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_windfury_weapon::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_windfury_weapon::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// AzerothCore's, still right in 4.3.4

// 52759 - Ancestral Awakening (Proc)
class spell_sha_ancestral_awakening_proc : public SpellScript
{
    PrepareSpellScript(spell_sha_ancestral_awakening_proc);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_ANCESTRAL_AWAKENING_PROC });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (targets.size() < 2)
            return;

        targets.sort(Acore::HealthPctOrderPred());

        WorldObject* target = targets.front();
        targets.clear();
        targets.push_back(target);
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        int32 damage = GetEffectValue();
        if (GetHitUnit())
            GetCaster()->CastCustomSpell(GetHitUnit(), SPELL_SHAMAN_ANCESTRAL_AWAKENING_PROC, &damage, nullptr, nullptr, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_ancestral_awakening_proc::FilterTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_RAID);
        OnEffectHitTarget += SpellEffectFn(spell_sha_ancestral_awakening_proc::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 16246 - Clearcasting
class spell_sha_clearcasting : public AuraScript
{
    PrepareAuraScript(spell_sha_clearcasting);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_ELEMENTAL_OATH });
    }

    // Elemental Oath bonus
    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        Unit const* owner = GetUnitOwner();
        if (Aura const* aura = owner->GetAuraOfRankedSpell(SPELL_SHAMAN_ELEMENTAL_OATH, owner->GetGUID()))
            amount = aura->GetSpellInfo()->Effects[EFFECT_1].CalcValue();
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_clearcasting::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
    }
};

// 6474 - Earthbind Totem - Fix Talent: Earthen Power
class spell_sha_earthbind_totem : public AuraScript
{
    PrepareAuraScript(spell_sha_earthbind_totem);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_TOTEM_EARTHBIND_TOTEM, SPELL_SHAMAN_TOTEM_EARTHEN_POWER });
    }

    void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
    {
        if (!GetCaster())
            return;
        if (Player* owner = GetCaster()->GetCharmerOrOwnerPlayerOrPlayerItself())
            if (AuraEffect* aur = owner->GetDummyAuraEffect(SPELLFAMILY_SHAMAN, 2289, 0))
                if (roll_chance_i(aur->GetBaseAmount()))
                    GetTarget()->CastSpell((Unit*)nullptr, SPELL_SHAMAN_TOTEM_EARTHEN_POWER, true);
    }

    void Apply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (!GetCaster())
            return;
        Player* owner = GetCaster()->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!owner)
            return;
        // Storm, Earth and Fire
        if (AuraEffect* aurEff = owner->GetAuraEffectOfRankedSpell(SPELL_SHAMAN_STORM_EARTH_AND_FIRE, EFFECT_1))
        {
            if (roll_chance_i(aurEff->GetAmount()))
                GetCaster()->CastSpell(GetCaster(), SPELL_SHAMAN_TOTEM_EARTHBIND_EARTHGRAB, false);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_earthbind_totem::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        OnEffectApply += AuraEffectApplyFn(spell_sha_earthbind_totem::Apply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

class EarthenPowerTargetSelector
{
public:
    EarthenPowerTargetSelector() { }

    bool operator() (WorldObject* target)
    {
        if (!target->ToUnit())
            return true;

        if (!target->ToUnit()->HasAuraWithMechanic(1 << MECHANIC_SNARE))
            return true;

        return false;
    }
};

// 59566 - Earthen Power
class spell_sha_earthen_power : public SpellScript
{
    PrepareSpellScript(spell_sha_earthen_power);

    void FilterTargets(std::list<WorldObject*>& unitList)
    {
        unitList.remove_if(EarthenPowerTargetSelector());
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_earthen_power::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// 16164 - Elemental Focus
// Prevents weapon imbue attacks (Frostbrand, Flametongue) from proccing Clearcasting
class spell_sha_elemental_focus : public AuraScript
{
    PrepareAuraScript(spell_sha_elemental_focus);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (SpellInfo const* procSpell = eventInfo.GetSpellInfo())
        {
            // Frostbrand Attack (mask0 = 0x1000000), Flametongue Attack (mask0 = 0x200000)
            if (procSpell->SpellFamilyName == SPELLFAMILY_SHAMAN &&
                (procSpell->SpellFamilyFlags[0] & 0x1200000))
                return false;
        }
        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_elemental_focus::CheckProc);
    }
};

// -16257 - Flurry (the haste charges): not used up by Windfury Attack, Stormstrike and Lava Lash
class spell_sha_flurry_proc : public AuraScript
{
    PrepareAuraScript(spell_sha_flurry_proc);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        // Should not proc from Windfury Attack, Stormstrike and Lava Lash
        if (SpellInfo const* spellInfo = eventInfo.GetSpellInfo())
        {
            constexpr std::array<uint32, 2> spellIcons = {SPELL_SHAMAN_STORMSTRIKE, SPELL_SHAMAN_LAVA_LASH};
            const auto found = std::find(std::begin(spellIcons), std::end(spellIcons), spellInfo->Id);

            if ((spellInfo->SpellFamilyName == SPELLFAMILY_SHAMAN && (spellInfo->SpellFamilyFlags[0] & 0x00800000) != 0) || found != std::end(spellIcons))
            {
                return false;
            }
        }

        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_flurry_proc::CheckProc);
    }
};

// 23551 - Lightning Shield
class spell_sha_item_lightning_shield : public AuraScript
{
    PrepareAuraScript(spell_sha_item_lightning_shield);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_ITEM_LIGHTNING_SHIELD });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(eventInfo.GetProcTarget(), SPELL_SHAMAN_ITEM_LIGHTNING_SHIELD, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_item_lightning_shield::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 23552 - Lightning Shield
class spell_sha_item_lightning_shield_trigger : public AuraScript
{
    PrepareAuraScript(spell_sha_item_lightning_shield_trigger);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_ITEM_MANA_SURGE });
    }
    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(eventInfo.GetProcTarget(), SPELL_SHAMAN_ITEM_LIGHTNING_SHIELD_DAMAGE, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_item_lightning_shield_trigger::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 23572 - Mana Surge
class spell_sha_item_mana_surge : public AuraScript
{
    PrepareAuraScript(spell_sha_item_mana_surge);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_ITEM_LIGHTNING_SHIELD_DAMAGE });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetSpellInfo() != nullptr;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        int32 mana = eventInfo.GetSpellInfo()->CalcPowerCost(GetTarget(), eventInfo.GetSchoolMask());
        int32 damage = CalculatePct(mana, 35);

        GetTarget()->CastCustomSpell(SPELL_SHAMAN_ITEM_MANA_SURGE, SPELLVALUE_BASE_POINT0, damage, GetTarget(), true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_item_mana_surge::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_item_mana_surge::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 40463 - Shaman Tier 6 Trinket
class spell_sha_item_t6_trinket : public AuraScript
{
    PrepareAuraScript(spell_sha_item_t6_trinket);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_ENERGY_SURGE,
            SPELL_SHAMAN_POWER_SURGE
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

        // Healing Surge (3.3.5a Lesser Healing Wave)
        if (spellInfo->SpellFamilyFlags[0] & 0x00000080)
        {
            spellId = SPELL_SHAMAN_ENERGY_SURGE;
            chance = 10;
        }
        // Lightning Bolt
        else if (spellInfo->SpellFamilyFlags[0] & 0x00000001)
        {
            spellId = SPELL_SHAMAN_ENERGY_SURGE;
            chance = 15;
        }
        // Stormstrike
        else if (spellInfo->SpellFamilyFlags[1] & 0x00000010)
        {
            spellId = SPELL_SHAMAN_POWER_SURGE;
            chance = 50;
        }
        else
            return;

        if (roll_chance_i(chance))
            eventInfo.GetActor()->CastSpell(nullptr, spellId, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_item_t6_trinket::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 70811 - Item - Shaman T10 Elemental 2P Bonus
class spell_sha_item_t10_elemental_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_sha_item_t10_elemental_2p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_ELEMENTAL_MASTERY });
    }

    void HandleEffectProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        if (Player* target = GetTarget()->ToPlayer())
            target->ModifySpellCooldown(SPELL_SHAMAN_ELEMENTAL_MASTERY, -aurEff->GetAmount());
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_item_t10_elemental_2p_bonus::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 58877 - Spirit Hunt
class spell_sha_spirit_hunt : public AuraScript
{
    PrepareAuraScript(spell_sha_spirit_hunt);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_SPIRIT_HUNT_HEAL });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetDamage())
            return;

        Unit* owner = GetTarget()->GetOwner();
        if (!owner)
            return;

        int32 bp = CalculatePct(int32(damageInfo->GetDamage()), aurEff->GetAmount());
        // Heal owner
        GetTarget()->CastCustomSpell(SPELL_SHAMAN_SPIRIT_HUNT_HEAL, SPELLVALUE_BASE_POINT0, bp, owner, true, nullptr, aurEff);
        // Heal wolf
        GetTarget()->CastCustomSpell(SPELL_SHAMAN_SPIRIT_HUNT_HEAL, SPELLVALUE_BASE_POINT0, bp, GetTarget(), true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_spirit_hunt::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 58875 - Spirit Walk: the Spirit Wolves cast it only near the shaman
class spell_sha_spirit_walk : public SpellScript
{
    PrepareSpellScript(spell_sha_spirit_walk);

    SpellCastResult CheckCast()
    {
        if (Unit* owner = GetCaster()->GetOwner())
            if (GetCaster()->IsWithinDist(owner, GetSpellInfo()->GetMaxRange(GetSpellInfo()->IsPositive())))
                return SPELL_CAST_OK;

        return SPELL_FAILED_OUT_OF_RANGE;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_sha_spirit_walk::CheckCast);
    }
};

// 55328 - Stoneclaw Totem
class spell_sha_stoneclaw_totem : public SpellScript
{
    PrepareSpellScript(spell_sha_stoneclaw_totem);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_STONECLAW_TOTEM, SPELL_SHAMAN_GLYPH_OF_STONECLAW_TOTEM });
    }

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        Unit* target = GetHitUnit();

        // Cast Absorb on totems
        for (uint8 slot = SUMMON_SLOT_TOTEM_FIRE; slot < MAX_TOTEM_SLOT; ++slot)
        {
            if (!target->m_SummonSlot[slot])
                continue;

            Creature* totem = target->GetMap()->GetCreature(target->m_SummonSlot[slot]);
            if (totem && totem->IsTotem())
                GetCaster()->CastCustomSpell(SPELL_SHAMAN_STONECLAW_TOTEM, SPELLVALUE_BASE_POINT0, GetEffectValue(), totem, true);
        }

        // Glyph of Stoneclaw Totem
        if (AuraEffect* aur = target->GetAuraEffect(SPELL_SHAMAN_GLYPH_OF_STONECLAW_TOTEM, 0))
            GetCaster()->CastCustomSpell(SPELL_SHAMAN_STONECLAW_TOTEM, SPELLVALUE_BASE_POINT0, GetEffectValue() * aur->GetAmount(), target, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_stoneclaw_totem::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 28823 - Totemic Power (T3 6P Bonus)
class spell_sha_t3_6p_bonus : public AuraScript
{
    PrepareAuraScript(spell_sha_t3_6p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_TOTEMIC_POWER_ARMOR,
            SPELL_SHAMAN_TOTEMIC_POWER_ATTACK_POWER,
            SPELL_SHAMAN_TOTEMIC_POWER_SPELL_POWER,
            SPELL_SHAMAN_TOTEMIC_POWER_MP5
        });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        uint32 spellId;
        Unit* caster = eventInfo.GetActor();
        Unit* target = eventInfo.GetActionTarget();

        switch (target->getClass())
        {
            case CLASS_PALADIN:
            case CLASS_PRIEST:
            case CLASS_SHAMAN:
            case CLASS_DRUID:
                spellId = SPELL_SHAMAN_TOTEMIC_POWER_MP5;
                break;
            case CLASS_MAGE:
            case CLASS_WARLOCK:
                spellId = SPELL_SHAMAN_TOTEMIC_POWER_SPELL_POWER;
                break;
            case CLASS_HUNTER:
            case CLASS_ROGUE:
                spellId = SPELL_SHAMAN_TOTEMIC_POWER_ATTACK_POWER;
                break;
            case CLASS_WARRIOR:
                spellId = SPELL_SHAMAN_TOTEMIC_POWER_ARMOR;
                break;
            default:
                return;
        }

        caster->CastSpell(target, spellId, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_t3_6p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 28820 - Lightning Shield (The Earthshatterer 8P Bonus)
class spell_sha_t3_8p_bonus : public AuraScript
{
    PrepareAuraScript(spell_sha_t3_8p_bonus);

    void PeriodicTick(AuraEffect const* /*aurEff*/)
    {
        PreventDefaultAction();

        // Need remove self if Lightning Shield not active
        if (!GetTarget()->GetAuraEffect(SPELL_AURA_PROC_TRIGGER_SPELL, SPELLFAMILY_SHAMAN, 0x400, 0, 0))
            Remove();
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_t3_8p_bonus::PeriodicTick, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// 64928 - Item - Shaman T8 Elemental 4P Bonus
class spell_sha_t8_elemental_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_sha_t8_elemental_4p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_ELECTRIFIED });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetDamage())
            return;

        // Do not proc from Lightning Overload (patch 3.1~)
        if (SpellInfo const* spellInfo = eventInfo.GetSpellInfo())
        {
            if (spellInfo->Id == SPELL_SHAMAN_LIGHTNING_BOLT_OVERLOAD)
                return;
        }

        SpellInfo const* electrifiedDot = sSpellMgr->AssertSpellInfo(SPELL_SHAMAN_ELECTRIFIED);
        int32 amount = CalculatePct(static_cast<int32>(damageInfo->GetDamage()), aurEff->GetAmount());
        amount /= electrifiedDot->GetMaxTicks();

        eventInfo.GetProcTarget()->CastDelayedSpellWithPeriodicAmount(eventInfo.GetActor(), SPELL_SHAMAN_ELECTRIFIED, SPELL_AURA_PERIODIC_DAMAGE, amount);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_t8_elemental_4p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 67228 - Item - Shaman T9 Elemental 4P Bonus (Lava Burst)
class spell_sha_t9_elemental_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_sha_t9_elemental_4p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_LAVA_BURST_BONUS_DAMAGE });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetDamage())
            return;

        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(SPELL_SHAMAN_LAVA_BURST_BONUS_DAMAGE);
        if (!spellInfo)
            return;

        int32 amount = CalculatePct(static_cast<int32>(damageInfo->GetDamage()), aurEff->GetAmount());
        amount /= spellInfo->GetMaxTicks();

        Unit* caster = eventInfo.GetActor();
        Unit* target = eventInfo.GetActionTarget();
        target->CastDelayedSpellWithPeriodicAmount(caster, SPELL_SHAMAN_LAVA_BURST_BONUS_DAMAGE, SPELL_AURA_PERIODIC_DAMAGE, amount);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_t9_elemental_4p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 70817 - Item - Shaman T10 Elemental 4P Bonus
class spell_sha_t10_elemental_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_sha_t10_elemental_4p_bonus);

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        Unit* caster = eventInfo.GetActor();
        Unit* target = eventInfo.GetActionTarget();

        // try to find spell Flame Shock on the target
        AuraEffect* flameShock = target->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_SHAMAN, 0x10000000, 0x00000000, 0x00000000, caster->GetGUID());
        if (!flameShock)
            return;

        Aura* flameShockAura = flameShock->GetBase();

        int32 maxDuration = flameShockAura->GetMaxDuration();
        int32 newDuration = flameShockAura->GetDuration() + aurEff->GetAmount() * IN_MILLISECONDS;

        flameShockAura->SetDuration(newDuration);
        // is it blizzlike to change max duration for FS?
        if (newDuration > maxDuration)
            flameShockAura->SetMaxDuration(newDuration);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_t10_elemental_4p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 70808 - Item - Shaman T10 Restoration 4P Bonus
class spell_sha_t10_restoration_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_sha_t10_restoration_4p_bonus);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetActor() && eventInfo.GetProcTarget();
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        uint32 triggered_spell_id = 70809;
        SpellInfo const* triggeredSpell = sSpellMgr->GetSpellInfo(triggered_spell_id);

        HealInfo* healInfo = eventInfo.GetHealInfo();

        if (!healInfo || !triggeredSpell)
        {
            return;
        }

        int32 amount = CalculatePct(healInfo->GetHeal(), aurEff->GetAmount()) / triggeredSpell->GetMaxTicks();
        eventInfo.GetProcTarget()->CastDelayedSpellWithPeriodicAmount(GetTarget(), triggered_spell_id, SPELL_AURA_PERIODIC_HEAL, amount, EFFECT_0);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_t10_restoration_4p_bonus::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_t10_restoration_4p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 55198 - Tidal Force
class spell_sha_tidal_force_dummy : public AuraScript
{
    PrepareAuraScript(spell_sha_tidal_force_dummy);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_TIDAL_FORCE_CRIT });
    }

    void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        eventInfo.GetActor()->RemoveAuraFromStack(SPELL_SHAMAN_TIDAL_FORCE_CRIT);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_tidal_force_dummy::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 38443 - Totemic Mastery (T6 2P Bonus)
class spell_sha_totemic_mastery : public AuraScript
{
    PrepareAuraScript(spell_sha_totemic_mastery);

    void HandlePeriodic(AuraEffect const*  /*aurEff*/)
    {
        PreventDefaultAction();

        for (uint8 i = SUMMON_SLOT_TOTEM_FIRE; i < MAX_TOTEM_SLOT; ++i)
            if (!GetTarget()->m_SummonSlot[i])
                return;

        GetTarget()->CastSpell(GetTarget(), 38437, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_totemic_mastery::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

void AddSC_shaman_spell_scripts()
{
    RegisterSpellScript(spell_sha_ancestral_awakening);
    RegisterSpellScript(spell_sha_ancestral_awakening_proc);
    RegisterSpellScript(spell_sha_ancestral_healing);
    RegisterSpellScript(spell_sha_ancestral_resolve);
    RegisterSpellScript(spell_sha_bloodlust);
    RegisterSpellScript(spell_sha_chain_heal);
    RegisterSpellScript(spell_sha_cleanse_spirit);
    RegisterSpellScript(spell_sha_clearcasting);
    RegisterSpellScript(spell_sha_earth_shield);
    RegisterSpellScript(spell_sha_earth_shock);
    RegisterSpellScript(spell_sha_earthbind_totem);
    RegisterSpellScript(spell_sha_earthen_power);
    RegisterSpellScript(spell_sha_earthliving_weapon);
    RegisterSpellScript(spell_sha_earthquake);
    RegisterSpellScript(spell_sha_earthquake_damage);
    RegisterSpellScript(spell_sha_elemental_focus);
    RegisterSpellScript(spell_sha_elemental_overload);
    RegisterSpellScript(spell_sha_feedback);
    RegisterSpellScript(spell_sha_fire_nova);
    RegisterSpellScript(spell_sha_fire_nova_damage);
    RegisterSpellScript(spell_sha_flame_shock);
    RegisterSpellScript(spell_sha_flametongue_weapon);
    RegisterSpellScript(spell_sha_flurry_proc);
    RegisterSpellScript(spell_sha_focused_insight);
    RegisterSpellScript(spell_sha_frozen_power);
    RegisterSpellScript(spell_sha_glyph_of_healing_wave);
    RegisterSpellScript(spell_sha_healing_rain);
    RegisterSpellScript(spell_sha_healing_rain_triggered);
    RegisterSpellScript(spell_sha_healing_stream_totem);
    RegisterSpellScript(spell_sha_heroism);
    RegisterSpellScript(spell_sha_item_lightning_shield);
    RegisterSpellScript(spell_sha_item_lightning_shield_trigger);
    RegisterSpellScript(spell_sha_item_mana_surge);
    RegisterSpellScript(spell_sha_item_t10_elemental_2p_bonus);
    RegisterSpellScript(spell_sha_item_t6_trinket);
    RegisterSpellScript(spell_sha_lava_lash);
    RegisterSpellScript(spell_sha_lava_lash_script);
    RegisterSpellScript(spell_sha_lava_surge);
    RegisterSpellScript(spell_sha_lava_surge_proc);
    RegisterSpellScript(spell_sha_lightning_shield);
    RegisterSpellScript(spell_sha_maelstrom_weapon);
    RegisterSpellScript(spell_sha_mana_tide_totem);
    RegisterSpellScript(spell_sha_nature_guardian);
    RegisterSpellScript(spell_sha_pet_scaling_04);
    RegisterSpellScript(spell_sha_resurgence);
    RegisterSpellScript(spell_sha_rolling_thunder);
    RegisterSpellScript(spell_sha_searing_bolt);
    RegisterSpellScript(spell_sha_spirit_hunt);
    RegisterSpellScript(spell_sha_spirit_link);
    RegisterSpellScript(spell_sha_spirit_walk);
    RegisterSpellScript(spell_sha_static_shock);
    RegisterSpellScript(spell_sha_stoneclaw_totem);
    RegisterSpellScript(spell_sha_t10_elemental_4p_bonus);
    RegisterSpellScript(spell_sha_t10_restoration_4p_bonus);
    RegisterSpellScript(spell_sha_t3_6p_bonus);
    RegisterSpellScript(spell_sha_t3_8p_bonus);
    RegisterSpellScript(spell_sha_t8_elemental_4p_bonus);
    RegisterSpellScript(spell_sha_t9_elemental_4p_bonus);
    RegisterSpellScript(spell_sha_telluric_currents);
    RegisterSpellScript(spell_sha_thunderstorm);
    RegisterSpellScript(spell_sha_tidal_force_dummy);
    RegisterSpellScript(spell_sha_tidal_waves);
    RegisterSpellScript(spell_sha_totemic_mastery);
    RegisterSpellScript(spell_sha_unleash_elements);
    RegisterSpellScript(spell_sha_wind_shear);
    RegisterSpellScript(spell_sha_windfury_weapon);
}
