#ifndef ERRORCODE_H
#define ERRORCODE_H

#include <QObject>

enum {
    NO_ERROR,
    CONNECTION_FAILURE,
    SOCKET_CREATION_FAILURE,
    SERVERINFO_FAILURE

};


class ErrorCode : public QObject
{
    Q_OBJECT
public:
    explicit ErrorCode(QObject *parent = 0) {m_value = NO_ERROR;}
    int value() const { return m_value; }
    void setValue(int value);// { m_value = value;}

signals:

public slots:

signals:
    void valueChanged(int newValue);


private:
    int m_value;


};

#endif // ERRORCODE_H
