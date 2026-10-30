-- spell_learn_spell (TrinityCore, CPP): learning `entry` also teaches `SpellID`. The 4.3.4 trainer's
-- Control Pet (79682) and Control Demon (80388) bring the passives that let the pet be commanded.
CREATE TABLE IF NOT EXISTS `spell_learn_spell` (
    `entry` int unsigned NOT NULL DEFAULT '0',
    `SpellID` int unsigned NOT NULL DEFAULT '0',
    `Active` tinyint unsigned NOT NULL DEFAULT '1',
    PRIMARY KEY (`entry`, `SpellID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='Spells learned with a spell';

DELETE FROM `spell_learn_spell` WHERE `entry` IN (79682, 80388);
INSERT INTO `spell_learn_spell` (`entry`, `SpellID`, `Active`) VALUES
(79682, 93321, 1),
(80388, 93375, 1);
