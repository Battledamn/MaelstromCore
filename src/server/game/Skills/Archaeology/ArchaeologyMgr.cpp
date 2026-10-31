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

#include "ArchaeologyMgr.h"
#include "DatabaseEnv.h"
#include "DBCStores.h"
#include "Field.h"
#include "Log.h"
#include "QueryResult.h"
#include "Random.h"
#include "SharedDefines.h"
#include "Timer.h"
#include <algorithm>

bool ArchaeologyProject::IsRare() const
{
    return Rarity >= ITEM_QUALITY_RARE;
}

ArchaeologyMgr* ArchaeologyMgr::instance()
{
    static ArchaeologyMgr instance;
    return &instance;
}

void ArchaeologyMgr::LoadData()
{
    uint32 oldMSTime = getMSTime();

    _sites.clear();
    _branches.clear();
    _currencyToBranch.clear();
    _projects.clear();
    _commonProjects.clear();
    _rareProjects.clear();

    // each dig site's perimeter: the QuestPOIPoint rows of its blob, in id order
    std::unordered_map<uint32, std::vector<std::pair<float, float>>> blobPoints;
    for (QuestPOIPointEntry const* point : sQuestPOIPointStore)
        blobPoints[point->QuestPOIBlobID].emplace_back(float(point->X), float(point->Y));

    //                                                0   1      2         3         4         5
    QueryResult result = WorldDatabase.Query("SELECT Id, MapId, QuestPOI, ReqSkill, MinLevel, GameobjectEntry FROM archaeology_sites");
    if (!result)
    {
        LOG_WARN("server.loading", ">> Loaded 0 archaeology sites. DB table `archaeology_sites` is empty.");
        LOG_INFO("server.loading", " ");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        ArchaeologySite site;
        site.Id = fields[0].Get<uint16>();
        site.MapId = fields[1].Get<uint32>();
        uint32 blob = fields[2].Get<uint16>();
        site.ReqSkill = fields[3].Get<uint32>();
        site.MinLevel = fields[4].Get<uint32>();
        site.FindEntry = fields[5].Get<uint32>();

        if (!sResearchSiteStore.LookupEntry(site.Id))
        {
            LOG_ERROR("sql.sql", "Table `archaeology_sites`: site {} is not in ResearchSite.dbc, skipped.", site.Id);
            continue;
        }

        if (GetContinentForMap(site.MapId) == MAX_ARCHAEOLOGY_CONTINENTS)
        {
            LOG_ERROR("sql.sql", "Table `archaeology_sites`: site {} is on map {}, which has no dig sites, skipped.", site.Id, site.MapId);
            continue;
        }

        auto points = blobPoints.find(blob);
        if (points == blobPoints.end() || points->second.size() < 3)
        {
            LOG_ERROR("sql.sql", "Table `archaeology_sites`: site {} links to QuestPOI blob {}, which has no polygon in QuestPOIPoint.dbc, skipped.", site.Id, blob);
            continue;
        }

        site.Polygon = points->second;
        site.MinX = site.MaxX = site.Polygon.front().first;
        site.MinY = site.MaxY = site.Polygon.front().second;
        for (auto const& [x, y] : site.Polygon)
        {
            site.MinX = std::min(site.MinX, x);
            site.MaxX = std::max(site.MaxX, x);
            site.MinY = std::min(site.MinY, y);
            site.MaxY = std::max(site.MaxY, y);
        }

        _sites[site.Id] = std::move(site);
    } while (result->NextRow());

    LOG_INFO("server.loading", ">> Loaded {} archaeology sites in {} ms", _sites.size(), GetMSTimeDiffToNow(oldMSTime));

    oldMSTime = getMSTime();

    // the races: every branch with fragments, each with its own slot in the project field
    uint8 fieldSlot = 0;
    for (ResearchBranchEntry const* entry : sResearchBranchStore)
    {
        if (!entry->CurrencyID || fieldSlot >= ARCHAEOLOGY_MAX_PROJECTS)
            continue;

        ArchaeologyBranch branch;
        branch.Id = entry->ID;
        branch.CurrencyId = entry->CurrencyID;
        branch.KeystoneId = entry->ItemID;
        branch.FieldSlot = fieldSlot++;
        _branches[branch.Id] = branch;
        _currencyToBranch[branch.CurrencyId] = branch.Id;
    }

    //                                      0      1       2       3         4          5         6
    result = WorldDatabase.Query("SELECT Entry, Branch, Rarity, ReqSkill, Fragments, Keystone, Spell FROM archaeology_projects");
    if (result)
    {
        std::unordered_map<uint32, uint32> rareCount, epicCount;
        do
        {
            Field* fields = result->Fetch();

            ArchaeologyProject project;
            project.Id = fields[0].Get<uint16>();
            project.Branch = fields[1].Get<uint8>();
            project.Rarity = fields[2].Get<uint8>();
            project.ReqSkill = fields[3].Get<uint16>();
            project.Fragments = fields[4].Get<uint8>();
            project.Keystones = fields[5].Get<uint8>();
            project.Spell = fields[6].Get<uint32>();

            if (!sResearchProjectStore.LookupEntry(project.Id) || !_branches.count(project.Branch))
            {
                LOG_ERROR("sql.sql", "Table `archaeology_projects`: project {} (branch {}) is not in ResearchProject.dbc or its branch has no fragments, skipped.", project.Id, project.Branch);
                continue;
            }

            if (project.IsRare())
            {
                _rareProjects[project.Branch].push_back(project.Id);
                if (project.Rarity == ITEM_QUALITY_RARE)
                    ++rareCount[project.Branch];
                else
                    ++epicCount[project.Branch];
            }
            else
                _commonProjects[project.Branch].push_back(project.Id);

            _projects[project.Id] = project;
        } while (result->NextRow());

        // the chance of each rare artifact once a rare one is rolled (CPP): epics 5% each, the rares share the rest
        for (auto& [branchId, branch] : _branches)
            if (rareCount[branchId])
                branch.RareChance = std::min((1.0f - float(epicCount[branchId]) * ARCHAEOLOGY_EPIC_CHANCE) / float(rareCount[branchId]), ARCHAEOLOGY_MAX_RARE_CHANCE);
    }

    LOG_INFO("server.loading", ">> Loaded {} archaeology races and {} artifacts in {} ms", _branches.size(), _projects.size(), GetMSTimeDiffToNow(oldMSTime));
    LOG_INFO("server.loading", " ");
}

