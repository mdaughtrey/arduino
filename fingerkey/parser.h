//#include <iostream>
#include <string>
#include <functional>
//#include <vector>
#include <map>

//using namespace std;
class Parser
{
    typedef std::function<bool(std::string)> MyMethod;
    typedef std::pair<std::string, MyMethod> MethodPair;
    typedef std::map<std::string, uint8_t> MetaMap;
protected:
    bool _error_state;
    std::string _error_message;
    bool meta(std::string);
    bool printable(std::string &);
    bool special(std::string);
    bool non_printable(std::string &);
    bool _parseonly;
    bool fkeys(std::string &);
    bool keypad(std::string &);

public:
    Parser(std::string & toparse, bool parseonly = false);
    static void usage();
    static MetaMap metamap_;
    static MetaMap specialmap_;
    static MetaMap keypadmap_;
    static MetaMap fkeymap_;
};
