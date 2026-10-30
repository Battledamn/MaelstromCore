/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * You may redistribute it and/or modify it under the terms of the GNU General Public License
 * version 2 or, at your option, any later version.
 */

#include "PartyPackets.h"
#include "Group.h"
#include "Pet.h"
#include "Player.h"
#include "SpellAuraDefines.h"
#include "SpellAuraEffects.h"
#include "SpellAuras.h"
#include "Vehicle.h"

WorldPacket const* WorldPackets::Party::PartyCommandResult::Write()
{
    _worldPacket << uint32(Command);
    _worldPacket << Name;
    _worldPacket << uint32(Result);
    _worldPacket << uint32(ResultData);
    _worldPacket << ResultGUID;

    return &_worldPacket;
}

void WorldPackets::Party::PartyInviteClient::Read()
{
    _worldPacket >> TargetCfgRealmID;
    _worldPacket >> ProposedRoles;

    TargetGUID[2] = _worldPacket.ReadBit();
    TargetGUID[7] = _worldPacket.ReadBit();
    uint32 targetRealmLen = _worldPacket.ReadBits(9);
    TargetGUID[3] = _worldPacket.ReadBit();
    uint32 targetNameLen = _worldPacket.ReadBits(10);
    for (uint8 index : { 5, 4, 6, 0, 1 })
        TargetGUID[index] = _worldPacket.ReadBit();

    for (uint8 index : { 4, 7, 6 })
        _worldPacket.ReadByteSeq(TargetGUID[index]);

    TargetName = _worldPacket.ReadString(targetNameLen);
    TargetRealm = _worldPacket.ReadString(targetRealmLen);

    for (uint8 index : { 1, 0, 5, 3, 2 })
        _worldPacket.ReadByteSeq(TargetGUID[index]);
}

WorldPacket const* WorldPackets::Party::PartyInvite::Write()
{
    _worldPacket.WriteBit(MustBeBNetFriend);
    _worldPacket.WriteBit(InviterGUID[0]);
    _worldPacket.WriteBit(InviterGUID[3]);
    _worldPacket.WriteBit(InviterGUID[2]);
    _worldPacket.WriteBit(CanAccept);
    _worldPacket.WriteBit(InviterGUID[6]);
    _worldPacket.WriteBit(InviterGUID[5]);
    _worldPacket.WriteBits(uint32(InviterRealmName.length()), 9);
    _worldPacket.WriteBit(InviterGUID[4]);
    _worldPacket.WriteBits(uint32(InviterName.length()), 7);
    _worldPacket.WriteBits(uint32(LfgSlots.size()), 24);
    _worldPacket.WriteBit(IsXRealm);
    _worldPacket.WriteBit(InviterGUID[1]);
    _worldPacket.WriteBit(InviterGUID[7]);
    _worldPacket.FlushBits();

    _worldPacket.WriteByteSeq(InviterGUID[1]);
    _worldPacket.WriteByteSeq(InviterGUID[4]);

    _worldPacket << uint32(Timestamp);
    _worldPacket << uint32(ProposedRoles);
    _worldPacket << uint32(LfgCompletedMask);

    for (uint8 index : { 6, 0, 2, 3 })
        _worldPacket.WriteByteSeq(InviterGUID[index]);

    for (uint32 lfgSlot : LfgSlots)
        _worldPacket << uint32(lfgSlot);

    _worldPacket.WriteByteSeq(InviterGUID[5]);
    _worldPacket.WriteString(InviterRealmName);
    _worldPacket.WriteByteSeq(InviterGUID[7]);
    _worldPacket.WriteString(InviterName);
    _worldPacket << uint32(InviterCfgRealmID);

    return &_worldPacket;
}

void WorldPackets::Party::PartyInviteResponse::Read()
{
    bool hasRolesDesired = _worldPacket.ReadBit();
    Accept = _worldPacket.ReadBit();

    if (hasRolesDesired)
        RolesDesired = _worldPacket.read<uint32>();
}

WorldPacket const* WorldPackets::Party::GroupDecline::Write()
{
    _worldPacket << Name;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Party::PartyUpdate::Write()
{
    _worldPacket << uint8(PartyFlags);
    _worldPacket << uint8(Subgroup);
    _worldPacket << uint8(Flags);
    _worldPacket << uint8(RolesAssigned);

    if (LfgInfo)
    {
        _worldPacket << uint8(LfgInfo->MyLfgFlags);
        _worldPacket << uint32(LfgInfo->LfgSlot);
        _worldPacket << uint8(LfgInfo->LfgAborted);
    }

    _worldPacket << PartyGUID;
    _worldPacket << uint32(SequenceNum);
    _worldPacket << uint32(PlayerList.size());

    for (PartyPlayerInfo const& playerInfo : PlayerList)
    {
        _worldPacket << playerInfo.Name;
        _worldPacket << playerInfo.Guid;
        _worldPacket << uint8(playerInfo.Connected);
        _worldPacket << uint8(playerInfo.Subgroup);
        _worldPacket << uint8(playerInfo.Flags);
        _worldPacket << uint8(playerInfo.RolesAssigned);
    }

    _worldPacket << LeaderGUID;

    if (!PlayerList.empty())
    {
        _worldPacket << uint8(LootSettings.LootMethod);
        _worldPacket << LootSettings.LootMaster;
        _worldPacket << uint8(LootSettings.LootThreshold);
        _worldPacket << uint8(DifficultySettings.DungeonDifficulty);
        _worldPacket << uint8(DifficultySettings.RaidDifficulty);
    }

    return &_worldPacket;
}

namespace
{
    void FillAuraStates(Unit* unit, uint64 mask, std::vector<WorldPackets::Party::PartyMemberAuraStates>& auras)
    {
        for (uint8 slot = 0; slot < MAX_AURAS_GROUP_UPDATE; ++slot)
        {
            if (!(mask & (uint64(1) << slot)))
                continue;

            WorldPackets::Party::PartyMemberAuraStates& aura = auras.emplace_back();
            AuraApplication const* aurApp = unit->GetVisibleAura(slot);
            if (!aurApp)
                continue;

            aura.SpellID = aurApp->GetBase()->GetId();
            aura.Flags = aurApp->GetFlags();
            if (aura.Flags & AFLAG_ANY_EFFECT_AMOUNT_SENT)
                for (uint8 effIndex = 0; effIndex < MAX_SPELL_EFFECTS; ++effIndex)
                    aura.Points.push_back(aurApp->GetBase()->GetEffect(effIndex) ?
                        aurApp->GetBase()->GetEffect(effIndex)->GetAmount() : 0);
        }
    }

    // A full state lists every visible aura; a partial one only the slots that changed.
    uint64 GetFullAuraMask(Unit* unit)
    {
        uint64 mask = 0;
        for (uint8 slot = 0; slot < MAX_AURAS_GROUP_UPDATE; ++slot)
            if (unit->GetVisibleAura(slot))
                mask |= uint64(1) << slot;
        return mask;
    }

    void WriteAuraStates(WorldPacket& packet, bool fullUpdate, uint64 mask, uint32 count,
        std::vector<WorldPackets::Party::PartyMemberAuraStates> const& auras)
    {
        packet << uint8(fullUpdate);
        packet << uint64(mask);
        packet << uint32(count);
        for (WorldPackets::Party::PartyMemberAuraStates const& aura : auras)
        {
            packet << uint32(aura.SpellID);
            packet << uint16(aura.Flags);
            for (int32 points : aura.Points)
                packet << int32(points);
        }
    }
}

void WorldPackets::Party::PartyMemberState::Initialize(Player* player)
{
    bool fullUpdate = GetOpcode() == SMSG_PARTY_MEMBER_FULL_STATE;
    ::Pet* pet = player->GetPet();
    MemberGuid = player->GetGUID();

    if (fullUpdate)
    {
        ChangeMask = GROUP_UPDATE_FULL;
        if (player->getPowerType() != POWER_MANA)
            ChangeMask |= GROUP_UPDATE_FLAG_POWER_TYPE;
        else
            ChangeMask &= ~GROUP_UPDATE_FLAG_POWER_TYPE;
        if (pet)
            ChangeMask |= GROUP_UPDATE_PET;
        else
            ChangeMask |= GROUP_UPDATE_FLAG_PET_AURAS;
        if (player->GetVehicle())
            ChangeMask |= GROUP_UPDATE_FLAG_VEHICLE_SEAT;
    }
    else
    {
        ChangeMask = player->GetGroupUpdateFlag();
        if (ChangeMask & GROUP_UPDATE_FLAG_POWER_TYPE)
            ChangeMask |= GROUP_UPDATE_FLAG_CUR_POWER | GROUP_UPDATE_FLAG_MAX_POWER;
        if (ChangeMask & GROUP_UPDATE_FLAG_PET_POWER_TYPE)
            ChangeMask |= GROUP_UPDATE_FLAG_PET_CUR_POWER | GROUP_UPDATE_FLAG_PET_MAX_POWER;
    }

    MemberStats.Status = MEMBER_STATUS_ONLINE;
    if (player->IsPvP())
        MemberStats.Status |= MEMBER_STATUS_PVP;
    if (!player->IsAlive())
        MemberStats.Status |= player->HasPlayerFlag(PLAYER_FLAGS_GHOST) ? MEMBER_STATUS_GHOST : MEMBER_STATUS_DEAD;
    if (player->IsFFAPvP())
        MemberStats.Status |= MEMBER_STATUS_PVP_FFA;
    if (player->isAFK())
        MemberStats.Status |= MEMBER_STATUS_AFK;
    if (player->isDND())
        MemberStats.Status |= MEMBER_STATUS_DND;

    Powers powerType = player->getPowerType();
    MemberStats.CurrentHealth = player->GetHealth();
    MemberStats.MaxHealth = player->GetMaxHealth();
    MemberStats.PowerType = uint8(powerType);
    MemberStats.CurrentPower = uint16(player->GetPower(powerType));
    MemberStats.MaxPower = uint16(player->GetMaxPower(powerType));
    MemberStats.Level = uint16(player->GetLevel());
    MemberStats.ZoneID = uint16(player->GetZoneId());
    MemberStats.PositionX = int16(player->GetPositionX());
    MemberStats.PositionY = int16(player->GetPositionY());
    MemberStats.PositionZ = int16(player->GetPositionZ());

    if (Vehicle* vehicle = player->GetVehicle())
        if (VehicleEntry const* vehicleInfo = vehicle->GetVehicleInfo())
            if (player->m_movementInfo.transport.seat >= 0 && player->m_movementInfo.transport.seat < MAX_VEHICLE_SEATS)
                MemberStats.VehicleSeat = vehicleInfo->m_seatID[player->m_movementInfo.transport.seat];

    MemberStats.AuraMask = fullUpdate ? GetFullAuraMask(player) : player->GetAuraUpdateMaskForRaid();
    FillAuraStates(player, MemberStats.AuraMask, MemberStats.Auras);

    // The core has no Cataclysm phase shift yet; the 3.3.5a phase mask has no wire equivalent here.
    MemberStats.PhaseShiftFlags = 0x08; // PhaseShiftFlags::Unphased

    if (pet)
    {
        PartyMemberPetStats& petStats = MemberStats.PetStats.emplace();
        Powers petPowerType = pet->getPowerType();
        petStats.GUID = pet->GetGUID();
        petStats.Name = pet->GetName();
        petStats.ModelId = int16(pet->GetDisplayId());
        petStats.CurrentHealth = pet->GetHealth();
        petStats.MaxHealth = pet->GetMaxHealth();
        petStats.PowerType = uint8(petPowerType);
        petStats.CurrentPower = uint16(pet->GetPower(petPowerType));
        petStats.MaxPower = uint16(pet->GetMaxPower(petPowerType));
        petStats.AuraMask = fullUpdate ? GetFullAuraMask(pet) : pet->GetAuraUpdateMaskForRaid();
        FillAuraStates(pet, petStats.AuraMask, petStats.Auras);
    }
}

WorldPacket const* WorldPackets::Party::PartyMemberState::Write()
{
    bool fullUpdate = GetOpcode() == SMSG_PARTY_MEMBER_FULL_STATE;
    if (fullUpdate)
        _worldPacket << uint8(ForEnemy);

    _worldPacket << MemberGuid.WriteAsPacked();
    _worldPacket << uint32(ChangeMask);

    if (ChangeMask & GROUP_UPDATE_FLAG_STATUS)
        _worldPacket << uint16(MemberStats.Status);
    if (ChangeMask & GROUP_UPDATE_FLAG_CUR_HP)
        _worldPacket << int32(MemberStats.CurrentHealth);
    if (ChangeMask & GROUP_UPDATE_FLAG_MAX_HP)
        _worldPacket << int32(MemberStats.MaxHealth);
    if (ChangeMask & GROUP_UPDATE_FLAG_POWER_TYPE)
        _worldPacket << uint8(MemberStats.PowerType);
    if (ChangeMask & GROUP_UPDATE_FLAG_CUR_POWER)
        _worldPacket << uint16(MemberStats.CurrentPower);
    if (ChangeMask & GROUP_UPDATE_FLAG_MAX_POWER)
        _worldPacket << uint16(MemberStats.MaxPower);
    if (ChangeMask & GROUP_UPDATE_FLAG_LEVEL)
        _worldPacket << uint16(MemberStats.Level);
    if (ChangeMask & GROUP_UPDATE_FLAG_ZONE)
        _worldPacket << uint16(MemberStats.ZoneID);
    if (ChangeMask & GROUP_UPDATE_FLAG_WMO_GROUP_ID)
        _worldPacket << int16(MemberStats.WmoGroupID);
    if (ChangeMask & GROUP_UPDATE_FLAG_POSITION)
        _worldPacket << int16(MemberStats.PositionX) << int16(MemberStats.PositionY) << int16(MemberStats.PositionZ);
    if (ChangeMask & GROUP_UPDATE_FLAG_AURAS)
        WriteAuraStates(_worldPacket, fullUpdate, MemberStats.AuraMask, MAX_AURAS_GROUP_UPDATE, MemberStats.Auras);

    // Unlike CPP, a pet flag without a pet still writes its (empty) field: the client reads every flagged field,
    // and the pet flags are raised exactly when the pet goes away.
    PartyMemberPetStats const noPet;
    PartyMemberPetStats const& pet = MemberStats.PetStats ? *MemberStats.PetStats : noPet;
    if (ChangeMask & GROUP_UPDATE_FLAG_PET_GUID)
        _worldPacket << pet.GUID;
    if (ChangeMask & GROUP_UPDATE_FLAG_PET_NAME)
        _worldPacket << pet.Name;
    if (ChangeMask & GROUP_UPDATE_FLAG_PET_MODEL_ID)
        _worldPacket << int16(pet.ModelId);
    if (ChangeMask & GROUP_UPDATE_FLAG_PET_CUR_HP)
        _worldPacket << int32(pet.CurrentHealth);
    if (ChangeMask & GROUP_UPDATE_FLAG_PET_MAX_HP)
        _worldPacket << int32(pet.MaxHealth);
    if (ChangeMask & GROUP_UPDATE_FLAG_PET_POWER_TYPE)
        _worldPacket << uint8(pet.PowerType);
    if (ChangeMask & GROUP_UPDATE_FLAG_PET_CUR_POWER)
        _worldPacket << uint16(pet.CurrentPower);
    if (ChangeMask & GROUP_UPDATE_FLAG_PET_MAX_POWER)
        _worldPacket << uint16(pet.MaxPower);
    if (ChangeMask & GROUP_UPDATE_FLAG_PET_AURAS)
        WriteAuraStates(_worldPacket, fullUpdate, pet.AuraMask, MemberStats.PetStats ? MAX_AURAS_GROUP_UPDATE : 0,
            pet.Auras);

    if (ChangeMask & GROUP_UPDATE_FLAG_VEHICLE_SEAT)
        _worldPacket << int32(MemberStats.VehicleSeat);

    if (ChangeMask & GROUP_UPDATE_FLAG_PHASE)
    {
        _worldPacket << uint32(MemberStats.PhaseShiftFlags);
        _worldPacket << uint32(MemberStats.Phases.size());
        for (uint16 phaseId : MemberStats.Phases)
            _worldPacket << uint16(phaseId);
    }

    return &_worldPacket;
}

void WorldPackets::Party::SetEveryoneIsAssistant::Read()
{
    EveryoneIsAssistant = _worldPacket.ReadBit();
}

void WorldPackets::Party::SetRole::Read()
{
    _worldPacket >> Role;

    for (uint8 index : { 2, 6, 3, 7, 5, 1, 0, 4 })
        ChangedUnit[index] = _worldPacket.ReadBit();

    for (uint8 index : { 6, 4, 1, 3, 0, 5, 2, 7 })
        _worldPacket.ReadByteSeq(ChangedUnit[index]);
}

WorldPacket const* WorldPackets::Party::RoleChangedInform::Write()
{
    _worldPacket.WriteBit(From[1]);
    _worldPacket.WriteBit(ChangedUnit[0]);
    _worldPacket.WriteBit(ChangedUnit[2]);
    _worldPacket.WriteBit(ChangedUnit[4]);
    _worldPacket.WriteBit(ChangedUnit[7]);
    _worldPacket.WriteBit(ChangedUnit[3]);
    _worldPacket.WriteBit(From[7]);
    _worldPacket.WriteBit(ChangedUnit[5]);
    _worldPacket.WriteBit(From[5]);
    _worldPacket.WriteBit(From[4]);
    _worldPacket.WriteBit(From[3]);
    _worldPacket.WriteBit(ChangedUnit[6]);
    _worldPacket.WriteBit(From[2]);
    _worldPacket.WriteBit(From[6]);
    _worldPacket.WriteBit(ChangedUnit[1]);
    _worldPacket.WriteBit(From[0]);
    _worldPacket.FlushBits();

    _worldPacket.WriteByteSeq(From[7]);
    _worldPacket.WriteByteSeq(ChangedUnit[3]);
    _worldPacket.WriteByteSeq(From[6]);
    _worldPacket.WriteByteSeq(ChangedUnit[4]);
    _worldPacket.WriteByteSeq(ChangedUnit[0]);
    _worldPacket << uint32(NewRole);
    _worldPacket.WriteByteSeq(ChangedUnit[5]);
    _worldPacket.WriteByteSeq(ChangedUnit[2]);
    _worldPacket.WriteByteSeq(From[0]);
    _worldPacket.WriteByteSeq(From[4]);
    _worldPacket.WriteByteSeq(ChangedUnit[1]);
    _worldPacket.WriteByteSeq(From[3]);
    _worldPacket.WriteByteSeq(From[5]);
    _worldPacket.WriteByteSeq(From[2]);
    _worldPacket.WriteByteSeq(ChangedUnit[6]);
    _worldPacket.WriteByteSeq(ChangedUnit[7]);
    _worldPacket.WriteByteSeq(From[1]);
    _worldPacket << uint32(OldRole);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Party::RolePollBegin::Write()
{
    for (uint8 index : { 1, 5, 7, 3, 2, 4, 0, 6 })
        _worldPacket.WriteBit(From[index]);
    _worldPacket.FlushBits();

    for (uint8 index : { 4, 7, 0, 5, 1, 6, 2, 3 })
        _worldPacket.WriteByteSeq(From[index]);

    return &_worldPacket;
}
