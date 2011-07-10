#ifndef CONSOLE_H
#define CONSOLE_H

#include <QObject>
#include <QProcess>

class console : public QObject {
    Q_OBJECT
public:
    QProcess myproc;
    explicit console(QObject* parent = 0);

public slots:
    void startprocess();
};

#endif // CONSOLE_H
