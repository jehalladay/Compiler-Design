/**
 * from :https://condor.depaul.edu/ichu/csc447/notes/wk2/pascal.html 
 * ...
 * T<identifier> ::= <letter > {<letter or digit>}
 * T<sign> ::= + | -
 * P?<unsigned number> ::= <unsigned integer> | <unsigned real>
 * T<unsigned integer> ::= <digit> {<digit>}
 * P<unsigned real> ::= <unsigned integer> . <unsigned integer> | 
 * 					<unsigned integer> . <unsigned integer> E <scale factor>|
 * 					<unsigned integer> E <scale factor>
 * P<scale factor> ::= <unsigned integer> | 
 * 				   <sign> <unsigned integer>
 * ...
 * <expression> ::= <simple expression> | <simple expression> <relational operator> <simple expression>
 * T <relational operator> ::= = | <> | < | <= | >= | >   
 * P?<simple expression> ::= <term> | <sign> <term>| <simple expression> <adding operator> <term>
 * T<adding operator> ::= + | - | or  Note: SIGN token is first two
 * P<term> ::= <factor> | <term> <multiplying operator> <factor> --> <factor> <multiplying operator>  <term>
 * T <multiplying operator> ::= * | / | div | mod | and
 * P<factor> ::= <variable> | <unsigned constant> | ( <expression> ) 
 * P<unsigned constant> ::= <unsigned number> | <string> 
 * T<string>:= '<character>{<character>}' 
 *  ...
 * P<variable> ::= <identifier>
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctype.h>
#include <vector>

using namespace std;


enum TokenType{
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
	STRING,
	END_OF_TEXT
};

// colors found here: https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
string highlightRed(string s) {
	return "\033[1;31m" + s + "\033[0m";
}

string highlightGreen(string s) {
	return "\033[1;32m" + s + "\033[0m";
}

string highlightYellow(string s) {
	return "\033[1;33m" + s + "\033[0m";
}

string highlightCyan(string s) {
	return "\033[1;36m" + s + "\033[0m";
}

string tokenString(TokenType t) {
	switch(t) {
		case RELATIONAL_OPERATOR: 
			return "RELATIONAL_OPERATOR";

		case ADDING_OPERATOR: 
			return "ADDING_OPERATOR";

		case MULTIPLYING_OPERATOR: 
			return "MULTIPLYING_OPERATOR";

		case SIGN: 
			return "SIGN";

		case IDENTIFIER: 
			return "IDENTIFIER";

		case UNSIGNED_INTEGER : 
			return "UNSIGNED_INTEGER";

		case E: 
			return "E";

		case DOT: 
			return "DOT";

		case OPEN_PAREN: 
			return "OPEN_PAREN";

		case CLOSE_PAREN: 
			return "CLOSE_PAREN";

		case END_OF_TEXT: 
			return "END_OF_TEXT";

		case STRING: 
			return "STRING" ;

		default: 
			return "UNRECOGNIZED";
	};
}


class Token {
	public:
		TokenType type;
		string value; 

		Token(TokenType newType = UNRECOGNIZED, string newValue = "") {
			type = newType;
			value = newValue;
		}

		friend ostream & operator <<(ostream &out, const Token &token) {
			return out << "Token:" << tokenString(token.type) << ":" << token.value;
		}

		string toString() {
			return tokenString(type) + ":" + value;
		}
};


class Tokenizer {
	bool verbose;
	int charpos;
	int linepos;
	unsigned pos;
	string text;
	Token currentToken;
	vector<Token> tokens;

	public:
		static string filename;

		Tokenizer(bool verbose = false) {
			verbose = verbose;
			text = "";
			currentToken = Token(UNRECOGNIZED, "Start of file");
			tokens.push_back(currentToken);
		}

		void setVerbose(bool v) {
			verbose = v;
		}


		bool error(string message) {
			
			if(verbose) {
				cerr << highlightRed(
					'\t' + 
					filename + 
					": " + 
					to_string(linepos) + 
					", " + 
					to_string(charpos) + 
					", " + 
					to_string(pos) + 
					": " + 
					message
				) << endl;
			}
			
			return false;
		}


		bool message(string message) {
			Token t = peek();

			if(verbose) {
				cout << linepos << ": " << message << "This Token: " << currentToken <<  ", Next Token: " << t << " next text:" << text.substr(pos, 10) << endl;
			}
			
			return true;
		}


		void readFile(string newFilename) {
			ifstream file(newFilename);
			stringstream buffer;
			Tokenizer::filename = newFilename;

			buffer << file.rdbuf();
			
			text = buffer.str();
			pos = 0;
			linepos = 1;
			charpos = 1;
		}


		void setText(string newText = "") {
			text = newText;
			pos = 0;
			linepos = 1;
			charpos = 1;
		}


		Token next() {
			// Keep track where we are in the text and produce the next token
			Token p = peek();
			
			pos += p.value.size();
			currentToken = p;
			tokens.push_back(currentToken);

			return p;
		}


		/**
		 * @brief produce the next token without consuming it
		 * 
		 * @return Token 
		 */
		Token peek(bool verbose = false) {
			char c;

			// Skip whitespace and comments delimited by curly braces, { or }
			while (pos < text.size() && (isspace(text[pos]) || text[pos] == '{')) {
				if (text[pos] == '\n') {
					linepos++;
				} 
				
				if (text[pos] == '{') {
					while (pos < text.size() && text[pos] != '}') {
						if (text[pos] == '\n') {
							linepos++;
						}
						
						pos++;
					}
				}

				pos++;
			}

			// check for strings delimited by single quotes
			c = text[pos];

			if (c == '\'') {
				int offset = 1;

				while (pos + offset < text.size() && text[pos + offset] != '\'') {
					if (text[pos + offset] == '\n') {
						linepos++;
					}

					offset++;
				}

				if (text[pos+offset] == '\'') {
					offset++;
				}

				Token retval = Token(STRING, text.substr(pos, offset));
				pos += offset;
				
				return retval;
			}
			
			// check for end of text
			if (pos >= text.size()) {
				return Token(END_OF_TEXT,"");
			} 

			// check for relational operators
			if (c == '<' || c == '=' || c == '>') {
				c = text[pos + 1];
				
				if (c == '=' || c == '>') {
					return Token(RELATIONAL_OPERATOR, text.substr(pos, 2));
				} else {
					return Token(RELATIONAL_OPERATOR, text.substr(pos, 1));
				}
			}

			// check for left parentheses
			if (c == '(') {
				return Token(OPEN_PAREN, text.substr(pos, 1));
			}

			// check for right parentheses
			if (c == ')') {
				return Token(CLOSE_PAREN, text.substr(pos, 1));
			}

			// check for addition operators
			if (c == '+' || c == '-') {
				return Token(SIGN, text.substr(pos, 1));
			}

			// check for multiplication operators	
			if (c == '*' || c == '/') {
				return Token(MULTIPLYING_OPERATOR, text.substr(pos, 1));
			}

			// check for . operator
			if (c == '.') {
				return Token(DOT, text.substr(pos, 1));
			}

			// check for unsigned integer
			if (isdigit(c)) {
				unsigned newpos = pos;

				while (newpos < text.size() && isdigit(text[newpos])) {
					newpos++;
				}

				return Token(UNSIGNED_INTEGER, text.substr(pos, newpos - pos));
			}

			// check for identifier
			if (isalpha(c)) {
				string value;
				unsigned offset = 0;

				while(pos + offset < text.size() && isalpha(text[pos + offset])) {
					offset++;
				}

				value = text.substr(pos, offset);

				if (value == "E") {
					return Token(E, value);
				}
					
				if (value == "mod" || value == "div" || value == "and") {
					return Token(MULTIPLYING_OPERATOR, value);
				}
					
				if (value == "or") {
					return Token(ADDING_OPERATOR, value);
				} 

				while (pos + offset < text.size() && isalnum(text[pos + offset])) {
					offset++;
				}

				value = text.substr(pos, offset);

				return Token(IDENTIFIER, value);
			}

			return Token(UNRECOGNIZED, "Unrecognized token");
		}

		vector<Token> getTokens() {
			return tokens;
		}
};

