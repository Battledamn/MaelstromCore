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
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "Item.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "SpellScriptLoader.h"
#include <cmath>
/*
 * Scripts for spells with SPELLFAMILY_ROGUE and SPELLFAMILY_GENERIC spells used by rogue players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_rog_".
 *
 * 4.3.4: ported from The Cataclysm Preservation Project (spell_rogue.cpp at b061f46), rewritten against
 * AzerothCore; the AzerothCore scripts that are still right for 4.3.4 are kept (docs/CLASS_SCRIPTS.md).
 */

enum RogueSpells
{
    SPELL_ROGUE_ADRENALINE_RUSH                     = 13750,
    SPELL_ROGUE_BANDITS_GUILE                       = 84748,
    SPELL_ROGUE_BANDITS_GUILE_TALENT_R1             = 84652,
    SPELL_ROGUE_BLADE_FLURRY                        = 13877,
    SPELL_ROGUE_BLADE_FLURRY_EXTRA_ATTACK           = 22482,
    SPELL_ROGUE_CHEAT_DEATH_COOLDOWN                = 31231,
    SPELL_ROGUE_CHEATING_DEATH                      = 45182,
    SPELL_ROGUE_COLD_BLOOD                          = 14177,
    SPELL_ROGUE_CRIPPLING_POISON                    = 3409,
    SPELL_ROGUE_DEEP_INSIGHT                        = 84747,
    SPELL_ROGUE_EVISCERATE_AND_ENVENOM_BONUS_DAMAGE = 37169,
    SPELL_ROGUE_EXPOSE_ARMOR                        = 8647,
    SPELL_ROGUE_GLYPH_OF_PREPARATION                = 56819,
    SPELL_ROGUE_GLYPH_HEMORRHAGE_TRIGGERED          = 89775,
    SPELL_ROGUE_HONOR_AMONG_THIEVES_TRIGGERED       = 51699,
    SPELL_ROGUE_IMPROVED_EXPOSE_ARMOR               = 79128,
    SPELL_ROGUE_KILLING_SPREE                       = 51690,
    SPELL_ROGUE_KILLING_SPREE_TELEPORT              = 57840,
    SPELL_ROGUE_KILLING_SPREE_WEAPON_DMG            = 57841,
    SPELL_ROGUE_KILLING_SPREE_DMG_BUFF              = 61851,
    SPELL_ROGUE_MAIN_GAUCHE                         = 86392,
    SPELL_ROGUE_MASTER_OF_SUBTLETY_DAMAGE_PERCENT   = 31665,
    SPELL_ROGUE_MASTER_OF_SUBTLETY_PASSIVE          = 31223,
    SPELL_ROGUE_MASTER_OF_SUBTLETY_PERIODIC         = 31666,
    SPELL_ROGUE_MASTER_POISONER                     = 58410,
    SPELL_ROGUE_MODERATE_INSIGHT                    = 84746,
    SPELL_ROGUE_MURDEROUS_INTENT                    = 79132,
    SPELL_ROGUE_OVERKILL_TALENT                     = 58426,
    SPELL_ROGUE_OVERKILL_PERIODIC                   = 58428,
    SPELL_ROGUE_OVERKILL_POWER_REGEN                = 58427,
    SPELL_ROGUE_PREY_ON_THE_WEAK                    = 58670,
    SPELL_ROGUE_REDIRECT                            = 73981,
    SPELL_ROGUE_RELENTLESS_STRIKES_ENERGIZE         = 98440,
    SPELL_ROGUE_REMORSELESS_ATTACKS_R1              = 14143,
    SPELL_ROGUE_REVEALING_STRIKE                    = 84617,
    SPELL_ROGUE_RUTHLESSNESS_COMBO_POINT            = 14157,
    SPELL_ROGUE_SHALLOW_INSIGHT                     = 84745,
    SPELL_ROGUE_SHIV_TRIGGERED                      = 5940,
    SPELL_ROGUE_SLICE_AND_DICE                      = 5171,
    SPELL_ROGUE_SPRINT                              = 2983,
    SPELL_ROGUE_STEALTH                             = 1784,
    SPELL_ROGUE_TRICKS_OF_THE_TRADE                 = 57934,
    SPELL_ROGUE_TRICKS_OF_THE_TRADE_DMG_BOOST       = 57933,
    SPELL_ROGUE_TRICKS_OF_THE_TRADE_PROC            = 59628,
    SPELL_ROGUE_VENOMOUS_VIM                        = 51637,
    SPELL_ROGUE_VENOMOUS_WOUND                      = 79136,
    SPELL_ROGUE_BLACKJACK_R1                        = 79123,
    SPELL_RACIAL_ELUSIVENESS                        = 21009
};

enum RogueSpellIcons
{
    ROGUE_ICON_ID_IMPROVED_RECUPERATE               = 4819,
    ROGUE_ICON_ID_SERRATED_BLADES                   = 2004,
    ROGUE_ICON_ID_SANGUINARY_VEIN                   = 4821,
    ROGUE_ICON_ID_VENOMOUS_WOUNDS                   = 4888
};

// 4.3.4 family flags (CPP SpellMgr.h)
enum RogueSpellFamilyFlags
{
    ROGUE_FLAG0_VANISH_SPRINT                       = 0x00000840,
    ROGUE_FLAG0_KICK                                = 0x00000010,
    ROGUE_FLAG1_SHADOWSTEP                          = 0x00000200,
    ROGUE_FLAG1_DISMANTLE_SMOKE_BOMB                = 0x80100000
};

// -84652 - Bandit's Guile
// Strikes on the same target build Shallow, Moderate and then Deep Insight; a new target starts over.
class spell_rog_bandits_guile : public AuraScript
{
    PrepareAuraScript(spell_rog_bandits_guile);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ROGUE_SHALLOW_INSIGHT, SPELL_ROGUE_MODERATE_INSIGHT, SPELL_ROGUE_DEEP_INSIGHT, SPELL_ROGUE_BANDITS_GUILE });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (GetTarget()->HasAura(SPELL_ROGUE_DEEP_INSIGHT, GetTarget()->GetGUID()))
            return false;

        return eventInfo.GetProcTarget();
    }

    void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* target = GetTarget();
        Unit* procTarget = eventInfo.GetProcTarget();
        uint32 spellId = SPELL_ROGUE_SHALLOW_INSIGHT;
        int32 basepoints = 10;

        // CPP: four successful strikes per step
        uint8 const neededProcs = 4;

        // a new opponent: the insight starts over
        if (_recentTargetGUID != procTarget->GetGUID())
        {
            target->RemoveAurasDueToSpell(SPELL_ROGUE_SHALLOW_INSIGHT, target->GetGUID());
            target->RemoveAurasDueToSpell(SPELL_ROGUE_MODERATE_INSIGHT, target->GetGUID());
            target->RemoveAurasDueToSpell(SPELL_ROGUE_DEEP_INSIGHT, target->GetGUID());
            _recentTargetGUID = procTarget->GetGUID();
            _procStrikes = 0;
        }

        _procStrikes = std::min<uint8>(_procStrikes + 1, neededProcs * 3);
        if (_procStrikes < neededProcs)
            return;

        if (_procStrikes >= neededProcs * 3)
        {
            spellId = SPELL_ROGUE_DEEP_INSIGHT;
            basepoints = 30;
            _procStrikes = 0;
        }
        else if (_procStrikes >= neededProcs * 2)
        {
            spellId = SPELL_ROGUE_MODERATE_INSIGHT;
            basepoints = 20;
        }

        if (spellId == SPELL_ROGUE_DEEP_INSIGHT)
            target->RemoveAurasDueToSpell(SPELL_ROGUE_MODERATE_INSIGHT, target->GetGUID());
        else if (spellId == SPELL_ROGUE_MODERATE_INSIGHT)
            target->RemoveAurasDueToSpell(SPELL_ROGUE_SHALLOW_INSIGHT, target->GetGUID());

        target->CastSpell(target, spellId, true);
        target->CastCustomSpell(procTarget, SPELL_ROGUE_BANDITS_GUILE, &basepoints, &basepoints, nullptr, true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rog_bandits_guile::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_rog_bandits_guile::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }

public:
    // Redirect: "as well as any insight gained from Bandit's Guile" - the target's debuff and the progress move
    void TransferInsight(Unit* newTarget)
    {
        // called from Redirect's script, outside this aura's hooks: GetTarget() is only valid inside them
        Unit* rogue = GetUnitOwner();
        if (!newTarget || _recentTargetGUID == newTarget->GetGUID())
            return;

        if (Unit* oldTarget = ObjectAccessor::GetUnit(*rogue, _recentTargetGUID))
            if (Aura* debuff = oldTarget->GetAura(SPELL_ROGUE_BANDITS_GUILE, rogue->GetGUID()))
            {
                int32 bp0 = debuff->GetEffect(EFFECT_0) ? debuff->GetEffect(EFFECT_0)->GetAmount() : 0;
                int32 bp1 = debuff->GetEffect(EFFECT_1) ? debuff->GetEffect(EFFECT_1)->GetAmount() : bp0;
                int32 const duration = debuff->GetDuration();
                debuff->Remove();
                rogue->CastCustomSpell(newTarget, SPELL_ROGUE_BANDITS_GUILE, &bp0, &bp1, nullptr, true);
                if (Aura* moved = newTarget->GetAura(SPELL_ROGUE_BANDITS_GUILE, rogue->GetGUID()))
                    moved->SetDuration(duration);
            }

        _recentTargetGUID = newTarget->GetGUID();
    }

private:
    ObjectGuid _recentTargetGUID;
    uint8 _procStrikes = 0;
};

// 13877, 33735, (check 51211, 65956) - Blade Flurry
class spell_rog_blade_flurry : public AuraScript
{
    PrepareAuraScript(spell_rog_blade_flurry);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ROGUE_BLADE_FLURRY_EXTRA_ATTACK });
    }

    bool Load() override
    {
        _procTargetGUID.Clear();
        return true;
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetActor())
            return false;

        Unit* _procTarget = eventInfo.GetActor()->SelectNearbyNoTotemTarget(eventInfo.GetProcTarget());
        if (_procTarget)
            _procTargetGUID = _procTarget->GetGUID();
        return _procTarget;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        // Xinef: no _procTarget but checkproc passed??
        // Unit::CalculateAOEDamageReduction (this=0x0, damage=4118, schoolMask=1, caster=0x7ffdad089000)
        Unit* procTarget = ObjectAccessor::GetUnit(*GetTarget(), _procTargetGUID);
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (procTarget && damageInfo)
        {
            int32 damage = damageInfo->GetUnmitigatedDamage();

            CustomSpellValues values;
            values.AddSpellMod(SPELLVALUE_BASE_POINT0, damage);
            values.AddSpellMod(SPELLVALUE_FORCED_CRIT_RESULT, int32(eventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT));
            GetTarget()->CastCustomSpell(SPELL_ROGUE_BLADE_FLURRY_EXTRA_ATTACK, values, procTarget, TRIGGERED_FULL_MASK, nullptr, aurEff);
        }
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rog_blade_flurry::CheckProc);
        // 4.3.4: the rogue's Blade Flurry lowers energy regeneration; the creature versions keep their haste
        if (m_scriptSpellId == SPELL_ROGUE_BLADE_FLURRY)
            OnEffectProc += AuraEffectProcFn(spell_rog_blade_flurry::HandleProc, EFFECT_0, SPELL_AURA_MOD_POWER_REGEN_PERCENT);
        else
            OnEffectProc += AuraEffectProcFn(spell_rog_blade_flurry::HandleProc, EFFECT_0, SPELL_AURA_MOD_MELEE_HASTE);
    }

private:
    ObjectGuid _procTargetGUID;
};

// -31228 - Cheat Death
class spell_rog_cheat_death : public AuraScript
{
    PrepareAuraScript(spell_rog_cheat_death);

    uint32 absorbChance;

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ROGUE_CHEAT_DEATH_COOLDOWN, SPELL_ROGUE_CHEATING_DEATH });
    }

    bool Load() override
    {
        absorbChance = GetSpellInfo()->Effects[EFFECT_0].CalcValue();
        return GetUnitOwner()->ToPlayer();
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        // Set absorbtion amount to unlimited
        amount = -1;
    }

    void Absorb(AuraEffect* /*aurEff*/, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        Player* target = GetTarget()->ToPlayer();
        if (dmgInfo.GetDamage() < target->GetHealth() || target->HasSpellCooldown(SPELL_ROGUE_CHEAT_DEATH_COOLDOWN) || !roll_chance_i(absorbChance))
            return;

        // Cheating Death: 80% less damage taken for its duration (4.3.4 tooltip); once per 90 seconds
        target->CastSpell(target, SPELL_ROGUE_CHEAT_DEATH_COOLDOWN, true);
        target->CastSpell(target, SPELL_ROGUE_CHEATING_DEATH, true);
        target->AddSpellCooldown(SPELL_ROGUE_CHEAT_DEATH_COOLDOWN, 0, 90 * IN_MILLISECONDS);

        uint32 health10 = target->CountPctFromMaxHealth(10);

        // hp > 10% - absorb hp till 10%
        if (target->GetHealth() > health10)
            absorbAmount = dmgInfo.GetDamage() - target->GetHealth() + health10;
        // hp lower than 10% - absorb everything
        else
            absorbAmount = dmgInfo.GetDamage();
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_cheat_death::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_rog_cheat_death::Absorb, EFFECT_0);
    }
};

// 14177 - Cold Blood
class spell_rog_cold_blood : public AuraScript
{
    PrepareAuraScript(spell_rog_cold_blood);

    bool _usedByMutilate = false;

public:
    bool WasUsedByMutilate() const
    {
        return _usedByMutilate;
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo)
            return true;

        // Block Mutilate MH (0x2) and OH (0x4) from consuming the charge
        if (spellInfo->SpellFamilyName == SPELLFAMILY_ROGUE
            && (spellInfo->SpellFamilyFlags[1] & 0x6))
        {
            _usedByMutilate = true;
            return false;
        }

        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rog_cold_blood::CheckProc);
    }
};

// -35541 - Combat Potency
// Off-hand auto attacks and Main Gauche (CPP).
class spell_rog_combat_potency : public AuraScript
{
    PrepareAuraScript(spell_rog_combat_potency);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (SpellInfo const* spellInfo = eventInfo.GetSpellInfo())
            return spellInfo->SpellFamilyFlags[2] == 0x00000002;
        if (DamageInfo* damageInfo = eventInfo.GetDamageInfo())
            return damageInfo->GetAttackType() == OFF_ATTACK;
        return false;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rog_combat_potency::CheckProc);
    }
};

