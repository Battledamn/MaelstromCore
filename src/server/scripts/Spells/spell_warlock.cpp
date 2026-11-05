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

#include "AreaDefines.h"
#include "GameObject.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Pet.h"
#include "PetDefines.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "SpellAuraEffects.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "SpellScriptLoader.h"
#include "TemporarySummon.h"
#include "Unit.h"
#include "Util.h"
/*
 * Scripts for spells with SPELLFAMILY_WARLOCK and SPELLFAMILY_GENERIC spells used by warlock players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_warl_".
 *
 * 4.3.4: the scripts of The Cataclysm Preservation Project (spell_warlock.cpp and the warlock pet scaling of
 * spell_pet.cpp at b061f46) in AzerothCore's script API, with AzerothCore's still right in 4.3.4 and ours.
 */

enum WarlockSpells
{
    SPELL_WARLOCK_AFTERMATH_STUN                    = 85387,
    SPELL_WARLOCK_BURNING_EMBERS_R1                 = 91986,
    SPELL_WARLOCK_BURNING_EMBERS_R2                 = 85112,
    SPELL_WARLOCK_AURA_OF_FOREBODING_R1             = 89604,
    SPELL_WARLOCK_AURA_OF_FOREBODING_R2             = 89605,
    SPELL_WARLOCK_AURA_OF_FOREBODING_ROOT_R1        = 93974,
    SPELL_WARLOCK_AURA_OF_FOREBODING_ROOT_R2        = 93987,
    SPELL_WARLOCK_AURA_OF_FOREBODING_STUN_R1        = 93975,
    SPELL_WARLOCK_AURA_OF_FOREBODING_STUN_R2        = 93986,
    SPELL_WARLOCK_BANE_OF_DOOM_EFFECT               = 18662,
    SPELL_WARLOCK_BANE_OF_HAVOC                     = 80240,
    SPELL_WARLOCK_BANE_OF_HAVOC_TRACKING_AURA       = 85466,
    SPELL_WARLOCK_BANE_OF_HAVOC_DAMAGE              = 85455,
    SPELL_WARLOCK_BURNING_EMBERS_DAMAGE             = 85421,
    SPELL_WARLOCK_CREATE_HEALTHSTONE                = 34130,
    SPELL_WARLOCK_CURSE_OF_GULDAN                   = 86000,
    SPELL_WARLOCK_DEATHS_EMBRACE_R1                 = 47198,
    SPELL_WARLOCK_CORRUPTION_TRIGGERED              = 87389,
    SPELL_WARLOCK_DEMONIC_CIRCLE_ALLOW_CAST         = 62388,
    SPELL_WARLOCK_DEMONIC_CIRCLE_SUMMON             = 48018,
    SPELL_WARLOCK_DEMONIC_CIRCLE_TELEPORT           = 48020,
    SPELL_WARLOCK_DEMONIC_EMPOWERMENT_FELGUARD      = 54508,
    SPELL_WARLOCK_DEMONIC_EMPOWERMENT_FELHUNTER     = 54509,
    SPELL_WARLOCK_DEMONIC_EMPOWERMENT_IMP           = 54444,
    SPELL_WARLOCK_DEMONIC_EMPOWERMENT_SUCCUBUS      = 54435,
    SPELL_WARLOCK_DEMONIC_EMPOWERMENT_VOIDWALKER    = 54443,
    SPELL_WARLOCK_DEMON_SOUL_IMP                    = 79459,
    SPELL_WARLOCK_DEMON_SOUL_FELHUNTER              = 79460,
    SPELL_WARLOCK_DEMON_SOUL_FELGUARD               = 79462,
    SPELL_WARLOCK_DEMON_SOUL_SUCCUBUS               = 79463,
    SPELL_WARLOCK_DEMON_SOUL_VOIDWALKER             = 79464,
    SPELL_WARLOCK_DRAIN_LIFE_HEAL                   = 89653,
    SPELL_WARLOCK_EYE_OF_KILROGG_FLY                = 58083,
    SPELL_WARLOCK_FEL_ARMOR_HEAL                    = 96379,
    SPELL_WARLOCK_FEL_SYNERGY_HEAL                  = 54181,
    SPELL_WARLOCK_GLYPH_OF_EYE_OF_KILROGG           = 58081,
    SPELL_WARLOCK_GLYPH_OF_FEAR                     = 56244,
    SPELL_WARLOCK_GLYPH_OF_SHADOWFLAME              = 63311,
    SPELL_WARLOCK_GLYPH_OF_SOUL_SWAP                = 56226,
    SPELL_WARLOCK_GLYPH_OF_SUCCUBUS                 = 56250,
    SPELL_WARLOCK_GLYPH_OF_VOIDWALKER               = 56247,
    SPELL_WARLOCK_HAUNT_HEAL                        = 48210,
    SPELL_WARLOCK_HEALTHSTONE                       = 6262,
    SPELL_WARLOCK_IMMOLATE                          = 348,
    SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_BUFF_R1    = 60955,
    SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_BUFF_R2    = 60956,
    SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_R1         = 18703,
    SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_R2         = 18704,
    SPELL_WARLOCK_IMPROVED_SOUL_FIRE_PCT            = 85383,
    SPELL_WARLOCK_IMPROVED_SOUL_FIRE_STATE          = 85385,
    SPELL_WARLOCK_JINX_AOE_R1                       = 85547,
    SPELL_WARLOCK_JINX_AOE_R2                       = 86105,
    SPELL_WARLOCK_JINX_TRIGGERED_RAGE               = 85539,
    SPELL_WARLOCK_JINX_TRIGGERED_ENERGY             = 85540,
    SPELL_WARLOCK_JINX_TRIGGERED_RUNIC_POWER        = 85541,
    SPELL_WARLOCK_JINX_TRIGGERED_FOCUS              = 85542,
    SPELL_WARLOCK_JINX_R1                           = 18179,
    SPELL_WARLOCK_LIFE_TAP_ENERGIZE                 = 31818,
    SPELL_WARLOCK_LIFE_TAP_ENERGIZE_2               = 32553,
    SPELL_WARLOCK_MANA_FEED_ENERGIZE                = 91706,
    SPELL_WARLOCK_METAMORPHOSIS                     = 47241,
    SPELL_WARLOCK_NETHER_PROTECTION_HOLY            = 54370,
    SPELL_WARLOCK_NETHER_PROTECTION_FIRE            = 54371,
    SPELL_WARLOCK_NETHER_PROTECTION_FROST           = 54372,
    SPELL_WARLOCK_NETHER_PROTECTION_ARCANE          = 54373,
    SPELL_WARLOCK_NETHER_PROTECTION_SHADOW          = 54374,
    SPELL_WARLOCK_NETHER_PROTECTION_NATURE          = 54375,
    SPELL_WARLOCK_NETHER_WARD                       = 91711,
    SPELL_WARLOCK_NETHER_TALENT                     = 91713,
    SPELL_WARLOCK_PANDEMIC_SCRIPT_EFFECT            = 92931,
    SPELL_WARLOCK_RAIN_OF_FIRE                      = 42223,
    SPELL_WARLOCK_SEARING_PAIN                      = 5676,
    SPELL_WARLOCK_SEED_OF_CORRUPTION                = 27243,
    SPELL_WARLOCK_SEED_OF_CORRUPTION_DAMAGE_GENERIC = 32865,
    SPELL_WARLOCK_SEED_OF_CORRUPTION_TRIGGERED      = 87385,
    SPELL_WARLOCK_SEED_OF_CORRUPTION_VISUAL         = 37826,
    SPELL_WARLOCK_SHADOW_TRANCE                     = 17941,
    SPELL_WARLOCK_SHADOW_WARD                       = 6229,
    SPELL_WARLOCK_SHADOWFLAME_PERIODIC              = 47960,
    SPELL_WARLOCK_SOULBURN_HEALTHSTONE              = 79437,
    SPELL_WARLOCK_SOULBURN_DEMONIC_CIRCLE           = 79438,
    SPELL_WARLOCK_SOULBURN_SEARING_PAIN             = 79440,
    SPELL_WARLOCK_SOULBURN_DUMMY_SEED_OF_CORRUPTION = 93313,
    SPELL_WARLOCK_SOUL_HARVEST_ENERGIZE             = 101977,
    SPELL_WARLOCK_SOUL_SHARD                        = 87388,
    SPELL_WARLOCK_SOUL_SHARD_ENERGIZE               = 95810,
    SPELL_WARLOCK_SOUL_LEECH_HEAL                   = 30294,
    SPELL_WARLOCK_SOULSHATTER_EFFECT                = 32835,
    SPELL_WARLOCK_SOUL_SWAP_CD_MARKER               = 94229,
    SPELL_WARLOCK_SOUL_SWAP_DOT_MARKER              = 92795,
    SPELL_WARLOCK_SOUL_SWAP_MOD_COST                = 92794,
    SPELL_WARLOCK_SOUL_SWAP_OVERRIDE                = 86211,
    SPELL_WARLOCK_UNSTABLE_AFFLICTION               = 30108,
    SPELL_WARLOCK_UNSTABLE_AFFLICTION_DISPEL        = 31117,

    SPELL_GEN_REPLENISHMENT                         = 57669,
    SPELL_PRIEST_SHADOW_WORD_DEATH                  = 32409
};

enum WarlockSpellIcons
{
    WARLOCK_ICON_ID_IMPROVED_LIFE_TAP               = 208,
    WARLOCK_ICON_ID_MANA_FEED                       = 1982,
    WARLOCK_ICON_ID_DEATHS_EMBRACE                  = 3223,
    WARLOCK_ICON_ID_SOULBURN_SEED_OF_CORRUPTION     = 1932,
    WARLOCK_ICON_ID_FIRE_AND_BRIMSTONE              = 3173,
    WARLOCK_ICON_ID_JINX                            = 5002
};

// -85113 - Aftermath: Rain of Fire's chance to stun
class spell_warl_aftermath : public AuraScript
{
    PrepareAuraScript(spell_warl_aftermath);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_AFTERMATH_STUN, SPELL_WARLOCK_RAIN_OF_FIRE });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetSpellInfo() || eventInfo.GetSpellInfo()->Id != SPELL_WARLOCK_RAIN_OF_FIRE)
            return;

        PreventDefaultAction();
        // the proc already rolled the talent's chance for Conflagrate's daze (50% at rank 1): the tooltip's stun chance
        // is on its own
        if (eventInfo.GetProcTarget() && roll_chance_i(aurEff->GetAmount() * 100 / GetSpellInfo()->ProcChance))
            GetTarget()->CastSpell(eventInfo.GetProcTarget(), SPELL_WARLOCK_AFTERMATH_STUN, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_warl_aftermath::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 603 - Bane of Doom: each tick has effect 1's chance (Impending Doom's bonus on it) to summon an Ebon Imp. Ours: CPP
