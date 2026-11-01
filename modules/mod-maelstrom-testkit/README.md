# mod-maelstrom-testkit

GM commands that set a character up for MaelstromCore's class tests (`docs/TEST_PLAN.md`, tier 9).
They are not 4.3.4 behaviour, so they live in this module, not in the core.

| Command | Permission | What it does |
|---|---|---|
| `.testkit tree <1-3>` | 2001 | every talent of that talent tree (the talent window's tabs from the left) at its highest rank, beyond the 41 points, and the tree as the active spec's specialization |
| `.testkit ready` | 2002 | full health and power, no cooldowns or Forbearance, five level 85 training dummies in front of you |
| `.testkit reset` | 2003 | resets the active spec's talents and specialization |

Permissions 2000-2003 (module range, `docs/MODULARITY.md`) are linked to the Gamemaster role (197) by
`data/sql/db-auth`; the command help is in `data/sql/db-world`. The `MaelstromClassTest` addon
(`G:\MaelstromCore\addons`) has buttons for these commands and a panel that marks each talent green
once its effect has been seen in the combat log.
