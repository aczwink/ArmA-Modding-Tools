add_executable(p3dEdit main.cpp)
target_include_directories(p3dEdit PRIVATE "../../include")
target_link_libraries(p3dEdit LINK_PUBLIC Std++ Std++Static libBISMod)