# camelMap
camelMap is a project that finding shortest path on map in real world. Using Bidirection A* Landmark Algorithm implemented in c++ and comunicate with django serverside web base through unix socket.

Installation on Unix
--------
**Install python3**
- `sudo apt-get install python3-pip`

**Install essential**
- `sudo apt-get install -y build-essential checkinstall zlib1g-dev`

**Install pkg-config**
- `sudo apt-get install pkg-config`

**Install download tools**
- `sudo apt-get install curl zip unzip tar`

Crossplatform installations
--------
**Install cmake**
- `python3 -m pip install cmake`

**Install Django**
- `python3 -m pip install Django`

#### Mongodb prerequisite
Application use mongodb for searching the nearest points. After download and install mongo, you need to create a file ./camelServe/cs.env based on ./camelServe/cs.env.example entry to connect application to mongod instance through mongocxx-driver.

Build and start server guild
--------
We start bootstraping project, this only need one time when you first clone project into your system.

**Bootstraping**
- `./bootstrap.sh`

#### Build application:
Depeding on what build tool you are using, run command corresponding
- Unix `$> cd build/` then `$> make`
- Window `$> cd build/` then `$> MSBuild .vcxproj`

#### Note:
This process may take minutes to complete. Take it easy. Then, start server by running service script.

**Service**
- `./service.sh`
