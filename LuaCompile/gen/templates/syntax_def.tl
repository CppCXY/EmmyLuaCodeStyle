//
// {{common.prefix}}{{def.name}}
//
class {{common.prefix}}{{def.name}} : public LuaSyntaxNode {
public:
{%- if def.single %}
    bool CanCast(LuaSyntaxNodeKind kind) {
        return kind == LuaSyntaxNodeKind::{{def.kind}};
    }
{%- else %}
    bool CanCast(LuaSyntaxNodeKind kind) {
        switch(kind) {
            {%- for k in def.kinds %}
            case LuaSyntaxNodeKind::{{k}}:
            {%- endif %}
            {
                return true;
            }
            default: 
            {
                return false;
            }
        }
        return kind == LuaSyntaxNodeKind::{{def.kind}};
    }
{%- endif %}

    explicit {{common.prefix}}{{def.name}}(LuaNodeOrToken n): LuaSyntaxNode(n) {}

{%- for n in def.node_getters %}
    {%- if n.single %}
    {{common.prefix}}{{n.kind}} {{n.name}}(const LuaSyntaxTree& t) const;
    {%- else %}
    std::vector<{{common.prefix}}{{n.kind}}> {{n.name}}(const LuaSyntaxTree& t) const;
    {%- endif %}
{%- endfor %}
{%- for n in def.token_getters %}
    {%- if n.single %}
    LuaSyntaxToken {{n.name}}(const LuaSyntaxTree& t) const;
    {%- else %}
    std::vector<LuaSyntaxToken> {{n.name}}(const LuaSyntaxTree& t) const;
    {%- endif %}
{%- endfor %}
};

