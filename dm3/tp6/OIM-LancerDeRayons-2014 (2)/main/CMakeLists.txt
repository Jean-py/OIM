######################
#main program

set(Sources
  main.cpp
  texturefetch.cpp
)

find_package(PNG REQUIRED)

include_directories (../coreraytracer ${PNG_INCLUDE_DIR})

add_executable (main ${Sources})
target_link_libraries (main raytracing )