// -51625 - Deadly Brew
class spell_rog_crippling_poison : public AuraScript
{
    PrepareAuraScript(spell_rog_crippling_poison);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ROGUE_CRIPPLING_POISON });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        if (Unit* target = eventInfo.GetProcTarget())
            GetTarget()->CastSpell(target, SPELL_ROGUE_CRIPPLING_POISON, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_rog_crippling_poison::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -51664 - Cut to the Chase
// Eviscerate and Envenom refresh Slice and Dice to its 5 combo point duration.
class spell_rog_cut_to_the_chase : public AuraScript
{
    PrepareAuraScript(spell_rog_cut_to_the_chase);

    void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        if (Aura* sliceAndDice = GetTarget()->GetAura(SPELL_ROGUE_SLICE_AND_DICE))
            sliceAndDice->SetDuration(sliceAndDice->GetSpellInfo()->GetMaxDuration(), true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_rog_cut_to_the_chase::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 5171 - Slice and Dice
// Remembers the duration it was cast with, for Deadly Momentum.
class spell_rog_slice_and_dice : public AuraScript
{
    PrepareAuraScript(spell_rog_slice_and_dice);

    void HandleEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        Duration = aurEff->GetBase()->GetDuration();
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_rog_slice_and_dice::HandleEffectApply, EFFECT_0, SPELL_AURA_MOD_MELEE_HASTE_3, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }

public:
    int32 Duration = 0;
};

// 73651 - Recuperate
// $s1% of maximum health per tick (Improved Recuperate adds its share).
class spell_rog_recuperate : public AuraScript
{
    PrepareAuraScript(spell_rog_recuperate);

    bool Load() override
    {
        return GetCaster() && GetCaster()->IsPlayer();
    }

    void OnPeriodic(AuraEffect const* /*aurEff*/)
    {
        // follows the rogue's maximum health
        if (Unit* caster = GetCaster())
            if (AuraEffect* effect = GetAura()->GetEffect(EFFECT_0))
                effect->RecalculateAmount(caster);
    }

    void CalculateBonus(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = true;
        if (Unit* caster = GetCaster())
        {
            int32 baseAmount = GetSpellInfo()->Effects[EFFECT_0].CalcValue(caster) * 1000;
            if (AuraEffect const* improved = caster->GetDummyAuraEffect(SPELLFAMILY_ROGUE, ROGUE_ICON_ID_IMPROVED_RECUPERATE, EFFECT_0))
                baseAmount += improved->GetAmount();

            amount = int32(CalculatePct(caster->GetMaxHealth(), float(baseAmount) / 1000.0f));
        }
    }

    void HandleEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        Duration = aurEff->GetBase()->GetDuration();
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_rog_recuperate::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_recuperate::CalculateBonus, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
        AfterEffectApply += AuraEffectApplyFn(spell_rog_recuperate::HandleEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }

public:
    int32 Duration = 0;
};

// -79121 - Deadly Momentum
// Critical strikes refresh Slice and Dice and Recuperate to the duration they were cast with.
class spell_rog_deadly_momentum : public AuraScript
{
    PrepareAuraScript(spell_rog_deadly_momentum);

    void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
    {
        Unit* target = GetTarget();

        if (AuraEffect* slice = target->GetAuraEffect(SPELL_AURA_MOD_MELEE_HASTE_3, SPELLFAMILY_ROGUE, 0x00040000, 0, 0))
            if (spell_rog_slice_and_dice* script = slice->GetBase()->GetScript<spell_rog_slice_and_dice>("spell_rog_slice_and_dice"))
                slice->GetBase()->SetDuration(script->Duration);

        if (AuraEffect* recuperate = target->GetAuraEffect(SPELL_AURA_PERIODIC_HEAL, SPELLFAMILY_ROGUE, 0, 0x08000000, 0))
            if (spell_rog_recuperate* script = recuperate->GetBase()->GetScript<spell_rog_recuperate>("spell_rog_recuperate"))
            {
                recuperate->GetBase()->SetDuration(script->Duration);
                recuperate->ResetTicks();
            }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_rog_deadly_momentum::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL_WITH_VALUE);
    }
};

// 2818 - Deadly Poison
class spell_rog_deadly_poison : public SpellScript
{
    PrepareSpellScript(spell_rog_deadly_poison);

    bool Load() override
    {
        _stackAmount = 0;
        // at this point CastItem must already be initialized
        return GetCaster()->IsPlayer() && GetCastItem();
    }

    void HandleBeforeHit(SpellMissInfo missInfo)
    {
        if (missInfo != SPELL_MISS_NONE)
        {
            return;
        }

        if (Unit* target = GetHitUnit())
            // Deadly Poison
            if (AuraEffect const* aurEff = target->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_ROGUE, 0x10000, 0x80000, 0, GetCaster()->GetGUID()))
                _stackAmount = aurEff->GetBase()->GetStackAmount();
    }

    void HandleAfterHit()
    {
        if (_stackAmount < 5)
            return;

        Player* player = GetCaster()->ToPlayer();

        if (Unit* target = GetHitUnit())
        {
            Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);

            if (item == GetCastItem())
                item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);

            if (!item)
                return;

            // item combat enchantments
            for (uint8 slot = 0; slot < MAX_ENCHANTMENT_SLOT; ++slot)
            {
                SpellItemEnchantmentEntry const* enchant = sSpellItemEnchantmentStore.LookupEntry(item->GetEnchantmentId(EnchantmentSlot(slot)));
                if (!enchant)
                    continue;

                for (uint8 s = 0; s < 3; ++s)
                {
                    if (enchant->type[s] != ITEM_ENCHANTMENT_TYPE_COMBAT_SPELL)
                        continue;

                    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(enchant->spellid[s]);
                    if (!spellInfo)
                    {
                        LOG_ERROR("misc", "Player::CastItemCombatSpell Enchant {}, player (Name: {}, {}) cast unknown spell {}",
                            enchant->ID, player->GetName(), player->GetGUID().ToString(), enchant->spellid[s]);
                        continue;
                    }

                    // Proc only rogue poisons
                    if (spellInfo->SpellFamilyName != SPELLFAMILY_ROGUE || spellInfo->Dispel != DISPEL_POISON)
                        continue;

                    // Do not reproc deadly
                    if (spellInfo->SpellFamilyFlags.IsEqual(0x10000, 0x80000, 0))
                        continue;

                    if (spellInfo->IsPositive())
                        player->CastSpell(player, enchant->spellid[s], true, item);
                    else
                        player->CastSpell(target, enchant->spellid[s], true, item);
                }
            }
        }
    }

    void Register() override
    {
        BeforeHit += BeforeSpellHitFn(spell_rog_deadly_poison::HandleBeforeHit);
        AfterHit += SpellHitFn(spell_rog_deadly_poison::HandleAfterHit);
    }

    uint8 _stackAmount;
};

// 32645 - Envenom
// One Deadly Poison dose per combo point (not with Master Poisoner, 4.0.1), each adding the effect's damage, plus
// 9% of attack power per combo point; Revealing Strike's bonus is used up.
class spell_rog_envenom : public SpellScript
{
    PrepareSpellScript(spell_rog_envenom);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ROGUE_REVEALING_STRIKE, SPELL_ROGUE_MASTER_POISONER });
    }

    bool Load() override
    {
        return GetCaster()->IsPlayer() && GetCaster()->IsClass(CLASS_ROGUE);
    }

    void ChangeDamage(SpellEffIndex /*effIndex*/)
    {
        Player* player = GetCaster()->ToPlayer();
        Unit* target = GetHitUnit();
        if (!target)
            return;

        uint8 const comboPoints = player->GetComboPoints();
        if (!comboPoints)
            return;

        int32 damage = GetEffectValue();

        if (AuraEffect const* deadlyPoison = target->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_ROGUE, 0x00010000, 0, 0, player->GetGUID()))
        {
            uint8 const consumedStacks = std::min<uint8>(comboPoints, deadlyPoison->GetBase()->GetStackAmount());

            if (!player->HasSpell(SPELL_ROGUE_MASTER_POISONER))
                deadlyPoison->GetBase()->ModStackAmount(-int32(consumedStacks));

            damage *= consumedStacks;
            damage += int32(player->GetTotalAttackPowerValue(BASE_ATTACK) * 0.09f * comboPoints);
        }

        // Eviscerate and Envenom Bonus Damage (item set effect)
        if (AuraEffect const* setBonus = player->GetAuraEffect(SPELL_ROGUE_EVISCERATE_AND_ENVENOM_BONUS_DAMAGE, EFFECT_0))
            damage += comboPoints * setBonus->GetAmount();

        if (AuraEffect* revealingStrike = target->GetAuraEffect(SPELL_ROGUE_REVEALING_STRIKE, EFFECT_2, player->GetGUID()))
        {
            damage += CalculatePct(damage, revealingStrike->GetAmount());
            revealingStrike->GetBase()->Remove();
        }

        SetEffectValue(damage);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_rog_envenom::ChangeDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 2098 - Eviscerate
