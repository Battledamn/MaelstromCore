-- Paladin starter quests (Practice using Judgement): the spell that hits the Training Dummy is
-- Judgement, not Seal of Righteousness, so the spell-hit quest credit must key on Judgement.
UPDATE `quest_template` SET `RequiredSpell` = 20271 WHERE `ID` IN (10069,24528,26918,26966,27023);
