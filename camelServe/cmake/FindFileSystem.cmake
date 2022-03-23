include(CMakePushCheckState)
include(CheckIncludeFileCXX)
include(CheckCXXSourceCompiles)

cmake_push_check_state(RESET)
set(CS_HAVE_FS)
check_include_file_cxx("filesystem" CS_HAVE_STD_FILESYSTEM)
check_include_file_cxx("experimental/filesystem" CS_HAVE_EXPERIMENTAL_FILESYSTEM)
