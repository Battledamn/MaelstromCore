-- Fear No Evil (28813): nothing handles the click spell 93072 (a dummy) or casts the 93097 the soldier waited for, and the
-- credit, face and say targeted an owner a world spawn does not have. Use SmartAI's spellclick event, credit the clicker,
-- run (ForcedMovement 2) along the escape path.
DELETE FROM `smart_scripts` WHERE `entryorguid` = 50047 AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`event_param5`,`event_param6`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_param4`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(50047,0,0,1,25,0,100,0,0,0,0,0,0,0,11,53034,0,0,0,0,0,1,0,0,0,0,0,0,0,0,'Injured Stormwind Infantry - On Reset - Cast \'Set Health Random\' spell'),
(50047,0,1,0,61,0,100,512,0,0,0,0,0,0,82,16777216,0,0,0,0,0,1,0,0,0,0,0,0,0,0,'Injured Stormwind Infantry - On Reset - Add npc_spellclick_spells flag'),
(50047,0,2,3,73,0,100,513,0,0,0,0,0,0,83,16777216,0,0,0,0,0,1,0,0,0,0,0,0,0,0,'Injured Stormwind Infantry - On Spellclick - Remove npc_spellclick_spells flag'),
(50047,0,3,4,61,0,100,512,0,0,0,0,0,0,33,50047,0,0,0,0,0,7,0,0,0,0,0,0,0,0,'Injured Stormwind Infantry - On Spellclick - Give Quest Credit to clicker'),
(50047,0,4,5,61,0,100,512,0,0,0,0,0,0,80,5004700,0,0,0,0,0,1,0,0,0,0,0,0,0,0,'Injured Stormwind Infantry - On Spellclick - Start timed event'),
(50047,0,5,7,61,0,100,512,0,0,0,0,0,0,18,33554432,0,0,0,0,0,1,0,0,0,0,0,0,0,0,'Injured Stormwind Infantry - On Spellclick - Add unit_flag not selectable'),
(50047,0,6,0,58,0,100,512,9,50047,0,0,0,0,41,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,'Injured Stormwind Infantry - On WP end - Despawn'),
(50047,0,7,0,61,0,100,512,0,0,0,0,0,0,11,93097,2,0,0,0,0,1,0,0,0,0,0,0,0,0,'Injured Stormwind Infantry - On Spellclick - Cast \'Renewed Life\' heal on self');
DELETE FROM `smart_scripts` WHERE `entryorguid` = 5004700 AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`event_param5`,`event_param6`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_param4`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(5004700,9,0,0,0,0,100,1,1000,1000,0,0,0,0,91,7,0,0,0,0,0,1,0,0,0,0,0,0,0,0,'Injured Stormwind Infantry - Actionlist - Stand up'),
(5004700,9,1,0,0,0,100,0,1200,1200,0,0,0,0,66,0,0,0,0,0,0,7,0,0,0,0,0,0,0,0,'Injured Stormwind Infantry - Actionlist - Face Player'),
(5004700,9,2,0,0,0,100,0,0,0,0,0,0,0,10,4,5,22,25,0,0,1,0,0,0,0,0,0,0,0,'Injured Stormwind Infantry - Actionlist - Random Emote'),
(5004700,9,3,0,0,0,100,1,200,200,0,0,0,0,1,0,0,0,0,0,0,7,0,0,0,0,0,0,0,0,'Injured Stormwind Infantry - Actionlist - Say line 0 (random)'),
(5004700,9,4,0,0,0,100,0,2000,2000,0,0,0,0,59,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,'Injured Stormwind Infantry - Actionlist - Set Run'),
(5004700,9,5,0,0,0,100,0,0,0,0,0,0,0,53,2,50047,0,0,0,0,1,0,0,0,0,0,0,0,0,'Injured Stormwind Infantry - Actionlist - Start WP movement (run)');
