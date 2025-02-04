#include <FS.h>
#include <LittleFS.h>

#include "keyvalue.h"
#define KV_FILENAME "kv.json"

KeyValue::KeyValue() : _head(nullptr), empty(""), _count(0)
{
}


int8_t KeyValue::count()
{
    return _count;
}

void KeyValue::put(String key, String value)
{
    return put(key.c_str(), value.c_str());
}

void KeyValue::put(const char * key, const char * value)
{
    Node * iter = _head;
    Node * prev = _head;

    if (nullptr == _head)
    {
        _head = new Node{String(key), String(value), nullptr, prev};
        _count++;
        return;
    }
    for (; iter != nullptr; prev = iter, iter = iter->next)
    {
        if (iter->key == key)
        {
            iter->value = value;
            return;
        }
    }
    if (iter == nullptr)
    {
        prev->next = new Node{String(key), String(value), nullptr, prev};
        _count++;
    }
}


String KeyValue::get(String key)
{
    return get(key.c_str());
}

String KeyValue::get(const char * key)
{
    Node * iter = _head;
    for (; iter != nullptr; iter = iter->next)
    {
        if (iter->key == key)
        {
            return iter->value;
        }
    }
    return "";
}

int8_t KeyValue::getIndexOf(const char * key)
{
    int8_t count = 0;
    Node * iter = _head;
    for (; iter != nullptr; count++, iter = iter->next)
    {
        if (iter->key == key)
        {
            return count;
        }
    }
    return -1;
}

Node * KeyValue::getNode(int8_t index)
{
    Node * iter = _head;
//    Serial.printf("getNode index %d\r\n", index);
    for (; (iter != nullptr) && index; iter=iter->next, index--)
        ;
    if (nullptr != iter)
    {
//        Serial.printf("matched, returning %08x\r\n", iter);
        return iter;
    }
//    Serial.println("getNode no match, returning null");
    return nullptr;
}

void KeyValue::remove(String & key)
{
    return remove(key.c_str());
}

void KeyValue::remove(const char * key)
{
    removeIndex(getIndexOf(key));
}

void KeyValue::removeIndex(int8_t index)
{
    Node * iter = _head;
    if (nullptr == _head)
    {
        return;
    }
    if (0 == index)
    {
        Node * tmp(_head);
        _head = _head->next;
        delete tmp;
        _count--;
        return;
    }
    for (; (iter != nullptr) && index; iter=iter->next, index--)
        ;
    if (nullptr != iter)
    {
        if (nullptr != iter->next)
        {
            iter->next->prev = iter->prev;
        }
        iter->prev->next = iter->next;
        delete iter;
        _count--;
    }
}

void KeyValue::clear()
{
    while (_count)
    {
        removeIndex(0);
    }
}

String & KeyValue::key(uint8_t index)
{
    Node * node = getNode(index);
    if (nullptr != node)
    {
        return node->key;
    }
    return empty;
}

String & KeyValue::value(uint8_t index)
{
    Node * node = getNode(index);
    if (nullptr != node)
    {
        return node->value;
    }
    return empty;
}

void KeyValue::load()
{
    clear();
    LittleFS.begin();
    File file = LittleFS.open(KV_FILENAME, "r");
    if (!file)
    {
        Serial.print("Error opening file: ");
        return;
    }
    while (file.available())
    {
        String line = file.readStringUntil('\n');
        String delimiter(" = ");
        String key(line.substring(0, line.indexOf(delimiter)));
        String value(line.substring(line.indexOf(delimiter) + delimiter.length()));

        key.replace("==", "=");
        value.replace("==", "=");
        key.trim();
        value.trim();
        put(key, value);
    }
    file.close();
}

void KeyValue::save()
{
    LittleFS.begin();
    File file = LittleFS.open(KV_FILENAME, "w");
    if (!file)
    {
        Serial.print("Error opening file: ");
        return;
    }
    int8_t ii;
    for (ii = 0; ii < count(); ii++)
    {
        Node * node(getNode(ii));
        String key(node->key);
        String value(node->value);
        key.replace("=", "==");
        value.replace("=", "==");
        
        String line(key + " = " + value + "\r\n");
        file.write(line.c_str(), line.length());
    }
    file.close();
}

void KeyValue::dump()
{
    Node * iter = _head;
    while (nullptr != iter)
    {
        Serial.printf("node %08x prev %08x next %08x key %s value %s\r\n",
                iter, iter->prev, iter->next, iter->key.c_str(), iter->value.c_str());
        iter = iter->next;
    }
    Serial.println("Done");
}

