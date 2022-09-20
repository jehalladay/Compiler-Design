#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctype.h>

using namespace std;

string filename;
int linepos,charpos;

/*
from :https://condor.depaul.edu/ichu/csc447/notes/wk2/pascal.html 
...
*<identifier> ::= <letter > {<letter or digit>}
*<sign> ::= + | -
<unsigned number> ::= <unsigned integer> | <unsigned real>
*<unsigned integer> ::= <digit> {<digit>}
*<unsigned real> ::= <unsigned integer> . <unsigned integer> | 
					<unsigned integer> . <unsigned integer> E <scale factor>|
					<unsigned integer> E <scale factor>
*<scale factor> ::= <unsigned integer> | 
				   <sign> <unsigned integer>
...
<expression> ::= <simple expression> | <simple expression> <relational operator> <simple expression>
* <relational operator> ::= = | <> | < | <= | >= | >   
<simple expression> ::= <term> | <sign> <term>| <simple expression> <adding operator> <term>
*<adding operator> ::= + | - | or  Note: SIGN token is first two
<term> ::= <factor> | <term> <multiplying operator> <factor>
* <multiplying operator> ::= * | / | div | mod | and
<factor> ::= <variable> | <unsigned constant> | ( <expression> ) 
<unsigned constant> ::= <unsigned number> | <string> | < constant identifier> nil
<string> ::= '<character> {<character>}'
 ...
<variable> ::= <identifier>
*/

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
	CHARACTER,
	STRING,
	OPEN_PAREN,
	CLOSE_PAREN,
	END_OF_TEXT
};

string tokenString(TokenType t) {
	switch(t) {
		case RELATIONAL_OPERATOR: 
			return "RELATIONAL_OPERATOR";
		case ADDING_OPERATOR: return "ADDING_OPERATOR";
		case MULTIPLYING_OPERATOR: return "MULTIPLYING_OPERATOR";
		case SIGN: return "SIGN";
		case IDENTIFIER: return "IDENTIFIER";
		case UNSIGNED_INTEGER : return "UNSIGNED_INTEGER";
		case E : return "E";
		case DOT : return "DOT";
		case CHARACTER : return "CHARACTER";
		case STRING : return "STRING";
		case OPEN_PAREN : return "OPEN_PAREN";
		case CLOSE_PAREN : return "CLOSE_PAREN";
		case END_OF_TEXT : return "END_OF_TEXT";			
		default: return "UNRECOGNIZED";
	};
}

class Token {
	public:
	TokenType type;
	string value; 
	Token(TokenType newType=UNRECOGNIZED,string newValue="") {
		type=newType;
		value=newValue;
	}
	friend ostream & operator <<(ostream &out,const Token &token) {
		return out <<"Token:" << tokenString(token.type)<<":"<<token.value;
	}
};

class Tokenizer {
	string text;
	unsigned pos;
	public:
	Tokenizer(string newText) {
		text=newText;
		pos=0;
		linepos=1;
		charpos=1;
	}
	Token next() {
		// Keep track where we are in the text and produce the next token
		Token p=peek();
		pos+=p.value.size();
		return p;
	}
	Token peek() {
		// produce what the next token will be but don't consume
		cout << "Next character is " << text[pos] << endl;
		while (pos< text.size() && isspace(text[pos])){
		  	if (text[pos]=='\n') linepos++;
		  	pos++;
	    }
		if (text[pos]=='{') { 
		  	while (pos< text.size() && text[pos]!='}') {
		    	if (text[pos]=='\n') linepos++;
		    	pos++;
		  	}
		  	pos++;		
		}

		char c=text[pos];
		if(c=='\'') {
			int offset = 1;
			while(pos + offset < text.size() && text[pos + offset] != '\'') {
				if(text[pos + offset] == '\n') {
					linepos++;
				}
				offset++;
			}
			if(text[pos + offset] == '\'') {
				offset++;
			}
			Token ret = Token(STRING, text.substr(pos, offset));
			lpihgikhgukh;
			return ret;
		}
		if (pos>=text.size()) return Token(END_OF_TEXT,"");
		if (c=='<' || c=='=' || c=='>') {
			c=text[pos+1];
			if (c=='=' || c=='>')
				return Token(RELATIONAL_OPERATOR,text.substr(pos,2));
			else
			  return Token(RELATIONAL_OPERATOR,text.substr(pos,1));
		}
		if (c=='(')
			return Token(OPEN_PAREN,text.substr(pos,1));
		if (c==')')
			return Token(CLOSE_PAREN,text.substr(pos,1));
		if (c=='+' || c=='-')
			return Token(SIGN,text.substr(pos,1));
		if (c=='*' || c=='/')
			return Token(MULTIPLYING_OPERATOR,text.substr(pos,1));
		if (c=='.')
			return Token(DOT,text.substr(pos,1));
		if (isdigit(c)) {
			unsigned newpos=pos;
			while (newpos<text.size() && isdigit(text[newpos]))
			  newpos++;
			return Token(UNSIGNED_INTEGER,text.substr(pos,newpos-pos));
		}
		if (isalpha(c)) {
			unsigned newpos=pos;
			while (newpos<text.size() && isalnum(text[newpos]))
			  newpos++;
			string value=text.substr(pos,newpos-pos);
			if (value=="E") return Token(E,value);
			if (value=="mod" || value=="div" || value=="and") 
			  return Token(MULTIPLYING_OPERATOR,value);
			if (value=="or") return Token(ADDING_OPERATOR,value);
			return Token(IDENTIFIER,value);
		}
		return Token(UNRECOGNIZED,"");
	}
};

