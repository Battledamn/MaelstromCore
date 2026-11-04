# mod-maelstrom-testkit

GM commands that set a character up for MaelstromCore's class tests (`docs/TEST_PLAN.md`, tier 9).
They are not 4.3.4 behaviour, so they live in this module, not in the core.

| Command | Permission | What it does |
|---|---|---|
| `.testkit tree <1-3>` | 2001 | every talent of that talent tree (the talent window's tabs from the left) at its highest rank, beyond the 41 points, and the tree as the active spec's specialization |
| `.testkit ready` | 2002 | full health and power, no cooldowns or Forbearance, five level 85 training dummies in front of you |
| `.testkit reset` | 2003 | resets the active spec's talents and specialization |
| `.testkit glyph <id> <1\|0> [spells]` | 2004 | puts a glyph in a slot of its type or takes it out, and the known spells of the list on action buttons 1-5 (they replace what was there); id 0 empties every slot; `<2>` instead checks that the glyph's modifiers reach and change a spell |
| `.testkit cast <spell> [triggered]` | 2004 | you cast it: at yourself if it is helpful, otherwise at your selection (`.cast self` makes the selection cast it at itself) |
| `.testkit audit` | 2005 | every glyph of every class, no character needed (also from the console and SOAP): its modifiers reach a spell and the effect they change exists |

Permissions 2000-2003 (module range, `docs/MODULARITY.md`) are linked to the Gamemaster role (197) by
`data/sql/db-auth`; the command help is in `data/sql/db-world`. The `MaelstromClassTest` addon
has buttons for these commands and a panel that marks each talent green
once its effect has been seen in the combat log.
