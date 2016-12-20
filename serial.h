#ifndef SERIAL_H
#define SERIAL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

class serial
{
public:
    serial();
    int   serialOpen      (char *device, int baud) ;
    void  serialClose     ();
    void  serialFlush     ();
    void  serialPutchar   (unsigned char c);
    void  serialPuts      (char *s);
    void  serialPrintf    (char *message, ...);
    int   serialDataAvail ();
    int   serialGetchar   ();
private:
    char* device;
    int fd;
};

#endif

