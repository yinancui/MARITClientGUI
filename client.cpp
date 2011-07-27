//---- Qt includes -----
#include <QtGui>
//----- local includes ------
#include "client.h"
#include "ui_client.h"
#include "port.h"
//----- other includes --------
#include <iostream>
#include <algorithm>
//#include "ClientCodes.h"
//#include "errorcode.h"
//#include <unistd.h>
//#include <stdlib.h>
//#include <string>
//#include <vector>




//------------------------- globals ------------------------------
bool bHoverflag = FALSE;    // set to true when control is desired
int g_iDuration = 0;        // flight duration

//-------- 6DOF data ------------
float g_fX;
float g_fY;
float g_fZ;
float g_fRoll;
float g_fPitch;
float g_fYaw;

//--------- ref trajectory -----------
float g_fRollRef;
float g_fPitchRef;
float g_fYawRef;
float g_fXRef;
float g_fYRef;
float g_fZRef;

//---------- attitude conroller param --------------------------
// errors
float eAphi = 0, eA1phi = 0, eA2phi = 0;
float eAtheta = 0, eA1theta = 0, eA2theta = 0;
float eApsi = 0, eA1psi = 0, eA2psi = 0;
float eAz = 0, eA1z = 0, eA2z = 0;
// PID param
float kAp_phi	= 3.1;
float kAi_phi	= 0.01; // old = 0.03
float kAd_phi	= 0;
float kA1phi = kAp_phi + kAi_phi + kAd_phi;
float kA2phi = -kAp_phi - 2 * kAd_phi;
float kA3phi = kAd_phi;

float kAp_theta = 3;
float kAi_theta = 0.08;
float kAd_theta = 0;
float kA1theta = kAp_theta + kAi_theta + kAd_theta;
float kA2theta = -kAp_theta - 2 * kAd_theta;
float kA3theta = kAd_theta;

float kAp_psi = 3;
float kAi_psi = 0.08;
float kAd_psi = 0;
float kA1psi = kAp_psi + kAi_psi + kAd_psi;
float kA2psi = -kAp_psi - 2 * kAd_psi;
float kA3psi = kAd_psi;

float kAp_z = 0.01;
float kAi_z = 0.0002;
float kAd_z = 0;
float kA1z = kAp_z + kAi_z + kAd_z;
float kA2z = -kAp_z - 2 * kAd_z;
float kA3z = kAd_z;

// output commands
float roll_a = 0, delta_roll_a = 0;	// 'a' for attitude controller
float pitch_a = 0, delta_pitch_a = 0;
float yaw_a = 0, delta_yaw_a = 0;
float coll_a = 0, delta_coll_a = 0;
float coll_at = 0;





Client::Client(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Client)
{
    ui->setupUi(this);

    //------------ declare widgets -----------------------------
    hostLabel = new QLabel(tr("&Server name:"));
    portLabel = new QLabel(tr("S&erver port:"));

    hostLineEdit = new QLineEdit("192.168.15.100");
    portLineEdit = new QLineEdit("800");
    portLineEdit->setValidator(new QIntValidator(1, 65535, this));

    hostLabel->setBuddy(hostLineEdit);
    portLabel->setBuddy(portLineEdit);

    statusLabel = new QLabel(tr("Display info here"));
//    this->textEdit = new QTextEdit(tr("switched to textEdit."));
//    this->textEdit->setReadOnly(true);
//    this->textBrowser = new QTextBrowser; //("switched to textBrowser.");


    connectButton = new QPushButton(tr("Connect"));
    connectButton->setDefault(true);
    //connectButton->setEnabled(false);
    connectButton->setEnabled(true);

    quitButton = new QPushButton(tr("Quit"));
    testHoverButton = new QPushButton(tr("TestHover"));



    //---------------------set signals---------------------------------

    //tcpSocket = new QTcpSocket(this);
    //   myconsole = new console;

    connect(hostLineEdit, SIGNAL(textChanged(const QString &)),this, SLOT(enableconnectButton()));
    connect(portLineEdit, SIGNAL(textChanged(const QString &)),this, SLOT(enableconnectButton()));
    connect(connectButton, SIGNAL(clicked()),this, SLOT(requestNewFortune()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));

    //errorCode.setValue(CONNECTION_FAILURE);
    connect(&errorCode, SIGNAL(valueChanged(int)), this, SLOT(displayError(int)));

    //----------test hover button
    connect(testHoverButton, SIGNAL(clicked()), this, SLOT(dummyHover()));

    //connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readFortune()));
    //connect(tcpSocket, SIGNAL(connected()), this, SLOT(readFortune()));
    //connect(tcpSocket, SIGNAL(connected()), this, SLOT(dummy()));
    //connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(dummy()));

    //----------- open new console-------------
    //connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError(QAbstractSocket::SocketError)));
    //connect(this->connectButton, SIGNAL(clicked()), myconsole, SLOT(startprocess()));
    //connect(tcpSocket, SIGNAL(readyRead()), myconsole, SLOT(startprocess()));


    //------------ set layouts ------------------------------------------------
    QGroupBox* buttonGroup = new QGroupBox;
    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->addWidget(connectButton);
    vbox->addWidget(quitButton);
    vbox->addWidget(testHoverButton);
    buttonGroup->setLayout(vbox);

    QGroupBox* addrGroup = new QGroupBox;
    QGridLayout* addrGrid = new QGridLayout;
    addrGrid->addWidget(hostLabel, 0, 0);
    addrGrid->addWidget(hostLineEdit, 0, 1);
    addrGrid->addWidget(portLabel, 1, 0);
    addrGrid->addWidget(portLineEdit, 1, 1);
    addrGroup->setLayout(addrGrid);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(statusLabel, 0, 0);
    mainLayout->addWidget(addrGroup, 1, 0);
    mainLayout->addWidget(buttonGroup, 1, 1);
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

    case SUCCESS:
        // no error
        std::cout << "connected.\n";
        Client::readFortune();
    }

}




