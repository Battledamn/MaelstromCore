-- .modify currency (4.x currencies): help text, like the other .modify commands.
DELETE FROM `command` WHERE `name` = 'modify currency';
INSERT INTO `command` (`name`, `security`, `help`) VALUES
('modify currency', 2, 'Syntax: .modify currency #currencyId #amount\n\nAdd #amount of a currency (CurrencyTypes.dbc id, e.g. 395 Justice Points, 384-401 archaeology fragments) to the selected player, or to yourself with no player selected. The amount is what the client shows; it can be negative, and the currency''s cap applies.');
