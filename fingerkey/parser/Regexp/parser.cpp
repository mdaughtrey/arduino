#include "parser.h"


Parser::Parser(std::string & toparse)
{
    int index = 0;
    int length = toparse.length();

    while (index < length && !errorstate)
    {
        index += meta(toparse);
        index += printable(toparse);
        index += non_printable(toparse);
    }
}

int Parser::meta(string & toparse)
{
    return 0;
}

int Parser::printable(string & toparse)
{
    return 0;
}

Parser::non_printable(string & toparse)
{
    return 0;
}
