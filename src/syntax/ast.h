#ifndef TEZ_AST_H
#define TEZ_AST_H

#include <string>
#include <memory>
#include <vector>

namespace tez {

enum class Ast_Node_Type {
    UNDEFINED,

    NAMESPACE,
    BLOCK,

    BOOLEAN,
    INTEGER,
    REAL_NUMBER,
    STRING,

    IDENTIFIER,

    UNARY_OPERATION,
    BINARY_OPERATION,
    GROUPING_EXPRESSION,
    CAST,

    FUNCTION_DECLARATION,
    FUNCTION_CALL,
    RETURN,
    VARIABLE_DECLARATION,

    IF,
    WHILE,
    DO_WHILE,
    FOR,
    BREAK,
    CONTINUE,

    STRUCT,
    ENUM,
    UNION,  // Not implemented yet.
    CLASS,  // Not implemented yet.
};

struct Ast_Node {
    virtual ~Ast_Node() = default;
    Ast_Node_Type node_type = Ast_Node_Type::UNDEFINED;
};
using Ast_Node_Ptr = std::unique_ptr<Ast_Node>;
using Ast          = std::vector<Ast_Node_Ptr>;

struct Ast_Block final : Ast_Node {
    Ast_Block() { node_type = Ast_Node_Type::BLOCK; }
    std::vector<Ast_Node_Ptr> statements;
};

struct Ast_Namespace final : Ast_Node {
    Ast_Namespace() { node_type = Ast_Node_Type::NAMESPACE; }
    std::string name;
    Ast_Block   body;
};

struct Ast_Boolean final : Ast_Node {
    Ast_Boolean() { node_type = Ast_Node_Type::BOOLEAN; }
    bool value;
};

struct Ast_Integer final : Ast_Node {
    Ast_Integer() { node_type = Ast_Node_Type::INTEGER; }
    long long value;
};

struct Ast_Real_Number final : Ast_Node {
    Ast_Real_Number() { node_type = Ast_Node_Type::REAL_NUMBER; }
    double value;
};

struct Ast_String final : Ast_Node {
    Ast_String() { node_type = Ast_Node_Type::STRING; }
    std::string value;
};

struct Ast_Identifier final : Ast_Node {
    Ast_Identifier() { node_type = Ast_Node_Type::IDENTIFIER; }
    std::string name;
};

struct Ast_Unary_Operation final : Ast_Node {
    Ast_Unary_Operation() { node_type = Ast_Node_Type::UNARY_OPERATION; }
    Ast_Node_Ptr left;
    std::string  operat;
};

struct Ast_Binary_Operation final : Ast_Node {
    Ast_Binary_Operation() { node_type = Ast_Node_Type::BINARY_OPERATION; }
    Ast_Node_Ptr left;
    Ast_Node_Ptr right;
    std::string  operat;
};

struct Ast_Grouping_Expression final : Ast_Node {
    Ast_Grouping_Expression() { node_type = Ast_Node_Type::GROUPING_EXPRESSION; }
    Ast_Node_Ptr expr;
};

struct Ast_Cast final : Ast_Node {
    Ast_Cast() { node_type = Ast_Node_Type::CAST; }
    Ast_Node_Ptr expr;      // Expression to cast.
    Ast_Node_Ptr to;        // Type to cast to.
};

struct Ast_Func_Decl final : Ast_Node {
    struct Param {
        std::string name;
        bool        constant;
        std::string type;
    };
    Ast_Func_Decl() { node_type = Ast_Node_Type::FUNCTION_DECLARATION; }
    std::string        name;
    std::vector<Param> params;
    std::string        return_type;
    Ast_Block          body;
};

struct Ast_Return final : Ast_Node {
    Ast_Return() { node_type = Ast_Node_Type::RETURN; }
    Ast_Node_Ptr expr;
};

struct Ast_Func_Call final : Ast_Node {
    Ast_Func_Call() { node_type = Ast_Node_Type::FUNCTION_CALL; }
    std::string               name;
    std::vector<Ast_Node_Ptr> args;
};

struct Ast_Var_Decl final : Ast_Node {
    Ast_Var_Decl() { node_type = Ast_Node_Type::VARIABLE_DECLARATION; }
    std::string  name;
    bool         constant;
    std::string  type;
    Ast_Node_Ptr initializer;
};

struct Ast_If final : Ast_Node {
    Ast_If() { node_type = Ast_Node_Type::IF; }
    Ast_Node_Ptr condition;
    Ast_Block    if_block;
    Ast_Node_Ptr else_block;
};

struct Ast_While final : Ast_Node {
    Ast_While() { node_type = Ast_Node_Type::WHILE; }
    Ast_Node_Ptr condition;
    Ast_Block    body;
};

struct Ast_Do_While final : Ast_Node {
    Ast_Do_While() { node_type = Ast_Node_Type::DO_WHILE; }
    Ast_Node_Ptr condition;
    Ast_Block    body;
};

struct Ast_For final : Ast_Node {
    Ast_For() { node_type = Ast_Node_Type::FOR; }
    Ast_Node_Ptr init_statement;
    Ast_Node_Ptr condition;
    Ast_Node_Ptr iteration_expr;
    Ast_Block    body;
};

struct Ast_Break final : Ast_Node {
    Ast_Break() { node_type = Ast_Node_Type::BREAK; }
};

struct Ast_Continue final : Ast_Node {
    Ast_Continue() { node_type = Ast_Node_Type::CONTINUE; }
};

struct Ast_Struct final : Ast_Node {
    struct Field {
        std::string name;
        std::string type;
    };
    Ast_Struct() { node_type = Ast_Node_Type::STRUCT; }
    std::string        name;
    std::vector<Field> fields;
};

struct Ast_Enum final : Ast_Node {
    struct Enumerator {
        std::string name;
        int         value;
    };
    Ast_Enum() { node_type = Ast_Node_Type::ENUM; }
    std::string             name;
    std::vector<Enumerator> enumerations;
};

}

#endif //TEZ_AST_H