// kept 3.3.5a's Doomguard when the target dies
class spell_warl_bane_of_doom : public AuraScript
{
    PrepareAuraScript(spell_warl_bane_of_doom);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_BANE_OF_DOOM_EFFECT });
    }

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        Unit* caster = GetCaster();
        if (caster && roll_chance_i(caster->CalculateSpellDamage(GetTarget(), GetSpellInfo(), EFFECT_1)))
            caster->CastSpell(GetTarget(), SPELL_WARLOCK_BANE_OF_DOOM_EFFECT, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_bane_of_doom::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 80240 - Bane of Havoc: the warlock carries a tracking aura that shares damage with the baned target
class spell_warl_bane_of_havoc : public AuraScript
{
    PrepareAuraScript(spell_warl_bane_of_havoc);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_BANE_OF_HAVOC_TRACKING_AURA });
    }

    void ApplyTrackingAura(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(caster, SPELL_WARLOCK_BANE_OF_HAVOC_TRACKING_AURA, TRIGGERED_FULL_MASK);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_warl_bane_of_havoc::ApplyTrackingAura, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 85466 - Bane of Havoc (tracking aura): the warlock's damage to other targets, its share to the baned one
class spell_warl_bane_of_havoc_tracking_aura : public AuraScript
{
    PrepareAuraScript(spell_warl_bane_of_havoc_tracking_aura);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_BANE_OF_HAVOC_DAMAGE, SPELL_WARLOCK_BANE_OF_HAVOC });
    }

    void StoreTrackedTarget(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        for (Aura const* aura : GetTarget()->GetSingleCastAuras())
            if (aura->GetId() == SPELL_WARLOCK_BANE_OF_HAVOC && aura->GetCasterGUID() == GetTarget()->GetGUID())
                if (Unit const* owner = aura->GetUnitOwner())
                    _trackedTargetGUID = owner->GetGUID();
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        // nothing to track, or damage to the baned target itself
        if (!_trackedTargetGUID || !eventInfo.GetProcTarget() || eventInfo.GetProcTarget()->GetGUID() == _trackedTargetGUID)
            return false;

        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetDamage())
            return false;

        Unit* tracked = ObjectAccessor::GetUnit(*GetTarget(), _trackedTargetGUID);
        AuraEffect const* bane = tracked ? tracked->GetAuraEffect(SPELL_WARLOCK_BANE_OF_HAVOC, EFFECT_0, GetCasterGUID()) : nullptr;
        if (!bane)
        {
            _trackedTargetGUID.Clear();
            return false;
        }

        _trackedDamage = CalculatePct(static_cast<int32>(damageInfo->GetDamage()), bane->GetAmount());
        return true;
    }

    void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
    {
        if (Unit* tracked = ObjectAccessor::GetUnit(*GetTarget(), _trackedTargetGUID))
            GetTarget()->CastCustomSpell(SPELL_WARLOCK_BANE_OF_HAVOC_DAMAGE, SPELLVALUE_BASE_POINT0, _trackedDamage, tracked, TRIGGERED_FULL_MASK);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_warl_bane_of_havoc_tracking_aura::StoreTrackedTarget, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        DoCheckProc += AuraCheckProcFn(spell_warl_bane_of_havoc_tracking_aura::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_warl_bane_of_havoc_tracking_aura::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }

private:
    ObjectGuid _trackedTargetGUID;
    int32 _trackedDamage = 0;
};

// 710 - Banish: casting it on a banished target ends it (AzerothCore's)
class spell_warl_banish : public SpellScript
{
    PrepareSpellScript(spell_warl_banish);

    void HandleBanish(SpellMissInfo missInfo)
    {
        if (missInfo != SPELL_MISS_IMMUNE)
            return;

        if (Unit* target = GetHitUnit())
            if (Aura* banishAura = target->GetAura(GetSpellInfo()->Id, GetCaster()->GetGUID()))
                banishAura->Remove();
    }

    void Register() override
    {
        BeforeHit += BeforeSpellHitFn(spell_warl_banish::HandleBanish);
    }
};

// Burning Embers: a share of the damage over its ticks, capped by the tooltip: ($SP * 0.7 per rank + effect 2)
static void CastBurningEmbers(Unit* warlock, Unit* target, int32 damage, AuraEffect const* embers)
{
    SpellInfo const* talent = embers->GetSpellInfo();
    int32 maxTicks = int32(std::max<uint32>(1, sSpellMgr->AssertSpellInfo(SPELL_WARLOCK_BURNING_EMBERS_DAMAGE)->GetMaxTicks()));
    int32 tick = CalculatePct(damage, embers->GetAmount()) / maxTicks;
    float coefficient = talent->GetRank() * 0.7f;
    int32 cap = int32(warlock->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_FIRE) * coefficient + talent->Effects[EFFECT_1].CalcValue(warlock)) / maxTicks;
    warlock->CastCustomSpell(SPELL_WARLOCK_BURNING_EMBERS_DAMAGE, SPELLVALUE_BASE_POINT0, std::min(tick, cap), target, true, nullptr, embers);
}

// 3110 - Firebolt: the Imp's hit gives its warlock's Burning Embers; a talent on the warlock does not proc from the
// demon's spells. Ours: CPP's proc row names Firebolt but never fires
class spell_warl_imp_firebolt : public SpellScript
{
    PrepareSpellScript(spell_warl_imp_firebolt);

    void HandleAfterHit()
    {
        Unit* warlock = GetCaster()->GetOwner();
        Unit* target = GetHitUnit();
        if (!warlock || !target || !GetHitDamage())
            return;

        AuraEffect const* embers = warlock->GetAuraEffect(SPELL_WARLOCK_BURNING_EMBERS_R2, EFFECT_0);
        if (!embers)
            embers = warlock->GetAuraEffect(SPELL_WARLOCK_BURNING_EMBERS_R1, EFFECT_0);
        if (embers)
            CastBurningEmbers(warlock, target, GetHitDamage(), embers);
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_warl_imp_firebolt::HandleAfterHit);
    }
};

// -91986 - Burning Embers: a share of Soul Fire's damage over time
class spell_warl_burning_embers : public AuraScript
{
    PrepareAuraScript(spell_warl_burning_embers);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_BURNING_EMBERS_DAMAGE });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetDamageInfo() && eventInfo.GetDamageInfo()->GetDamage() && eventInfo.GetProcTarget();
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        CastBurningEmbers(GetTarget(), eventInfo.GetProcTarget(), static_cast<int32>(eventInfo.GetDamageInfo()->GetDamage()), aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warl_burning_embers::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_warl_burning_embers::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// Fire and Brimstone: Incinerate's and Chaos Bolt's bonus on a target with the warlock's Immolate
static void ApplyFireAndBrimstone(Unit* caster, Unit* target, int32& damage)
{
    if (target->HasAuraState(AURA_STATE_CONFLAGRATE) && target->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_WARLOCK, 0x4, 0, 0, caster->GetGUID()))
        if (AuraEffect const* aurEff = caster->GetDummyAuraEffect(SPELLFAMILY_WARLOCK, WARLOCK_ICON_ID_FIRE_AND_BRIMSTONE, EFFECT_0))
            AddPct(damage, aurEff->GetAmount());
}

// 50796 - Chaos Bolt
class spell_warl_chaos_bolt : public SpellScript
{
    PrepareSpellScript(spell_warl_chaos_bolt);

    void HandleDamageBonus(SpellEffIndex /*effIndex*/)
    {
        int32 damage = GetEffectValue();
        if (Unit* target = GetHitUnit())
            ApplyFireAndBrimstone(GetCaster(), target, damage);
        SetEffectValue(damage);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_warl_chaos_bolt::HandleDamageBonus, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 17962 - Conflagrate: a share of the whole Immolate, which stays
class spell_warl_conflagrate : public SpellScript
{
    PrepareSpellScript(spell_warl_conflagrate);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_IMMOLATE });
    }

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            if (AuraEffect const* aurEff = target->GetAuraEffect(SPELL_WARLOCK_IMMOLATE, EFFECT_2, GetCaster()->GetGUID()))
            {
                int32 damage = aurEff->GetAmount() * aurEff->GetTotalTicks();
                SetEffectValue(CalculatePct(damage, GetSpellInfo()->Effects[EFFECT_1].CalcValue(GetCaster())));
            }
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_warl_conflagrate::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 6201 - Create Healthstone: one Healthstone, no soul shard cost
class spell_warl_create_healthstone : public SpellScript
{
    PrepareSpellScript(spell_warl_create_healthstone);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_CREATE_HEALTHSTONE });
    }

    bool Load() override
    {
        return GetCaster()->IsPlayer();
    }

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_WARLOCK_CREATE_HEALTHSTONE, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warl_create_healthstone::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Jinx's spread of Curse of the Elements to the enemies near the cursed target (its 3.5 s copy)
static void CastJinxSpread(Unit* caster, Unit* cursed, AuraEffect const* jinx, AuraEffect const* triggeredBy)
{
    int32 targets = sSpellMgr->AssertSpellInfo(SPELL_WARLOCK_JINX_R1)->Effects[EFFECT_2].CalcValue();
    uint32 spellId = jinx->GetSpellInfo()->GetRank() == 1 ? SPELL_WARLOCK_JINX_AOE_R1 : SPELL_WARLOCK_JINX_AOE_R2;
    caster->CastCustomSpell(spellId, SPELLVALUE_MAX_TARGETS, targets, cursed, true, nullptr, triggeredBy);
}

// 1490 - Curse of the Elements: its periodic dummy (every 2 s) renews Jinx's 3.5 s spread, so the spread lasts as
// long as the curse. Ours: CPP spreads only when the curse lands, and AzerothCore's periodic dummy ticks do not proc
class spell_warl_curse_of_the_elements : public AuraScript
{
    PrepareAuraScript(spell_warl_curse_of_the_elements);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_JINX_R1, SPELL_WARLOCK_JINX_AOE_R1, SPELL_WARLOCK_JINX_AOE_R2 });
    }

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        if (AuraEffect const* jinx = caster->GetDummyAuraEffect(SPELLFAMILY_WARLOCK, WARLOCK_ICON_ID_JINX, EFFECT_0))
            CastJinxSpread(caster, GetTarget(), jinx, aurEff);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_curse_of_the_elements::HandlePeriodic, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 89603 - Cremation: Hand of Gul'dan refreshes the warlock's Immolate on the target, with the current spell power
class spell_warl_cremation : public SpellScript
{
    PrepareSpellScript(spell_warl_cremation);

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            if (AuraEffect* immolate = target->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_WARLOCK, 0x4, 0, 0, GetCaster()->GetGUID()))
            {
                immolate->GetBase()->RefreshTimersWithMods();
                immolate->ChangeAmount(immolate->CalculateAmount(immolate->GetCaster()), false);
            }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warl_cremation::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 702 - Curse of Weakness: Jinx's resource drain on warriors, rogues, death knights and hunters
class spell_warl_curse_of_weakness : public AuraScript
{
    PrepareAuraScript(spell_warl_curse_of_weakness);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_JINX_TRIGGERED_ENERGY, SPELL_WARLOCK_JINX_TRIGGERED_FOCUS,
            SPELL_WARLOCK_JINX_TRIGGERED_RAGE, SPELL_WARLOCK_JINX_TRIGGERED_RUNIC_POWER });
    }

    void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();
        Player* target = GetTarget()->ToPlayer();
        if (!caster || !target)
            return;

        AuraEffect const* jinx = caster->GetDummyAuraEffect(SPELLFAMILY_WARLOCK, WARLOCK_ICON_ID_JINX, EFFECT_1);
        if (!jinx)
            return;

        uint32 spellId = 0;
        switch (target->getClass())
        {
            case CLASS_WARRIOR:
                spellId = SPELL_WARLOCK_JINX_TRIGGERED_RAGE;
                break;
            case CLASS_ROGUE:
                spellId = SPELL_WARLOCK_JINX_TRIGGERED_ENERGY;
                break;
            case CLASS_DEATH_KNIGHT:
                spellId = SPELL_WARLOCK_JINX_TRIGGERED_RUNIC_POWER;
                break;
            case CLASS_HUNTER:
                spellId = SPELL_WARLOCK_JINX_TRIGGERED_FOCUS;
                break;
            default:
                return;
        }

        caster->CastCustomSpell(spellId, SPELLVALUE_BASE_POINT0, jinx->GetAmount(), target, true);
    }

    void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        target->RemoveAurasDueToSpell(SPELL_WARLOCK_JINX_TRIGGERED_RAGE);
        target->RemoveAurasDueToSpell(SPELL_WARLOCK_JINX_TRIGGERED_ENERGY);
        target->RemoveAurasDueToSpell(SPELL_WARLOCK_JINX_TRIGGERED_RUNIC_POWER);
        target->RemoveAurasDueToSpell(SPELL_WARLOCK_JINX_TRIGGERED_FOCUS);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_warl_curse_of_weakness::HandleApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_warl_curse_of_weakness::HandleRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
    }
};

