# PLC-Gnosis (Working Title) #

> _This document is a work in progress as the build process is still being finalized pending successful (and painless) compilation on our target platforms, as well as the consolidation of all the instructions and prerequisites. It is subject to change and we suggest that you procure a heafty supply of salt to take while reading it._ 

This software is the GUI for an experimental power line communication module built around the ns-3 network simulator.

##Overview##

The purpose of this software is to abstract away the difficult and time-consuming process of hand-piecing together power line communication network models from scratch in c++ using the ns-3 simulator. 

The GUI provides most of the tools necessary to graphically represent the available network components and provides an intuitive interface for adjusting model parameters, cable types, noise sources and others. Also built by this package is a library through which models created in the GUI can be loaded into hand-crafted simulation code. This allows for extreme flexibility in both method and purpose when using the ns-3 plc module to model power line communication networks.

##License Stuff##

As much of the code as possible has been verified to be compatible with GPLv3. While we have done our best to ensure that there are no licensing conflicts, there remains the possibility that we have overlooked something. If something suspicious catches your eye, please let us know so that we can rectify the situation.

##Build Instructions##

A lot of work has been put into making sure that this code is easy to build and use. Should anyone have any problems please post in the issue tracker and we will attempt to fix it as soon as possible. 

Building software successfully should never be a headache... even though it often is.

If you want to skip to less verbose instructions, please see the tl;dr version below.

###Prerequisites##
The first and most obvious prerequisite is downloading the source code. If you're reading this document then you have probably already found it. If not however, please wander over to our [GitHub](http://github.com) page [here](http://github.com/Valiance/plc-gnosis "PLC-Gnosis Main Git Repository"). Clone the repository into a directory of your choosing.

Running

	git clone git://github.com/Valiance/plc-gnosis.git

Will obtain the latest version of the software


Git is obviously required and can be downloaded from it's [website](http://git-scm.com/)

###NS-3 Simulator Software w/ the PLC Module##

This prereq is probably going to be the most time consuming to satisfy. It involves getting the ns-3 simulator software from [here](http://www.nsnam.org/), downloading our plc module into the src directory, and then building everything. As of writing the current release is ns-3.16, however we have only been testing with 3.14 and 3.15. If you feel adventurous you should have no problem using the latest version but we cannot guarantee (as if we could guarantee anything at all anyway) that it will work or compile or build as expected. You can find previous releases, including the versions we have tested with [here](http://www.nsnam.org/releases/older/).

**The plc module does not check for the presence of the boost libraries even though they are required. We need to make sure it does.**

>*It is also required that the user build the plc module, however we have not had the chance to get this into a repository yet. I will include basic instructions here (like making sure the boost library is available and where to put the plc module source code inside the ns-3 directory) but the plc module will have a similar front page on it's own git repository explaining in as much detail as possible how to build the software.

Once built the ns-3 software will essentially be a collection of shared library files sitting in a directory similar to this:

	/home/user/Desktop/ns-allinone-3.15/ns-3.15/build

**You will need this location in order to build the software**, since we do two things:

1. We link some code against these dynamic libraries so that both the simulator and loader can call it.
2. We use the directory as a default setting for setting the LD\_LIBRARY\_PATH environment variable when starting up the simulator from the GUI. 

**Note the directory of your build folder and keep it for use later in the build process.**

>Whether or not this is an ideal solution to dealing with necessary external libraries is still in question. We'd rather not require that users notify ldconfig or other equivalents of the existence of the ns-3 libraries since that's not really how the ns-3 software seems to do things (scripts and whatnot are run through the waf build system and everything seems to stay in the local directory structure). Again, as with anything to do with this software if you feel you have a better solution please let us know. 

###Qt##

The GUI has been designed using the Qt GUI Framework and SHOULD work with the all 4.8 series of releases. We have TESTED mostly with 4.8.3, so please let us know if you run into any problems with your specific version. We will port to Qt 5 when it becomes a little more widespread.

Please see the main Qt page for downloads/installation instructions for your platform [here](http://qt-project.org "Qt Project Main Site")

If you're system has Qt installed properly you should be able to execute
	
	qmake -v

And get a reasonable output.

###_[QJson](https://github.com/flavio/qjson "GitHub page for QJson")_###

The QJson library provides us with an elegant way of interfacing our Qt based GUI with our JSON based format for diagram files. Shared libraries are often a massive pain and since we would like to keep our instructions as platform-agnostic as possible, we compile QJson directly and link statically. Satisfying this prerequisite involves making a clone of the QJson repository into the lib directory of our source tree as follows:

	cd plc-gnosis/lib  
	git clone git://github.com/flavio/qjson.git qjson

You are done with this prerequisite once the cloning process completes.

##GUI Compilation##

In order to build the software after all of the prerequisites have been satisfied, you simply need to navigate to the plc-gnosis/src directory, and execute the following, modifying the path to your ns-3 build directory that was noted earlier:

	qmake plc-gnosis.unix.pro NS3_DIR=/your-path-to-ns3/ns-allinone-3.15/ns-3.15/build -r
	make

The software should compile and you will find the executables in the bin/app directory.

*Note that while there is windows project file, this hasn't been updated for a while and probably won't work. We will eventually ensure the GUI is usable on windows (although obviously it will not be able to interface with the ns-3 simulator software)*

##TL;DR##

The following code snippet is a summary of the necessary steps to build the gui

	git clone git://github.com/Valiance/plc-gnosis.git plc-gnosis  
	cd plc-gnosis/lib  
	git clone git://github.com/flavio/qjson.git qjson  
	cd ../src  
	qmake plc-gnosis.unix.pro NS3_DIR=/your-path-to-ns3/ns-allinone-3.15/ns-3.15/build -r  
	make  






