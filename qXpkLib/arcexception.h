///////////////////////////////////
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
    IOException(const char *szMessage)
        : std::exception()
        , m_message(szMessage)
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
    ArcException(const char *szMessage, const std::string &szData)
        : std::exception()
        , m_message(szMessage)
        , m_data(szData)
    {
    }
    ArcException(const char *szMessage, const size_t nData)
        : std::exception()
        , m_message(szMessage)
        , m_data()
    {
        // TODO:
        //m_szData = std::lexical_cast<std::string>(nData);
    }

    virtual const char* what() const noexcept
    {
        return m_message.c_str();
    }

    std::string GetData()
    {
        return m_data;
    }
};


#endif // ARCEXCEPTION_H

