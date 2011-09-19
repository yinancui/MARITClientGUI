#ifndef CLIENT_H
#define CLIENT_H

//------Qt includes--------
#include <QDialog>

//------POSIX socket includes--------
#include <limits>
#include <netdb.h>
#include <arpa/inet.h>

//--------local includes---------
//#include "ClientCodes.h"
#include "Data.h"
#include "errorcode.h"


// vtk includes ------------------------
//#include <QVTKWidget.h>
//#include <vtkSmartPointer.h>
//#include <vtkOBJReader.h>
//#include <vtkPolyDataMapper.h>
//#include <vtkActor.h>
//#include <vtkRenderer.h>
//#include <vtkRenderWindow.h>
//---------------------------------------

//------ not needed----
//#include <sys/socket.h>
//#include <cassert>
//#include <string>
//#include <vector>
//#include <algorithm>
//#include <functional>
//#include <math.h>
//#include <unistd.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <errno.h>
//#include <sys/types.h>
//#include <netinet/in.h>
//#include <sys/socket.h>


namespace Ui {
    class Client;
}

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QPushButton;
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
    void getFrame();
    void mainLoop();
    void displayError(int errorCode);
    void enableconnectButton();
    void getFrameAndHover();

private:
    Ui::Client *ui;
    QLabel *hostLabel;
    QLabel *portLabel;
    QLineEdit *hostLineEdit;
    QLineEdit *portLineEdit;
    QLabel *statusLabel;
    //QTextEdit* textEdit;
    //QTextBrowser* textBrowser;
    QPushButton *connectButton;
    QPushButton *quitButton;
    QPushButton* testHoverButton;
    //QDialogButtonBox *buttonBox;

    //------------- Unix socket-----------------------
    int sockfd, numbytes;
    struct addrinfo hints, *servinfo, *pAddrinfo;
    char sIPv6[INET6_ADDRSTRLEN];
    bool receive(int Socket, char* pBuffer, int BufferSize);
    bool receive(int Socket, long int & Val);
    bool receive(int Socket, unsigned long int & Val);
    bool receive(int Socket, double &Val);
    //---------- connect to server routine
    int connectServer();
    void* get_in_addr(struct sockaddr* sa);
    //--------error handling-----------
    ErrorCode errorCode;

    //------------- controllers --------------------
    void hoverAtt(float fRollRef, float fPitchRef,
                  float fYawRef, float fZRef);
    void hoverPos();



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
