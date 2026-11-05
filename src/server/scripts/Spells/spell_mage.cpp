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
 * Scripts for spells with SPELLFAMILY_MAGE and SPELLFAMILY_GENERIC spells used by mage players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_mage_".
 *
 * 4.3.4: ported from The Cataclysm Preservation Project (spell_mage.cpp at b061f46); the scripts marked
 * "AzerothCore's" are still right in 4.3.4, the ones marked "ours" have no script in CPP.
 */

#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "ObjectAccessor.h"
#include "Pet.h"
#include "Player.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "SpellScriptLoader.h"
#include "TemporarySummon.h"

enum MageSpells
{
    SPELL_MAGE_ARCANE_POTENCY_RANK_1             = 31571,
    SPELL_MAGE_ARCANE_POTENCY_RANK_2             = 31572,
    SPELL_MAGE_ARCANE_POTENCY_TRIGGER_RANK_1     = 57529,
    SPELL_MAGE_ARCANE_POTENCY_TRIGGER_RANK_2     = 57531,
    SPELL_MAGE_ARCANE_MISSILES                   = 5143,
    SPELL_MAGE_ARCANE_MISSILES_AURASTATE         = 79808,
    SPELL_MAGE_BLAZING_SPEED                     = 31643,
    SPELL_MAGE_BRAIN_FREEZE_R1                   = 44546,
    SPELL_MAGE_CAUTERIZE_DOT                     = 87023,
    SPELL_MAGE_COLD_SNAP                         = 11958,
    SPELL_MAGE_COMBUSTION_DAMAGE                 = 83853,
    SPELL_MAGE_DEEP_FREEZE_DAMAGE                = 71757,
    SPELL_MAGE_EARLY_FROST_R1                    = 83049,
    SPELL_MAGE_EARLY_FROST_R2                    = 83050,
    SPELL_MAGE_EARLY_FROST_TRIGGERED_R1          = 83162,
    SPELL_MAGE_EARLY_FROST_TRIGGERED_R2          = 83239,
    SPELL_MAGE_EARLY_FROST_VISUAL                = 94315,
    SPELL_MAGE_FIRE_BLAST                        = 2136,
    SPELL_MAGE_FROST_NOVA                        = 122,
    SPELL_MAGE_FLAME_ORB_DUMMY                   = 82731,
    SPELL_MAGE_FLAME_ORB_SUMMON                  = 84765,
    SPELL_MAGE_FLAME_ORB_AOE                     = 82734,
    SPELL_MAGE_FLAME_ORB_BEAM_DUMMY              = 86719,
    SPELL_MAGE_FLAME_ORB_DAMAGE                  = 82739,
    SPELL_MAGE_FLAME_ORB_SELF_SNARE              = 82736,
    SPELL_MAGE_FROSTFIRE_ORB_DUMMY               = 92283,
    SPELL_MAGE_FROSTFIRE_ORB_SUMMON              = 84714,
    SPELL_MAGE_FROSTFIRE_ORB_AOE                 = 84718,
    SPELL_MAGE_FROSTFIRE_ORB_DAMAGE_R1           = 95969,
    SPELL_MAGE_FROSTFIRE_ORB_DAMAGE_R2           = 84721,
    SPELL_MAGE_FROSTFIRE_ORB_RANK_R2             = 84727,
    SPELL_MAGE_HOT_STREAK                        = 44445,
    SPELL_MAGE_HOT_STREAK_TRIGGERED              = 48108,
    SPELL_MAGE_HYPOTHERMIA                       = 41425,
    SPELL_MAGE_ICE_BARRIER                       = 11426,
    SPELL_MAGE_IMPROVED_HOT_STREAK               = 44446,
    SPELL_MAGE_IMPROVED_POLYMORPH_RANK_1         = 11210,
    SPELL_MAGE_IMPROVED_POLYMORPH_STUN_RANK_1    = 83046,
    SPELL_MAGE_IMPROVED_POLYMORPH_MARKER         = 87515,
    SPELL_MAGE_INCANTERS_ABSORBTION_TRIGGERED    = 44413,
    SPELL_MAGE_INCANTERS_ABSORBTION_KNOCKBACK    = 86261,
    SPELL_MAGE_IGNITE                            = 12654,
    SPELL_MAGE_MASTER_OF_ELEMENTS_ENERGIZE       = 29077,
    SPELL_MAGE_MIRROR_IMAGE_TRIGGERED_FIRE       = 88092,
    SPELL_MAGE_MIRROR_IMAGE_TRIGGERED_ARCANE     = 88091,
    SPELL_MAGE_MIRROR_IMAGE_TRIGGERED_FROST      = 58832,
    SPELL_MAGE_PERMAFROST_REDUCE_HEAL            = 68391,
    SPELL_MAGE_PERMAFROST_HEAL                   = 91394,
    SPELL_MAGE_PIERCING_CHILL                    = 83154,
    SPELL_MAGE_PYROMANIAC_TRIGGERED              = 83582,
    SPELL_MAGE_SLOW                              = 31589,
    SPELL_MAGE_T12_4P_BONUS                      = 99064,
    SPELL_MAGE_FLAMESTRIKE                       = 2120,
    SPELL_MAGE_CHILLED_R1                        = 12484,
    SPELL_MAGE_CHILLED_R2                        = 12485,
    SPELL_MAGE_CONE_OF_COLD_TRIGGER_R1           = 83301,
    SPELL_MAGE_CONE_OF_COLD_TRIGGER_R2           = 83302,
    SPELL_MAGE_SHATTERED_BARRIER_R1              = 44745,
    SPELL_MAGE_SHATTERED_BARRIER_R2              = 54787,
    SPELL_MAGE_SHATTERED_BARRIER_FREEZE_R1       = 55080,
    SPELL_MAGE_SHATTERED_BARRIER_FREEZE_R2       = 83073,
    SPELL_MAGE_IMPROVED_MANA_GEM_TRIGGERED       = 83098,
    SPELL_MAGE_MOLTEN_ARMOR_TRIGGERED            = 34913,
    SPELL_MAGE_RING_OF_FROST_SUMMON              = 82676,
    SPELL_MAGE_RING_OF_FROST_FREEZE              = 82691,
    SPELL_MAGE_RING_OF_FROST_DUMMY               = 91264,
    SPELL_MAGE_FINGERS_OF_FROST                  = 44544,
    SPELL_MAGE_TEMPORAL_DISPLACEMENT             = 80354,

    // AzerothCore's kept scripts
    SPELL_MAGE_SQUIRREL_FORM                     = 32813,
    SPELL_MAGE_GIRAFFE_FORM                      = 32816,
    SPELL_MAGE_SERPENT_FORM                      = 32817,
    SPELL_MAGE_DRAGONHAWK_FORM                   = 32818,
    SPELL_MAGE_WORGEN_FORM                       = 32819,
    SPELL_MAGE_SHEEP_FORM                        = 32820,
    SPELL_MAGE_MANA_SURGE                        = 37445,
    SPELL_MAGE_T8_4P_BONUS                       = 64869,
    SPELL_MAGE_T10_2P_BONUS                      = 70752,
    SPELL_MAGE_T10_2P_BONUS_EFFECT               = 70753
};

enum MageIcons
{
    ICON_MAGE_SHATTER                            = 976,
    ICON_MAGE_IMPROVED_CONE_OF_COLD              = 35,
    ICON_MAGE_IMPROVED_FLAMESTRIKE               = 37,
    ICON_MAGE_IMPROVED_FREEZE                    = 94,
    ICON_MAGE_INCANTERS_ABSORPTION               = 2941,
    ICON_MAGE_IMPROVED_MANA_GEM                  = 1036,
    ICON_MAGE_EARLY_FROST_SKILL                  = 189,
    ICON_MAGE_EARLY_FROST                        = 2114,
    ICON_MAGE_GLYPH_OF_ICE_BLOCK                 = 14,
    ICON_MAGE_GLYPH_OF_ICY_VEINS                 = 2162,
    ICON_MAGE_GLYPH_OF_MIRROR_IMAGE              = 331,
    ICON_MAGE_GLYPH_OF_POLYMORPH                 = 82,
    ICON_MAGE_LIVING_BOMB                        = 3000,
    ICON_MAGE_GLYPH_OF_FROSTFIRE                 = 2946,
    ICON_MAGE_HOT_STREAK                         = 2999
};

enum MageMisc
{
    // Ice Shards' class scripts on Blizzard (TrinityCore's Unit::IsScriptOverriden)
    MAGE_SCRIPT_ICE_SHARDS_R1                    = 836,
    MAGE_SCRIPT_ICE_SHARDS_R2                    = 988,
    NPC_AUROSALIA                                = 18744,

    // family flags of Ignite (12654, flags[0]) and Living Bomb (44457, flags[1])
    SPELL_MAGE_IGNITE_FAMILY_FLAG0               = 0x08000000,
    SPELL_MAGE_LIVING_BOMB_FAMILY_FLAG1          = 0x00020000,

