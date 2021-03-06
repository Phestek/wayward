#include "lexer.h"

#include <cctype>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace tez {

namespace {

class File_Not_Found : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

// Get entire file content, including newlines.
// NOTE: leaves empty line on the end.
std::string read_file_content(const std::string& filename) {
    std::ifstream file{filename};
    if(!file.good()) {
        throw File_Not_Found{"Failed to open file \"" + filename + "\".\n"};
    }
    std::string content;
    while(!file.eof()) {
        std::string line;
        std::getline(file, line);
        if(line != "\n") {
            content += line + '\n';
        }
    }
    return content;
}

}

const std::map<std::string, Token_Type> Lexer::_keywords{
        {"namespace", Token_Type::KW_NAMESPACE},
        {"func",      Token_Type::KW_FUNC},
        {"return",    Token_Type::KW_RETURN},
        {"var",       Token_Type::KW_VAR},
        {"let",       Token_Type::KW_LET},
        {"struct",    Token_Type::KW_STRUCT},
        {"if",        Token_Type::KW_IF},
        {"else",      Token_Type::KW_ELSE},
        {"while",     Token_Type::KW_WHILE},
        {"do",        Token_Type::KW_DO},
        {"for",       Token_Type::KW_FOR},
        {"true",      Token_Type::KW_TRUE},
        {"false",     Token_Type::KW_FALSE},
        {"null",      Token_Type::KW_NULL},
        {"struct",    Token_Type::KW_STRUCT},
        {"enum",      Token_Type::KW_ENUM},
        {"union",     Token_Type::KW_UNION},
        {"break",     Token_Type::KW_BREAK},
        {"continue",  Token_Type::KW_CONTINUE},
        {"using",     Token_Type::KW_USING},
        {"as",        Token_Type::KW_AS},
        {"new",       Token_Type::KW_NEW},
        {"free",      Token_Type::KW_FREE},
        {"asm",       Token_Type::KW_ASM},
};

const std::map<std::string, Token_Type> Lexer::_single_char_operators{
        {"{", Token_Type::L_BRACE},
        {"}", Token_Type::R_BRACE},
        {"(", Token_Type::L_PAREN},
        {")", Token_Type::R_PAREN},
        {"[", Token_Type::L_BRACKET},
        {"]", Token_Type::R_BRACKET},
        {"+", Token_Type::PLUS},
        {"-", Token_Type::MINUS},
        {"*", Token_Type::ASTERISK},
        {"/", Token_Type::SLASH},
        {"&", Token_Type::AMPERSAND},
        {"|", Token_Type::BITWISE_OR},
        {"^", Token_Type::CARET},
        {"%", Token_Type::MODULO},
        {"!", Token_Type::BANG},
        {"=", Token_Type::EQUALS},
        {">", Token_Type::GREATER},
        {"<", Token_Type::LESS},
        {";", Token_Type::SEMICOLON},
        {":", Token_Type::COLON},
        {",", Token_Type::COMMA},
        {".", Token_Type::DOT},
};

const std::map<std::string, Token_Type> Lexer::_double_char_operators{
        {"&&", Token_Type::LOGICAL_AND},
        {"||", Token_Type::LOGICAL_OR},
        {"<<", Token_Type::BITWISE_SHIFT_LEFT},
        {">>", Token_Type::BITWISE_SHIFT_RIGHT},
        {"+=", Token_Type::PLUS_EQUALS},
        {"-=", Token_Type::MINUS_EQUALS},
        {"*=", Token_Type::MULTIPLY_EQUALS},
        {"/=", Token_Type::DIVIDE_EQUALS},
        {"%=", Token_Type::MODULO_EQUALS},
        {"!=", Token_Type::BANG_EQUALS},
        {"&=", Token_Type::AND_EQUALS},
        {"|=", Token_Type::OR_EQUALS},
        {"^=", Token_Type::XOR_EQUALS},
        {"==", Token_Type::EQUALS_EQUALS},
        {">",  Token_Type::GREATER},
        {">=", Token_Type::GREATER_EQUALS},
        {"<",  Token_Type::LESS},
        {"<=", Token_Type::LESS_EQUALS},
        {"->", Token_Type::ARROW},
        {"::", Token_Type::SCOPE_RESOLUTION},
};

const std::map<std::string, Token_Type> Lexer::_triple_char_operators{
        {"<<=", Token_Type::LEFT_SHIFT_EQUALS},
        {">>=", Token_Type::RIGHT_SHIFT_EQUALS},
};

Lexer::Lexer(const std::string& filename)
        : _filename{filename} {
    try {
        _tez_source = read_file_content(filename);
    } catch(const tez::File_Not_Found& e) {
        std::cerr << "Error: " << e.what();
        _errors_reported = true;
    }
}

Lexer::Lexer(const std::string& tez_source,
        [[maybe_unused]] bool doesnt_matter) {
    _tez_source = tez_source;
}

bool Lexer::errors_reported() const {
    return _errors_reported;
}

void Lexer::report_error(const std::string& message) {
    _errors_reported = true;
    std::cerr << "<filename>:" << std::to_string(_lines_count) << ":"
            << std::to_string(_current_char - _columns_count + 1) << ": "
            << message << ".\n";
}

