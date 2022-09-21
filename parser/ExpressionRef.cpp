#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctype.h>
#include <vector>

using namespace std;

class TestCase {
	protected:
	string name;
	public:
	TestCase(string newName) {
		name=newName;
	}
	bool test() {
		cout << "Test " << name << endl;
		bool retval=doTest();
		cout << "End Test " << name;
		if (retval) cout << "->Passed" << endl;
		else cout << "->Failed" << endl;
		return retval;
	}
	virtual bool doTest()=0;
};

vector<TestCase *> testcases;

/*
from :https://condor.depaul.edu/ichu/csc447/notes/wk2/pascal.html 
...
T<identifier> ::= <letter > {<letter or digit>}
T<sign> ::= + | -
P?<unsigned number> ::= <unsigned integer> | <unsigned real>
T<unsigned integer> ::= <digit> {<digit>}
P<unsigned real> ::= <unsigned integer> . <unsigned integer> | 
					<unsigned integer> . <unsigned integer> E <scale factor>|
					<unsigned integer> E <scale factor>
P<scale factor> ::= <unsigned integer> | 
				   <sign> <unsigned integer>
...
<expression> ::= <simple expression> | <simple expression> <relational operator> <simple expression>
T <relational operator> ::= = | <> | < | <= | >= | >   
P?<simple expression> ::= <term> | <sign> <term>| <simple expression> <adding operator> <term>
T<adding operator> ::= + | - | or  Note: SIGN token is first two
P<term> ::= <factor> | <term> <multiplying operator> <factor> --> <factor> <multiplying operator>  <term>
T <multiplying operator> ::= * | / | div | mod | and
P<factor> ::= <variable> | <unsigned constant> | ( <expression> ) 
P<unsigned constant> ::= <unsigned number> | <string> 
T<string>:= '<character>{<character>}' 
 ...
P<variable> ::= <identifier>

*/

