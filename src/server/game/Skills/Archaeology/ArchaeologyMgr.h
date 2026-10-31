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

#ifndef _ARCHAEOLOGYMGR_H
#define _ARCHAEOLOGYMGR_H

#include "Define.h"
#include <map>
#include <unordered_map>
#include <vector>

// 4.3.4 archaeology, ported from The Cataclysm Preservation Project (Skills/Archaeology), rewritten
// against AzerothCore. Dig sites come from archaeology_sites (TDB 434) and their QuestPOI polygon in
// QuestPOIPoint.dbc; races (branches) from ResearchBranch.dbc; artifacts from archaeology_projects.

enum ArchaeologyContinent : uint8
{
    ARCHAEOLOGY_EASTERN_KINGDOMS = 0,
    ARCHAEOLOGY_KALIMDOR         = 1,
    ARCHAEOLOGY_OUTLAND          = 2,
    ARCHAEOLOGY_NORTHREND        = 3,
    MAX_ARCHAEOLOGY_CONTINENTS   = 4
};

constexpr uint32 ARCHAEOLOGY_SITES_PER_CONTINENT = 4;
constexpr uint32 ARCHAEOLOGY_MAX_SITES           = ARCHAEOLOGY_SITES_PER_CONTINENT * MAX_ARCHAEOLOGY_CONTINENTS; // PLAYER_FIELD_RESEARCH_SITE_1, 8 x 2
constexpr uint32 ARCHAEOLOGY_MAX_PROJECTS        = 16;  // PLAYER_FIELD_RESEARCH_PROJECT_1, 8 x 2
constexpr uint32 ARCHAEOLOGY_FINDS_PER_SITE      = 3;
constexpr uint32 ARCHAEOLOGY_KEYSTONE_WEIGHT     = 12;  // fragments a keystone stands in for
constexpr float  ARCHAEOLOGY_EPIC_CHANCE         = 0.05f;
constexpr float  ARCHAEOLOGY_MAX_RARE_CHANCE     = 0.2f;
constexpr uint32 SPELL_ARCHAEOLOGY_SURVEY        = 80451;

struct ArchaeologySite
{
    uint16 Id = 0;
    uint32 MapId = 0;
    uint32 ReqSkill = 0;
    uint32 MinLevel = 0;
    uint32 FindEntry = 0;                      // the race's find gameobject
    std::vector<std::pair<float, float>> Polygon;
    float MinX = 0.0f, MinY = 0.0f, MaxX = 0.0f, MaxY = 0.0f;
};

struct ArchaeologyBranch
{
    uint32 Id = 0;
    uint32 CurrencyId = 0;                     // fragments
    uint32 KeystoneId = 0;                     // keystone item, 0 for none
    uint8 FieldSlot = 0;                       // its slot in PLAYER_FIELD_RESEARCH_PROJECT_1
    float RareChance = 0.0f;
};

struct ArchaeologyProject
{
    uint16 Id = 0;
    uint32 Branch = 0;
    uint8 Rarity = 0;
    uint32 ReqSkill = 0;
    uint32 Fragments = 0;
    uint8 Keystones = 0;
    uint32 Spell = 0;

    [[nodiscard]] bool IsRare() const;
};

struct ArchaeologyCompletedProject
{
    uint32 FirstCompleted = 0;
    uint32 Count = 0;
};

typedef std::map<uint16, ArchaeologyCompletedProject> ArchaeologyCompletedMap;

class AC_GAME_API ArchaeologyMgr
{
public:
    static ArchaeologyMgr* instance();

    void LoadData();

    [[nodiscard]] static ArchaeologyContinent GetContinentForMap(uint32 mapId);

    // a random site of the continent the player may dig at that none of `taken` holds, 0 if none
    [[nodiscard]] uint16 PickSite(ArchaeologyContinent continent, uint32 skill, uint32 level, std::vector<uint16> const& taken) const;
    [[nodiscard]] ArchaeologySite const* GetSite(uint16 siteId) const;
    bool RandomPointInSite(uint16 siteId, float& x, float& y) const;

    [[nodiscard]] std::map<uint32, ArchaeologyBranch> const& GetBranches() const { return _branches; }
    [[nodiscard]] ArchaeologyBranch const* GetBranch(uint32 branchId) const;
    [[nodiscard]] ArchaeologyBranch const* GetBranchForCurrency(uint32 currencyId) const;
    [[nodiscard]] ArchaeologyProject const* GetProject(uint16 projectId) const;

    // a new artifact for the branch: sometimes a rare one not yet solved, otherwise a common one
    // (preferably one not solved yet); 0 if the skill allows none
    [[nodiscard]] uint16 PickProject(uint32 branchId, ArchaeologyCompletedMap const& completed, uint32 skill) const;

private:
    ArchaeologyMgr() = default;
    ~ArchaeologyMgr() = default;

    std::unordered_map<uint16, ArchaeologySite> _sites;
    std::map<uint32, ArchaeologyBranch> _branches;
    std::unordered_map<uint32, uint32> _currencyToBranch;
    std::unordered_map<uint16, ArchaeologyProject> _projects;
    std::unordered_map<uint32, std::vector<uint16>> _commonProjects;
    std::unordered_map<uint32, std::vector<uint16>> _rareProjects;
};

#define sArchaeologyMgr ArchaeologyMgr::instance()

#endif
