# OpenXcom [![Workflow Status][workflow-badge]][actions-url]

[workflow-badge]: https://github.com/OpenXcom/OpenXcom/workflows/ci/badge.svg
[actions-url]: https://github.com/OpenXcom/OpenXcom/actions

OpenXcom is an open-source clone of the popular "UFO: Enemy Unknown" ("X-COM:
UFO Defense" in the USA release) and "X-COM: Terror From the Deep" videogames
by Microprose, licensed under the GPL and written in C++ / SDL.

See more info at the [website](https://openxcom.org)
and the [wiki](https://www.ufopaedia.org/index.php/OpenXcom).

Uses modified code from SDL\_gfx (LGPL) with permission from author.


## -=AKIMBO FEATURES=-

-=< FEATURES DESCRIPTION >=-


1) Akimbo shooting.
Support extra firing mode – "Akimbo Shots". It allows units to fire weapons held in each hand as a single action (dual-wielding). Acts as other firing modes (snap, auto or aimed), but with one exception: both weapons the unit is holding must possess this property.

Gameplay features:
- possible as reaction shot (only if shots quantity lesser or equal to proper snap shot shots quantity (a la "dual snap"))
- possible as berserk unit state;
- adjustable hotkeys for action menu and reserve TU;
- reserve button located at bottom-right side of UI panel (absents, if Akimbo shooting option is disabled or hidden);
- spread waypointing support (if both weapons has that property);
- aliens (any unit controlled by AI)  are able to use akimbo, if it possible.
- there are default akimbo submods for EU and TFTD are included. This submods can be used for fun and as tutorial how to add<->adjust dual-wield firing mode for any weapon
- Plug'n'Play pattern: at the start of the game, each item  from any loaded mod will be scanned for "pistol" features and akimboTU   gains a dual-wielding mode, based on snap shot config of proper weapon with formula:
tuAkimbo = tuSnap + 3 TU (absolute, not %), accuracyAkimbo = accuracySnap – 7 %, shots quantity just equates. Of course, if the specific pistol already have this feature (predefined in ruleset) it will not be touched.
The original concept behind the feature - to make pistols more usable, at least early in the game. That's why pistols gain dual-wielded feature by default. Also any firearm weapon, except Blaster Launcher, can be used in akimbo mode – there are need just to define next properties in ruleset for target gun:

tuAkimbo: -> defines TU usage amount and enables Akimbo fire mode for current weapon;
flatAkimbo: -> TU usage is absolute (false) or relative (true);
accuracyAkimbo: -> firing accuracy.
akimboShots: -> similar to autoshots, but applies for current weapon with akimbo action type.
sprayWaypoints: -> similar to autoshot mode, but works when both weapon has it. Picks lesser number of possible spots from both weapons.
akimboRange: -> tile range, when accuracy will start to decrease.

Supports new ruleset extraSprites object - akimboReserveIco. Spritesheet should contain 2 frames - for untoggled and toggled states.

--------------------------------------------------------------------------------------------------


2) Pierce (pass through) ammo bullet type (aka "rail guns") support.

This allows the projectile to function like a "railgun" or a beam weapon: it continues its flight after colliding with an object on the map - provided the object was destroyed (for terrain) or damaged (for units) and the projectile itself retained a non-zero "pierce power" value after the hit.

Projectile's pierce power usually equals to their damage and decreases after each collision. Actual damage may to decrease with the current pierce power value or remain constant, depending on the pierce type bullet property (kinetic or beam). Pierce power decrement calculation based on the HP, frontal armor and proper damage modifier (for units), or just armor value and tile damage modifier (for terrain).

Gameplay features:
-hi arc projectiles are supported (parabola trajectory);
-waypointing projectiles  are supported (aka blaster bombs);
-AOE (explosive) ammo are supported (damage type due pierce processing is AP, but can be overridden);
-shotguns currently are not supported.

Possible ammo ruleset properties:
  - pierceType. Default value 0, supports 0-2, any other value converts to 0 (typo protection).
  0 – vanilla behavior (fly till any hit);
  1 - kinetic type. Projectile's pierce power equals damage and both values decreases simultaneously after each hit;
  2 – beam type. Projectile reduces pierce power only during each hit, damage stays the same.

  - piercePowerCap. Pierce power overrider. Default is 0, supports 0-1000. 0 – power capacity equals to ammo damage power amount by default, other positive values overrides  pierce bullet "durability" (limit is 1000 for that case).

  - pierceAOEDamageType. Suitable for HE, Incendiary and Stun ammo, which are able to pierce objects with that damage type, then will explode with their original power and damage type. Default is 0 and it automatically converts to 1 (Armor Piercing) for AOE ammo.

*Hint: Adding just pierceType: 1 parameter to the desired ammo in the ruleset should be enough to make this feature work correctly in the most cases.

--------------------------------------------------------------------------------------------------


3) Melee mechanic adjustment