    SPELL_HUNTER_INSANITY                        = 95809,
    SPELL_SHAMAN_EXHAUSTION                      = 57723,
    SPELL_SHAMAN_SATED                           = 57724
};

// TrinityCore's Unit::IsScriptOverriden: a class script aura with this script value on the caster
static bool IsScriptOverriden(Unit const* caster, int32 script)
{
    for (AuraEffect const* aurEff : caster->GetAuraEffectsByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS))
        if (aurEff->GetMiscValue() == script)
            return true;
    return false;
}

// -31571 - Arcane Potency
class spell_mage_arcane_potency : public AuraScript
{
    PrepareAuraScript(spell_mage_arcane_potency);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_ARCANE_POTENCY_TRIGGER_RANK_1, SPELL_MAGE_ARCANE_POTENCY_TRIGGER_RANK_2 });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        uint32 spellId = 0;
        if (GetSpellInfo()->Id == SPELL_MAGE_ARCANE_POTENCY_RANK_1)
            spellId = SPELL_MAGE_ARCANE_POTENCY_TRIGGER_RANK_1;
        else if (GetSpellInfo()->Id == SPELL_MAGE_ARCANE_POTENCY_RANK_2)
            spellId = SPELL_MAGE_ARCANE_POTENCY_TRIGGER_RANK_2;
        if (spellId)
            GetTarget()->CastSpell(GetTarget(), spellId, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_mage_arcane_potency::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 30451 - Arcane Blast (AzerothCore's): the stacking buff comes after the cast, so it does not raise this Arcane Blast
class spell_mage_arcane_blast : public SpellScript
{
    PrepareSpellScript(spell_mage_arcane_blast);

    void HandleTriggerSpell(SpellEffIndex effIndex)
    {
        _triggerSpellId = GetSpellInfo()->Effects[effIndex].TriggerSpell;
        PreventHitDefaultEffect(effIndex);
    }

    void HandleAfterCast()
    {
        if (_triggerSpellId)
            GetCaster()->CastSpell(GetCaster(), _triggerSpellId, TRIGGERED_FULL_MASK);
    }

    void Register() override
    {
        OnEffectLaunch += SpellEffectFn(spell_mage_arcane_blast::HandleTriggerSpell, EFFECT_1, SPELL_EFFECT_TRIGGER_SPELL);
        OnEffectLaunchTarget += SpellEffectFn(spell_mage_arcane_blast::HandleTriggerSpell, EFFECT_1, SPELL_EFFECT_TRIGGER_SPELL);
        AfterCast += SpellCastFn(spell_mage_arcane_blast::HandleAfterCast);
    }

private:
    uint32 _triggerSpellId = 0;
};

// 79683 - Arcane Missiles!: Arcane Missiles (5143) needs the hidden aura 79808 on the caster, which this buff applies
// while it lasts
class spell_mage_arcane_missiles_trigger : public AuraScript
{
    PrepareAuraScript(spell_mage_arcane_missiles_trigger);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_ARCANE_MISSILES_AURASTATE });
    }

    void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
    }

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->CastSpell(GetTarget(), SPELL_MAGE_ARCANE_MISSILES_AURASTATE, true);
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_MAGE_ARCANE_MISSILES_AURASTATE);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_mage_arcane_missiles_trigger::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
        AfterEffectApply += AuraEffectApplyFn(spell_mage_arcane_missiles_trigger::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_mage_arcane_missiles_trigger::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 11113 - Blast Wave: Improved Flamestrike's Flamestrike when it hits two or more
class spell_mage_blast_wave : public SpellScript
{
    PrepareSpellScript(spell_mage_blast_wave);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_FLAMESTRIKE });
    }

    void HandleImprovedFlamestrike(SpellEffIndex /*effIndex*/)
    {
        if (++_hitTargetsCount != 2)
            return;

        AuraEffect const* aurEff = GetCaster()->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_MAGE, ICON_MAGE_IMPROVED_FLAMESTRIKE, EFFECT_0);
        if (!aurEff || !roll_chance_i(aurEff->GetAmount()))
            return;

        if (WorldLocation const* dest = GetExplTargetDest())
            GetCaster()->CastSpell(dest->GetPositionX(), dest->GetPositionY(), dest->GetPositionZ(), SPELL_MAGE_FLAMESTRIKE, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_blast_wave::HandleImprovedFlamestrike, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
    }

private:
    uint8 _hitTargetsCount = 0;
};

// -31641 - Blazing Speed
class spell_mage_blazing_speed : public AuraScript
{
    PrepareAuraScript(spell_mage_blazing_speed);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_BLAZING_SPEED });
    }

    void OnProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(GetTarget(), SPELL_MAGE_BLAZING_SPEED, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_mage_blazing_speed::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 42208 - Blizzard: Ice Shards' chill
class spell_mage_blizzard : public SpellScript
{
    PrepareSpellScript(spell_mage_blizzard);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_CHILLED_R1, SPELL_MAGE_CHILLED_R2 });
    }

    void AddChillEffect(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (Unit* target = GetHitUnit())
        {
            if (IsScriptOverriden(caster, MAGE_SCRIPT_ICE_SHARDS_R1))
                caster->CastSpell(target, SPELL_MAGE_CHILLED_R1, true);
            else if (IsScriptOverriden(caster, MAGE_SCRIPT_ICE_SHARDS_R2))
                caster->CastSpell(target, SPELL_MAGE_CHILLED_R2, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_blizzard::AddChillEffect, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// -86948 - Cauterize (ours; CPP has none): a killing blow is absorbed on the rank's $s1% chance and leaves the mage at
// $s2% of the health, burning (87023's damage); once a minute
class spell_mage_cauterize : public AuraScript
{
    PrepareAuraScript(spell_mage_cauterize);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_CAUTERIZE_DOT });
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        amount = -1;
    }

    void Absorb(AuraEffect* aurEff, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        absorbAmount = 0;
        Unit* target = GetTarget();
        Player* player = target->ToPlayer();
        if (!player || dmgInfo.GetDamage() < target->GetHealth() || player->HasSpellCooldown(SPELL_MAGE_CAUTERIZE_DOT))
            return;

        if (!roll_chance_i(GetSpellInfo()->Effects[EFFECT_0].CalcValue()))
            return;

        absorbAmount = dmgInfo.GetDamage();
        int32 heal = std::max<int32>(0, int32(target->CountPctFromMaxHealth(GetSpellInfo()->Effects[EFFECT_1].CalcValue())) - int32(target->GetHealth()));
        target->CastCustomSpell(target, SPELL_MAGE_CAUTERIZE_DOT, nullptr, &heal, nullptr, true, nullptr, aurEff);
        player->AddSpellCooldown(SPELL_MAGE_CAUTERIZE_DOT, 0, MINUTE * IN_MILLISECONDS);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_cauterize::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_mage_cauterize::Absorb, EFFECT_0);
    }
};

// 11958 - Cold Snap: finishes the cooldowns of the mage's Frost spells
class spell_mage_cold_snap : public SpellScript
{
    PrepareSpellScript(spell_mage_cold_snap);

