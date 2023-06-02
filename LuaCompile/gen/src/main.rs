use lua_syntax_def_list::LuaGen;

mod syntax_def;
mod lua_syntax_def_list;

fn main() {
    let gen = LuaGen::new();
    gen.run("include/Lua/SyntaxNode/LuaSyntaxs.h")
}
