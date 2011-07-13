#include "client.h"
#include "ui_client.h"
#include <QtGui>
//#include <QtNetwork>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include "ClientCodes.h"
#include <string>
#include <vector>
#include <algorithm>
#include "errorcode.h"


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

    statusLabel = new QLabel(tr("Display the real time data here"));
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


    //tcpSocket = new QTcpSocket(this);
    //   myconsole = new console;



    connect(hostLineEdit, SIGNAL(textChanged(const QString &)),this, SLOT(enableconnectButton()));
    connect(portLineEdit, SIGNAL(textChanged(const QString &)),this, SLOT(enableconnectButton()));
    connect(connectButton, SIGNAL(clicked()),this, SLOT(requestNewFortune()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));

    //errorCode.setValue(CONNECTION_FAILURE);
    connect(&errorCode, SIGNAL(valueChanged(int)), this, SLOT(displayError(int)));

    //connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readFortune()));
    //connect(tcpSocket, SIGNAL(connected()), this, SLOT(readFortune()));
    //connect(tcpSocket, SIGNAL(connected()), this, SLOT(dummy()));
    //connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(dummy()));

    //----------- open new console-------------
    //connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError(QAbstractSocket::SocketError)));
    //connect(this->connectButton, SIGNAL(clicked()), myconsole, SLOT(startprocess()));
    //connect(tcpSocket, SIGNAL(readyRead()), myconsole, SLOT(startprocess()));


    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(hostLabel, 0, 0);
    mainLayout->addWidget(hostLineEdit, 0, 1);
    mainLayout->addWidget(portLabel, 1, 0);
    mainLayout->addWidget(portLineEdit, 1, 1);
    mainLayout->addWidget(statusLabel, 2, 0, 1, 2);
    //mainLayout->addWidget(this->textEdit, 2, 0, 1, 2);
    //mainLayout->addWidget(this->textBrowser, 2, 0, 1, 2);
    //mainLayout->addWidget(this->vtkWidget, 2, 0, 1, 2);
    mainLayout->addWidget(buttonBox, 3, 0, 1, 2);
    setLayout(mainLayout);


    //    // vtk ----------------------------
    //    QString filename = "/home/marrk/coding/qtproj/qtcp/ClientGUI/cow.obj";
    //    vtkWidget = new QVTKWidget(this, QFlag(0));
    //    //-------------------------------------
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





    setWindowTitle(tr("MARIT ClientGUI"));
    portLineEdit->setFocus();
    std::cout << "out of ctor.\n";
}

Client::~Client()
{
    delete ui;
}


void Client::requestNewFortune()
{
    connectButton->setEnabled(false);

    int retVal = connectServer();
    switch (retVal) {

    case SOCKET_CREATION_FAILURE:
        errorCode.setValue(SOCKET_CREATION_FAILURE);
        break;

    case SERVERINFO_FAILURE:
        errorCode.setValue(SERVERINFO_FAILURE);
       break;

    case CONNECTION_FAILURE:
        errorCode.setValue(CONNECTION_FAILURE);
        break;

    case NO_ERROR:
        // no error
        std::cout << "connected.\n";
        Client::readFortune();
    }

}




void Client::readFortune()
{
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
        if (::send(sockfd, buff, pBuff - buff, 0) == -1)
            throw std::string("Error Requesting");
        //std::cout << "write to server: " << write_tmp << std::endl;

        long int packet;
        long int type;

        // recv and pass data to packet
        if (!receive(sockfd, packet))
            throw std::string("Error receiving packet.\n");
        //recv and pass data to type
        if (!receive(sockfd, type))
            throw std::string("Bad receiving.\n");
        if (type != ClientCodes::EReply)
            throw std::string("Bad reply type.\n");
        if (packet != ClientCodes::EInfo)
            throw std::string("Bad packet.\n");

        //long int size;
        unsigned long int size;

        // recv and pass data to size, thus get the size of future data?
        if (!receive(sockfd, size))
            throw std::string("Wrong size.\n");   // ????

        info.resize(size);

        std::vector<std::string>::iterator iInfo;
        //info.
        for (iInfo = info.begin(); iInfo != info.end(); iInfo++) {
            long int s;
            char c[255];
            char* p = c;

            if (!receive(sockfd, s))
                throw std::string();
            if (!receive(sockfd, c, s))
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
        int FrameChannel = 0;

        for (iInfo = info.begin(); iInfo != info.end(); iInfo++) {
            // Extract the channel type
            // looking for the FIRST '<' in each info string

            //int openBrace = iInfo->find('<');
            unsigned int openBrace = iInfo->find('<');
            // if no '<' found
            if (openBrace == iInfo->npos)
                throw std::string("Bad channel ID");

            //int closeBrace = iInfo->find('>');
            unsigned int closeBrace = iInfo->find('>');
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
            //int space = Name.rfind(' ');
            unsigned int space = Name.rfind(' ');
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
        for (i = 0; i < 1000; i++) {
            // print frame # at the beginning of the loop
            std::cout << "--------------Frame: " << timestamp << std::endl;
            // use the same routine as when getting channel info

            pBuff = buff;

            * ((long int *) pBuff) = ClientCodes::EData;
            pBuff += sizeof(long int);
            * ((long int *) pBuff) = ClientCodes::ERequest;
            pBuff += sizeof(long int);


            if (::send(sockfd, buff, pBuff - buff, 0) == -1)
                throw std::string("Error Requesting");

            long int packet;
            long int type;
            //  Get and check the packet header.

            if(!receive(sockfd, packet))
                throw std::string("Error Recieving");

            if(!receive(sockfd, type))
                throw std::string("Error Recieving");

            if(type != ClientCodes::EReply)
                throw std::string("Bad Packet");

            if(packet != ClientCodes::EData)
                throw std::string("Bad Reply Type");

            if(!receive(sockfd, size))
                throw std::string();

            if(size != info.size())
                throw std::string("Bad Data Packet");


            // Actually getting the data and store in "data"
            std::vector<double>::iterator iData;
            for (iData = data.begin(); iData != data.end(); iData++) {
                if (!receive(sockfd, *iData))
                    throw std::string();
            }

            //- Look up channels -------------
            // get the timestamp
            timestamp = data[FrameChannel];

            //Get channels corresponding to markers
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



//            * Get the channels corresponding to bodies
//            * the world is Z-up
//            * the translational values are in millimeters
//            * the rotational values are in radians

            std::vector<BodyChannel>::iterator iBody;
            std::vector<BodyData>::iterator iBodyData;

            for (       iBody = BodyChannels.begin(),
                        iBodyData = bodyPositions.begin();
                        iBody != BodyChannels.end(); iBody++, iBodyData++) {

                iBodyData->TX = data[iBody->TX];
                iBodyData->TY = data[iBody->TY];
                iBodyData->TZ = data[iBody->TZ];



//                        The channel data is in the angle-axis form.
//                        The following converts this to a quaternion.
//                        =============================================================
//                        An angle-axis is vector, the direction of which is the axis
//                        of rotation and the length of which is the amount of
//                        rotation in radians.
//                        =============================================================


                double len, tmp;

                len = sqrt(	data[iBody->RX] * data[iBody->RX] +
                                data[iBody->RY] * data[iBody->RY] +
                                data[iBody->RZ] * data[iBody->RZ]);

                iBodyData->QW = cos(len / 2.0);
                tmp = sin(len / 2.0);
                if (len < 1e-10)
                {
                    iBodyData->QX = data[iBody->RX];
                    iBodyData->QY = data[iBody->RY];
                    iBodyData->QZ = data[iBody->RZ];
                }
                else
                {
                    iBodyData->QX = data[iBody->RX] * tmp/len;
                    iBodyData->QY = data[iBody->RY] * tmp/len;
                    iBodyData->QZ = data[iBody->RZ] * tmp/len;
                }

                //	The following converts angle-axis to a rotation matrix.

                double c, s, x, y, z;

                if (len < 1e-15)
                {
                    iBodyData->GlobalRotation[0][0] = iBodyData->GlobalRotation[1][1] = iBodyData->GlobalRotation[2][2] = 1.0;
                    iBodyData->GlobalRotation[0][1] = iBodyData->GlobalRotation[0][2] = iBodyData->GlobalRotation[1][0] =
                            iBodyData->GlobalRotation[1][2]	= iBodyData->GlobalRotation[2][0] = iBodyData->GlobalRotation[2][1] = 0.0;
                }
                else
                {
                    x = data[iBody->RX]/len;
                    y = data[iBody->RY]/len;
                    z = data[iBody->RZ]/len;

                    c = cos(len);
                    s = sin(len);

                    iBodyData->GlobalRotation[0][0] = c + (1-c)*x*x;
                    iBodyData->GlobalRotation[0][1] =     (1-c)*x*y + s*(-z);
                    iBodyData->GlobalRotation[0][2] =     (1-c)*x*z + s*y;
                    iBodyData->GlobalRotation[1][0] =     (1-c)*y*x + s*z;
                    iBodyData->GlobalRotation[1][1] = c + (1-c)*y*y;
                    iBodyData->GlobalRotation[1][2] =     (1-c)*y*z + s*(-x);
                    iBodyData->GlobalRotation[2][0] =     (1-c)*z*x + s*(-y);
                    iBodyData->GlobalRotation[2][1] =     (1-c)*z*y + s*x;
                    iBodyData->GlobalRotation[2][2] = c + (1-c)*z*z;
                }

                // now convert rotation matrix to nasty Euler angles (yuk)
                // you could convert direct from angle-axis to Euler if you wish

                //	'Look out for angle-flips, Paul...'
                //  Algorithm: GraphicsGems II - Matrix Techniques VII.1 p 320
                assert(fabs(iBodyData->GlobalRotation[0][2]) <= 1);
                iBodyData->EulerY = asin(-iBodyData->GlobalRotation[2][0]);

                if(fabs(cos(y)) >
                        std::numeric_limits<double>::epsilon() ) 	// cos(y) != 0 Gimbal-Lock
                {
                    iBodyData->EulerX = atan2(iBodyData->GlobalRotation[2][1], iBodyData->GlobalRotation[2][2]);
                    iBodyData->EulerZ = atan2(iBodyData->GlobalRotation[1][0], iBodyData->GlobalRotation[0][0]);
                }
                else
                {
                    iBodyData->EulerZ = 0;
                    iBodyData->EulerX = atan2(iBodyData->GlobalRotation[0][1], iBodyData->GlobalRotation[1][1]);
                }

                std::cout << "BodyName: " << iBody->Name        << std::endl
                          << "X: "        << iBodyData->TX      << std::endl
                          << "Y: "        << iBodyData->TY      << std::endl
                          << "Z: "        << iBodyData->TZ      << std::endl;

                std::cout << "Roll: "    << iBodyData->EulerX    << std::endl
                          << "Pitch: "   << iBodyData->EulerY    << std::endl
                          << "Yaw: "     << iBodyData->EulerZ    << std::endl;


            }
            //std::cout << "--------------Frame: " << timestamp << std::endl;



        }


    }
    catch (const std::string& rMsg) {
        if (rMsg.empty())
            std::cout << "Error!\n";
        else
            std::cout << rMsg.c_str() << std::endl;
    }

    std::cout << "Done.\n";
    connectButton->setEnabled(true);    
}











void Client::displayError(int errorVal)
{
    switch (errorVal) {

    case CONNECTION_FAILURE: {
        QMessageBox::information(this, tr("ClientGUI"),
                                 tr("Failed to connect, check your input."));
        std::cout << "Frome error handling CONNECTIION_FAILURE.\n";
        errorCode.setValue(NO_ERROR);
        break;
    }

    case SOCKET_CREATION_FAILURE:
        QMessageBox::information(this, tr("ClientGUI"),
                                 tr("Failed to create socekt, try again."));
        errorCode.setValue(NO_ERROR);
        break;

    case SERVERINFO_FAILURE:
        std::cout << "From server info failure.\n";
        QMessageBox::information(this, tr("ClientGUI"),
                                 tr("Failed to get server information, check your input."));
        errorCode.setValue(NO_ERROR);
        break;

    case NO_ERROR:
        // do nothing
        break;

    }

    connectButton->setEnabled(true);
}



void Client::enableconnectButton(){
    connectButton->setEnabled(!hostLineEdit->text().isEmpty()
                              && !portLineEdit->text().isEmpty());
}






// RTE SDK

bool Client::receive(int Socket, char* pBuffer, int BufferSize) {

    //std::cout << "in receive call.\n";
    char* p = pBuffer;
    char* e = pBuffer + BufferSize;
    int result;

    // qint64 QIODevice::read ( char * data, qint64 maxSize )
    // ref: http://doc.qt.nokia.com/latest/qiodevice.html#read
    while (p != e) {
        result = recv(Socket, p, e - p, 0);
        if (result == -1) {
            std::cout << "read failed.\n";
            return false;
        }
        p += result;

    }
    //std::cout << "got something.\n";
    return true;
}


// other receving functions

bool Client::receive(int Socket, long int & Val)
{return receive(Socket, (char*)& Val, sizeof(Val));}


bool Client::receive(int Socket, unsigned long int & Val)
{return receive(Socket, (char*)& Val, sizeof(Val));}


bool Client::receive(int Socket, double &Val)
{return receive(Socket, (char*)& Val, sizeof(Val));}


//---------- test connted()
void Client::dummy() {
    std::cout << "Connected to server.\n";
    //std::cout << "Ready to read.\n";
    connectButton->setEnabled(true);
}




//--------------connect to server routine, using Unix socket

void* Client::get_in_addr(struct sockaddr* sa) {
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//-------------------------
int Client::connectServer() {

    ::memset(&hints, 0, sizeof hints);
    hints.ai_family     = AF_UNSPEC;
    hints.ai_socktype   = SOCK_STREAM;

    //----- loading the serverip and port from labels
    QByteArray a1 = this->hostLineEdit->text().toAscii();
    const char* ServerIP = a1.data();

    QByteArray a2 = this->portLineEdit->text().toAscii();
    const char* PORT = a2.data();


    std::cout << "Connecting to " << ServerIP << ":" << PORT << "...";
    // hard code the server's IP and port
    if ((::getaddrinfo(ServerIP, PORT, &hints, &servinfo)) != 0) {
        std::cout << "Failed to get server info.\n";
        return SERVERINFO_FAILURE;
    }

   // for (pAddrinfo = servinfo; pAddrinfo != NULL; pAddrinfo = pAddrinfo->ai_next) {
        // create client's socket

        pAddrinfo = servinfo;
        if ((sockfd = socket(pAddrinfo->ai_family, pAddrinfo->ai_socktype,
                             pAddrinfo->ai_protocol)) == -1) {
            std::cout << "Failed to create client socket.\n";
            //errorCode.setValue(SOCKET_CREATION_FAILURE);
            //continue;
            //break;
            return SOCKET_CREATION_FAILURE;
        }
        // connect to server

        if (::connect(sockfd, pAddrinfo->ai_addr, pAddrinfo->ai_addrlen) == -1) {
            ::close(sockfd);
            std::cout << "Failed to connect to server.\n";
            //errorCode.setValue(CONNECTION_FAILURE);
            //continue;
            //break;
            return CONNECTION_FAILURE;
        }
        //usleep(0.01 * M);
     //   break;
   // }
    // if still not connected after looping
    if (pAddrinfo == NULL) {
        std::cout << "Failed to connect to server.\n";
        return CONNECTION_FAILURE;
    }

    //std::cout << "Connected.\n";

    inet_ntop(pAddrinfo->ai_family, get_in_addr((struct sockaddr *) pAddrinfo->ai_addr),
              sIPv6, sizeof sIPv6);

    freeaddrinfo(servinfo);


    return NO_ERROR;
}