ArchaeologyContinent ArchaeologyMgr::GetContinentForMap(uint32 mapId)
{
    switch (mapId)
    {
        case 0:   return ARCHAEOLOGY_EASTERN_KINGDOMS;
        case 1:   return ARCHAEOLOGY_KALIMDOR;
        case 530: return ARCHAEOLOGY_OUTLAND;
        case 571: return ARCHAEOLOGY_NORTHREND;
        default:  return MAX_ARCHAEOLOGY_CONTINENTS;
    }
}

uint16 ArchaeologyMgr::PickSite(ArchaeologyContinent continent, uint32 skill, uint32 level, std::vector<uint16> const& taken) const
{
    std::vector<uint16> candidates;
    for (auto const& [id, site] : _sites)
    {
        if (GetContinentForMap(site.MapId) != continent || site.ReqSkill > skill || site.MinLevel > level)
            continue;

        if (std::find(taken.begin(), taken.end(), id) != taken.end())
            continue;

        candidates.push_back(id);
    }

    if (candidates.empty())
        return 0;

    return candidates[urand(0, candidates.size() - 1)];
}

ArchaeologySite const* ArchaeologyMgr::GetSite(uint16 siteId) const
{
    auto itr = _sites.find(siteId);
    return itr != _sites.end() ? &itr->second : nullptr;
}

