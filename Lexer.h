#ifndef LEXER_H
#define LEXER_H

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/MemoryBuffer.h"

class Lexer;

class Token {
    friend class Lexer

    public:
        enum TokenKind: unsigned short
        {
            eoi, //end of input
            unknown, //in case of error at the lexical level
            ident,
            number,
            equal, 
            comma,
            semicolon,
            colon,
            plus,
            minus,
            star,
            percent,
            slash,
            power,
            plusequal,
            minusequal,
            starequal,
            slashequal,
            percentequal,
            l_paren,
            r_paren,
            greater,
            less,
            greaterequal,
            lessequal,
            equalequal,
            notequal,
            KW_int,
            KW_and,
            KW_or,
            KW_begin,
            KW_end,
            KW_if,
            KW_elif,
            KW_else,
            KW_loopc
        };
    private:
    TokenKind;
    llvm::StringRef Text; // points to the start of the text of the token

    public:
    TokenKind getKind() const { return Kind; }
    llvm::StringRef getText() const { return Text; }

    //to test if the token is of a certain kind
    bool is(TokenKind K) const { return Kind == K}
    bool isOneOf(TokenKind K1, TokenKind K2) const { return is(K1) || is(K2); }
        template <typename... Ts>
        bool isOneOf(TokenKind K1, TokenKind K2, Ts... Ks) const { return is(K1) || isOneOf(K2, Ks...); }
};

class Lexer {
    const char *BufferStart;
    const char *BufferPtr;

    public:
    Lexer(const llvm::StringRef &Buffer) {
        BufferStart = Buffer.begin();
        BufferPtr = BufferStart;
    }
    
    void next(Token &token);

    private:
    void formToken(Token &Result, const char *TokEnd, Token::TokenKind Kind);
};
#endif