#pragma once

#include "precompiled.h"

template <typename NodeTypename>
class ListIterator : public std::iterator<std::input_iterator_tag, NodeTypename>
{
    template <typename, typename> friend class List;
    NodeTypename *pNode;
    ListIterator(NodeTypename *pNode);

public:
    ListIterator(const ListIterator &other) = default;
    ~ListIterator() = default;
    bool operator!=(ListIterator const &other) const;
    bool operator==(ListIterator const &other) const;
    typename NodeTypename *operator*() const;
    ListIterator operator++();
    ListIterator operator++(int);
};

template <typename NodeTypename>
ListIterator<NodeTypename>::ListIterator(NodeTypename *pNode) : pNode(pNode) {}

template <typename NodeTypename>
bool ListIterator<NodeTypename>::operator!=(ListIterator const &other) const
{
    return pNode != other.pNode;
}

template <typename NodeTypename>
bool ListIterator<NodeTypename>::operator==(ListIterator const &other) const
{
    return pNode == other.pNode;
}

template <typename NodeTypename>
typename NodeTypename *ListIterator<NodeTypename>::operator*() const
{
    return pNode;
}

template <typename NodeTypename>
ListIterator<NodeTypename> ListIterator<NodeTypename>::operator++()
{
    if (pNode)
        pNode = pNode->pNext;
    return *this;
}

template <typename NodeTypename>
ListIterator<NodeTypename> ListIterator<NodeTypename>::operator++(int)
{
    NodeTypename *temp = pNode;
    if (pNode)
        pNode = pNode->pNext;
    return ListIterator<NodeTypename>(temp);
}
