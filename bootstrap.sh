#! /bin/bash
echo -e "Bootstrap application before building"
if ! ls -AU "./camelServe/external/vcpkg" | read _;
then
    git submodule update --init
fi
./camelServe/external/vcpkg/bootstrap-vcpkg.sh
cmake -S camelServe/ -B build/ -DCMAKE_INSTALL_PREFIX=../camelServe.image -DBUILD_SHARED_LIBS=ON