// -980 - Bane of Agony: slow at first, building up (AzerothCore's ticks by quarter of the duration)
class spell_warl_curse_of_agony : public AuraScript
{
    PrepareAuraScript(spell_warl_curse_of_agony);

    void ApplyEffect(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        _tickAmount = aurEff->GetAmount();
    }

    void HandleEffectPeriodicUpdate(AuraEffect* aurEff)
    {
        switch (aurEff->GetTickNumber())
        {
            // 1..4 ticks, 1/2 from normal tick damage
            case 1:
                aurEff->SetAmount(_tickAmount / 2);
                break;
            // 5..8 ticks have normal tick damage
            case 5:
                aurEff->SetAmount(_tickAmount);
                break;
            // 9..12 ticks, 3/2 from normal tick damage
            case 9:
                aurEff->SetAmount((_tickAmount + 1) * 3 / 2); // +1 prevent 0.5 damage possible lost at 1..4 ticks
                break;
            // 13 and later (the glyph's and haste's ticks), twice normal tick damage
            case 13:
                aurEff->SetAmount(_tickAmount * 2);
                break;
        }
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_warl_curse_of_agony::ApplyEffect, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_warl_curse_of_agony::HandleEffectPeriodicUpdate, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }

private:
    int32 _tickAmount = 0;
};

// -63156 - Decimation: only against targets at or below its health threshold
class spell_warl_decimation : public AuraScript
{
    PrepareAuraScript(spell_warl_decimation);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (AuraEffect const* threshold = GetEffect(EFFECT_1))
            return eventInfo.GetProcTarget() && eventInfo.GetProcTarget()->GetHealthPct() <= threshold->GetAmount();
        return false;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warl_decimation::CheckProc);
    }
};

// -30143 - Demonic Aegis: losing the talent removes Demon Armor and Fel Armor, so they lose its bonus (AzerothCore's)
class spell_warl_demonic_aegis : public AuraScript
{
    PrepareAuraScript(spell_warl_demonic_aegis);

    void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasWithFamily(SPELLFAMILY_WARLOCK, 0, 0x20000020, 0, ObjectGuid::Empty);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_warl_demonic_aegis::HandleRemove, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
    }
};

// 48018 - Demonic Circle: Summon
class spell_warl_demonic_circle_summon : public AuraScript
{
    PrepareAuraScript(spell_warl_demonic_circle_summon);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_DEMONIC_CIRCLE_ALLOW_CAST, SPELL_WARLOCK_DEMONIC_CIRCLE_TELEPORT });
    }

    void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes mode)
    {
        // If effect is removed by expire remove the summoned demonic circle too.
        if (!(mode & AURA_EFFECT_HANDLE_REAPPLY))
            GetTarget()->RemoveGameObject(GetId(), true);

        GetTarget()->RemoveAura(SPELL_WARLOCK_DEMONIC_CIRCLE_ALLOW_CAST);
    }

    void HandleDummyTick(AuraEffect const* /*aurEff*/)
    {
        if (GameObject* circle = GetTarget()->GetGameObject(GetId()))
        {
            // Here we check if player is in demonic circle teleport range, if so add
            // WARLOCK_DEMONIC_CIRCLE_ALLOW_CAST; allowing him to cast the WARLOCK_DEMONIC_CIRCLE_TELEPORT.
            // If not in range remove the WARLOCK_DEMONIC_CIRCLE_ALLOW_CAST.

            SpellInfo const* spellInfo = sSpellMgr->AssertSpellInfo(SPELL_WARLOCK_DEMONIC_CIRCLE_TELEPORT);

            if (GetTarget()->IsWithinDist(circle, spellInfo->GetMaxRange(true)))
            {
                if (!GetTarget()->HasAura(SPELL_WARLOCK_DEMONIC_CIRCLE_ALLOW_CAST))
                    GetTarget()->CastSpell(GetTarget(), SPELL_WARLOCK_DEMONIC_CIRCLE_ALLOW_CAST, true);
            }
            else
                GetTarget()->RemoveAura(SPELL_WARLOCK_DEMONIC_CIRCLE_ALLOW_CAST);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_warl_demonic_circle_summon::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_demonic_circle_summon::HandleDummyTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 48020 - Demonic Circle: Teleport
class spell_warl_demonic_circle_teleport : public AuraScript
{
    PrepareAuraScript(spell_warl_demonic_circle_teleport);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_DEMONIC_CIRCLE_SUMMON });
    }

    void HandleTeleport(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* player = GetTarget()->ToPlayer())
        {
            if (GameObject* circle = player->GetGameObject(SPELL_WARLOCK_DEMONIC_CIRCLE_SUMMON))
            {
                player->NearTeleportTo(circle->GetPositionX(), circle->GetPositionY(), circle->GetPositionZ(), circle->GetOrientation(), false, false, false, true);
                player->RemoveMovementImpairingAuras(false);
            }
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_warl_demonic_circle_teleport::HandleTeleport, EFFECT_0, SPELL_AURA_MECHANIC_IMMUNITY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 47193 - Demonic Empowerment
class spell_warl_demonic_empowerment : public SpellScript
{
    PrepareSpellScript(spell_warl_demonic_empowerment);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_DEMONIC_EMPOWERMENT_SUCCUBUS, SPELL_WARLOCK_DEMONIC_EMPOWERMENT_VOIDWALKER,
            SPELL_WARLOCK_DEMONIC_EMPOWERMENT_FELGUARD, SPELL_WARLOCK_DEMONIC_EMPOWERMENT_FELHUNTER, SPELL_WARLOCK_DEMONIC_EMPOWERMENT_IMP });
    }

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        Creature* pet = GetHitCreature();
        if (!pet || !pet->IsPet())
            return;

        switch (pet->GetCreatureTemplate()->family)
        {
            case CREATURE_FAMILY_SUCCUBUS:
                pet->CastSpell(pet, SPELL_WARLOCK_DEMONIC_EMPOWERMENT_SUCCUBUS, true);
                break;
            case CREATURE_FAMILY_VOIDWALKER:
            {
                SpellInfo const* spellInfo = sSpellMgr->AssertSpellInfo(SPELL_WARLOCK_DEMONIC_EMPOWERMENT_VOIDWALKER);
                int32 hp = int32(pet->CountPctFromMaxHealth(GetCaster()->CalculateSpellDamage(pet, spellInfo, EFFECT_0)));
                pet->CastCustomSpell(pet, SPELL_WARLOCK_DEMONIC_EMPOWERMENT_VOIDWALKER, &hp, nullptr, nullptr, true);
                break;
            }
            case CREATURE_FAMILY_FELGUARD:
                pet->CastSpell(pet, SPELL_WARLOCK_DEMONIC_EMPOWERMENT_FELGUARD, true);
                break;
            case CREATURE_FAMILY_FELHUNTER:
                pet->CastSpell(pet, SPELL_WARLOCK_DEMONIC_EMPOWERMENT_FELHUNTER, true);
                break;
            case CREATURE_FAMILY_IMP:
                pet->CastSpell(pet, SPELL_WARLOCK_DEMONIC_EMPOWERMENT_IMP, true);
                break;
            default:
                break;
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warl_demonic_empowerment::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 77801 - Demon Soul: the warlock's buff by the demon it is cast on
class spell_warl_demon_soul : public SpellScript
{
    PrepareSpellScript(spell_warl_demon_soul);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_DEMON_SOUL_IMP, SPELL_WARLOCK_DEMON_SOUL_FELHUNTER, SPELL_WARLOCK_DEMON_SOUL_FELGUARD,
            SPELL_WARLOCK_DEMON_SOUL_SUCCUBUS, SPELL_WARLOCK_DEMON_SOUL_VOIDWALKER });
    }

    void OnHitTarget(SpellEffIndex /*effIndex*/)
    {
        Creature* pet = GetHitCreature();
        if (!pet || !pet->IsPet())
            return;

        uint32 spellId = 0;
        switch (pet->GetCreatureTemplate()->family)
        {
            case CREATURE_FAMILY_SUCCUBUS:
                spellId = SPELL_WARLOCK_DEMON_SOUL_SUCCUBUS;
                break;
            case CREATURE_FAMILY_VOIDWALKER:
                spellId = SPELL_WARLOCK_DEMON_SOUL_VOIDWALKER;
                break;
            case CREATURE_FAMILY_FELGUARD:
                spellId = SPELL_WARLOCK_DEMON_SOUL_FELGUARD;
                break;
            case CREATURE_FAMILY_FELHUNTER:
                spellId = SPELL_WARLOCK_DEMON_SOUL_FELHUNTER;
                break;
            case CREATURE_FAMILY_IMP:
                spellId = SPELL_WARLOCK_DEMON_SOUL_IMP;
                break;
            default:
                return;
        }

        GetCaster()->CastSpell(GetCaster(), spellId, false);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warl_demon_soul::OnHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 689, 89420 - Drain Life: heals the warlock every tick (89653), more with Death's Embrace
class spell_warl_drain_life : public AuraScript
{
    PrepareAuraScript(spell_warl_drain_life);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_DRAIN_LIFE_HEAL, SPELL_WARLOCK_DEATHS_EMBRACE_R1 });
    }

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        SpellInfo const* healSpell = sSpellMgr->AssertSpellInfo(SPELL_WARLOCK_DRAIN_LIFE_HEAL);
        int32 baseAmount = caster->CalculateSpellDamage(caster, healSpell, aurEff->GetEffIndex());

        // Death's Embrace: its threshold is rank 1's effect 2 for every rank ($47198s3; CPP read the heal's, which
        // has none, so it never applied)
        if (AuraEffect const* deathsEmbrace = caster->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_WARLOCK, WARLOCK_ICON_ID_DEATHS_EMBRACE, EFFECT_0))
            if (caster->HealthBelowPct(sSpellMgr->AssertSpellInfo(SPELL_WARLOCK_DEATHS_EMBRACE_R1)->Effects[EFFECT_2].CalcValue()))
                baseAmount += deathsEmbrace->GetAmount();

        caster->CastCustomSpell(caster, SPELL_WARLOCK_DRAIN_LIFE_HEAL, &baseAmount, nullptr, nullptr, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_drain_life::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 1120 - Drain Soul: 3 soul shards when the target dies while drained and gives experience or honor
class spell_warl_drain_soul : public AuraScript
{
    PrepareAuraScript(spell_warl_drain_soul);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_SOUL_SHARD_ENERGIZE });
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_DEATH)
            return;

        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
                if (player->isHonorOrXPTarget(GetTarget()))
                {
                    caster->CastSpell(caster, SPELL_WARLOCK_SOUL_SHARD_ENERGIZE, true);
                    // Glyph of Drain Soul: the mana back. Ours: not in CPP.
                    if (caster->HasAura(58070))
                        caster->CastSpell(caster, 58068, true);
                }
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_warl_drain_soul::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};

