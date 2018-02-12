#ifndef _RESULT_HPP_
#define _RESULT_HPP_

#include <string>

/*A class which represents a result for an operation in this software. A result consists of an error number and
an eventual description for the error. Please note that the special value of 0 stands for a succesful result*/

class Result {
    private:
        //members
        unsigned int error_number; //the error number for this operation (0 means SUCCESS)
        std::string description; //a string description for this operation

    public:
        //const
        static const unsigned int SUCCESS = 0; //as said 0 means a succesful operation

        //ctor
        Result(const unsigned int theErrorNumber = SUCCESS, const std::string& theDescription = ""); //default constructor more or less

        //getters
        unsigned int getErrorNumber() const { return error_number; }
        std::string getDescription() const { return description; }
        bool isSuccesful() const { return error_number == SUCCESS; }

        //setters
        void setErrorNumber(const unsigned int theErrorNumber) { error_number = theErrorNumber; }
        void setDescription(const std::string& theDescription) { description = theDescription; }

        //operator
        operator bool() const { return isSuccesful(); } //a safe cast to bool (with this we can use a result in an IF condition)
};

#endif // _RESULT_HPP_


