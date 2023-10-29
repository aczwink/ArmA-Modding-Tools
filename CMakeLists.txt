cmake_minimum_required(VERSION 3.6)
project(ArmA-Modding-Tools)
find_package(XP-CMake REQUIRED) #we need advanced cmake modules

set(CMAKE_CXX_STANDARD 20)

XPC_AutoBuildType()
XPC_SetCompileDefinitions()

find_package(StdPlusPlus REQUIRED)

add_subdirectory(tools/p3dEdit)
add_subdirectory(tools/raPEdit)
add_subdirectory(tools/wrpEdit)
add_subdirectory(tools/wssEdit)

add_subdirectory(include)
add_subdirectory(src)
add_library(libBISMod SHARED ${SOURCE_FILES})
target_include_directories(libBISMod PRIVATE "include")
target_link_libraries(libBISMod Std++)