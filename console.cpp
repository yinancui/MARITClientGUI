#include "console.h"
#include <QString>
#include <QStringList>

console::console(QObject* parent) : QObject(parent)
{
}

void console::startprocess() {
    QStringList list;
    //list << "/home/marrk/Downloads/foo.txt";
    list << "";
    myproc.start("gnome-terminal", list);
}
