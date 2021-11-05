# Informations about this repo
This repo contains all the files to launch NS-3 simulator with GUI
I use UNBUNTU 1804 64 bits

# Step i have done
NS-3 Version is 3.15


PLC module used is : https://github.com/ns3-plc-module/plc
On this following commit
```
git checkout 2cd8884cfbc5d8f1094f25aeef747d7f4f38ac71
```

How to know your G++ version :
```
sudo update-alternatives --config g++
```
You will need the version 4.4 of g++


To launch the compilation of NS-3 use this following command :
```
CXXFLAGS="-std=c++0x" ./waf configure
./waf
```

GNOSIS (link is dead, but i clone it into https://github.com/73jn/plc-gnosis) :
Install Qt version 4.8
Replace interference.InitializeRx(rxPSD); by interference.StartRx(rxPSD);
Problem with Qjson, go on the branch 1_0_0















This is ns-3-allinone.

If you have downloaded this in tarball release format, this directory
contains some released ns-3 version, along with 3rd party components
necessary to support all optional ns-3 features, such as Python
bindings and Network Simulation Cradle.  In this case, just run the
script build.py; all the components, plus ns-3 itself, will thus be
built.

If, on the other hand, you have obtained this by cloning the mercurial
repository, this directory only contains a few python scripts:

  download.py:
   
     This script will take care of downloading all necessary
     components, including pybindgen and NSC, along with cloning an
     ns-3 repository.  By default, the main development ns-3 branch,
     ns-3-dev, will be cloned, so most users should simply call:

       ./download.py

     but the ns-3-dev repository can be overridden via the -n
     command line option.  For example:
     
       ./download.py -n jabraham3/netanim

     will clone the repository http://code.nsnam.org/jabraham3/netanim
     into the allinone directory.

  build.py:

     This script will get all external components that need to be
     built, and then will build ns-3 using the downloaded components.

       ./build.py

     The most recently downloaded components will be built, according
     to the .config file that is written by download.py.
