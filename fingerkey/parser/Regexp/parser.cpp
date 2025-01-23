#include <functional>
#include <vector>
#include <utility>
#include <iterator>

#include "parser.h"
#include "src/Regexp.h"

Parser::Parser(string & toparse, bool parseonly)
{
    _index = 0;
    _parseonly = parseonly;
//    MatchState ms(const_cast<char *>(toparse.c_str()));
    _parseloop(toparse);
}

//int Parser::parseloop( string & toparse, int index)
int Parser::_parseloop(string & toparse)
{
    int index = 0;
    MatchState ms(const_cast<char *>(toparse.c_str()));
    while (index < static_cast<int>(toparse.length()) && !_error_state)
    {
        index += _meta(toparse);
        index += _printable(toparse);
//        index += _non_printable(ms, toparse, index);
    }
    return 0;
}

int Parser::_meta(string & toparse)
{
    char match;
    char buffer[16];

    if ('<' != toparse[0])
    {
        return 0;
    }

    MatchState ms(const_cast<char *>(toparse.c_str()));
    int index = 1;
    do
    {
        match = ms.Match("Alt%-(.-)>", index);
        if (REGEXP_MATCHED == match)
        {
            _l_alt(true);
            index += 5;
            ms.GetCapture(buffer, 0);
            string ss(buffer);
            index += _parseloop(ss);
            continue;
        }
        match = ms.Match("L-Alt%-([^>]+)", index);
        if (REGEXP_MATCHED == match)
        {
            _l_alt(true);
            index += 6;
            continue;
        }
        match = ms.Match("R-Alt%-([^>]+)", index);
        if (REGEXP_MATCHED == match)
        {
            _r_alt(true);
            index += 6;
            continue;
        }
        // Ctrl
        match = ms.Match("Ctrl%-([^>]+)", index);
        if (REGEXP_MATCHED == match)
        {
            _l_ctrl(true);
            index += 5;
            continue;
        }
        match = ms.Match("L-Ctrl%-([^>]+)", index);
        if (REGEXP_MATCHED == match)
        {
            _l_ctrl(true);
            index += 7;
            continue;
        }
        match = ms.Match("R-Ctrl%-([^>]+)", index);
        if (REGEXP_MATCHED == match)
        {
            _r_ctrl(true);
            index += 7;
            continue;
        }
        // Shift
        match = ms.Match("Shift%-([^>]+)", index);
        if (REGEXP_MATCHED == match)
        {
            _l_shift(true);
            index += 6;
            continue;
        }
        match = ms.Match("<-Shift%-([^>]+)", index);
        if (REGEXP_MATCHED == match)
        {
            _l_shift(true);
            index += 8;
            continue;
        }
        match = ms.Match("R-Shift%-([^>]+)", index);
        if (REGEXP_MATCHED == match)
        {
            _r_shift(true);
            index += 8;
            continue;
        }

    } while (REGEXP_NOMATCH != match);

    return index;
}

int Parser::_printable(string & toparse)
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
    for (char * ptr = matched; *ptr, length++; ptr++)
    {
        _onekey(*ptr);
    }
    return length;
}

// Parser::_non_printable( string & toparse, int index)
// {
//     return 0;
// }

void Parser::_onekey(char ch)
{
}
