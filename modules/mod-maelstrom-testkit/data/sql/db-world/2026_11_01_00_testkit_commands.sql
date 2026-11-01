-- mod-maelstrom-testkit: help for its GM commands
DELETE FROM `command` WHERE `name` IN ('testkit', 'testkit tree', 'testkit ready', 'testkit reset');
INSERT INTO `command` (`name`, `security`, `help`) VALUES
('testkit', 2, 'Syntax: .testkit $subcommand\nClass test helpers (mod-maelstrom-testkit): tree, ready, reset.'),
('testkit tree', 2, 'Syntax: .testkit tree #number\nLearns every talent of talent tree #number (1-3, the talent window\'s tabs from the left) at its highest rank and makes it the active spec\'s specialization.'),
('testkit ready', 2, 'Syntax: .testkit ready\nFull health and power, no cooldowns or Forbearance, and five training dummies in front of you.'),
('testkit reset', 2, 'Syntax: .testkit reset\nResets the active spec\'s talents and specialization.');
