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
-- Table structure for table `spell_bonus_data`
--

DROP TABLE IF EXISTS `spell_bonus_data`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `spell_bonus_data` (
  `entry` int unsigned NOT NULL DEFAULT '0',
  `direct_bonus` float NOT NULL DEFAULT '0',
  `dot_bonus` float NOT NULL DEFAULT '0',
  `ap_bonus` float NOT NULL DEFAULT '0',
  `ap_dot_bonus` float NOT NULL DEFAULT '0',
  `comments` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  PRIMARY KEY (`entry`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `spell_bonus_data`
--

LOCK TABLES `spell_bonus_data` WRITE;
/*!40000 ALTER TABLE `spell_bonus_data` DISABLE KEYS */;
INSERT INTO `spell_bonus_data` VALUES
(339,0,0.1,0,0,'Druid - Entangling Roots'),
(703,0,0,0,0.07,'Rogue - Garrote'),
(779,0,0,0.123,0,'Druid - Swipe (Bear)'),
(845,0,0,0.45,0,'Warrior - Cleave'),
(879,0,0,0,0,'Paladin - Exorcism'),
(1822,0,0,0.147,0.147,'Druid Rake (Cat Form)'),
(1978,0,0,0,0.08,'Hunter - Serpent Sting($RAP*0.2 / number of ticks)'),
(2812,0.61,0,0,0,'Paladin - Holy Wrath'),
(3044,0,0,0.0483,0,'Hunter - Arcane Shot'),
(3110,0.657,0,0,0,'Warlock - Firebolt'),
(3674,0,0,0,0.0665,'Hunter - Black Arrow ($RAP*0.1 / number of ticks)'),
(3716,0.512,0,0,0,'Warlock - Torment'),
(6360,0.85,0,0,0,'Warlock - Whiplash'),
(7001,0,0.308,0,0,'Priest - Lightwell Renew (4.3.4 tooltip: $SP*.308 per tick)'),
(7814,0.612,0,0,0,'Warlock - Lash of Pain'),
(9007,0,0,0,0.03,'Druid - Pounce Bleed (Triggered)'),
(13797,0,0,0,0.02,'Hunter - Immolation Trap($RAP*0.1 / number of ticks)'),
(13812,0,0,0,0.0546,'Hunter - Explosive Trap Effect'),
(19306,0,0,0.2,0,'Hunter - Counterattack'),
(19434,0,0,0.724,0,'Hunter - Aimed Shot'),
(20153,0.8,0,0,0,'Warlock - Immolation (Infernal), our estimate'),
(20167,0,0,0,0,'Paladin - Seal of Insight heal (spell_pal_seal_of_insight)'),
(20187,0,0,0,0,'Paladin - Judgement of Righteousness'),
(22568,0,0,0,0,'Druid - Ferocious Bite'),
(24640,0,0,0,0.014,'Hunter - Scorpid Poison'),
(25742,0,0,0,0,'Paladin - Seal of Righteousness Dummy Proc'),
(30213,0.528,0,0,0,'Warlock - Legion Strike (tooltip)'),
(31803,0,0,0,0,'Paladin - Censure (spell_pal_seal_of_truth)'),
(31804,0,0,0,0,'Paladin - Judgement of Truth'),
(33745,0,0,0.0552,0.00369,'Druid - Lacerate ($AP*0.05/number of ticks)'),
(45462,0,0,0,0,'Death Knight - Plague Strike'),
(45477,0,0,0.2,0,'Death Knight - Icy Touch'),
(45902,0,0,0,0,'Death Knight - Blood Strike'),
(47632,0,0,0,0,'Death Knight - Death Coil (Damage)'),
(47633,0,0,0,0,'Death Knight - Death Coil (Heal)'),
(48181,0.697,0,0,0,'Warlock - Haunt (description variable)'),
(48721,0,0,0.08,0,'Death Knight - Blood Boil'),
(48982,0,0,0,0,'Death Knight - Rune Tap'),
(49143,0,0,0,0,'Death Knight - Frost Strike'),
(49184,0,0,0.44,0,'Death Knight - Howling Blast'),
(49966,0,0,0.2,0,'Hunter - Smack (Pet)'),
(50401,0,0,0,0,'Death Knight - Razor Frost'),
(50536,0,0,0,0,'Death Knight - Unholy Blight (Rank 1)'),
(51963,0.453,0,0,0,'Death Knight - Gargoyle Strike (Gargoyle)'),
(52212,0,0,0.064,0,'Death Knight - Death and Decay'),
(53209,0,0,0.732,0,'Hunter - Chimera Shot (CPP)'),
(53301,0,0,0,0.273,'Hunter - Explosive Shot'),
(53351,0,0,0.45,0,'Hunter - Kill Shot'),
(54049,1.228,0,0,0,'Warlock - Shadow Bite'),
(54158,0,0,0,0,'Paladin - Judgement'),
(55050,0,0,0,0,'Death Knight - Heart Strike'),
(55078,0,0,0,0,'Death Knight - Blood Plague'),
(55095,0,0,0,0,'Death Knight - Frost Fever'),
(56641,0,0,0.021,0,'Hunter - Steady Shot'),
(56815,0,0,0.1,0,'Death Knight - Rune Strike'),
(56903,0,0,0,0,'Death Knight - Lichflame'),
(58621,0,0,0.08,0,'Death Knight - Glyph of Chains of Ice'),
(59754,0,0,0,0,'Death Knight - Glyph of Rune Tap (Triggered)'),
(59881,0,0,0.0175,0.0175,'Hunter - Rake'),
(60089,0,0,0.108,0,'Druid - Faerie Fire (feral)'),
(61193,0,0,0.04,0.04,'Hunter - Spirit Strike'),
(62036,0,0,0.1,0,'Death Knight - Rune Strike'),
(66217,0,0,0.05,0.05,'Death Knight - Rune Strike Off-Hand'),
(70890,0,0,0,0,'Death Knight - Scourge Strike (Shadow)'),
(77758,0,0,0.0982,0.0167,'Druid - Trash (Bear Form)'),
(77767,0,0,0.017,0,'Hunter - Cobra Shot'),
(80003,0,0,0,0.023,'Hunter - Black Arrow'),
(82928,0,0,0.724,0,'Hunter - Aimed Shot!'),
(88453,0,0,0,0.08,'Hunter - Serpent Sting'),
(88466,0,0,0,0.08,'Hunter - Serpent Sting'),
(89753,0.462,0,0,0,'Warlock - Felstorm (tooltip)'),
(95714,0,0,0.288,0,'Hunter - Burrow Attack (Pet)'),
(95725,0,0,0.288,0,'Hunter - Froststorm Breath (Pet)'),
(95933,0,0,0.1115,0,'Hunter - Black Arrow'),
(101423,0,0,0,0,'Seal of Righteousness Dummy Proc AOE');
/*!40000 ALTER TABLE `spell_bonus_data` ENABLE KEYS */;
UNLOCK TABLES;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed
