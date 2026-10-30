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

#include "Battleground.h"
#include "ConditionMgr.h"
#include "BattlegroundMgr.h"
#include "Creature.h"
#include "DatabaseEnv.h"
#include "GameGraveyard.h"
#include "NPCPackets.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "Pet.h"
#include "Player.h"
#include "ReputationMgr.h"
#include "ScriptMgr.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "Trainer.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include <cmath>

// 4.3.4 values (CPP Pet.h)
enum StableResultCode
{
    STABLE_ERR_MONEY        = 0x01,                         // "you don't have enough money"
    STABLE_ERR_INVALID_SLOT = 0x03,                         // "That slot is locked"
    STABLE_SUCCESS_STABLE   = 0x08,                         // stable success
    STABLE_SUCCESS_UNSTABLE = 0x09,                         // unstable/swap success
    STABLE_SUCCESS_BUY_SLOT = 0x0A,                         // buy slot success
    STABLE_ERR_EXOTIC       = 0x0B,                         // "you are unable to control exotic creatures"
    STABLE_ERR_STABLE       = 0x0C                          // "Internal pet error"
};

enum PetStableFlags : uint8
{
    PET_STABLE_ACTIVE       = 0x01,
    PET_STABLE_INACTIVE     = 0x02
};

void WorldSession::HandleTabardVendorActivateOpcode(WorldPacket& recvData)
{
    ObjectGuid guid;
    recvData >> guid;

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_TABARDDESIGNER);
    if (!unit)
    {
        LOG_DEBUG("network", "WORLD: HandleTabardVendorActivateOpcode - Unit ({}) not found or you can not interact with him.", guid.ToString());
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    SendTabardVendorActivate(guid);
}

void WorldSession::SendTabardVendorActivate(ObjectGuid guid)
{
    WorldPacket data(MSG_TABARDVENDOR_ACTIVATE, 8);
    data << guid;
    SendPacket(&data);
}

void WorldSession::SendShowMailBox(ObjectGuid guid)
{
    WorldPacket data(SMSG_SHOW_MAILBOX, 8);
    data << guid;
    SendPacket(&data);
}

void WorldSession::HandleTrainerListOpcode(WorldPackets::NPC::Hello& packet)
{
    Creature* npc = GetPlayer()->GetNPCIfCanInteractWith(packet.Unit, UNIT_NPC_FLAG_TRAINER);
    if (!npc)
    {
        LOG_DEBUG("network", "WorldSession: SendTrainerList - {} not found or you can not interact with him.", packet.Unit.ToString().c_str());
        return;
    }

    SendTrainerList(npc);
}

void WorldSession::SendTrainerList(Creature* npc)
{
    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    Trainer::Trainer const* trainer = sObjectMgr->GetTrainer(npc->GetEntry());
    if (!trainer)
    {
        LOG_DEBUG("network", "WorldSession: SendTrainerList - trainer spells not found for {}", npc->GetGUID().ToString().c_str());
        return;
    }

    if (!trainer->IsTrainerValidForPlayer(_player))
    {
        LOG_DEBUG("network", "WorldSession: SendTrainerList - trainer {} not valid for player {}", npc->GetGUID().ToString().c_str(), GetPlayerInfo().c_str());
        return;
    }

    npc->PauseMovementForInteraction();

    trainer->SendSpells(npc, _player, GetSessionDbLocaleIndex());
}

void WorldSession::HandleTrainerBuySpellOpcode(WorldPackets::NPC::TrainerBuySpell& packet)
{
    LOG_DEBUG("network", "WORLD: Received CMSG_TRAINER_BUY_SPELL {}, learn spell id is: {}", packet.TrainerGUID.ToString().c_str(), packet.SpellID);

    Creature* npc = GetPlayer()->GetNPCIfCanInteractWith(packet.TrainerGUID, UNIT_NPC_FLAG_TRAINER);
    if (!npc)
    {
        LOG_DEBUG("network", "WORLD: HandleTrainerBuySpellOpcode - {} not found or you can not interact with him.", packet.TrainerGUID.ToString().c_str());
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    Trainer::Trainer* trainer = sObjectMgr->GetTrainer(npc->GetEntry());
    if (!trainer)
        return;

    // 4.x carries the trainer id in the request; it must name the trainer this NPC currently offers
    // through AC's creature -> default trainer mapping (the list a client was previously shown is not
    // tracked, unlike the reference's interaction history)
    if (trainer->GetTrainerId() != uint32(packet.TrainerID))
    {
        LOG_DEBUG("network", "WORLD: HandleTrainerBuySpellOpcode - trainer id {} does not match {} for {}",
            packet.TrainerID, trainer->GetTrainerId(), packet.TrainerGUID.ToString());
        return;
    }

    trainer->TeachSpell(npc, _player, packet.SpellID);
}

void WorldSession::HandleGossipHelloOpcode(WorldPacket& recvData)
{
    ObjectGuid guid;
    recvData >> guid;

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_NONE);
    if (!unit)
    {
        LOG_DEBUG("network", "WORLD: HandleGossipHelloOpcode - Unit ({}) not found or you can not interact with him.", guid.ToString());
        return;
    }

    // xinef: check if we have ANY npc flags
    if (unit->GetNpcFlags() == UNIT_NPC_FLAG_NONE)
        return;

    // Check GossipHello conditions - block gossip opening if conditions not met
    ConditionList gossipConditions = sConditionMgr->GetConditionsForNotGroupedEntry(CONDITION_SOURCE_TYPE_GOSSIP_HELLO, unit->GetEntry());
    if (!sConditionMgr->IsObjectMeetToConditions(_player, unit, gossipConditions))
        return;

    // set faction visible if needed
    if (FactionTemplateEntry const* factionTemplateEntry = sFactionTemplateStore.LookupEntry(unit->GetFaction()))
        _player->GetReputationMgr().SetVisible(factionTemplateEntry);

    GetPlayer()->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_TALK);
    // remove fake death
    //if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
    //    GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    unit->PauseMovementForInteraction();

    // Update home position for patrolling NPCs only (prevents drift for stationary NPCs)
    if (unit->GetDefaultMovementType() == WAYPOINT_MOTION_TYPE ||
        unit->GetMotionMaster()->GetCurrentMovementGeneratorType() == WAYPOINT_MOTION_TYPE)
        unit->SetHomePosition(unit->GetPosition());

    // If spiritguide, no need for gossip menu, just put player into resurrect queue
    if (unit->IsSpiritGuide())
    {
        Battleground* bg = _player->GetBattleground();
        if (bg)
        {
            bg->AddPlayerToResurrectQueue(unit->GetGUID(), _player->GetGUID());
            sBattlegroundMgr->SendAreaSpiritHealerQueryOpcode(_player, bg, unit->GetGUID());
            return;
        }
    }

    if (!sScriptMgr->OnGossipHello(_player, unit))
    {
        //        _player->TalkedToCreature(unit->GetEntry(), unit->GetGUID());
        _player->PrepareGossipMenu(unit, unit->GetGossipMenuId(), true);
        _player->SendPreparedGossip(unit);
    }
    unit->AI()->sGossipHello(_player);
}

/*void WorldSession::HandleGossipSelectOptionOpcode(WorldPacket & recvData)
{
    LOG_DEBUG("network.opcode", "WORLD: CMSG_GOSSIP_SELECT_OPTION");

    uint32 option;
    uint32 unk;
    ObjectGuid guid;
    std::string code = "";

    recvData >> guid >> unk >> option;

    if (_player->PlayerTalkClass->GossipOptionCoded(option))
    {
        LOG_DEBUG("network.opcode", "reading string");
        recvData >> code;
        LOG_DEBUG("network.opcode", "string read: {}", code);
    }

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_NONE);
    if (!unit)
    {
        LOG_DEBUG("network.opcode", "WORLD: HandleGossipSelectOptionOpcode - Unit ({}) not found or you can't interact with him.", guid.ToString());
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    if (!code.empty())
    {
        if (!Script->GossipSelectWithCode(_player, unit, _player->PlayerTalkClass->GossipOptionSender (option), _player->PlayerTalkClass->GossipOptionAction(option), code.c_str()))
            unit->OnGossipSelect (_player, option);
    }
    else
    {
        if (!Script->OnGossipSelect (_player, unit, _player->PlayerTalkClass->GossipOptionSender (option), _player->PlayerTalkClass->GossipOptionAction (option)))
           unit->OnGossipSelect (_player, option);
    }
}*/

void WorldSession::HandleSpiritHealerActivateOpcode(WorldPacket& recvData)
{
    LOG_DEBUG("network", "WORLD: CMSG_SPIRIT_HEALER_ACTIVATE");

    ObjectGuid guid;

    recvData >> guid;

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_SPIRITHEALER);
    if (!unit)
    {
        LOG_DEBUG("network", "WORLD: HandleSpiritHealerActivateOpcode - Unit ({}) not found or you can not interact with him.", guid.ToString());
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    SendSpiritResurrect();
}

void WorldSession::SendSpiritResurrect()
{
    if (_player->IsAlive())
        return;

    _player->ResurrectPlayer(0.5f, true);

    float durabilityLossOnSpiritResurrect = sWorld->getRate(RATE_DURABILITY_LOSS_ON_SPIRIT_RESURRECT) / 100.0f;
    if (durabilityLossOnSpiritResurrect)
        _player->DurabilityLossAll(durabilityLossOnSpiritResurrect, true);

    // get corpse nearest graveyard
    GraveyardStruct const* corpseGrave = nullptr;

    // Search for any graveyards near the player's corpse.
    corpseGrave = sGraveyard->GetClosestGraveyard(_player, _player->GetTeamId(), _player->HasCorpse());

    // now can spawn bones
    _player->SpawnCorpseBones();

    // teleport to nearest from corpse graveyard, if different from nearest to player ghost
    if (corpseGrave)
    {
        GraveyardStruct const* ghostGrave = sGraveyard->GetClosestGraveyard(_player, _player->GetTeamId());

        if (corpseGrave != ghostGrave)
            _player->TeleportTo(corpseGrave->Map, corpseGrave->x, corpseGrave->y, corpseGrave->z, _player->GetOrientation());
        // or update at original position
        //else
        //    _player->UpdateObjectVisibility(); // xinef: not needed, called in ResurrectPlayer
    }
    // or update at original position
    //else
    //    _player->UpdateObjectVisibility(); // xinef: not needed, called in ResurrectPlayer
}

