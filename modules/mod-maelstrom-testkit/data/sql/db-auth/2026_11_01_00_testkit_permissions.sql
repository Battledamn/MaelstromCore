-- mod-maelstrom-testkit: its GM commands, in the module range (2000+), for game masters (role 197)
DELETE FROM `rbac_linked_permissions` WHERE `linkedId` IN (2000, 2001, 2002, 2003);
DELETE FROM `rbac_permissions` WHERE `id` IN (2000, 2001, 2002, 2003);
INSERT INTO `rbac_permissions` (`id`, `name`) VALUES
(2000, 'Command: testkit'),
(2001, 'Command: testkit tree'),
(2002, 'Command: testkit ready'),
(2003, 'Command: testkit reset');
INSERT INTO `rbac_linked_permissions` (`id`, `linkedId`) VALUES
(197, 2000), (197, 2001), (197, 2002), (197, 2003);
