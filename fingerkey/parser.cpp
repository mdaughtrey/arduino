#include <Arduino.h>
#include <Wire.h>
#include <functional>
//#include <utility>
#include <iterator>
#include <iostream>
//#include <cstd::string>
#include <string>
#include <map>
//#include <algorithm>
#include <sstream>

#include "parser.h"
#include <Regexp.h>
#include <Keyboard.h>

//typedef std::vector<std::function<bool(std::string)>> MyMethod;
typedef std::function<bool(std::string)> MyMethod;
typedef std::pair<std::string, MyMethod> MethodPair;
//    methods.push_back(std::bind(&Parser::meta, this, std::placeholders::_1));

Parser::Parser(std::string & toparse, bool parseonly)
{
//    std::vector<std::function<bool(std::string)>> methods;
    Keyboard.releaseAll();
    std::stringstream tokens(toparse);
    std::string word;

    while (tokens >> word)
    {
        Serial.print("Processing [");
        Serial.print(word.c_str());
        Serial.println("]");
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
        if (keypad(word))
        {
            continue;
        }
        if (fkeys(word))
        {
            continue;
        }
        Serial.print("Unknown parser token: ");
        Serial.println(word.c_str());
    }
}

// Expecting a meta command without the brackets
bool Parser::meta(std::string toparse)
{
    Serial.print("meta processing 1 [");
    Serial.print(toparse.c_str());
    Serial.println("]");

    bool state = !(toparse[0] == '^');
    if (!state)
    {
        toparse = toparse.substr(1);
    }
    Serial.print("meta processing 2 [");
    Serial.print(toparse.c_str());
    Serial.println("]");
    MetaMap::iterator found = Parser::metamap_.find(toparse);
    if (found == Parser::metamap_.end())
    {
        return false;
    }
    if (state)
    {
        Keyboard.press(found->second);
    }
    else
    {
        Keyboard.release(found->second);
    }
    return true;
}

bool Parser::special(std::string toparse)
{
    MetaMap::iterator found = Parser::specialmap_.find(toparse);
    if (found == Parser::specialmap_.end())
    {
        return false;
    }
    Keyboard.write(found->second);
    return true;
}

bool Parser::printable(std::string & toparse)
{
    if (toparse[0] != '<')
    {
        return false;
    }

    Keyboard.write(reinterpret_cast<const uint8_t *>(toparse.substr(1).c_str()), toparse.length() - 1);
    return true;
}

bool Parser::non_printable(std::string & toparse)
{
    return false;
}

bool Parser::fkeys(std::string & toparse)
{
    if (toparse[0] != 'f')
    {
        return false;
    }
    MetaMap::iterator found = Parser::fkeymap_.find(toparse.substr(1));
    if (found == Parser::fkeymap_.end())
    {
        return false;
    }
    Keyboard.write(found->second);
    return true;
}

bool Parser::keypad(std::string & toparse)
{
    if (toparse[0] != '#')
    {
        return false;
    }
    MetaMap::iterator found = Parser::keypadmap_.find(toparse.substr(1));
    if (found == Parser::keypadmap_.end())
    {
        return false;
    }
    Keyboard.write(found->second);
    return true;
}

void Parser::usage()
{
    Serial.println(F("Prefixes: Text < NegateMeta ^ Keypad # FKey f"));
    Serial.println(F("Meta: "));
    MetaMap::iterator iter = Parser::metamap_.begin();
    for (; iter!= Parser::metamap_.end(); iter++)
    {
        Serial.print(iter->first.c_str());
        Serial.print(' ');
    }
    Serial.println("");

    Serial.println(F("Special: "));
    iter = Parser::specialmap_.begin();
    for (; iter!= Parser::specialmap_.end(); iter++)
    {
        Serial.print(iter->first.c_str());
        Serial.print(' ');
    }
    Serial.println("");

    Serial.println(F("Keypad: "));
    iter = Parser::keypadmap_.begin();
    for (; iter!= Parser::keypadmap_.end(); iter++)
    {
        Serial.print(iter->first.c_str());
        Serial.print(' ');
    }
    Serial.println("");

    Serial.println(F("FKey: "));
    iter = Parser::fkeymap_.begin();
    for (; iter!= Parser::fkeymap_.end(); iter++)
    {
        Serial.print("");
        Serial.print(iter->first.c_str());
        Serial.print(' ');
    }
    Serial.println("");
}

Parser::MetaMap Parser::fkeymap_ = {
    {"1", KEY_F1 },
    {"2", KEY_F2 },
    {"3", KEY_F3 },
    {"4", KEY_F4 },
    {"5", KEY_F5 },
    {"6", KEY_F6 },
    {"7", KEY_F7 },
    {"8", KEY_F8 },
    {"9", KEY_F9 },
    {"10", KEY_F10 },
    {"11", KEY_F11 },
    {"12", KEY_F12 },
    {"13", KEY_F13 },
    {"14", KEY_F14 },
    {"15", KEY_F15 },
    {"16", KEY_F16 },
    {"17", KEY_F17 },
    {"18", KEY_F18 },
    {"19", KEY_F19 },
    {"20 ", KEY_F20 },
    {"21", KEY_F21 },
    {"22", KEY_F22 },
    {"23", KEY_F23 },
    {"24", KEY_F24 },
};

Parser::MetaMap Parser::keypadmap_ = {
    { "/", KEY_KP_SLASH },
    { "*", KEY_KP_ASTERISK },
    { "-", KEY_KP_MINUS },
    { "+", KEY_KP_PLUS },
    { "enter", KEY_KP_ENTER },
    { "1", KEY_KP_1 },
    { "2", KEY_KP_2 },
    { "3", KEY_KP_3 },
    { "4", KEY_KP_4 },
    { "5", KEY_KP_5 },
    { "6", KEY_KP_6 },
    { "7", KEY_KP_7 },
    { "8", KEY_KP_8 },
    { "9", KEY_KP_9 },
    { "0", KEY_KP_0 },
    { ".", KEY_KP_DOT }
};

Parser::MetaMap Parser::metamap_ = {
    {"l-alt", KEY_LEFT_ALT},
    {"alt", KEY_LEFT_ALT},
    {"r-alt", KEY_RIGHT_ALT},
    {"l-ctrl", KEY_LEFT_CTRL},
    {"ctrl", KEY_LEFT_CTRL},
    {"r-ctrl", KEY_RIGHT_CTRL},
    {"l-shift", KEY_LEFT_SHIFT},
    {"shift", KEY_LEFT_SHIFT},
    {"r-shift", KEY_RIGHT_SHIFT}
};

Parser::MetaMap Parser::specialmap_ = {
    {"tab", KEY_TAB },
    {"esc", KEY_ESC },
    {"enter", KEY_RETURN },
    {"up", KEY_UP_ARROW },
    {"down", KEY_DOWN_ARROW },
    {"left", KEY_LEFT_ARROW },
    {"right", KEY_RIGHT_ARROW },
    {"insert", KEY_INSERT },
    {"backspace", KEY_BACKSPACE },
    {"delete", KEY_DELETE },
    {"home", KEY_HOME },
    {"end", KEY_END },
    {"pgup", KEY_PAGE_UP },
    {"pgdn", KEY_PAGE_DOWN },
    {"pause", KEY_PAUSE },
    {"scrl-lock", KEY_SCROLL_LOCK },
    {"prtscr", KEY_PRINT_SCREEN },
    {"capslock", KEY_CAPS_LOCK },
    {"lgui", KEY_LEFT_GUI },
    {"rgui", KEY_RIGHT_GUI },
    {"numlock", KEY_NUM_LOCK },
    {"space", ' ' }
};