static bool IsInPolygon(std::vector<std::pair<float, float>> const& polygon, float x, float y)
{
    bool inside = false;
    for (std::size_t i = 0, j = polygon.size() - 1; i < polygon.size(); j = i++)
    {
        auto const& [xi, yi] = polygon[i];
        auto const& [xj, yj] = polygon[j];
        if ((yi > y) != (yj > y) && x < (xj - xi) * (y - yi) / (yj - yi) + xi)
            inside = !inside;
    }
    return inside;
}

bool ArchaeologyMgr::RandomPointInSite(uint16 siteId, float& x, float& y) const
{
    ArchaeologySite const* site = GetSite(siteId);
    if (!site)
        return false;

    // own (CPP picks a point on a polygon edge): a random point inside the site's polygon
    for (uint32 attempt = 0; attempt < 100; ++attempt)
    {
        float px = frand(site->MinX, site->MaxX);
        float py = frand(site->MinY, site->MaxY);
        if (IsInPolygon(site->Polygon, px, py))
        {
            x = px;
            y = py;
            return true;
        }
    }

    // a sliver of a polygon: fall back to its first corner
    x = site->Polygon.front().first;
    y = site->Polygon.front().second;
    return true;
}

ArchaeologyBranch const* ArchaeologyMgr::GetBranch(uint32 branchId) const
{
    auto itr = _branches.find(branchId);
    return itr != _branches.end() ? &itr->second : nullptr;
}

ArchaeologyBranch const* ArchaeologyMgr::GetBranchForCurrency(uint32 currencyId) const
{
    auto itr = _currencyToBranch.find(currencyId);
    return itr != _currencyToBranch.end() ? GetBranch(itr->second) : nullptr;
}

ArchaeologyProject const* ArchaeologyMgr::GetProject(uint16 projectId) const
{
    auto itr = _projects.find(projectId);
    return itr != _projects.end() ? &itr->second : nullptr;
}

uint16 ArchaeologyMgr::PickProject(uint32 branchId, ArchaeologyCompletedMap const& completed, uint32 skill) const
{
    ArchaeologyBranch const* branch = GetBranch(branchId);
    if (!branch)
        return 0;

    static std::vector<uint16> const none;
    auto rareItr = _rareProjects.find(branchId);
    auto commonItr = _commonProjects.find(branchId);
    std::vector<uint16> const& rares = rareItr != _rareProjects.end() ? rareItr->second : none;
    std::vector<uint16> const& commons = commonItr != _commonProjects.end() ? commonItr->second : none;

    // a rare artifact comes up in proportion to the branch's share of rares (CPP), each weighted by
    // its own chance; solved rares and rares above the skill are out
    if (!rares.empty() && frand(0.0f, 1.0f) < float(rares.size()) / float(rares.size() + commons.size()))
    {
        std::vector<std::pair<uint16, float>> eligible;
        float total = 0.0f;
        for (uint16 projectId : rares)
        {
            ArchaeologyProject const* project = GetProject(projectId);
            if (project->ReqSkill > skill || completed.count(projectId))
                continue;

            float chance = project->Rarity == ITEM_QUALITY_RARE ? branch->RareChance : ARCHAEOLOGY_EPIC_CHANCE;
            eligible.emplace_back(projectId, chance);
            total += chance;
        }

        if (total > 0.0f)
        {
            float roll = frand(0.0f, total);
            for (auto const& [projectId, chance] : eligible)
            {
                if (roll < chance)
                    return projectId;
                roll -= chance;
            }
            return eligible.back().first;
        }
    }

    // a common artifact the skill allows, one not solved yet when there is one
    std::vector<uint16> unsolved, solved;
    for (uint16 projectId : commons)
    {
        if (GetProject(projectId)->ReqSkill > skill)
            continue;

        (completed.count(projectId) ? solved : unsolved).push_back(projectId);
    }

    std::vector<uint16> const& pool = !unsolved.empty() ? unsolved : solved;
    if (pool.empty())
        return 0;

    return pool[urand(0, pool.size() - 1)];
}
