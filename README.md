Second Contract Mudlib for DGD
==============================

Second Contract is a Kernel-based MUDLib running on DGD.

This MUDlib is influenced in part by the available architecture 
documentation from Skotos that has been published on the web. Additional
influences are the Dead Souls and Discworld MUDlibs.

Installing
----------

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
* Copy the sc.dgd config file from this repo to an appropriate place to use with your DGD driver
* Start up the mud
* Create your admin account according to the KernelLib directions
* Telnet to the first telnet port (6047 by default) and create your first account (but don't try to create a character)
* Edit the bootstrap.sh script to reflect the account you just created
* Run bootstrap.sh to create the default domains, areas, and templates
* Point your browser to the second binary port (http://localhost:6050/admin/index.html by default)

There are a few steps there, but that's because I haven't created a general
distribution for the mudlib yet. A distribution will combine the KernelLib
with the mudlib and bundle in the DGD-networking driver source.

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
