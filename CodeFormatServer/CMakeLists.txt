cmake_minimum_required(VERSION 3.11)

project(CodeFormatServer)

add_executable(CodeFormatServer)

add_dependencies(CodeFormatServer CodeService)

target_include_directories(CodeFormatServer PUBLIC
        ${LuaCodeStyle_SOURCE_DIR}/include
        ${LuaCodeStyle_SOURCE_DIR}/3rd/asio-1.24.0/include
        ${LuaCodeStyle_SOURCE_DIR}/3rd/nlohmann_json/include
        ${LuaCodeStyle_SOURCE_DIR}/3rd/mimalloc-2.0.3/include
        src
        )

target_compile_options(CodeFormatServer PUBLIC -DASIO_STANDALONE)

target_sources(CodeFormatServer
        PRIVATE
        ${CodeFormatServer_SOURCE_DIR}/src/main.cpp
        ${CodeFormatServer_SOURCE_DIR}/src/LanguageServer.cpp

        #Config
        ${CodeFormatServer_SOURCE_DIR}/src/Config/ClientConfig.cpp

        #LSP
        ${CodeFormatServer_SOURCE_DIR}/src/LSP/LSP.cpp
        ${CodeFormatServer_SOURCE_DIR}/src/LSP/LSPHandle.cpp
        #Session
        ${CodeFormatServer_SOURCE_DIR}/src/Session/IOSession.cpp
        ${CodeFormatServer_SOURCE_DIR}/src/Session/SocketIOSession.cpp
        ${CodeFormatServer_SOURCE_DIR}/src/Session/StandardIOSession.cpp
        #protocol
        ${CodeFormatServer_SOURCE_DIR}/src/Session/Protocol/ProtocolParser.cpp
        ${CodeFormatServer_SOURCE_DIR}/src/Session/Protocol/ProtocolBuffer.cpp

        #vfs
        ${CodeFormatServer_SOURCE_DIR}/src/VFS/VirtualFile.cpp
        ${CodeFormatServer_SOURCE_DIR}/src/VFS/VirtualFileSystem.cpp

        #DB
        ${CodeFormatServer_SOURCE_DIR}/src/DB/FileDB.cpp
        ${CodeFormatServer_SOURCE_DIR}/src/DB/UriDB.cpp
        ${CodeFormatServer_SOURCE_DIR}/src/DB/LineIndexDB.cpp

        #lib
        ${CodeFormatServer_SOURCE_DIR}/src/Lib/LineIndex/LineIndex.cpp
        ${CodeFormatServer_SOURCE_DIR}/src/Lib/LineIndex/LineTypes.cpp

        #service
        ${CodeFormatServer_SOURCE_DIR}/src/Service/Service.cpp
        ${CodeFormatServer_SOURCE_DIR}/src/Service/FormatService.cpp
        ${CodeFormatServer_SOURCE_DIR}/src/Service/DiagnosticService.cpp
        ${CodeFormatServer_SOURCE_DIR}/src/Service/CommandService.cpp
        ${CodeFormatServer_SOURCE_DIR}/src/Service/CodeActionService.cpp
        ${CodeFormatServer_SOURCE_DIR}/src/Service/ConfigService.cpp

        # mimalloc
        ${LuaCodeStyle_SOURCE_DIR}/3rd/mimalloc-2.0.3/src/static.c
        )

target_link_libraries(CodeFormatServer PUBLIC CodeService)


if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_compile_definitions(CodeFormatServer PUBLIC -D_WIN32_WINNT=0x0601)
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(CodeFormatServer PUBLIC pthread -static-libstdc++ -static-libgcc)
endif ()

install(
        TARGETS CodeFormatServer
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)
