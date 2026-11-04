-- mod-maelstrom-testkit: .testkit glyph, for game masters (role 197)
DELETE FROM `rbac_linked_permissions` WHERE `linkedId` = 2004;
DELETE FROM `rbac_permissions` WHERE `id` = 2004;
INSERT INTO `rbac_permissions` (`id`, `name`) VALUES
(2004, 'Command: testkit glyph');
INSERT INTO `rbac_linked_permissions` (`id`, `linkedId`) VALUES
(197, 2004);
