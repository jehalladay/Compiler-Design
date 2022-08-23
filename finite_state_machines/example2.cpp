

/*
  
{+|-}?{0-9}*.{0-9}+{{e|E}{+|-}?{0-9}+}?

*/

using namespace std;

enum State {    
    NOPLUS,
    PREDIGIT,
    POINT,
    POSTDIGIT,
    EXP,
    EXPNOPLUS,
    EXPDIGIT,
    ERROR
};

enum Symbol {
    SIGN,
    DIGIT,
    DECPOINT,
    E,
    OTHER
};

// State current = LOCKED;

State doAction(Symbol sym, State state) {
    // if (current == LOCKED && s == COIN) {
    //     current = UNLOCKED;
    // }
    // if (current == UNLOCKED && s == PUSH) {
    //     current = LOCKED;
    // }

    State val = POINT;

    return val;
}




int main() {

    State start = NOPLUS;
    Symbol nextSymbol = DIGIT;

    State current = doAction(nextSymbol, start);


    return 0;
}