void Client::readFortune()
{
    std::cout << "Now begin receiving...\n";


    //---------- request for data block ---------------
    try {

        //---------init serial port-----------------
        int retSe;
        if ((retSe = initPort()) < 0)
            throw std::string("Failed to initializ serial port");


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
            std::cout << "--------------Frame: " << timestamp << "------------------" << std::endl;
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



            //---------------------------------------------------
            //--------------TODO: only get the info of DF6
            //---------------------------------------------------

//            * Get the channels corresponding to bodies
//            * the world is Z-up
//            * the translational values are in millimeters
//            * the rotational values are in radians

            std::vector<BodyChannel>::iterator iBody;
            std::vector<BodyData>::iterator iBodyData;
            //----
            std::string df6 = "DF6:DF6";

            for (       iBody = BodyChannels.begin(),
                        iBodyData = bodyPositions.begin();
                        iBody != BodyChannels.end(); iBody++, iBodyData++) {

                if (iBody->Name == df6) {
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

                    //--------------- passing realtime data to globals
                    g_fX = iBodyData->TX;
                    g_fY = iBodyData->TY;
                    g_fZ = iBodyData->TZ;
                    g_fRoll = iBodyData->EulerX;
                    g_fPitch = iBodyData->EulerY;
                    g_fYaw = iBodyData->EulerZ;

                    //---------- run att controller if flag was set
                    bHoverflag = TRUE;
                    if (bHoverflag) {

                        hoverAtt(0, 0, 0, 100);
                    }
                    bHoverflag = FALSE;




                } // out of if == df6 loop
            } // out of for loop
        }
    }
    catch (const std::string& rMsg) {
        if (rMsg.empty())
            std::cout << "Error!\n";
        else
            std::cout << rMsg.c_str() << std::endl;
    }


    //--------done with this connection-------------
    std::cout << "Done.\n";
    closePort(); // close serial port
    connectButton->setEnabled(true);    
}










//---------------------GUI error handling-------------------------------------
void Client::displayError(int errorVal)
{
    switch (errorVal) {

    case CONNECTION_FAILURE: {
        QMessageBox::information(this, tr("ClientGUI"),
                                 tr("Failed to connect, check your input."));
        std::cout << "Frome error handling CONNECTIION_FAILURE.\n";
        errorCode.setValue(SUCCESS);
        break;
    }

    case SOCKET_CREATION_FAILURE:
        QMessageBox::information(this, tr("ClientGUI"),
                                 tr("Failed to create socekt, try again."));
        errorCode.setValue(SUCCESS);
        break;

    case SERVERINFO_FAILURE:
        std::cout << "From server info failure.\n";
        QMessageBox::information(this, tr("ClientGUI"),
                                 tr("Failed to get server information, check your input."));
        errorCode.setValue(SUCCESS);
        break;

    case SUCCESS:
        // do nothing
        break;

    }

    connectButton->setEnabled(true);
}
//-----------------------------------------------------------------------------


void Client::enableconnectButton(){
    connectButton->setEnabled(!hostLineEdit->text().isEmpty()
                              && !portLineEdit->text().isEmpty());
}






//------------------- RTE SDK modified recv methods------------------
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


////---------- test connted()
//void Client::dummy() {
//    std::cout << "Connected to server.\n";
//    //std::cout << "Ready to read.\n";
//    connectButton->setEnabled(true);
//}
//------------------------------------------------------------------



//--------------connect to server routine, using Unix socket

