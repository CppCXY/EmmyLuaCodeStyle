cmake_minimum_required(VERSION 3.11)

project(CodeFormatServer)

add_executable(CodeFormatServer)

add_dependencies(CodeFormatServer CodeService)

target_include_directories(CodeFormatServer PUBLIC
	${LuaCodeStyle_SOURCE_DIR}/include
	${LuaCodeStyle_SOURCE_DIR}/3rd/asio-1.20.0/include
	${LuaCodeStyle_SOURCE_DIR}/3rd/nlohmann_json/include
	${LuaCodeStyle_SOURCE_DIR}/3rd/mimalloc-2.0.3/include
	src
)

target_compile_options(CodeFormatServer PUBLIC -DASIO_STANDALONE)

target_sources(CodeFormatServer
	PUBLIC
	${LuaCodeStyle_SOURCE_DIR}/include/CodeFormatServer/Session/IOSession.h
	${LuaCodeStyle_SOURCE_DIR}/include/CodeFormatServer/Session/SocketIOSession.h
	${LuaCodeStyle_SOURCE_DIR}/include/CodeFormatServer/Session/StandardIOSession.h

	${LuaCodeStyle_SOURCE_DIR}/include/CodeFormatServer/Protocol/ProtocolParser.h

	${LuaCodeStyle_SOURCE_DIR}/include/CodeFormatServer/LanguageService.h
	${LuaCodeStyle_SOURCE_DIR}/include/CodeFormatServer/LanguageClient.h

	PRIVATE
	${CodeFormatServer_SOURCE_DIR}/src/CodeFormatServer.cpp
	${CodeFormatServer_SOURCE_DIR}/src/LanguageClient.cpp
	${CodeFormatServer_SOURCE_DIR}/src/LanguageService.cpp
	${CodeFormatServer_SOURCE_DIR}/src/VirtualFile/VirtualFile.cpp
	${CodeFormatServer_SOURCE_DIR}/src/Session/IOSession.cpp
	${CodeFormatServer_SOURCE_DIR}/src/Session/SocketIOSession.cpp
	${CodeFormatServer_SOURCE_DIR}/src/Session/StandardIOSession.cpp

	#protocol
	${CodeFormatServer_SOURCE_DIR}/src/Protocol/ProtocolParser.cpp
	${CodeFormatServer_SOURCE_DIR}/src/Protocol/ProtocolBuffer.cpp

	${CodeFormatServer_SOURCE_DIR}/src/VSCode.cpp
	
	#Service/indexs
	${CodeFormatServer_SOURCE_DIR}/src/Service/Indexs/ModuleIndex.cpp

	#Service/indexs/Config
	${CodeFormatServer_SOURCE_DIR}/src/Service/Indexs/Config/ModuleConfig.cpp

	#Service/AstUtil
	${CodeFormatServer_SOURCE_DIR}/src/Service/AstUtil/ModuleFinder.cpp
	${CodeFormatServer_SOURCE_DIR}/src/Service/AstUtil/AstPattern.cpp

	#service
	${CodeFormatServer_SOURCE_DIR}/src/Service/Service.cpp
	${CodeFormatServer_SOURCE_DIR}/src/Service/ModuleService.cpp
	${CodeFormatServer_SOURCE_DIR}/src/Service/CodeFormatService.cpp
	${CodeFormatServer_SOURCE_DIR}/src/Service/CompletionService.cpp
	${CodeFormatServer_SOURCE_DIR}/src/Service/CommandService.cpp
	${CodeFormatServer_SOURCE_DIR}/src/Service/CodeActionService.cpp

	# mimalloc
	${LuaCodeStyle_SOURCE_DIR}/3rd/mimalloc-2.0.3/src/static.c
)

target_link_libraries(CodeFormatServer PUBLIC CodeService)


if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	target_compile_definitions(CodeFormatServer PUBLIC -D_WIN32_WINNT=0x0601)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	target_link_libraries(CodeFormatServer PUBLIC pthread -static-libstdc++ -static-libgcc)
endif()

install(
    TARGETS CodeFormatServer
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
