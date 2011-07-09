#ifndef CLIENT_H
#define CLIENT_H

#include <QDialog>
#include <QTcpSocket>

namespace Ui {
    class Client;
}

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTcpSocket;
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
    void displayError(QAbstractSocket::SocketError socketError);
    void enableGetFortuneButton();

private:
    Ui::Client *ui;
    QLabel *hostLabel;
    QLabel *portLabel;
    QLineEdit *hostLineEdit;
    QLineEdit *portLineEdit;
    QLabel *statusLabel;
    QPushButton *getFortuneButton;
    QPushButton *quitButton;
    QDialogButtonBox *buttonBox;

    QTcpSocket *tcpSocket;
    QString currentFortune;
    quint16 blockSize;
};

#endif // CLIENT_H