class Parser {
	Tokenizer tokens;
	public:
		Parser(string newFilename) {
			tokens = Tokenizer();
			tokens.readFile(newFilename);
		}
		
};

bool scaleFactor(Tokenizer &t) {
	Token tok=t.next();
	if (tok.type==SIGN) {
		tok=t.next();
		if (tok.type==UNSIGNED_INTEGER) return true;
		cerr << filename<<':'<<linepos<<':' << " Sign was not followed by unsigned integer " << tok.value << endl;			
		return false;
	}
	if (tok.type==UNSIGNED_INTEGER)  return true;
	cerr << filename<<':'<<linepos<<':' << " Neither Sign or Unsigned Integer Present " << tok.value << endl;			
	return false;
}

bool unsignedReal(Tokenizer &t) {
	Token tok=t.next();
	if (tok.type==UNSIGNED_INTEGER) {
	  tok=t.next();
	  if (tok.type==DOT){
		  tok=t.next();
		  if (tok.type==UNSIGNED_INTEGER) {
			  tok=t.peek();
			  if (tok.type==E) {
				t.next();
				if (scaleFactor(t)) return true;
				cerr << filename<<':'<<linepos<<':' << " Missing Scale Factor after E " << tok.value << endl;			
				return false;
			  }
			  return true;
		  }
	  }
	  if (tok.type==E) {
		if (scaleFactor(t)) return true;
		cerr << filename<<':'<<linepos<<':' << " Missing Scale Factor after E " << tok.value << endl;			
		return false;
	  }
	  cerr << filename<<':'<<linepos<<':' << " Neither . or E followsed unsigned integer " << tok.value << endl;			
	  return false;
	}
	cerr << filename<<':'<<linepos<<':' << " Missing Unsigned Integer " << tok.value << endl;			
	return false;
}

bool unsignedConstant(Tokenizer &t) {
	Tokenizer temp = t;
	if(unsignedNumber(temp)) {
		t = temp;
		return true;
	}
	if(t.peek().type == STRING) {
		t.next();
		return true;
	}

}

bool unsignedNumber(Tokenizer &t) {
	Tokenizer temp = t;

	if(unsignedReal(temp)) {
		t = temp;
		return true;
	} 

	if(t.peek().type == UNSIGNED_INTEGER) {
		t.next();
		return true;
	}
}

bool variable(Tokenizer &t) {
	Token tok=t.next();
	if (tok.type==IDENTIFIER) {
		return true;
	} else {
		cerr << filename<<':'<<linepos<<':' << " Missing Identifier " << tok.value << endl;			
		return false;
	}
}

int main(int argc,char **argv) {
	if (argc<2) {
		cerr<<"Learn how to run your compiler" << endl;
		return -1;
	}
	filename=argv[1];
	ifstream file(filename);
    stringstream buffer;
    buffer << file.rdbuf();
	Tokenizer t(buffer.str());
	Token token;
	int count=0;
	int status=0;
	do {
		token=t.peek();
		cout << token << endl;
/*		if (token.type==UNRECOGNIZED) { 
		  cerr << filename<<':'<<linepos<<':' << " Unrecognized Token " << token.value << endl;
		  status=1;
	    }*/
		count++;
		if (unsignedReal(t)) {
			cout << "unsigned real Found" << endl;
			status=0;
		} else {
			status=1;
		}
	} while (token.type!=END_OF_TEXT && count<10);
	return status;
}
