#include <FS.h>
#include <LittleFS.h>

#include "keyvalue.h"
#define KV_FILENAME "kv.txt"


KeyValue::KeyValue() : head(nullptr),
    iter(nullptr)
{
    LittleFS.begin();
}

void KeyValue::insert(String & key, String & value)
{
    KeyValuePair pair = {key, value};
    Node* newNode = new Node(pair);
    newNode->next = head;
    head = newNode;
}

void KeyValue::put(String & key, String & value)
{
    Serial.printf("KeyValue::put [%s] [%s]\r\n", key.c_str(), value.c_str());
    Node* current = head;
    while (current != nullptr)
    {
        if (current->data.key == key)
        {
            current->data.value = value;
            return;
        }
        current = current->next;
    }
    if (current == nullptr)
    {
        insert(key, value);
    }
}

String KeyValue::get(String & key) 
{
    Node* current = head;
    while (current != nullptr) {
        if (current->data.key == key) {
            return current->data.value;
        }
        current = current->next;
    }
    return ""; // Key not found
}

bool KeyValue::first(String & key, String & value)
{
    iter = head;
    return next(key, value);
}

bool KeyValue::next(String & key, String & value)
{
    if (nullptr == iter)
    {
         return false;
    }
    key = iter->data.key;
    value = iter->data.value;
    iter = iter->next;
    return true;
}

void KeyValue::load()
{
    LittleFS.begin();
    File file = LittleFS.open(KV_FILENAME, "r");
    if (!file)
    {
        Serial.print("Error opening file: ");
        return;
    }
    clear();

    while (file.available()) {
        String line = file.readStringUntil('\n');
        int eqPos = line.indexOf('=');
        if (eqPos != -1) {
            String key = line.substring(0, eqPos);
            String value = line.substring(eqPos + 1);
            insert(key, value);
        }
    }
    file.close();
}

void KeyValue::save()
{
    File file = LittleFS.open(KV_FILENAME, "w");
    if (!file)
    {
        Serial.print("Error opening file: ");
        return;
    }
    Node * node = head;
    for (; node != nullptr; node = node->next)
    {
        String buffer = node->data.key + " = " + node->data.value + "\r\n";
        file.write(buffer.c_str(), buffer.length());
    }
    file.close();
}

void KeyValue::format()
{
    LittleFS.format();

}

void KeyValue::clear()
{
    while (head != nullptr)
    {
        remove(0);
    }
}

void KeyValue::remove(String &key)
{
    byte index = 0;
    Node* current = head;
    for (current = head; current != nullptr; current = current->next, index++)
    {
        if (current->data.key == key)
        {
            remove(index);
            return;
        }
    }
}

void KeyValue::remove(byte index)
{
    byte count;
    Node * iter = head;
    Node * next = head;

    Serial.printf("remove index %d\r\n", index);
    Serial.printf("head %08x iter %08x next %08x\r\n", head, iter, next);

    if (nullptr == iter)
    {
        return;
    }

    next = next->next;

    for (count = 0; count < index, next != nullptr; count++)
    {
        iter = next;
        next = next->next;
        Serial.printf("iter %08x next %08x\r\n", iter, next);
    }

    Serial.printf("head %08x iter %08x next %08x\r\n", head, iter, next);

    if (head == iter)
    {
        delete head;
        head = next;
        return;
    }
    iter->next = next->next;
    delete next;
    Serial.println("remove done");
}
