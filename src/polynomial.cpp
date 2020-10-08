#include "polynomial.h"

MonomialList::iterator Polynomial::findPrevOrderedDegree(unsigned degree) const
{
    if (!monomials->empty())
    {
        Monomial* first = *(monomials->begin());
        if (degree >= first->key)
            return monomials->end();
    }
    MonomialList::iterator i = monomials->begin();
    MonomialList::iterator temp = i;
    for (; getNextIterator(i) != monomials->end(); i++)
    {
        temp = i;
        MonomialList::iterator next = getNextIterator(temp);
        if ((*next)->key <= degree)
            return temp;
    }
    if (getNextIterator(temp) == monomials->end())
        return temp;
    else
        return i;
}

MonomialList::iterator Polynomial::getNextIterator(MonomialList::iterator iterator) const
{
    return ++iterator;
}

void Polynomial::add(double coefficient, unsigned degree)
{
    MonomialList::iterator prevOrdered = findPrevOrderedDegree(degree);
    if (prevOrdered != monomials->end())
        monomials->insertAfter(prevOrdered, degree, coefficient);
    else
        monomials->insertToStart(degree, coefficient);
}

void Polynomial::addNonzero(Monomial* primary, const Monomial* secondary)
{
	if (primary)
	{
        if(secondary)
		    *primary = *primary + *secondary;
		if (primary->data == 0)
			monomials->remove(primary->key);
		return;
	}
	if (secondary && (secondary->data != 0))
		add(secondary->data, secondary->key);
}

void Polynomial::reduce()
{
    for (MonomialList::iterator i = monomials->begin(); i != monomials->end(); i++)
    {
        Monomial* control = *i;
        if (control->data == 0)
        {
            monomials->remove(i);
            continue;
        }
        for (MonomialList::iterator j = getNextIterator(i); j != monomials->end(); j++)
        {
            Monomial* monomial = *j;
            if (monomial->key == control->key)
            {
                control->data += monomial->data;
                monomials->remove(j);
            }
        }
    }
}

void Polynomial::sort()
{
    if (monomials->empty() || monomials->begin()++ == monomials->end())
        return;
    for (MonomialList::iterator i = monomials->begin(); i != monomials->end(); i++)
    {
        Monomial* prev = *i;
        for (MonomialList::iterator j = monomials->begin()++; j != monomials->end(); j++)
        {
            Monomial* current = *j;
            if (prev->key > current->key)
            {
                std::swap(prev->key, current->key);
                std::swap(prev->data, current->data);
            }
        }
    }
}

void Polynomial::nullify()
{
    monomials->removeAll();
}

void Polynomial::parse(const char* const expression)
{
    std::string rawStr = expression;
    rawStr = rawStr.substr(rawStr.find_first_not_of(' '));
    if (rawStr.find_first_not_of("0123456789+-*xyz^. \t\n") < rawStr.size())
        throw SyntaxError();
    std::string str;
    for (char symbol : rawStr)
        if (symbol != ' ')
            str += symbol;
    rawStr.clear();
    do
    {
        size_t delta = 0U;
        size_t offset = str.find_first_of("0123456789.+-");
        size_t offset2 = str.find_first_of("+-", offset + 1);
        size_t offset3 = str.find_first_of("xyz");
        if ((offset >= str.size()) && (offset3 >= str.size()))
            throw SyntaxError();
        bool hasCoef = true;
        if (offset3 < offset)
        {
            offset = offset3;
            offset2 = str.find_first_of("+-", offset);
            hasCoef = false;
        }
        std::string strMonom;
        if (offset2 >= str.size())
            strMonom = str;
        else
            strMonom = str.substr(0, offset2);
        std::string strCoef;
        size_t strCoefSize = 0ULL;
        if (hasCoef)
        {
            strCoef = getFirstFloatNumber(strMonom.c_str(), offset);
            strCoefSize = strCoef.size();
            if (strCoef == "-")
            {
                strCoefSize = 1ULL;
                strCoef = "-1";
            }
            else if (strCoef == "+")
            {
                strCoefSize = 1ULL;
                strCoef = "1";
            }
        }
        else
        {
            strCoef = "1";
        }
        std::string remain = hasCoef ? strMonom.substr(strCoefSize + offset) : strMonom;
        size_t remainFirstNums = remain.find_first_of("0123456789");
        if (remainFirstNums < remain.find_first_of("xyz"))
            throw SyntaxError(); // like -123 45*x^2*...
        size_t remainVars;
        unsigned degree = 0U;
        while ((remainVars = remain.find_first_of("xyz")) < remain.size())
        {
            size_t remainVarsNext = remain.find_first_of("xyz", remainVars + 1U) - 1U;
            std::string strVarArea = remain.substr(remainVars, remainVarsNext > 0ULL ? remainVarsNext : 1ULL);
            unsigned factor;
            unsigned addingDegree = getDegreeMask(strVarArea.c_str(), factor);
            unsigned currentDegree = factor == 1U ? DEGZ(degree) : factor == 10U ? DEGY(degree) : DEGX(degree);
            if (currentDegree + addingDegree > 9U)
                throw SyntaxError();
            degree += addingDegree * factor;
            remain = remain.substr(remainVars + strVarArea.size());
        }
        double coefficient = 0.;
        try
        {
            coefficient = std::stod(strCoef);
        }
        catch (...)
        {
            throw SyntaxError();
        }
        add(coefficient, degree);
        str = str.substr(strMonom.size() + delta);
        if(str.size() > 0)
            str = str.substr(str.find_first_not_of(' '));
    } while (str.size() > 0);
    reduce();
}

const std::string Polynomial::monomToStr(const Monomial& monomial) const
{
    constexpr double EPSILON = 1e-4;
    std::string str;
    if (monomial.data == 0)
        return std::string("0");
    if (fabs(monomial.data - (int)monomial.data) < EPSILON)
        str += std::to_string((int)monomial.data);
    else
        str += std::to_string(monomial.data);
    if (monomial.key > 0U)
    {
        if (DEGX(monomial.key) > 0)
            str += "*x";
        if (DEGX(monomial.key) > 1)
        {
            str += '^';
            str += std::to_string(DEGX(monomial.key));
        }
        if (DEGY(monomial.key) > 0)
            str += "*y";
        if (DEGY(monomial.key) > 1)
        {
            str += '^';
            str += std::to_string(DEGY(monomial.key));
        }
        if (DEGZ(monomial.key) > 0)
            str += "*z";
        if (DEGZ(monomial.key) > 1)
        {
            str += '^';
            str += std::to_string(DEGZ(monomial.key));
        }
    }
    return str;
}

const std::string Polynomial::getFirstFloatNumber(const char* const expression, size_t& offset) const
{
    std::string str = expression;
    offset = str.find_first_of("0123456789.+-");
    return str.substr(offset, str.find_first_not_of("0123456789.+-", offset));
}

const std::string Polynomial::getFirstUIntNumber(const char* const expression, size_t& offset) const
{
    std::string str = expression;
    offset = str.find_first_of("0123456789+");
    size_t count = str.find_first_not_of("0123456789+", offset);
    if(count < str.size())
        return str.substr(offset, count);
    return str.substr(offset);
}

unsigned Polynomial::getDegreeMask(const char* const expression, unsigned& factor) const
{
    std::string str = expression;
    if (str.find_first_of("+-.") < str.size())
        return 1000U;
    size_t varsFound = str.find_first_of("xyz");
    size_t numsFound = str.find_first_of("0123456789");
    if ((numsFound < varsFound) || (varsFound >= str.size()))
        return 1000U;
    factor = 1U;
    if (str[varsFound] == 'x')
        factor = 100U;
    else if (str[varsFound] == 'y')
        factor = 10U;
    if (numsFound >= str.size())
        return 1U;
    size_t offset;
    unsigned degree = static_cast<unsigned>(std::stoul(getFirstUIntNumber(expression, offset)));
    return degree;
}

Polynomial::Polynomial()
{
    monomials = new MonomialList;
}

Polynomial::Polynomial(const char* const expression) : Polynomial()
{
    parse(expression);
}

Polynomial::Polynomial(const Monomial& monomial) : Polynomial()
{
    monomials->insertToStart(monomial.key, monomial.data);
}

Polynomial::Polynomial(double number)
    : Polynomial(Monomial(number, 0U))
{
}

Polynomial::Polynomial(const Polynomial& other)
{
    monomials = new MonomialList(*other.monomials);
}

Polynomial::Polynomial(const MonomialList& list)
{
    monomials = new MonomialList(list);
    sort();
    reduce();
}

Polynomial::~Polynomial()
{
    delete monomials;
}

Polynomial& Polynomial::operator=(const Polynomial& other)
{
    if (this == &other)
        return *this;
    delete monomials;
    monomials = new MonomialList(*other.monomials);
    return *this;
}

