#include <QtGui/QApplication>
#include "client.h"
#include "console.h"
#include <QPushButton>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Client w;
//    QPushButton* showButton = new QPushButton("show");

//    console* myconsole = new console();
//    //a.connect(w.consoleButton, SIGNAL(clicked()), myconsole, SLOT(startprocess()));
//    a.connect(showButton, SIGNAL(clicked()), myconsole, SLOT(starprocess()));

//    showButton->show();
    w.show();

    return a.exec();
}
