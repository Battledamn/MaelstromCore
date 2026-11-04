-- mod-maelstrom-testkit: help for .testkit audit
DELETE FROM `command` WHERE `name` IN ('testkit audit', 'testkit cast');
INSERT INTO `command` (`name`, `security`, `help`) VALUES
('testkit cast', 2, 'Syntax: .testkit cast #spell [triggered]\nYou cast it: at yourself if it is helpful, otherwise at your selection.'),
('testkit audit', 2, 'Syntax: .testkit audit\nChecks every glyph of every class: its modifiers reach a spell and the effect they change exists.');
