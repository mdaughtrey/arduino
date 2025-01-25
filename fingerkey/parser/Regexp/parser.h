//#include <iostream>
#include <string>
#include <functional>
#include <vector>

using namespace std;
class Parser
{
protected:
    bool _error_state;
    bool _l_shift;
    bool _r_shift;
    bool _l_alt;
    bool _r_alt;
    bool _winkey;
    string _error_message;
    bool meta(string);
    bool printable(string &);
    bool special(string);
    bool non_printable(string &);
    bool _parseonly;
    int _index;

public:
    Parser(string & toparse, bool parseonly = false);
};
