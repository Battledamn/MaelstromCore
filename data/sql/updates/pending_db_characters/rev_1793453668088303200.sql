-- 4.3.4 archaeology (ported from The Cataclysm Preservation Project): each character's dig sites,
-- artifacts in research and solved artifacts.
CREATE TABLE IF NOT EXISTS `character_archaeology_sites` (
  `guid` int unsigned NOT NULL,
  `site` tinyint unsigned NOT NULL COMMENT 'position 0-15: four per continent',
  `type` smallint unsigned NOT NULL COMMENT 'ResearchSite.dbc id',
  `finds` tinyint unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`site`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE IF NOT EXISTS `character_archaeology_projects` (
  `guid` int unsigned NOT NULL,
  `branch` smallint unsigned NOT NULL COMMENT 'ResearchBranch.dbc id',
  `project` smallint unsigned NOT NULL COMMENT 'ResearchProject.dbc id',
  PRIMARY KEY (`guid`,`branch`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE IF NOT EXISTS `character_archaeology_completed` (
  `guid` int unsigned NOT NULL,
  `project` smallint unsigned NOT NULL COMMENT 'ResearchProject.dbc id',
  `time` int unsigned NOT NULL COMMENT 'first solved',
  `count` int unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`guid`,`project`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
