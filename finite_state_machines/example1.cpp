

using namespace std;

enum State {
    LOCKED,
    UNLOCKED
};

enum Symbol {
    PUSH,
    COIN
};

State current = LOCKED;

void doAction(Symbol s) {
    if (current == LOCKED && s == COIN) {
        current = UNLOCKED;
    }
    if (current == UNLOCKED && s == PUSH) {
        current = LOCKED;
    }
}


int main() {

    Symbol action = PUSH;

    doAction(action);


    return 0;
}