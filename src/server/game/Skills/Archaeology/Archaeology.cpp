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

#include "Archaeology.h"
#include "ArchaeologyPackets.h"
#include "DatabaseEnv.h"
#include "Field.h"
#include "GameTime.h"
#include "Log.h"
#include "Map.h"
#include "Player.h"
#include "QueryResult.h"
#include "Random.h"
#include "ScriptMgr.h"
#include <algorithm>
#include <cmath>

// squared distances from the find, in yards
enum ArchaeologySurveyDistance
{
    SURVEY_DIST_FAR   = 100 * 100,
    SURVEY_DIST_MED   = 30 * 30,
    SURVEY_DIST_CLOSE = 10 * 10,
};

enum ArchaeologySurveyTool
{
    GO_SURVEY_TOOL_RED    = 206590,
    GO_SURVEY_TOOL_YELLOW = 206589,
    GO_SURVEY_TOOL_GREEN  = 204272,
};

constexpr uint32 SURVEY_TOOL_DURATION = 10;                 // seconds
constexpr uint32 FIND_DURATION        = MINUTE;
constexpr float  DIG_SPOT_CLEARANCE   = 2.0f;               // yards a surface may stand above the bare ground
constexpr uint32 DIG_SPOT_ATTEMPTS    = 30;

static GameObject* SummonFacing(Player* player, uint32 entry, float x, float y, float z, float angle, uint32 duration)
{
    return player->SummonGameObject(entry, x, y, z, angle, 0.0f, 0.0f, std::sin(angle / 2.0f), std::cos(angle / 2.0f), duration);
}

void Archaeology::LoadFromDB(PreparedQueryResult sites, PreparedQueryResult projects, PreparedQueryResult completedProjects)
{
    _loaded = true;
    if (!_player->HasSkill(SKILL_ARCHAEOLOGY))
        return;

    if (PreparedQueryResult result = sites)
    {
        do
        {
            Field* fields = result->Fetch();
            uint8 position = fields[0].Get<uint8>();
            if (position < ARCHAEOLOGY_MAX_SITES)
                SetSite(position, fields[1].Get<uint16>(), fields[2].Get<uint8>());
        } while (result->NextRow());
    }

    if (PreparedQueryResult result = completedProjects)
    {
        do
        {
            Field* fields = result->Fetch();
            ArchaeologyCompletedProject& completed = _completed[fields[0].Get<uint16>()];
            completed.FirstCompleted = fields[1].Get<uint32>();
            completed.Count = fields[2].Get<uint32>();
        } while (result->NextRow());
    }

    if (PreparedQueryResult result = projects)
    {
        do
        {
            Field* fields = result->Fetch();
            _projects[fields[0].Get<uint16>()] = fields[1].Get<uint16>();
        } while (result->NextRow());
    }

    RegenerateSites();
    VerifyProjects();
}

void Archaeology::SaveToDB(CharacterDatabaseTransaction trans)
{
    // saved with the character, like its other data: the whole set of sites or projects when one of
    // them changed, and each artifact solved since the last save
    ObjectGuid::LowType guid = _player->GetGUID().GetCounter();
    CharacterDatabasePreparedStatement* stmt = nullptr;

    if (_sitesChanged)
    {
        stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_ARCHAEOLOGY_SITES);
        stmt->SetData(0, guid);
        trans->Append(stmt);

        for (uint8 position = 0; position < ARCHAEOLOGY_MAX_SITES; ++position)
        {
            if (!_sites[position].Id)
                continue;

            stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHAR_ARCHAEOLOGY_SITE);
            stmt->SetData(0, guid);
            stmt->SetData(1, position);
            stmt->SetData(2, _sites[position].Id);
            stmt->SetData(3, _sites[position].Finds);
            trans->Append(stmt);
        }
        _sitesChanged = false;
    }

    if (_projectsChanged)
    {
        stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_ARCHAEOLOGY_PROJECTS);
        stmt->SetData(0, guid);
        trans->Append(stmt);

        for (auto const& [branchId, projectId] : _projects)
        {
            stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHAR_ARCHAEOLOGY_PROJECT);
            stmt->SetData(0, guid);
            stmt->SetData(1, uint16(branchId));
            stmt->SetData(2, projectId);
            trans->Append(stmt);
        }
        _projectsChanged = false;
    }

    for (uint16 projectId : _completedChanged)
    {
        ArchaeologyCompletedProject const& completed = _completed[projectId];
        stmt = CharacterDatabase.GetPreparedStatement(CHAR_REP_CHAR_ARCHAEOLOGY_COMPLETED);
        stmt->SetData(0, guid);
        stmt->SetData(1, projectId);
        stmt->SetData(2, completed.FirstCompleted);
        stmt->SetData(3, completed.Count);
        trans->Append(stmt);
    }
    _completedChanged.clear();
}

