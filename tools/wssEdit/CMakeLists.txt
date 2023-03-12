add_executable(wssEdit main.cpp)
target_include_directories(wssEdit PRIVATE "../../include")
target_link_libraries(wssEdit LINK_PUBLIC Std++ Std++Static libBISMod)