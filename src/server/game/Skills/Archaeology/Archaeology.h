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

#ifndef _ARCHAEOLOGY_H
#define _ARCHAEOLOGY_H

#include "ArchaeologyMgr.h"
#include "DatabaseEnvFwd.h"
#include "ObjectGuid.h"
#include <array>
#include <map>
#include <optional>
#include <set>

class Player;

// the fragments and keystones the client says a solve uses (cast flag 0x08)
struct ArchaeologySolveData
{
    uint32 CurrencyId = 0;
    uint32 CurrencyCount = 0;
    uint32 KeystoneId = 0;
    uint32 KeystoneCount = 0;
};

// One player's archaeology (CPP Archaeology, ported): four dig sites per continent, one artifact per
// race in research, and the solved artifacts. Sites and projects are shown to the client through
// PLAYER_FIELD_RESEARCH_SITE_1 and PLAYER_FIELD_RESEARCH_PROJECT_1.
class Archaeology
{
public:
    explicit Archaeology(Player* player) : _player(player) { }

    // at login, after the skills and currencies; the results of the login query holder
    void LoadFromDB(PreparedQueryResult sites, PreparedQueryResult projects, PreparedQueryResult completedProjects);
    void SaveToDB(CharacterDatabaseTransaction trans);
    static void DeleteFromDB(ObjectGuid::LowType guid, CharacterDatabaseTransaction trans);

    void Learn();
    void UnLearn();
    void OnSkillChange();                      // new continents and sites as the skill grows
    void OnCurrencyGained(uint32 currencyId);  // the first fragments of a race start its research

    void Survey();

    [[nodiscard]] bool HasProject(uint16 projectId) const;
    void SetSolveData(ArchaeologySolveData const& data) { _solveData = data; }
    [[nodiscard]] bool CanSolve(uint16 projectId) const;
    void Solve(uint16 projectId);

    void SendResearchHistory() const;

private:
    struct DigSite
    {
        uint16 Id = 0;
        uint8 Finds = 0;
        float X = 0.0f;
        float Y = 0.0f;
    };

    void SetSite(uint8 position, uint16 siteId, uint8 finds);
    void RegenerateSites();
    void ReplaceSite(uint8 position);
    [[nodiscard]] int32 GetNearestSite(float& distSq) const;
    [[nodiscard]] bool IsDiggable(float x, float y) const;

    void ShowProject(uint32 branchId);
    void NewProject(uint32 branchId);
    void VerifyProjects();

    Player* _player;
    std::array<DigSite, ARCHAEOLOGY_MAX_SITES> _sites{};
    std::map<uint32, uint16> _projects;        // branch -> artifact in research
    ArchaeologyCompletedMap _completed;
    std::optional<ArchaeologySolveData> _solveData;
    std::set<uint16> _completedChanged;        // solved since the last save
    bool _sitesChanged = false;
    bool _projectsChanged = false;
    bool _loaded = false;                      // LoadFromDB done: skill and currency changes before it are the login itself
};

#endif
