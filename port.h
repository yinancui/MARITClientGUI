/* 
 * File:   port.h
 * Author: marrk
 *
 * Created on April 20, 2011, 11:53 AM
 */

#ifndef PORT_H
#define	PORT_H

int initPort();
void closePort();

void sendToPC2RC(float roll_arg, float pitch_arg,
        float collective_arg, float yaw_arg);

#endif	/* PORT_H */

