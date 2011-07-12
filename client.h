#ifndef CLIENT_H
#define CLIENT_H

#include <QDialog>
//#include <QTcpSocket>
#include "console.h"
#include "ClientCodes.h"

//------ includes from Unix socket
#include <sys/socket.h>
#include <cassert>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <limits>
#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>




// vtk includes ------------------------
//#include <QVTKWidget.h>
//#include <vtkSmartPointer.h>
//#include <vtkOBJReader.h>
//#include <vtkPolyDataMapper.h>
//#include <vtkActor.h>
//#include <vtkRenderer.h>
//#include <vtkRenderWindow.h>
//---------------------------------------

namespace Ui {
    class Client;
}

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QPushButton;
//class QTcpSocket;
class QTextEdit;
class QTextBrowser;
class QProcess;
QT_END_NAMESPACE


class Client : public QDialog
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = 0);
    ~Client();

private slots:
    void requestNewFortune();
    void readFortune();
    //int readFortune();
    //void displayError(QAbstractSocket::SocketError socketError);
    void enableconnectButton();

    void dummy();

private:
    Ui::Client *ui;
    QLabel *hostLabel;
    QLabel *portLabel;
    QLineEdit *hostLineEdit;
    QLineEdit *portLineEdit;
    QLabel *statusLabel;
    QTextEdit* textEdit;
    QTextBrowser* textBrowser;
    QPushButton *connectButton;

    QPushButton *quitButton;
    QDialogButtonBox *buttonBox;

    //QTcpSocket *tcpSocket;

    //QString currentFortune;
    //quint16 blockSize;

    //QString data_show;


    //------------- Unix socket
    int sockfd, numbytes;
    //char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *pAddrinfo;
    char sIPv6[INET6_ADDRSTRLEN];

    bool receive(int Socket, char* pBuffer, int BufferSize);
    bool receive(int Socket, long int & Val);
    bool receive(int Socket, unsigned long int & Val);
    bool receive(int Socket, double &Val);

    //---------- connect to server routine
    int connectServer();
    void* get_in_addr(struct sockaddr* sa);



    //console* myconsole;



//    // vtk -------------------------------
//    QVTKWidget* vtkWidget;
//    vtkSmartPointer<vtkOBJReader> reader;
//    vtkSmartPointer<vtkPolyDataMapper> mapper;
//    vtkSmartPointer<vtkActor> actor;
//    vtkSmartPointer<vtkRenderer> renderer;
    //QProcess* proc;
};

#endif // CLIENT_H
