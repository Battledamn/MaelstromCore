# MaelstromCore

**MaelstromCore is [AzerothCore](https://www.azerothcore.org/) ported to World of Warcraft: Cataclysm
4.3.4 (client build 15595).** It is not a separate core that borrows from its parent: the code,
structure, conventions, modules and tools are the same, renamed and changed only where Cataclysm needs it.

> **Work in progress.** MaelstromCore is not fully playable yet. Large parts of 4.3.4 already work in
> game; classes, zones and dungeons are being brought over step by step.

## Goal

A fully working Cataclysm 4.3.4: every race and class from level 1 to 85, the Cataclysm version of Azeroth with
its quests, professions, the dungeons and raids, PvP and the Dungeon Finder, playing the way patch 4.3.4
did.

## Where it stands

Seen working in game with the 4.3.4 client:

- Connection, login, the realm list, and creating every race and class.
- Movement, swimming, flight paths, portals and riding.
- The Cataclysm stats, attack table and power regeneration, and every class's resource.
- Talents with specializations, mastery and dual spec, and glyphs.
- Questing through the starting zones, except the Goblin, Worgen and death knight ones.
- Chat, groups, trade, mail, guilds and friends.
- All professions to 525, archaeology, currencies and achievements.

In progress:

- Class spells, one class at a time: paladin, druid, warrior, hunter, rogue, priest and mage are done and
  tested; warlock and shaman are in testing; the death knight is next.
- The Goblin, Worgen and death knight starting zones

Not done yet: vehicles, pathfinding, the Guild Finder and the Dungeon Finder. The Outland, Northrend and
older dungeons and raids still run Cataclysm data under scripts written for the WotLK versions, and do not
work yet; the Cataclysm dungeons and raids have not been tested.

## Principles

- **The 4.3.4 client is the reference.** When sources disagree about a mechanic, what the client itself
  shows and predicts decides.
- **AzerothCore's way.** Code is written the way AzerothCore writes it, so it stays easy to read, edit
  and extend, and upstream fixes keep fitting.
- **Modular.** Anything that is not 4.3.4 behaviour lives in a module, never in the core.
- **Credit where it is due.** Code or data taken from another project keeps its original author.

### Modules

Like AzerothCore, MaelstromCore is built to be extended through modules (`modules/`), script hooks and
configuration rather than changes to the core. Its own 4.3.4 hooks, functions and options for module
authors are documented alongside the project.

## Building

MaelstromCore builds like AzerothCore (CMake, C++20, MySQL); see AzerothCore's
[installation guide](https://www.azerothcore.org/wiki/installation). It is developed and run on Windows
with MSVC. It needs a Cataclysm 4.3.4 (build 15595) client and its extracted data.

## AI-assisted development

MaelstromCore is developed with an AI coding agent, Anthropic's Claude (through Claude Code), working
under the project owner's direction. The agent investigates against the 4.3.4 client data, packet logs
and the reference projects, ports and writes the code, builds and deploys the server, and keeps the
documentation; the owner decides what is built and tests every change in game. Nothing counts as working
until it has been seen in game.

The agent follows the same rules a human contributor would: AzerothCore's
[AI agentic engineering guidelines](https://www.azerothcore.org/wiki/agentic-engineering), the agent
instructions in [AGENTS.md](../AGENTS.md) with their task-scoped guides in
[.agents/docs/](../.agents/docs/), and the project's own rules for 4.3.4 work. Commits it co-authors
say so in their trailer.

## Credits

MaelstromCore is built on the work of others:

- **[AzerothCore](https://github.com/azerothcore/azerothcore-wotlk)** and all of its contributors: the
  core MaelstromCore is. AzerothCore was born in 2016 from SunwellCore and itself builds on
  **[MaNGOS](https://www.getmangos.eu/)** and **[TrinityCore](https://www.trinitycore.org/)**. See the
  [AUTHORS](../AUTHORS) file; contributors before 2016 have no git history of their own (SunwellCore was
  published without it).
- **[trolloks/azerothcore-cata](https://github.com/trolloks/azerothcore-cata)** and its contributors: the
  AzerothCore Cataclysm fork MaelstromCore started from.
- **[The Cataclysm Preservation Project](https://github.com/The-Cataclysm-Preservation-Project/TrinityCore)**
  and **TrinityCore**: much of the 4.3.4 behaviour is ported from their code; each ported change carries
  its original author.
- **TrinityDB (TDB 434)**: the Cataclysm world data.
- **[ArkCORE-NG](https://github.com/Arkania/ArkCORE-NG)** by the Arkania team (gpn39f, ladel and others):
  the Goblin start's scripts for Kezan and the Lost Isles, ported from its code, and the Lost Isles'
  creature placement, paths and movement and Life Savings' route from its world database (GPL-2.0).
- **[MaNGOS Three's world database](https://github.com/mangosthree/database)**, based upon the work of
  [getmangos.eu](https://www.getmangos.eu/): creature spawns TDB 434 lacks, such as quest givers that
  moved in Cataclysm.

## License

- The MaelstromCore source code is released under the
  [GNU GPL v2](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html), like AzerothCore's
  ([LICENSE](../LICENSE)).
- World data taken from MaNGOS Three's world database is copyright of the MaNGOS authors and stays under
  [CC BY-NC-SA 3.0](https://creativecommons.org/licenses/by-nc-sa/3.0/): it may not be used commercially,
  and copies or changes of it must keep that license. The files that hold it say so in their header.

MaelstromCore is not an official Blizzard Entertainment product and is not affiliated with or endorsed
by World of Warcraft or Blizzard Entertainment. It is a project for testing and learning; it does not
sponsor or support illegal public servers.