// 47422 - Everlasting Affliction: refreshes the warlock's Corruption on the target, with the current spell power
class spell_warl_everlasting_affliction : public SpellScript
{
    PrepareSpellScript(spell_warl_everlasting_affliction);

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            if (AuraEffect* corruption = target->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_WARLOCK, 0x2, 0, 0, GetCaster()->GetGUID()))
            {
                corruption->GetBase()->RefreshTimersWithMods();
                corruption->ChangeAmount(corruption->CalculateAmount(corruption->GetCaster()), false);
            }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warl_everlasting_affliction::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 126 - Eye of Kilrogg: the Glyph of Eye of Kilrogg lets it fly where flying mounts can (AzerothCore's)
class spell_warl_eye_of_kilrogg : public AuraScript
{
    PrepareAuraScript(spell_warl_eye_of_kilrogg);

    void HandleAuraApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        PreventDefaultAction();
        if (Player* player = GetTarget()->ToPlayer())
        {
            player->UnsummonPetTemporaryIfAny();

            if (player->HasAura(SPELL_WARLOCK_GLYPH_OF_EYE_OF_KILROGG))
                if (Unit* charm = player->GetCharm())
                    if (charm->GetMapId() == MAP_OUTLAND || charm->GetMapId() == MAP_NORTHREND)
                        charm->CastSpell(charm, SPELL_WARLOCK_EYE_OF_KILROGG_FLY, true);
        }
    }

    void HandleAuraRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* player = GetTarget()->ToPlayer())
        {
            if (Unit* charm = player->GetCharm())
                charm->ToTempSummon()->UnSummon();

            player->ResummonPetTemporaryUnSummonedIfAny();
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_warl_eye_of_kilrogg::HandleAuraApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_warl_eye_of_kilrogg::HandleAuraRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 5782 - Fear: the root (effect 2) only with the Glyph of Fear. By the glyph's id: Improved Fear shares its icon
// (98), so CPP's icon check rooted with the talent alone
class spell_warl_fear : public SpellScript
{
    PrepareSpellScript(spell_warl_fear);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_GLYPH_OF_FEAR });
    }

    void HandleGlyphEffect(WorldObject*& target)
    {
        if (!GetCaster()->HasAura(SPELL_WARLOCK_GLYPH_OF_FEAR))
            target = nullptr;
    }

    void Register() override
    {
        OnObjectTargetSelect += SpellObjectTargetSelectFn(spell_warl_fear::HandleGlyphEffect, EFFECT_2, TARGET_UNIT_TARGET_ENEMY);
    }
};

// 28176 - Fel Armor: heals a share of single-target spell damage (Hellfire, Rain of Fire and Seed of Corruption's
// explosion do not count, nor Hellfire's damage to the warlock; CPP takes all of them)
class spell_warl_fel_armor : public AuraScript
{
    PrepareAuraScript(spell_warl_fel_armor);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_FEL_ARMOR_HEAL });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        return eventInfo.GetDamageInfo() && eventInfo.GetDamageInfo()->GetDamage() && spellInfo
            && !spellInfo->IsAffectingArea() && eventInfo.GetProcTarget() != GetTarget();
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        int32 heal = CalculatePct(static_cast<int32>(eventInfo.GetDamageInfo()->GetDamage()), aurEff->GetAmount());
        GetTarget()->CastCustomSpell(SPELL_WARLOCK_FEL_ARMOR_HEAL, SPELLVALUE_BASE_POINT0, heal, GetTarget(), true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warl_fel_armor::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_warl_fel_armor::HandleProc, EFFECT_1, SPELL_AURA_DUMMY);
    }
};

// 77799 - Fel Flame: extends the warlock's Unstable Affliction or Immolate on the target
class spell_warl_fel_flame : public SpellScript
{
    PrepareSpellScript(spell_warl_fel_flame);

    void OnHitTarget(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        Aura* aura = target->GetAura(SPELL_WARLOCK_UNSTABLE_AFFLICTION, caster->GetGUID());
        if (!aura)
            aura = target->GetAura(SPELL_WARLOCK_IMMOLATE, caster->GetGUID());
        if (!aura)
            return;

        int32 newDuration = aura->GetDuration() + GetSpellInfo()->Effects[EFFECT_1].CalcValue() * IN_MILLISECONDS;
        aura->SetDuration(std::min(newDuration, aura->GetMaxDuration()));
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warl_fel_flame::OnHitTarget, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// -47230 - Fel Synergy: heals the warlock's demon by a share of the warlock's damage
class spell_warl_fel_synergy : public AuraScript
{
    PrepareAuraScript(spell_warl_fel_synergy);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_FEL_SYNERGY_HEAL });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        return damageInfo && damageInfo->GetDamage() && (GetTarget()->GetGuardianPet() || GetTarget()->GetCharm());
    }

    void OnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        int32 heal = CalculatePct(static_cast<int32>(eventInfo.GetDamageInfo()->GetDamage()), aurEff->GetAmount());
        GetTarget()->CastCustomSpell(SPELL_WARLOCK_FEL_SYNERGY_HEAL, SPELLVALUE_BASE_POINT0, heal, (Unit*)nullptr, true, nullptr, aurEff); // TARGET_UNIT_PET
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warl_fel_synergy::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_warl_fel_synergy::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 56244 - Glyph of Fear: Fear's cooldown (its amount in seconds)
class spell_warl_glyph_of_fear : public AuraScript
{
    PrepareAuraScript(spell_warl_glyph_of_fear);

    void HandleCooldownMod(AuraEffect const* aurEff, SpellModifier*& spellMod)
    {
        if (!spellMod)
        {
            spellMod = new SpellModifier(GetAura());
            spellMod->op = SPELLMOD_COOLDOWN;
            spellMod->type = SPELLMOD_FLAT;
            spellMod->spellId = GetId();
            spellMod->mask = GetSpellInfo()->Effects[aurEff->GetEffIndex()].SpellClassMask;
        }

        spellMod->value = aurEff->GetAmount() * IN_MILLISECONDS;
    }

    void Register() override
    {
        DoEffectCalcSpellMod += AuraEffectCalcSpellModFn(spell_warl_glyph_of_fear::HandleCooldownMod, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 63310 - Glyph of Shadowflame
class spell_warl_glyph_of_shadowflame : public AuraScript
{
    PrepareAuraScript(spell_warl_glyph_of_shadowflame);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_GLYPH_OF_SHADOWFLAME });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(eventInfo.GetProcTarget(), SPELL_WARLOCK_GLYPH_OF_SHADOWFLAME, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_warl_glyph_of_shadowflame::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 48181 - Haunt: the soul heals the warlock for its share of the damage when the aura ends
class spell_warl_haunt : public SpellScript
{
    PrepareSpellScript(spell_warl_haunt);

    void HandleAfterHit()
    {
        if (Aura* aura = GetHitAura())
            if (AuraEffect* aurEff = aura->GetEffect(EFFECT_1))
                aurEff->SetAmount(CalculatePct(aurEff->GetAmount(), GetHitDamage()));
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_warl_haunt::HandleAfterHit);
    }
};

class spell_warl_haunt_aura : public AuraScript
{
    PrepareAuraScript(spell_warl_haunt_aura);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_HAUNT_HEAL });
    }

    void HandleRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
        {
            int32 amount = aurEff->GetAmount();
            GetTarget()->CastCustomSpell(caster, SPELL_WARLOCK_HAUNT_HEAL, &amount, nullptr, nullptr, true, nullptr, aurEff, GetCasterGUID());
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_warl_haunt_aura::HandleRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }
};

// 755 - Health Funnel: the warlock sacrifices effect 2's share of health every tick; Improved Health Funnel's buff
class spell_warl_health_funnel : public AuraScript
{
    PrepareAuraScript(spell_warl_health_funnel);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_R1, SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_R2,
            SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_BUFF_R1, SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_BUFF_R2 });
    }

    void ApplyEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        Unit* target = GetTarget();
        if (caster->HasAura(SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_R2))
            target->CastSpell(target, SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_BUFF_R2, true);
        else if (caster->HasAura(SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_R1))
            target->CastSpell(target, SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_BUFF_R1, true);
    }

    void RemoveEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        target->RemoveAurasDueToSpell(SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_BUFF_R1);
        target->RemoveAurasDueToSpell(SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_BUFF_R2);
    }

    // CPP sacrificed effect 0's 6% (the demon's heal); the tooltip's cost is effect 2's ("Sacrifices $s3%")
    void HandlePeriodic(AuraEffect const* /*aurEff*/)
    {
        Unit* caster = GetCaster();
        if (!caster || !caster->IsAlive())
            return;

        int32 cost = int32(caster->CountPctFromMaxHealth(GetSpellInfo()->Effects[EFFECT_2].CalcValue(caster)));
        if (Player* modOwner = caster->GetSpellModOwner())
            modOwner->ApplySpellMod(GetId(), SPELLMOD_COST, cost);

        SpellNonMeleeDamage damageInfo(caster, caster, GetSpellInfo(), GetSpellInfo()->SchoolMask);
        damageInfo.damage = uint32(std::max(cost, 0));
        caster->SendSpellNonMeleeDamageLog(&damageInfo);
        caster->DealSpellDamage(&damageInfo, false);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_warl_health_funnel::ApplyEffect, EFFECT_0, SPELL_AURA_OBS_MOD_HEALTH, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_warl_health_funnel::RemoveEffect, EFFECT_0, SPELL_AURA_OBS_MOD_HEALTH, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_health_funnel::HandlePeriodic, EFFECT_0, SPELL_AURA_OBS_MOD_HEALTH);
    }
};

// 6262 - Healthstone: heals a share of the user's base health
class spell_warl_healthstone_heal : public SpellScript
{
    PrepareSpellScript(spell_warl_healthstone_heal);

    void HandleOnHit()
    {
        SetHitHeal(int32(CalculatePct(GetCaster()->GetCreateHealth(), GetHitHeal())));
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_warl_healthstone_heal::HandleOnHit);
    }
};

// -18119 - Improved Soul Fire: the damage buff and its marker after Soul Fire
class spell_warl_improved_soul_fire : public AuraScript
{
    PrepareAuraScript(spell_warl_improved_soul_fire);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_IMPROVED_SOUL_FIRE_PCT, SPELL_WARLOCK_IMPROVED_SOUL_FIRE_STATE });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastCustomSpell(SPELL_WARLOCK_IMPROVED_SOUL_FIRE_PCT, SPELLVALUE_BASE_POINT0, aurEff->GetAmount(), GetTarget(), true, nullptr, aurEff);
        GetTarget()->CastSpell(GetTarget(), SPELL_WARLOCK_IMPROVED_SOUL_FIRE_STATE, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_warl_improved_soul_fire::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 29722 - Incinerate: a sixth more on a target with the warlock's Immolate, and Fire and Brimstone
class spell_warl_incinerate : public SpellScript
{
    PrepareSpellScript(spell_warl_incinerate);

    void HandleDamageBonus(SpellEffIndex /*effIndex*/)
    {
        int32 damage = GetEffectValue();
        Unit* target = GetHitUnit();
        if (target && target->HasAuraState(AURA_STATE_CONFLAGRATE)
            && target->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_WARLOCK, 0x4, 0, 0, GetCaster()->GetGUID()))
        {
            damage += damage / 6;
            ApplyFireAndBrimstone(GetCaster(), target, damage);
        }

        SetEffectValue(damage);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_warl_incinerate::HandleDamageBonus, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// -85106 - Impending Doom: Shadow Bolt, Hand of Gul'dan, Soul Fire and Incinerate may shorten Metamorphosis's cooldown
// by effect 1's seconds (a dummy effect at ranks 2 and 3, not an aura, so the proc is on effect 0). Ours: not in CPP
class spell_warl_impending_doom : public AuraScript
{
    PrepareAuraScript(spell_warl_impending_doom);

    void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        if (Player* player = GetTarget()->ToPlayer())
            player->ModifySpellCooldown(SPELL_WARLOCK_METAMORPHOSIS, -GetSpellInfo()->Effects[EFFECT_1].CalcValue() * IN_MILLISECONDS);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_warl_impending_doom::HandleProc, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER);
    }
};

