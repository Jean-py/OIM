######################
#raytracing API


FILE(GLOB folder_source *.cpp)
FILE(GLOB folder_header *.hpp)
SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})


add_library (raytracing STATIC ${folder_source} ${folder_header})
target_link_libraries (raytracing IlmImf Half Iex ${PNG_LIBRARY})
