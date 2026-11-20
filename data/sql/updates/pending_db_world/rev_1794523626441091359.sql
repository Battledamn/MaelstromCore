-- Lou's Parting Thoughts (26232): the thugs' meeting spot, the thugs and the trigger that runs their
-- conversation had no scripts, so "Eavesdrop on Thugs" could never complete.
DELETE FROM `areatrigger_scripts` WHERE `entry` = 5988;
INSERT INTO `areatrigger_scripts` (`entry`,`ScriptName`) VALUES
(5988,'at_westfall_two_shoed_lou_thugs');
UPDATE `creature_template` SET `ScriptName` = 'npc_westfall_thug' WHERE `entry` = 42387;
UPDATE `creature_template` SET `ScriptName` = 'npc_westfall_lous_parting_thoughts_trigger' WHERE `entry` = 42562;
