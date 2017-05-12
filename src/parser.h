#ifndef WAYWARD_PARSER_H
#define WAYWARD_PARSER_H

#include <array>
#include <string>

#include "ast.h"
#include "lexer.h"

namespace wayward {

class parser {
public:
    parser(const std::vector<token>& tokens);

    ast_program parse();

private:
    // Get next token.
    token next_token();

    // Get next token and validate its type.
    token next_token(token_type type);

    // Get next token and validate its both type and value.
    token next_token(token_type type, const std::string& value);

    // Get current token.
    token current_token() const;

    token peek_token(unsigned int depth = 1);
    
    ast_node_ptr parse_expression(const token& token);
    std::unique_ptr<ast_function_declaration> parse_function();
    void parse_variable();
    void parse_constant();

    const std::vector<token>& _tokens;  // Vector of tokens to parse.
    unsigned int _current_token = 0;    // Current token counter.

    const std::array<std::string, 3> _keywords{{
            "func", "var", "let"}};
};

}

#endif //WAYWARD_PARSER_H

