/**
 * @file expression.cpp
 * @author James Halladay (jehalladay112@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-01
 * 
 * @copyright Copyright (c) 2022
 * 
 * BNF notation from: https://condor.depaul.edu/ichu/csc447/notes/wk2/pascal.html
 * 
 * 
 * <identifier> ::= <letter > {<letter or digit>}
 * 
 * <unsigned number> ::= <unsigned integer> | <unsigned real>
 * 
 * <unsigned integer> ::= <digit> {<digit>}
 * 
 * <unsigned real> ::= <unsigned integer> . <unsigned integer> | <unsigned integer> . <unsigned integer> E <scale factor>| <unsigned integer> E <scale factor>
 * 
 * <scale factor> ::= <unsigned integer> | <sign> <unsigned integer>
 * 
 * <constant identifier> ::= <identifier>
 * 
 * <string> ::= '<character> {<character>}'
 * 
 * <expression> ::= <simple expression> | <simple expression> <relational operator> <simple expression>
 * 
 * <relational operator> ::= = | <> | < | <= | >= | >
 * 
 * <simple expression> ::= <term> | <sign> <term>| <simple expression> <adding operator> <term>
 * 
 * <adding operator> ::= + | - | or
 * 
 * <term> ::= <factor> | <term> <multiplying operator> <factor>
 * 
 * <multiplying operator> ::= * | / | div | mod | and
 * 
 * <factor> ::= <variable> | <unsigned constant> | ( <expression> ) 
 * 
 * <unsigned constant> ::= <unsigned number> | <string> | < constant identifier> < nil>
 * 
 * <variable> ::= <entire variable> | <referenced variable>
 * 
 * <entire variable> ::= <variable identifier>
 * 
 * <variable identifier> ::= <identifier>
 * 
 * <referenced variable> ::= <pointer variable>
 * 
 * <pointer variable> ::= <variable>
 * 
 * <nil>
 */

#include <iostream>
#include <string>

using namespace std;

enum TokenType {
    RELATIONAL_OPERATOR,
    ADDING_OPERATOR,
    MULTIPLYING_OPERATOR,
    SIGN,
    IDENTIFIER,
    UNSIGNED_INTEGER,
    E,
    DOT,
    UNRECOGNIZED,
    OPEN_PAREN,
    CLOSE_PAREN,
    END_OF_TEXT
    // UNSIGNED_NUMBER,
    // UNSIGNED_REAL,
    // SCALE_FACTOR,
    // CONSTANT_IDENTIFIER,
    // STRING,
    // EXPRESSION,
    // SIMPLE_EXPRESSION,
    // TERM,
    // FACTOR,
    // UNSIGNED_CONSTANT,
    // VARIABLE,
    // ENTIRE_VARIABLE,
    // VARIABLE_IDENTIFIER,
    // REFERENCED_VARIABLE,
    // POINTER_VARIABLE,
    // NIL
};


class Token {
    public:
        TokenType type;
        string value;
};


class Tokenizer {
    string text;
    int position;

    public:

        Tokenizer(string newText) {
            text = newText;
            position = 0;
        }

        /**
         * Keep track of where we are in the text and produce next token
         * 
         * @return Token 
         */
        Token next() {
            Token p = peek();
            position += p.value.length();

            return p;
        }

        /**
         * Look at the next token without advancing the position
         * 
         * @return Token 
         */
        Token peek() {
            


        }
};

int main() {

    Tokenizer tokenizer("var*2.0E10/12+6");
    Token token;

    do {
        token = tokenizer.next();
        cout << token.type << " " << token.value << endl;
    } while (token.type != END_OF_TEXT);
    
    return 0;
}
