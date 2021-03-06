#ifndef TEZ_PARSER_H
#define TEZ_PARSER_H

#include <vector>

#include "syntax/ast.h"
#include "syntax/token.h"

namespace tez {

class Parser {
public:
    Parser(const std::vector<Token>& tokens);

    Ast_File parse();

    bool errors_reported() const;

private:
    /** Just get next token. */
    Token next_token();
    /** Validates token type and returns it. Error is reported if check fails. */
    Token next_token(Token_Type type);
    /** Peek token; depth = 0 returns current token. */
    Token peek_token(size_t depth = 1) const;
    /** Check next token type. If it matches any of given types, return it. */
    bool match_token(const std::initializer_list<Token_Type>& types);
    /** Check token type; returns false if eof. */
    bool check_token(Token_Type type) const;

    void report_error(const std::string& message);

    Ast_Node_Ptr statement();

    /** Parse all statements between '{' and '}'. */
    Ast_Block block();

    Ast_Node_Ptr namespace_declaration();
    Ast_Node_Ptr using_declaration();
    Ast_Node_Ptr function_declaration();
    Ast_Func_Decl::Param function_param();
    Ast_Node_Ptr function_call(const std::string& name);
    Ast_Node_Ptr variable_declaration(bool constant);
    Ast_Node_Ptr structure();
    Ast_Node_Ptr enumeration();
    Ast_Node_Ptr union_declaration();
    Ast_Node_Ptr array_initializer();

    Ast_Node_Ptr if_statement();
    Ast_Node_Ptr while_statement();
    Ast_Node_Ptr do_while_statement();
    Ast_Node_Ptr for_statement();

    Ast_Node_Ptr asm_block();

    // Recursive descent parsing. The lower the operator is in the hierarchy,
    // the higher precedence it have. It may be very bloated implementation,
    // but it's very safe and easy to maintain / extend.
    Ast_Node_Ptr expression();       // Base.
    Ast_Node_Ptr assignment();       // = += -= *= /= %=
    Ast_Node_Ptr logical_or();       // ||
    Ast_Node_Ptr logical_and();      // &&
    Ast_Node_Ptr bitwise_or();       // |
    Ast_Node_Ptr bitwise_xor();      // ^
    Ast_Node_Ptr bitwise_and();      // &
    Ast_Node_Ptr equality();         // == !=
    Ast_Node_Ptr comparison();       // >= <= > <
    Ast_Node_Ptr bitwise_shift();    // << >>
    Ast_Node_Ptr term();             // * / %
    Ast_Node_Ptr factor();           // + -
    Ast_Node_Ptr cast();             // 'as'
    Ast_Node_Ptr prefix_unary();     // ! -
    Ast_Node_Ptr postfix_unary();    // () [] . ->
    Ast_Node_Ptr scope_resolution(); // ::
    Ast_Node_Ptr primary();          // int, double, bool, null, identifier

    // Types also are parsed using recursive descent parsing, completely
    // separate for one for expressions.
    Ast_Node_Ptr type();
    Ast_Node_Ptr type_array();              // [type, size]
    Ast_Node_Ptr type_pointer();            // ^
    Ast_Node_Ptr type_scope_resolution();   // ::
    Ast_Node_Ptr type_identifier();         // identifier

    const std::vector<Token> _tokens;
    unsigned int             _current = 0;

    bool _errors_reported = false;
};

}

#endif //TEZ_PARSER_H

