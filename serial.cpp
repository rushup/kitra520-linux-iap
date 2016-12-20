#include "serial.h"

serial::serial()
{

}

int serial::serialOpen (char *device, int baud)
{
struct termios options ;
speed_t myBaud ;
int     status, fd ;

switch (baud)
		{
		case     50:	myBaud =     B50 ; break ;
		case     75:	myBaud =     B75 ; break ;
		case    110:	myBaud =    B110 ; break ;
		case    134:	myBaud =    B134 ; break ;
		case    150:	myBaud =    B150 ; break ;
		case    200:	myBaud =    B200 ; break ;
		case    300:	myBaud =    B300 ; break ;
		case    600:	myBaud =    B600 ; break ;
		case   1200:	myBaud =   B1200 ; break ;
		case   1800:	myBaud =   B1800 ; break ;
		case   2400:	myBaud =   B2400 ; break ;
		case   9600:	myBaud =   B9600 ; break ;
		case  19200:	myBaud =  B19200 ; break ;
		case  38400:	myBaud =  B38400 ; break ;
		case  57600:	myBaud =  B57600 ; break ;
		case 115200:	myBaud = B115200 ; break ;
		case 230400:	myBaud = B230400 ; break ;

		default:
				return -2 ;
		}

if ((fd = open (device, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK)) == -1)
		return -1 ;

fcntl (fd, F_SETFL, O_RDWR) ;

// Get and modify current options:

tcgetattr (fd, &options) ;

cfmakeraw   (&options) ;
cfsetispeed (&options, myBaud) ;
cfsetospeed (&options, myBaud) ;

options.c_cflag |= (CLOCAL | CREAD) ;
options.c_cflag &= ~PARENB ;
options.c_cflag &= ~CSTOPB ;
options.c_cflag &= ~CSIZE ;
options.c_cflag |= CS8 ;
options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG) ;
options.c_oflag &= ~OPOST ;
options.c_cc [VMIN]  =   0 ;
options.c_cc [VTIME] = 100 ;	// Ten seconds (100 deciseconds)

tcsetattr (fd, TCSANOW | TCSAFLUSH, &options) ;

ioctl (fd, TIOCMGET, &status);

status |= TIOCM_DTR ;
status |= TIOCM_RTS ;

ioctl (fd, TIOCMSET, &status);

usleep (10000) ;	// 10mS

this->fd = fd;
this->device = device;

return fd ;
}


void serial::serialFlush ()
{
tcflush (fd, TCIOFLUSH) ;
}


void serial::serialClose ()
{
close (fd) ;
}


void serial::serialPutchar (unsigned char c)
{
write (fd, &c, 1) ;
}


void serial::serialPuts (char *s)
{
write (fd, s, strlen (s)) ;
}


void serial::serialPrintf (char *message, ...)
{
va_list argp ;
char buffer [1024] ;

va_start (argp, message) ;
vsnprintf (buffer, 1023, message, argp) ;
va_end (argp) ;

serialPuts (buffer) ;
}


int serial::serialDataAvail ()
{
int result ;


if (ioctl (fd, FIONREAD, &result) == -1)
		return -1 ;

return result ;
}


int serial::serialGetchar ()
{
uint8_t x ;

if (read (fd, &x, 1) != 1)
		return -1 ;

return ((int)x) & 0xFF ;
}


