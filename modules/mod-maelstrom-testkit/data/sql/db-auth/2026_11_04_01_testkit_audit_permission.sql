-- mod-maelstrom-testkit: .testkit audit, for game masters (role 197)
DELETE FROM `rbac_linked_permissions` WHERE `linkedId` = 2005;
DELETE FROM `rbac_permissions` WHERE `id` = 2005;
INSERT INTO `rbac_permissions` (`id`, `name`) VALUES
(2005, 'Command: testkit audit');
INSERT INTO `rbac_linked_permissions` (`id`, `linkedId`) VALUES
(197, 2005);