// -18179 - Jinx: its curse spreads to nearby enemies when it lands
class spell_warl_jinx : public AuraScript
{
    PrepareAuraScript(spell_warl_jinx);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_JINX_R1, SPELL_WARLOCK_JINX_AOE_R1, SPELL_WARLOCK_JINX_AOE_R2 });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetProcTarget();
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        CastJinxSpread(GetTarget(), eventInfo.GetProcTarget(), aurEff, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warl_jinx::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_warl_jinx::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 85547, 86105 - Jinx: Curse of the Elements: the enemies near the cursed target, not the target itself, whose own
// Curse of the Elements it would replace (one curse per warlock)
class spell_warl_jinx_aoe : public SpellScript
{
    PrepareSpellScript(spell_warl_jinx_aoe);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (Unit* cursed = GetExplTargetUnit())
            targets.remove(cursed);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warl_jinx_aoe::FilterTargets, EFFECT_ALL, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

// 1454 - Life Tap: effect 2's share of the warlock's health as effect 1's share in mana; Improved Life Tap, Mana Feed
class spell_warl_life_tap : public SpellScript
{
    PrepareSpellScript(spell_warl_life_tap);

    bool Load() override
    {
        return GetCaster()->IsPlayer();
    }

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_LIFE_TAP_ENERGIZE, SPELL_WARLOCK_LIFE_TAP_ENERGIZE_2 });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Player* caster = GetCaster()->ToPlayer();
        if (Unit* target = GetHitUnit())
        {
            int32 damage = int32(caster->CountPctFromMaxHealth(GetSpellInfo()->Effects[EFFECT_2].CalcValue()));
            int32 mana = CalculatePct(damage, GetSpellInfo()->Effects[EFFECT_1].CalcValue());

            // Shouldn't Appear in Combat Log
            target->ModifyHealth(-damage);

            // Improved Life Tap mod
            if (AuraEffect const* aurEff = caster->GetDummyAuraEffect(SPELLFAMILY_WARLOCK, WARLOCK_ICON_ID_IMPROVED_LIFE_TAP, EFFECT_0))
                AddPct(mana, aurEff->GetAmount());

            caster->CastCustomSpell(target, SPELL_WARLOCK_LIFE_TAP_ENERGIZE, &mana, nullptr, nullptr, false);

            // Mana Feed
            if (AuraEffect const* aurEff = caster->GetAuraEffect(SPELL_AURA_ADD_FLAT_MODIFIER, SPELLFAMILY_WARLOCK, WARLOCK_ICON_ID_MANA_FEED, EFFECT_0))
            {
                int32 manaFeedVal = aurEff->GetAmount();
                ApplyPct(manaFeedVal, mana);
                caster->CastCustomSpell(caster, SPELL_WARLOCK_LIFE_TAP_ENERGIZE_2, &manaFeedVal, nullptr, nullptr, true);
            }
        }
    }

    SpellCastResult CheckCast()
    {
        if (int32(GetCaster()->GetHealth()) > int32(GetCaster()->CountPctFromMaxHealth(GetSpellInfo()->Effects[EFFECT_2].CalcValue())))
            return SPELL_CAST_OK;
        return SPELL_FAILED_FIZZLE;
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warl_life_tap::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnCheckCast += SpellCheckCastFn(spell_warl_life_tap::CheckCast);
    }
};

// 91702 - Tamed Pet Passive 07 (every demon's): a basic attack's crit gives the warlock Mana Feed's effect 2 percent of
// total mana. The chance is 0 in the client data; the talent's +100 through the warlock's spell mods. Ours: not in CPP
class spell_warl_mana_feed_passive : public AuraScript
{
    PrepareAuraScript(spell_warl_mana_feed_passive);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_MANA_FEED_ENERGIZE });
    }

    void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        Unit* demon = GetTarget();
        Unit* owner = demon->GetOwner();
        if (!owner)
            return;

        if (AuraEffect const* manaFeed = owner->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_WARLOCK, WARLOCK_ICON_ID_MANA_FEED, EFFECT_2))
            demon->CastCustomSpell(SPELL_WARLOCK_MANA_FEED_ENERGIZE, SPELLVALUE_BASE_POINT0, manaFeed->GetAmount(), owner, true); // TARGET_UNIT_MASTER
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_warl_mana_feed_passive::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// -30299 - Nether Protection: absorbing damage of a school (Shadow Ward, Nether Ward and others) reduces that
// school's damage taken by the talent's share. Ours: AzerothCore's script was 3.3.5a's (a proc trigger on any spell
// hit); 4.3.4's effect 0 is a dummy, the proc needs an absorb, and the buffs carry rank 1's 15%.
class spell_warl_nether_protection : public AuraScript
{
    PrepareAuraScript(spell_warl_nether_protection);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_NETHER_PROTECTION_HOLY, SPELL_WARLOCK_NETHER_PROTECTION_FIRE, SPELL_WARLOCK_NETHER_PROTECTION_NATURE,
            SPELL_WARLOCK_NETHER_PROTECTION_FROST, SPELL_WARLOCK_NETHER_PROTECTION_SHADOW, SPELL_WARLOCK_NETHER_PROTECTION_ARCANE });
    }

    static uint32 BuffForSchool(SpellSchoolMask schoolMask)
    {
        switch (GetFirstSchoolInMask(schoolMask))
        {
            case SPELL_SCHOOL_HOLY:   return SPELL_WARLOCK_NETHER_PROTECTION_HOLY;
            case SPELL_SCHOOL_FIRE:   return SPELL_WARLOCK_NETHER_PROTECTION_FIRE;
            case SPELL_SCHOOL_NATURE: return SPELL_WARLOCK_NETHER_PROTECTION_NATURE;
            case SPELL_SCHOOL_FROST:  return SPELL_WARLOCK_NETHER_PROTECTION_FROST;
            case SPELL_SCHOOL_SHADOW: return SPELL_WARLOCK_NETHER_PROTECTION_SHADOW;
            case SPELL_SCHOOL_ARCANE: return SPELL_WARLOCK_NETHER_PROTECTION_ARCANE;
            default:                  return 0;
        }
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        return damageInfo && damageInfo->GetAbsorb() && BuffForSchool(damageInfo->GetSchoolMask());
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        int32 reduction = -aurEff->GetAmount();
        GetTarget()->CastCustomSpell(BuffForSchool(eventInfo.GetDamageInfo()->GetSchoolMask()), SPELLVALUE_BASE_POINT0, reduction, GetTarget(), true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warl_nether_protection::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_warl_nether_protection::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 687 - Demon Armor, 28176 - Fel Armor: the action bar's ward is Nether Ward with the talent, Shadow Ward without
class spell_warl_nether_ward_overrride : public AuraScript
{
    PrepareAuraScript(spell_warl_nether_ward_overrride);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_NETHER_TALENT, SPELL_WARLOCK_NETHER_WARD, SPELL_WARLOCK_SHADOW_WARD });
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        amount = GetUnitOwner()->HasAura(SPELL_WARLOCK_NETHER_TALENT) ? SPELL_WARLOCK_NETHER_WARD : SPELL_WARLOCK_SHADOW_WARD;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_nether_ward_overrride::CalculateAmount, EFFECT_2, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS);
    }
};

// -85099 - Pandemic: Drain Soul on a target at or below 25% health may refresh Unstable Affliction
class spell_warl_pandemic : public AuraScript
{
    PrepareAuraScript(spell_warl_pandemic);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_PANDEMIC_SCRIPT_EFFECT });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetProcTarget() && eventInfo.GetProcTarget()->GetHealthPct() <= 25.0f
            && roll_chance_i(GetSpellInfo()->Effects[EFFECT_0].CalcValue());
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(eventInfo.GetProcTarget(), SPELL_WARLOCK_PANDEMIC_SCRIPT_EFFECT, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warl_pandemic::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_warl_pandemic::HandleProc, EFFECT_1, SPELL_AURA_ADD_FLAT_MODIFIER);
    }
};

// 92931 - Pandemic: refreshes the warlock's Unstable Affliction on the target, with the current spell power
class spell_warl_pandemic_script : public SpellScript
{
    PrepareSpellScript(spell_warl_pandemic_script);

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            if (AuraEffect* affliction = target->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_WARLOCK, 0x0, 0x100, 0x0, GetCaster()->GetGUID()))
            {
                affliction->GetBase()->RefreshTimersWithMods();
                affliction->ChangeAmount(affliction->CalculateAmount(affliction->GetCaster()), false);
            }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warl_pandemic_script::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

/*
 * Warlock pet scaling (CPP's spell_pet.cpp): the demon's share of the warlock's stats, through the scaling auras
 * every warlock pet learns. AzerothCore has no pet scaling update on the owner's stat changes, so each aura
 * recalculates every 2 seconds, as the hunter's (spell_hunter.cpp).
 */
static Player* GetWarlockPetOwner(Unit* unit)
{
    Unit* owner = unit->GetOwner();
    return owner ? owner->ToPlayer() : nullptr;
}

static void UpdateWarlockPetScaling(AuraEffect const* aurEff)
{
    AuraEffect* effect = aurEff->GetBase()->GetEffect(aurEff->GetEffIndex());
    Unit* pet = aurEff->GetBase()->GetUnitOwner();
    if (!effect || !pet)
        return;

    // a new maximum health or mana keeps the current one, as far as it fits
    uint32 const health = pet->GetHealth();
    uint32 const mana = pet->GetPower(POWER_MANA);
    effect->RecalculateAmount();
    if (aurEff->GetAuraType() == SPELL_AURA_230)
        pet->SetHealth(std::min<uint32>(pet->GetMaxHealth(), health));
    else if (aurEff->GetAuraType() == SPELL_AURA_MOD_INCREASE_ENERGY)
        pet->SetPower(POWER_MANA, std::min<uint32>(pet->GetMaxPower(POWER_MANA), mana));
}

// the warlock's higher of fire and shadow spell power
static int32 GetWarlockSpellPower(Player* owner)
{
    return std::max(owner->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_FIRE), owner->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_SHADOW));
}

#define WARLOCK_PET_SCALING_PERIODIC(ClassName)                                                         \
    void CalcPeriodic(AuraEffect const* /*aurEff*/, bool& isPeriodic, int32& amplitude)                \
    {                                                                                                   \
        isPeriodic = true;                                                                              \
        amplitude = 2 * IN_MILLISECONDS;                                                                \
    }                                                                                                   \
                                                                                                        \
    void HandlePeriodic(AuraEffect const* aurEff)                                                       \
    {                                                                                                   \
        PreventDefaultAction();                                                                         \
        UpdateWarlockPetScaling(aurEff);                                                                \
    }

// 34947 - Warlock Pet Scaling 01: health from the warlock's Stamina by demon (the Glyph of Voidwalker's 20% on
// top), attack power and half the spell power from the warlock's spell power
class spell_warl_pet_scaling_01 : public AuraScript
{
    PrepareAuraScript(spell_warl_pet_scaling_01);

    void CalculateHealthAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = true;
        Unit* pet = GetUnitOwner();
        Player* owner = GetWarlockPetOwner(pet);
        if (!owner)
            return;

        float stamina = owner->GetStat(STAT_STAMINA) * 0.6496f;
        float healthBonus = 0.0f;
        switch (pet->GetEntry())
        {
            case NPC_IMP:
                healthBonus = stamina * 8.4f;
                break;
            case NPC_FELGUARD:
            case NPC_VOIDWALKER:
                healthBonus = stamina * 11.0f;
                break;
            case NPC_SUCCUBUS:
                healthBonus = stamina * 9.1f;
                break;
            case NPC_FELHUNTER:
                healthBonus = stamina * 9.5f;
                break;
            default:
                break;
        }

        // Glyph of Voidwalker: the demon's total health (CPP finds it by its icon)
        if (pet->GetEntry() == NPC_VOIDWALKER)
            if (AuraEffect const* glyph = owner->GetAuraEffect(SPELL_WARLOCK_GLYPH_OF_VOIDWALKER, EFFECT_0))
                healthBonus += CalculatePct(pet->GetCreateHealth() + healthBonus, glyph->GetAmount());

        amount = int32(healthBonus);
    }

    void CalculateAttackPowerAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = true;
        if (Player* owner = GetWarlockPetOwner(GetUnitOwner()))
            amount = GetWarlockSpellPower(owner);
    }

    void CalculateDamageDoneAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = true;
        if (Player* owner = GetWarlockPetOwner(GetUnitOwner()))
        {
            amount = int32(GetWarlockSpellPower(owner) * 0.5f);
            owner->SetUInt32Value(PLAYER_PET_SPELL_POWER, uint32(amount));
        }
    }

    WARLOCK_PET_SCALING_PERIODIC(spell_warl_pet_scaling_01)

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_pet_scaling_01::CalculateHealthAmount, EFFECT_0, SPELL_AURA_230);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_pet_scaling_01::CalculateAttackPowerAmount, EFFECT_1, SPELL_AURA_MOD_ATTACK_POWER);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_pet_scaling_01::CalculateDamageDoneAmount, EFFECT_2, SPELL_AURA_MOD_DAMAGE_DONE);
        DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_warl_pet_scaling_01::CalcPeriodic, EFFECT_ALL, SPELL_AURA_ANY);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_pet_scaling_01::HandlePeriodic, EFFECT_ALL, SPELL_AURA_ANY);
    }
};

// 34956 - Warlock Pet Scaling 02: mana from the warlock's Intellect by demon, 35% of the armor, 40% of the fire
// resistance
class spell_warl_pet_scaling_02 : public AuraScript
{
    PrepareAuraScript(spell_warl_pet_scaling_02);

    void CalculateManaAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = true;
        Player* owner = GetWarlockPetOwner(GetUnitOwner());
        if (!owner)
            return;

        float intellect = owner->GetStat(STAT_INTELLECT);
        amount = int32(intellect * (GetUnitOwner()->GetEntry() == NPC_IMP ? 4.9f : 11.5f));
    }

    void CalculateResistanceAmount(AuraEffect const* aurEff, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = true;
        if (Player* owner = GetWarlockPetOwner(GetUnitOwner()))
        {
            SpellSchoolMask schoolMask = SpellSchoolMask(GetSpellInfo()->Effects[aurEff->GetEffIndex()].MiscValue);
            amount = int32(CalculatePct(float(owner->GetResistance(schoolMask)), schoolMask == SPELL_SCHOOL_MASK_NORMAL ? 35 : 40));
        }
    }

    WARLOCK_PET_SCALING_PERIODIC(spell_warl_pet_scaling_02)

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_pet_scaling_02::CalculateManaAmount, EFFECT_0, SPELL_AURA_MOD_INCREASE_ENERGY);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_pet_scaling_02::CalculateResistanceAmount, EFFECT_1, SPELL_AURA_MOD_RESISTANCE);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_pet_scaling_02::CalculateResistanceAmount, EFFECT_2, SPELL_AURA_MOD_RESISTANCE);
        DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_warl_pet_scaling_02::CalcPeriodic, EFFECT_ALL, SPELL_AURA_ANY);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_pet_scaling_02::HandlePeriodic, EFFECT_ALL, SPELL_AURA_ANY);
    }
};

// 34957 - Warlock Pet Scaling 03: 40% of the frost, arcane and nature resistances
class spell_warl_pet_scaling_03 : public AuraScript
{
    PrepareAuraScript(spell_warl_pet_scaling_03);

    void CalculateResistanceAmount(AuraEffect const* aurEff, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = true;
        if (Player* owner = GetWarlockPetOwner(GetUnitOwner()))
            amount = int32(owner->GetResistance(SpellSchoolMask(GetSpellInfo()->Effects[aurEff->GetEffIndex()].MiscValue)) * 0.4f);
    }

    WARLOCK_PET_SCALING_PERIODIC(spell_warl_pet_scaling_03)

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_pet_scaling_03::CalculateResistanceAmount, EFFECT_ALL, SPELL_AURA_MOD_RESISTANCE);
        DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_warl_pet_scaling_03::CalcPeriodic, EFFECT_ALL, SPELL_AURA_ANY);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_pet_scaling_03::HandlePeriodic, EFFECT_ALL, SPELL_AURA_ANY);
    }
};

// 34958 - Warlock Pet Scaling 04: 40% of the shadow resistance, mana regeneration from the demon's mana and the
// warlock's spell hit (ours: CPP hooks only the first two of its three effects)
class spell_warl_pet_scaling_04 : public AuraScript
{
    PrepareAuraScript(spell_warl_pet_scaling_04);

    void CalculateResistanceAmount(AuraEffect const* aurEff, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = true;
        if (Player* owner = GetWarlockPetOwner(GetUnitOwner()))
            amount = int32(owner->GetResistance(SpellSchoolMask(GetSpellInfo()->Effects[aurEff->GetEffIndex()].MiscValue)) * 0.4f);
    }

    void CalculatePowerRegenAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = true;
        amount = int32(GetUnitOwner()->GetMaxPower(POWER_MANA) * 0.02f);
    }

    void CalculateSpellHitAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = true;
        if (Player* owner = GetWarlockPetOwner(GetUnitOwner()))
            amount = int32(owner->GetTotalAuraModifier(SPELL_AURA_MOD_SPELL_HIT_CHANCE) + owner->GetRatingBonusValue(CR_HIT_SPELL));
    }

    WARLOCK_PET_SCALING_PERIODIC(spell_warl_pet_scaling_04)

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_pet_scaling_04::CalculateResistanceAmount, EFFECT_0, SPELL_AURA_MOD_RESISTANCE);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_pet_scaling_04::CalculatePowerRegenAmount, EFFECT_1, SPELL_AURA_MOD_POWER_REGEN);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_pet_scaling_04::CalculateSpellHitAmount, EFFECT_2, SPELL_AURA_MOD_SPELL_HIT_CHANCE);
        DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_warl_pet_scaling_04::CalcPeriodic, EFFECT_ALL, SPELL_AURA_ANY);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_pet_scaling_04::HandlePeriodic, EFFECT_ALL, SPELL_AURA_ANY);
    }
};

// 61013 - Warlock Pet Scaling 05: the warlock's hit and expertise
class spell_warl_pet_scaling_05 : public AuraScript
{
    PrepareAuraScript(spell_warl_pet_scaling_05);

    void CalculateMeleeHitAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = true;
        if (Player* owner = GetWarlockPetOwner(GetUnitOwner()))
            amount = int32(owner->GetTotalAuraModifier(SPELL_AURA_MOD_HIT_CHANCE) + owner->GetRatingBonusValue(CR_HIT_MELEE));
    }

    void CalculateSpellHitAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = true;
        if (Player* owner = GetWarlockPetOwner(GetUnitOwner()))
            amount = int32(owner->GetTotalAuraModifier(SPELL_AURA_MOD_SPELL_HIT_CHANCE) + owner->GetRatingBonusValue(CR_HIT_SPELL));
    }

    void CalculateExpertiseAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = true;
        if (Player* owner = GetWarlockPetOwner(GetUnitOwner()))
            amount = int32(owner->GetTotalAuraModifier(SPELL_AURA_MOD_EXPERTISE) + owner->GetRatingBonusValue(CR_EXPERTISE));
    }

    WARLOCK_PET_SCALING_PERIODIC(spell_warl_pet_scaling_05)

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_pet_scaling_05::CalculateMeleeHitAmount, EFFECT_0, SPELL_AURA_MOD_HIT_CHANCE);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_pet_scaling_05::CalculateSpellHitAmount, EFFECT_1, SPELL_AURA_MOD_SPELL_HIT_CHANCE);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_pet_scaling_05::CalculateExpertiseAmount, EFFECT_2, SPELL_AURA_MOD_EXPERTISE);
        DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_warl_pet_scaling_05::CalcPeriodic, EFFECT_ALL, SPELL_AURA_ANY);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_pet_scaling_05::HandlePeriodic, EFFECT_ALL, SPELL_AURA_ANY);
    }
};

// 89953 - Warlock Pet Scaling 06: the warlock's spell crit and haste. Ours: CPP gave effect 0 (the crit) the melee
// haste formula; the demons take the warlock's crit as the Water Elemental its mage's (spell_mage_pet_scaling_05).
// The penetration effect stays 0: pets take their owner's in Unit::CalcAbsorbResist.
class spell_warl_pet_scaling_06 : public AuraScript
{
    PrepareAuraScript(spell_warl_pet_scaling_06);

    void CalculateCritAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = true;
        if (Player* owner = GetWarlockPetOwner(GetUnitOwner()))
            amount = int32(std::max(owner->GetFloatValue(PLAYER_SPELL_CRIT_PERCENTAGE1 + SPELL_SCHOOL_FIRE),
                owner->GetFloatValue(PLAYER_SPELL_CRIT_PERCENTAGE1 + SPELL_SCHOOL_SHADOW)));
    }

    void CalculateHasteAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = true;
        if (Player* owner = GetWarlockPetOwner(GetUnitOwner()))
            if (float castSpeed = owner->GetFloatValue(UNIT_MOD_CAST_SPEED))
                amount = int32((1.0f / castSpeed - 1.0f) * 100.0f);
    }

    WARLOCK_PET_SCALING_PERIODIC(spell_warl_pet_scaling_06)

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_pet_scaling_06::CalculateCritAmount, EFFECT_0, SPELL_AURA_MOD_CRIT_PCT);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_pet_scaling_06::CalculateHasteAmount, EFFECT_1, SPELL_AURA_MELEE_SLOW);
        DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_warl_pet_scaling_06::CalcPeriodic, EFFECT_ALL, SPELL_AURA_ANY);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_pet_scaling_06::HandlePeriodic, EFFECT_ALL, SPELL_AURA_ANY);
    }
};

#undef WARLOCK_PET_SCALING_PERIODIC

// 698 - Ritual of Summoning: not in battlegrounds (AzerothCore's)
class spell_warl_ritual_of_summoning : public SpellScript
{
    PrepareSpellScript(spell_warl_ritual_of_summoning);

    SpellCastResult CheckCast()
    {
        if (Player* player = GetCaster()->ToPlayer())
            if (player->InBattleground())
                return SPELL_FAILED_NOT_IN_BATTLEGROUND;
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_warl_ritual_of_summoning::CheckCast);
    }
};

// 6358 - Seduction: the Glyph of Succubus removes the target's damage over time effects (Shadow Word: Death's
// backlash stays) (AzerothCore's)
class spell_warl_seduction : public AuraScript
{
    PrepareAuraScript(spell_warl_seduction);

    void HandleAuraApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();
        Unit* owner = caster ? caster->GetOwner() : nullptr;
        if (!owner || !owner->HasAura(SPELL_WARLOCK_GLYPH_OF_SUCCUBUS))
            return;

        Unit* target = GetTarget();
        target->RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE, ObjectGuid::Empty, target->GetAura(SPELL_PRIEST_SHADOW_WORD_DEATH));
        target->RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE_PERCENT);
        target->RemoveAurasByType(SPELL_AURA_PERIODIC_LEECH);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_warl_seduction::HandleAuraApply, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
    }
};

// 27243 - Seed of Corruption: explodes when the target took effect 1's damage or dies; with Soulburn: Seed of
// Corruption the explosion also gives a soul shard
class spell_warl_seed_of_corruption : public AuraScript
{
    PrepareAuraScript(spell_warl_seed_of_corruption);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_SEED_OF_CORRUPTION_TRIGGERED, SPELL_WARLOCK_SEED_OF_CORRUPTION_VISUAL, SPELL_WARLOCK_SOUL_SHARD });
    }

    void HandleSoulburnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();
        Player* player = caster ? caster->ToPlayer() : nullptr;
        if (player && player->GetLastSoulburnSpell() == GetSpellInfo())
        {
            _affectedBySoulburn = true;
            player->SetLastSoulburnSpell(nullptr);
        }
    }

    void Explode(AuraEffect const* aurEff)
    {
        Unit* caster = GetCaster();
        if (!caster || !caster->IsPlayer())
            return;

        // only a soulburned explosion is cast as triggered by the Seed: it spreads Corruption (CPP)
        if (_affectedBySoulburn)
        {
            caster->CastSpell(caster, SPELL_WARLOCK_SOUL_SHARD, true, nullptr, aurEff);
            caster->CastSpell(GetTarget(), SPELL_WARLOCK_SEED_OF_CORRUPTION_TRIGGERED, true, nullptr, aurEff);
        }
        else
            caster->CastSpell(GetTarget(), SPELL_WARLOCK_SEED_OF_CORRUPTION_TRIGGERED, true);
    }

    void HandleRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_DEATH)
            return;

        Explode(aurEff);
        GetTarget()->CastSpell(GetTarget(), SPELL_WARLOCK_SEED_OF_CORRUPTION_VISUAL, true);
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetDamageInfo() && eventInfo.GetDamageInfo()->GetDamage();
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        _takenDamage += eventInfo.GetDamageInfo()->GetDamage();
        if (_takenDamage < uint32(aurEff->GetAmount()))
            return;

        Explode(aurEff);
        Remove();
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_warl_seed_of_corruption::HandleSoulburnApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_warl_seed_of_corruption::HandleRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        DoCheckProc += AuraCheckProcFn(spell_warl_seed_of_corruption::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_warl_seed_of_corruption::HandleProc, EFFECT_1, SPELL_AURA_DUMMY);
    }