// 9.1% of attack power per combo point (the 4.3.4 tooltip), the set bonus, Revealing Strike's bonus; Serrated
// Blades refreshes Rupture with a chance per combo point.
class spell_rog_eviscerate : public SpellScript
{
    PrepareSpellScript(spell_rog_eviscerate);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ROGUE_REVEALING_STRIKE });
    }

    bool Load() override
    {
        return GetCaster()->IsPlayer() && GetCaster()->IsClass(CLASS_ROGUE);
    }

    void CalculateDamage(SpellEffIndex /*effIndex*/)
    {
        Player* player = GetCaster()->ToPlayer();
        uint8 const comboPoints = player->GetComboPoints();
        int32 damage = GetEffectValue() + int32(comboPoints * player->GetTotalAttackPowerValue(BASE_ATTACK) * 0.091f);

        // Eviscerate and Envenom Bonus Damage (item set effect)
        if (AuraEffect const* setBonus = player->GetAuraEffect(SPELL_ROGUE_EVISCERATE_AND_ENVENOM_BONUS_DAMAGE, EFFECT_0))
            damage += comboPoints * setBonus->GetAmount();

        SetEffectValue(damage);
    }

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        if (!target)
            return;

        if (AuraEffect const* revealingStrike = target->GetAuraEffect(SPELL_ROGUE_REVEALING_STRIKE, EFFECT_2, caster->GetGUID()))
        {
            int32 damage = GetHitDamage();
            AddPct(damage, revealingStrike->GetAmount());
            SetHitDamage(damage);
        }

        // Serrated Blades
        if (AuraEffect const* blades = caster->GetDummyAuraEffect(SPELLFAMILY_ROGUE, ROGUE_ICON_ID_SERRATED_BLADES, EFFECT_0))
            if (roll_chance_i(blades->GetAmount() * caster->GetComboPoints()))
                if (AuraEffect const* rupture = target->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_ROGUE, 0x00100000, 0, 0, caster->GetGUID()))
                    rupture->GetBase()->RefreshDuration();
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_rog_eviscerate::CalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnEffectHitTarget += SpellEffectFn(spell_rog_eviscerate::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 56807 - Glyph of Hemorrhage
// A bleed for $s1% of Hemorrhage's damage, spread over its ticks.
class spell_rog_glyph_of_hemorrhage : public AuraScript
{
    PrepareAuraScript(spell_rog_glyph_of_hemorrhage);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ROGUE_GLYPH_HEMORRHAGE_TRIGGERED });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetVictim())
            return;

        SpellInfo const* bleed = sSpellMgr->AssertSpellInfo(SPELL_ROGUE_GLYPH_HEMORRHAGE_TRIGGERED);
        if (uint32 ticks = bleed->GetMaxTicks())
        {
            int32 bp = CalculatePct(int32(damageInfo->GetDamage()), aurEff->GetAmount()) / int32(ticks);
            GetTarget()->CastCustomSpell(damageInfo->GetVictim(), SPELL_ROGUE_GLYPH_HEMORRHAGE_TRIGGERED, &bp, nullptr, nullptr, true, nullptr, aurEff);
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_rog_glyph_of_hemorrhage::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 1776 - Gouge
// The rogue's own bleeds may leave it standing (Sanguinary Vein).
class spell_rog_gouge : public AuraScript
{
    PrepareAuraScript(spell_rog_gouge);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (Unit* actor = eventInfo.GetActor())
            if (actor == GetCaster())
                if (SpellInfo const* spellInfo = eventInfo.GetSpellInfo())
                    if (spellInfo->SpellFamilyName == SPELLFAMILY_ROGUE && spellInfo->Mechanic == MECHANIC_BLEED)
                        if (AuraEffect const* vein = actor->GetDummyAuraEffect(SPELLFAMILY_ROGUE, ROGUE_ICON_ID_SANGUINARY_VEIN, EFFECT_1))
                            if (roll_chance_i(vein->GetAmount()))
                                return false;

        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rog_gouge::CheckProc);
    }
};

// 16511 - Hemorrhage
// ${$m2*1.45}% weapon damage with a dagger (4.3.4 tooltip).
class spell_rog_hemorrhage : public SpellScript
{
    PrepareSpellScript(spell_rog_hemorrhage);

    bool Load() override
    {
        return GetCaster()->IsPlayer();
    }

    void HandleDagger(SpellEffIndex /*effIndex*/)
    {
        Player* player = GetCaster()->ToPlayer();
        for (WeaponAttackType attType : { BASE_ATTACK, OFF_ATTACK })
            if (Item* item = player->GetWeaponForAttack(attType, true))
                if (item->GetTemplate()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER)
                {
                    int32 damage = GetHitDamage();
                    AddPct(damage, 45);
                    SetHitDamage(damage);
                    return;
                }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_rog_hemorrhage::HandleDagger, EFFECT_1, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
    }
};

// -51698 - Honor Among Thieves
// A party aura: a member's critical strike gives the rogue a combo point on the rogue's target.
class spell_rog_honor_among_thieves : public AuraScript
{
    PrepareAuraScript(spell_rog_honor_among_thieves);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ROGUE_HONOR_AMONG_THIEVES_TRIGGERED });
    }

    // "This effect cannot occur more than once every 4/3/2 seconds": the rogue's, not each group member's (every
    // member carries their own copy of the area aura, each with its own proc cooldown); kept on the rogue's copy
    Aura* GetRoguesAura() const
    {
        Unit* caster = GetCaster();
        return caster ? caster->GetAura(GetId(), caster->GetGUID()) : nullptr;
    }

    bool CheckProc(ProcEventInfo& /*eventInfo*/)
    {
        Aura* roguesAura = GetRoguesAura();
        return !roguesAura || !roguesAura->IsProcOnCooldown(std::chrono::steady_clock::now());
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();

        Unit* caster = GetCaster();
        if (!caster)
            return;

        if (Player* player = caster->ToPlayer())
            if (Unit* target = ObjectAccessor::GetUnit(*player, player->GetTarget()))
                caster->CastSpell(target, SPELL_ROGUE_HONOR_AMONG_THIEVES_TRIGGERED, true, nullptr, aurEff);

        if (Aura* roguesAura = GetRoguesAura())
            if (SpellProcEntry const* procEntry = sSpellMgr->GetSpellProcEntry(GetId()))
                roguesAura->AddProcCooldown(std::chrono::steady_clock::now() + procEntry->Cooldown);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rog_honor_among_thieves::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_rog_honor_among_thieves::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -14168 - Improved Expose Armor
class spell_rog_improved_expose_armor : public AuraScript
{
    PrepareAuraScript(spell_rog_improved_expose_armor);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ROGUE_IMPROVED_EXPOSE_ARMOR, SPELL_ROGUE_EXPOSE_ARMOR });
    }

    bool Load() override
    {
        return GetUnitOwner()->IsPlayer();
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        // Expose Armor shares its family mask with the other finishing moves
        if (!eventInfo.GetSpellInfo() || eventInfo.GetSpellInfo()->Id != SPELL_ROGUE_EXPOSE_ARMOR)
            return false;

        return eventInfo.GetProcTarget();
    }

    void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Player* player = GetTarget()->ToPlayer();
        int32 comboPoints = player->GetComboPoints();
        player->CastCustomSpell(eventInfo.GetProcTarget(), SPELL_ROGUE_IMPROVED_EXPOSE_ARMOR, &comboPoints, nullptr, nullptr, true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rog_improved_expose_armor::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_rog_improved_expose_armor::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 51690 - Killing Spree
class spell_rog_killing_spree_aura : public AuraScript
{
    PrepareAuraScript(spell_rog_killing_spree_aura);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_ROGUE_KILLING_SPREE_TELEPORT,
                SPELL_ROGUE_KILLING_SPREE_WEAPON_DMG,
                SPELL_ROGUE_KILLING_SPREE_DMG_BUFF
            });
    }

    void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->CastSpell(GetTarget(), SPELL_ROGUE_KILLING_SPREE_DMG_BUFF, true);
    }

    void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
    {
        while (!_targets.empty())
        {
            ObjectGuid guid = Acore::Containers::SelectRandomContainerElement(_targets);
            if (Unit* target = ObjectAccessor::GetUnit(*GetTarget(), guid))
            {
                // xinef: target may be no longer valid
                if (!GetTarget()->IsValidAttackTarget(target) || target->HasStealthAura() || target->HasInvisibilityAura())
                {
                    _targets.remove(guid);
                    continue;
                }

                GetTarget()->CastSpell(target, SPELL_ROGUE_KILLING_SPREE_TELEPORT, true);

                // xinef: ensure fast coordinates switch, dont wait for client to send opcode
                WorldLocation const& dest = GetTarget()->ToPlayer()->GetTeleportDest();
                GetTarget()->ToPlayer()->UpdatePosition(dest, true);

                GetTarget()->CastSpell(target, SPELL_ROGUE_KILLING_SPREE_WEAPON_DMG, TriggerCastFlags(TRIGGERED_FULL_MASK & ~TRIGGERED_DONT_REPORT_CAST_ERROR));
                break;
            }
            else
                _targets.remove(guid);
        }
    }

    void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_ROGUE_KILLING_SPREE_DMG_BUFF);
    }

