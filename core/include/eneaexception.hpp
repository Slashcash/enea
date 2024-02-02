#ifndef ENEAEXCEPTION_HPP
#define ENEAEXCEPTION_HPP

#include <stdexcept>

class EneaException : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

#endif // ENEAEXCEPTION_HPP