    bool Load() override
    {
        return GetCaster()->IsPlayer();
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Player* caster = GetCaster()->ToPlayer();
        PlayerSpellMap const& spellMap = caster->GetSpellMap();
        for (PlayerSpellMap::const_iterator itr = spellMap.begin(); itr != spellMap.end(); ++itr)
        {
            SpellInfo const* spellInfo = sSpellMgr->AssertSpellInfo(itr->first);
            if (spellInfo->SpellFamilyName == SPELLFAMILY_MAGE && (spellInfo->GetSchoolMask() & SPELL_SCHOOL_MASK_FROST) && spellInfo->Id != SPELL_MAGE_COLD_SNAP && spellInfo->GetRecoveryTime() > 0)
            {
                SpellCooldowns::iterator citr = caster->GetSpellCooldownMap().find(spellInfo->Id);
                caster->RemoveSpellCooldown(spellInfo->Id, citr != caster->GetSpellCooldownMap().end() && citr->second.needSendToClient);
            }
        }
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_mage_cold_snap::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 120 - Cone of Cold: Improved Cone of Cold's freeze
class spell_mage_cone_of_cold : public SpellScript
{
    PrepareSpellScript(spell_mage_cone_of_cold);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_CONE_OF_COLD_TRIGGER_R1, SPELL_MAGE_CONE_OF_COLD_TRIGGER_R2 });
    }

    void HandleConeOfColdScript(SpellEffIndex /*effIndex*/)
    {
        if (AuraEffect const* aurEff = GetCaster()->GetDummyAuraEffect(SPELLFAMILY_MAGE, ICON_MAGE_IMPROVED_CONE_OF_COLD, EFFECT_0))
            if (aurEff->IsAffectedOnSpell(GetSpellInfo()))
                GetHitUnit()->CastSpell(GetHitUnit(), aurEff->GetSpellInfo()->GetRank() > 1 ? SPELL_MAGE_CONE_OF_COLD_TRIGGER_R2 : SPELL_MAGE_CONE_OF_COLD_TRIGGER_R1, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_cone_of_cold::HandleConeOfColdScript, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// 42955 - Conjure Refreshment: the level's refreshment
struct ConjureRefreshmentData
{
    uint32 minLevel;
    uint32 maxLevel;
    uint32 spellId;
};

static ConjureRefreshmentData const ConjureRefreshment[] =
{
    { 33, 43, 92739 },
    { 44, 53, 92799 },
    { 54, 63, 92802 },
    { 64, 73, 92805 },
    { 74, 79, 74625 },
    { 80, 84, 92822 },
    { 85, 85, 92727 }
};

class spell_mage_conjure_refreshment : public SpellScript
{
    PrepareSpellScript(spell_mage_conjure_refreshment);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        for (ConjureRefreshmentData const& data : ConjureRefreshment)
            if (!ValidateSpellInfo({ data.spellId }))
                return false;
        return true;
    }

    bool Load() override
    {
        return GetCaster()->IsPlayer();
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* target = GetHitUnit();
        uint8 level = target->GetLevel();
        for (ConjureRefreshmentData const& data : ConjureRefreshment)
        {
            if (level < data.minLevel || level > data.maxLevel)
                continue;
            target->CastSpell(target, data.spellId, true);
            break;
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_conjure_refreshment::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 11129 - Combustion: a damage over time of the mage's fire damage over time effects on the target
class spell_mage_combustion : public SpellScript
{
    PrepareSpellScript(spell_mage_combustion);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_COMBUSTION_DAMAGE });
    }

    void HandleScriptEffect(SpellEffIndex effIndex)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        int32 basePoints = 0;
        float scalingFactor = float(GetEffectValue());

        for (AuraEffect const* aurEff : target->GetAuraEffectsByType(SPELL_AURA_PERIODIC_DAMAGE))
        {
            if (aurEff->GetCasterGUID() != caster->GetGUID() || !(aurEff->GetSpellInfo()->GetSchoolMask() & SPELL_SCHOOL_MASK_FIRE)
                || aurEff->GetSpellInfo()->SpellFamilyName != SPELLFAMILY_MAGE)
                continue;

            // the script effect's class mask names Pyroblast, Flamestrike and Frostfire Bolt but neither Ignite nor
            // Living Bomb, which 4.3.4's Combustion combines too
            if (!aurEff->GetSpellInfo()->IsAffected(SPELLFAMILY_MAGE, GetSpellInfo()->Effects[effIndex].SpellClassMask
                | flag96(SPELL_MAGE_IGNITE_FAMILY_FLAG0, SPELL_MAGE_LIVING_BOMB_FAMILY_FLAG1, 0)))
                continue;

            basePoints += CalculatePct(aurEff->GetAmount(), scalingFactor);
        }

        if (basePoints > 0)
            caster->CastCustomSpell(target, SPELL_MAGE_COMBUSTION_DAMAGE, &basePoints, nullptr, nullptr, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_combustion::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 44572 - Deep Freeze: its damage on a target that cannot be stunned
class spell_mage_deep_freeze : public SpellScript
{
    PrepareSpellScript(spell_mage_deep_freeze);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_DEEP_FREEZE_DAMAGE });
    }

    void HandleDamage(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetExplTargetUnit();
        if (target && target->IsCreature() && target->IsImmunedToSpell(GetSpellInfo()))
            caster->CastSpell(target, SPELL_MAGE_DEEP_FREEZE_DAMAGE, true);
    }

    void Register() override
    {
        OnEffectLaunch += SpellEffectFn(spell_mage_deep_freeze::HandleDamage, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// -31661 - Dragon's Breath: Living Bomb's explosion does not break it
class spell_mage_dragon_breath : public AuraScript
{
    PrepareAuraScript(spell_mage_dragon_breath);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        return !(spellInfo && spellInfo->SpellIconID == ICON_MAGE_LIVING_BOMB && spellInfo->SpellFamilyName == SPELLFAMILY_MAGE);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_dragon_breath::CheckProc);
    }
};

// 83162, 83239 - Early Frost (cooldown marker): the visual comes back when it ends
class spell_mage_early_frost : public AuraScript
{
    PrepareAuraScript(spell_mage_early_frost);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_EARLY_FROST_VISUAL });
    }

    void AfterRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(caster, SPELL_MAGE_EARLY_FROST_VISUAL, true, nullptr, aurEff);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_mage_early_frost::AfterRemove, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
    }
};

// 82731 - Flame Orb, 92283 - Frostfire Orb
class spell_mage_flame_orb : public SpellScript
{
    PrepareSpellScript(spell_mage_flame_orb);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_FLAME_ORB_SUMMON, SPELL_MAGE_FROSTFIRE_ORB_SUMMON });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (GetSpellInfo()->Id == SPELL_MAGE_FLAME_ORB_DUMMY)
            caster->CastSpell(caster, SPELL_MAGE_FLAME_ORB_SUMMON, true);
        else if (GetSpellInfo()->Id == SPELL_MAGE_FROSTFIRE_ORB_DUMMY)
            caster->CastSpell(caster, SPELL_MAGE_FROSTFIRE_ORB_SUMMON, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_flame_orb::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 82734 - Flame Orb, 84718 - Frostfire Orb (the orb's pulse): the closest enemy takes the summoner's damage
class spell_mage_flame_orb_aoe_dummy : public SpellScript
{
    PrepareSpellScript(spell_mage_flame_orb_aoe_dummy);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_FLAME_ORB_BEAM_DUMMY, SPELL_MAGE_FROSTFIRE_ORB_DAMAGE_R1, SPELL_MAGE_FROSTFIRE_ORB_DAMAGE_R2,
            SPELL_MAGE_FLAME_ORB_DAMAGE, SPELL_MAGE_FLAME_ORB_SELF_SNARE, SPELL_MAGE_FROSTFIRE_ORB_RANK_R2 });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (targets.empty())
            return;

        targets.sort(Acore::ObjectDistanceOrderPred(GetCaster(), true));
        targets.resize(1);
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        TempSummon* orb = GetCaster()->ToTempSummon();
        Unit* summoner = orb ? orb->GetSummonerUnit() : nullptr;
        Unit* target = GetHitUnit();
        if (!summoner || !target)
            return;

        orb->CastSpell(orb, SPELL_MAGE_FLAME_ORB_SELF_SNARE, true);
        orb->CastSpell(target, SPELL_MAGE_FLAME_ORB_BEAM_DUMMY, true);
        if (GetSpellInfo()->Id == SPELL_MAGE_FLAME_ORB_AOE)
            summoner->CastSpell(target, SPELL_MAGE_FLAME_ORB_DAMAGE, true);
        else if (GetSpellInfo()->Id == SPELL_MAGE_FROSTFIRE_ORB_AOE)
            summoner->CastSpell(target, summoner->HasAura(SPELL_MAGE_FROSTFIRE_ORB_RANK_R2) ? SPELL_MAGE_FROSTFIRE_ORB_DAMAGE_R2 : SPELL_MAGE_FROSTFIRE_ORB_DAMAGE_R1, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_flame_orb_aoe_dummy::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_mage_flame_orb_aoe_dummy::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 54646 - Focus Magic: the mage gets the buff when the target crits
class spell_mage_focus_magic : public AuraScript
{
    PrepareAuraScript(spell_mage_focus_magic);

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        if (Unit* caster = GetCaster())
            if (caster->IsAlive())
                caster->CastSpell(caster, GetSpellInfo()->Effects[EFFECT_1].TriggerSpell, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_mage_focus_magic::HandleProc, EFFECT_1, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 116 - Frostbolt: Early Frost's cooldown, Shatter's damage on frozen targets
class spell_mage_frostbolt : public SpellScript
{
    PrepareSpellScript(spell_mage_frostbolt);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_EARLY_FROST_TRIGGERED_R1, SPELL_MAGE_EARLY_FROST_TRIGGERED_R2, SPELL_MAGE_EARLY_FROST_VISUAL });
    }

    bool Load() override
    {
        // Early Frost is used when the cast starts with it (and not with its cooldown marker)
        Unit* caster = GetCaster();
        if (AuraEffect const* aurEff = caster->GetAuraEffect(SPELL_AURA_ADD_FLAT_MODIFIER, SPELLFAMILY_MAGE, ICON_MAGE_EARLY_FROST_SKILL, EFFECT_0))
            if (!caster->GetAuraEffect(SPELL_AURA_ADD_FLAT_MODIFIER, SPELLFAMILY_MAGE, ICON_MAGE_EARLY_FROST, EFFECT_0))
            {
                if (aurEff->GetId() == SPELL_MAGE_EARLY_FROST_R1)
                    _earlyFrostSpellId = SPELL_MAGE_EARLY_FROST_TRIGGERED_R1;
                else if (aurEff->GetId() == SPELL_MAGE_EARLY_FROST_R2)
                    _earlyFrostSpellId = SPELL_MAGE_EARLY_FROST_TRIGGERED_R2;
            }
        return true;
    }

    void HandleEarlyFrost()
    {
        if (!_earlyFrostSpellId)
            return;

        GetCaster()->CastSpell(GetCaster(), _earlyFrostSpellId, true);
        GetCaster()->RemoveAurasDueToSpell(SPELL_MAGE_EARLY_FROST_VISUAL);
    }

    void RecalculateDamage(SpellEffIndex /*effIndex*/)
    {
        if (GetHitUnit() && GetHitUnit()->HasAuraState(AURA_STATE_FROZEN, GetSpellInfo(), GetCaster()))
            if (AuraEffect const* aurEff = GetCaster()->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_MAGE, ICON_MAGE_SHATTER, EFFECT_1))
            {
                int32 damage = GetHitDamage();
                AddPct(damage, aurEff->GetAmount());
                SetHitDamage(damage);
            }
    }

    void Register() override
    {
        BeforeCast += SpellCastFn(spell_mage_frostbolt::HandleEarlyFrost);
        OnEffectHitTarget += SpellEffectFn(spell_mage_frostbolt::RecalculateDamage, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
    }

private:
    uint32 _earlyFrostSpellId = 0;
};

// 44614 - Frostfire Bolt: the Glyph of Frostfire trades the slow for a damage over time
class spell_mage_frostfire_bolt : public SpellScript
{
    PrepareSpellScript(spell_mage_frostfire_bolt);

    void HandleGlyphSlow(SpellEffIndex effIndex)
    {
        if (Aura* aura = GetHitAura())
            if (AuraEffect* effect = aura->GetEffect(effIndex))
            {
                if (GetCaster()->GetDummyAuraEffect(SPELLFAMILY_MAGE, ICON_MAGE_GLYPH_OF_FROSTFIRE, EFFECT_2))
                    effect->SetAmount(0);
                else
                    effect->SetAmount(GetSpellInfo()->Effects[effIndex].CalcValue());
            }
    }

    void HandleGlyphDot(SpellEffIndex effIndex)
    {
        if (!GetCaster()->GetDummyAuraEffect(SPELLFAMILY_MAGE, ICON_MAGE_GLYPH_OF_FROSTFIRE, EFFECT_2))
            if (Aura* aura = GetHitAura())
                if (AuraEffect* effect = aura->GetEffect(effIndex))
                    effect->SetAmount(0);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_frostfire_bolt::HandleGlyphSlow, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        OnEffectHitTarget += SpellEffectFn(spell_mage_frostfire_bolt::HandleGlyphDot, EFFECT_2, SPELL_EFFECT_APPLY_AURA);
    }
};

// 48108 - Hot Streak, 57761 - Brain Freeze (AzerothCore's): the T8 4P keeps the buff on its chance, the T10 2P gives
// haste when it is used
class spell_mage_gen_extra_effects : public AuraScript
{
    PrepareAuraScript(spell_mage_gen_extra_effects);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_T8_4P_BONUS, SPELL_MAGE_T10_2P_BONUS, SPELL_MAGE_T10_2P_BONUS_EFFECT });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (AuraEffect const* aurEff = eventInfo.GetActor()->GetAuraEffect(SPELL_MAGE_T8_4P_BONUS, EFFECT_0))
            if (roll_chance_i(aurEff->GetAmount()))
                return false;
        return true;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        Unit* caster = eventInfo.GetActor();
        if (caster->HasAura(SPELL_MAGE_T10_2P_BONUS))
            caster->CastSpell(caster, SPELL_MAGE_T10_2P_BONUS_EFFECT, true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_gen_extra_effects::CheckProc);
        OnProc += AuraProcFn(spell_mage_gen_extra_effects::HandleProc);
    }
};

