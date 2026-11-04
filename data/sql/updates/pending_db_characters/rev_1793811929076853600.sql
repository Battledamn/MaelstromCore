-- 4.3.4's pet stances are Assist (3), Defensive (1) and Passive (0), and its pet bar has Move To in Stay's place.
-- Pets used to be saved as Defensive, which also attacked the owner's targets: that is Assist now.
UPDATE `character_pet` SET `Reactstate` = 3 WHERE `Reactstate` IN (1, 2);
UPDATE `character_pet` SET `abdata` = CONCAT('7 2 7 1 7 4 ', SUBSTRING(`abdata`, 13)) WHERE `abdata` LIKE '7 2 7 1 7 0 %';
UPDATE `character_pet` SET `abdata` = CONCAT(SUBSTRING(`abdata`, 1, CHAR_LENGTH(`abdata`) - 12), '6 3 6 1 6 0 ')
    WHERE `abdata` LIKE '% 6 2 6 1 6 0 ';
