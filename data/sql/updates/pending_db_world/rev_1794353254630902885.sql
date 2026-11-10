-- Northshire's non-human class chain (29078-29083: every race but Human) carried Death Knight on five steps and Warrior
-- on Fear No Evil, so no class could finish it. It is the race fallback of the per-class human chains: any class.
UPDATE `quest_template_addon` SET `AllowableClasses` = 0 WHERE `ID` BETWEEN 29078 AND 29083;
