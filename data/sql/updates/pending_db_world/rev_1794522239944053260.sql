-- Livin' the Life (26228): Two-Shoed Lou's Old House (57761) summons the crate, the actors and the
-- dialogue pings that TDB's SmartAI already drives; the spells themselves had no scripts.
DELETE FROM `spell_script_names` WHERE `spell_id` IN (79262,79273,79275,79279,79283,79284,79287,79290,79292,79294,79297);
INSERT INTO `spell_script_names` (`spell_id`,`ScriptName`) VALUES
(79262,'spell_westfall_summon_lous_house'),
(79273,'spell_westfall_despawn_jangolode_actor'),
(79275,'spell_westfall_quest_credit_jangolode_event'),
(79279,'spell_westfall_livin_the_life_ping_figure'),
(79283,'spell_westfall_livin_the_life_ping_figure'),
(79284,'spell_westfall_livin_the_life_ping_figure'),
(79287,'spell_westfall_livin_the_life_ping_figure'),
(79290,'spell_westfall_livin_the_life_ping_glubtok'),
(79292,'spell_westfall_livin_the_life_ping_glubtok'),
(79294,'spell_westfall_livin_the_life_ping_glubtok'),
(79297,'spell_westfall_livin_the_life_ping_glubtok');
