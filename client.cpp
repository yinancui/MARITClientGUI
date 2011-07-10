#include "client.h"
#include "ui_client.h"
#include <QtGui>
#include <QtNetwork>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include "ClientCodes.h"
#include <string>
#include <vector>
#include <algorithm>


Client::Client(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Client)
{
    ui->setupUi(this);

    hostLabel = new QLabel(tr("&Server name:"));
    portLabel = new QLabel(tr("S&erver port:"));

    hostLineEdit = new QLineEdit("192.168.15.100");
    portLineEdit = new QLineEdit("800");
    portLineEdit->setValidator(new QIntValidator(1, 65535, this));

    hostLabel->setBuddy(hostLineEdit);
    portLabel->setBuddy(portLineEdit);

    statusLabel = new QLabel(tr("This examples requires that you run the "
                                "Fortune Server example as well."));
    this->textEdit = new QTextEdit(tr("switched to textEdit."));
    this->textEdit->setReadOnly(true);
    this->textBrowser = new QTextBrowser; //("switched to textBrowser.");


    connectButton = new QPushButton(tr("Connect"));
    connectButton->setDefault(true);
    //connectButton->setEnabled(false);
    connectButton->setEnabled(true);

    quitButton = new QPushButton(tr("Quit"));

    buttonBox = new QDialogButtonBox;
    buttonBox->addButton(connectButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);


    tcpSocket = new QTcpSocket(this);

 //   myconsole = new console;

//    // vtk ----------------------------
//    QString filename = "/home/marrk/coding/qtproj/qtcp/ClientGUI/cow.obj";
//    vtkWidget = new QVTKWidget(this, QFlag(0));
//    //-------------------------------------

    connect(hostLineEdit, SIGNAL(textChanged(const QString &)),this, SLOT(enableconnectButton()));
    connect(portLineEdit, SIGNAL(textChanged(const QString &)),this, SLOT(enableconnectButton()));
    connect(connectButton, SIGNAL(clicked()),this, SLOT(requestNewFortune()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readFortune()));
    //connect(tcpSocket, SIGNAL(connected()), this, SLOT(readFortune()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError(QAbstractSocket::SocketError)));
    //connect(this->connectButton, SIGNAL(clicked()), myconsole, SLOT(startprocess()));
    //connect(tcpSocket, SIGNAL(readyRead()), myconsole, SLOT(startprocess()));


    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(hostLabel, 0, 0);
    mainLayout->addWidget(hostLineEdit, 0, 1);
    mainLayout->addWidget(portLabel, 1, 0);
    mainLayout->addWidget(portLineEdit, 1, 1);
    //mainLayout->addWidget(statusLabel, 2, 0, 1, 2);
    //mainLayout->addWidget(this->textEdit, 2, 0, 1, 2);
    //mainLayout->addWidget(this->textBrowser, 2, 0, 1, 2);
    //mainLayout->addWidget(this->vtkWidget, 2, 0, 1, 2);
    mainLayout->addWidget(buttonBox, 3, 0, 1, 2);
    setLayout(mainLayout);


    //    // vtk render ----------------------------------------
    //    reader = vtkSmartPointer<vtkOBJReader>::New();
    //    reader->SetFileName(filename.toStdString().c_str());
    //    reader->Update();

    //    mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    //    mapper->SetInputConnection(reader->GetOutputPort());



    //    actor = vtkSmartPointer<vtkActor>::New();
    //    actor->SetMapper(mapper);


    //    renderer = vtkSmartPointer<vtkRenderer>::New();
    //    //renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    //    //renderWindow->AddRenderer(renderer);

    //    vtkWidget->GetRenderWindow()->AddRenderer(renderer);
    //    renderer->AddActor(actor);
    //    renderer->ResetCamera();
    //    renderer->SetBackground(0, 0, 0);
    //    //renderWindow->SetSize(640, 480);
    //    //renderWindow->Render();
    //    //vtkWidget->setBaseSize(800, 600);
    //    renderer->Render();
    //    //-------------------------------------------------


    setWindowTitle(tr("Fortune Client"));
    portLineEdit->setFocus();
    std::cout << "outof ctor.\n";
}

Client::~Client()
{
    delete ui;
}


void Client::requestNewFortune()
{
    std::cout << "sending request...\n";
    connectButton->setEnabled(false);
    blockSize = 0;
    tcpSocket->abort();
    tcpSocket->connectToHost(hostLineEdit->text(),
                             portLineEdit->text().toInt());
    std::cout << "after connect.\n";
}
void Client::readFortune()
{
//    char buf[512];
//    char buf_in[512];
//    memset(buf, 0, 512);
//    memset(buf_in, 0, 512);


    //Client::receive(tcpSocket, buf, 512);

    std::cout << "Now begin receiving...\n";

    try {

        std::vector<std::string> info;
        const int bufferSize = 2040;
        char buff[bufferSize];
        char* pBuff;

        // Get info
        // request channel info
        pBuff = buff;
        // save EInfo into buff thru pBuff
        *((long int*) pBuff) = ClientCodes::EInfo;
        // and move pointer to next available position in buff
        pBuff += sizeof(long int);

        *((long int*) pBuff) = ClientCodes::ERequest;
        pBuff += sizeof(long int);

        // check packet header
        // pBuff - buff is an int equal to 2*sizeof(long int) in this case
        // send request to server
        // qint64 QIODevice::write ( const char * data, qint64 maxSize )
        //if (send(tcpSocket, buff, pBuff - buff, 0) == -1)
        if (tcpSocket->write(buff, pBuff - buff) == -1)
            throw std::string("Error Requesting");

        long int packet;
        long int type;

        // recv and pass data to packet
        if (!receive(tcpSocket, packet))
            throw std::string("Error receiving.\n");
        //recv and pass data to type
        if (!receive(tcpSocket, type))
            throw std::string("Bad receiving.\n");
        if (type != ClientCodes::EReply)
            throw std::string("Bad reply type.\n");
        if (packet != ClientCodes::EInfo)
            throw std::string("Bad packet.\n");

        long int size;

        // recv and pass data to size, thus get the size of future data?
        if (!receive(tcpSocket, size))
            throw std::string("Wrong size.\n");   // ????

        info.resize(size);

        std::vector<std::string>::iterator iInfo;
        //info.
        for (iInfo = info.begin(); iInfo != info.end(); iInfo++) {
            long int s;
            char c[255];
            char* p = c;

            if (!receive(tcpSocket, s))
                throw std::string();
            if (!receive(tcpSocket, c, s))
                throw std::string();
            p += s;
            *p = 0;
            *iInfo = std::string(c);
        }

        //----------------------------
        //--------- Parse info --------
        // the info packets contain channel names.
        // identify the channels with DOFs?
        std::vector<MarkerChannel> MarkerChannels;
        std::vector<BodyChannel> BodyChannels;
        int FrameChannel;

        for (iInfo = info.begin(); iInfo != info.end(); iInfo++) {
            // Extract the channel type
            // looking for the FIRST '<' in each info string

            int openBrace = iInfo->find('<');
            // if no '<' found
            if (openBrace == iInfo->npos)
                throw std::string("Bad channel ID");

            int closeBrace = iInfo->find('>');
            if (closeBrace == iInfo->npos)
                throw std::string("Bad channel ID");

            closeBrace++;
            // Type is given the content within the first '<>' of each info
            // which is the channel type
            std::string Type = iInfo->substr(openBrace, closeBrace - openBrace);

            // Extract the Name
            // the Name is the substring from the begin to the first '<'
            std::string Name = iInfo->substr(0, openBrace);
            // rfind return the LAST occurrence of the specified char
            int space = Name.rfind(' ');
            if (space != Name.npos)
                Name.resize(space);

            std::vector<MarkerChannel>::iterator iMarker;
            std::vector<BodyChannel>::iterator iBody;
            std::vector<std::string>::const_iterator iTypes;

            // need to #include <algorithm> to use this version of find
            iMarker = std::find(MarkerChannels.begin(), MarkerChannels.end(), Name);
            iBody = std::find(BodyChannels.begin(),BodyChannels.end(),Name);

            if(iMarker != MarkerChannels.end())
            {
                //  The channel is for a marker we already have.
                iTypes = std::find( ClientCodes::MarkerTokens.begin(), ClientCodes::MarkerTokens.end(), Type);
                if(iTypes != ClientCodes::MarkerTokens.end())
                    iMarker->operator[](iTypes - ClientCodes::MarkerTokens.begin()) = iInfo - info.begin();
            }
            else
                if(iBody != BodyChannels.end())
                {
                    //  The channel is for a body we already have.
                    iTypes = std::find(ClientCodes::BodyTokens.begin(), ClientCodes::BodyTokens.end(), Type);
                    if(iTypes != ClientCodes::BodyTokens.end())
                        iBody->operator[](iTypes - ClientCodes::BodyTokens.begin()) = iInfo - info.begin();
                }
                else
                    if((iTypes = std::find(ClientCodes::MarkerTokens.begin(), ClientCodes::MarkerTokens.end(), Type))
                            != ClientCodes::MarkerTokens.end())
                    {
                        //  Its a new marker.
                        MarkerChannels.push_back(MarkerChannel(Name));
                        MarkerChannels.back()[iTypes - ClientCodes::MarkerTokens.begin()] = iInfo - info.begin();
                    }

                    else
                        if((iTypes = std::find(ClientCodes::BodyTokens.begin(), ClientCodes::BodyTokens.end(), Type))
                                != ClientCodes::BodyTokens.end())
                        {
                            //  Its a new body.
                            BodyChannels.push_back(BodyChannel(Name));
                            BodyChannels.back()[iTypes - ClientCodes::BodyTokens.begin()] = iInfo - info.begin();
                        }
                        else
                            if(Type == "<F>")
                            {
                                FrameChannel = iInfo - info.begin();
                            }
                            else
                            {
                                //  It could be a new channel type.
                            }

        }

        //------ Up to now all markersnames are in MarkerChannels
        //------ all bodynames are in BodyChannels
        //----------- Get Data
        // get the data using request/reply protocol.

        int i;
        std::vector<double> data;
        data.resize(info.size());
        double timestamp;

        std::vector<MarkerData> markerPositions;
        markerPositions.resize(MarkerChannels.size());

        std::vector<BodyData> bodyPositions;
        bodyPositions.resize(BodyChannels.size());


        // set loop count
        for (i = 0; i < 10; i++) {

            // use the same routine as when getting channel info

            pBuff = buff;

            * ((long int *) pBuff) = ClientCodes::EData;
            pBuff += sizeof(long int);
            * ((long int *) pBuff) = ClientCodes::ERequest;
            pBuff += sizeof(long int);

           // if(send(tcpSocket, buff, pBuff - buff, 0) == -1)
            // qint64 QIODevice::write ( const char * data, qint64 maxSize )
            //if (send(tcpSocket, buff, pBuff - buff, 0) == -1)
            if (tcpSocket->write(buff, pBuff - buff) == -1)
                throw std::string("Error Requesting");

            long int packet;
            long int type;
            //  Get and check the packet header.

            if(!receive(tcpSocket, packet))
                throw std::string("Error Recieving");

            if(!receive(tcpSocket, type))
                throw std::string("Error Recieving");

            if(type != ClientCodes::EReply)
                throw std::string("Bad Packet");

            if(packet != ClientCodes::EData)
                throw std::string("Bad Reply Type");

            if(!receive(tcpSocket, size))
                throw std::string();

            if(size != info.size())
                throw std::string("Bad Data Packet");


            // Actually getting the data and store in "data"
            std::vector<double>::iterator iData;
            for (iData = data.begin(); iData != data.end(); iData++) {
                if (!receive(tcpSocket, *iData))
                    throw std::string();
            }

            //- Look up channels -------------
            // get the timestamp
            timestamp = data[FrameChannel];



            /*
                                                       * Get channels corresponding to markers
                                                       *
                                                       */

            std::vector< MarkerChannel >::iterator iMarker;
            std::vector< MarkerData >::iterator iMarkerData;

            for(    iMarker = MarkerChannels.begin(),
                    iMarkerData = markerPositions.begin();
                    iMarker != MarkerChannels.end(); iMarker++, iMarkerData++)
            {
                iMarkerData->X = data[iMarker->X];
                iMarkerData->Y = data[iMarker->Y];
                iMarkerData->Y = data[iMarker->Z];
                if(data[iMarker->O] > 0.5)
                    iMarkerData->Visible = false;
                else
                    iMarkerData->Visible = true;
            }



            /*
                                                                     * Get the channels corresponding to bodies
                                                                     * the world is Z-up
                                                                     * the translational values are in millimeters
                                                                     * the rotational values are in radians
                                                                     */

            std::vector<BodyChannel>::iterator iBody;
            std::vector<BodyData>::iterator iBodyData;

            for (       iBody = BodyChannels.begin(),
                        iBodyData = bodyPositions.begin();
                        iBody != BodyChannels.end(); iBody++, iBodyData++) {

                iBodyData->TX = data[iBody->TX];
                iBodyData->TY = data[iBody->TY];
                iBodyData->TZ = data[iBody->TZ];

                std::cout << "BodyName: " << iBody->Name        << std::endl
                          << "X: "        << iBodyData->TX      << std::endl
                          << "Y: "        << iBodyData->TY      << std::endl
                          << "Z: "        << iBodyData->TZ      << std::endl
                          << "Roll: "     << data[iBody->RX]    << std::endl
                          << "Pitch: "    << data[iBody->RY]    << std::endl
                          << "Yaw: "      << data[iBody->RZ]    << std::endl;


                /*
                                                                        double len, tmp;
                                                                        len = sqrt( data[iBody->RX] * data[iBody->RX] +
                                                                                    data[iBody->RY] * data[iBody->RY] +
                                                                                    data[iBody->RZ] * data[iBody->RZ]);

                                                                        iBodyData->QW = cos(len / 2.0);
                                                                        tmp = sin(len / 2.0);
                                                                        */


                std::cout << "--------------Frame: " << timestamp << std::endl;
            }

        }

        //-----------------------------
        //------STOPPED HERE----------
        //----------------------------


    }
    catch (const std::string& rMsg) {
        if (rMsg.empty())
            std::cout << "Error!\n";
        else
            std::cout << rMsg.c_str() << std::endl;

    }

    connectButton->setEnabled(true);

    //return 0;




//    int count = 0;
//    for (count = 0; count < 10; count++) {
//        tcpSocket->read(buf, 512);

//        data_show = QString::number(count) + tr(" ") + tr(buf);

//        std::cout << data_show.toStdString() << std::endl;
//        this->connectButton->setEnabled(true);
//        usleep(0.5*1000000);
//    }












//    QDataStream in(tcpSocket);
//    in.setVersion(QDataStream::Qt_4_0);

//    if (blockSize == 0) {
//        if (tcpSocket->bytesAvailable() < (int)sizeof(quint16))
//            return;
//        in >> blockSize;
//    }

//    if (tcpSocket->bytesAvailable() < blockSize)
//        return;
//    QString nextFortune;
//    in >> nextFortune;

//    if (nextFortune == currentFortune) {
//        QTimer::singleShot(0, this, SLOT(requestNewFortune()));
//        return;
//    }
//    currentFortune = nextFortune;
//    statusLabel->setText(currentFortune);
//    connectButton->setEnabled(true);
}











void Client::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(tcpSocket->errorString()));
    }

    connectButton->setEnabled(true);
}
void Client::enableconnectButton(){
    connectButton->setEnabled(!hostLineEdit->text().isEmpty()
                                 && !portLineEdit->text().isEmpty());
}






// RTE SDK

bool Client::receive(QTcpSocket *pTcpSocket, char *pBuffer, int BufferSize) {
    char* p = pBuffer;
    char* e = pBuffer + BufferSize;
    int result;

    // qint64 QIODevice::read ( char * data, qint64 maxSize )
    // ref: http://doc.qt.nokia.com/latest/qiodevice.html#read
    while (p != e) {
        result = pTcpSocket->read(p, e - p);
        if (result == -1)
            return false;
        p += result;
    }

    return true;
}