string Tokenizer::filename;

class Parser {
	Tokenizer tokens;
	
	public:
		Parser(string newFilename, bool verbose = false) { 
			tokens = Tokenizer(verbose);
			tokens.readFile(newFilename); 
		}

		Token peek() { 
			return tokens.peek(); 
		}

		Tokenizer& getTokenizer() { 
			return tokens; 
		}

		void setVerbose(bool verbose) { 
			tokens.setVerbose(verbose); 
		}

		/**
		 * @brief parse expression
		 * 
		 * <expression> ::= 
		 * 		<simple expression> | 
		 * 		<simple expression> <relational operator> <simple expression>
		 */
		bool expression(Tokenizer &t) {
			t.message("Parsing Expression: ");

			if (simpleExpression(t)) {
				if (t.peek().type == RELATIONAL_OPERATOR) {
					t.next();

					if (simpleExpression(t)) {
						return t.message("Found Expression relational version");
					} else {
						return t.error("Expected a simple expression after relational operator");
					} 
				} else {
					return t.message("!!Found Expression not relational");
				} 
			} else {
				return t.error("Expected an expression");
			}
		}

		/**
		 * @brief parse simple expression
		 * 
		 * <simple expression> ::= 
		 * 		<term> | 
		 * 		<sign> <term> | 
		 * 		<term> <adding operator> <simple expression>
		 */
		bool simpleExpression(Tokenizer &t) {
			t.message("Parsing Simple Expression: ");
			
			// check for <sign> <term>
			if (t.peek().type == SIGN) {
				t.next();
				if (term(t)) {
					return t.message("!!Found Simple Expression after sign");
				} else {
					return t.error("Expected a term after sign"); 
				} 
			}

			// check for <term>
			if (term(t)) {
				// while (t.peek().type == ADDING_OPERATOR  || t.peek().type == SIGN)  {
				if(t.peek().type == ADDING_OPERATOR  || t.peek().type == SIGN)  {
					t.next();

					if (simpleExpression(t)) {
						return t.message("!!Found Simple Expression after adding operator");
					} else {
						return t.error("Expected an simple expression after adding operator");
					}
				}
			} else {
				return t.error("Expected a term");
			}

			return true;
		}