void* Client::get_in_addr(struct sockaddr* sa) {
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


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


    return SUCCESS;
}





//----------------- att hovering controller --------------------------
void Client::hoverAtt(float fRollRef, float fPitchRef, float fYawRef, float fZRef) {
    float testColl = -0.45;
    //g_iDuration = duration;
    g_fRollRef = fRollRef;
    g_fPitchRef = fPitchRef;
    g_fYawRef = fYawRef;
    g_fZRef = fZRef;

//    int retVal;
//    if ((retVal = initPort()) < 0) {
//        std::cout << "Failed to initialize serial port.\n";
//        return;
//    }
    //------ print control param-------------------------
    std::cout << "########### Attitude controller ##########\n"
            << "ref roll: " << g_fRollRef << std::endl
            << "ref pitch: " << g_fPitchRef << std::endl
            << "ref yaw: " << g_fYawRef << std::endl
            << "ref height: " << g_fZRef << std::endl;
            //<< "##########################################\n";

    //---------- begin control loop -------------------------
    //int timer = 0;
    //for (timer = 0; timer < g_iDuration; timer++) {
//        std::cout << "	Roll: "     << g_fRoll  << std::endl; //X axis rotation value(roll)
//        std::cout << "	Pitch: "    << g_fPitch << std::endl; //Y axis rotation value(pitch)
//        std::cout << "	Yaw: "      << g_fYaw   << std::endl; //Z axis rotation value(yaw)
//        std::cout << "	X value: "  << g_fX     << std::endl;
//        std::cout << "	Y value: "  << g_fY     << std::endl;
//        std::cout << "	Z value: "  << g_fZ     << std::endl;

        //---------- generating commands
        eA2phi = eA1phi;
        eA1phi = eAphi;
        eAphi = g_fRollRef - g_fRoll;
        delta_roll_a = kA1phi * eAphi + kA2phi * eA1phi
                + kA3phi * eA2phi;
        roll_a += delta_roll_a;
        if (roll_a > 0.3)
            roll_a = 0.3;
        else if (roll_a < -0.3)
            roll_a = -0.3;

        /* use OL to roll here	*/
        //roll_o = 0.2 * sin(timera * 2 * PI / 50);

        eA2theta = eA1theta;
        eA1theta = eAtheta;
        eAtheta = g_fPitchRef - g_fPitch;
        delta_pitch_a = kA1theta * eAtheta + kA2theta * eA1theta
                + kA3theta * eA2theta;
        pitch_a += delta_pitch_a;
        if (pitch_a > 0.3)
            pitch_a = 0.3;
        else if (pitch_a < -0.3)
            pitch_a = -0.3;
        // OL ch2 pitch control
        //ch2_o = 0.15 * sin(timera*2*PI/50);
        // ch2_o = -0.1;

        eA2psi = eA1psi;
        eA1psi = eApsi;
        eApsi = g_fYawRef - g_fYaw;
        if (eApsi > 0.2)
            eApsi = 0.2;
        else if (eApsi < -0.2)
            eApsi = -0.2;
        //cout << "yaw error: " << eApsi << endl;
        delta_yaw_a = kA1psi * eApsi + kA2psi * eA1psi
                + kA3psi * eA2psi;
        yaw_a += delta_yaw_a;
        if (yaw_a > 0.3)
            yaw_a = 0.3;
        else if (yaw_a < -0.3)
            yaw_a = -0.3;
        // OL SID for yaw
        //ch4_o = -0.1 * sin(timera*2*PI/25);
        // ch4_o = -0.25;

        /*------------collective, z----------------*/
//        //z_temp = (z_ref / duration) * ((timera / 10) + 1);
//        eA2z = eA1z;
//        eA1z = eAz;
//        eAz = g_fZRef - g_fZ;
//        //eAz = z_temp - rZ;
//        delta_coll_a = kA1z * eAz + kA2z * eA1z + kA3z * eA2z;
//        //delta_coll_a = delta_z * M;	//??????????
//        coll_at += delta_coll_a;
//        coll_a = coll_at + coll_h;
//        if (coll_a > 0.2)
//            coll_a = 0.2;
//        else if (coll_a < -0.5)
//            coll_a = -0.5;

//        std::cout << "Sending control commands:\n"
//                  << "ch1: " << roll_a << std::endl
//                  << "ch2: " << pitch_a << std::endl
//                  << "ch3: " << testColl << std::endl
//                  << "ch4: " << yaw_a << std::endl;
        sendToPC2RC(roll_a, pitch_a, testColl, yaw_a);
        //usleep(0.003 * M);
    //}
    //closePort();

}


void Client::dummyHover() {
    hoverAtt(0, 0, 0, 100);
}
