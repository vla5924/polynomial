#pragma once

#include "precompiled.h"

namespace ListException
{
    class NodeNotFound : public ::std::exception
    {
        const ::std::string whatStr = "Node with given key not found.";

    public:
        virtual const char *what() { return whatStr.c_str(); }
    };
} // namespace ListException
