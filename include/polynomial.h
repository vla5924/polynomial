#pragma once

#include "precompiled.h"
#include "list.h"
#include "monomial.h"

class Polynomial
{
    List<unsigned, double> *monomials;

    MonomialList::iterator findPrevOrderedDegree(unsigned degree) const;
    MonomialList::iterator getNextIterator(MonomialList::iterator iterator) const;
    void add(double coefficient, unsigned degree);
    void addNonzero(Monomial *primary, const Monomial *secondary);
    void reduce();
    void sort();
    void nullify();
    void parse(const char *const expression);
    const std::string monomToStr(const Monomial &monomial) const;
    const std::string getFirstFloatNumber(const char *const expression, size_t &offset) const;
    const std::string getFirstUIntNumber(const char *const expression, size_t &offset) const;
    unsigned getDegreeMask(const char *const expression, unsigned &factor) const;

public:
    Polynomial();
    Polynomial(const char *const expression);
    Polynomial(const Monomial &monomial);
    Polynomial(double number);
    Polynomial(const Polynomial &other);
    Polynomial(const MonomialList &list);
    ~Polynomial();
    Polynomial &operator=(const Polynomial &other);
    Polynomial operator+(const Polynomial &other);
    Polynomial operator+(const Monomial &monomial);
    Polynomial operator+(double number);
    Polynomial operator-(const Polynomial &other);
    Polynomial operator-(const Monomial &monomial);
    Polynomial operator-(double number);
    Polynomial operator*(const Polynomial &other);
    Polynomial operator*(const Monomial &monomial);
    Polynomial operator*(double number);
    Polynomial &operator+=(const Polynomial &other);
    Polynomial &operator+=(const Monomial &monomial);
    Polynomial &operator+=(double number);
    Polynomial &operator-=(const Polynomial &other);
    Polynomial &operator-=(const Monomial &monomial);
    Polynomial &operator-=(double number);
    Polynomial &operator*=(const Polynomial &other);
    Polynomial &operator*=(const Monomial &monomial);
    Polynomial &operator*=(double number);
    friend std::ostream &operator<<(std::ostream &stream, const Polynomial &polynomial);
    friend std::istream &operator>>(std::istream &stream, Polynomial &polynomial);
    class SyntaxError : public std::exception
    {
        const std::string whatStr = "Serialization cannot be parsed.";

    public:
        virtual const char *what() { return whatStr.c_str(); }
    };
};

Polynomial operator""_poly(const char *literal, size_t);
