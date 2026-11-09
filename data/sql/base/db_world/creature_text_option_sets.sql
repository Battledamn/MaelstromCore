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
-- Table structure for table `creature_text_option_sets`
--

DROP TABLE IF EXISTS `creature_text_option_sets`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `creature_text_option_sets` (
  `SetID` tinyint unsigned NOT NULL,
  `Cooldown` int unsigned NOT NULL DEFAULT '0' COMMENT 'Group cooldown in ms before it can fire again',
  `TriggerChance` tinyint unsigned NOT NULL DEFAULT '100' COMMENT '0-100 pct chance to fire at all',
  `PlayerOnly` tinyint unsigned NOT NULL DEFAULT '0' COMMENT 'Only fire if target is a player',
  `comment` varchar(255) COLLATE utf8mb4_unicode_ci DEFAULT '',
  PRIMARY KEY (`SetID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `creature_text_option_sets`
--

LOCK TABLES `creature_text_option_sets` WRITE;
/*!40000 ALTER TABLE `creature_text_option_sets` DISABLE KEYS */;
INSERT INTO `creature_text_option_sets` VALUES
(1,6000,100,0,'Standard kill yell: 6s cooldown'),
(2,6000,100,1,'Player-only kill yell: 6s cooldown'),
(3,5000,100,1,'Short cooldown player-only kill yell: 5s cooldown'),
(4,0,30,0,'Chance-based: 30% trigger, no cooldown'),
(5,5000,100,0,'Standard kill yell: 5s cooldown'),
(6,0,25,1,'Chance-based: 25% trigger, player-only'),
(7,0,50,1,'Chance-based: 50% trigger, player-only');
/*!40000 ALTER TABLE `creature_text_option_sets` ENABLE KEYS */;
UNLOCK TABLES;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed
