#pragma once

#include <Arduino.h>
#include <string>


struct Node
{
    String key;
    String value;
    Node * next;
    Node * prev;
};

class KeyValue 
{
protected:
    Node * _head;
    Node * getNode(int8_t);
public:
    KeyValue();
    int8_t count();
    void put(String, String);
    void put(const char *, const char *);
    String get(String);
    String get(const char *);
    int8_t getIndexOf(const char *);
    String key(uint8_t);
    String value(uint8_t);
    void remove(const char *);
    void remove(String &);
    void removeIndex(int8_t);
    void clear();
    void load();
    void save();
};