public:
    void AddTarget(Unit* target)
    {
        _targets.push_back(target->GetGUID());
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_rog_killing_spree_aura::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_rog_killing_spree_aura::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        AfterEffectRemove += AuraEffectRemoveFn(spell_rog_killing_spree_aura::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }

private:
    GuidList _targets;
};

#define KillingSpreeScriptName "spell_rog_killing_spree"
typedef spell_rog_killing_spree_aura spell_rog_killing_spree_aura_script;
class spell_rog_killing_spree : public SpellScript
{
    PrepareSpellScript(spell_rog_killing_spree);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ROGUE_KILLING_SPREE });
    }

    SpellCastResult CheckCast()
    {
        // Kologarn area, Killing Spree should not work
        if (GetCaster()->GetMapId() == MAP_ULDUAR && GetCaster()->GetDistance2d(1766.936f, -24.748f) < 50.0f)
            return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
        return SPELL_CAST_OK;
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (targets.empty() || GetCaster()->GetVehicleBase() || GetCaster()->HasUnitState(UNIT_STATE_ROOT))
            FinishCast(SPELL_FAILED_OUT_OF_RANGE);
        else
        {
            // Added attribute not breaking stealth, removes auras here
            GetCaster()->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_CAST);
            GetCaster()->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_SPELL_ATTACK);
        }
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Aura* aura = GetCaster()->GetAura(SPELL_ROGUE_KILLING_SPREE))
        {
            if (spell_rog_killing_spree_aura* script = dynamic_cast<spell_rog_killing_spree_aura_script*>(aura->GetScriptByName(KillingSpreeScriptName)))
                script->AddTarget(GetHitUnit());
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_rog_killing_spree::CheckCast);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rog_killing_spree::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_rog_killing_spree::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
    }
};

// 76806 - Main Gauche (Combat mastery)
// Main-hand attacks have the mastery's chance of an extra attack.
class spell_rog_main_gauche : public AuraScript
{
    PrepareAuraScript(spell_rog_main_gauche);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ROGUE_MAIN_GAUCHE });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Player* player = eventInfo.GetActor() ? eventInfo.GetActor()->ToPlayer() : nullptr;
        if (!player || !eventInfo.GetProcTarget())
            return false;

        Item* item = player->GetWeaponForAttack(BASE_ATTACK);
        if (!item || !item->IsEquipped())
            return false;

        if (!(eventInfo.GetTypeMask() & PROC_FLAG_DONE_MAINHAND_ATTACK))
            return false;

        return roll_chance_i(GetEffect(EFFECT_0)->GetAmount());
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Player* player = eventInfo.GetActor()->ToPlayer();
        player->CastSpell(eventInfo.GetProcTarget(), SPELL_ROGUE_MAIN_GAUCHE, true, player->GetWeaponForAttack(BASE_ATTACK), aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rog_main_gauche::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_rog_main_gauche::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 31666 - Master of Subtlety
// 6 seconds after stealth ends the damage bonus goes, unless the rogue is stealthed again.
class spell_rog_master_of_subtlety : public AuraScript
{
    PrepareAuraScript(spell_rog_master_of_subtlety);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ROGUE_MASTER_OF_SUBTLETY_DAMAGE_PERCENT });
    }

    void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
    {
        if (!GetTarget()->HasAuraType(SPELL_AURA_MOD_STEALTH))
            GetTarget()->RemoveAurasDueToSpell(SPELL_ROGUE_MASTER_OF_SUBTLETY_DAMAGE_PERCENT);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_rog_master_of_subtlety::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// -14158 - Murderous Intent
// Backstab on a target at or below $s2% health gives energy.
class spell_rog_murderous_intent : public AuraScript
{
    PrepareAuraScript(spell_rog_murderous_intent);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ROGUE_MURDEROUS_INTENT });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetProcTarget() && eventInfo.GetProcTarget()->GetHealthPct() <= GetSpellInfo()->Effects[EFFECT_1].CalcValue(GetTarget());
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        int32 energy = aurEff->GetAmount();
        GetTarget()->CastCustomSpell(GetTarget(), SPELL_ROGUE_MURDEROUS_INTENT, &energy, nullptr, nullptr, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rog_murderous_intent::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_rog_murderous_intent::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 14143, 14149 - Remorseless Attacks
class spell_rog_remorseless_attacks : public AuraScript
{
    PrepareAuraScript(spell_rog_remorseless_attacks);

    bool _usedByMutilate = false;

public:
    bool WasUsedByMutilate() const
    {
        return _usedByMutilate;
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo)
            return true;

        // Block Mutilate MH (0x2) and OH (0x4) from consuming the charge immediately
        if (spellInfo->SpellFamilyName == SPELLFAMILY_ROGUE
            && (spellInfo->SpellFamilyFlags[1] & 0x6))
        {
            _usedByMutilate = true;
            return false;
        }

        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rog_remorseless_attacks::CheckProc);
    }
};

// 1329 - Mutilate
// The two hands' hits use up Cold Blood and Remorseless once, after the cast.
class spell_rog_mutilate : public SpellScript
{
    PrepareSpellScript(spell_rog_mutilate);

    void HandleAfterCast()
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        if (Aura* cb = caster->GetAura(SPELL_ROGUE_COLD_BLOOD))
        {
            auto* script = dynamic_cast<spell_rog_cold_blood*>(
                cb->GetScriptByName("spell_rog_cold_blood"));
            if (script && script->WasUsedByMutilate())
                cb->Remove();
        }

        if (Aura* remorseless = caster->GetAuraOfRankedSpell(SPELL_ROGUE_REMORSELESS_ATTACKS_R1))
        {
            auto* script = dynamic_cast<spell_rog_remorseless_attacks*>(
                remorseless->GetScriptByName("spell_rog_remorseless_attacks"));
            if (script && script->WasUsedByMutilate())
                remorseless->Remove();
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_rog_mutilate::HandleAfterCast);
    }
};

// -31130 - Nerves of Steel
class spell_rog_nerves_of_steel : public AuraScript
{
    PrepareAuraScript(spell_rog_nerves_of_steel);

    uint32 absorbPct;

    bool Load() override
    {
        absorbPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster());
        return true;
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        // Set absorbtion amount to unlimited
        amount = -1;
    }

    void Absorb(AuraEffect* /*aurEff*/, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        // reduces all damage taken while stun or fear
        if (GetTarget()->GetUnitFlags() & (UNIT_FLAG_FLEEING) || (GetTarget()->GetUnitFlags() & (UNIT_FLAG_STUNNED) && GetTarget()->HasAuraWithMechanic(1 << MECHANIC_STUN)))
            absorbAmount = CalculatePct(dmgInfo.GetDamage(), absorbPct);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_nerves_of_steel::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_rog_nerves_of_steel::Absorb, EFFECT_0);
    }
};

// 58428 - Overkill
// 6 seconds after stealth ends the energy regeneration bonus goes, unless the rogue is stealthed again.
class spell_rog_overkill : public AuraScript
{
    PrepareAuraScript(spell_rog_overkill);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ROGUE_OVERKILL_POWER_REGEN });
    }

    void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
    {
        if (!GetTarget()->HasAuraType(SPELL_AURA_MOD_STEALTH))
            GetTarget()->RemoveAurasDueToSpell(SPELL_ROGUE_OVERKILL_POWER_REGEN);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_rog_overkill::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 921 - Pick Pocket
class spell_rog_pickpocket : public SpellScript
{
    PrepareSpellScript(spell_rog_pickpocket);

    SpellCastResult CheckCast()
    {
        if (!GetExplTargetUnit() || !GetCaster()->IsValidAttackTarget(GetExplTargetUnit(), GetSpellInfo()))
            return SPELL_FAILED_BAD_TARGETS;

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_rog_pickpocket::CheckCast);
    }
};

// 14185 - Preparation
// 4.3.4: resets Vanish, Sprint and Shadowstep; with the glyph also Kick, Dismantle and Smoke Bomb (CPP).
class spell_rog_preparation : public SpellScript
{
    PrepareSpellScript(spell_rog_preparation);

