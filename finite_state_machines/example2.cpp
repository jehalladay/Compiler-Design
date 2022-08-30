/*
Creating a finite state machine using the following pattern:
    {+|-}?{0-9}*.{0-9}+{{e|E}{+|-}?{0-9}+}?

    James Halladay, c. 8/30/22 
*/

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

using namespace std;

enum State {    
    NOPLUS,
    PREDIGIT,
    POINT,
    POSTDIGIT,
    EXP,
    EXPHASSIGN,
    EXPDIGIT,
    ACCEPT,
    ERROR
};


enum Symbol {
    SIGN,
    DIGIT,
    DECPOINT,
    E,
    OTHER
};

const int MAX_STATES = 9;
const int MAX_SYMBOLS = 5;

string States[MAX_STATES] = {
    "NOPLUS",
    "PREDIGIT",
    "POINT",
    "POSTDIGIT",
    "EXP",
    "EXPHASSIGN",
    "EXPDIGIT",
    "ACCEPT",
    "ERROR"
};

State stt[MAX_SYMBOLS][MAX_STATES] = {
    // NOPLUS, PREDIGIT, POINT    , POSTDIGIT , EXP       , EXPHASSIGN, EXPDIGIT, ACCEPT, ERROR
    {PREDIGIT, ERROR   , ERROR    , ACCEPT    , EXPHASSIGN, ERROR     , ACCEPT  , ACCEPT, ERROR}, // SIGN
    {PREDIGIT, PREDIGIT, POSTDIGIT, POSTDIGIT , EXPDIGIT  , EXPDIGIT  , EXPDIGIT, ACCEPT, ERROR}, // DIGIT
    {POINT   , POINT   , ERROR    , ACCEPT    , ERROR     , ERROR     , ACCEPT  , ACCEPT, ERROR}, // DECPOINT
    {ERROR   , ERROR   , ERROR    , EXP       , ERROR     , ERROR     , ACCEPT  , ACCEPT, ERROR}, // E
    {ERROR   , ERROR   , ERROR    , ACCEPT    , ERROR     , ERROR     , ACCEPT  , ACCEPT, ERROR}, // OTHER
};

State transition(State current, Symbol s) {
    return stt[s][current];
}

Symbol parse(char c) {

    Symbol s = OTHER;

    if (c == '+' || c == '-') {
        s = SIGN;
    } else if (c >= '0' && c <= '9') {
        s = DIGIT;
    } else if (c == '.') {
        s = DECPOINT;
    } else if (c == 'e' || c == 'E') {
        s = E;
    }

    return s;
}

State endState(State current) {
    if (current == PREDIGIT || current == POSTDIGIT || current == EXPDIGIT) {
        current = ACCEPT;
    } 

    return current;
}

class TestCase { // Pure Abstract Class
    public:
    virtual bool doTest() = 0; // Virtual allows subclasses to override
};

vector<TestCase*> testCases;

class SimpleTests: public TestCase {
    string test;

    public:
        SimpleTests(string t = "0.1") {
            test = t;
        }
        
        bool doTest() {
            string test = "1.2e3";
            State current = NOPLUS;
            Symbol action;
            
            for (auto it = test.begin(); it != test.end() && current != ACCEPT && current != ERROR; ++it) {
                action = parse(*it);
                current = transition(current, action);
            }
            
            current = endState(current);

            return current == ACCEPT;
        }
};

int main() {

    string test = "q+1.0e+1";
    State start = NOPLUS;
    Symbol action = SIGN;

    State current = NOPLUS;
    
    for (auto it = test.begin(); it != test.end() && current != ACCEPT && current != ERROR; ++it) {
        action = parse(*it);
        current = transition(current, action);
    }

    State second = NOPLUS;

    for(auto c:test) {
        action = parse(c);
        second = transition(second, action);
    }

    cout << States[current] << endl;
    cout << States[second] << endl;

    if (current == ACCEPT || current == POSTDIGIT || current == EXPDIGIT) {
        cout << "Matches a Floating Point Number" << endl;
    } else {
        cerr << "Does not match a Floating Point Number" << endl;
    }

    return 0;
}

