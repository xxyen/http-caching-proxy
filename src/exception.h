#include <exception>
#include <string>

class MyException : public std::exception {
    private:
        std::string message;
    public:
        MyException(std::string message): message(message){}
        MyException(const char* message): message(message){}
        virtual const char* what() const noexcept override{
            return message.c_str();
        }
    };