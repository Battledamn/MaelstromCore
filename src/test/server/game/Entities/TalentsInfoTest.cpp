/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "IntegrationTestFixture.h"
#include "Player.h"
#include <gtest/gtest.h>

// 4.3.4 writes the primary talent tree (TalentTab.dbc id) before each spec's talent list;
// the 3.3.5a layout omitted it, so the client misread group, talents and glyphs (#171).
class TalentsInfoTest : public IntegrationTestFixture
{
};

TEST_F(TalentsInfoTest, WritesPrimaryTalentTreeBeforeTheTalentCount)
{
    TestPlayer* player = CreateTestPlayer();
    player->SetSpecsCount(1);
    player->SetActiveSpec(0);

    WorldPacket data;
    player->BuildPlayerTalentsInfoData(&data);

    uint32 unspentTalentPoints = 0;
    uint8 specsCount = 0;
    uint8 activeSpec = 0;
    uint32 primaryTalentTree = 1;
    uint8 talentCount = 1;
    uint8 glyphCount = 0;
    data >> unspentTalentPoints >> specsCount >> activeSpec >> primaryTalentTree >> talentCount >> glyphCount;

    EXPECT_EQ(specsCount, 1);
    EXPECT_EQ(activeSpec, 0);
    EXPECT_EQ(primaryTalentTree, 0u);                 // no tree chosen yet
    EXPECT_EQ(talentCount, 0u);
    EXPECT_EQ(glyphCount, uint8(MAX_GLYPH_SLOT_INDEX));
    EXPECT_EQ(data.rpos() + MAX_GLYPH_SLOT_INDEX * sizeof(uint16), data.size());
}

// The primary tree is the one the player chose (characters.talentTree), not derived from the points,
// and Cataclysm sends 9 glyph slots (3 prime, 3 major, 3 minor) per spec.
TEST_F(TalentsInfoTest, WritesTheChosenPrimaryTalentTreeAndNineGlyphs)
{
    TestPlayer* player = CreateTestPlayer();
    player->SetSpecsCount(2);
    player->SetActiveSpec(0);
    player->SetPrimaryTalentTree(0, 746);                 // Warrior Arms
    player->SetPrimaryTalentTree(1, 845);                 // Warrior Protection

    WorldPacket data;
    player->BuildPlayerTalentsInfoData(&data);

    uint32 unspentTalentPoints = 0;
    uint8 specsCount = 0;
    uint8 activeSpec = 0;
    data >> unspentTalentPoints >> specsCount >> activeSpec;
    ASSERT_EQ(specsCount, 2);

    uint32 const expectedTrees[] = { 746, 845 };
    for (uint32 expectedTree : expectedTrees)
    {
        uint32 primaryTalentTree = 0;
        uint8 talentCount = 1;
        uint8 glyphCount = 0;
        data >> primaryTalentTree >> talentCount >> glyphCount;
        EXPECT_EQ(primaryTalentTree, expectedTree);
        EXPECT_EQ(talentCount, 0u);
        EXPECT_EQ(glyphCount, 9u);
        data.read_skip(glyphCount * sizeof(uint16));
    }

    EXPECT_EQ(data.rpos(), data.size());
}
