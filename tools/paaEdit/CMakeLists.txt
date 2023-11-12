add_executable(paaEdit main.cpp)
target_include_directories(paaEdit PRIVATE "../../include")
target_link_libraries(paaEdit LINK_PUBLIC Std++ Std++Static libBISMod)