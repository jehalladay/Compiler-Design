#include <iostream>
#include <iomanip>
#include <regex>
// #include <string>

using namespace std;


int main() {
    regex exp("([+-]?[0-9]*\\.[0-9]+([eE][+-]?[0-9]+)?)");
    // regex exp("[+-]");
    smatch sm;

    // string test = "wwk";
    string test = "3.145E+10";

    if(regex_search(test, sm, exp)) {
        cout << "it matches" << endl;
    } else {
        cout << "it doesnt match" << endl;
    }

    return 0;
}