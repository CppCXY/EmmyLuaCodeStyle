local git_ref = '1.5.7'
local modrev = 'scm'
local specrev = '1'

rockspec_format = '3.0'
package = 'EmmyLuaCodeStyle'
version = modrev .. '-' .. specrev

local repo_url = 'https://github.com/CppCXY/EmmyLuaCodeStyle'

description = {
  summary = 'fast, powerful, and feature-rich Lua formatting and checking tool.',
  detailed =
  [[Lua code parsing, formatting, and code diagnosis algorithm library and language service implementation based on C++]],
  labels = { 'lua' },
  homepage = repo_url,
  license = 'MIT'
}

source = {
  url = repo_url .. '/archive/' .. git_ref .. '.zip',
  dir = package .. '-' .. git_ref,
}

build = {
  type = 'cmake',
  variables = {
    CMAKE_INSTALL_PREFIX = 'build',
  },
  install = {
    bin = {
      CodeFormat = 'build/bin/CodeFormat',
      CodeFormatServer = 'build/bin/CodeFormatServer',
    },
  }
}