// 44445 - Hot Streak: two crits in a row (Improved Hot Streak takes over), the T12 4P's extra chance
class spell_mage_hot_streak : public AuraScript
{
    PrepareAuraScript(spell_mage_hot_streak);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_HOT_STREAK_TRIGGERED, SPELL_MAGE_T12_4P_BONUS, SPELL_MAGE_IMPROVED_HOT_STREAK });
    }

    bool CheckProc(ProcEventInfo& /*eventInfo*/)
    {
        if (AuraEffect const* aurEff = GetTarget()->GetDummyAuraEffect(SPELLFAMILY_MAGE, ICON_MAGE_HOT_STREAK, EFFECT_0))
            if (aurEff->GetSpellInfo()->Id == SPELL_MAGE_IMPROVED_HOT_STREAK || aurEff->GetSpellInfo()->GetRank() > 1)
                return false;

        int32 procChance = GetSpellInfo()->Effects[EFFECT_0].CalcValue();
        if (AuraEffect const* aurEff = GetTarget()->GetAuraEffect(SPELL_MAGE_T12_4P_BONUS, EFFECT_1))
            procChance += aurEff->GetAmount();
        return roll_chance_i(procChance);
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(GetTarget(), SPELL_MAGE_HOT_STREAK_TRIGGERED, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_hot_streak::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mage_hot_streak::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 11426 - Ice Barrier: 87% of spell power; Shattered Barrier's freeze when it breaks
class spell_mage_ice_barrier : public AuraScript
{
    PrepareAuraScript(spell_mage_ice_barrier);

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;
        if (Unit* caster = GetCaster())
        {
            amount += int32(0.87f * caster->SpellBaseHealingBonusDone(GetSpellInfo()->GetSchoolMask()));
            // Glyph of Ice Barrier. Ours: not in CPP.
            if (AuraEffect const* glyph = caster->GetAuraEffect(63095, EFFECT_0))
                AddPct(amount, glyph->GetAmount());
        }
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_ENEMY_SPELL)
            return;

        if (GetTarget()->HasAura(SPELL_MAGE_SHATTERED_BARRIER_R1))
            GetTarget()->CastSpell(GetTarget(), SPELL_MAGE_SHATTERED_BARRIER_FREEZE_R1, true);
        else if (GetTarget()->HasAura(SPELL_MAGE_SHATTERED_BARRIER_R2))
            GetTarget()->CastSpell(GetTarget(), SPELL_MAGE_SHATTERED_BARRIER_FREEZE_R2, true);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_ice_barrier::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        AfterEffectRemove += AuraEffectRemoveFn(spell_mage_ice_barrier::AfterRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
    }
};

// 45438 - Ice Block: Hypothermia; the Glyph of Ice Block's Frost Nova reset
class spell_mage_ice_block : public AuraScript
{
    PrepareAuraScript(spell_mage_ice_block);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_HYPOTHERMIA, SPELL_MAGE_FROST_NOVA });
    }

    void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        target->CastSpell(target, SPELL_MAGE_HYPOTHERMIA, true);

        if (target->GetDummyAuraEffect(SPELLFAMILY_MAGE, ICON_MAGE_GLYPH_OF_ICE_BLOCK, EFFECT_0))
            if (Player* player = target->ToPlayer())
                player->RemoveSpellCooldown(SPELL_MAGE_FROST_NOVA, true);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_mage_ice_block::AfterApply, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }
};

// 12472 - Icy Veins: the Glyph of Icy Veins removes cast time and movement slows
class spell_mage_icy_veins : public AuraScript
{
    PrepareAuraScript(spell_mage_icy_veins);

    void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetTarget()->GetDummyAuraEffect(SPELLFAMILY_MAGE, ICON_MAGE_GLYPH_OF_ICY_VEINS, EFFECT_0))
        {
            GetTarget()->RemoveAurasByType(SPELL_AURA_HASTE_SPELLS, ObjectGuid::Empty, nullptr, true, false);
            GetTarget()->RemoveAurasByType(SPELL_AURA_MOD_DECREASE_SPEED);
        }
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_mage_icy_veins::AfterApply, EFFECT_0, SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }
};