- big unit wrong side issue fix;
- fixed misses to units at higher terrain (16+), small or kneel units at some "straw" tiles.
- added terrain melee helper, which allow to hit weird terrain tile configuration (e.g. tall floor tileparts, floor parts in wall tile slots). Usage: Alt + click.


3.1) Diagonal Terrain Melee option.

It provides possibility to hit terrain with melee weapon at diagonal directions.
Diagonal melee aim helper: Alt+click – allows unit to attack objects at same and adjacent tiles. Suitable for "weird terrain tiles" hitting in some cases (etc. between 2 diagonal bigWalls).
Could be activated at Options->Advanced -> BOXCE -> BattleScape section. Of course it works when extendedTerrainMelee feature is activated too.


3.2) Forced Melee to Floor feature.

This feature allows to use a melee weapon to attack an unconscious unit located directly beneath your unit feet. Note: enabling the `extendedTerrainMelee` option allows to attack terrain objects located beneath your feet (e.g. for clearing beneath unit space from terrain staff or for breaching a hole in the floor).
Usage: stay at target tile and Ctrl+click to melee weapon.

---------------------------------------------------------------------------------------------------


4) maxRangeEvent ammo item property

Works in combination with existed maxRange property. It limits flight range and defines projectile behavior when projectile is passed that limited range on map (thanks to mumble for inspiration). Yes: maxRange for weapon and maxRange for ammo works with different way, could be combined and could act simultaneously: defined weapon's maxRange limits aim range, defined ammo's maxRange limits projectile's range.
This property also makes guided projectiles (aka Blaster Bomb) more guided:
- missile will react (explode or vanish) when it reaches the last waypoint;
- if maxRange for ammo is defines – reacts too if restricted distance is passed (whatever event occurs first: last waypoint or maxRange distance).

Can be applied to any ammo type of any weapon.
Possible options:
  0 - vanilla (default). Projectile flies till end of map/any object (unit);
  1 - hit (or explode for HE), when ammo's maxRange distance is passed;
  2 – projectile just vanish over ammo's maxRange distance.

Practical purpose: could be applicable for laser weapons under water (limit projectile range), or flame throwers, or improvised ranged melee weapons (prevent to fly projectile (and hit) over unintended distances).

--------------------------------------------------------------------------------------------------


5) Shotgun adjustments

  5.1) OXC buckshot hit to void issue fix. Rest shotgun pellets will be emitted even when the "primary" projectile flies outside the map. Also each pellet hit will be sounded. Some adjustments may be made in the future (lowering sound volume or allowing to play that sound only once).

  5.2) added explosive shells support (e.g. HE, Stun). Seems, it is finished (I hope) todo feature from OXC(E).

--------------------------------------------------------------------------------------------------


6) Numpad unit movement control option

Allows to move units via numpad. Strafing and sneaking supports too. Borrowed and experimental feature.
Can be found at Options -> Advanced -> BOXCE -> BattleScape -> Numpad Movement Control.
Possible Options:
  0 - disabled;
  1 - movement to relative direction;
  2 - movement to absolute direction.

--------------------------------------------------------------------------------------------------


7) Scanner ruleset item properties (aka "Motion scanner")
    scanRange. Scan Radius in tiles.  Default is 9 (vanilla range), clamped to 0 (non-negative). Allows to regulate scanner checking battlescape area.
    isScanAll. Allow to scan static units too (any live object). aka UFO Extender advanced scanner feature. Default is false.
