-- Stormwind Royal Guard patrol (leader 10280010): two by two, leader front left
UPDATE `creature_formations` SET `dist` = 5, `angle` = 270 WHERE `leaderGUID` = 10280010 AND `memberGUID` = 10280023;
UPDATE `creature_formations` SET `dist` = 5, `angle` = 180 WHERE `leaderGUID` = 10280010 AND `memberGUID` = 10280024;
UPDATE `creature_formations` SET `dist` = 7, `angle` = 225 WHERE `leaderGUID` = 10280010 AND `memberGUID` = 10280025;