// -11119 - Ignite: half the talent's share of the crit per tick, over 4 seconds (the orbs' damage does not ignite)
class spell_mage_ignite : public AuraScript
{
    PrepareAuraScript(spell_mage_ignite);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_IGNITE, SPELL_MAGE_FLAME_ORB_DAMAGE, SPELL_MAGE_FROSTFIRE_ORB_DAMAGE_R1, SPELL_MAGE_FROSTFIRE_ORB_DAMAGE_R2 });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetSpellInfo() || !eventInfo.GetProcTarget() || !eventInfo.GetDamageInfo())
            return false;

        switch (eventInfo.GetSpellInfo()->Id)
        {
            case SPELL_MAGE_FLAME_ORB_DAMAGE:
            case SPELL_MAGE_FROSTFIRE_ORB_DAMAGE_R1:
            case SPELL_MAGE_FROSTFIRE_ORB_DAMAGE_R2:
            case SPELL_MAGE_MOLTEN_ARMOR_TRIGGERED:
                return false;
            default:
                return true;
        }
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        int32 bp = int32(CalculatePct(eventInfo.GetDamageInfo()->GetDamage(), aurEff->GetAmount()) * 0.5f);
        if (bp > 0)
            GetTarget()->CastCustomSpell(eventInfo.GetProcTarget(), SPELL_MAGE_IGNITE, &bp, nullptr, nullptr, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_ignite::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mage_ignite::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 12654 - Ignite (damage over time): a new crit adds its two ticks to the ones left
class spell_mage_ignite_periodic : public AuraScript
{
    PrepareAuraScript(spell_mage_ignite_periodic);

    void CalculateRefreshedDot(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;
        _critDamageValues.emplace_back(2, amount);

        amount = 0;
        for (auto const& value : _critDamageValues)
            amount += value.second;
    }

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        // every crit is paid off: the tick a refresh rolled over has nothing left
        if (_critDamageValues.empty())
        {
            PreventDefaultAction();
            return;
        }

        int32 newDotValue = 0;
        bool changed = false;
        for (auto itr = _critDamageValues.begin(); itr != _critDamageValues.end();)
        {
            --itr->first;
            newDotValue += itr->second;
            if (itr->first == 0)
            {
                itr = _critDamageValues.erase(itr);
                changed = true;
            }
            else
                ++itr;
        }

        if (changed)
            const_cast<AuraEffect*>(aurEff)->SetAmount(newDotValue);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_ignite_periodic::CalculateRefreshedDot, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_ignite_periodic::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }

private:
    std::vector<std::pair<uint8, int32>> _critDamageValues;
};

// -11103 - Impact: Fire Blast's cooldown ends with the proc; Molten Armor's damage to attackers is no spell of the
// mage's (the client lets Impact proc from triggered spells, CPP and TDB 434 took it too)
class spell_mage_impact : public AuraScript
{
    PrepareAuraScript(spell_mage_impact);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        return !spellInfo || spellInfo->Id != SPELL_MAGE_MOLTEN_ARMOR_TRIGGERED;
    }

    void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
    {
        if (Player* player = GetTarget()->ToPlayer())
            player->RemoveSpellCooldown(SPELL_MAGE_FIRE_BLAST, true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_impact::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mage_impact::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 12355 - Impact (stun): spreads the mage's fire damage over time effects from the target to the others
class spell_mage_impact_triggered : public SpellScript
{
    PrepareSpellScript(spell_mage_impact_triggered);

    void RegisterFireDots(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetExplTargetUnit();
        if (!target || target != GetHitUnit())
            return;

        for (AuraEffect const* effect : target->GetAuraEffectsByType(SPELL_AURA_PERIODIC_DAMAGE))
            if (effect->GetCasterGUID() == caster->GetGUID() && effect->GetSpellInfo()->SpellFamilyFlags[2] & 0x00000008)
                _fireDots.push_back(effect->GetBase());
    }

    void SpreadFireDots(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (GetExplTargetUnit() == GetHitUnit())
            return;

        for (Aura* aura : _fireDots)
            if (Aura* addAura = caster->AddAura(aura->GetSpellInfo()->Id, GetHitUnit()))
            {
                for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                    if (AuraEffect* originalEffect = aura->GetEffect(i))
                        if (AuraEffect* effect = addAura->GetEffect(i))
                            effect->SetAmount(originalEffect->GetAmount());
                addAura->SetDuration(aura->GetDuration());
            }
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_mage_impact_triggered::RegisterFireDots, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
        OnEffectHitTarget += SpellEffectFn(spell_mage_impact_triggered::SpreadFireDots, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }

private:
    std::vector<Aura*> _fireDots;
};

// 37447, 61062 - Improved Mana Gems (AzerothCore's): a mana gem gives Mana Surge; 4.3.4's effect 1 is a dummy (chance
// in build_world_db.py)
class spell_mage_imp_mana_gems : public AuraScript
{
    PrepareAuraScript(spell_mage_imp_mana_gems);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_MANA_SURGE });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(GetTarget(), SPELL_MAGE_MANA_SURGE, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_mage_imp_mana_gems::HandleProc, EFFECT_1, SPELL_AURA_DUMMY);
    }
};

// -44446 - Improved Hot Streak: two direct damage crits in a row
class spell_mage_improved_hot_streak : public AuraScript
{
    PrepareAuraScript(spell_mage_improved_hot_streak);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_HOT_STREAK_TRIGGERED });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || damageInfo->GetDamageType() != SPELL_DIRECT_DAMAGE)
            return false;

        if (damageInfo->GetHitMask() & PROC_HIT_CRITICAL)
            ++_critCount;
        else
            _critCount = 0;

        if (_critCount == 2)
        {
            _critCount = 0;
            return roll_chance_i(GetSpellInfo()->Effects[EFFECT_0].CalcValue());
        }

        return false;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(GetTarget(), SPELL_MAGE_HOT_STREAK_TRIGGERED, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_improved_hot_streak::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mage_improved_hot_streak::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }

private:
    uint8 _critCount = 0;
};

// 58836 - Initialize Images: only the caster's own Mirror Images
class spell_mage_initialize_images : public SpellScript
{
    PrepareSpellScript(spell_mage_initialize_images);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        Unit* caster = GetCaster();
        targets.remove_if([caster](WorldObject* object)
        {
            Unit* unit = object->ToUnit();
            TempSummon* summon = unit ? unit->ToTempSummon() : nullptr;
            return summon && summon->GetSummonerUnit() && summon->GetSummonerUnit() != caster;
        });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_initialize_images::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// 44457 - Living Bomb: the explosion when it runs out
class spell_mage_living_bomb : public AuraScript
{
    PrepareAuraScript(spell_mage_living_bomb);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ uint32(spellInfo->Effects[EFFECT_1].CalcValue()) });
    }

    void AfterRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
            return;

        if (Unit* caster = GetCaster())
            caster->CastSpell(GetTarget(), uint32(aurEff->GetAmount()), true, nullptr, aurEff);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_mage_living_bomb::AfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 543 - Mage Ward: 80.7% of spell power; Incanter's Absorption
class spell_mage_mage_ward : public AuraScript
{
    PrepareAuraScript(spell_mage_mage_ward);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_INCANTERS_ABSORBTION_TRIGGERED });
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;
        if (Unit* caster = GetCaster())
            amount += int32(0.807f * caster->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_ARCANE));
    }

    void HandleAbsorb(AuraEffect* /*aurEff*/, DamageInfo& /*dmgInfo*/, uint32& absorbAmount)
    {
        if (AuraEffect const* aurEff = GetTarget()->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_GENERIC, ICON_MAGE_INCANTERS_ABSORPTION, EFFECT_0))
        {
            int32 bp = CalculatePct(absorbAmount, aurEff->GetAmount());
            GetTarget()->CastCustomSpell(GetTarget(), SPELL_MAGE_INCANTERS_ABSORBTION_TRIGGERED, &bp, nullptr, nullptr, true);
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_mage_ward::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        AfterEffectAbsorb += AuraEffectAbsorbFn(spell_mage_mage_ward::HandleAbsorb, EFFECT_0);
    }
};

// 1463 - Mana Shield: 80.7% of spell power; Incanter's Absorption and its knockback when the shield breaks
class spell_mage_mana_shield : public AuraScript
{
    PrepareAuraScript(spell_mage_mana_shield);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_INCANTERS_ABSORBTION_TRIGGERED, SPELL_MAGE_INCANTERS_ABSORBTION_KNOCKBACK });
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;
        if (Unit* caster = GetCaster())
            amount += int32(0.807f * caster->SpellBaseDamageBonusDone(GetSpellInfo()->GetSchoolMask()));
    }

    void HandleAbsorb(AuraEffect* /*aurEff*/, DamageInfo& /*dmgInfo*/, uint32& absorbAmount)
    {
        if (AuraEffect const* aurEff = GetTarget()->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_GENERIC, ICON_MAGE_INCANTERS_ABSORPTION, EFFECT_0))
        {
            int32 bp = CalculatePct(absorbAmount, aurEff->GetAmount());
            GetTarget()->CastCustomSpell(GetTarget(), SPELL_MAGE_INCANTERS_ABSORBTION_TRIGGERED, &bp, nullptr, nullptr, true);
        }
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_ENEMY_SPELL)
            if (GetTarget()->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_GENERIC, ICON_MAGE_INCANTERS_ABSORPTION, EFFECT_0))
                GetTarget()->CastSpell(GetTarget(), SPELL_MAGE_INCANTERS_ABSORBTION_KNOCKBACK, true);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_mana_shield::CalculateAmount, EFFECT_0, SPELL_AURA_MANA_SHIELD);
        AfterEffectManaShield += AuraEffectManaShieldFn(spell_mage_mana_shield::HandleAbsorb, EFFECT_0);
        AfterEffectRemove += AuraEffectRemoveFn(spell_mage_mana_shield::AfterRemove, EFFECT_0, SPELL_AURA_MANA_SHIELD, AURA_EFFECT_HANDLE_REAL);
    }
};

