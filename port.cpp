//#include <stddef.h>
#include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <math.h>

using namespace std;

/*--------- port setting parameters ---------*/
#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyUSB0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

/* slopes for normalization ----------------*/
#define PC2RC2_RPY_SLOPE (19050-10950)/2
#define PC2RC2_COLL_SLOPE (18500-11500)/2

bool flashOn = true;
int fd, cl;
struct termios oldtio, newtio;

int initPort() {
    fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror(MODEMDEVICE);
        exit(-1);
    }
    else
        std::cout << "ttyUSB0 port opened.\n";

    tcgetattr(fd, &oldtio); /* save current port settings */
    std::cout << "old port settings saved.\n";

    bzero(&newtio, sizeof (newtio));
    newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
    newtio.c_cc[VMIN] = 5; /* blocking read until 5 chars received */

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &newtio);
    std::cout << "ttyUSB0 initialized.\n";
    return fd;
}

void closePort() {
    tcsetattr(fd, TCSANOW, &oldtio);
    std::cout << "old port settings recovered.\n";
    cl = close(fd);
    if (cl != -1)
        std::cout << "port closed.\n";
}



int normalized_to_pc2rc2(float cntlcmd, int troff, float slp)
{
  int y = 15000 + troff + ((int) (cntlcmd * slp));
  if ((y % 2 == 0) && (y != 15000))
    y += 1;

  return y;
}

void sendArray(char *buffer, int len)
{
    write(fd, buffer, len);
}


void sendToPC2RC(float roll_arg, float pitch_arg,
        float collective_arg, float yaw_arg)
{

    char buf[12];
    int roll = normalized_to_pc2rc2(-roll_arg, 0, PC2RC2_RPY_SLOPE);
    int pitch = normalized_to_pc2rc2(pitch_arg, 0, PC2RC2_RPY_SLOPE);
    int yaw = normalized_to_pc2rc2(-yaw_arg, 0, PC2RC2_RPY_SLOPE);
    int collective = normalized_to_pc2rc2(collective_arg,
            0, PC2RC2_COLL_SLOPE);
//
//    cout << "-----control commands------\n";
//    cout << "roll_input: " << roll << endl;
//    cout << "pitch_input: " << pitch << endl;
//    cout << "yaw_input: " << yaw << endl;
//    cout << "collective_input: " << collective << endl;

    buf[0] = '~';
    buf[1] = '~';

    // Set the channel
    buf[2] = (char) 15; // Here we are setting channels 1-4

    // Flash the LED as a confirmation
    if (flashOn)
        buf[3] = (char) '0';
    else
        buf[3] = (char) '1';
    flashOn = !flashOn;

    // Set the value of the channel; high byte first...
    buf[4] = (char) ((roll & 0xFF00) >> 8);
    // ...followed by low byte
    buf[5] = (char) (roll & 0x00FF);

    // Set the value of the channel; high byte first...
    buf[6] = (char) ((pitch & 0xFF00) >> 8);
    // ...followed by low byte
    buf[7] = (char) (pitch & 0x00FF);

    // Set the value of the channel; high byte first...
    buf[8] = (char) ((collective & 0xFF00) >> 8);
    // ...followed by low byte
    buf[9] = (char) (collective & 0x00FF);

    // Set the value of the channel; high byte first...
    buf[10] = (char) ((yaw & 0xFF00) >> 8);
    // ...followed by low byte
    buf[11] = (char) (yaw & 0x00FF);

    sendArray(buf, 12);
}