Polynomial Polynomial::operator+(const Polynomial& other)
{
    Polynomial result(*this);
    for (MonomialList::iterator i = other.monomials->begin(); i != other.monomials->end(); i++)
    {
        Monomial* current = *i;
        Monomial* monomial = result.monomials->find(current->key);
		result.addNonzero(monomial, current);
    }
    return result;
}

Polynomial Polynomial::operator+(const Monomial& monomial)
{
    Polynomial result(*this);
    Monomial* needle = result.monomials->find(monomial.key);
	result.addNonzero(needle, &monomial);
    return result;
}

Polynomial Polynomial::operator+(double number)
{
    if (number == 0.)
        return *this;
    return *this + Monomial(number, 0U);
}

Polynomial Polynomial::operator-(const Polynomial& other)
{
    Polynomial result(*this);
    for (MonomialList::iterator i = other.monomials->begin(); i != other.monomials->end(); i++)
    {
        Monomial* current = *i;
        Monomial* monomial = result.monomials->find(current->key);
		result.addNonzero(monomial, &-*current);
    }
    return result;
}

Polynomial Polynomial::operator-(const Monomial& monomial)
{
    Polynomial result(*this);
    Monomial* needle = result.monomials->find(monomial.key);
	result.addNonzero(needle, &-monomial);
    return result;
}

Polynomial Polynomial::operator-(double number)
{
    if (number == 0.)
        return *this;
    return *this - Monomial(number, 0U);
}

Polynomial Polynomial::operator*(const Polynomial& other)
{
    Polynomial result;
    for (MonomialList::iterator i = monomials->begin(); i != monomials->end(); i++)
    {
        for (MonomialList::iterator j = other.monomials->begin(); j != other.monomials->end(); j++)
        {
            Monomial* lhs = *i;
            Monomial* rhs = *j;
            Monomial monomial = *lhs * (*rhs);
            if(monomial.data)
                result.add(monomial.data, monomial.key);
        }
    }
    result.reduce();
    return result;
}

Polynomial Polynomial::operator*(const Monomial& monomial)
{
    if (monomial.data == 0.)
        return Polynomial();
    Polynomial result(*this);
    for (MonomialList::iterator i = monomials->begin(); i != monomials->end(); i++)
    {
        Monomial* current = *i;
        Monomial res = *current * monomial;
        if (res.data)
            result.add(res.data, res.key);
    }
    result.reduce();
    return result;
}

Polynomial Polynomial::operator*(double number)
{
    if (number == 0.)
        return Polynomial();
    Polynomial result(*this);
    for (MonomialList::iterator i = result.monomials->begin(); i != result.monomials->end(); i++)
    {
        Monomial* current = *i;
        current->data *= number;
    }
    return result;
}

Polynomial& Polynomial::operator+=(const Polynomial& other)
{
    *this = *this + other;
    return *this;
}

Polynomial& Polynomial::operator+=(const Monomial& monomial)
{
    *this = *this + monomial;
    return *this;
}

Polynomial& Polynomial::operator+=(double number)
{
    *this = *this + number;
    return *this;
}

Polynomial& Polynomial::operator-=(const Polynomial& other)
{
    *this = *this - other;
    return *this;
}

Polynomial& Polynomial::operator-=(const Monomial& monomial)
{
    *this = *this - monomial;
    return *this;
}

Polynomial& Polynomial::operator-=(double number)
{
    *this = *this - number;
    return *this;
}

Polynomial& Polynomial::operator*=(const Polynomial& other)
{
    *this = *this * other;
    return *this;
}

Polynomial& Polynomial::operator*=(const Monomial& monomial)
{
    *this = *this * monomial;
    return *this;
}

Polynomial& Polynomial::operator*=(double number)
{
    *this = *this * number;
    return *this;
}

std::ostream& operator<<(std::ostream& stream, const Polynomial& polynomial)
{
    if (polynomial.monomials->empty())
        return stream << "0";
    MonomialList::iterator i = polynomial.monomials->begin();
    stream << polynomial.monomToStr(Monomial((*i)->data, (*i)->key));
    for (i++; i != polynomial.monomials->end(); i++)
    {
        Monomial* monomial = *i;
        if (monomial->data >= 0)
            stream << " + ";
        else
            stream << " - ";
        stream << polynomial.monomToStr(Monomial(fabs(monomial->data), monomial->key));
    }
    return stream;
}

std::istream& operator>>(std::istream& stream, Polynomial& polynomial)
{
    std::string str;
    std::getline(stream, str, '\n');
    polynomial.parse(str.c_str());
    return stream;
}

Polynomial operator""_poly(const char* literal, size_t)
{
    return Polynomial(literal);
}
