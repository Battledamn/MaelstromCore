-- WP_START's first parameter is ForcedMovement (0 none, 1 walk, 2 run) since the walk/run rework; these paths were
-- written when it was a run flag (0 walk). With 0 the creature keeps its own mode and creatures run, so Brother Paxton
-- ran his 6-yard Northshire patrol at run speed. Their comments say walk: 1.
DELETE FROM `smart_scripts` WHERE `entryorguid` = 951 AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`event_param5`,`event_param6`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_param4`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(951,0,0,0,11,0,100,0,0,0,0,0,0,0,53,1,279895,1,0,0,0,0,0,0,0,0,0,0,0,0,'On Respawn - Self: Start path #279895, walk, repeat, Passive // ');
DELETE FROM `smart_scripts` WHERE `entryorguid` = -10277709 AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`event_param5`,`event_param6`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_param4`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(-10277709,0,0,1,63,0,100,0,0,0,0,0,0,0,53,1,524340,0,0,0,0,1,0,0,0,0,0,0,0,0,'When just created - Self: Start path #524340, walk, do not repeat, Passive // '),
(-10277709,0,1,0,61,0,100,0,0,0,0,0,0,0,116,20,0,0,0,0,0,1,0,0,0,0,0,0,0,0,' Linked - Sets corpse delay of Self to 20s // '),
(-10277709,0,2,0,40,0,100,0,25,524340,0,0,0,0,11,97013,0,0,0,0,0,1,0,0,0,0,0,0,0,0,'On wapoint 25 of path 524340 reached - Self: Cast spell 97013 on Self // ');
DELETE FROM `smart_scripts` WHERE `entryorguid` = -10255483 AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`event_param5`,`event_param6`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_param4`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(-10255483,0,0,0,63,0,100,0,0,0,0,0,0,0,53,1,2554640,0,0,0,0,1,0,0,0,0,0,0,0,0,'When just created - Self: Start path #2554640, walk, do not repeat, Passive // '),
(-10255483,0,1,0,40,0,100,0,5,2554640,0,0,0,0,41,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,'On wapoint 5 of path 2554640 reached - Self: Despawn instantly // ');
