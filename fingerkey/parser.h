//#include <iostream>
#include <string>
#include <functional>
#include <vector>
#include <Keyboard.h>

//using namespace std;
class Parser
{
protected:
    bool _error_state;
    bool _l_shift;
    bool _r_shift;
    bool _l_alt;
    bool _r_alt;
    bool _winkey;
    std::string _error_message;
    bool meta(std::string);
    bool printable(std::string &);
    bool special(std::string);
    bool non_printable(std::string &);
    bool _parseonly;
    int _index;

public:
    Parser(std::string & toparse, bool parseonly = false);
};