		/**
		 * @brief parse term
		 * 
		 * <term> ::= 
		 * 		<factor> | 
		 * 		<factor> <multiplying operator> <term> |
		 * 		<factor> <sign> <term>
		 */
		bool term(Tokenizer &t) {
			t.message("Parsing Term: ");

			// check for <factor> 
			if (factor(t)) {

				// check for <factor> <multiplying operator> <term> 
				if (t.peek().type == MULTIPLYING_OPERATOR) {
					t.next();

					if (term(t)) {
						return t.message("!!Found Term after Multiplying Operator");
					} else {
						return t.error("Missing term");
					} 
				}

				// check for <factor> <sign> <term>, added by me not Castleton
				if (t.peek().type == SIGN) {
					t.next();

					if (term(t)) {
						return t.message("!!Found Term after sign");
					} else {
						return t.error("Missing term");
					} 
				}

				return t.message("!!Found Term alone");
			}

			return t.error("Expected a term");
		}


		/**
		 * @brief parse factor
		 * 
		 * <factor> ::= 
		 * 		<variable> | 
		 * 		<unsigned constant> | 
		 * 		( <expression> )
		 */
		bool factor(Tokenizer &t) {

			t.message("Parsing Factor: ");
			
			// check for ( <expression> )
			if (t.peek().type == OPEN_PAREN) {
				t.next();

				if (expression(t)) {
					if (t.next().type == CLOSE_PAREN) {
						return t.message("!!Found Factor with sub expression");
					}

					return t.error("Missing ) after expression");
				}
				return t.error("Expected an expression");
			}

			// check for <variable>
			if (variable(t)) {
				return t.message("!!Found Factor variable");
			}

			// check for <unsigned constant>
			if (unsignedConstant(t)) {
				return t.message("!!Found Factor unsigned constant");
			}

			return t.error("Expected (Expression), Variable, Unsigned Constant");
		}


		/**
		 * @brief parse variable
		 * 
		 * <scale factor> ::= 
		 * 		<unsigned integer> | 
		 * 		<sign> <unsigned integer>
		 */ 
		bool scaleFactor(Tokenizer &t) {
			Token tok = t.next();

			t.message("Parsing Scale Factor: ");

			// check for <sign> <unsigned integer>
			if (tok.type == SIGN) {
				tok = t.next();
				
				if (tok.type == UNSIGNED_INTEGER) {
					return t.message("!!Found Scale Factor after sign");
				}

				return t.error("Sign was not followed by unsigned integer");
			}

			// check for <unsigned integer>
			if (tok.type == UNSIGNED_INTEGER) {
				return t.message("!!Found Scale Factor no sign");
			} 
			
			return t.error("Neither Sign or Unsigned Integer Present");
		}


