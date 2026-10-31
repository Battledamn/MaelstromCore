-- .modify currency (4.x currencies): its own permission, for the role that has .modify money (197).
-- MaelstromCore's own permissions start at 3000.
DELETE FROM `rbac_linked_permissions` WHERE `linkedId` = 3000;
DELETE FROM `rbac_permissions` WHERE `id` = 3000;
INSERT INTO `rbac_permissions` (`id`, `name`) VALUES (3000, 'Command: modify currency');
INSERT INTO `rbac_linked_permissions` (`id`, `linkedId`) VALUES (197, 3000);