// -29074 - Master of Elements: a share of the spell's cost back
class spell_mage_master_of_elements : public AuraScript
{
    PrepareAuraScript(spell_mage_master_of_elements);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_MASTER_OF_ELEMENTS_ENERGIZE });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetDamageInfo() && eventInfo.GetDamageInfo()->GetSpellInfo();
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        int32 mana = CalculatePct(int32(spellInfo->CalcPowerCost(GetTarget(), spellInfo->GetSchoolMask())), aurEff->GetAmount());
        if (mana > 0)
            GetTarget()->CastCustomSpell(GetTarget(), SPELL_MAGE_MASTER_OF_ELEMENTS_ENERGIZE, &mana, nullptr, nullptr, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_master_of_elements::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mage_master_of_elements::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 55342 - Mirror Image: the Glyph of Mirror Image's fire or arcane images in those specializations
class spell_mage_mirror_image : public SpellScript
{
    PrepareSpellScript(spell_mage_mirror_image);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_MIRROR_IMAGE_TRIGGERED_FIRE, SPELL_MAGE_MIRROR_IMAGE_TRIGGERED_ARCANE, SPELL_MAGE_MIRROR_IMAGE_TRIGGERED_FROST });
    }

    void HandleDummyEffect(SpellEffIndex /*effIndex*/)
    {
        Player* player = GetHitPlayer();
        if (!player)
            return;

        uint32 spellId = SPELL_MAGE_MIRROR_IMAGE_TRIGGERED_FROST;
        if (player->GetDummyAuraEffect(SPELLFAMILY_MAGE, ICON_MAGE_GLYPH_OF_MIRROR_IMAGE, EFFECT_0))
        {
            uint32 tree = player->GetPrimaryTalentTree(player->GetActiveSpec());
            if (tree == TALENT_TREE_MAGE_FIRE)
                spellId = SPELL_MAGE_MIRROR_IMAGE_TRIGGERED_FIRE;
            else if (tree == TALENT_TREE_MAGE_ARCANE)
                spellId = SPELL_MAGE_MIRROR_IMAGE_TRIGGERED_ARCANE;
        }

        player->CastSpell(player, spellId, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_mirror_image::HandleDummyEffect, EFFECT_1, SPELL_EFFECT_DUMMY);
    }
};

// 55342 - Mirror Image (aura): Initialize Images on the first tick, once the images are out
class spell_mage_mirror_image_aura : public AuraScript
{
    PrepareAuraScript(spell_mage_mirror_image_aura);

    void HandleEffectPeriodic(AuraEffect const* aurEff)
    {
        if (aurEff->GetTickNumber() == 1)
            GetTarget()->CastSpell(GetTarget(), GetSpellInfo()->Effects[aurEff->GetEffIndex()].TriggerSpell, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_mirror_image_aura::HandleEffectPeriodic, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// -86181 - Nether Vortex: Slow on the target when no other has the mage's Slow
class spell_mage_nether_vortex : public AuraScript
{
    PrepareAuraScript(spell_mage_nether_vortex);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_SLOW });
    }

    bool CheckProc(ProcEventInfo& /*eventInfo*/)
    {
        for (Aura const* aura : GetTarget()->GetSingleCastAuras())
            if (aura->GetId() == SPELL_MAGE_SLOW)
                return false;
        return true;
    }

    void HandleEffectProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        if (Unit* target = eventInfo.GetProcTarget())
            GetTarget()->CastSpell(target, SPELL_MAGE_SLOW, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_nether_vortex::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mage_nether_vortex::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 79684 - Offensive State (DND): the passive that procs Arcane Missiles!, not with Hot Streak or Brain Freeze
class spell_mage_offensive_state_dnd : public AuraScript
{
    PrepareAuraScript(spell_mage_offensive_state_dnd);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Player* player = GetTarget()->ToPlayer();
        if (!player || !eventInfo.GetSpellInfo())
            return false;

        return player->HasSpell(SPELL_MAGE_ARCANE_MISSILES) && !player->HasAura(SPELL_MAGE_HOT_STREAK)
            && !player->GetAuraOfRankedSpell(SPELL_MAGE_BRAIN_FREEZE_R1);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_offensive_state_dnd::CheckProc);
    }
};

// -11175 - Permafrost: the Water Elemental is healed by a share of the mage's damage, the target heals less
class spell_mage_permafrost : public AuraScript
{
    PrepareAuraScript(spell_mage_permafrost);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_PERMAFROST_HEAL, SPELL_MAGE_PERMAFROST_REDUCE_HEAL });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return GetTarget()->GetGuardianPet() && eventInfo.GetDamageInfo() && eventInfo.GetDamageInfo()->GetDamage() && eventInfo.GetProcTarget();
    }

    void HandleEffectProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* target = GetTarget();
        int32 heal = int32(CalculatePct(eventInfo.GetDamageInfo()->GetDamage(), aurEff->GetAmount()));
        target->CastCustomSpell(SPELL_MAGE_PERMAFROST_HEAL, SPELLVALUE_BASE_POINT0, heal, target->GetGuardianPet(), true, nullptr, aurEff);
        target->CastSpell(eventInfo.GetProcTarget(), SPELL_MAGE_PERMAFROST_REDUCE_HEAL, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_permafrost::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mage_permafrost::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -83156 - Piercing Chill (ours; CPP has none): a Frostbolt crit gives $s1 more enemies near the target the chill
// (83154, the client's Piercing Chill slow; proc row in build_world_db.py)
class spell_mage_piercing_chill : public AuraScript
{
    PrepareAuraScript(spell_mage_piercing_chill);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_PIERCING_CHILL });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* caster = GetTarget();
        Unit* target = eventInfo.GetProcTarget();
        if (!target)
            return;

        std::list<Unit*> targets;
        Acore::AnyUnfriendlyUnitInObjectRangeCheck check(target, caster, 10.0f);
        Acore::UnitListSearcher<Acore::AnyUnfriendlyUnitInObjectRangeCheck> searcher(target, targets, check);
        Cell::VisitObjects(target, searcher, 10.0f);
        targets.remove_if([caster, target](Unit* unit) { return unit == target || !caster->IsValidAttackTarget(unit); });
        targets.sort(Acore::ObjectDistanceOrderPred(target));
        if (targets.size() > uint32(aurEff->GetAmount()))
            targets.resize(aurEff->GetAmount());

        for (Unit* unit : targets)
            caster->AddAura(SPELL_MAGE_PIERCING_CHILL, unit);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_mage_piercing_chill::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 118 - Polymorph: the Glyph of Polymorph removes damage over time effects; Improved Polymorph's stun when it breaks
class spell_mage_polymorph : public AuraScript
{
    PrepareAuraScript(spell_mage_polymorph);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_IMPROVED_POLYMORPH_RANK_1, SPELL_MAGE_IMPROVED_POLYMORPH_STUN_RANK_1, SPELL_MAGE_IMPROVED_POLYMORPH_MARKER });
    }

    void HandleGlyphEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        if (AuraEffect const* aurEff = caster->GetDummyAuraEffect(SPELLFAMILY_MAGE, ICON_MAGE_GLYPH_OF_POLYMORPH, EFFECT_0))
        {
            // Improved Polymorph and the Glyph of Polymorph are dummies with the same icon
            if (aurEff->GetSpellInfo()->Id == SPELL_MAGE_IMPROVED_POLYMORPH_RANK_1 || aurEff->GetSpellInfo()->GetRank() > 1)
                return;

            GetTarget()->RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
            GetTarget()->RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE_PERCENT);
            GetTarget()->RemoveAurasByType(SPELL_AURA_PERIODIC_LEECH);
        }
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetDamageInfo() || !eventInfo.GetDamageInfo()->GetDamage())
            return false;

        Unit* caster = GetCaster();
        return caster && !caster->HasAura(SPELL_MAGE_IMPROVED_POLYMORPH_MARKER);
    }

    void HandleEffectProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        Unit* caster = GetCaster();
        if (!caster)
            return;

        if (AuraEffect const* improvedPolymorph = caster->GetAuraEffectOfRankedSpell(SPELL_MAGE_IMPROVED_POLYMORPH_RANK_1, EFFECT_0))
        {
            GetTarget()->CastSpell(GetTarget(), sSpellMgr->GetSpellWithRank(SPELL_MAGE_IMPROVED_POLYMORPH_STUN_RANK_1, improvedPolymorph->GetSpellInfo()->GetRank()), true, nullptr, aurEff);
            caster->CastSpell(caster, SPELL_MAGE_IMPROVED_POLYMORPH_MARKER, true, nullptr, aurEff);
        }
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_mage_polymorph::HandleGlyphEffect, EFFECT_0, SPELL_AURA_MOD_CONFUSE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        DoCheckProc += AuraCheckProcFn(spell_mage_polymorph::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mage_polymorph::HandleEffectProc, EFFECT_0, SPELL_AURA_MOD_CONFUSE);
    }
};

// 32826 - Polymorph (Visual) (AzerothCore's): Aurosalia in Silvermoon turns into a random form
class spell_mage_polymorph_visual : public SpellScript
{
    PrepareSpellScript(spell_mage_polymorph_visual);