		/**
		 * @brief parse unsigned constant
		 * 
		 *  <unsigned constant> ::=
		 * 		<unsigned number> |
		 * 		<string>
		 */
		bool unsignedConstant(Tokenizer &t) {
			t.message("Parsing Unsigned Constant");
			
			// check for <string>
			if (t.peek().type == STRING) {
				t.next();
				
				return t.message("!!Found unsigned constant String");
			}
			
			// check for <unsigned number>
			if (unsignedNumber(t)) {
				return t.message("!!Found unsigned constant number");
			}

			return t.error("Expected a Unsigned Constant");
		}

		/** 
		 * @brief parse unsigned number
		 * 
		 * <unsigned number> ::=
		 * 		<unsigned integer> |
		 * 		<unsigned real> 
		 */
		bool unsignedNumber(Tokenizer &t) {
			t.message("Parsing unsigned number");

			// check for <unsigned real>
			if (unsignedReal(t)) {
				return t.message("!!Found unsigned number real");
			}

			// check for <unsigned integer>
			if (t.peek().type == UNSIGNED_INTEGER) {
				t.next();

				return t.message("!!Found unsigned number integer");
			} 

			return t.error("Expected a Unsigned Real or Unsigned Integer");
		}

		/**
		 * @brief parse variable
		 * 
		 * <variable> ::=
		 * 		<identifier>
		 */
		bool variable(Tokenizer &t) {
			t.message("Parsing Variable");
			
			// check for <identifier>
			if (t.next().type == IDENTIFIER) {
				return t.message("!!Found Variable");
			} else {
				return t.error("Expected Variable");
			} 
		}


		/**
		 * @brief parse unsigned real
		 * 
		 * <unsigned real> ::=
		 * 		<unsigned integer> . <unsigned integer> |
		 *  	<unsigned integer> . <unsigned integer> E <scale factor> |
		 *  	<unsigned integer> E <scale factor>
		 */
		bool unsignedReal(Tokenizer &t) {
			Token tok = t.next();

			t.message("Parsing unsigned real");

			// check for <unsigned integer>
			if (tok.type == UNSIGNED_INTEGER) {
				tok = t.next();

				// check for <unsigned integer> .
				if (tok.type == DOT) {
					tok = t.next();

					// check for <unsigned integer> . <unsigned integer>
					if (tok.type == UNSIGNED_INTEGER) {
						tok = t.peek();

						// check for <unsigned integer> . <unsigned integer> E <scale factor>
						if (tok.type == E) {
							t.next();

							if (scaleFactor(t)) {
								return t.message("!!Found unsigned real complex real");;
							} 

							return t.error("Missing Scale Factor after E");
						}

						return true;
					}
				}

				// check for <unsigned integer> E <scale factor>
				if (tok.type == E) {
					if (scaleFactor(t)) {
						return t.message("!!Found unsigned real with E");
					} 

					return t.error("Missing Scale Factor after E ");
				}

				return t.error("Neither . or E followsed unsigned integer");
			}

			return t.error(" Missing Unsigned Integer ");
		}
};


class TestCase {
	protected:
		string name;

	public:
		TestCase(string newName = "Unnamed Test Case") {
			name = newName;
		}


		bool test(bool verbose = false) {
			bool retval = false;

			cout << highlightCyan("Start Test: ") << highlightCyan(name) << endl << endl;

			retval = doTest(verbose);
			
			if (retval) {
				cout << highlightGreen("Finished Test: " + name + "->Passed") << endl << endl;
			} else {
				cout << highlightRed("Finished Test: " + name + "->Failed") << endl << endl;
			}

			return retval;
		}

		virtual bool doTest(bool verbose = false) = 0;
};


class TokenizerTest: public TestCase { 
	protected:
		string text;
		bool shouldFail;

	public:
	    TokenizerTest(string newText = ".", bool newShouldFail = false): TestCase("TokenizerTest") {
			text = newText;
			shouldFail = newShouldFail;
		}

