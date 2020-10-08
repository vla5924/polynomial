#pragma once

#include "precompiled.h"
#include "listiterator.h"
#include "listexception.h"

template <typename Key, typename Data>
class List
{
public:
    class Node
    {
        template <typename, typename>
        friend class List;
        template <typename>
        friend class ListIterator;
        Node *pNext;

    public:
        Key key;
        Data data;

        explicit Node(Key key = 0, Data data = 0, Node *pNext = nullptr)
            : key(key), data(data), pNext(pNext)
        {
        }
        Node(const Node &other)
            : key(other.key), data(other.data), pNext(pNext)
        {
        }
    };

private:
    List::Node *pFirst;

public:
    List();
    List(const List &other);
    List(const List<Key, Data>::Node *firsNode);
    ~List();

    typedef ListIterator<List::Node> iterator;
    typedef ListIterator<List::Node> const_iterator;

    typename Node *find(Key needle);
    typename Node *getFirst();
    void insertToStart(Key key, Data data = 0);
    void insertToEnd(Key key, Data data = 0);
    void insertBefore(Key needle, Key key, Data data = 0);
    void insertBefore(iterator i, Key key, Data data = 0);
    void insertAfter(Key needle, Key key, Data data = 0);
    void insertAfter(iterator i, Key key, Data data = 0);
    void remove(Key needle);
    void remove(iterator &i);
    void removeAll();

    size_t size() const;
    bool empty() const;

    typename iterator begin();
    typename iterator end();
    typename const_iterator begin() const;
    typename const_iterator end() const;

    void output(std::ostream &stream, const char *separator = " ", const char *ending = "\n") const;
    void outputRaw(std::ostream &stream) const;
};

template <typename Key, typename Data>
List<Key, Data>::List()
{
    pFirst = nullptr;
}

template <typename Key, typename Data>
List<Key, Data>::List(const List &other) : List()
{
    if (!other.pFirst)
        return;
    pFirst = new Node(other.pFirst->key, other.pFirst->data);
    Node *temp = other.pFirst->pNext;
    Node *prev = pFirst;
    while (temp)
    {
        Node *pNode = new Node(temp->key, temp->data);
        prev->pNext = pNode;
        prev = pNode;
        temp = temp->pNext;
    }
}

template <typename Key, typename Data>
List<Key, Data>::List(const List<Key, Data>::Node *firsNode) : List()
{
    if (!firsNode)
        return;
    pFirst = new Node(firsNode->key, firsNode->data);
    Node *temp = firsNode->pNext;
    Node *prev = pFirst;
    while (temp)
    {
        Node *pNode = new Node(temp.key, temp.data);
        prev->pNext = pNode;
        prev = pNode;
        temp = temp->pNext;
    }
}

template <typename Key, typename Data>
List<Key, Data>::~List()
{
    removeAll();
}

template <typename Key, typename Data>
typename List<Key, Data>::Node *List<Key, Data>::find(Key needle)
{
    if (!pFirst)
        return nullptr;
    Node *temp = pFirst;
    while (temp)
    {
        if (temp->key == needle)
            return temp;
        temp = temp->pNext;
    }
    return nullptr;
}

template <typename Key, typename Data>
typename List<Key, Data>::Node *List<Key, Data>::getFirst()
{
    return pFirst;
}

template <typename Key, typename Data>
void List<Key, Data>::insertToStart(Key key, Data data)
{
    Node *pNode = new Node(key, data, pFirst);
    pFirst = pNode;
}

template <typename Key, typename Data>
void List<Key, Data>::insertToEnd(Key key, Data data)
{
    if (!pFirst)
    {
        pFirst = new Node(key, data);
        return;
    }
    Node *temp = pFirst;
    while (temp->pNext)
        temp = temp->pNext;
    temp->pNext = new Node(key, data, nullptr);
}

template <typename Key, typename Data>
void List<Key, Data>::insertBefore(Key needle, Key key, Data data)
{
    if (!pFirst)
        throw ListException::NodeNotFound();
    if (pFirst->key == needle)
    {
        Node *pNode = new Node(key, data, pFirst);
        pFirst = pNode;
        return;
    }
    Node *temp = pFirst;
    while (temp->pNext && (temp->pNext->key != needle))
        temp = temp->pNext;
    if (!temp->pNext)
        throw ListException::NodeNotFound();
    Node *pNode = new Node(key, data, temp->pNext);
    temp->pNext = pNode;
}

