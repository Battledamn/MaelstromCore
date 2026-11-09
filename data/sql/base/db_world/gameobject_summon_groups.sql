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
-- Table structure for table `gameobject_summon_groups`
--

DROP TABLE IF EXISTS `gameobject_summon_groups`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `gameobject_summon_groups` (
  `summonerId` int unsigned NOT NULL DEFAULT '0',
  `summonerType` tinyint unsigned NOT NULL DEFAULT '0',
  `groupId` tinyint unsigned NOT NULL DEFAULT '0',
  `entry` int unsigned NOT NULL DEFAULT '0',
  `position_x` float NOT NULL DEFAULT '0',
  `position_y` float NOT NULL DEFAULT '0',
  `position_z` float NOT NULL DEFAULT '0',
  `orientation` float NOT NULL DEFAULT '0',
  `rotation0` float NOT NULL DEFAULT '0',
  `rotation1` float NOT NULL DEFAULT '0',
  `rotation2` float NOT NULL DEFAULT '0',
  `rotation3` float NOT NULL DEFAULT '0',
  `respawnTime` int unsigned NOT NULL DEFAULT '120',
  `Comment` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT ''
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `gameobject_summon_groups`
--

LOCK TABLES `gameobject_summon_groups` WRITE;
/*!40000 ALTER TABLE `gameobject_summon_groups` DISABLE KEYS */;
INSERT INTO `gameobject_summon_groups` VALUES
(2289,1,0,2332,-14652.4,146.512,3.50118,0.349065,0,0,0.173648,0.984808,120,'Enticing Negolash - Barbequed Buzzard Wings'),
(2289,1,0,2332,-14652.9,146.867,2.50691,2.9496,0,0,0.995396,0.0958512,120,'Enticing Negolash - Barbequed Buzzard Wings'),
(2289,1,0,2332,-14653,146.524,2.35559,6.00393,0,0,-0.139173,0.990268,120,'Enticing Negolash - Barbequed Buzzard Wings'),
(2289,1,0,2332,-14653,145.138,2.65327,5.84685,0,0,-0.216439,0.976296,120,'Enticing Negolash - Barbequed Buzzard Wings'),
(2289,1,0,2332,-14654,147.306,2.46731,0.942477,0,0,0.45399,0.891007,120,'Enticing Negolash - Barbequed Buzzard Wings'),
(2289,1,0,2332,-14654.5,147.324,2.45636,1.81514,0,0,0.788011,0.615662,120,'Enticing Negolash - Barbequed Buzzard Wings'),
(2289,1,0,2332,-14654.7,146.142,2.08906,2.37364,0,0,0.927183,0.374608,120,'Enticing Negolash - Barbequed Buzzard Wings'),
(2289,1,0,2332,-14654.9,147.278,2.42524,5.88176,0,0,-0.199368,0.979925,120,'Enticing Negolash - Barbequed Buzzard Wings'),
(2289,1,0,2332,-14655.1,146.671,2.23095,2.65289,0,0,0.970295,0.241925,120,'Enticing Negolash - Barbequed Buzzard Wings'),
(2289,1,0,2332,-14655.3,147.802,2.63972,6.16101,0,0,-0.0610485,0.998135,120,'Enticing Negolash - Barbequed Buzzard Wings'),
(2289,1,0,2332,-14656.2,147.096,2.38777,0.174532,0,0,0.0871553,0.996195,120,'Enticing Negolash - Barbequed Buzzard Wings'),
(2289,1,0,2332,-14656.8,148.893,3.28866,5.09636,0,0,-0.559193,0.829038,120,'Enticing Negolash - Barbequed Buzzard Wings'),
(2289,1,0,2333,-14653,145.389,2.852,4.85202,0,0,-0.656058,0.75471,120,'Enticing Negolash - Stranglevine Wine'),
(2289,1,0,2333,-14655.7,148.978,4.0564,3.47321,0,0,-0.986285,0.16505,120,'Enticing Negolash - Stranglevine Wine'),
(2289,1,0,2333,-14656.4,147.595,3.12908,1.58825,0,0,0.71325,0.70091,120,'Enticing Negolash - Stranglevine Wine'),
(2289,1,0,2333,-14656.8,147.434,3.10207,3.01941,0,0,0.998135,0.0610518,120,'Enticing Negolash - Stranglevine Wine'),
(2289,1,0,2333,-14657.2,148.228,2.88632,1.53589,0,0,0.694658,0.71934,120,'Enticing Negolash - Stranglevine Wine'),
(2289,1,0,2562,-14652.4,145.753,3.25464,2.93214,0,0,0.994521,0.104536,120,'Enticing Negolash - Baked Bread'),
(2289,1,0,2562,-14653.8,146.204,2.14631,4.88692,0,0,-0.642787,0.766045,120,'Enticing Negolash - Baked Bread'),
(2289,1,0,2562,-14656.1,148.367,3.51564,5.63741,0,0,-0.317305,0.948324,120,'Enticing Negolash - Baked Bread'),
(30871,0,0,192946,571.367,-322.152,110.503,2.96704,0,0,0.996194,0.087165,7200,'Brigg Smallshanks - Quest Junk In My Trunk'),
(30871,0,0,192946,571.532,-321.752,110.711,5.044,0,0,-0.580703,0.814116,7200,'Brigg Smallshanks - Quest Junk In My Trunk'),
(30871,0,0,192946,572.012,-321.806,110.51,0.977383,0,0,0.469471,0.882948,7200,'Brigg Smallshanks - Quest Junk In My Trunk'),
(30871,0,0,192947,571.228,-322.263,110.315,5.49779,0,0,-0.382683,0.92388,7200,'Brigg Smallshanks - Quest Junk In My Trunk'),
(30871,0,0,192947,571.474,-322.023,110.322,2.35619,0,0,0.92388,0.382683,7200,'Brigg Smallshanks - Quest Junk In My Trunk'),
(30871,0,0,192947,571.649,-321.839,110.531,5.65487,0,0,-0.309016,0.951057,7200,'Brigg Smallshanks - Quest Junk In My Trunk'),
(30871,0,0,192947,571.791,-321.695,110.336,5.49779,0,0,-0.382683,0.92388,7200,'Brigg Smallshanks - Quest Junk In My Trunk'),
(30871,0,0,192948,571.728,-321.827,110.732,6.26573,0,0,-0.00872612,0.999962,7200,'Brigg Smallshanks - Quest Junk In My Trunk'),
(30871,0,0,192949,571.046,-322.154,110.6,0.994837,0,0,0.477159,0.878817,7200,'Brigg Smallshanks - Quest Junk In My Trunk'),
(30871,0,0,192949,571.21,-321.93,110.489,1.41372,0,0,0.649447,0.760406,7200,'Brigg Smallshanks - Quest Junk In My Trunk'),
(30871,0,0,192949,571.363,-322.444,110.497,0.0698117,0,0,0.0348988,0.999391,7200,'Brigg Smallshanks - Quest Junk In My Trunk'),
(30871,0,0,192949,571.591,-322.228,110.587,0.523598,0,0,0.258819,0.965926,7200,'Brigg Smallshanks - Quest Junk In My Trunk'),
(30871,0,0,192949,571.678,-321.49,110.48,0.645772,0,0,0.317305,0.948324,7200,'Brigg Smallshanks - Quest Junk In My Trunk'),
(30871,0,0,192949,571.842,-321.624,110.564,4.69494,0,0,-0.71325,0.70091,7200,'Brigg Smallshanks - Quest Junk In My Trunk'),
(30871,0,0,192950,571.137,-322.173,110.517,4.39823,0,0,-0.809016,0.587786,7200,'Brigg Smallshanks - Quest Junk In My Trunk'),
(30871,0,0,192950,571.265,-322.012,110.711,4.90438,0,0,-0.636078,0.771625,7200,'Brigg Smallshanks - Quest Junk In My Trunk'),
(30871,0,0,192950,571.327,-322.26,110.565,1.8675,0,0,0.803857,0.594823,7200,'Brigg Smallshanks - Quest Junk In My Trunk'),
(30871,0,0,192950,571.371,-322.319,110.642,1.18682,0,0,0.559193,0.829038,7200,'Brigg Smallshanks - Quest Junk In My Trunk'),
(30871,0,0,192950,571.498,-321.629,110.731,1.95477,0,0,0.829038,0.559193,7200,'Brigg Smallshanks - Quest Junk In My Trunk'),
(30871,0,0,192950,571.552,-322.062,110.53,0.95993,0,0,0.461748,0.887011,7200,'Brigg Smallshanks - Quest Junk In My Trunk'),
(30871,0,0,192950,571.88,-321.668,110.502,1.18682,0,0,0.559193,0.829038,7200,'Brigg Smallshanks - Quest Junk In My Trunk'),
(1557,1,0,1558,2485.39,22.1752,27.5909,4.20625,0,0,-0.861629,0.507539,120,'Lillith\'s Dinner Table - Candle of Beckoning'),
(3737,1,0,3743,-1271.31,-3013.2,71.9223,0.488691,0,0,0.241921,0.970296,120,'Bubbling Fissure - Fissure Plant'),
(3737,1,0,3743,-1272.33,-3010.5,71.3295,5.60251,0,0,-0.333807,0.942641,120,'Bubbling Fissure - Fissure Plant'),
(3737,1,0,3743,-1272.46,-3016.28,72.8214,0.59341,0,0,0.292371,0.956305,120,'Bubbling Fissure - Fissure Plant'),
(3737,1,0,3743,-1273.74,-3017.27,73.0493,3.78737,0,0,-0.948323,0.317306,120,'Bubbling Fissure - Fissure Plant'),
(3737,1,0,3743,-1274.77,-3008.18,71.5097,0,0,0,0,1,120,'Bubbling Fissure - Fissure Plant'),
(3737,1,0,3743,-1275.6,-3016.53,72.6837,3.50812,0,0,-0.983254,0.182238,120,'Bubbling Fissure - Fissure Plant'),
(3737,1,0,3743,-1277.43,-3009.51,71.5239,0,0,0,0,1,120,'Bubbling Fissure - Fissure Plant'),
(3737,1,0,3743,-1278.46,-3015.85,72.2563,0,0,0,0,1,120,'Bubbling Fissure - Fissure Plant'),
(3737,1,0,3743,-1279.01,-3010.59,71.5588,2.07694,0,0,0.861629,0.507539,120,'Bubbling Fissure - Fissure Plant'),
(3737,1,0,3743,-1279.74,-3013.78,71.5814,3.80482,0,0,-0.945518,0.325568,120,'Bubbling Fissure - Fissure Plant'),
(23921,0,0,186457,863.741,-4335.81,175.882,2.80998,0,0,0.986285,0.16505,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23921,0,0,186459,863.839,-4335.78,175.87,2.25147,0,0,0.902585,0.430512,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23921,0,0,186457,870.085,-4333.66,175.913,2.05949,0,0,0.857167,0.515038,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23921,0,0,186457,865.093,-4329.5,184.862,5.044,0,0,-0.580703,0.814116,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23921,0,0,186459,870.059,-4333.67,175.912,3.26377,0,0,-0.998135,0.0610518,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23921,0,0,186459,865.027,-4329.54,184.859,3.76991,0,0,-0.951056,0.309017,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23921,0,0,186457,868.802,-4339.79,182.212,1.93731,0,0,0.824125,0.566408,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23921,0,0,186459,868.747,-4339.82,182.209,4.06662,0,0,-0.894934,0.446199,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23922,0,0,186457,989.369,-4306.89,178.847,5.60251,0,0,-0.333807,0.942641,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23922,0,0,186459,989.394,-4306.69,178.823,1.44862,0,0,0.66262,0.748956,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23922,0,0,186457,990.37,-4312.48,169.811,6.14356,0,0,-0.0697556,0.997564,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23922,0,0,186457,984.746,-4311.37,170.311,0.925024,0,0,0.446198,0.894935,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23922,0,0,186459,986.956,-4317.4,175.751,5.49779,0,0,-0.382683,0.92388,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23922,0,0,186459,984.89,-4311.39,170.28,5.11382,0,0,-0.551936,0.833886,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23922,0,0,186457,987,-4317.33,175.794,5.28835,0,0,-0.477159,0.878817,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23922,0,0,186459,990.378,-4312.45,169.814,4.67748,0,0,-0.719339,0.694659,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23923,0,0,186457,1091.08,-4492.35,199.826,1.53589,0,0,0.694658,0.71934,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23923,0,0,186459,1090.99,-4492.31,199.843,4.34587,0,0,-0.824126,0.566406,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23923,0,0,186457,1082.45,-4486.05,197.245,2.26893,0,0,0.906307,0.422619,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23923,0,0,186457,1088.52,-4486.68,191.176,2.65289,0,0,0.970295,0.241925,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23923,0,0,186459,1082.47,-4486.01,197.243,3.75246,0,0,-0.953716,0.300708,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23923,0,0,186459,1088.55,-4486.6,191.189,5.79449,0,0,-0.241921,0.970296,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23923,0,0,186457,1084.6,-4491.49,190.98,1.23918,0,0,0.580703,0.814116,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23923,0,0,186459,1084.63,-4491.51,190.978,2.58308,0,0,0.961261,0.27564,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23924,0,0,186457,798.11,-4505.71,186.714,5.79449,0,0,-0.241921,0.970296,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23924,0,0,186459,798.052,-4505.68,186.727,0.209439,0,0,0.104528,0.994522,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23924,0,0,186457,793.686,-4502.09,187.292,1.53589,0,0,0.694658,0.71934,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23924,0,0,186457,794.305,-4506.92,196.746,0.733038,0,0,0.358368,0.93358,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23924,0,0,186459,793.655,-4502.07,187.294,2.26893,0,0,0.906307,0.422619,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23924,0,0,186459,794.087,-4507.06,196.769,6.05629,0,0,-0.113203,0.993572,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23924,0,0,186457,800.002,-4499.43,192.178,0.59341,0,0,0.292371,0.956305,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(23924,0,0,186459,800.015,-4499.37,192.14,5.23599,0,0,-0.5,0.866025,60,'Halgrind Torch Bunny - Mission: Eternal Flame'),
(24290,0,0,186457,423.099,-4608.35,247.876,0.610863,0,0,0.300705,0.953717,60,'New Agamand Plague Tank Bunny - Mission: Plague This!'),
(24290,0,0,186459,423.039,-4608.36,247.832,3.97935,0,0,-0.913545,0.406738,60,'New Agamand Plague Tank Bunny - Mission: Plague This!'),
(24290,0,0,186457,431.65,-4604.21,252.061,4.59022,0,0,-0.748956,0.66262,60,'New Agamand Plague Tank Bunny - Mission: Plague This!'),
(24290,0,0,186459,431.542,-4604.23,252.05,4.76475,0,0,-0.688354,0.725374,60,'New Agamand Plague Tank Bunny - Mission: Plague This!'),
(24290,0,1,186457,375.438,-4655.56,251.9,4.57276,0,0,-0.754709,0.656059,60,'New Agamand Plague Tank Bunny - Mission: Plague This!'),
(24290,0,1,186459,375.424,-4655.62,251.928,0.174532,0,0,0.0871553,0.996195,60,'New Agamand Plague Tank Bunny - Mission: Plague This!'),
(24290,0,1,186457,369.526,-4663.41,254.98,0.471238,0,0,0.233445,0.97237,60,'New Agamand Plague Tank Bunny - Mission: Plague This!'),
(24290,0,1,186459,369.434,-4663.53,254.965,3.9619,0,0,-0.91706,0.39875,60,'New Agamand Plague Tank Bunny - Mission: Plague This!'),
(24290,0,2,186457,357.424,-4498.92,250.582,4.7473,0,0,-0.694658,0.71934,60,'New Agamand Plague Tank Bunny - Mission: Plague This!'),
(24290,0,2,186459,357.5,-4499.03,250.514,4.72984,0,0,-0.700909,0.713251,60,'New Agamand Plague Tank Bunny - Mission: Plague This!'),
(24290,0,2,186457,352.185,-4490.65,253.284,2.91469,0,0,0.993571,0.113208,60,'New Agamand Plague Tank Bunny - Mission: Plague This!'),
(24290,0,2,186459,352.146,-4490.48,253.261,6.16101,0,0,-0.0610485,0.998135,60,'New Agamand Plague Tank Bunny - Mission: Plague This!'),
(24290,0,3,186457,371.951,-4565.31,250.922,5.77704,0,0,-0.25038,0.968148,60,'New Agamand Plague Tank Bunny - Mission: Plague This!'),
(24290,0,3,186459,371.923,-4565.43,250.925,3.35105,0,0,-0.994521,0.104536,60,'New Agamand Plague Tank Bunny - Mission: Plague This!'),
(24290,0,3,186457,362.21,-4566.04,254.082,0.418879,0,0,0.207911,0.978148,60,'New Agamand Plague Tank Bunny - Mission: Plague This!'),
(24290,0,3,186459,362.188,-4566.2,254.066,5.86431,0,0,-0.207911,0.978148,60,'New Agamand Plague Tank Bunny - Mission: Plague This!'),
(24290,0,4,186457,469.041,-4591.96,251.803,1.13446,0,0,0.537299,0.843392,60,'New Agamand Plague Tank Bunny - Mission: Plague This!'),
(24290,0,4,186459,469.198,-4591.82,251.922,5.98648,0,0,-0.147809,0.989016,60,'New Agamand Plague Tank Bunny - Mission: Plague This!'),
(24290,0,4,186457,473.667,-4583.1,255.491,0.872664,0,0,0.422618,0.906308,60,'New Agamand Plague Tank Bunny - Mission: Plague This!'),
(24290,0,4,186459,473.587,-4583.14,255.273,0.785397,0,0,0.382683,0.92388,60,'New Agamand Plague Tank Bunny - Mission: Plague This!'),
(603,2,1,194481,235.419,-138.526,409.567,0,0,0,0,1,0,'Flame Leviathan outro - Portal to Dalaran'),
(329,2,0,175966,3458.01,-3111.46,137.434,5.35816,0,0,-0.4462,0.8949,604800,'Enchanted Scarlet Thread - Archivist Galford room'),
(329,2,1,175966,3593.51,-3060.4,136.482,1.53589,0,0,0.6947,0.7193,604800,'Enchanted Scarlet Thread - Hall opposite Malor room'),
(329,2,2,175966,3614.64,-3124.92,137.012,3.4383,0,0,-0.989,0.1478,604800,'Enchanted Scarlet Thread - Malor room table'),
(329,2,3,175966,3625.63,-3123.89,135.665,5.77704,0,0,-0.2504,0.9681,604800,'Enchanted Scarlet Thread - Malor room back-left crate'),
(177673,1,0,177705,252.547,2963.69,1.64267,5.58505,0,0,-0.34202,0.939693,4,'Serpent Statue - Naga Beam'),
(29175,0,0,191301,2254.84,-5298.75,82.168,1.134,0,0,0.537102,0.843517,20,'Highlord Tirion Fordring - Holy Lightning'),
(29175,0,0,191301,2296.24,-5296.44,81.9964,5.3398,0,0,0.454395,-0.8908,20,'Highlord Tirion Fordring - Holy Lightning'),
(29175,0,0,191301,2314.29,-5261.78,83.1349,3.05822,0,0,0.999131,0.0416735,20,'Highlord Tirion Fordring - Holy Lightning'),
(29175,0,0,191301,2278.43,-5270.14,81.7247,0.70988,0,0,0.347534,0.937667,20,'Highlord Tirion Fordring - Holy Lightning');
/*!40000 ALTER TABLE `gameobject_summon_groups` ENABLE KEYS */;
UNLOCK TABLES;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed
