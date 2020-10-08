#pragma once

#include "precompiled.h"
#include "list.h"

#define DEGX(A) ((A) / 100U)
#define DEGY(A) ((A) / 10U % 10U)
#define DEGZ(A) ((A) % 10U)
#define MAKEDEG(X, Y, Z) ((X)*100U + (Y)*10U + (Z))

typedef List<unsigned, double> MonomialList;
typedef MonomialList::Node Monomial;

template <>
class List<unsigned, double>::Node
{
public:
    class DegreeOverflow : std::exception
    {
        const std::string whatStr = "Degree cannot be greater than 9.";

    public:
        virtual const char *what() { return whatStr.c_str(); }
    };

    class DegreeUnequality : std::exception
    {
        const std::string whatStr = "Degrees are not equal.";

    public:
        virtual const char *what() { return whatStr.c_str(); }
    };

private:
    template <typename, typename> friend class List;
    template <typename> friend class ListIterator;
    friend class Polynomial;

    friend std::ostream &operator<<(std::ostream &stream, const Polynomial &polynomial);
    
    unsigned key;
    double data;
    Node *pNext;

    bool checkDegrees(unsigned degrees) const
    {
        return degrees <= 999U;
    }

    bool checkDegrees(unsigned x, unsigned y, unsigned z) const
    {
        return (x <= 9) && (y <= 9) && (z <= 9);
    }

public:
    explicit Node(unsigned degree = 0U, double coefficient = 0., Node *pNext_ = nullptr)
    {
        if (degree > 999U)
            throw DegreeOverflow();
        key = degree;
        data = coefficient;
        pNext = pNext_;
    }
    Node(const Node &) = default;
    Node(double coefficient, unsigned degree) : Node(degree, coefficient){};

    Node operator+(const Node &other) const
    {
        if (key != other.key)
            throw DegreeUnequality();
        double coefficient = data + other.data;
        return Monomial(key, coefficient, pNext);
    }

    Node operator-(const Node &other) const
    {
        if (key != other.key)
            throw DegreeUnequality();
        double coefficient = data - other.data;
        return Monomial(key, coefficient, pNext);
    }

    Node operator*(const Node &other) const
    {
        if (!checkDegrees(key + other.key))
            throw DegreeOverflow();
        unsigned x = DEGX(key) + DEGX(other.key);
        unsigned y = DEGY(key) + DEGY(other.key);
        unsigned z = DEGZ(key) + DEGZ(other.key);
        if (!checkDegrees(x, y, z))
            throw DegreeOverflow();
        unsigned degree = MAKEDEG(x, y, z);
        double coefficient = data * other.data;
        return Monomial(degree, coefficient, pNext);
    }

    Node operator*(double number) const
    {
        return Monomial(key, data * number, pNext);
    }

    Node operator+() const
    {
        return *this;
    }

    Node operator-() const
    {
        return Node(key, -data, pNext);
    }
};
