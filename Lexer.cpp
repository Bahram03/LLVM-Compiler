#include "Lexer.h"

namespace charinfo {
    LLVM_READNONE inline bool isWhitespace(char c) {
        return c == ' ' || c == '\t' || c == '\f' || 
        c == '\v' ||
        c == '\r' || c == '\n'; 
    }
    LLVM_READNONE inline bool isDigit(char c) {
        return c >= '0' && c <= '9';
    }
    LLVM_READNONE inline bool isLetter(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }   
}

void Lexer::next(Token &token) {
    while(*BufferPtr && charinfo::isWhitespace(*BufferPtr)) {
        ++BufferPtr;
    }
    
    if (!BufferPtr) {
        token.Kind = Token::eoi;
        return;
    }

    if (charinfo::isLetter(*BufferPtr)) {
        const char *end = BufferPtr + 1;
        while (charinfo::isLetter(*end)) ++end;
        llvm::StringRef Name(BufferPtr, end - BufferPtr);
        if (Name == "int")
            Token::TokenKind kind = Token::KW_int;
        else if (Name == "and")
            Token::TokenKind kind = Token::KW_and;
        else if (Name == "or")
            Token::TokenKind kind = Token::KW_or;
        else if (Name == "begin")
            Token::TokenKind kind = Token::KW_begin;
        else if (Name == "end")
            Token::TokenKind kind = Token::KW_end;
        else if (Name == "if")
            Token::TokenKind kind = Token::KW_if;
        else if (Name == "elif")
            Token::TokenKind kind = Token::KW_elif;
        else if (Name == "else")
            Token::TokenKind kind = Token::KW_else;
        else if (Name == "loopc")
            Token::TokenKind kind = Token::KW_loopc;
        else
            Token::TokenKind kind = Token::ident
        formToken(token, end, kind);
        return;
    }

    else if (charinfo:: isDigit(*BufferPtr)) {
        const char *end = BufferPtr + 1;
        while (charinfo::isDigit(*end))
            ++end;
        formToken(token, end, Token::number);
        return;
    }

    else {
        // switch (*BufferPtr) {
        //     #define CASE(ch, tok) \ 
        //     case ch: formToken(token, BufferPtr + 1, tok); break
        //     CASE('+', Token::plus)
        //     CASE('-', Token::minus)
        //     CASE('*', Token::star)
        //     CASE('/', Token::slash)
        //     CASE('(', Token::l_paren)
        //     CASE(')', Token::r_paren)
        //     CASE(':', Token::colon)
        //     CASE(',', Token::comma)
        //     CASE('=', Token::equal)
        //     CASE(';', Token::semicolon)
        //     CASE('^', Token::power)
        //     CASE('%', Token::percent)
        //     CASE('>', Token::greater)
        //     CASE('<', Token::less)
        //     #undef CASE
        // }

        int mode;
        if (*BufferPtr == '+' && *(BufferPtr + 1) == '=') {
            mode = 2;
            Token::TokenKind kind = Token::plusequal;
        }
        else if (*BufferPtr == '-' && *(BufferPtr + 1) == '='){
            mode = 2;
            Token::TokenKind kind = Token::minusequal;
        }
        else if (*BufferPtr == '/' && *(BufferPtr + 1) == '='){
            mode = 2;
            Token::TokenKind kind = Token::slashequal;
        }
        else if (*BufferPtr == '%' && *(BufferPtr + 1) == '='){
            mode = 2;
            Token::TokenKind kind = Token::percentequal;
        }
        else if (*BufferPtr == '*' && *(BufferPtr + 1) == '='){
            mode = 2;            
            Token::TokenKind kind = Token::starequal;
        }
        else if (*BufferPtr == '>' && *(BufferPtr + 1) == '='){
            mode = 2;            
            Token::TokenKind kind = Token::greaterequal;
        }
        else if (*BufferPtr == '<' && *(BufferPtr + 1) == '='){
            mode = 2;            
            Token::TokenKind kind = Token::lessequal;
        }
        else if (*BufferPtr == '=' && *(BufferPtr + 1) == '='){
            mode = 2;            
            Token::TokenKind kind = Token::equalequal;
        }
        else if (*BufferPtr == '!' && *(BufferPtr + 1) == '='){
            mode = 2;            
            Token::TokenKind kind = Token::notqeual;
        }
        else if (*BufferPtr == '+'){
            mode = 1;            
            Token::TokenKind kind = Token::plus;
        }
        else if (*BufferPtr == '-'){
            mode = 1;            
            Token::TokenKind kind = Token::minus;
        }
        else if (*BufferPtr == '/'){
            mode = 1;            
            Token::TokenKind kind = Token::slash;
        }
        else if (*BufferPtr == '%'){
            mode = 1;
            Token::TokenKind kind = Token::percent;
        }
        else if (*BufferPtr == '*'){
            mode = 1;
            Token::TokenKind kind = Token::star;
        }
        else if (*BufferPtr == '>'){
            mode = 1;
            Token::TokenKind kind = Token::greater;
        }
        else if (*BufferPtr == '<'){
            mode = 1;
            Token::TokenKind kind = Token::less;
        }
        else if (*BufferPtr == '='){
            mode = 1;
            Token::TokenKind kind = Token::equal;
        }
        else{
            Token::TokenKind kind = Token::unknown;
        }
        formToken(token, BufferPtr + mode, Token::kind)
        return;
    }
}

void Lexer::formToken(Token &Tok, const char *TokEnd, Token::TokenKind Kind) {
    Tok.Kind = Kind;
    Tok.Text = llvm::StringRef(BufferPtr, TokEnd, BufferPtr);
    BufferPtr = TokEnd
}