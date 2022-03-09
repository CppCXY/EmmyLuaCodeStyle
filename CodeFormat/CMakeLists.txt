cmake_minimum_required(VERSION 3.11)

project(CodeFormat)

add_executable(CodeFormat)

add_dependencies(CodeFormat CodeService Util)

target_include_directories(CodeFormat PUBLIC
	${LuaCodeStyle_SOURCE_DIR}/include
	src
)

target_sources(CodeFormat
	PUBLIC
	${CodeFormat_SOURCE_DIR}/src/CodeFormat.cpp
	${CodeFormat_SOURCE_DIR}/src/LuaFormat.cpp
)

target_link_libraries(CodeFormat CodeService Util)

if(CMAKE_SYSTEM_NAME STREQUAL Linux)
	target_link_libraries(CodeFormat -static-libstdc++ -static-libgcc)	
endif()

install(
    TARGETS CodeFormat
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