private:
    bool _affectedBySoulburn = false;
    uint32 _takenDamage = 0;
};

// 87385 - Seed of Corruption (explosion): a soulburned Seed's explosion puts Corruption on the targets it hits
class spell_warl_seed_of_corruption_aoe : public SpellScript
{
    PrepareSpellScript(spell_warl_seed_of_corruption_aoe);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_CORRUPTION_TRIGGERED, SPELL_WARLOCK_SEED_OF_CORRUPTION });
    }

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        SpellInfo const* triggeredBy = GetSpell()->GetTriggeredByAuraSpellInfo();
        if (triggeredBy && triggeredBy->Id == SPELL_WARLOCK_SEED_OF_CORRUPTION)
            if (Unit* caster = GetCaster())
                caster->CastSpell(GetHitUnit(), SPELL_WARLOCK_CORRUPTION_TRIGGERED, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warl_seed_of_corruption_aoe::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 32863, 36123, 38252, 39367, 44141, 70388 - Seed of Corruption (creatures' and items'; AzerothCore's)
class spell_warl_seed_of_corruption_generic : public AuraScript
{
    PrepareAuraScript(spell_warl_seed_of_corruption_generic);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_SEED_OF_CORRUPTION_DAMAGE_GENERIC });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetDamage())
            return;

        int32 amount = aurEff->GetAmount() - damageInfo->GetDamage();
        if (amount > 0)
        {
            const_cast<AuraEffect*>(aurEff)->SetAmount(amount);
            return;
        }

        Remove();

        if (Unit* caster = GetCaster())
            caster->CastSpell(eventInfo.GetActionTarget(), SPELL_WARLOCK_SEED_OF_CORRUPTION_DAMAGE_GENERIC, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_warl_seed_of_corruption_generic::HandleProc, EFFECT_1, SPELL_AURA_DUMMY);
    }
};

// 6229 - Shadow Ward: ${$M1+($SP*0.807)}
class spell_warl_shadow_ward : public AuraScript
{
    PrepareAuraScript(spell_warl_shadow_ward);

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;
        if (Unit* caster = GetCaster())
            amount += int32(caster->SpellBaseDamageBonusDone(GetSpellInfo()->GetSchoolMask()) * 0.807f);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_shadow_ward::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
    }
};

// -18094 - Nightfall, 56218 - Glyph of Corruption: Shadow Trance
class spell_warl_shadow_trance_proc : public AuraScript
{
    PrepareAuraScript(spell_warl_shadow_trance_proc);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_SHADOW_TRANCE });
    }

    void OnProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(GetTarget(), SPELL_WARLOCK_SHADOW_TRANCE, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_warl_shadow_trance_proc::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 29341 - Shadowburn: soul shards when the target dies within its duration and gives experience or honor
class spell_warl_shadowburn : public AuraScript
{
    PrepareAuraScript(spell_warl_shadowburn);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_SOUL_SHARD });
    }

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();
        if (!caster || !caster->IsPlayer() || GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_DEATH)
            return;

        if (caster->ToPlayer()->isHonorOrXPTarget(GetTarget()))
            caster->CastCustomSpell(SPELL_WARLOCK_SOUL_SHARD, SPELLVALUE_BASE_POINT0, aurEff->GetAmount(), caster, true, nullptr, aurEff);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_warl_shadowburn::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 47897 - Shadowflame: its burn
class spell_warl_shadowflame : public SpellScript
{
    PrepareSpellScript(spell_warl_shadowflame);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_SHADOWFLAME_PERIODIC });
    }

    void HandlePeriodicDamageEffect(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            GetCaster()->CastSpell(target, SPELL_WARLOCK_SHADOWFLAME_PERIODIC, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warl_shadowflame::HandlePeriodicDamageEffect, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 79268 - Soul Harvest: one soul shard every third tick
class spell_warl_soul_harvest : public AuraScript
{
    PrepareAuraScript(spell_warl_soul_harvest);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_SOUL_HARVEST_ENERGIZE });
    }

    void HandleDummy(AuraEffect const* aurEff)
    {
        if (!(aurEff->GetTickNumber() % 3))
            GetTarget()->CastSpell(GetTarget(), SPELL_WARLOCK_SOUL_HARVEST_ENERGIZE, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_soul_harvest::HandleDummy, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// -30293 - Soul Leech: the talent's share of health and mana, and Replenishment. The engine's trigger with value sets
// only effect 0, the heal: effect 1, the mana, would give nothing
class spell_warl_soul_leech : public AuraScript
{
    PrepareAuraScript(spell_warl_soul_leech);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_GEN_REPLENISHMENT, SPELL_WARLOCK_SOUL_LEECH_HEAL });
    }

    void OnProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        Unit* target = GetTarget();
        int32 amount = aurEff->GetAmount();
        target->CastCustomSpell(target, SPELL_WARLOCK_SOUL_LEECH_HEAL, &amount, &amount, nullptr, true, nullptr, aurEff);
        target->CastSpell(target, SPELL_GEN_REPLENISHMENT, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_warl_soul_leech::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL_WITH_VALUE);
    }
};

// 74434 - Soulburn: what it empowers (Healthstone, Demonic Circle, Searing Pain, Seed of Corruption)
class spell_warl_soulburn : public AuraScript
{
    PrepareAuraScript(spell_warl_soulburn);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_SOULBURN_HEALTHSTONE, SPELL_WARLOCK_SOULBURN_SEARING_PAIN, SPELL_WARLOCK_SOULBURN_DEMONIC_CIRCLE,
            SPELL_WARLOCK_SOULBURN_DUMMY_SEED_OF_CORRUPTION, SPELL_WARLOCK_HEALTHSTONE, SPELL_WARLOCK_DEMONIC_CIRCLE_TELEPORT,
            SPELL_WARLOCK_SEARING_PAIN, SPELL_WARLOCK_SEED_OF_CORRUPTION });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetSpellInfo() && GetTarget()->IsPlayer();
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        Unit* target = GetTarget();
        target->RemoveAurasDueToSpell(SPELL_WARLOCK_SOULBURN_DUMMY_SEED_OF_CORRUPTION);

        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        switch (spellInfo->Id)
        {
            case SPELL_WARLOCK_HEALTHSTONE:
                target->CastSpell(target, SPELL_WARLOCK_SOULBURN_HEALTHSTONE, true, nullptr, aurEff);
                break;
            case SPELL_WARLOCK_DEMONIC_CIRCLE_TELEPORT:
                target->CastSpell(target, SPELL_WARLOCK_SOULBURN_DEMONIC_CIRCLE, true, nullptr, aurEff);
                break;
            case SPELL_WARLOCK_SEARING_PAIN:
                target->CastSpell(target, SPELL_WARLOCK_SOULBURN_SEARING_PAIN, true, nullptr, aurEff);
                break;
            case SPELL_WARLOCK_SEED_OF_CORRUPTION:
                target->ToPlayer()->SetLastSoulburnSpell(spellInfo);
                break;
            default:
                break;
        }
    }

    // the talent Soulburn: Seed of Corruption shows its marker while Soulburn is up
    void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        if (target->GetDummyAuraEffect(SPELLFAMILY_WARLOCK, WARLOCK_ICON_ID_SOULBURN_SEED_OF_CORRUPTION, EFFECT_0))
            target->CastSpell(target, SPELL_WARLOCK_SOULBURN_DUMMY_SEED_OF_CORRUPTION, true);
    }

    void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_WARLOCK_SOULBURN_DUMMY_SEED_OF_CORRUPTION);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warl_soulburn::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_warl_soulburn::HandleProc, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER);
        AfterEffectApply += AuraEffectApplyFn(spell_warl_soulburn::HandleApply, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_warl_soulburn::HandleRemove, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
    }
};

