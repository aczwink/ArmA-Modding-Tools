add_executable(wrpEdit main.cpp)
target_include_directories(wrpEdit PRIVATE "../../include")
target_link_libraries(wrpEdit LINK_PUBLIC Std++ Std++Static libBISMod)