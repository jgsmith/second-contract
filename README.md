Second Contract Mudlib for DGD
==============================

Second Contract is a Kernel-based MUDLib running on DGD.

This MUDlib is influenced in part by the available architecture 
documentation from Skotos that has been published on the web. Additional
influences are the Dead Souls (dsIIr10 code and DS 3.x behavior) and 
Discworld MUDlibs.

**N.B.:** This MUDlib is very alpha. Do not expect to run a mud with it yet.
Everything internally is subject to change. Persistence should be considered
a curiosity at the moment and nothing more.

Installing from a Distribution
------------------------------

Everything you need is bundled in the distribution. The DGD driver is in the
src/ directory. Everything that is part of the mudlib (Second Contract code
as well as the Kernel Lib) is under the mud/ directory.

### DGD

First, install the DGD driver by changing into the `src/` directory and running:

    make
    make install

This will install the driver in the `bin/` directory.

### Configure mud.dgd and startmud

Edit the `mud.dgd` file to specify the absolute directory for the `mud/`
directory in the distribution.

Edit `bin/startmud` to indicate the full path of your installation if you plan
to run `startmud` from elsewhere.

### Start the Game

Run the following to start the game:

    ./bin/startmud

This will start the game in such a way that you can "reboot" from an admin
account and the driver will restart. The script will copy the state dump to
a new file for backup. If an existing dump file exists, the script will start
the game with the dump file to restore the prior game state.

### Create Admin Accounts

If you've modified the telnet and binary ports in the configuration file,
then modify the port numbers below. Port 6048 is the first binary port. Port
6047 is the first telnet port.

    telnet localhost 6048

Enter "admin" as your username. The system will prompt you to set a password.
This account is the Kernel Lib's admin user. This account is for disaster
recovery and mudlib modification, not for game creation. This account can not
be used to play the game or enter the game world.

    telnet localhost 6047

Create an account for yourself. The first account created through this port
is made part of the ADMIN group with access to everything. Don't create any
characters yet since there are no character archetypes or locations defined.

### Bootstrap Object Definitions

Modify the `bootstrap.sh` script so that it uses the username and password you
created through port 6047. Then load the demo domains, verbs, etc.

    ./bootstrap.sh

### Create First Character

Now you can go back to port 6047, log in with the port 6047 account you created,
and create your first character.

The demo code in this distribution will create a single start scene and a
single human character archetype. When you log in, you will find yourself
floating in the void.

All you can do is "look" and "quit". You can use "%who" to see which other
characters are logged in and "%help look" for a small bit of help on the
"look" verb.

### Using Admin Web Interface

You can point your web browser at /admin/index.html on port 6050. If you're
browser is on the same system as the driver, then you can browse to
http://localhost:6050/admin/index.html. Use the account for port 6047 to
log in.


Installing from Scratch
-----------------------

You will need a working installation of DGD and the Kernel Library that
came with the version of DGD you have installed. Once we have something
closer to a working MUBLib, we'll expand these instructions.

The general process for getting the system up and running (once the bootstrap
files are included):

* Install DGD with Networking extensions
* Install KernelLib into your target mud location
* Copy all files from /mud in this repo into your target mud location
* Change into the www/ directory in your target mud location
* Run "make" to assemble the HTML and JavaScript files for the web admin console
* Copy the mud.dgd config file from this repo to an appropriate place to use with your DGD driver
* Start up the mud
* At this point, you can pick up the directions in the section on installing from a distribution (starting with "Create Admin Accounts")

Licensing
---------

This MUDLib is released under the GNU Affero General Public License.

Due to how the MUDLib is designed, the world you build on top of this
MUDLib is not subject to the same license. Any files you modify or create
in the following directories are subject to this GNU Affero GPL (with '/'
being taken as the root of this repository):

* /mud/data/
* /mud/include/devlib/
* /mud/include/gamelib/
* /mud/include/iflib/
* /mud/include/system/
* /mud/include/toollib/
* /mud/include/worldlib/
* /mud/usr/DevLib/
* /mud/usr/GameLib/
* /mud/usr/IFLib/
* /mud/usr/System/
* /mud/usr/ToolLib/
* /mud/usr/WorldLib/

The following files are also subject to the GNU Affero GPL:

* /mud/include/data.h
* /mud/include/devlib.h
* /mud/include/gamelib.h
* /mud/include/iflib.h
* /mud/include/toollib.h
* /mud/include/worldlib.h