void Archaeology::DeleteFromDB(ObjectGuid::LowType guid, CharacterDatabaseTransaction trans)
{
    for (CharacterDatabaseStatements statement : { CHAR_DEL_CHAR_ARCHAEOLOGY_SITES, CHAR_DEL_CHAR_ARCHAEOLOGY_PROJECTS, CHAR_DEL_CHAR_ARCHAEOLOGY_COMPLETED })
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(statement);
        stmt->SetData(0, guid);
        trans->Append(stmt);
    }
}

void Archaeology::Learn()
{
    if (!_loaded)
        return;

    RegenerateSites();
    VerifyProjects();
}

void Archaeology::UnLearn()
{
    if (!_loaded)
        return;

    // the dig sites and artifacts in research go; the solved artifacts stay (CPP)
    for (uint8 i = 0; i < ARCHAEOLOGY_MAX_SITES; ++i)
        SetSite(i, 0, 0);

    std::vector<uint32> branches;
    for (auto const& [branchId, projectId] : _projects)
        branches.push_back(branchId);
    _projects.clear();
    for (uint32 branchId : branches)
        ShowProject(branchId);

    _sitesChanged = true;
    _projectsChanged = true;
}

void Archaeology::OnSkillChange()
{
    // the login re-learns the profession spells (and so touches the skill) before the sites are loaded;
    // LoadFromDB checks everything once they are
    if (!_loaded)
        return;

    RegenerateSites();
    VerifyProjects();
}

void Archaeology::OnCurrencyGained(uint32 currencyId)
{
    if (!_loaded || !_player->HasSkill(SKILL_ARCHAEOLOGY))
        return;

    if (ArchaeologyBranch const* branch = sArchaeologyMgr->GetBranchForCurrency(currencyId))
        if (!_projects.count(branch->Id))
            NewProject(branch->Id);
}

void Archaeology::SetSite(uint8 position, uint16 siteId, uint8 finds)
{
    DigSite& site = _sites[position];
    site.Id = siteId;
    site.Finds = finds;
    site.X = 0.0f;
    site.Y = 0.0f;

    if (siteId && !sArchaeologyMgr->RandomPointInSite(siteId, site.X, site.Y))
        site.Id = 0;

    _player->SetUInt16Value(PLAYER_FIELD_RESEARCH_SITE_1 + position / 2, position % 2, site.Id);
}

void Archaeology::RegenerateSites()
{
    uint32 skill = _player->GetSkillValue(SKILL_ARCHAEOLOGY);
    uint32 level = _player->GetLevel();

    // keep every site the player may still dig at; fill the rest of each continent's four
    for (uint8 continent = 0; continent < MAX_ARCHAEOLOGY_CONTINENTS; ++continent)
    {
        for (uint8 i = 0; i < ARCHAEOLOGY_SITES_PER_CONTINENT; ++i)
        {
            uint8 position = continent * ARCHAEOLOGY_SITES_PER_CONTINENT + i;
            DigSite const& site = _sites[position];

            ArchaeologySite const* entry = site.Id ? sArchaeologyMgr->GetSite(site.Id) : nullptr;
            bool valid = entry && ArchaeologyMgr::GetContinentForMap(entry->MapId) == continent && entry->ReqSkill <= skill &&
                entry->MinLevel <= level && site.Finds < ARCHAEOLOGY_FINDS_PER_SITE;

            if (!valid)
                ReplaceSite(position);
        }
    }
}

void Archaeology::ReplaceSite(uint8 position)
{
    uint8 continent = position / ARCHAEOLOGY_SITES_PER_CONTINENT;
    uint8 first = continent * ARCHAEOLOGY_SITES_PER_CONTINENT;
    uint16 oldSite = _sites[position].Id;

    // not one of the continent's other sites, and not the one just dug out unless nothing else is left
    std::vector<uint16> taken;
    for (uint8 i = first; i < first + ARCHAEOLOGY_SITES_PER_CONTINENT; ++i)
        if (_sites[i].Id)
            taken.push_back(_sites[i].Id);

    uint32 skill = _player->GetSkillValue(SKILL_ARCHAEOLOGY);
    uint32 level = _player->GetLevel();
    uint16 siteId = sArchaeologyMgr->PickSite(ArchaeologyContinent(continent), skill, level, taken);
    if (!siteId && oldSite)
    {
        taken.erase(std::remove(taken.begin(), taken.end(), oldSite), taken.end());
        siteId = sArchaeologyMgr->PickSite(ArchaeologyContinent(continent), skill, level, taken);
    }

    if (!siteId && !oldSite)
        return;

    SetSite(position, siteId, 0);
    _sitesChanged = true;
}

int32 Archaeology::GetNearestSite(float& distSq) const
{
    ArchaeologyContinent continent = ArchaeologyMgr::GetContinentForMap(_player->GetMapId());
    if (continent == MAX_ARCHAEOLOGY_CONTINENTS)
        return -1;

    int32 nearest = -1;
    for (uint8 i = 0; i < ARCHAEOLOGY_SITES_PER_CONTINENT; ++i)
    {
        uint8 position = continent * ARCHAEOLOGY_SITES_PER_CONTINENT + i;
        DigSite const& site = _sites[position];
        if (!site.Id)
            continue;

        float dx = _player->GetPositionX() - site.X;
        float dy = _player->GetPositionY() - site.Y;
        float dist = dx * dx + dy * dy;
        if (nearest < 0 || dist < distSq)
        {
            nearest = position;
            distSq = dist;
        }
    }

    return nearest;
}

bool Archaeology::IsDiggable(float x, float y) const
{
    // own: a find lies on open ground, not in or on a wall, building, rock or gameobject, nor under water
    Map* map = _player->GetMap();
    float ground = map->GetGridHeight(x, y);
    if (ground <= INVALID_HEIGHT)
        return true;                                        // no terrain data there: nothing to judge by

    // the highest surface over the spot, collision models and gameobjects included
    float top = map->GetHeight(_player->GetPhaseMask(), x, y, ground + 100.0f, true, 150.0f);
    if (top > ground + DIG_SPOT_CLEARANCE)
        return false;

    LiquidData const& liquid = map->GetLiquidData(_player->GetPhaseMask(), x, y, ground, _player->GetCollisionHeight(), {});
    return !(liquid.Status & MAP_LIQUID_STATUS_SWIMMING);
}

void Archaeology::Survey()
{
    float distSq = 0.0f;
    int32 position = GetNearestSite(distSq);
    if (position < 0)
        return;

    DigSite& site = _sites[position];

    // the spot is checked here, where the site's map is the player's: a blocked one is replaced by an
    // open spot of the same site
    if (!IsDiggable(site.X, site.Y))
    {
        for (uint32 attempt = 0; attempt < DIG_SPOT_ATTEMPTS; ++attempt)
        {
            float x, y;
            if (sArchaeologyMgr->RandomPointInSite(site.Id, x, y) && IsDiggable(x, y))
            {
                site.X = x;
                site.Y = y;
                break;
            }
        }

        float dx = _player->GetPositionX() - site.X;
        float dy = _player->GetPositionY() - site.Y;
        distSq = dx * dx + dy * dy;
    }

    // two yards in front of the player, on the ground there if it is in sight, else at the player's feet
    float o = _player->GetOrientation();
    float x = _player->GetPositionX() + std::cos(o) * 2.0f;
    float y = _player->GetPositionY() + std::sin(o) * 2.0f;
    float z = _player->GetPositionZ();
    float ground = _player->GetMapHeight(x, y, z + 5.0f);
    if (std::fabs(z - ground) >= 5.0f || !_player->IsWithinLOS(x, y, ground))
    {
        x = _player->GetPositionX();
        y = _player->GetPositionY();
        ground = z;
    }

    if (distSq > float(SURVEY_DIST_CLOSE))
    {
        // the survey tool points at the find, the closer the more exactly (CPP): red, yellow, green
        float angle = std::atan2(site.Y - _player->GetPositionY(), site.X - _player->GetPositionX());
        uint32 tool;
        if (distSq < float(SURVEY_DIST_MED))
        {
            tool = GO_SURVEY_TOOL_GREEN;
            angle += frand(-float(M_PI) / 12.0f, float(M_PI) / 12.0f);
        }
        else if (distSq < float(SURVEY_DIST_FAR))
        {
            tool = GO_SURVEY_TOOL_YELLOW;
            angle += frand(-float(M_PI) / 6.0f, float(M_PI) / 6.0f);
        }
        else
        {
            tool = GO_SURVEY_TOOL_RED;
            angle += frand(-float(M_PI) / 3.0f, float(M_PI) / 3.0f);
        }

        SummonFacing(_player, tool, x, y, ground, angle, SURVEY_TOOL_DURATION);
        return;
    }

    // found it: the race's find, then the next find of the site, or a new site after the third
    uint16 siteId = site.Id;
    GameObject* find = nullptr;
    if (ArchaeologySite const* entry = sArchaeologyMgr->GetSite(siteId))
        find = SummonFacing(_player, entry->FindEntry, x, y, ground, frand(0.0f, 2.0f * float(M_PI)), FIND_DURATION);

    if (++site.Finds >= ARCHAEOLOGY_FINDS_PER_SITE)
        ReplaceSite(uint8(position));
    else
        sArchaeologyMgr->RandomPointInSite(site.Id, site.X, site.Y);
    _sitesChanged = true;

    sScriptMgr->OnPlayerArchaeologyFind(_player, siteId, find);
}

void Archaeology::ShowProject(uint32 branchId)
{
    ArchaeologyBranch const* branch = sArchaeologyMgr->GetBranch(branchId);
    if (!branch)
        return;

    auto itr = _projects.find(branchId);
    uint16 projectId = itr != _projects.end() ? itr->second : 0;
    _player->SetUInt16Value(PLAYER_FIELD_RESEARCH_PROJECT_1 + branch->FieldSlot / 2, branch->FieldSlot % 2, projectId);
}

void Archaeology::NewProject(uint32 branchId)
{
    uint16 projectId = sArchaeologyMgr->PickProject(branchId, _completed, _player->GetSkillValue(SKILL_ARCHAEOLOGY));
    if (projectId)
        _projects[branchId] = projectId;
    else
        _projects.erase(branchId);

    _projectsChanged = true;
    ShowProject(branchId);
}

void Archaeology::VerifyProjects()
{
    // a race has an artifact in research once it has fragments; a solved rare one is replaced
    for (auto const& [branchId, branch] : sArchaeologyMgr->GetBranches())
    {
        auto itr = _projects.find(branchId);
        if (itr != _projects.end())
        {
            ArchaeologyProject const* project = sArchaeologyMgr->GetProject(itr->second);
            if (!project || project->Branch != branchId || (project->IsRare() && _completed.count(project->Id)))
                NewProject(branchId);
            else
                ShowProject(branchId);
        }
        else if (_player->GetCurrency(branch.CurrencyId, false))
            NewProject(branchId);
    }
}

bool Archaeology::HasProject(uint16 projectId) const
{
    ArchaeologyProject const* project = sArchaeologyMgr->GetProject(projectId);
    if (!project)
        return false;

    auto itr = _projects.find(project->Branch);
    return itr != _projects.end() && itr->second == projectId;
}

bool Archaeology::CanSolve(uint16 projectId) const
{
    if (!_solveData || !HasProject(projectId))
        return false;

    ArchaeologyProject const* project = sArchaeologyMgr->GetProject(projectId);
    ArchaeologyBranch const* branch = sArchaeologyMgr->GetBranch(project->Branch);
    ArchaeologySolveData const& data = *_solveData;

    if (data.CurrencyId != branch->CurrencyId)
        return false;

    if (data.KeystoneCount && (data.KeystoneId != branch->KeystoneId || data.KeystoneCount > project->Keystones))
        return false;

    // keystones stand in for 12 fragments each, the fragments pay the rest
    uint32 keystoneWeight = data.KeystoneCount * ARCHAEOLOGY_KEYSTONE_WEIGHT;
    uint32 fragmentsNeeded = project->Fragments > keystoneWeight ? project->Fragments - keystoneWeight : 0;
    if (data.CurrencyCount != fragmentsNeeded)
        return false;

    if (!_player->HasCurrency(data.CurrencyId, data.CurrencyCount))
        return false;

    return !data.KeystoneCount || _player->HasItemCount(data.KeystoneId, data.KeystoneCount);
}

void Archaeology::Solve(uint16 projectId)
{
    ArchaeologyProject const* project = sArchaeologyMgr->GetProject(projectId);
    if (!project || !_solveData)
        return;

    ArchaeologySolveData data = *_solveData;
    _solveData.reset();

    if (data.CurrencyCount)
        _player->ModifyCurrency(data.CurrencyId, -int32(data.CurrencyCount));
    if (data.KeystoneCount)
        _player->DestroyItemCount(data.KeystoneId, data.KeystoneCount, true);

    uint32 now = uint32(GameTime::GetGameTime().count());
    ArchaeologyCompletedProject& completed = _completed[projectId];
    if (!completed.Count)
        completed.FirstCompleted = now;
    ++completed.Count;
    _completedChanged.insert(projectId);

    _player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_ARCHAEOLOGY_PROJECTS, projectId, completed.Count == 1 ? 1 : 0);

    WorldPackets::Archaeology::ResearchComplete packet;
    packet.Research = WorldPackets::Archaeology::ResearchHistory(projectId, completed.Count, completed.FirstCompleted);
    _player->SendDirectMessage(packet.Write());

    sScriptMgr->OnPlayerArchaeologySolve(_player, projectId, completed.Count == 1);

    NewProject(project->Branch);
}

void Archaeology::SendResearchHistory() const
{
    WorldPackets::Archaeology::SetupResearchHistory packet;
    for (auto const& [projectId, completed] : _completed)
        packet.History.emplace_back(projectId, completed.Count, completed.FirstCompleted);

    _player->SendDirectMessage(packet.Write());
}