    static constexpr uint32 PolymorphForms[6] =
    {
        SPELL_MAGE_SQUIRREL_FORM, SPELL_MAGE_GIRAFFE_FORM, SPELL_MAGE_SERPENT_FORM,
        SPELL_MAGE_DRAGONHAWK_FORM, SPELL_MAGE_WORGEN_FORM, SPELL_MAGE_SHEEP_FORM
    };

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        for (uint32 spellId : PolymorphForms)
            if (!sSpellMgr->GetSpellInfo(spellId))
                return false;
        return true;
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Creature* target = GetCaster()->FindNearestCreature(NPC_AUROSALIA, 30.0f))
            target->CastSpell(target, PolymorphForms[urand(0, 5)], true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_polymorph_visual::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// -34293 - Pyromaniac: the haste buff while three or more targets burn with the mage's damage over time effects
class spell_mage_pyromaniac : public AuraScript
{
    PrepareAuraScript(spell_mage_pyromaniac);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_PYROMANIAC_TRIGGERED });
    }

    void CalcPeriodic(AuraEffect const* /*aurEff*/, bool& isPeriodic, int32& amplitude)
    {
        isPeriodic = true;
        amplitude = 1 * IN_MILLISECONDS;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        if (!eventInfo.GetProcTarget())
            return;

        _dotTargetGuids.insert(eventInfo.GetProcTarget()->GetGUID());
        if (_buffActive)
            return;

        Unit* target = GetTarget();
        CleanDotTargets(target);
        if (_dotTargetGuids.size() >= 3)
        {
            int32 bp = aurEff->GetAmount();
            target->CastCustomSpell(target, SPELL_MAGE_PYROMANIAC_TRIGGERED, &bp, nullptr, nullptr, true, nullptr, aurEff);
            _buffActive = true;
        }
    }

    void HandlePeriodic(AuraEffect const* /*aurEff*/)
    {
        if (!_buffActive)
            return;

        Unit* target = GetTarget();
        CleanDotTargets(target);
        if (_dotTargetGuids.size() < 3)
        {
            target->RemoveAurasDueToSpell(SPELL_MAGE_PYROMANIAC_TRIGGERED);
            _buffActive = false;
        }
    }

    void Register() override
    {
        DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_mage_pyromaniac::CalcPeriodic, EFFECT_0, SPELL_AURA_DUMMY);
        OnEffectProc += AuraEffectProcFn(spell_mage_pyromaniac::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_pyromaniac::HandlePeriodic, EFFECT_0, SPELL_AURA_DUMMY);
    }

private:
    void CleanDotTargets(Unit* caster)
    {
        GuidSet guids = _dotTargetGuids;
        for (ObjectGuid guid : guids)
            if (!IsValidDotTarget(caster, guid))
                _dotTargetGuids.erase(guid);
    }

    // a target that still has one of the mage's fire damage over time effects; any fire school, as the tooltip says
    // (CPP took only family flag 2 0x8, which leaves out Flamestrike)
    static bool IsValidDotTarget(Unit* caster, ObjectGuid guid)
    {
        Unit* target = ObjectAccessor::GetUnit(*caster, guid);
        if (!target)
            return false;

        for (AuraEffect const* effect : target->GetAuraEffectsByType(SPELL_AURA_PERIODIC_DAMAGE))
            if (effect->GetCasterGUID() == caster->GetGUID() && effect->GetSpellInfo()->GetSchoolMask() & SPELL_SCHOOL_MASK_FIRE)
                return true;
        return false;
    }

    GuidSet _dotTargetGuids;
    bool _buffActive = false;
};

// -86303 - Reactive Barrier (ours; its trigger spell 18350 is not in 4.3.4): a hit that takes the mage below half health
// casts Ice Barrier without its mana cost when it is off cooldown, and starts its cooldown
class spell_mage_reactive_barrier : public AuraScript
{
    PrepareAuraScript(spell_mage_reactive_barrier);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_ICE_BARRIER });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Player* player = GetTarget()->ToPlayer();
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!player || !player->IsAlive() || !damageInfo || !damageInfo->GetDamage())
            return false;

        if (!player->HasSpell(SPELL_MAGE_ICE_BARRIER) || player->HasSpellCooldown(SPELL_MAGE_ICE_BARRIER))
            return false;

        // the hit is already taken: below half now, at or above half before it
        uint32 half = player->CountPctFromMaxHealth(50);
        return player->GetHealth() < half && player->GetHealth() + damageInfo->GetDamage() >= half;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        Player* player = GetTarget()->ToPlayer();
        player->CastSpell(player, SPELL_MAGE_ICE_BARRIER, true, nullptr, aurEff);
        player->AddSpellAndCategoryCooldowns(sSpellMgr->AssertSpellInfo(SPELL_MAGE_ICE_BARRIER), 0);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_reactive_barrier::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mage_reactive_barrier::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 5405 - Replenish Mana (Mana Gem): Improved Mana Gem's spell power and mana
class spell_mage_replenish_mana : public SpellScript
{
    PrepareSpellScript(spell_mage_replenish_mana);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_IMPROVED_MANA_GEM_TRIGGERED });
    }

    void HandleImprovedManaGem()
    {
        Unit* caster = GetCaster();
        if (AuraEffect const* aurEff = caster->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_MAGE, ICON_MAGE_IMPROVED_MANA_GEM, EFFECT_0))
        {
            int32 bp = CalculatePct(caster->GetMaxPower(POWER_MANA), aurEff->GetAmount());
            caster->CastCustomSpell(caster, SPELL_MAGE_IMPROVED_MANA_GEM_TRIGGERED, &bp, &bp, nullptr, true);
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_mage_replenish_mana::HandleImprovedManaGem);
    }
};

// 82676 - Ring of Frost: the newest ring freezes enemies entering it once it is armed (91264's duration)
class spell_mage_ring_of_frost : public AuraScript
{
    PrepareAuraScript(spell_mage_ring_of_frost);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_RING_OF_FROST_FREEZE, SPELL_MAGE_RING_OF_FROST_DUMMY });
    }

    void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
    {
        TempSummon* ring = GetRing();
        if (ring && GetMaxDuration() - int32(ring->GetTimer()) >= sSpellMgr->AssertSpellInfo(SPELL_MAGE_RING_OF_FROST_DUMMY)->GetDuration())
            GetTarget()->CastSpell(ring->GetPositionX(), ring->GetPositionY(), ring->GetPositionZ(), SPELL_MAGE_RING_OF_FROST_FREEZE, true);
    }

    void Apply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        std::list<Creature*> minions;
        GetTarget()->GetAllMinionsByEntry(minions, GetSpellInfo()->Effects[EFFECT_0].MiscValue);

        // keep the last summoned ring, despawn the older ones
        TempSummon* ring = nullptr;
        for (Creature* creature : minions)
        {
            TempSummon* summon = creature->ToTempSummon();
            if (!summon)
                continue;

            if (!ring)
                ring = summon;
            else if (summon->GetTimer() > ring->GetTimer())
            {
                ring->DespawnOrUnsummon();
                ring = summon;
            }
            else
                summon->DespawnOrUnsummon();
        }

        _ringGUID = ring ? ring->GetGUID() : ObjectGuid::Empty;
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_ring_of_frost::HandleEffectPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        OnEffectApply += AuraEffectApplyFn(spell_mage_ring_of_frost::Apply, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }

private:
    TempSummon* GetRing() const
    {
        Creature* creature = ObjectAccessor::GetCreature(*GetTarget(), _ringGUID);
        return creature ? creature->ToTempSummon() : nullptr;
    }

    ObjectGuid _ringGUID;
};

// 82691 - Ring of Frost (freeze): enemies on the ring, not inside it, and not frozen by it in the last seconds
class spell_mage_ring_of_frost_freeze : public SpellScript
{
    PrepareSpellScript(spell_mage_ring_of_frost_freeze);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_RING_OF_FROST_SUMMON, SPELL_MAGE_RING_OF_FROST_DUMMY });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        WorldLocation const* dest = GetExplTargetDest();
        if (!dest)
            return;

        float outRadius = sSpellMgr->AssertSpellInfo(SPELL_MAGE_RING_OF_FROST_SUMMON)->Effects[EFFECT_0].CalcRadius();
        float inRadius = 4.7f;
        targets.remove_if([dest, outRadius, inRadius](WorldObject* object)
        {
            Unit* unit = object->ToUnit();
            if (!unit)
                return false;
            float dist = unit->GetExactDist(dest);
            return unit->HasAura(SPELL_MAGE_RING_OF_FROST_DUMMY) || unit->HasAura(SPELL_MAGE_RING_OF_FROST_FREEZE) || dist > outRadius || dist < inRadius;
        });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_ring_of_frost_freeze::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

