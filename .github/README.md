# MaelstromCore

**MaelstromCore is [AzerothCore](https://www.azerothcore.org/) ported to World of Warcraft: Cataclysm
4.3.4 (client build 15595).** It is AzerothCore itself, under a new name, not a separate core that
borrows from it: the same code, structure, conventions, modules and tools, changed only where
Cataclysm needs it.

> **Work in progress.** MaelstromCore is not fully playable yet. Large parts of 4.3.4 already work in
> game; classes, zones and dungeons are being brought to 4.3.4 step by step.

## Goal

Version 1.0 covers every race and class from level 1 to 85, the Cataclysm 5-man dungeons and the
Dungeon Finder. Raids and PvP come after 1.0.

What works today, among others: the 4.3.4 client connection and login, character creation, the
Cataclysm stats, attack table and power regeneration, talents with specializations, mastery and
dual spec, archaeology, currencies, achievements, professions and flights. The class spells are being
brought to 4.3.4 one class at a time.

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

Agent instructions for this repository live in [AGENTS.md](../AGENTS.md), with task-scoped guides in
[.agents/docs/](../.agents/docs/).

## Credits

MaelstromCore stands entirely on the work of others:

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

## License

- The MaelstromCore source code is released under the
  [GNU GPL v2](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html), like AzerothCore's
  ([LICENSE](../LICENSE)).

MaelstromCore is not an official Blizzard Entertainment product and is not affiliated with or endorsed
by World of Warcraft or Blizzard Entertainment. It is a project for testing and learning; it does not
sponsor or support illegal public servers.
