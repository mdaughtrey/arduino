#include <functional>
#include <vector>
#include <utility>
#include <iterator>
#include <iostream>
#include <cstring>

#include "parser.h"
#include "src/Regexp.h"

using namespace std;

Parser::Parser(string & toparse, bool parseonly)
{
    _index = 0;
    parseonly = parseonly;
//    MatchState ms(const_cast<char *>(toparse.c_str()));
    parseloop(toparse);
}

//int Parser::parseloop( string & toparse, int index)
int Parser::parseloop(string & toparse)
{
    int index = 0;
    MatchState ms(const_cast<char *>(toparse.c_str()));
    while (index < static_cast<int>(toparse.length()) && !_error_state)
    {
        index += meta(toparse);
        index += special(toparse);
        index += printable(toparse);
//        index += _nonprintable(ms, toparse, index);
    }
    return index;
}

int Parser::meta(string & toparse)
{
    char match = REGEXP_MATCHED;
    char buffer[16];
    string matchon;

//    if ('<' != toparse[0])
//    {
//        return 0;
//    }

    MatchState ms(const_cast<char *>(toparse.c_str()));
    int index = 0;
    int length = 0;
    if (ms.Match("<(.*)>"))
    {
        length = 2;
        ms.Target(ms.GetCapture(buffer, 0));
    }
    //while (index < static_cast<int>(toparse.length()))
    while (REGEXP_MATCHED == match)
    {
        _inmeta = false;
        matchon = "alt%-(.*)";
        match = ms.Match(matchon.c_str(), index);
        if (REGEXP_MATCHED == match)
        {
            l_alt(true);
            _inmeta = true;
            index += matchon.length();
            ms.GetCapture(buffer, 0);
            string ss(buffer);
            index += parseloop(ss);
            continue;
        }
        matchon = "lalt%-(.*)";
        match = ms.Match(matchon.c_str(), index);
        if (REGEXP_MATCHED == match)
        {
            l_alt(true);
            index += matchon.length();
            continue;
        }
        match = ms.Match("ralt%-([^>]+)", index);
        if (REGEXP_MATCHED == match)
        {
            r_alt(true);
            index += 6;
            continue;
        }
        // Ctrl
        match = ms.Match("Ctrl%-([^>]+)", index);
        if (REGEXP_MATCHED == match)
        {
            l_ctrl(true);
            index += 5;
            continue;
        }
        match = ms.Match("L%-Ctrl%-([^>]+)", index);
        if (REGEXP_MATCHED == match)
        {
            l_ctrl(true);
            index += 7;
            continue;
        }
        match = ms.Match("R%-Ctrl%-([^>]+)", index);
        if (REGEXP_MATCHED == match)
        {
            r_ctrl(true);
            index += 7;
            continue;
        }
        // Shift
        match = ms.Match("Shift%-([^>]+)", index);
        if (REGEXP_MATCHED == match)
        {
            l_shift(true);
            index += 6;
            continue;
        }
        match = ms.Match("<L%-Shift%-([^>]+)", index);
        if (REGEXP_MATCHED == match)
        {
            l_shift(true);
            index += 8;
            continue;
        }
        match = ms.Match("R%-Shift%-([^>]+)", index);
        if (REGEXP_MATCHED == match)
        {
            r_shift(true);
            index += 8;
            continue;
        }

    } //  while (index < toparse.length());
    //} while (REGEXP_NOMATCH != match);

    return index;
}

int Parser::special(string & toparse)
{
    int index = 0;
    char buffer[16];
    if ('<' != toparse[0])
    {
        return 0;
    }

    MatchState ms(const_cast<char *>(toparse.c_str()));
    if (!ms.Match("<(.*)>", 0))
    {
        return 0;
    }
    ms.Target(ms.GetCapture(buffer, 0));

    if (ms.Match("<sleep (%d)", 0))
    {
        char buffer[16];
        index += strlen(ms.GetMatch(buffer));
    }
    if (ms.Match("tab", 0))
    {
        index += 5;
    }
    if (ms.Match("cr", 0))
    {
        index += 4;
    }
    if (ms.Match("up", 0))
    {
        index += 4;
    }
    if (ms.Match("left", 0))
    {
        index += 6;
    }
    if (ms.Match("down", 0))
    {
        index += 6;
    }
    if (ms.Match("right", 0))
    {
        index += 7;
    }
    if (ms.Match("back", 0))
    {
        index += 6;
    }
    if (ms.Match("del", 0))
    {
        index += 5;
    }
    if (ms.Match("esc", 0))
    {
        index += 5;
    }
    if (ms.Match("ins", 0))
    {
        index += 5;
    }
    if (ms.Match("home", 0))
    {
        index += 6;
    }
    if (ms.Match("end", 0))
    {
        index += 5;
    }
    if (ms.Match("pgup", 0))
    {
        index += 6;
    }
    if (ms.Match("pgdn", 0))
    {
        index += 6;
    }
    if (ms.Match("pause", 0))
    {
        index += 7;
    }
    if (ms.Match("scrl-lock", 0))
    {
        index += 11;
    }
    if (ms.Match("prtscr", 0))
    {
        index += 8;
    }
    if (ms.Match("capslock", 0))
    {
        index += 10;
    }
    if (ms.Match("win", 0))
    {
        index += 5;
    }
    if (ms.Match("fn", 0))
    {
        index += 4;
    }
    if (ms.Match("f(%d)", 0))
    {
        char buffer[16];
        index += strlen(ms.GetMatch(buffer));
    }
    return index;
}

int Parser::printable(string & toparse)
{
    MatchState ms(const_cast<char *>(toparse.c_str()));
    char matched[16];
    char match = ms.Match("[^<]*", 0);
    if (REGEXP_NOMATCH == match)
    {
        return 0;
    }
    // TODO escaped <
    ms.GetMatch(matched);
    int length = 0;
    for (char * ptr = matched; *ptr; length++, ptr++)
    {
        onekey(*ptr);
    }
    return length;
}

// Parser::_nonprintable( string & toparse, int index)
// {
//     return 0;
// }

void Parser::onekey(char ch)
{
    cout << "Onekey: " << ch << endl;
}


void Parser::l_alt(bool b)
{
    cout << "l_alt " << b << endl;
}

void Parser::r_alt(bool b)
{
    cout << "r_alt " << b << endl;
}

void Parser::l_ctrl(bool b)
{
    cout << "l_ctrl " << b << endl;
}

void Parser::r_ctrl(bool b)
{
    cout << "r_ctrl " << b << endl;
}

void Parser::l_shift(bool b)
{
    cout << "l_shift " << b << endl;
}

void Parser::r_shift(bool b)
{
    cout << "r_shift " << b << endl;
}

void Parser::set_meta(bool b)
{
    cout << "set_meta " << b << endl;
}
