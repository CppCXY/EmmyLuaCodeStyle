cmake_minimum_required(VERSION 3.11)

project(CodeFormatServer)

add_executable(CodeFormatServer)

add_dependencies(CodeFormatServer CodeService)

target_include_directories(CodeFormatServer PUBLIC
	${LuaCodeStyle_SOURCE_DIR}/include
	${LuaCodeStyle_SOURCE_DIR}/3rd/asio-1.20.0/include
	${LuaCodeStyle_SOURCE_DIR}/3rd/nlohmann_json/include
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
	${CodeFormatServer_SOURCE_DIR}/src/Session/IOSession.cpp
	${CodeFormatServer_SOURCE_DIR}/src/Session/SocketIOSession.cpp
	${CodeFormatServer_SOURCE_DIR}/src/Session/StandardIOSession.cpp

	#protocol
	${CodeFormatServer_SOURCE_DIR}/src/Protocol/ProtocolParser.cpp
	${CodeFormatServer_SOURCE_DIR}/src/Protocol/ProtocolBuffer.cpp
	# protocol/request
	${CodeFormatServer_SOURCE_DIR}/src/Protocol/Request/JsonRequestProtocol.cpp
	${CodeFormatServer_SOURCE_DIR}/src/Protocol/Request/RequestProtocolFactory.cpp
	${CodeFormatServer_SOURCE_DIR}/src/Protocol/Request/InitializeRequestProtocol.cpp
)

target_link_libraries(CodeFormatServer PUBLIC CodeService)

#if(WIN32)
#	target_link_libraries(CodeFormatServer PUBLIC -lwsock32 -lws2_32)
#endif()