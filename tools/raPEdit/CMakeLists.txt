add_executable(raPEdit main.cpp)
target_include_directories(raPEdit PRIVATE "../../include")
target_link_libraries(raPEdit LINK_PUBLIC Std++ Std++Static libBISMod)