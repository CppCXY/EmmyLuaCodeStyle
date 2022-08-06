# uriparser - RFC 3986 URI parsing library
#
# Copyright (C) 2018, Sebastian Pipping <sebastian@pipping.org>
# All rights reserved.
#
# Redistribution and use in source  and binary forms, with or without
# modification, are permitted provided  that the following conditions
# are met:
#
#     1. Redistributions  of  source  code   must  retain  the  above
#        copyright notice, this list  of conditions and the following
#        disclaimer.
#
#     2. Redistributions  in binary  form  must  reproduce the  above
#        copyright notice, this list  of conditions and the following
#        disclaimer  in  the  documentation  and/or  other  materials
#        provided with the distribution.
#
#     3. Neither the  name of the  copyright holder nor the  names of
#        its contributors may be used  to endorse or promote products
#        derived from  this software  without specific  prior written
#        permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND  ANY EXPRESS OR IMPLIED WARRANTIES,  INCLUDING, BUT NOT
# LIMITED TO,  THE IMPLIED WARRANTIES OF  MERCHANTABILITY AND FITNESS
# FOR  A  PARTICULAR  PURPOSE  ARE  DISCLAIMED.  IN  NO  EVENT  SHALL
# THE  COPYRIGHT HOLDER  OR CONTRIBUTORS  BE LIABLE  FOR ANY  DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL,  EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA,  OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT  LIABILITY,  OR  TORT (INCLUDING  NEGLIGENCE  OR  OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGE.
#

# I modified the library to get the CMake file by @CppCXY
cmake_minimum_required(VERSION 3.11)

project(uriparser
    VERSION
        0.9.6
    LANGUAGES
        C
)

add_library(uriparser STATIC)

target_include_directories(uriparser PUBLIC
	${uriparser_SOURCE_DIR}/include
	PRIVATE
	${uriparser_SOURCE_DIR}/src
)
#
# C library
#
target_sources(uriparser
	PUBLIC
    ${uriparser_SOURCE_DIR}/include/uriparser/UriBase.h
    ${uriparser_SOURCE_DIR}/include/uriparser/UriDefsAnsi.h
    ${uriparser_SOURCE_DIR}/include/uriparser/UriDefsConfig.h
    ${uriparser_SOURCE_DIR}/include/uriparser/UriDefsUnicode.h
    ${uriparser_SOURCE_DIR}/include/uriparser/Uri.h
    ${uriparser_SOURCE_DIR}/include/uriparser/UriIp4.h
    PRIVATE
    ${uriparser_SOURCE_DIR}/src/UriCommon.c
    ${uriparser_SOURCE_DIR}/src/UriCommon.h
    ${uriparser_SOURCE_DIR}/src/UriCompare.c
    ${uriparser_SOURCE_DIR}/src/UriEscape.c
    ${uriparser_SOURCE_DIR}/src/UriFile.c
    ${uriparser_SOURCE_DIR}/src/UriIp4Base.c
    ${uriparser_SOURCE_DIR}/src/UriIp4Base.h
    ${uriparser_SOURCE_DIR}/src/UriIp4.c
    ${uriparser_SOURCE_DIR}/src/UriMemory.c
    ${uriparser_SOURCE_DIR}/src/UriMemory.h
    ${uriparser_SOURCE_DIR}/src/UriNormalizeBase.c
    ${uriparser_SOURCE_DIR}/src/UriNormalizeBase.h
    ${uriparser_SOURCE_DIR}/src/UriNormalize.c
    ${uriparser_SOURCE_DIR}/src/UriParseBase.c
    ${uriparser_SOURCE_DIR}/src/UriParseBase.h
    ${uriparser_SOURCE_DIR}/src/UriParse.c
    ${uriparser_SOURCE_DIR}/src/UriQuery.c
    ${uriparser_SOURCE_DIR}/src/UriRecompose.c
    ${uriparser_SOURCE_DIR}/src/UriResolve.c
    ${uriparser_SOURCE_DIR}/src/UriShorten.c
)

configure_file(${uriparser_SOURCE_DIR}/src/UriConfig.h.in ${uriparser_SOURCE_DIR}/src/config.h)

target_compile_definitions(uriparser PRIVATE URI_LIBRARY_BUILD)

target_compile_definitions(uriparser PUBLIC URI_STATIC_BUILD)

