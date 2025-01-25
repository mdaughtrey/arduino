//#include <functional>
//#include <utility>
#include <iterator>
#include <iostream>
#include <cstring>
#include <string>
//#include <algorithm>
#include <sstream>

#include "parser.h"
#include "src/Regexp.h"

using namespace std;

Parser::Parser(string & toparse, bool parseonly)
{
    _index = 0;
    parseonly = parseonly;

    stringstream tokens(toparse);
    string word;

    while (tokens >> word)
    {
        if (meta(word))
        {
            continue;
        }
        if (special(word))
        {
            continue;
        }
        if (printable(word))
        {
            continue;
        }
        if (non_printable(word))
        {
            continue;
        }
    }
}

// Expecting a meta command without the brackets
bool Parser::meta(string toparse)
{
    bool state = !(toparse[0] == '^');
    if (state)
    {
        toparse = toparse.substr(1);
    }
    if (toparse == "l-alt" || toparse == "alt")
    {
//        l_alt(state);
        return true;
    }
    if (toparse == "r-alt")
    {
//        r_alt(state);
        return true;
    }
    if (toparse == "l-ctrl" || toparse == "ctrl")
    {
//        l_ctrl(state);
        return true;
    }
    if (toparse == "r-ctrl")
    {
//        r_ctrl(state);
        return true;
    }
    if (toparse == "l-shift" || toparse == "shift")
    {
//        l_shift(state);
        return true;
    }
    if (toparse == "r-shift")
    {
//        r_shift(state);
        return true;
    }
    return false;
}

bool Parser::special(string toparse)
{
    if (toparse == "tab")
    {
        return true;
    }
    if (toparse == "enter")
    {
        return true;
    }
    if (toparse == "up")
    {
        return true;
    }
    if (toparse == "down")
    {
        return true;
    }
    if (toparse == "left")
    {
        return true;
    }
    if (toparse == "right")
    {
        return true;
    }
    if (toparse == "insert")
    {
        return true;
    }
    if (toparse == "delete")
    {
        return true;
    }
    if (toparse == "home")
    {
        return true;
    }
    if (toparse == "end")
    {
        return true;
    }
    if (toparse == "pgup")
    {
        return true;
    }
    if (toparse == "pgdn")
    {
        return true;
    }
    if (toparse == "pause")
    {
        return true;
    }
    if (toparse == "scrl-lock")
    {
        return true;
    }
    if (toparse == "prtscr")
    {
        return true;
    }
    if (toparse == "capslock")
    {
        return true;
    }
    if (toparse == "win")
    {
        return true;
    }
    if (toparse == "fn")
    {
        return true;
    }
    MatchState ms(const_cast<char *>(toparse.c_str()));
    if (ms.Match("f(%d*)"))
    {
        char buffer[4];
        ms.GetCapture(buffer, 0);
        // fn(buffer);
        return true;
    }
    if (ms.Match("sleep (%d)"))
    {
        char buffer[5];
        ms.GetCapture(buffer, 0);
        // sleep(buffer);
        return true;
    }
    return false;
}

bool Parser::printable(string & toparse)
{
    if (toparse[0] != '<')
    {
        return false;
    }

    for (const char * ptr = toparse.c_str(); *ptr; ptr++)
    {
        cout << "Onekey: " << *ptr << endl;
    }
    return true;
}

bool Parser::non_printable(string & toparse)
{
    return false;
}