As addition: scanner "blobs" and "spoiler arrows" at map of detected units will be darker if targets are not located at the same altitude as game camera. This feature is currently fixed, adjustable option in future is possible.

--------------------------------------------------------------------------------------------------


8) PSI weapon ruleset  item properties (aka "PSI-Amp")
Allows to define LOS requirement for each action separately.
    panicLOS. Default is false. Prevent to cast panic attack action, when target is not visible for attacker.
    mindControlLOS. Default is false. Prevent to cast mind control action, when target is not visible for attacker.
General LOSRequired: true ruleset item setting overrides this properties.

--------------------------------------------------------------------------------------------------


9) No Multi Hit for Explosion game option.

    Disabled. Multiple hits may be dealt to large units and units positioned on high ground/stairs (up to 8 hits on a large unit and up to 2 hits on small units).
    Enabled. Each unit receives only one hit from a single explosion.   
Locates at Options->Advanced->BOXCE->Battlescape->No extra hits for explosion.

--------------------------------------------------------------------------------------------------



10) Visual and some QoL

10.1) Floating unit bobbing and shade casting game option.
Each unit will slightly bobbing and shade cast when in floating/flying state.
Possible options:
  0 - Disabled;
  1 - Bobbing;
  2 - Casting shadow;
  3 - Bobbing and shading;
  4 - Bobbing and dynamic shading.
Locates at Options->Advanced->BOXCE->Battlescape->Floating unit bobbing and shading.
Additional ruleset unit armor property - canFloatBob (true), which allow to deny any floating unit to bob and cast dynamic (but not static) shade.


10.2) Unit strafing
- "moonwalk" animation from OXC is restored (unfortunately, this feature broken in OXCE and is still not restored);
- small units are able to strafe at backward directions too (QoL);


10.3) Alternate arrow color schema game option
Change view for unit selector and detected by scanner unit's spoiler arrows (when Alt button is pressed). Arrows begin to blink slightly. Scanner arrow will be darker for detected units at another camera view level.
Locates at Options->Advanced->BOXCE->Battlescape->Alternative arrow scheme.


10.4) Morale Attack Success Notification behavior game option.
Allows to define how to notify player about successful morale attack (e.g. PSI AMP panic action):
  0 - No notify (Original);
  1 - Flash message;
  2 - Info Box (introduced by OXC).
This feature can be found at Options->Advanced->BOXCE->BattleScape->Morale Attack Success Notification.


10.5) Dynamic flashes game option.
Enables dynamic flashes / lighting for exposions and flying projectiles.
Options:
  0 - Disabled;
  1 - Explosion (and hits too) flashes;
  2 - Explosion and projectile flashes. 
Each flare item will emit proper light during throwing too.
Additional ruleset ammo item propery - flyFlashRadius (3 is default), which allow to define light radius (power) for proper projectile.


10.6) unit selector and detected by scanner unit arrow image unhardcodded.
Supports new ruleset extraSprites objects:
  arrow - unit selector arrow image;
  scanArrow - detected by scanner units arrow image (when Alt button is pressed).


10.7) Single big boom game option.

Perform one big single explosion instead of default OXC blasting serie for terrain.
Locates at Options->Advanced->BOXCE->Battlescape->Single big boom.
  
===================================================================================================


## Realistic Accuracy and Cover system, v3.0
[Full information HERE](Realistic%20Accuracy%20v3.0.md)

## 'Rejected features' fork

This is a fork from official master of OXCE+ which allows some new features which
have been rejected by official OXCE+ developers. Currently there are 2 new features
implemented:

### More than 8 bases

Now, player can choose the limit of available bases to build using a new entry in
file 'openxcom.cfg': 

oxceMaxBases= maxNumBases

Default is 8 bases.
If more than the 8 visible bases, player can scroll bases miniview using RMB at the 
edges of the miniview. A blue arrow appears when there are more bases at either edge.
 