template <typename Key, typename Data>
void List<Key, Data>::insertBefore(iterator i, Key key, Data data)
{
    if (!pFirst)
        throw ListException::NodeNotFound();
    if (!i.pNode)
    {
        insertToEnd((*i).key, (*i).data);
        return;
    }
    Node *needle = i.pNode;
    if (pFirst == needle)
    {
        Node *pNode = new Node(key, data, pFirst);
        pFirst = pNode;
        return;
    }
    Node *temp = pFirst;
    while (temp->pNext && (temp->pNext != needle))
        temp = temp->pNext;
    if (!temp->pNext)
        throw ListException::NodeNotFound();
    Node *pNode = new Node(key, data, temp->pNext);
    temp->pNext = pNode;
}

template <typename Key, typename Data>
void List<Key, Data>::insertAfter(iterator i, Key key, Data data)
{
    if (!i.pNode)
        throw ListException::NodeNotFound();
    Node *pNode = new Node(key, data, i.pNode->pNext);
    i.pNode->pNext = pNode;
}

template <typename Key, typename Data>
void List<Key, Data>::insertAfter(Key needle, Key key, Data data)
{
    Node *prev = findNode(needle);
    if (!prev)
        throw ListException::NodeNotFound();
    Node *pNode = new Node(key, data, prev->pNext);
    prev->pNext = pNode;
}

template <typename Key, typename Data>
void List<Key, Data>::remove(Key needle)
{
    if (!pFirst)
        throw ListException::NodeNotFound();
    if (pFirst->key == needle)
    {
        Node *pNode = pFirst;
        pFirst = pFirst->pNext;
        delete pNode;
        return;
    }
    Node *temp = pFirst;
    while (temp->pNext && (temp->pNext->key != needle))
        temp = temp->pNext;
    if (!temp->pNext)
        throw ListException::NodeNotFound();
    Node *pNode = temp->pNext;
    temp->pNext = pNode->pNext;
    delete pNode;
}

template <typename Key, typename Data>
void List<Key, Data>::remove(iterator &i)
{
    if (!pFirst || !i.pNode)
        throw ListException::NodeNotFound();
    Node *needle = i.pNode;
    if (pFirst == needle)
    {
        Node *pNode = pFirst;
        pFirst = pFirst->pNext;
        delete pNode;
        i.pNode = nullptr;
        return;
    }
    Node *temp = pFirst;
    while (temp->pNext && (temp->pNext != needle))
        temp = temp->pNext;
    if (!temp->pNext)
        throw ListException::NodeNotFound();
    temp->pNext = needle->pNext;
    delete needle;
    i.pNode = nullptr;
}

template <typename Key, typename Data>
void List<Key, Data>::removeAll()
{
    if (!pFirst)
        return;
    Node *temp = pFirst;
    while (temp)
    {
        Node *pNode = temp;
        temp = temp->pNext;
        delete pNode;
    }
    pFirst = nullptr;
}

template <typename Key, typename Data>
size_t List<Key, Data>::size() const
{
    if (!pFirst)
        return 0;
    Node *temp = pFirst;
    size_t i = 0;
    while (temp)
    {
        i++;
        temp = temp->pNext;
    }
    return i;
}

template <typename Key, typename Data>
bool List<Key, Data>::empty() const
{
    return pFirst == nullptr;
}

template <typename Key, typename Data>
void List<Key, Data>::output(std::ostream &stream, const char *separator, const char *ending) const
{
    Node *temp = pFirst;
    while (temp)
    {
        if (temp->data)
            stream << *(temp->data);
        else
            stream << "nullptr";
        stream << separator;
        temp = temp->pNext;
    }
    stream << ending;
}

template <typename Key, typename Data>
void List<Key, Data>::outputRaw(std::ostream &stream) const
{
    Node *temp = pFirst;
    stream << "[\n";
    while (temp)
    {
        stream << "    {\n        " << temp->key << "\n        ";
        if (temp->data)
            stream << *(temp->data);
        else
            stream << "nullptr";
        stream << "\n    }\n";
        temp = temp->pNext;
    }
    stream << "]\n";
}

// ------------------------------------------------------------------
// ----------------------- ITERATION IN List -----------------------
// ------------------------------------------------------------------

template <typename Key, typename Data>
typename List<Key, Data>::iterator List<Key, Data>::begin()
{
    return iterator(pFirst);
}

template <typename Key, typename Data>
typename List<Key, Data>::iterator List<Key, Data>::end()
{
    return iterator(nullptr);
}

template <typename Key, typename Data>
typename List<Key, Data>::const_iterator List<Key, Data>::begin() const
{
    return iterator(pFirst);
}

template <typename Key, typename Data>
typename List<Key, Data>::const_iterator List<Key, Data>::end() const
{
    return iterator(nullptr);
}
