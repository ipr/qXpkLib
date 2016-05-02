///////////////////////////////////
//
// arcexception.h
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#ifndef ARCEXCEPTION_H
#define ARCEXCEPTION_H

#include <string>
#include <exception>

// exception-classes for error cases
class IOException : public std::exception
{
protected:
    std::string m_message;

public:
    IOException(const char *message)
        : std::exception()
        , m_message(message)
    {
    }

    virtual const char* what() const noexcept
    {
        return m_message.c_str();
    }
};

class ArcException : public std::exception
{
protected:
    std::string m_message;
    std::string m_data;

public:
    ArcException(const char *message, const std::string &data)
        : std::exception()
        , m_message(message)
        , m_data(data)
    {
    }
    ArcException(const char *message, const size_t data)
        : std::exception()
        , m_message(message)
        , m_data()
    {
        // since C++11
        m_data = std::to_string(data);
    }

    virtual const char* what() const noexcept
    {
        return m_message.c_str();
    }

    std::string getData()
    {
        return m_data;
    }
};


#endif // ARCEXCEPTION_H