    bool Load() override
    {
        return GetCaster()->IsPlayer();
    }

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ROGUE_GLYPH_OF_PREPARATION });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Player* caster = GetCaster()->ToPlayer();
        bool const hasGlyph = caster->HasAura(SPELL_ROGUE_GLYPH_OF_PREPARATION);

        PlayerSpellMap const& spellMap = caster->GetSpellMap();
        for (PlayerSpellMap::const_iterator itr = spellMap.begin(); itr != spellMap.end(); ++itr)
        {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(itr->first);
            if (!spellInfo || spellInfo->SpellFamilyName != SPELLFAMILY_ROGUE)
                continue;

            bool const reset = (spellInfo->SpellFamilyFlags[0] & ROGUE_FLAG0_VANISH_SPRINT) || (spellInfo->SpellFamilyFlags[1] & ROGUE_FLAG1_SHADOWSTEP) ||
                (hasGlyph && ((spellInfo->SpellFamilyFlags[1] & ROGUE_FLAG1_DISMANTLE_SMOKE_BOMB) || (spellInfo->SpellFamilyFlags[0] & ROGUE_FLAG0_KICK)));
            if (!reset)
                continue;

            SpellCooldowns::iterator citr = caster->GetSpellCooldownMap().find(spellInfo->Id);
            if (citr != caster->GetSpellCooldownMap().end() && citr->second.needSendToClient)
                caster->RemoveSpellCooldown(spellInfo->Id, true);
            else
                caster->RemoveSpellCooldown(spellInfo->Id, false);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_rog_preparation::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// -51685 - Prey on the Weak
class spell_rog_prey_on_the_weak : public AuraScript
{
    PrepareAuraScript(spell_rog_prey_on_the_weak);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ROGUE_PREY_ON_THE_WEAK });
    }

    void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
    {
        Unit* target = GetTarget();
        Unit* victim = target->GetVictim();
        if (!victim && target->IsPlayer())
            victim = target->ToPlayer()->GetSelectedUnit();

        if (victim && victim->IsAlive() && (target->GetHealthPct() > victim->GetHealthPct()))
        {
            if (!target->HasAura(SPELL_ROGUE_PREY_ON_THE_WEAK))
            {
                int32 bp = GetSpellInfo()->Effects[EFFECT_0].CalcValue();
                target->CastCustomSpell(target, SPELL_ROGUE_PREY_ON_THE_WEAK, &bp, 0, 0, true);
            }
        }
        else
            target->RemoveAurasDueToSpell(SPELL_ROGUE_PREY_ON_THE_WEAK);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_rog_prey_on_the_weak::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 14181 - Relentless Strikes
// The talent's target trigger (14179, 58422, 58423) casts this per finishing move; it restores the energy.
class spell_rog_relentless_strikes : public SpellScript
{
    PrepareSpellScript(spell_rog_relentless_strikes);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ROGUE_RELENTLESS_STRIKES_ENERGIZE });
    }

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(caster, SPELL_ROGUE_RELENTLESS_STRIKES_ENERGIZE, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_rog_relentless_strikes::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// -79095 - Restless Blades
// Damaging finishing moves shorten Adrenaline Rush, Killing Spree, Redirect and Sprint per combo point.
class spell_rog_restless_blades : public AuraScript
{
    PrepareAuraScript(spell_rog_restless_blades);

    bool Load() override
    {
        return GetUnitOwner()->IsPlayer();
    }

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ROGUE_ADRENALINE_RUSH, SPELL_ROGUE_KILLING_SPREE, SPELL_ROGUE_REDIRECT, SPELL_ROGUE_SPRINT });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        Player* player = GetTarget()->ToPlayer();
        int32 const reduction = -int32(player->GetComboPoints()) * aurEff->GetAmount();
        if (!reduction)
            return;

        for (uint32 spellId : { SPELL_ROGUE_ADRENALINE_RUSH, SPELL_ROGUE_KILLING_SPREE, SPELL_ROGUE_REDIRECT, SPELL_ROGUE_SPRINT })
            player->ModifySpellCooldown(spellId, reduction);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_rog_restless_blades::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -14156 - Ruthlessness
// "add a combo point to your target": the finishing move's target, which still holds the combo points when the
// cast-phase proc comes (TDB's row; it has no target there). The point is added after the move spends its own
// (Unit::EndComboPointSpend), so it is not wiped. Finishing moves on the rogue (Slice and Dice, Recuperate) give
// it to the target the points are on.
class spell_rog_ruthlessness : public AuraScript
{
    PrepareAuraScript(spell_rog_ruthlessness);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ROGUE_RUTHLESSNESS_COMBO_POINT });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        if (Unit* target = GetTarget()->GetComboTarget())
            GetTarget()->CastSpell(target, SPELL_ROGUE_RUTHLESSNESS_COMBO_POINT, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_rog_ruthlessness::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 73981 - Redirect
// "Transfers any existing combo points to the current enemy target, as well as any insight gained from Bandit's
// Guile" (with the talent). Its data is an add-combo-points effect of 0, which does nothing (CPP neither).
class spell_rog_redirect : public SpellScript
{
    PrepareSpellScript(spell_rog_redirect);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ROGUE_BANDITS_GUILE_TALENT_R1 });
    }

    SpellCastResult CheckCast()
    {
        // "Requires active combo points"
        if (!GetCaster()->GetComboPoints())
            return SPELL_FAILED_NO_COMBO_POINTS;
        return SPELL_CAST_OK;
    }

    void HandleTransfer(SpellEffIndex effIndex)
    {
        PreventHitDefaultEffect(effIndex);
        Unit* rogue = GetCaster();
        Unit* target = GetHitUnit();
        if (!target)
            return;

        if (uint8 const points = rogue->GetComboPoints(); points && rogue->GetComboTarget() != target)
            rogue->AddComboPoints(target, int8(points));     // a new target takes the count over

        if (Aura* guile = rogue->GetAuraOfRankedSpell(SPELL_ROGUE_BANDITS_GUILE_TALENT_R1))
            if (spell_rog_bandits_guile* script = guile->GetScript<spell_rog_bandits_guile>("spell_rog_bandits_guile"))
                script->TransferInsight(target);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_rog_redirect::CheckCast);
        OnEffectHitTarget += SpellEffectFn(spell_rog_redirect::HandleTransfer, EFFECT_0, SPELL_EFFECT_ADD_COMBO_POINTS);
    }
};

// 1943 - Rupture
// Attack power per combo point (4.3.4 tooltip), Revealing Strike's bonus; Venomous Wounds gives back energy
// for the time left when the target dies with it.
class spell_rog_rupture : public AuraScript
{
    PrepareAuraScript(spell_rog_rupture);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ROGUE_REVEALING_STRIKE, SPELL_ROGUE_VENOMOUS_VIM });
    }

    bool Load() override
    {
        Unit* caster = GetCaster();
        return caster && caster->IsPlayer();
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        if (Unit* caster = GetCaster())
        {
            canBeRecalculated = false;

            float const attackpowerPerCombo[6] =
            {
                0.0f,
                0.015f,         // 1 point:  ${($m1 + $b1*1 + 0.015 * $AP) * 4} damage over 8 secs
                0.024f,         // 2 points: ${($m1 + $b1*2 + 0.024 * $AP) * 5} damage over 10 secs
                0.03f,          // 3 points: ${($m1 + $b1*3 + 0.03 * $AP) * 6} damage over 12 secs
                0.03428571f,    // 4 points: ${($m1 + $b1*4 + 0.03428571 * $AP) * 7} damage over 14 secs
                0.0375f         // 5 points: ${($m1 + $b1*5 + 0.0375 * $AP) * 8} damage over 16 secs
            };

            uint8 const cp = std::min<uint8>(caster->GetComboPoints(), 5);
            amount += int32(caster->GetTotalAttackPowerValue(BASE_ATTACK) * attackpowerPerCombo[cp]);

            if (AuraEffect* revealingStrike = GetUnitOwner()->GetAuraEffect(SPELL_ROGUE_REVEALING_STRIKE, EFFECT_2, caster->GetGUID()))
            {
                amount += CalculatePct(amount, revealingStrike->GetAmount());
                revealingStrike->GetBase()->Remove();
            }
        }
    }

    void HandleVenomousWounds(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_DEATH)
            return;

        // the rogue's talent, the rogue's energy (CPP looked both up on the dying target)
        Unit* caster = GetCaster();
        if (!caster || !caster->GetDummyAuraEffect(SPELLFAMILY_ROGUE, ROGUE_ICON_ID_VENOMOUS_WOUNDS, EFFECT_0))
            return;

        int32 energy = int32(std::ceil(GetDuration() / float(IN_MILLISECONDS))) * 5;
        if (energy > 0)
            caster->CastCustomSpell(caster, SPELL_ROGUE_VENOMOUS_VIM, &energy, nullptr, nullptr, true);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_rupture::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
        AfterEffectRemove += AuraEffectRemoveFn(spell_rog_rupture::HandleVenomousWounds, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};

