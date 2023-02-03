# Auto Import

Nowadays, most language plugins have auto import functions. Due to the inconsistent modularization of Lua in this respect, there are large differences in personal understanding and use of modules, resulting in almost no auto import plugins.

This project provides a commonly used auto import function that is almost a standard way of importing modules.

## lua.module.json configuration file

The auto import function no longer uses the .editorconfig file as a configuration file. Use lua.module.json as the configuration file. In which directory the file is located, all lua files in that directory will use lua.module.json in that directory as the module configuration. And the starting directory of the extension is this directory.

If there are multiple lua.module.json in the workspace, the principle of proximity is adopted, which means that the module configuration of a file is based on the lua.module.json closest to it.

### lua.module.json basic form
```json
{
     "name": "root",
     "import": [
         "root"
     ]
}
```

This basic form defines the module name and the imported module name, that is to say, after the module (lua.modudle.json is called the module) defines its own name, the name can be imported through the import field for all lua files that the module can affect , You can only see the lua module path introduced by the module name introduced in the import field.

In other words for the following directory structure
```plaintext
scirpt
|-lua.module.json
|-aaa.lua
|-lib
| |-lua.module.json
| |-bbb.lua
```

If the contents of lua.module.json are respectively
```json
// script/lua.module.json
{
     "name": "root",
     "import": [
         "root", "lib"
     ]
}
// script/lib/lua.module.json
{
     "name": "lib",
     "import": [
         "lib"
     ]
}
```

Then the bbb file in the lib directory cannot see the aaa.lua file during auto import because it did not introduce the root module
The script/aaa.lua file can see the bbb.lua file because it introduced the lib module name

### Configuration "separator"

This configuration item determines the path separator symbol during auto import. Generally speaking, Lua recommends'.' as the module path separator symbol, but many items are not sufficiently standardized to use'/' as the separator.

The default value of this configuration item is'.'

### Configuration "import.function"

This configuration item indicates the import function used in auto import. Generally speaking, the Lua standard uses the require function, but many projects have also created a function with slightly different usage and semantics to import modules such as import

The default value of this configuration item is "require"

### Configuration item special.module

The configuration item is an array, the basic form is
```json
{
     "special.module": [
         {"name": "socket_core", "module": "socket.core"}
     ]
}
```
This configuration item is to set the auto import matching name of some existing modules. When the above settings are set, the socket.core module will be automatically imported when socket_core is entered.
