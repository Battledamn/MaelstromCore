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
-- Table structure for table `creature_text_options`
--

DROP TABLE IF EXISTS `creature_text_options`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `creature_text_options` (
  `CreatureID` int unsigned NOT NULL,
  `GroupID` tinyint unsigned NOT NULL,
  `OptionSetID` tinyint unsigned NOT NULL,
  PRIMARY KEY (`CreatureID`,`GroupID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `creature_text_options`
--

LOCK TABLES `creature_text_options` WRITE;
/*!40000 ALTER TABLE `creature_text_options` DISABLE KEYS */;
INSERT INTO `creature_text_options` VALUES
(11502,9,6),
(12018,2,6),
(15687,2,3),
(16061,1,4),
(17257,2,5),
(17767,1,2),
(17808,1,2),
(17842,1,2),
(17888,0,2),
(18805,2,7),
(18831,3,5),
(19044,3,5),
(19516,1,5),
(21212,6,1),
(21213,3,1),
(21214,5,1),
(21216,2,1),
(21216,5,1),
(22871,2,2),
(22887,2,3),
(22917,1,1),
(22947,3,1),
(22948,1,1),
(22949,3,1),
(22950,3,1),
(22951,3,1),
(22952,3,1),
(23418,2,1),
(23419,1,1),
(23420,1,1),
(24850,1,7),
(24882,6,7),
(25038,1,7),
(26630,1,1),
(26631,1,1),
(26632,1,1),
(26723,1,1),
(26731,1,1),
(26794,5,1),
(28684,1,1),
(29120,1,1),
(29304,1,1),
(29305,1,1),
(29306,1,1),
(36597,10,3),
(39746,1,1),
(39747,3,1),
(39751,2,1),
(39863,6,2);
/*!40000 ALTER TABLE `creature_text_options` ENABLE KEYS */;
UNLOCK TABLES;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed
