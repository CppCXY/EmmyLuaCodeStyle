cmake_minimum_required(VERSION 3.11)

project(CodeService)

add_executable(CodeService)

add_dependencies(CodeService LuaParser)

target_include_directories(CodeService PUBLIC
	${LuaCodeStyle_SOURCE_DIR}/include
	src
)

target_sources(CodeService PRIVATE
	${CodeService_SOURCE_DIR}/src/CodeService.cpp	
)

target_link_libraries(CodeService LuaParser)