# camelMap
camelMap is a project that finding shortest path on map in real world. Using Bidirection A* Landmark Algorithm implemented in c++ and comunicate with django serverside web base through unix socket.

<img width="960" alt="camelMap" src="https://user-images.githubusercontent.com/59383987/162615582-62798d26-731f-4196-95bf-de3d7ee5b46d.png">

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

Mongodb prerequisite
--------
Application use mongodb for searching the nearest points. After download and install mongo, you need to create a file ./camelServe/cs.env based on ./camelServe/cs.env.example entry to connect application to mongod instance through mongocxx-driver.

**Configure file cs.env**
- `CS_MONGO_HOST_NAME=127.0.0.1` 
- `CS_MONGO_PORT=27017` 
- `CS_MONGO_USER_NAME=` 
- `CS_MONGO_PASSWORD=` 
- `CS_SERVICE_THREAD_POOL_SIZE=100` 

**Create Database**
- Open MongoDBCompass and connect to 127.0.0.1:27017 
- Create a DatabaseName with name: `CamelMap`
- Create a Collection with name: `US_NewYork_Coor`
- Import a data file into US_NewYork_Coor: [File USA-road-mongo.NY](https://drive.google.com/file/d/1cpRJ0yV_DZ1Ixb8ANOl6Ttjn9Qyi7Sir/view?usp=sharing).



Build and start server guild
--------
We start bootstraping project, this only need one time when you first clone project into your system or everytime you change the configuration

**Bootstraping**
- `./bootstrap.sh`

#### Build application:
Depeding on what build tool you are using, run command corresponding
- Unix `$> cd build/` then `$> make` then `$ ./camelServe`
<img width="954" alt="build" src="https://user-images.githubusercontent.com/59383987/162615786-079aff5c-dd41-4173-a3f2-f846512d932f.png">
<img width="958" alt="build_binary" src="https://user-images.githubusercontent.com/59383987/162615832-6806e807-ded6-4b26-886d-ee3768c53e8f.png">

- Window `$> cd build/` then `$> MSBuild .vcxproj`

#### Note:
This process may take minutes to complete. Take it easy. Then, start server by running service script.

**Service**
- `./service.sh`
