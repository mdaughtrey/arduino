//#include <iostream>
#include <string>
#include <functional>

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
    int meta(string &);
    int printable(string &);
    int special(string &);
//    int _non_printable(string &);
    bool _inmeta;
    bool _parseonly;
    int _index;

    void onekey(char ch);
    void l_alt(bool);
    void r_alt(bool);
    void l_ctrl(bool);
    void r_ctrl(bool);
    void l_shift(bool);
    void r_shift(bool);
    void set_meta(bool);
    int parseloop(string & toparse);

public:
    Parser(string & toparse, bool parseonly = false);
};