// 82691 - Ring of Frost (freeze, aura): a freeze broken early still keeps the target from being frozen again at once
class spell_mage_ring_of_frost_freeze_aura : public AuraScript
{
    PrepareAuraScript(spell_mage_ring_of_frost_freeze_aura);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_RING_OF_FROST_DUMMY });
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
            if (Unit* caster = GetCaster())
                caster->CastSpell(GetTarget(), SPELL_MAGE_RING_OF_FROST_DUMMY, true);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_mage_ring_of_frost_freeze_aura::OnRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
    }
};

// 80353 - Time Warp: not on targets with Temporal Displacement, Insanity, Exhaustion or Sated
class spell_mage_time_warp : public SpellScript
{
    PrepareSpellScript(spell_mage_time_warp);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_TEMPORAL_DISPLACEMENT, SPELL_HUNTER_INSANITY, SPELL_SHAMAN_EXHAUSTION, SPELL_SHAMAN_SATED });
    }

    void RemoveInvalidTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(Acore::UnitAuraCheck(true, SPELL_MAGE_TEMPORAL_DISPLACEMENT));
        targets.remove_if(Acore::UnitAuraCheck(true, SPELL_HUNTER_INSANITY));
        targets.remove_if(Acore::UnitAuraCheck(true, SPELL_SHAMAN_EXHAUSTION));
        targets.remove_if(Acore::UnitAuraCheck(true, SPELL_SHAMAN_SATED));
    }

    void ApplyDebuff()
    {
        if (Unit* target = GetHitUnit())
            target->CastSpell(target, SPELL_MAGE_TEMPORAL_DISPLACEMENT, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_time_warp::RemoveInvalidTargets, EFFECT_ALL, TARGET_UNIT_CASTER_AREA_RAID);
        AfterHit += SpellHitFn(spell_mage_time_warp::ApplyDebuff);
    }
};

// 33395 - Freeze (Water Elemental): Improved Freeze's two Fingers of Frost charges when it hits
class spell_mage_water_elemental_freeze : public SpellScript
{
    PrepareSpellScript(spell_mage_water_elemental_freeze);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_FINGERS_OF_FROST });
    }

    void CountTargets(std::list<WorldObject*>& targets)
    {
        _didHit = !targets.empty();
    }

    void HandleImprovedFreeze()
    {
        Unit* owner = _didHit ? GetCaster()->GetOwner() : nullptr;
        if (!owner)
            return;

        if (AuraEffect const* aurEff = owner->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_MAGE, ICON_MAGE_IMPROVED_FREEZE, EFFECT_0))
            if (roll_chance_i(aurEff->GetAmount()))
                owner->CastCustomSpell(SPELL_MAGE_FINGERS_OF_FROST, SPELLVALUE_AURA_STACK, 2, owner, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_water_elemental_freeze::CountTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
        AfterCast += SpellCastFn(spell_mage_water_elemental_freeze::HandleImprovedFreeze);
    }

private:
    bool _didHit = false;
};

// 89764 - Mage Pet Scaling 05 (ours; the Water Elemental learns it from its family's skill line, CPP had no script,
// so its client amounts of 0 applied): the mage's frost spell crit and spell haste; the spell penetration effect
// stays 0, pets already take their owner's in Unit::CalcAbsorbResist
class spell_mage_pet_scaling_05 : public AuraScript
{
    PrepareAuraScript(spell_mage_pet_scaling_05);

    Player* GetMageOwner() const
    {
        Unit* owner = GetUnitOwner()->GetOwner();
        return owner ? owner->ToPlayer() : nullptr;
    }

    void CalculateCritAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = true;
        if (Player* owner = GetMageOwner())
            amount = int32(owner->GetFloatValue(PLAYER_SPELL_CRIT_PERCENTAGE1 + SPELL_SCHOOL_FROST));
    }

    void CalculateHasteAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = true;
        if (Player* owner = GetMageOwner())
            if (float castSpeed = owner->GetFloatValue(UNIT_MOD_CAST_SPEED))
                amount = int32((1.0f / castSpeed - 1.0f) * 100.0f);
    }

    void CalcPeriodic(AuraEffect const* /*aurEff*/, bool& isPeriodic, int32& amplitude)
    {
        isPeriodic = true;
        amplitude = 2 * IN_MILLISECONDS;
    }

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        PreventDefaultAction();
        if (AuraEffect* effect = aurEff->GetBase()->GetEffect(aurEff->GetEffIndex()))
            effect->RecalculateAmount();
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_pet_scaling_05::CalculateCritAmount, EFFECT_0, SPELL_AURA_MOD_CRIT_PCT);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_pet_scaling_05::CalculateHasteAmount, EFFECT_1, SPELL_AURA_MELEE_SLOW);
        DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_mage_pet_scaling_05::CalcPeriodic, EFFECT_ALL, SPELL_AURA_ANY);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_pet_scaling_05::HandlePeriodic, EFFECT_ALL, SPELL_AURA_ANY);
    }
};

// 122 - Frost Nova: its one charge goes with the first hit. The Glyph of Frost Nova lets the target take its share of its maximum
// health first: the data has no base amount for "additional", so that share is the whole allowance. Ours: not in CPP.
class spell_mage_frost_nova : public AuraScript
{
    PrepareAuraScript(spell_mage_frost_nova);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Unit* caster = GetCaster();
        DamageInfo* damage = eventInfo.GetDamageInfo();
        AuraEffect const* glyph = caster ? caster->GetAuraEffect(56376, EFFECT_0) : nullptr;
        if (!glyph || !damage)
            return true;

        _taken += damage->GetDamage();
        return _taken > GetTarget()->CountPctFromMaxHealth(glyph->GetAmount());
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_frost_nova::CheckProc);
    }

private:
    uint32 _taken = 0;
};

void AddSC_mage_spell_scripts()
{
    RegisterSpellScript(spell_mage_frost_nova);
    RegisterSpellScript(spell_mage_arcane_blast);
    RegisterSpellScript(spell_mage_arcane_missiles_trigger);
    RegisterSpellScript(spell_mage_arcane_potency);
    RegisterSpellScript(spell_mage_blast_wave);
    RegisterSpellScript(spell_mage_blazing_speed);
    RegisterSpellScript(spell_mage_blizzard);
    RegisterSpellScript(spell_mage_cauterize);
    RegisterSpellScript(spell_mage_cold_snap);
    RegisterSpellScript(spell_mage_combustion);
    RegisterSpellScript(spell_mage_cone_of_cold);
    RegisterSpellScript(spell_mage_conjure_refreshment);
    RegisterSpellScript(spell_mage_deep_freeze);
    RegisterSpellScript(spell_mage_dragon_breath);
    RegisterSpellScript(spell_mage_early_frost);
    RegisterSpellScript(spell_mage_flame_orb);
    RegisterSpellScript(spell_mage_flame_orb_aoe_dummy);
    RegisterSpellScript(spell_mage_focus_magic);
    RegisterSpellScript(spell_mage_frostbolt);
    RegisterSpellScript(spell_mage_frostfire_bolt);
    RegisterSpellScript(spell_mage_gen_extra_effects);
    RegisterSpellScript(spell_mage_hot_streak);
    RegisterSpellScript(spell_mage_ice_barrier);
    RegisterSpellScript(spell_mage_ice_block);
    RegisterSpellScript(spell_mage_icy_veins);
    RegisterSpellScript(spell_mage_ignite);
    RegisterSpellScript(spell_mage_ignite_periodic);
    RegisterSpellScript(spell_mage_impact);
    RegisterSpellScript(spell_mage_impact_triggered);
    RegisterSpellScript(spell_mage_imp_mana_gems);
    RegisterSpellScript(spell_mage_improved_hot_streak);
    RegisterSpellScript(spell_mage_initialize_images);
    RegisterSpellScript(spell_mage_living_bomb);
    RegisterSpellScript(spell_mage_mage_ward);
    RegisterSpellScript(spell_mage_mana_shield);
    RegisterSpellScript(spell_mage_master_of_elements);
    RegisterSpellAndAuraScriptPair(spell_mage_mirror_image, spell_mage_mirror_image_aura);
    RegisterSpellScript(spell_mage_nether_vortex);
    RegisterSpellScript(spell_mage_offensive_state_dnd);
    RegisterSpellScript(spell_mage_permafrost);
    RegisterSpellScript(spell_mage_piercing_chill);
    RegisterSpellScript(spell_mage_polymorph);
    RegisterSpellScript(spell_mage_polymorph_visual);
    RegisterSpellScript(spell_mage_pyromaniac);
    RegisterSpellScript(spell_mage_reactive_barrier);
    RegisterSpellScript(spell_mage_replenish_mana);
    RegisterSpellScript(spell_mage_ring_of_frost);
    RegisterSpellAndAuraScriptPair(spell_mage_ring_of_frost_freeze, spell_mage_ring_of_frost_freeze_aura);
    RegisterSpellScript(spell_mage_time_warp);
    RegisterSpellScript(spell_mage_water_elemental_freeze);
    RegisterSpellScript(spell_mage_pet_scaling_05);
}