### Hangars reworked 
Every hangar and craft has a "hangarType" tag, so a Hangar with an specific "hangarType"
value can only store crafts with the same "hangarType". "hangaType" tag can be given 
at both facilities and crafts, in their corresponding YAML ruleset. If no tag is given, 
that craft/hangar will have a "-1" default tag, so their behavior will be the same
as in vanilla engine.
Another feature added is the possibility of allocate more than 1 craft at an specific
hangar facility, so all crafts will be shown at Basescape, and can be right-clicked
to go to craft screen. If more than craft is defined for an hangar facility, a new 
YAML tag "craftSlots" should be defined in the hangar ruleset. This tag will be 
followed by as many "position entries" as crafts are allowed in the hangar.
Every position is a "- [x, y, z]" entry, where x,y are position offesets respect to 
the center of the hangar sprite, and z is ignored. This way, modders can choose where
they want to place crafts in an hangar which allows more than one.  

An "example MOD" is included in "Examples" folder. Though this mod is a submod of the
fantastic master mod "The Xcom Files", you can adapt it to any other mod or vanilla 
game, which needed using differentiated hangars and/or more than 1 craft per hangar.

### Windows Binary

For convenience, a ZIP file with windows binary has been added to folder 'WindowsBinaries'.
 This exe has been tested to work in a Windows10 machine (64 bits), thought it has not been
intensively tested. It also include extra Language files needed for some specific features

[Link to Windows Binary](/WindowsBinaries/UnzipInOXCEFolder.zip)

You can just place the binary and folder in the game folder of the original one, and just click
it when you want start the game with these new features.

## Installation

OpenXcom requires a vanilla copy of the X-COM resources -- from either or both
of the original games.  If you own the games on Steam, the Windows installer
will automatically detect it and copy the resources over for you.

If you want to copy things over manually, you can find the Steam game folders
at:

    UFO: "Steam\SteamApps\common\XCom UFO Defense\XCOM"
    TFTD: "Steam\SteamApps\common\X-COM Terror from the Deep\TFD"

Do not use modded versions (e.g. with XcomUtil) as they may cause bugs and
crashes.  Copy the UFO subfolders to the UFO subdirectory in OpenXcom's data
or user folder and/or the TFTD subfolders to the TFTD subdirectory in OpenXcom's
data or user folder (see below for folder locations).

## Mods