enum TokenType{RELATIONAL_OPERATOR,ADDING_OPERATOR,MULTIPLYING_OPERATOR,
				SIGN,IDENTIFIER,UNSIGNED_INTEGER,E,DOT,UNRECOGNIZED,
				OPEN_PAREN,CLOSE_PAREN,STRING,END_OF_TEXT};

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
			case OPEN_PAREN : return "OPEN_PAREN";
			case CLOSE_PAREN : return "CLOSE_PAREN";
			case END_OF_TEXT : return "END_OF_TEXT";
			case STRING : return "STRING" ;
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
	int linepos,charpos;
	string text;
	unsigned pos;
	public:
	static string filename;
	bool error(string message) {
		cerr << filename<<':'<<linepos<<':' << message << endl;	
		return false;
	}
	bool message(string message) {
		cout << filename<<':'<<linepos<<':' << message <<": Next Token "<<peek() << " next text:" <<text.substr(pos,10) <<endl;	
		return true;
	}
	Tokenizer(){
		text="";
	}
	void readFile(string newFilename) {
		ifstream file(newFilename);
		Tokenizer::filename=newFilename;
		stringstream buffer;
		buffer << file.rdbuf();
		text=buffer.str();
		pos=0;
		linepos=1;
		charpos=1;
	}
	void setText(string newText="") {
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
		//cout << "Next character is " << text[pos] << endl;
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
		if (c=='\'') {
			int offset=1;
			while (pos+offset<text.size() && text[pos+offset]!='\'') {
		        if (text[pos+offset]=='\n') linepos++;
				offset++;
			}
			if (text[pos+offset]=='\'') offset++;
			Token retval=Token(STRING,text.substr(pos,offset));
			pos+=offset;
			return retval;
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
string Tokenizer::filename;

class Parser {
	Tokenizer tokens;
	public:
	Token peek() { return tokens.peek(); }
	Tokenizer& getTokenizer() { return tokens; }
	Parser(string newFilename) { 
		tokens=Tokenizer();
		tokens.readFile(newFilename); 
	}
	bool expression(Tokenizer &t) {
		t.message("Parsing Expression next is ");
		if (simpleExpression(t))
		  if (t.peek().type==RELATIONAL_OPERATOR) {
			  t.next();
			  if (simpleExpression(t)) return t.message("Found Expression relational version");
			  else return t.error("Expected a simple expression after relational operator");
		  }
		  else return t.message("!!Found Expression no relational");
		else
		  return t.error("Expected an expression");
	}
	bool simpleExpression(Tokenizer &t) {
		t.message("Parsing Simple Expression next is ");
		if (t.peek().type==SIGN) {
			t.next();
			if (term(t)) return t.message("!!Found Simple Expression after sign");
			else return t.error("Expected a term after sign"); 
		}
		if (term(t))
		  if (t.peek().type==ADDING_OPERATOR || t.peek().type==SIGN) { 
//		  while (t.peek().type==ADDING_OPERATOR  || t.peek().type==SIGN)  {
			t.next();
		    if (simpleExpression(t))
		      return t.message("!!Found Simple Expression after adding operator");
		    else 
			  return t.error("Expected an simple expression after adding operator");
		  }
		  else 
		    return t.message("!!Found Simple Expression alone");
		else
			return t.error("Expected a term");
		return true;
	}
	bool term(Tokenizer &t) {
		t.message("Parsing Term next is ");
		if (factor(t)) {
			if (t.peek().type==MULTIPLYING_OPERATOR) {
				t.next();
				if (term(t)) return t.message("!!Found Term after Multiplying Operator");
				else return t.error("Missing term");
			}
			return t.message("!!Found Term alone");
		}
		return t.error("Expected a term");
	}
	bool factor(Tokenizer &t) { /// Yeah Recursion !!!!
		t.message("Parsing Factor next is ");
		if (t.peek().type==OPEN_PAREN) {
			t.next();
			if (expression(t)) {
				if (t.next().type==CLOSE_PAREN) {
					return t.message("!!Found Factor with sub expression");
				} 
				return t.error("Missing ) after expression");
			}
			return t.error("Expected an expression");
		}
		Tokenizer temp=t;
		if (variable(temp)) {
			t=temp;
			return t.message("!!Found Factor variable");
		}
		temp=t;
		if (unsignedConstant(temp)) {
			t=temp;
			return t.message("!!Found Factor unsigned constant");
		}
		return t.error("Expected (Expression), Variable, Unsigned Constant");
	}
	bool scaleFactor(Tokenizer &t) {
		t.message("Parsing Scale Factor next is ");
		Token tok=t.next();
		if (tok.type==SIGN) {
			tok=t.next();
			if (tok.type==UNSIGNED_INTEGER) return t.message("!!Found Scale Factor after sign");
			return t.error("Sign was not followed by unsigned integer");
		}
		if (tok.type==UNSIGNED_INTEGER)  return t.message("!!Found Scale Factor no sign");
		return t.error("Neither Sign or Unsigned Integer Present");
	}
	bool unsignedConstant(Tokenizer &t) {
		t.message("Parsing Unsigned Constant");
		if (t.peek().type==STRING) {
			t.next();
			return t.message("!!Found unsigned constant String");
		}
		Tokenizer temp=t;
		if (unsignedNumber(temp)) {
			t=temp;
			return t.message("!!Found unsigned constant number");
		}
		return t.error("Expected a Unsigned Constant");
	}
	bool unsignedNumber(Tokenizer &t) {
		t.message("Parsing unsigned number");
		Tokenizer temp=t;
		if (unsignedReal(temp)) {
			t=temp;
			return t.message("!!Found unsigned number real");
		}
		if (t.peek().type==UNSIGNED_INTEGER) {
			t.next();
			return t.message("!!Found unsigned number integer");
		} 
		return t.error("Expected a Unsigned Real or Unsigned Integer");
	}
	bool variable(Tokenizer &t) {
		t.message("Parsing Variable");
		if (t.next().type==IDENTIFIER) return t.message("!!Found Variable");
		else return t.error("Expected Variable");
	}
	bool unsignedReal(Tokenizer &t) {
		t.message("Parsing unsigned real");
		Token tok=t.next();
		if (tok.type==UNSIGNED_INTEGER) {
			tok=t.next();
			if (tok.type==DOT){
				tok=t.next();
				if (tok.type==UNSIGNED_INTEGER) {
					tok=t.peek();
					if (tok.type==E) {
						t.next();
						if (scaleFactor(t)) return t.message("!!Found unsigned real complex real");;
						return t.error("Missing Scale Factor after E");
					}
					return true;
				}
			}
			if (tok.type==E) {
				if (scaleFactor(t)) return t.message("!!Found unsigned real with E");
				return t.error("Missing Scale Factor after E ");
			}
			return t.error("Neither . or E followsed unsigned integer");
		}
		return t.error(" Missing Unsigned Integer ");
	}
};

class TokenizerTest:public TestCase {  // Tokenizer Test at most 100 tokens
	protected:
		string text;
		bool shouldFail;
	public:
	    TokenizerTest(string newText=".",bool newShouldFail=false) :TestCase("TokenizerTest"){
			text=newText;
			shouldFail=newShouldFail;
		}
	virtual bool doTest() {
		Tokenizer t;
		t.setText(text);
		int count=0;
		int status=0;
		Token token;
		do {
			token=t.next();
			cout << token << endl;
			if (token.type==UNRECOGNIZED) { 
				t.error(" Unrecognized Token ");
				status=1;
			}
			count++;
		} while (token.type!=END_OF_TEXT && count<100);
		return status==0;
    }
};

class TokenizerTestGood:public TokenizerTest {
	public:
		TokenizerTestGood():TokenizerTest("{This is a comment}var * 	2.0E10 /12 \n+ 6"){
			name="TokenizerTestGood "+name;
		}
};

class ParserTest:public TestCase {
	protected:
		Parser *p;
		bool shouldFail;
	public:
	ParserTest(string newFilename,bool newShouldFail=false):TestCase("ParserTest") {
		p=new Parser(newFilename);
		shouldFail=newShouldFail;
	}
	~ParserTest() {
		delete p;
	}
};

class ParserTestScaleFactor:public ParserTest{
	public:
	ParserTestScaleFactor():ParserTest("ScaleFactor.pas") {
		name="ScaleFactor "+name;
	}
	bool doTest() {
		int count=0;
		while (p->peek().type!=END_OF_TEXT) {
			if (p->scaleFactor(p->getTokenizer())) {
				count++;
			}
		}
		return count==3;
	}
};

class ParserTestUnsignedReal:public ParserTest{
	public:
	ParserTestUnsignedReal():ParserTest("UnsignedReal.pas") {
		name="UnsignedReal "+name;
	}
	bool doTest() {
		int count=0;
		while (p->peek().type!=END_OF_TEXT) {
			if (p->unsignedReal(p->getTokenizer())) {
				count++;
			}
		}
		return count==1;
	}
};

class ParserWholeEnchilada:public ParserTest{
	public:
	ParserWholeEnchilada():ParserTest("WholeEnchilada.pas") {
		name="UnsignedReal "+name;
	}
	bool doTest() {
		int count=0;
		while (p->peek().type!=END_OF_TEXT) {
			cout << "Expression Count is "<< count << endl;
			if (p->expression(p->getTokenizer())) {
				count++;
			}
		}
		return count==1;
	}
};

class ParserWholeEnchilada2:public ParserTest{
	public:
	ParserWholeEnchilada2():ParserTest("WholeEnchilada.old.pas") {
		name="UnsignedReal "+name;
	}
	bool doTest() {
		int count=0;
		while (p->peek().type!=END_OF_TEXT) {
			cout << "Expression Count is "<< count << endl;
			if (p->expression(p->getTokenizer())) {
				count++;
			}
		}
		return count==1;
	}
};


bool runTests() {
	testcases.push_back(new TokenizerTest());
	testcases.push_back(new TokenizerTestGood());
	testcases.push_back(new ParserTestScaleFactor());
//	testcases.push_back(new ParserTestUnsignedReal());
	// testcases.push_back(new ParserWholeEnchilada());
	// testcases.push_back(new ParserWholeEnchilada2());
	bool success=true;
	for (auto test:testcases)
		if (!test->test()) success=false;
	if (success) {
		cout << "All passed" << endl;
		return true;
	}
	cerr << "Failed some test"<<endl; 
	return false;	
}

int main(int argc,char **argv) {
	if (argc<2) {
		cerr<<"Expression <filename>" << endl;
		return -1;
	}
    Parser p(argv[1]);
    int status=0;
    if(!runTests()) status=1;
	return status;
}
