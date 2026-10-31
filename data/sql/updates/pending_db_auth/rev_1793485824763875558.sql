-- .modify currency moves from 3000 to 1000: the core's permissions are 1-1999 (AzerothCore's up to
-- 999, MaelstromCore's from 1000), 2000+ is for server owners and modules. Account grants follow.
DELETE FROM `rbac_linked_permissions` WHERE `linkedId` = 1000;
DELETE FROM `rbac_permissions` WHERE `id` = 1000;
INSERT INTO `rbac_permissions` (`id`, `name`) VALUES (1000, 'Command: modify currency');
INSERT INTO `rbac_linked_permissions` (`id`, `linkedId`) VALUES (197, 1000);
UPDATE `rbac_account_permissions` SET `permissionId` = 1000 WHERE `permissionId` = 3000;
DELETE FROM `rbac_linked_permissions` WHERE `linkedId` = 3000;
DELETE FROM `rbac_permissions` WHERE `id` = 3000;
