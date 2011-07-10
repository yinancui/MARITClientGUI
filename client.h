#ifndef CLIENT_H
#define CLIENT_H

#include <QDialog>
#include <QTcpSocket>
#include "console.h"
#include "ClientCodes.h"

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
class QTcpSocket;
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
    void displayError(QAbstractSocket::SocketError socketError);
    void enableconnectButton();

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

    QTcpSocket *tcpSocket;
    QString currentFortune;
    quint16 blockSize;

    QString data_show;

    // RTE SDK
    bool receive(QTcpSocket* pTcpSocket, char* pBuffer, int BufferSize);
    bool receive(QTcpSocket *pTcpSocket, long int & Val)
        {return receive(pTcpSocket, (char*)& Val, sizeof(Val));}
    bool receive(QTcpSocket *pTcpSocket, unsigned long int & Val)
        {return receive(pTcpSocket, (char*)& Val, sizeof(Val));}
    bool receive(QTcpSocket *pTcpSocket, double &Val)
        {return receive(pTcpSocket, (char*)& Val, sizeof(Val));}


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
