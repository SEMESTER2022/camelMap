#! /bin/bash
echo -e "Bootstrap application before building"
if ! ls -AU "./camelServe/external/vcpkg" | read _;
then
    git submodule update --init
fi
./camelServe/external/vcpkg/bootstrap-vcpkg.sh
