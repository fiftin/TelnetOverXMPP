#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>
#include <QString>

class Exception : std::exception {
public:
    Exception(const QString& AMessage)
        : FMessage(AMessage)
    {
    }
    virtual ~Exception() throw() {
    }

    const QString& message() const {
        return FMessage;
    }

private:
    QString FMessage;
};


#endif // EXCEPTION_H
