#include <Arduino.h>
#include <Wire.h>
//#include <functional>
//#include <utility>
#include <iterator>
#include <iostream>
//#include <cstd::string>
#include <string>
//#include <algorithm>
#include <sstream>

#include "parser.h"
#include <Regexp.h>

Parser::Parser(std::string & toparse, bool parseonly)
{
    _index = 0;
    parseonly = parseonly;

    Keyboard.releaseAll();
    std::stringstream tokens(toparse);
    std::string word;

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
        if (keypad(word))
        {
            continue;
        }
        if (fkeys(word))
        {
            continue;
        }
    }
    }
}

// Expecting a meta command without the brackets
bool Parser::meta(std::string toparse)
{
    bool state = !(toparse[0] == '^');
    if (state)
    {
        toparse = toparse.substr(1);
    }
    if (toparse == "l-alt" || toparse == "alt")
    {
        Keyboard.send(KEY_LEFT_ALT);
        return true;
    }
    if (toparse == "r-alt")
    {
        Keyboard.send(KEY_RIGHT_ALT);
        return true;
    }
    if (toparse == "l-ctrl" || toparse == "ctrl")
    {
        Keyboard.send(KEY_LEFT_CTRL);
        return true;
    }
    if (toparse == "r-ctrl")
    {
        Keyboard.send(KEY_RIGHT_CTRL);
        return true;
    }
    if (toparse == "l-shift" || toparse == "shift")
    {
        Keyboard.send(KEY_LEFT_SHIFT);
        return true;
    }
    if (toparse == "r-shift")
    {
        Keyboard.send(KEY_RIGHT_SHIFT);
        return true;
    }
    return false;
}

bool Parser::special(std::string toparse)
{
    MatchState ms(const_cast<char *>(toparse.c_str()));
    if (toparse == "tab")
    {
        Keyboard.send(KEY_TAB);
        return true;
    }
    if (toparse == "enter")
    {
        Keyboard.write(KEY_RETURN);
        return true;
    }
    if (toparse == "up")
    {
        Keyboard.send(KEY_UP_ARROW);
        return true;
    }
    if (toparse == "down")
    {
        Keyboard.send(KEY_DOWN_ARROW);
        return true;
    }
    if (toparse == "left")
    {
        Keyboard.send(KEY_LEFT_ARROW);
        return true;
    }
    if (toparse == "right")
    {
        Keyboard.send(KEY_RIGHT_ARROW);
        return true;
    }
    if (toparse == "insert")
    {
        Keyboard.send(KEY_INSERT);
        return true;
    }
    if (toparser == "backspace")
    {
        Keyboard.send(KEY_BACKSPACE);
        return true;
    }
    if (toparse == "delete")
    {
        Keyboard.send(KEY_DELETE);
        return true;
    }
    if (toparse == "home")
    {
        Keyboard.send(KEY_HOME);
        return true;
    }
    if (toparse == "end")
    {
        Keyboard.send(KEY_END);
        return true;
    }
    if (toparse == "pgup")
    {
        Keyboard.send(KEY_PAGE_UP);
        return true;
    }
    if (toparse == "pgdn")
    {
        Keyboard.send(KEY_PAGE_DOWN);
        return true;
    }
    if (toparse == "pause")
    {
        Keyboard.send(KEY_PAUSE);
        return true;
    }
    if (toparse == "scrl-lock")
    {
        Keyboard.send(KEY_SCROLL_LOCK);
        return true;
    }
    if (toparse == "prtscr")
    {
        Keyboard.send(KEY_PRINT_SCREEN);
        return true;
    }
    if (toparse == "capslock")
    {
        Keyboard.send(KEY_CAPS_LOCK);
        return true;
    }
    if (toparse == "lgui")
    {
        Keyboard.send(KEY_LEFT_GUI);
        return true;
    }
    if (toparse == "rgui")
    {
        Keyboard.send(KEY_RIGHT_GUI);
        return true;
    }
    if (toparse == "numlock")
    {
        Keyboard.send(KEY_NUM_LOCK);
        return true;
    }
    if (toparse == "fn")
    {
        return true;
    }
    if (ms.Match("f(%d*)"))
    {
        char buffer[4];
        ms.GetCapture(buffer, 0);
        Keyboard.send(KEY_F1 + std::stoi(buffer) - 1);
        return true;
    }
//    if (ms.Match("sleep (%d)"))
//    {
//        char buffer[5];
//        ms.GetCapture(buffer, 0);
//        // sleep(buffer);
//        return true;
//    }
    return false;
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