// 29858 - Soulshatter: threat reduction on the enemies threatened by the warlock
class spell_warl_soulshatter : public SpellScript
{
    PrepareSpellScript(spell_warl_soulshatter);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_SOULSHATTER_EFFECT });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (Unit* target = GetHitUnit())
            if (target->CanHaveThreatList() && target->GetThreatMgr().GetThreat(caster) > 0.0f)
                caster->CastSpell(target, SPELL_WARLOCK_SOULSHATTER_EFFECT, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warl_soulshatter::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 86211 - Soul Swap (override): Soul Swap Exhale on the action bar; holds the damage over time effects to move
class spell_warl_soul_swap_override : public AuraScript
{
    PrepareAuraScript(spell_warl_soul_swap_override);

public:
    static constexpr char const* ScriptName = "spell_warl_soul_swap_override";

    void AddDot(uint32 spellId) { _dotList.push_back(spellId); }
    std::list<uint32> const& GetDotList() const { return _dotList; }
    ObjectGuid GetSwapSourceGUID() const { return _swapSourceGUID; }
    void SetSwapSourceGUID(ObjectGuid guid) { _swapSourceGUID = guid; }

    void Register() override { }

private:
    std::list<uint32> _dotList;
    ObjectGuid _swapSourceGUID;
};

static spell_warl_soul_swap_override* GetSoulSwapOverride(Unit* warlock)
{
    Aura* aura = warlock->GetAura(SPELL_WARLOCK_SOUL_SWAP_OVERRIDE);
    return aura ? aura->GetScript<spell_warl_soul_swap_override>(spell_warl_soul_swap_override::ScriptName) : nullptr;
}

// 86121 - Soul Swap: the override on the warlock, then the target names its damage over time effects
class spell_warl_soul_swap : public SpellScript
{
    PrepareSpellScript(spell_warl_soul_swap);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_GLYPH_OF_SOUL_SWAP, SPELL_WARLOCK_SOUL_SWAP_CD_MARKER, SPELL_WARLOCK_SOUL_SWAP_OVERRIDE,
            SPELL_WARLOCK_SOUL_SWAP_DOT_MARKER });
    }

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_WARLOCK_SOUL_SWAP_OVERRIDE, true);
        GetHitUnit()->CastSpell(GetCaster(), SPELL_WARLOCK_SOUL_SWAP_DOT_MARKER, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warl_soul_swap::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 92795 - Soul Swap (marker, cast by the target on the warlock): the warlock's effects its class mask names
class spell_warl_soul_swap_dot_marker : public SpellScript
{
    PrepareSpellScript(spell_warl_soul_swap_dot_marker);

    void HandleHit(SpellEffIndex effIndex)
    {
        Unit* swapSource = GetCaster();
        Unit* warlock = GetHitUnit();
        if (!warlock || !swapSource)
            return;

        spell_warl_soul_swap_override* swapScript = GetSoulSwapOverride(warlock);
        if (!swapScript)
            return;

        flag96 classMask = GetSpellInfo()->Effects[effIndex].SpellClassMask;
        for (auto const& [spellId, aurApp] : swapSource->GetAppliedAuras())
        {
            SpellInfo const* spellInfo = aurApp->GetBase()->GetSpellInfo();
            if (aurApp->GetBase()->GetCasterGUID() == warlock->GetGUID() && spellInfo->SpellFamilyName == SPELLFAMILY_WARLOCK
                && (spellInfo->SpellFamilyFlags & classMask))
                swapScript->AddDot(spellId);
        }

        swapScript->SetSwapSourceGUID(swapSource->GetGUID());
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warl_soul_swap_dot_marker::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 86213 - Soul Swap Exhale: the effects on the new target; without the Glyph of Soul Swap they leave the old one
class spell_warl_soul_swap_exhale : public SpellScript
{
    PrepareSpellScript(spell_warl_soul_swap_exhale);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_SOUL_SWAP_MOD_COST, SPELL_WARLOCK_SOUL_SWAP_OVERRIDE });
    }

    SpellCastResult CheckCast()
    {
        // not on the target Soul Swap took them from
        if (spell_warl_soul_swap_override* swapScript = GetSoulSwapOverride(GetCaster()))
            if (Unit* target = GetExplTargetUnit())
                if (target->GetGUID() == swapScript->GetSwapSourceGUID())
                    return SPELL_FAILED_BAD_TARGETS;

        return SPELL_CAST_OK;
    }

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        caster->CastSpell(caster, SPELL_WARLOCK_SOUL_SWAP_MOD_COST, true);
        bool hasGlyph = caster->HasAura(SPELL_WARLOCK_GLYPH_OF_SOUL_SWAP);

        spell_warl_soul_swap_override* swapScript = GetSoulSwapOverride(caster);
        if (!swapScript)
            return;

        std::list<uint32> const dotList = swapScript->GetDotList();
        Unit* swapSource = ObjectAccessor::GetUnit(*caster, swapScript->GetSwapSourceGUID());
        if (dotList.empty())
            return;

        for (uint32 spellId : dotList)
        {
            caster->AddAura(spellId, GetHitUnit());
            if (!hasGlyph && swapSource)
                swapSource->RemoveAurasDueToSpell(spellId, caster->GetGUID());
        }

        // Soul Swap Exhale leaves the action bar
        caster->RemoveAurasDueToSpell(SPELL_WARLOCK_SOUL_SWAP_OVERRIDE);

        // the glyph's cooldown on Soul Swap
        if (hasGlyph)
            caster->CastSpell(caster, SPELL_WARLOCK_SOUL_SWAP_CD_MARKER, false);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_warl_soul_swap_exhale::CheckCast);
        OnEffectHitTarget += SpellEffectFn(spell_warl_soul_swap_exhale::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 30108, 34438, 34439, 35183 - Unstable Affliction: dispelling it hurts the dispeller for nine ticks and silences
class spell_warl_unstable_affliction : public AuraScript
{
    PrepareAuraScript(spell_warl_unstable_affliction);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_UNSTABLE_AFFLICTION_DISPEL });
    }

    void HandleDispel(DispelInfo* dispelInfo)
    {
        if (Unit* caster = GetCaster())
            if (AuraEffect const* aurEff = GetEffect(EFFECT_1))
            {
                int32 damage = aurEff->GetAmount() * 9;
                caster->CastCustomSpell(dispelInfo->GetDispeller(), SPELL_WARLOCK_UNSTABLE_AFFLICTION_DISPEL, &damage, nullptr, nullptr, true, nullptr, aurEff);
            }
    }

    void Register() override
    {
        AfterDispel += AuraDispelFn(spell_warl_unstable_affliction::HandleDispel);
    }
};

// 46157 - Hand of Gul'dan: Curse of Gul'dan where the meteor lands; Aura of Foreboding roots the enemies within 4 yards
// and stuns those still there 6 s later. Ours: not in CPP, whose creature has no script
struct npc_warl_hand_of_guldan : public ScriptedAI
{
    npc_warl_hand_of_guldan(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(WorldObject* summoner) override
    {
        me->SetReactState(REACT_PASSIVE);
        me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

        Unit* warlock = summoner->ToUnit();
        if (!warlock)
            return;

        // its template and summon properties leave it friendly to everyone, so its area spells would find no enemy
        me->SetFaction(warlock->GetFaction());

        _warlock = summoner->GetGUID();
        me->CastSpell(me, SPELL_WARLOCK_CURSE_OF_GULDAN, true);

        if (warlock->HasAura(SPELL_WARLOCK_AURA_OF_FOREBODING_R2))
            _stun = SPELL_WARLOCK_AURA_OF_FOREBODING_STUN_R2;
        else if (warlock->HasAura(SPELL_WARLOCK_AURA_OF_FOREBODING_R1))
            _stun = SPELL_WARLOCK_AURA_OF_FOREBODING_STUN_R1;
        else
            return;

        uint32 root = _stun == SPELL_WARLOCK_AURA_OF_FOREBODING_STUN_R2 ? SPELL_WARLOCK_AURA_OF_FOREBODING_ROOT_R2 : SPELL_WARLOCK_AURA_OF_FOREBODING_ROOT_R1;
        me->CastSpell(me, root, true, nullptr, nullptr, _warlock);
        _stunTimer = 6 * IN_MILLISECONDS;
    }

    void UpdateAI(uint32 diff) override
    {
        if (!_stunTimer)
            return;

        if (_stunTimer > diff)
        {
            _stunTimer -= diff;
            return;
        }

        _stunTimer = 0;
        me->CastSpell(me, _stun, true, nullptr, nullptr, _warlock);
    }

private:
    ObjectGuid _warlock;
    uint32 _stun = 0;
    uint32 _stunTimer = 0;
};

// 17877 - Shadowburn: the Glyph of Shadowburn resets the cooldown when it fails to kill a target at or below the glyph's
// health, once per marker's duration (as the Glyph of Shadow Word: Death). Ours: not in CPP.
class spell_warl_shadowburn_glyph : public SpellScript
{
    PrepareSpellScript(spell_warl_shadowburn_glyph);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ 56229, 91001 });
    }

    void RememberHealth(SpellEffIndex /*effIndex*/)
    {
        _healthPct = GetHitUnit()->GetHealthPct();
    }

    void HandleGlyph()
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        if (!target || !target->IsAlive() || caster->HasAura(91001))
            return;

        if (AuraEffect const* glyph = caster->GetAuraEffect(56229, EFFECT_0))
            if (_healthPct <= glyph->GetAmount())
            {
                if (Player* player = caster->ToPlayer())
                    player->RemoveSpellCooldown(GetSpellInfo()->Id, true);
                caster->CastSpell(caster, 91001, true); // cooldown marker
            }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warl_shadowburn_glyph::RememberHealth, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
        AfterHit += SpellHitFn(spell_warl_shadowburn_glyph::HandleGlyph);
    }

private:
    float _healthPct = 100.0f;
};

// 25228 - Soul Link: the Glyph of Soul Link shares more of the damage. Ours: not in CPP.
class spell_warl_soul_link : public AuraScript
{
    PrepareAuraScript(spell_warl_soul_link);

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;
        if (Unit* caster = GetCaster())
            if (AuraEffect const* glyph = caster->GetAuraEffect(63312, EFFECT_0))
                amount += glyph->GetAmount();
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_soul_link::CalculateAmount, EFFECT_0, SPELL_AURA_SPLIT_DAMAGE_PCT);
    }
};

void AddSC_warlock_spell_scripts()
{
    RegisterSpellScript(spell_warl_soul_link);
    RegisterSpellScript(spell_warl_shadowburn_glyph);
    RegisterCreatureAI(npc_warl_hand_of_guldan);
    RegisterSpellScript(spell_warl_aftermath);
    RegisterSpellScript(spell_warl_bane_of_doom);
    RegisterSpellScript(spell_warl_bane_of_havoc);
    RegisterSpellScript(spell_warl_bane_of_havoc_tracking_aura);
    RegisterSpellScript(spell_warl_banish);
    RegisterSpellScript(spell_warl_burning_embers);
    RegisterSpellScript(spell_warl_chaos_bolt);
    RegisterSpellScript(spell_warl_conflagrate);
    RegisterSpellScript(spell_warl_create_healthstone);
    RegisterSpellScript(spell_warl_cremation);
    RegisterSpellScript(spell_warl_curse_of_agony);
    RegisterSpellScript(spell_warl_curse_of_the_elements);
    RegisterSpellScript(spell_warl_curse_of_weakness);
    RegisterSpellScript(spell_warl_decimation);
    RegisterSpellScript(spell_warl_demonic_aegis);
    RegisterSpellScript(spell_warl_demonic_circle_summon);
    RegisterSpellScript(spell_warl_demonic_circle_teleport);
    RegisterSpellScript(spell_warl_demonic_empowerment);
    RegisterSpellScript(spell_warl_demon_soul);
    RegisterSpellScript(spell_warl_drain_life);
    RegisterSpellScript(spell_warl_drain_soul);
    RegisterSpellScript(spell_warl_everlasting_affliction);
    RegisterSpellScript(spell_warl_eye_of_kilrogg);
    RegisterSpellScript(spell_warl_fear);
    RegisterSpellScript(spell_warl_fel_armor);
    RegisterSpellScript(spell_warl_fel_flame);
    RegisterSpellScript(spell_warl_fel_synergy);
    RegisterSpellScript(spell_warl_glyph_of_fear);
    RegisterSpellScript(spell_warl_glyph_of_shadowflame);
    RegisterSpellAndAuraScriptPair(spell_warl_haunt, spell_warl_haunt_aura);
    RegisterSpellScript(spell_warl_health_funnel);
    RegisterSpellScript(spell_warl_healthstone_heal);
    RegisterSpellScript(spell_warl_impending_doom);
    RegisterSpellScript(spell_warl_imp_firebolt);
    RegisterSpellScript(spell_warl_improved_soul_fire);
    RegisterSpellScript(spell_warl_incinerate);
    RegisterSpellScript(spell_warl_jinx);
    RegisterSpellScript(spell_warl_jinx_aoe);
    RegisterSpellScript(spell_warl_life_tap);
    RegisterSpellScript(spell_warl_mana_feed_passive);
    RegisterSpellScript(spell_warl_nether_protection);
    RegisterSpellScript(spell_warl_nether_ward_overrride);
    RegisterSpellScript(spell_warl_pandemic);
    RegisterSpellScript(spell_warl_pandemic_script);
    RegisterSpellScript(spell_warl_pet_scaling_01);
    RegisterSpellScript(spell_warl_pet_scaling_02);
    RegisterSpellScript(spell_warl_pet_scaling_03);
    RegisterSpellScript(spell_warl_pet_scaling_04);
    RegisterSpellScript(spell_warl_pet_scaling_05);
    RegisterSpellScript(spell_warl_pet_scaling_06);
    RegisterSpellScript(spell_warl_ritual_of_summoning);
    RegisterSpellScript(spell_warl_seduction);
    RegisterSpellScript(spell_warl_seed_of_corruption);
    RegisterSpellScript(spell_warl_seed_of_corruption_aoe);
    RegisterSpellScript(spell_warl_seed_of_corruption_generic);
    RegisterSpellScript(spell_warl_shadow_trance_proc);
    RegisterSpellScript(spell_warl_shadow_ward);
    RegisterSpellScript(spell_warl_shadowburn);
    RegisterSpellScript(spell_warl_shadowflame);
    RegisterSpellScript(spell_warl_soul_harvest);
    RegisterSpellScript(spell_warl_soul_leech);
    RegisterSpellScript(spell_warl_soul_swap);
    RegisterSpellScript(spell_warl_soul_swap_dot_marker);
    RegisterSpellScript(spell_warl_soul_swap_exhale);
    RegisterSpellScript(spell_warl_soul_swap_override);
    RegisterSpellScript(spell_warl_soulburn);
    RegisterSpellScript(spell_warl_soulshatter);
    RegisterSpellScript(spell_warl_unstable_affliction);
}