std::vector<Token> Lexer::tokenize() {
    _tokens.clear();
    // TODO: Make it cleaner, this is Jonathan Blow style function.
    try {
        while(_current_char < _tez_source.length()) {
            char c = _tez_source.at(_current_char);
            if(std::isspace(c)) {
                handle_whitespace();
                continue;
            }
            if(peek_string("//") || peek_string("/*")) {
                handle_comment();
                continue;
            }
            if(std::isspace(c)) {
                handle_whitespace();
                continue;
            }
            if(c == '"') {
                push_string();
                continue;
            }
            if(c == '\'') {
                push_character();
                continue;
            }
            if(std::ispunct(c)) {
                push_operator();
                continue;
            }
            if(std::isdigit(c)) {
                push_number(c);
                continue;
            }
            if(std::isalpha(c) || c == '_') {
                push_identifier(c);
                continue;
            }
            report_error("Unexpected character '" + std::string{c} + "'");
        }
    } catch(const std::out_of_range& e) {
        report_error("Unexpected end of file");
    }
    _tokens.shrink_to_fit();
    return _tokens;
}

char Lexer::peek_char(std::size_t depth) const {
    return _tez_source.at(_current_char + depth);
}

char Lexer::peek_string(const std::string&& to_peek) const {
    const auto l = to_peek.length();
    if(_current_char + l >= _tez_source.length()) {
        return false;
    }
    return _tez_source.substr(_current_char, l) == to_peek;
}

void Lexer::push_token(Token_Type type, const std::string& value) {
    _tokens.push_back({type, value, _filename, _lines_count,
            _current_char - _columns_count + 1});
}

void Lexer::push_token(Token_Type type, const std::string& value,
        unsigned int col) {
    _tokens.push_back({type, value, _filename, _lines_count, col + 1});
}

void Lexer::handle_whitespace() {
    char c = _tez_source.at(_current_char);
    if(c == '\n') {
        ++_lines_count;
        _columns_count = _current_char;
    }
    ++_current_char;
}

// TODO: Nested multiline comments.
void Lexer::handle_comment() {
    if(peek_string("//")) {
        while(_tez_source.at(_current_char) != '\n') {
            ++_current_char;
        }
        return;
    }
    // When "/*".
    while(!peek_string("*/")) {
        if(_tez_source.at(_current_char) == '\n') {
            ++_lines_count;
            _columns_count = _current_char;
        }
        ++_current_char;
    }
    _current_char += 2;
}

void Lexer::push_operator() {
    // Because all operators are made from 1 or 2 characters.
    std::string three_chars{_tez_source.substr(_current_char, 3)};
    if(auto res = _triple_char_operators.find(three_chars);
            res != _triple_char_operators.end()) {
        push_token(res->second, "");
        _current_char += 3;
        return;
    }
    std::string two_chars{_tez_source.substr(_current_char, 2)};
    if(auto res = _double_char_operators.find(two_chars);
            res != _double_char_operators.end()) {
        push_token(res->second, "");
        _current_char += 2;
        return;
    }
    std::string op{_tez_source.at(_current_char)};
    if(auto res = _single_char_operators.find(op);
            res != _single_char_operators.end()) {
        push_token(res->second, "");
        ++_current_char;
        return;
    }
    report_error("Unknown operator '"
            + std::string{_tez_source.at(_current_char)} + "'");
    ++_current_char;
}

void Lexer::push_number(char c) {
    std::size_t beginning = _current_char;
    std::string number;
    bool is_real = false;   // False means integer value.
    while(std::isdigit(c) || c == '.') {
        if(c == '.') {
            if(is_real) {   // Error if dot was used before.
                report_error("Too many decimal points in number.");
            }
            is_real = true;
        }
        number += c;
        try {
            c = _tez_source.at(++_current_char);
        } catch(const std::out_of_range& e) {
            break;
        }
    }
    if(is_real) {
        push_token(Token_Type::REAL_NUMBER, number, beginning - _columns_count);
    } else {
        push_token(Token_Type::INTEGER, number, beginning - _columns_count);
    }
}

void Lexer::push_identifier(char c) {
    std::size_t beginning = _current_char;
    std::string word;
    while(std::isalpha(c) || std::isdigit(c) || c == '_') {
        word += c;
        try {
            c = _tez_source.at(++_current_char);
        } catch(const std::out_of_range& e) {
            break;
        }
    }
    auto keyword = _keywords.find(word);
    if(keyword != _keywords.end()) {
        push_token(keyword->second, "");
    } else {
        push_token(Token_Type::IDENTIFIER, word, beginning - _columns_count);
    }
}

void Lexer::push_string() {
    std::size_t beginning = _current_char;
    char c = _tez_source.at(++_current_char);
    std::string s;
    bool escape_char = false;
    while(c != '"' || escape_char) {
        escape_char = false;
        s.push_back(c);
        if(c == '\\') {
            escape_char = true;
        }
        c = _tez_source.at(++_current_char);
    }
    ++_current_char;
    push_token(Token_Type::STRING, s, beginning - _columns_count);
}

void Lexer::push_character() {
    std::size_t beginning = _current_char - _columns_count;
    std::string character = {_tez_source.at(++_current_char)};
    if(character == "\\") {
        character += _tez_source.at(++_current_char);
    }
    char c = _tez_source.at(_current_char);
    if(c = _tez_source.at(++_current_char); c != '\'') {
        report_error("Expected ''', got '" + std::string{c} + "'");
        while(c != '\'') {
            c = ++_current_char;
        }
    }
    ++_current_char;
    push_token(Token_Type::CHARACTER, {character}, beginning);
}

}

