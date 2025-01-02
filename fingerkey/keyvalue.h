#pragma once

#include <Arduino.h>

// Custom Key-Value Pair Structure
struct KeyValuePair {
  String key;
  String value;
};

class KeyValue
{
private:
  struct Node {
    KeyValuePair data;
    Node* next;
    Node(KeyValuePair data) : data(data), next(nullptr) {}
  };
  Node* head;
  Node* iter;

public:
  KeyValue();

  void insert(String &key, String &value);
  void put(String &key, String &value);
  String get(String &key);
  bool first(String & , String &);
  bool next(String & , String &);
  void load();
  void save();
  void format();
  void clear();
  void remove(String &key);
  void remove(byte index);
};