void WorldSession::HandleBinderActivateOpcode(WorldPacket& recvData)
{
    ObjectGuid npcGUID;
    recvData >> npcGUID;

    if (!GetPlayer()->IsInWorld() || !GetPlayer()->IsAlive())
        return;

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(npcGUID, UNIT_NPC_FLAG_INNKEEPER);
    if (!unit)
    {
        LOG_DEBUG("network", "WORLD: HandleBinderActivateOpcode - Unit ({}) not found or you can not interact with him.", npcGUID.ToString());
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    SendBindPoint(unit);
}

void WorldSession::SendBindPoint(Creature* npc)
{
    // prevent set homebind to instances in any case
    if (GetPlayer()->GetMap()->Instanceable())
        return;

    uint32 bindspell = 3286;

    // send spell for homebinding (3286)
    npc->CastSpell(_player, bindspell, true);

    WorldPacket data(SMSG_TRAINER_BUY_SUCCEEDED, (8 + 4));
    data << npc->GetGUID();
    data << uint32(bindspell);
    SendPacket(&data);

    _player->PlayerTalkClass->SendCloseGossip();
}

void WorldSession::HandleListStabledPetsOpcode(WorldPacket& recvData)
{
    LOG_DEBUG("network", "WORLD: Recv MSG_LIST_STABLED_PETS");
    ObjectGuid npcGUID;

    recvData >> npcGUID;

    if (!CheckStableMaster(npcGUID))
        return;

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    // remove mounts this fix bug where getting pet from stable while mounted deletes pet.
    if (GetPlayer()->IsMounted())
        GetPlayer()->RemoveAurasByType(SPELL_AURA_MOUNTED);

    SendStablePet(npcGUID);
}

void WorldSession::SendStablePet(ObjectGuid guid)
{
    // 4.3.4 layout (CPP): every hunter pet with its slot. Sent with an empty guid at login and when the
    // pets change, the client fills the Call Pet flyout from it.
    WorldPacket data(MSG_LIST_STABLED_PETS, 200);
    data << guid;
    std::size_t countPos = data.wpos();
    data << uint8(0);                                       // pet count, set below
    data << uint8(MAX_PET_STABLES);                         // stable slots: all of them in 4.3.4

    uint8 count = 0;
    auto writePet = [&data, &count](PetStable::PetInfo const& pet)
    {
        data << int32(pet.Slot);
        data << uint32(pet.PetNumber);
        data << uint32(pet.CreatureId);
        data << uint32(pet.Level);
        data << pet.Name;
        data << uint8(PET_STABLE_ACTIVE | (pet.Slot > PET_SLOT_LAST_ACTIVE_SLOT ? PET_STABLE_INACTIVE : 0));
        ++count;
    };

    if (PetStable const* petStable = GetPlayer()->GetPetStable())
    {
        if (petStable->CurrentPet && petStable->CurrentPet->Type == HUNTER_PET)
            writePet(*petStable->CurrentPet);

        for (PetStable::PetInfo const& pet : petStable->UnslottedPets)
            if (pet.Type == HUNTER_PET)
                writePet(pet);

        for (Optional<PetStable::PetInfo> const& pet : petStable->StabledPets)
            if (pet)
                writePet(*pet);
    }

    data.put<uint8>(countPos, count);
    SendPacket(&data);
}

void WorldSession::SendStableResult(uint8 res)
{
    WorldPacket data(SMSG_STABLE_RESULT, 1);
    data << uint8(res);
    SendPacket(&data);
}

void WorldSession::HandleStableRevivePet(WorldPacket& /* recvData */)
{
    LOG_DEBUG("network", "HandleStableRevivePet: Not implemented");
}

// CMSG_SET_PET_SLOT: the 4.3.4 stable moves a pet to another slot (CPP HandleSetPetSlot / UpdatePetSlot);
// a pet already there takes the moved pet's old slot. A summoned pet involved is sent back first.
void WorldSession::HandleSetPetSlot(WorldPacket& recvData)
{
    uint32 petNumber;
    uint8 newSlot;
    recvData >> petNumber >> newSlot;

    ObjectGuid npcGuid;
    npcGuid[3] = recvData.ReadBit();
    npcGuid[2] = recvData.ReadBit();
    npcGuid[0] = recvData.ReadBit();
    npcGuid[7] = recvData.ReadBit();
    npcGuid[5] = recvData.ReadBit();
    npcGuid[6] = recvData.ReadBit();
    npcGuid[1] = recvData.ReadBit();
    npcGuid[4] = recvData.ReadBit();

    recvData.ReadByteSeq(npcGuid[5]);
    recvData.ReadByteSeq(npcGuid[3]);
    recvData.ReadByteSeq(npcGuid[1]);
    recvData.ReadByteSeq(npcGuid[7]);
    recvData.ReadByteSeq(npcGuid[4]);
    recvData.ReadByteSeq(npcGuid[0]);
    recvData.ReadByteSeq(npcGuid[6]);
    recvData.ReadByteSeq(npcGuid[2]);

    if (!GetPlayer()->IsAlive() || !CheckStableMaster(npcGuid) || newSlot > PET_SLOT_LAST_STABLE_SLOT)
    {
        SendStableResult(STABLE_ERR_STABLE);
        return;
    }

    PetStable* petStable = GetPlayer()->GetPetStable();
    PetStable::PetInfo const* petInfo = petStable ? petStable->GetPet(petNumber) : nullptr;
    CreatureTemplate const* creatureInfo = petInfo ? sObjectMgr->GetCreatureTemplate(petInfo->CreatureId) : nullptr;
    if (!creatureInfo || petInfo->Type != HUNTER_PET || !creatureInfo->IsTameable(true))
    {
        SendStableResult(STABLE_ERR_STABLE);
        return;
    }

    if (!creatureInfo->IsTameable(_player->CanTameExoticPets()) && newSlot <= PET_SLOT_LAST_ACTIVE_SLOT)
    {
        SendStableResult(STABLE_ERR_EXOTIC);
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    Pet* pet = _player->GetPet();

    // can't place in stable dead pet
    if (pet && pet->GetCharmInfo()->GetPetNumber() == petNumber && (!pet->IsAlive() || !pet->IsHunterPet()))
    {
        SendStableResult(STABLE_ERR_STABLE);
        return;
    }

    uint8 oldSlot = petInfo->Slot;
    if (oldSlot == newSlot)
    {
        SendStableResult(STABLE_SUCCESS_STABLE);
        return;
    }

    PetStable::PetInfo const* otherInfo = nullptr;
    if (newSlot <= PET_SLOT_LAST_ACTIVE_SLOT)
        otherInfo = petStable->GetActivePet(newSlot);
    else if (Optional<PetStable::PetInfo> const& stabled = petStable->StabledPets[newSlot - PET_SLOT_FIRST_STABLE_SLOT])
        otherInfo = &*stabled;

    uint32 otherNumber = otherInfo ? otherInfo->PetNumber : 0;

    // a summoned (or temporarily unsummoned) pet that moves is dismissed into its active slot first
    uint32 currentNumber = petStable->CurrentPet ? petStable->CurrentPet->PetNumber : 0;
    if (currentNumber && (currentNumber == petNumber || currentNumber == otherNumber))
        _player->RemovePet(pet, PET_SAVE_NOT_IN_SLOT);

    // take both pets out of their slots, then put each into its new one
    auto takePet = [petStable](uint32 number) -> Optional<PetStable::PetInfo>
    {
        Optional<PetStable::PetInfo> taken;
        auto unslotted = std::find_if(petStable->UnslottedPets.begin(), petStable->UnslottedPets.end(),
            [number](PetStable::PetInfo const& unslottedPet) { return unslottedPet.PetNumber == number; });
        if (unslotted != petStable->UnslottedPets.end())
        {
            taken = std::move(*unslotted);
            petStable->UnslottedPets.erase(unslotted);
            return taken;
        }

        for (Optional<PetStable::PetInfo>& stabled : petStable->StabledPets)
        {
            if (stabled && stabled->PetNumber == number)
            {
                taken = std::move(stabled);
                stabled.reset();
                return taken;
            }
        }

        return taken;
    };

    auto placePet = [this, petStable](PetStable::PetInfo&& movedPet, uint8 slot)
    {
        movedPet.Slot = slot;
        if (slot <= PET_SLOT_LAST_ACTIVE_SLOT)
        {
            _player->UpdatePetDBSlot(movedPet.PetNumber, PET_SAVE_NOT_IN_SLOT + slot);
            petStable->UnslottedPets.push_back(std::move(movedPet));
        }
        else
        {
            _player->UpdatePetDBSlot(movedPet.PetNumber, slot);
            petStable->StabledPets[slot - PET_SLOT_FIRST_STABLE_SLOT] = std::move(movedPet);
        }
    };

    Optional<PetStable::PetInfo> movedPet = takePet(petNumber);
    Optional<PetStable::PetInfo> otherPet = otherNumber ? takePet(otherNumber) : Optional<PetStable::PetInfo>();
    if (!movedPet)
    {
        if (otherPet)
            placePet(std::move(*otherPet), newSlot);

        SendStableResult(STABLE_ERR_STABLE);
        return;
    }

    placePet(std::move(*movedPet), newSlot);
    if (otherPet)
        placePet(std::move(*otherPet), oldSlot);

    WorldPacket data(SMSG_PET_SLOT_UPDATED, 4 + 4 + 4 + 4);
    data << int32(newSlot);
    data << uint32(petNumber);
    data << uint32(otherNumber);
    data << int32(oldSlot);
    SendPacket(&data);

    SendStableResult(STABLE_SUCCESS_STABLE);
}

void WorldSession::HandleRepairItemOpcode(WorldPacket& recvData)
{
    LOG_DEBUG("network", "WORLD: CMSG_REPAIR_ITEM");

    ObjectGuid npcGUID, itemGUID;
    uint8 guildBank;                                        // new in 2.3.2, bool that means from guild bank money

    recvData >> npcGUID >> itemGUID >> guildBank;

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(npcGUID, UNIT_NPC_FLAG_REPAIR);
    if (!unit)
    {
        LOG_DEBUG("network", "WORLD: HandleRepairItemOpcode - Unit ({}) not found or you can not interact with him.", npcGUID.ToString());
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    // reputation discount
    float discountMod = _player->GetReputationPriceDiscount(unit);

    sScriptMgr->OnPlayerBeforeDurabilityRepair(_player, npcGUID, itemGUID, discountMod, guildBank);

    if (itemGUID)
    {
        LOG_DEBUG("network", "ITEM: Repair item, item {}, npc {}", itemGUID.ToString(), npcGUID.ToString());

        Item* item = _player->GetItemByGuid(itemGUID);
        if (item)
            _player->DurabilityRepair(item->GetPos(), true, discountMod, guildBank);
    }
    else
    {
        LOG_DEBUG("network", "ITEM: Repair all items, npc {}", npcGUID.ToString());
        _player->DurabilityRepairAll(true, discountMod, guildBank);
    }
}
