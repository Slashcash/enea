#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <stdexcept>

/**
 * @brief This is the base class for our exceptions.
 * All our exceptions are supposed to inherit from this.
 *
 */
class Exception : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

#endif // EXCEPTION_HPP
