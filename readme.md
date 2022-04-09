# camelMap
camelMap is a project that finding shortest path on map in real world. Using Bidirection A* Landmark implemented in c++ and comunicate with django serverside web base through unix socket.

Installation on Ubuntu os
--------
**Install python3**
- `sudo apt-get install python3-pip`

**Install cmake**
- `python3 -m pip install cmake`

**Install Django**
- `python3 -m pip install Django`

**Install essential**
- `sudo apt-get install -y build-essential checkinstall zlib1g-dev`

**Install pkg-config**
- `sudo apt-get install pkg-config`

**Install download tools**
- `sudo apt-get install curl zip unzip tar`

Build and start server guild
--------
After clone this projection, your need to bootstrap project before doing anything, this only need one time when you first clone project to your system.
**Bootstraping**
- `./bootstrap.sh`

#### Note:
You may see some error because of laking of some essential libs on your system, so don't worry and install those library then retry bootstraping. You may need minutes to complete this command depending on how strong your computer is. Then, start server by running service script
**Service**
- `./service.sh`