// 6770 - Sap
// Blackjack lingers after Sap wears off.
class spell_rog_sap : public AuraScript
{
    PrepareAuraScript(spell_rog_sap);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ROGUE_BLACKJACK_R1 });
    }

    void HandleBlackjack(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        // the rank's effect spell follows its talent id (79123 -> 79124, 79125 -> 79126)
        if (Aura const* blackjack = caster->GetAuraOfRankedSpell(SPELL_ROGUE_BLACKJACK_R1))
            caster->CastSpell(GetTarget(), blackjack->GetSpellInfo()->Id + 1, true);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_rog_sap::HandleBlackjack, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
    }
};

// 58683, 58684 - Savage Combat
class spell_rog_savage_combat : public AuraScript
{
    PrepareAuraScript(spell_rog_savage_combat);

    void CalcPeriodic(AuraEffect const* /*effect*/, bool& isPeriodic, int32& amplitude)
    {
        isPeriodic = true;
        amplitude = 1000;
    }

    void Update(AuraEffect*  /*auraEffect*/)
    {
        Unit::AuraApplicationMap const& auras = GetUnitOwner()->GetAppliedAuras();
        for (Unit::AuraApplicationMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
            if (itr->second->GetBase()->GetCasterGUID() == this->GetCasterGUID() && itr->second->GetBase()->GetSpellInfo()->Dispel == DISPEL_POISON)
                return;

        SetDuration(0);
    }

    void Register() override
    {
        DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_rog_savage_combat::CalcPeriodic, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN);
        OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_rog_savage_combat::Update, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN);
    }
};

// -13983 - Setup
class spell_rog_setup : public AuraScript
{
    PrepareAuraScript(spell_rog_setup);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (Player* target = GetTarget()->ToPlayer())
            if (eventInfo.GetActor() == target->GetSelectedUnit())
                return true;

        return false;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rog_setup::CheckProc);
    }
};

// 5938 - Shiv
class spell_rog_shiv : public SpellScript
{
    PrepareSpellScript(spell_rog_shiv);

    bool Load() override
    {
        return GetCaster()->IsPlayer();
    }

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ROGUE_SHIV_TRIGGERED });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (Unit* unitTarget = GetHitUnit())
            caster->CastSpell(unitTarget, SPELL_ROGUE_SHIV_TRIGGERED, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_rog_shiv::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 1784 - Stealth
// Master of Subtlety and Overkill while stealthed and for 6 seconds after; Elusiveness' speed.
class spell_rog_stealth : public AuraScript
{
    PrepareAuraScript(spell_rog_stealth);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_ROGUE_MASTER_OF_SUBTLETY_PASSIVE,
                SPELL_ROGUE_MASTER_OF_SUBTLETY_DAMAGE_PERCENT,
                SPELL_ROGUE_MASTER_OF_SUBTLETY_PERIODIC,
                SPELL_ROGUE_OVERKILL_TALENT,
                SPELL_ROGUE_OVERKILL_POWER_REGEN,
                SPELL_ROGUE_OVERKILL_PERIODIC,
                SPELL_RACIAL_ELUSIVENESS
            });
    }

    void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();

        if (AuraEffect const* subtlety = target->GetAuraEffect(SPELL_ROGUE_MASTER_OF_SUBTLETY_PASSIVE, EFFECT_0))
        {
            target->RemoveAurasDueToSpell(SPELL_ROGUE_MASTER_OF_SUBTLETY_PERIODIC);
            int32 amount = subtlety->GetAmount();
            target->CastCustomSpell(target, SPELL_ROGUE_MASTER_OF_SUBTLETY_DAMAGE_PERCENT, &amount, nullptr, nullptr, true);
        }

        if (target->HasAura(SPELL_ROGUE_OVERKILL_TALENT))
        {
            target->RemoveAurasDueToSpell(SPELL_ROGUE_OVERKILL_PERIODIC);
            target->CastSpell(target, SPELL_ROGUE_OVERKILL_POWER_REGEN, true);
        }
    }

    void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();

        if (target->HasAura(SPELL_ROGUE_MASTER_OF_SUBTLETY_PASSIVE))
            target->CastSpell(target, SPELL_ROGUE_MASTER_OF_SUBTLETY_PERIODIC, true);

        if (target->HasAura(SPELL_ROGUE_OVERKILL_TALENT))
            target->CastSpell(target, SPELL_ROGUE_OVERKILL_PERIODIC, true);
    }

    void HandleSpeedIncrease(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        // Elusiveness (Night Elf racial)
        if (AuraEffect const* elusiveness = GetUnitOwner()->GetAuraEffect(SPELL_RACIAL_ELUSIVENESS, EFFECT_0))
            amount += elusiveness->GetAmount();
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_rog_stealth::HandleEffectApply, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        AfterEffectRemove += AuraEffectRemoveFn(spell_rog_stealth::HandleEffectRemove, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_stealth::HandleSpeedIncrease, EFFECT_2, SPELL_AURA_MOD_SPEED_ALWAYS);
    }
};

// 57934 - Tricks of the Trade (AuraScript)
class spell_rog_tricks_of_the_trade_aura : public AuraScript
{
    PrepareAuraScript(spell_rog_tricks_of_the_trade_aura);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ROGUE_TRICKS_OF_THE_TRADE_DMG_BOOST, SPELL_ROGUE_TRICKS_OF_THE_TRADE_PROC });
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_DEFAULT || !GetTarget()->HasAura(SPELL_ROGUE_TRICKS_OF_THE_TRADE_PROC))
            GetTarget()->GetThreatMgr().UnregisterRedirectThreat(SPELL_ROGUE_TRICKS_OF_THE_TRADE);
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();

        Unit* rogue = GetTarget();
        Unit* target = ObjectAccessor::GetUnit(*rogue, _redirectTarget);
        if (target)
        {
            rogue->CastSpell(target, SPELL_ROGUE_TRICKS_OF_THE_TRADE_DMG_BOOST, true, nullptr, aurEff);
            rogue->CastSpell(rogue, SPELL_ROGUE_TRICKS_OF_THE_TRADE_PROC, true, nullptr, aurEff);
        }
        Remove(AURA_REMOVE_BY_DEFAULT);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_rog_tricks_of_the_trade_aura::OnRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectProc += AuraEffectProcFn(spell_rog_tricks_of_the_trade_aura::HandleProc, EFFECT_1, SPELL_AURA_DUMMY);
    }

    ObjectGuid _redirectTarget;
public:
    void SetRedirectTarget(ObjectGuid const& guid) { _redirectTarget = guid; }
};

// 57934 - Tricks of the Trade (SpellScript)
class spell_rog_tricks_of_the_trade : public SpellScript
{
    PrepareSpellScript(spell_rog_tricks_of_the_trade);

    void DoAfterHit()
    {
        if (Aura* aura = GetHitAura())
            if (auto* script = aura->GetScript<spell_rog_tricks_of_the_trade_aura>("spell_rog_tricks_of_the_trade"))
            {
                if (Unit* explTarget = GetExplTargetUnit())
                    script->SetRedirectTarget(explTarget->GetGUID());
                else
                    script->SetRedirectTarget(ObjectGuid::Empty);
            }
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_rog_tricks_of_the_trade::DoAfterHit);
    }
};

// 59628 - Tricks of the Trade (Proc)
class spell_rog_tricks_of_the_trade_proc : public AuraScript
{
    PrepareAuraScript(spell_rog_tricks_of_the_trade_proc);

