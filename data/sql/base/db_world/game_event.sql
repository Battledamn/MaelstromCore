-- MySQL dump 10.13  Distrib 8.4.9, for Win64 (x86_64)
--
-- Host: 127.0.0.1    Database: acore_world_cata
-- ------------------------------------------------------
-- Server version	8.4.9

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `game_event`
--

DROP TABLE IF EXISTS `game_event`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `game_event` (
  `eventEntry` tinyint unsigned NOT NULL COMMENT 'Entry of the game event',
  `start_time` timestamp NULL DEFAULT '2000-01-01 18:00:00' COMMENT 'Absolute start date, the event will never start before',
  `end_time` timestamp NULL DEFAULT '2000-01-01 18:00:00' COMMENT 'Absolute end date, the event will never start after',
  `occurence` bigint unsigned NOT NULL DEFAULT '5184000' COMMENT 'Delay in minutes between occurences of the event',
  `length` bigint unsigned NOT NULL DEFAULT '2592000' COMMENT 'Length in minutes of the event',
  `holiday` int unsigned NOT NULL DEFAULT '0' COMMENT 'Client side holiday id',
  `holidayStage` tinyint unsigned NOT NULL DEFAULT '0',
  `description` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci DEFAULT NULL COMMENT 'Description of the event displayed in console',
  `world_event` tinyint unsigned NOT NULL DEFAULT '0' COMMENT '0 if normal event, 1 if world event',
  `announce` tinyint unsigned NOT NULL DEFAULT '2' COMMENT '0 dont announce, 1 announce, 2 value from config',
  PRIMARY KEY (`eventEntry`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `game_event`
--

LOCK TABLES `game_event` WRITE;
/*!40000 ALTER TABLE `game_event` DISABLE KEYS */;
INSERT INTO `game_event` VALUES
(1,'2017-06-21 00:01:00','2030-12-31 06:00:00',525600,20160,341,1,'Midsummer Fire Festival',0,2),
(2,'2016-12-15 06:00:00','2030-12-31 06:00:00',525600,25920,141,1,'Winter Veil',0,2),
(3,'2020-01-05 00:01:00','2030-01-01 06:00:00',43200,10078,479,2,'Darkmoon Faire',0,2),
(6,'2016-12-31 06:00:00','2030-12-31 06:00:00',525600,1440,0,0,'New Year\'s Eve',0,2),
(7,'2020-01-24 00:01:00','2030-12-31 06:00:00',525600,30240,327,1,'Lunar Festival',0,2),
(8,'2020-02-08 00:01:00','2030-12-31 06:00:00',525600,20160,423,1,'Love is in the Air',0,2),
(9,'2020-04-13 00:01:00','2030-12-31 06:00:00',525600,10080,181,1,'Noblegarden',0,2),
(10,'2020-05-01 00:01:00','2030-12-31 06:00:00',525600,10080,201,1,'Children\'s Week ',0,2),
(11,'2020-09-29 00:01:00','2030-12-31 06:00:00',525600,10080,321,1,'Harvest Festival',0,2),
(12,'2016-10-18 01:00:00','2030-12-31 06:00:00',525600,20160,324,1,'Hallow\'s End',0,2),
(13,NULL,NULL,525600,1,0,0,'Elemental Invasions',0,2),
(14,'2016-10-29 00:00:00','2030-12-31 06:00:00',10080,1440,0,0,'Stranglethorn Fishing Extravaganza Announce',0,2),
(15,'2016-10-30 14:00:00','2030-12-31 06:00:00',10080,120,301,0,'Stranglethorn Fishing Extravaganza Fishing Pools',0,2),
(16,'2016-10-29 03:00:00','2030-12-31 06:00:00',180,120,0,0,'Gurubashi Arena Booty Run',0,2),
(17,'2000-01-01 14:00:00','2000-01-01 14:00:00',525600,1,0,0,'Scourge Invasion',5,2),
(18,'2020-01-31 00:01:00','2030-01-01 06:00:00',80640,6240,283,0,'Call to Arms: Alterac Valley!',0,2),
(19,'2020-02-07 00:01:00','2030-01-01 06:00:00',80640,6240,284,0,'Call to Arms: Warsong Gulch!',0,2),
(20,'2020-01-17 00:01:00','2030-01-01 06:00:00',80640,6240,285,0,'Call to Arms: Arathi Basin!',0,2),
(21,'2020-01-24 00:01:00','2030-01-01 06:00:00',80640,6240,353,0,'Call to Arms: Eye of the Storm!',0,2),
(22,NULL,NULL,525600,1,0,0,'AQ War Effort',0,2),
(23,'2016-12-01 00:01:00','2030-12-31 06:00:00',43200,4320,374,1,'Darkmoon Faire Building',0,2),
(24,'2017-09-20 00:01:00','2030-12-31 06:00:00',525600,21600,372,2,'Brewfest',0,2),
(25,'2016-10-28 21:00:00','2030-12-31 06:00:00',1440,720,0,0,'Nights',0,2),
(26,'2020-11-23 01:00:00','2030-12-31 06:00:00',525600,10080,404,1,'Pilgrim\'s Bounty',0,2),
(27,'2008-03-24 06:00:00','2020-12-31 06:00:00',86400,21600,0,0,'Edge of Madness, Gri\'lek',0,2),
(28,'2008-04-07 07:00:00','2020-12-31 06:00:00',86400,21600,0,0,'Edge of Madness, Hazza\'rah',0,2),
(29,'2008-04-21 07:00:00','2020-12-31 06:00:00',86400,21600,0,0,'Edge of Madness, Renataki',0,2),
(30,'2008-05-05 07:00:00','2020-12-31 06:00:00',86400,21600,0,0,'Edge of Madness, Wushoolay',0,2),
(31,NULL,NULL,5184000,2592000,0,0,'Arena Tournament',0,2),
(32,'2014-09-19 00:00:01','2030-12-31 06:00:00',10080,5,0,0,'L70ETC Concert',0,2),
(33,'2011-03-22 00:10:00','2030-12-31 06:00:00',30,5,0,0,'Dalaran: Minigob',0,2),
(34,'2016-10-01 00:01:00','2030-12-31 06:00:00',525600,44640,0,0,'Brew of the Month October',0,2),
(35,'2016-11-01 00:01:00','2030-12-31 06:00:00',525600,43200,0,0,'Brew of the Month November',0,2),
(36,'2016-12-01 00:01:00','2030-12-31 06:00:00',525600,44640,0,0,'Brew of the Month December',0,2),
(37,'2017-01-01 00:01:00','2030-12-31 06:00:00',525600,44640,0,0,'Brew of the Month January',0,2),
(38,'2017-02-01 00:01:00','2030-12-31 06:00:00',525600,40320,0,0,'Brew of the Month February',0,2),
(39,'2017-03-01 00:01:00','2030-12-31 06:00:00',525600,44640,0,0,'Brew of the Month March',0,2),
(40,'2017-04-01 00:01:00','2030-12-31 06:00:00',525600,43200,0,0,'Brew of the Month April',0,2),
(41,'2017-05-01 00:01:00','2030-12-31 06:00:00',525600,44640,0,0,'Brew of the Month May',0,2),
(42,'2017-06-01 00:01:00','2020-12-31 06:00:00',525600,43200,0,0,'Brew of the Month June',0,2),
(43,'2017-07-01 00:01:00','2030-12-31 06:00:00',525600,44640,0,0,'Brew of the Month July',0,2),
(44,'2017-08-01 00:01:00','2030-12-31 06:00:00',525600,44640,0,0,'Brew of the Month August',0,2),
(45,'2017-09-01 00:01:00','2030-12-31 06:00:00',525600,44640,0,0,'Brew of the Month September',0,2),
(46,'2008-09-08 12:00:00','2008-09-24 12:00:00',1051897,23040,0,0,'Spirit of Competition',0,2),
(47,'2008-07-31 12:00:00','2008-08-05 12:00:00',5184000,7200,0,0,'BlizzCon 2007',0,2),
(48,NULL,NULL,5184000,2592000,0,0,'Wintergrasp Alliance Defence',5,2),
(49,NULL,NULL,5184000,2592000,0,0,'Wintergrasp Horde Defence',5,2),
(50,'2017-09-19 00:01:00','2030-12-31 06:00:00',525600,1440,398,1,'Pirates\' Day',0,2),
(51,'2016-11-01 00:01:00','2030-12-31 06:00:00',525600,2820,409,1,'Day of the Dead',0,2),
(52,'2010-12-25 06:00:00','2030-12-31 06:00:00',525600,11700,0,0,'Winter Veil: Gifts',0,2),
(53,'2020-02-14 00:01:00','2030-01-01 06:00:00',80640,6240,400,0,'Call to Arms: Strand of the Ancients!',0,2),
(54,'2020-02-21 00:01:00','2030-01-01 06:00:00',80640,6240,420,0,'Call to Arms: Isle of Conquest!',0,2),
(55,NULL,NULL,5184000,2592000,0,0,'Arena Season 3',0,2),
(56,NULL,NULL,5184000,2592000,0,0,'Arena Season 4',0,2),
(57,NULL,NULL,5184000,2592000,0,0,'Arena Season 5',0,2),
(58,NULL,NULL,5184000,2592000,0,0,'Arena Season 6',0,2),
(59,NULL,NULL,5184000,2592000,0,0,'Arena Season 7',0,2),
(60,NULL,NULL,5184000,2592000,0,0,'Arena Season 8',0,2),
(61,'2010-09-07 01:00:00','2010-10-10 01:00:00',9999999,47520,0,0,'Zalazane\'s Fall',0,2),
(62,'2016-10-30 14:00:00','2030-12-31 06:00:00',10080,180,376,1,'Stranglethorn Fishing Extravaganza Turn-ins',0,2),
(63,'2016-10-29 13:00:00','2030-12-31 06:00:00',10080,180,0,0,'Kalu\'ak Fishing Derby Turn-ins',0,2),
(64,'2016-10-29 14:00:00','2030-12-31 06:00:00',10080,60,424,0,'Kalu\'ak Fishing Derby Fishing Pools',0,2),
(65,'2008-01-02 15:55:00','2030-12-31 06:00:00',240,15,0,0,'Perry Gatner',0,2),
(66,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Venture Bay Horde Defence',5,2),
(67,'2010-01-02 00:40:00','2030-12-31 06:00:00',60,5,0,0,'AT Event Trigger (Tirion Speech)',0,2),
(68,'2010-01-02 00:55:00','2030-12-31 06:00:00',60,5,0,0,'AT Event Trigger (Horde Event)',0,2),
(69,'2010-01-02 00:10:00','2030-12-31 06:00:00',60,5,0,0,'AT Event Trigger (Alliance Event)',0,2),
(70,'2013-11-18 00:01:00','2013-12-01 23:59:00',525600,10080,0,0,'WoW\'s 9th Anniversary',0,2),
(71,'2013-01-06 01:01:00','2020-12-31 06:00:00',131040,8639,0,0,'Darkmoon Faire Building (Mulgore)',0,2),
(72,NULL,NULL,525600,1080,62,0,'Fireworks Spectacular',0,2),
(73,'2010-01-01 01:00:00','2025-01-01 01:00:00',60,1,0,0,'Hourly Bells',0,2),
(74,'2022-01-01 07:00:00','2030-01-01 08:00:00',1440,60,0,0,'Children of Goldshire',0,2),
(75,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Arena Season 1',0,2),
(76,'2018-01-01 07:00:00','2030-12-31 06:00:00',1440,60,0,0,'Childeren of Goldshire',0,2),
(77,NULL,NULL,1440,1440,0,0,'Cataclysm Pre-Launch Deathwing Event',0,2),
(78,NULL,NULL,180,180,0,0,'Dragonwrath Stormwind Event',0,2),
(79,NULL,NULL,180,180,0,0,'Dragonwrath Orgrimmar Event',0,2),
(80,NULL,NULL,1440,1440,0,0,'Nefarian\'s Head Stormwind',0,2),
(81,NULL,NULL,1440,1440,0,0,'Nefarian\'s Head Orgrimmar',0,2),
(82,'2020-01-03 00:01:00','2030-01-01 06:00:00',80640,6240,435,0,'Call to Arms: Battle for Gilneas!',0,2),
(83,'2020-01-10 00:01:00','2030-01-01 06:00:00',80640,6240,436,0,'Call to Arms: Twim Peaks!',0,2),
(85,'2011-03-22 01:00:00','2030-12-31 07:00:00',5184000,35,0,0,'Stitches Event',0,2),
(86,'2008-01-02 11:55:00','2030-12-31 06:00:00',240,15,0,0,'Perry Gatner',0,2),
(87,'2008-01-02 11:55:00','2030-12-31 06:00:00',5184000,90,0,0,'Scarlet Oracle',0,2),
(88,'2016-10-28 18:00:00','2030-12-30 23:00:00',1440,720,0,0,'Evening',0,2),
(89,'2016-10-28 20:00:00','2030-12-30 23:00:00',1440,600,0,0,'Leprithus',0,2),
(90,'2016-10-30 14:00:00','2030-12-31 07:00:00',10080,180,0,0,'Stranglethorn Fishing Extravaganza - Turn-ins',0,2),
(91,NULL,NULL,525600,4320,372,1,'Brewfest Building (Orgrimmar)',0,2),
(92,'2000-01-01 00:00:00',NULL,60,30,0,0,'What the Cold Wind Brings...',0,2),
(97,NULL,NULL,5184000,2592000,0,0,'Terokk Boss Event',0,2),
(98,NULL,NULL,5184000,15,0,0,'Steel Gate Gargoyle Attack',0,2),
(101,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Sun\'s Reach Reclamation Phase 1',5,2),
(102,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Sun\'s Reach Reclamation Phase 2 Only',5,2),
(103,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Sun\'s Reach Reclamation Phase 2 Permanent',5,2),
(104,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Sun\'s Reach Reclamation Phase No Portal',5,2),
(105,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Sun\'s Reach Reclamation Phase Portal',5,2),
(106,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Sun\'s Reach Reclamation Phase 3 Only',5,2),
(107,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Sun\'s Reach Reclamation Phase 3 Permanent',5,2),
(108,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Sun\'s Reach Reclamation Phase No Anvil',5,2),
(109,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Sun\'s Reach Reclamation Phase Anvil',5,2),
(110,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Sun\'s Reach Reclamation Phase 4',5,2),
(111,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Sun\'s Reach Reclamation Phase No Monument',5,2),
(112,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Sun\'s Reach Reclamation Phase Monument',5,2),
(113,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Sun\'s Reach Reclamation Phase No Alchemy Lab',5,2),
(114,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Sun\'s Reach Reclamation Phase Alchemy Lab',5,2),
(115,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Sun\'s Reach Reclamation Phase K\'iru',5,2),
(116,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'SWP - All Gates Closed',5,2),
(117,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'SWP - First Gate Open',5,2),
(118,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'SWP - Second Gate Open',5,2),
(119,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'SWP - All Gates Open',5,2),
(120,'2000-01-01 18:00:00','2000-01-01 18:00:00',5184000,2592000,0,0,'Scourge Invasion - Boss in instance activation',5,2),
(121,'2000-01-01 18:00:00','2000-01-01 18:00:00',5184000,2592000,0,0,'Scourge Invasion - Attacking Winterspring',5,2),
(122,'2000-01-01 18:00:00','2000-01-01 18:00:00',5184000,2592000,0,0,'Scourge Invasion - Attacking Tanaris',5,2),
(123,'2000-01-01 18:00:00','2000-01-01 18:00:00',5184000,2592000,0,0,'Scourge Invasion - Attacking Azshara',5,2),
(124,'2000-01-01 18:00:00','2000-01-01 18:00:00',5184000,2592000,0,0,'Scourge Invasion - Attacking Blasted Lands',5,2),
(125,'2000-01-01 18:00:00','2000-01-01 18:00:00',5184000,2592000,0,0,'Scourge Invasion - Attacking Eastern Plaguelands',5,2),
(126,'2000-01-01 18:00:00','2000-01-01 18:00:00',5184000,2592000,0,0,'Scourge Invasion - Attacking Burning Steppes',5,2),
(127,'2000-01-01 18:00:00','2000-01-01 18:00:00',5184000,2592000,0,0,'Scourge Invasion - 50 Invasions Done',5,2),
(128,'2000-01-01 18:00:00','2000-01-01 18:00:00',5184000,2592000,0,0,'Scourge Invasion - 100 Invasions Done',5,2),
(129,'2000-01-01 18:00:00','2000-01-01 18:00:00',5184000,2592000,0,0,'Scourge Invasion - 150 Invasions Done',5,2),
(130,'2000-01-01 18:00:00','2000-01-01 18:00:00',5184000,2592000,0,0,'Scourge Invasion - Invasions Done',5,2),
(131,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Bars Initial',5,2),
(132,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Bars Tier 1',5,2),
(133,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Bars Tier 2',5,2),
(134,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Bars Tier 3',5,2),
(135,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Bars Tier 4',5,2),
(136,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Bars Tier 5',5,2),
(137,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Herbs Initial',5,2),
(138,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Herbs Tier 1',5,2),
(139,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Herbs Tier 2',5,2),
(140,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Herbs Tier 3',5,2),
(141,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Herbs Tier 4',5,2),
(142,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Herbs Tier 5',5,2),
(143,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Skins Initial',5,2),
(144,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Skins Tier 1',5,2),
(145,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Skins Tier 2',5,2),
(146,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Skins Tier 3',5,2),
(147,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Skins Tier 4',5,2),
(148,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Skins Tier 5',5,2),
(149,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Bandages Initial',5,2),
(150,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Bandages Tier 1',5,2),
(151,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Bandages Tier 2',5,2),
(152,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Bandages Tier 3',5,2),
(153,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Bandages Tier 4',5,2),
(154,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Bandages Tier 5',5,2),
(155,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Cooked Goods Initial',5,2),
(156,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Cooked Goods Tier 1',5,2),
(157,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Cooked Goods Tier 2',5,2),
(158,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Cooked Goods Tier 3',5,2),
(159,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Cooked Goods Tier 4',5,2),
(160,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Alliance Cooked Goods Tier 5',5,2),
(161,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Bars Initial',5,2),
(162,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Bars Tier 1',5,2),
(163,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Bars Tier 2',5,2),
(164,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Bars Tier 3',5,2),
(165,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Bars Tier 4',5,2),
(166,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Bars Tier 5',5,2),
(167,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Herbs Initial',5,2),
(168,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Herbs Tier 1',5,2),
(169,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Herbs Tier 2',5,2),
(170,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Herbs Tier 3',5,2),
(171,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Herbs Tier 4',5,2),
(172,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Herbs Tier 5',5,2),
(173,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Skins Initial',5,2),
(174,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Skins Tier 1',5,2),
(175,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Skins Tier 2',5,2),
(176,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Skins Tier 3',5,2),
(177,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Skins Tier 4',5,2),
(178,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Skins Tier 5',5,2),
(179,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Bandages Initial',5,2),
(180,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Bandages Tier 1',5,2),
(181,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Bandages Tier 2',5,2),
(182,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Bandages Tier 3',5,2),
(183,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Bandages Tier 4',5,2),
(184,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Bandages Tier 5',5,2),
(185,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Cooked Goods Initial',5,2),
(186,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Cooked Goods Tier 1',5,2),
(187,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Cooked Goods Tier 2',5,2),
(188,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Cooked Goods Tier 3',5,2),
(189,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Cooked Goods Tier 4',5,2),
(190,'2000-01-01 14:00:00','2000-01-01 14:00:00',5184000,2592000,0,0,'Ahn\'Qiraj War Effort - Horde Cooked Goods Tier 5',5,2);
/*!40000 ALTER TABLE `game_event` ENABLE KEYS */;
UNLOCK TABLES;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed
