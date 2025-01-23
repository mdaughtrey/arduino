//#include <iostream>
#include <string>
#include <functional>

using namespace std;
class Parser
{
protected:
    bool _error_state;
//    bool _l_shift;
//    bool _r_shift;
//    bool _l_alt;
//    bool _r_alt;
//    bool _winkey;
    string _error_message;
    int _meta(string &);
    int _printable(string &);
//    int _non_printable(string &);
    bool _parseonly;
    int _index;

    void _onekey(char ch);
    void _l_alt(bool){};
    void _r_alt(bool){};
    void _l_ctrl(bool){};
    void _r_ctrl(bool){};
    void _l_shift(bool){};
    void _r_shift(bool){};
    void _set_meta(bool){};
    int _parseloop(string & toparse);

public:
    Parser(string & toparse, bool parseonly = false);
};
