//#include <iostream>
#include <string>

using namespace std;
class Parser
{
protected:
    bool error_state;
    bool l_shift;
    bool r_shift;
    bool l_alt;
    bool r_alt;
    bool winkey;
    string error_message;
    int meta(string &);
    int printable(string &);
    int non_printable(string &);

public:
    Parser(std::string & toparse);
};
