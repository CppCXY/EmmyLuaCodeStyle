cmake_minimum_required(VERSION 3.14)

project(CodeFormatServer)

add_executable(CodeFormatServer)

add_dependencies(CodeFormatServer CodeFormatCore)

target_include_directories(CodeFormatServer PRIVATE
        include
        ${LuaCodeStyle_SOURCE_DIR}/3rd/asio-1.24.0/include
        ${LuaCodeStyle_SOURCE_DIR}/3rd/nlohmann_json/include
        ${LuaCodeStyle_SOURCE_DIR}/3rd/mimalloc-2.0.9/include
        src
        )

target_compile_options(CodeFormatServer PRIVATE -DASIO_STANDALONE)

target_sources(CodeFormatServer
        PRIVATE
        src/main.cpp
        src/LanguageServer.cpp

        #Config
        src/Config/ClientConfig.cpp

        #LSP
        src/LSP/LSP.cpp
        src/LSP/LSPHandle.cpp
        #Session
        src/Session/IOSession.cpp
        src/Session/SocketIOSession.cpp
        src/Session/StandardIOSession.cpp
        #protocol
        src/Session/Protocol/ProtocolParser.cpp
        src/Session/Protocol/ProtocolBuffer.cpp

        #vfs
        src/VFS/VirtualFile.cpp
        src/VFS/VirtualFileSystem.cpp

        #DB
        src/DB/FileDB.cpp
        src/DB/UriDB.cpp
        src/DB/LineIndexDB.cpp

        #lib
        src/Lib/LineIndex/LineIndex.cpp
        src/Lib/LineIndex/LineTypes.cpp

        #service
        src/Service/Service.cpp
        src/Service/FormatService.cpp
        src/Service/DiagnosticService.cpp
        src/Service/CommandService.cpp
        src/Service/CodeActionService.cpp
        src/Service/ConfigService.cpp

        # mimalloc
        ${LuaCodeStyle_SOURCE_DIR}/3rd/mimalloc-2.0.9/src/static.c
        )

target_link_libraries(CodeFormatServer CodeFormatCore)


if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_compile_definitions(CodeFormatServer PUBLIC -D_WIN32_WINNT=0x0601)
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(CodeFormatServer pthread -static-libstdc++ -static-libgcc)
endif ()

install(
        TARGETS CodeFormatServer
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)
