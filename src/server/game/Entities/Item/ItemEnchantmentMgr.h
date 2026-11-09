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

#ifndef _ITEM_ENCHANTMENT_MGR_H
#define _ITEM_ENCHANTMENT_MGR_H

#include "Common.h"

// `item_enchantment_template`.`type`: 4.3.4 uses the same entry ids for both kinds, each with its own rows
enum ItemRandomEnchantType : uint8
{
    ITEM_RANDOM_ENCHANT_PROPERTY  = 0, // item_template.RandomProperty, ench is an ItemRandomProperties.dbc id
    ITEM_RANDOM_ENCHANT_SUFFIX    = 1, // item_template.RandomSuffix, ench is an ItemRandomSuffix.dbc id
    MAX_ITEM_RANDOM_ENCHANT_TYPES = 2
};

void LoadRandomEnchantmentsTable();
uint32 GetItemEnchantMod(int32 entry, ItemRandomEnchantType type);
uint32 GenerateEnchSuffixFactor(uint32 item_id);
#endif