		virtual bool doTest() {
			int count = 0;
			bool status = true;
			Tokenizer t;
			Token token;

			t.setText(text);

			do {
				token = t.next();

				cout << "Next Token: " << highlightYellow(token.toString()) << endl;

				if (token.type == UNRECOGNIZED) { 
					t.error("Unrecognized Token");
					status = false;
				}

				count++;
			} while (token.type != END_OF_TEXT && count < 100);

			return status;
		}
};


class TokenizerTestGood: public TokenizerTest {
	public:
		TokenizerTestGood(): TokenizerTest("{This is a comment}var * 	2.0E10 /12 \n+ 6"){
			name = "TokenizerTestGood " + name;
		}
};

class ParserTest: public TestCase {
	protected:
		Parser *p;
		bool shouldFail;

	public:
		ParserTest(string newFilename, bool newShouldFail = false): TestCase("ParserTest") {
			p = new Parser(newFilename);
			shouldFail = newShouldFail;
			name = name + " File: " + newFilename;
		}
		
		~ParserTest() {
			delete p;
		}
};

class ParserTestScaleFactor: public ParserTest {
	public:
		ParserTestScaleFactor(): ParserTest("ScaleFactor.pas") {
			name = "ScaleFactor " + name;
		}

		bool doTest() {
			int count = 0;
			bool status = false;
			

			while (p->peek().type != END_OF_TEXT) {
				if (p->scaleFactor(p->getTokenizer())) {
					count++;
				}
			}

			status = count == 3;

			if(!status) {
				for(auto token: p->getTokenizer().getTokens()) {
					cout << "Token: " << highlightYellow(token.toString()) << endl;
				}
			}

			return status;
		}
};

class ParserTestUnsignedReal: public ParserTest {
	public:
		ParserTestUnsignedReal(): ParserTest("UnsignedReal.pas") {
			name = "UnsignedReal " + name;
		}

		bool doTest() {
			bool status = false;
			int count = 0;
			
			while (p->peek().type != END_OF_TEXT) {
				if (p->unsignedReal(p->getTokenizer())) {
					count++;
				}
			}

			status = count == 1;

			if(!status) {
				for(auto token: p->getTokenizer().getTokens()) {
					cout << "Token: " << highlightYellow(token.toString()) << endl;
				}
			}

			return status;
		}
};

class ParserWholeEnchilada: public ParserTest {
	public:
		ParserWholeEnchilada(bool verbose = false): ParserTest("WholeEnchilada.pas") {
			name = "UnsignedReal " + name;
		}

		bool doTest(bool verbose = false) {
			bool status = false;
			int count = 0;

			p->setVerbose(verbose);

			while (p->peek().type != END_OF_TEXT) {
				if(verbose) {
					cout << "Expression Count is " << count << endl;
				}

				if (p->expression(p->getTokenizer())) {
					count++;
				}
			}

			status = count == 1;

			if(!status && verbose) {
				for(auto token: p->getTokenizer().getTokens()) {
					cout << "Token: " << highlightYellow(token.toString()) << endl;
				}
			}

			return status;
		}
};


vector<TestCase *> testcases;

bool runTests(bool verbose = false) {
	bool success = true;

	// testcases.push_back(new TokenizerTest());
	// testcases.push_back(new TokenizerTestGood());
		// testcases.push_back(new ParserTestScaleFactor());
		// testcases.push_back(new ParserTestUnsignedReal());
	testcases.push_back(new ParserWholeEnchilada());
	
	for (auto test:testcases) {
		if (!test->test(verbose)) {
			success = false;
		} 
	}

	if (success) {
		cout << highlightGreen("All passed") << endl;
		
		return true;
	} else {
		cout << highlightRed("Some failed") << endl;
		
		return false;
	}

	cerr << "Failed some test" << endl; 
	
	return false;	
}


int main(int argc, char **argv) {
    int status = 0;
	bool verbose = false;

	if (argc < 2) {
		cerr << "Missing an argument: <filename>" << endl;
		return -1;
	}

	if (argc > 2) {
		verbose = bool(atoi(argv[2]));
	}

    Parser p(argv[1]);
    
	if(!runTests(verbose)) {
		status = 1;
	} 
		
	return status;
}
