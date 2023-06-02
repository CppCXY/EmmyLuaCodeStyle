use crate::syntax_def::{NodeGetter, SyntaxCommonDef, SyntaxDef, SyntaxKind, TokenGetter};
use serde::Serialize;
use std::io::Write;
use tera::{Context, Tera};
extern crate serde;

#[derive(Debug, Serialize)]
pub(crate) struct LuaGen {
    def_list: Vec<SyntaxDef>,
    common_def: SyntaxCommonDef,
}

impl LuaGen {
    pub fn new() -> Self {
        let def_list = vec![
            // stmt
            SyntaxDef::stmt("StatAssign")
                .kind("AssignStatement")
                .nodes(vec![
                    NodeGetter {
                        name: "get_var_list",
                        kind: "VarList",
                        single: true,
                    },
                    NodeGetter {
                        name: "get_expr_list",
                        kind: "ExprList",
                        single: true,
                    },
                ]),
            SyntaxDef::stmt("StatDo")
                .kind("DoStatement")
                .nodes(vec![NodeGetter {
                    name: "get_body",
                    kind: "Block",
                    single: true,
                }]),
            SyntaxDef::stmt("StatIf").kind("IfStatement").nodes(vec![
                NodeGetter {
                    name: "get_condition",
                    kind: "Expr",
                    single: true,
                },
                NodeGetter {
                    name: "get_body",
                    kind: "Block",
                    single: true,
                },
            ]),
            SyntaxDef::stmt("StatForRange")
                .kind("ForRangeStatement")
                .nodes(vec![
                    NodeGetter {
                        name: "get_params",
                        kind: "ParamNameDef",
                        single: false,
                    },
                    NodeGetter {
                        name: "get_expr_list",
                        kind: "ExprList",
                        single: true,
                    },
                    NodeGetter {
                        name: "get_body",
                        kind: "Block",
                        single: true,
                    },
                ]),
            SyntaxDef::stmt("StatFor")
                .kind("ForNumberStatement")
                .nodes(vec![
                    NodeGetter {
                        name: "get_param",
                        kind: "ParamNameDef",
                        single: true,
                    },
                    NodeGetter {
                        name: "get_exprs",
                        kind: "Expr",
                        single: false,
                    },
                    NodeGetter {
                        name: "get_body",
                        kind: "Block",
                        single: true,
                    },
                ]),
            SyntaxDef::stmt("StatLocalDef")
                .kind("LocalStatement")
                .nodes(vec![
                    NodeGetter {
                        name: "get_name_list",
                        kind: "NameList",
                        single: true,
                    },
                    NodeGetter {
                        name: "get_expr_list",
                        kind: "ExprList",
                        single: true,
                    },
                ]),
            SyntaxDef::stmt("StatGoTo")
                .kind("GotoStatement")
                .tokens(vec![TokenGetter {
                    name: "get_label",
                    kind: "ID",
                    single: true,
                }]),
            SyntaxDef::stmt("StatLabel")
                .kind("LabelStatement")
                .tokens(vec![TokenGetter {
                    name: "get_label",
                    kind: "ID",
                    single: true,
                }]),
            SyntaxDef::stmt("StatReturn")
                .kind("ReturnStatement")
                .nodes(vec![NodeGetter {
                    name: "get_expr_list",
                    kind: "ExprList",
                    single: true,
                }]),
            SyntaxDef::stmt("StatBreak").kind("BreakStatement"),
            SyntaxDef::stmt("StatRepeat")
                .kind("ForRangeStatement")
                .nodes(vec![
                    NodeGetter {
                        name: "get_body",
                        kind: "Block",
                        single: true,
                    },
                    NodeGetter {
                        name: "get_condition",
                        kind: "Expr",
                        single: true,
                    },
                ]),
            SyntaxDef::stmt("StatWhile")
                .kind("WhileStatement")
                .nodes(vec![
                    NodeGetter {
                        name: "get_body",
                        kind: "Block",
                        single: true,
                    },
                    NodeGetter {
                        name: "get_condition",
                        kind: "Expr",
                        single: true,
                    },
                ]),
            SyntaxDef::stmt("StatExpr")
                .kind("ExpressionStatement")
                .nodes(vec![NodeGetter {
                    name: "get_expr",
                    kind: "Expr",
                    single: true,
                }]),
            SyntaxDef::stmt("StatFunctionDef")
                .kind("FunctionStatement")
                .nodes(vec![
                    NodeGetter {
                        name: "get_params",
                        kind: "ParamNameDef",
                        single: false,
                    },
                    NodeGetter {
                        name: "get_body",
                        kind: "Block",
                        single: true,
                    },
                ])
                .exist_node("has_vararg", "ELLIPSIS"),
            SyntaxDef::stmt("LocalFunctionDef")
                .kind("LocalFunctionStatement")
                .nodes(vec![
                    NodeGetter {
                        name: "get_params",
                        kind: "ParamNameDef",
                        single: false,
                    },
                    NodeGetter {
                        name: "get_body",
                        kind: "Block",
                        single: true,
                    },
                ])
                .tokens(vec![TokenGetter {
                    name: "get_vararg",
                    kind: "ELLIPSIS",
                    single: true,
                }]),
            // expr
            SyntaxDef::expr("ExprClosure")
                .kind("ClosureExpression")
                .nodes(vec![
                    NodeGetter {
                        name: "get_params",
                        kind: "ParamNameDef",
                        single: false,
                    },
                    NodeGetter {
                        name: "get_body",
                        kind: "Block",
                        single: true,
                    },
                ])
                .exist_node("has_vararg", "ELLIPSIS"),
            SyntaxDef::expr("ExprLiteral")
                .kind("LiteralExpression")
                .tokens(vec![
                    TokenGetter {
                        name: "get_num",
                        kind: "NUMBER",
                        single: true,
                    },
                    TokenGetter {
                        name: "get_string",
                        kind: "STRING",
                        single: true,
                    },
                ]),
            SyntaxDef::expr("ExprUnary")
                .kind("UnaryExpression")
                .nodes(vec![NodeGetter {
                    name: "get_expr",
                    kind: "Expr",
                    single: true,
                }])
                .exist_node("is_not", "KeyNot")
                .exist_node("is_tilde", "TILDE"),
            SyntaxDef::expr("ExprBinary")
                .kind("BinaryExpression")
                .nodes(vec![NodeGetter {
                    name: "get_expr",
                    kind: "Expr",
                    single: false,
                }]),
            SyntaxDef::expr("ExprParen")
                .kind("ParExpression")
                .nodes(vec![NodeGetter {
                    name: "get_expr",
                    kind: "Expr",
                    single: true,
                }]),
            SyntaxDef::expr("ExprTable").kind("TableExpression"),
            SyntaxDef::expr("ExprIndex").kind("IndexExpression"),
            SyntaxDef::expr("ExprName").kind("NameExpression"),
            SyntaxDef::expr("ExprCall")
                .kind("CallExpression")
                .nodes(vec![NodeGetter {
                    name: "get_arg_list",
                    kind: "ListArgs",
                    single: true,
                }]),
            SyntaxDef::expr("ExprList").kind("ExpressionList"),
            SyntaxDef::node("Block")
                .kind("Body")
                .nodes(vec![NodeGetter {
                    name: "get_stmt_list",
                    kind: "Stmt",
                    single: false,
                }]),
            SyntaxDef::node("NameList")
                .kind("NameDefList")
                .nodes(vec![NodeGetter {
                    name: "get_name_def_list",
                    kind: "NameDef",
                    single: false,
                }]),
            SyntaxDef::node("NameDef").kind("NameDef"),
            SyntaxDef::node("ParamNameDef").kind("ParamNameDef"),
            SyntaxDef::node("ListArgs").nodes(vec![NodeGetter {
                name: "get_expr_list",
                kind: "Expr",
                single: false,
            }]),
            SyntaxDef::node("ClassMethodName"),
            SyntaxDef::node("TableField"),
            SyntaxDef::node("VarList")
                .kind("VarList")
                .nodes(vec![NodeGetter {
                    name: "get_expr_list",
                    kind: "Expr",
                    single: false,
                }]),
        ];
        return LuaGen {
            def_list,
            common_def: SyntaxCommonDef {
                prefix: "Lua",
                kind: "LuaSyntaxKind",
            },
        };
    }

    pub fn run(&self, path: &str) {
        let tera = Tera::new("gen/templates/*").unwrap();
        let mut f = std::fs::File::create(path).unwrap();
        f.write(
            tera.render("header.tl", &Context::default())
                .unwrap()
                .as_bytes(),
        )
        .unwrap();
        // class
        {
            for def in self.def_list.iter() {
                let mut ctx = Context::new();
                ctx.insert("def", def);
                ctx.insert("common", &self.common_def);
                let s = tera.render("syntax_def.tl", &ctx).unwrap();
                f.write(s.as_bytes()).unwrap();

                if !def.is_empty() {
                    let s = tera.render("syntax_impl.tl", &ctx).unwrap();
                    f.write(s.as_bytes()).unwrap();
                }
            }
        }
    }
}
