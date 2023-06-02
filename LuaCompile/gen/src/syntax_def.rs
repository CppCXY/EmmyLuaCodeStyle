use std::vec;

use serde::Serialize;

#[derive(Debug, Serialize, PartialEq)]
pub(crate) enum SyntaxKind {
    Stmt,
    Expr,
    Node,
    Tag,
    Type,
}

#[derive(Debug, Serialize)]
pub(crate) struct TokenGetter {
    pub name: &'static str,
    pub kind: &'static str,
    pub single: bool,
}

#[derive(Debug, Serialize)]
pub(crate) struct NodeGetter {
    pub name: &'static str,
    pub kind: &'static str,
    pub single: bool,
}

#[derive(Debug, Serialize)]
pub(crate) struct TokenExist {
    pub name: &'static str,
    pub kind: &'static str,
}

#[derive(Debug, Serialize)]
pub(crate) struct SyntaxCommonDef {
    pub prefix: &'static str,
    pub kind: &'static str,
}

#[derive(Debug, Serialize)]
pub(crate) struct SyntaxDef {
    pub syntax_kind: SyntaxKind,
    pub name: String,

    pub single: bool,
    pub kind: &'static str,
    pub kinds: Vec<&'static str>,
    pub token_getters: Vec<TokenGetter>,
    pub node_getters: Vec<NodeGetter>,
    pub implements: Vec<&'static str>,
    pub node_exists: Vec<TokenExist>,
}

impl SyntaxDef {
    fn new(name: &str, syntax_kind: SyntaxKind) -> Self {
        SyntaxDef {
            syntax_kind,
            name: name.to_string(),
            single: true,
            kind: "",
            kinds: vec![],
            token_getters: vec![],
            node_getters: vec![],
            implements: vec![],
            node_exists: vec![],
        }
    }

    pub fn node(name: &str) -> Self {
        SyntaxDef::new(name, SyntaxKind::Node)
    }

    pub fn stmt(name: &str) -> Self {
        SyntaxDef::new(name, SyntaxKind::Stmt)
    }

    pub fn expr(name: &str) -> Self {
        SyntaxDef::new(name, SyntaxKind::Expr)
    }

    pub fn tag(name: &str) -> Self {
        SyntaxDef::new(name, SyntaxKind::Tag)
    }

    pub fn ty(name: &str) -> Self {
        SyntaxDef::new(name, SyntaxKind::Type)
    }

    pub fn kind(mut self, kind:&'static str) -> Self {
        self.kind = kind;
        self.single = true;
        self
    }

    pub fn kinds(mut self, kinds: Vec<&'static str>) -> Self {
        self.kinds = kinds;
        self.single = false;
        self   
    }

    #[allow(unused)]
    pub fn tokens(mut self, getters: Vec<TokenGetter>) -> Self {
        self.token_getters = getters;
        self
    }

    pub fn nodes(mut self, getters: Vec<NodeGetter>) -> Self {
        self.node_getters = getters;
        self
    }

    pub fn implement(mut self, trait_name: &'static str) -> Self {
        self.implements.push(trait_name);
        self
    }

    pub fn exist_node(mut self, name: &'static str, kind: &'static str) -> Self {
        self.node_exists.push(TokenExist { name, kind });
        self
    }

    pub fn is_empty(&self) -> bool {
        self.implements.is_empty() && self.node_getters.is_empty() && self.token_getters.is_empty()
    }
}