Mods are an important and exciting part of the game.  OpenXcom comes with a set
of standard mods based on traditional XcomUtil and UFOExtender functionality.
There is also a [mod portal website](https://openxcom.mod.io/) with a thriving
mod community with hundreds of innovative mods to choose from.

To install a mod, go to the mods subdirectory in your user directory (see below
for folder locations).  Extract the mod into a new subdirectory.  WinZip has an
"Extract to" option that creates a directory whose name is based on the archive
name.  It doesn't really matter what the directory name is as long as it is
unique.  Some mods are packed with extra directories at the top, so you may
need to move files around inside the new mod directory to get things straighted
out.  For example, if you extract a mod to mods/LulzMod and you see something
like:

    mods/LulzMod/data/TERRAIN/
    mods/LulzMod/data/Rulesets/

and so on, just move everything up a level so it looks like:

    mods/LulzMod/TERRAIN/
    mods/LulzMod/Rulesets/

and you're good to go!  Enable your new mod on the Options -> Mods page in-game.

## Directory Locations

OpenXcom has three directory locations that it searches for user and game files:

<table>
  <tr>
    <th>Folder Type</th>
    <th>Folder Contents</th>
  </tr>
  <tr>
    <td>user</td>
    <td>mods, savegames, screenshots</td>
  </tr>
  <tr>
    <td>config</td>
    <td>game configuration</td>
  </tr>
  <tr>
    <td>data</td>
    <td>UFO and TFTD data files, standard mods, common resources</td>
  </tr>
</table>

Each of these default to different paths on different operating systems (shown
below).  For the user and config directories, OpenXcom will search a list of
directories and use the first one that already exists.  If none exist, it will
create a directory and use that.  When searching for files in the data
directory, OpenXcom will search through all of the named directories, so some
files can be installed in one directory and others in another.  This gives
you some flexibility in case you can't copy UFO or TFTD resource files to some
system locations.  You can also specify your own path for each of these by
passing a commandline argument when running OpenXcom.  For example:

    openxcom -data "$HOME/bin/OpenXcom/usr/share/openxcom"

or, if you have a fully self-contained installation:

    openxcom -data "$HOME/games/openxcom/data" -user "$HOME/games/openxcom/user" -config "$HOME/games/openxcom/config"

### Windows

User and Config folder:
- C:\Documents and Settings\\\<user\>\My Documents\OpenXcom (Windows 2000/XP)
- C:\Users\\\<user\>\Documents\OpenXcom (Windows Vista/7)
- \<game directory\>\user
- .\user

Data folders:
- C:\Documents and Settings\\\<user\>\My Documents\OpenXcom\data (Windows 2000/XP)
- DATADIR build flag
- C:\Users\\\<user\>\Documents\OpenXcom\data (Windows Vista/7/8)
- \<game directory\>
- . (the current directory)

### Mac OS X

User and Config folder:
- $XDG\_DATA\_HOME/openxcom (if $XDG\_DATA\_HOME is defined)
- $HOME/Library/Application Support/OpenXcom
- $HOME/.openxcom
- ./user

Data folders:
- $XDG\_DATA\_HOME/openxcom (if $XDG\_DATA\_HOME is defined)
- $HOME/Library/Application Support/OpenXcom (if $XDG\_DATA\_HOME is not defined)
- DATADIR build flag
- $XDG\_DATA\_DIRS/openxcom (for each directory in $XDG\_DATA\_DIRS if $XDG\_DATA\_DIRS is defined)
- /Users/Shared/OpenXcom (if $XDG\_DATA\_DIRS is not defined or is empty)
- . (the current directory)

### Linux

User folder:
- $XDG\_DATA\_HOME/openxcom (if $XDG\_DATA\_HOME is defined)
- $HOME/.local/share/openxcom (if $XDG\_DATA\_HOME is not defined)
- $HOME/.openxcom
- ./user

Config folder:
- $XDG\_CONFIG\_HOME/openxcom (if $XDG\_CONFIG\_HOME is defined)
- $HOME/.config/openxcom (if $XDG\_CONFIG\_HOME is not defined)

Data folders:
- $XDG\_DATA\_HOME/openxcom (if $XDG\_DATA\_HOME is defined)
- $HOME/.local/share/openxcom (if $XDG\_DATA\_HOME is not defined)
- DATADIR build flag
- $XDG\_DATA\_DIRS/openxcom (for each directory in $XDG\_DATA\_DIRS if $XDG\_DATA\_DIRS is defined)
- /usr/local/share/openxcom (if $XDG\_DATA\_DIRS is not defined or is empty)
- /usr/share/openxcom (if $XDG\_DATA\_DIRS is not defined or is empty)
- the directory data files were installed to
- . (the current directory)

## Configuration

OpenXcom has a variety of game settings and extras that can be customized, both
in-game and out-game. These options are global and affect any old or new
savegame.

For more details please check the [wiki](https://ufopaedia.org/index.php/Options_(OpenXcom)).

## Development

OpenXcom requires the following developer libraries:

- [SDL](https://www.libsdl.org) (libsdl1.2)
- [SDL\_mixer](https://www.libsdl.org/projects/SDL_mixer/) (libsdl-mixer1.2)
- [SDL\_gfx](https://www.ferzkopp.net/wordpress/2016/01/02/sdl_gfx-sdl2_gfx/) (libsdl-gfx1.2), version 2.0.22 or later
- [SDL\_image](https://www.libsdl.org/projects/SDL_image/) (libsdl-image1.2)

The source code includes files for the following build tools:

- Microsoft Visual C++ 2010 or newer
- Xcode
- Make (see Makefile.simple)
- CMake

It's also been tested on a variety of other tools on Windows/Mac/Linux. More
detailed compiling instructions are available at the
[wiki](https://ufopaedia.org/index.php/Compiling_(OpenXcom)), along with
pre-compiled dependency packages.
