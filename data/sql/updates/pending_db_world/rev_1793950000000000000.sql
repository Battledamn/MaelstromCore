-- 4.3.4 uses the same item_enchantment_template entry ids for random properties (type 0) and random
-- suffixes (type 1), each with its own rows, as TDB 434's table does.
ALTER TABLE `item_enchantment_template`
    ADD COLUMN `type` tinyint unsigned NOT NULL DEFAULT '0' AFTER `entry`,
    DROP PRIMARY KEY,
    ADD PRIMARY KEY (`entry`, `type`, `ench`);
