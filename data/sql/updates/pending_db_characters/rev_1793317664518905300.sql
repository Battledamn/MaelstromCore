-- 4.3.4 class power slots: power1..power7 used to hold one column per WotLK power type
-- (mana, rage, focus, energy, happiness, rune, runic power). They now hold the class's
-- ChrClassesXPowerTypes slots (pinned TrinityCore-Cata), so move each class's powers over.
-- MySQL applies the assignments left to right, so every source column is read before it is reset.
UPDATE `characters` SET
    `power1` = CASE `class` WHEN 1 THEN `power2` WHEN 3 THEN `power3` WHEN 4 THEN `power4` WHEN 6 THEN `power7` ELSE `power1` END,
    `power2` = CASE `class` WHEN 11 THEN `power2` ELSE 0 END,
    `power3` = CASE `class` WHEN 11 THEN `power4` ELSE 0 END,
    `power4` = 0,
    `power5` = 0,
    `power6` = 0,
    `power7` = 0;