    void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->GetThreatMgr().UnregisterRedirectThreat(SPELL_ROGUE_TRICKS_OF_THE_TRADE);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_rog_tricks_of_the_trade_proc::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// -51627 - Turn the Tables
class spell_rog_turn_the_tables : public AuraScript
{
    PrepareAuraScript(spell_rog_turn_the_tables);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->Effects[EFFECT_0].TriggerSpell });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();

        Unit* caster = GetCaster();
        if (!caster)
            return;

        caster->CastSpell(caster, GetSpellInfo()->Effects[EFFECT_0].TriggerSpell, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_rog_turn_the_tables::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 52910 - Turn the Tables (rank 1's buff targets the raid around the rogue in the 4.3.4 data; only the rogue gets it)
class spell_rog_turn_the_tables_proc : public SpellScript
{
    PrepareSpellScript(spell_rog_turn_the_tables_proc);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.clear();
        targets.push_back(GetCaster());
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rog_turn_the_tables_proc::FilterTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_RAID);
    }
};

enum vanish
{
    SPELL_PARALYZE          = 38132,
    SPELL_CLEAN_ESCAPE_AURA = 23582,
    SPELL_CLEAN_ESCAPE_HEAL = 23583
};

// 18461 - Vanish Purge (Server Side)
class spell_rog_vanish_purge : public SpellScript
{
    PrepareSpellScript(spell_rog_vanish_purge);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PARALYZE });
    }

    void HandleRootRemove(SpellEffIndex /*effIndex*/)
    {
        if (GetCaster() && !GetCaster()->HasAura(SPELL_PARALYZE)) // Root from Tainted Core SSC, should not be removed by vanish.
            GetCaster()->RemoveAurasWithMechanic(1ULL << MECHANIC_ROOT);
    }

    void HandleSnareRemove(SpellEffIndex /*effIndex*/)
    {
        if (GetCaster())
            GetCaster()->RemoveAurasWithMechanic(1ULL << MECHANIC_SNARE);
    }

    void Register() override
    {
        // Blizzard handles EFFECT_0 as the unroot and EFFECT_1 as unsnare. Hence why they are not done in the same place.
        OnEffectHitTarget += SpellEffectFn(spell_rog_vanish_purge::HandleRootRemove, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        OnEffectHitTarget += SpellEffectFn(spell_rog_vanish_purge::HandleSnareRemove, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
    }
};

// 1856 - Vanish
class spell_rog_vanish : public SpellScript
{
    PrepareSpellScript(spell_rog_vanish);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ROGUE_STEALTH, SPELL_CLEAN_ESCAPE_AURA, SPELL_CLEAN_ESCAPE_HEAL });
    }

    void HandleEffect(SpellEffIndex /*effIndex*/)
    {
        if (GetCaster())
        {
            GetCaster()->RemoveAurasByType(SPELL_AURA_MOD_STALKED);

            if (!GetCaster()->HasAura(SPELL_ROGUE_STEALTH))
            {
                // Remove stealth cooldown if needed.
                if (GetCaster()->IsPlayer() && GetCaster()->HasSpellCooldown(SPELL_ROGUE_STEALTH))
                    GetCaster()->ToPlayer()->RemoveSpellCooldown(SPELL_ROGUE_STEALTH);

                GetCaster()->CastSpell(GetCaster(), SPELL_ROGUE_STEALTH, true);
            }

            if (GetCaster()->HasAura(SPELL_CLEAN_ESCAPE_AURA))
                GetCaster()->CastSpell(GetCaster(), SPELL_CLEAN_ESCAPE_HEAL, true);
        }
    }

    void Register() override
    {
        // Cataclysm: the sanctuary moved to EFFECT_0 as SPELL_EFFECT_SANCTUARY_2
        OnEffectHitTarget += SpellEffectFn(spell_rog_vanish::HandleEffect, EFFECT_0, SPELL_EFFECT_SANCTUARY_2);
    }
};

// -79133 - Venomous Wounds
// Rupture and Garrote ticks on a target with one of the rogue's poisons: extra Nature damage and energy.
class spell_rog_venomous_wounds : public AuraScript
{
    PrepareAuraScript(spell_rog_venomous_wounds);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ROGUE_VENOMOUS_WOUND, SPELL_ROGUE_VENOMOUS_VIM });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Unit* target = eventInfo.GetProcTarget();
        if (!target)
            return false;

        ObjectGuid const rogue = GetTarget()->GetGUID();
        // Deadly, Wound, Crippling and Mind-numbing Poison (CPP)
        return target->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_ROGUE, 0, 0x80000, 0, rogue)
            || target->GetAuraEffect(SPELL_AURA_MOD_HEALING_PCT, SPELLFAMILY_ROGUE, 0, 0x80000, 0, rogue)
            || target->GetAuraEffect(SPELL_AURA_MOD_DECREASE_SPEED, SPELLFAMILY_ROGUE, 0, 0x80000, 0, rogue)
            || target->GetAuraEffect(SPELL_AURA_HASTE_SPELLS, SPELLFAMILY_ROGUE, 0, 0x80000, 0, rogue);
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(eventInfo.GetProcTarget(), SPELL_ROGUE_VENOMOUS_WOUND, true, nullptr, aurEff);
        if (AuraEffect const* energize = GetEffect(EFFECT_1))
        {
            int32 energy = energize->GetAmount();
            GetTarget()->CastCustomSpell(GetTarget(), SPELL_ROGUE_VENOMOUS_VIM, &energy, nullptr, nullptr, true, nullptr, aurEff);
        }
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rog_venomous_wounds::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_rog_venomous_wounds::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

void AddSC_rogue_spell_scripts()
{
    RegisterSpellScript(spell_rog_bandits_guile);
    RegisterSpellScript(spell_rog_blade_flurry);
    RegisterSpellScript(spell_rog_cheat_death);
    RegisterSpellScript(spell_rog_cold_blood);
    RegisterSpellScript(spell_rog_combat_potency);
    RegisterSpellScript(spell_rog_crippling_poison);
    RegisterSpellScript(spell_rog_cut_to_the_chase);
    RegisterSpellScript(spell_rog_deadly_momentum);
    RegisterSpellScript(spell_rog_deadly_poison);
    RegisterSpellScript(spell_rog_envenom);
    RegisterSpellScript(spell_rog_eviscerate);
    RegisterSpellScript(spell_rog_glyph_of_hemorrhage);
    RegisterSpellScript(spell_rog_gouge);
    RegisterSpellScript(spell_rog_hemorrhage);
    RegisterSpellScript(spell_rog_honor_among_thieves);
    RegisterSpellScript(spell_rog_improved_expose_armor);
    RegisterSpellAndAuraScriptPair(spell_rog_killing_spree, spell_rog_killing_spree_aura);
    RegisterSpellScript(spell_rog_main_gauche);
    RegisterSpellScript(spell_rog_master_of_subtlety);
    RegisterSpellScript(spell_rog_murderous_intent);
    RegisterSpellScript(spell_rog_mutilate);
    RegisterSpellScript(spell_rog_nerves_of_steel);
    RegisterSpellScript(spell_rog_overkill);
    RegisterSpellScript(spell_rog_pickpocket);
    RegisterSpellScript(spell_rog_preparation);
    RegisterSpellScript(spell_rog_prey_on_the_weak);
    RegisterSpellScript(spell_rog_recuperate);
    RegisterSpellScript(spell_rog_redirect);
    RegisterSpellScript(spell_rog_relentless_strikes);
    RegisterSpellScript(spell_rog_remorseless_attacks);
    RegisterSpellScript(spell_rog_restless_blades);
    RegisterSpellScript(spell_rog_rupture);
    RegisterSpellScript(spell_rog_ruthlessness);
    RegisterSpellScript(spell_rog_sap);
    RegisterSpellScript(spell_rog_savage_combat);
    RegisterSpellScript(spell_rog_setup);
    RegisterSpellScript(spell_rog_shiv);
    RegisterSpellScript(spell_rog_slice_and_dice);
    RegisterSpellScript(spell_rog_stealth);
    RegisterSpellAndAuraScriptPair(spell_rog_tricks_of_the_trade, spell_rog_tricks_of_the_trade_aura);
    RegisterSpellScript(spell_rog_tricks_of_the_trade_proc);
    RegisterSpellScript(spell_rog_turn_the_tables);
    RegisterSpellScript(spell_rog_turn_the_tables_proc);
    RegisterSpellScript(spell_rog_vanish);
    RegisterSpellScript(spell_rog_vanish_purge);
    RegisterSpellScript(spell_rog_venomous_wounds);
}
