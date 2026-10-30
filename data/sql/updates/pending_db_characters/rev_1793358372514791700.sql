-- Cataclysm primary talent tree per talent spec (TalentTab.dbc ID, space separated), as in CPP
ALTER TABLE `characters` ADD COLUMN `talentTree` VARCHAR(10) NOT NULL DEFAULT '0 0' AFTER `activeTalentGroup`;

-- Cataclysm has 9 glyph slots (3 prime, 3 major, 3 minor); 3.3.5a had 6
ALTER TABLE `character_glyphs`
    ADD COLUMN `glyph7` SMALLINT UNSIGNED DEFAULT '0' AFTER `glyph6`,
    ADD COLUMN `glyph8` SMALLINT UNSIGNED DEFAULT '0' AFTER `glyph7`,
    ADD COLUMN `glyph9` SMALLINT UNSIGNED DEFAULT '0' AFTER `glyph8`;
