cmake_minimum_required(VERSION 3.14)

project(CodeFormat)

add_executable(CodeFormat)

add_dependencies(CodeFormat CodeFormatCore Util)

target_include_directories(CodeFormat PRIVATE
	src
)

target_sources(CodeFormat
	PRIVATE
	src/CodeFormat.cpp
	src/LuaFormat.cpp
)

target_link_libraries(CodeFormat CodeFormatCore Util)

if(CMAKE_SYSTEM_NAME STREQUAL Linux)
	target_link_libraries(CodeFormat -static-libstdc++ -static-libgcc)	
endif()

install(
    TARGETS CodeFormat
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
