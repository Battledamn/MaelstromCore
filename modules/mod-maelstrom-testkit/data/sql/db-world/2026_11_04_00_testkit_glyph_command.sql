-- mod-maelstrom-testkit: help for .testkit glyph
DELETE FROM `command` WHERE `name` = 'testkit glyph';
INSERT INTO `command` (`name`, `security`, `help`) VALUES
('testkit glyph', 2, 'Syntax: .testkit glyph #id #apply [#spell ...]\nPuts glyph #id (GlyphProperties.dbc) in a slot of its type (#apply 1) or takes it out (0), and the known spells of the list on action buttons 1-5. #id 0 empties every glyph slot.');
