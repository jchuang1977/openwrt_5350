#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>


char*pname;
int s[2],st[2];
static char lookup[] = { '0', '1', '2','3','4','5','6','7','8','9','A','B','C','D','E','F' };

void help(){
	fprintf(stderr,"Usage: %s [r|w] [command] data1 data2 ...\n",pname);
	return;
}

int setserial(int s,struct termios*cfg,int speed,int data,unsigned char parity,int stopb){
	cfmakeraw(cfg);
	switch(speed){
		case 50     : { cfsetispeed(cfg,B50)    ; cfsetospeed(cfg,B50)    ; break; }
		case 75     : { cfsetispeed(cfg,B75)    ; cfsetospeed(cfg,B75)    ; break; }
		case 110    : { cfsetispeed(cfg,B110)   ; cfsetospeed(cfg,B110)   ; break; }
		case 134    : { cfsetispeed(cfg,B134)   ; cfsetospeed(cfg,B134)   ; break; }
		case 150    : { cfsetispeed(cfg,B150)   ; cfsetospeed(cfg,B150)   ; break; }
		case 200    : { cfsetispeed(cfg,B200)   ; cfsetospeed(cfg,B200)   ; break; }
		case 300    : { cfsetispeed(cfg,B300)   ; cfsetospeed(cfg,B300)   ; break; }
		case 600    : { cfsetispeed(cfg,B600)   ; cfsetospeed(cfg,B600)   ; break; }
		case 1200   : { cfsetispeed(cfg,B1200)  ; cfsetospeed(cfg,B1200)  ; break; }
		case 1800   : { cfsetispeed(cfg,B1800)  ; cfsetospeed(cfg,B1800)  ; break; }
		case 2400   : { cfsetispeed(cfg,B2400)  ; cfsetospeed(cfg,B2400)  ; break; }
		case 4800   : { cfsetispeed(cfg,B4800)  ; cfsetospeed(cfg,B4800)  ; break; }
		case 9600   : { cfsetispeed(cfg,B9600)  ; cfsetospeed(cfg,B9600)  ; break; }
		case 19200  : { cfsetispeed(cfg,B19200) ; cfsetospeed(cfg,B19200) ; break; }
		case 38400  : { cfsetispeed(cfg,B38400) ; cfsetospeed(cfg,B38400) ; break; }
		case 57600  : { cfsetispeed(cfg,B57600) ; cfsetospeed(cfg,B57600) ; break; }
		case 115200 : { cfsetispeed(cfg,B115200); cfsetospeed(cfg,B115200); break; }
		case 230400 : { cfsetispeed(cfg,B230400); cfsetospeed(cfg,B230400); break; }
	}
	switch(parity|32){
		case 'n' : { cfg->c_cflag &= ~PARENB; break; }
		case 'e' : { cfg->c_cflag |= PARENB; cfg->c_cflag &= ~PARODD; break; }
		case 'o' : { cfg->c_cflag |= PARENB; cfg->c_cflag |= PARODD ; break; }
	}
	cfg->c_cflag &= ~CSIZE;
	switch(data){
		case 5 : { cfg->c_cflag |= CS5; break; }
		case 6 : { cfg->c_cflag |= CS6; break; }
		case 7 : { cfg->c_cflag |= CS7; break; }
		case 8 : { cfg->c_cflag |= CS8; break; }
	}
	if(stopb==1)cfg->c_cflag&=~CSTOPB;
	else cfg->c_cflag|=CSTOPB;
	return tcsetattr(s,TCSANOW,cfg);
}

void gotint(int x){
	if(st[0]&2){
		tcflush(s[0],TCIOFLUSH);
		close(s[0]);
	}
	if(st[1]&2){
		tcflush(s[1],TCIOFLUSH);
		close(s[1]);
	}
	printf("%s exiting.\n",pname);
	exit(1);
}

int main(int argc,char**argv){
   int i , ret;
   char CMD[17];
   char buf[2048];
   char readbuf[512];
   int fd;
   int checksum  = 0;
   struct termios cfg;
   char parity = 'n';
   
    pname=argv[0];
    
   if(argc  <  3){
		help();
		return 1;
	}

	
	

	if ( strchr(argv[1],'r' ) )
			printf("read command\n");
	else if ( strchr(argv[1],'w' ))
			printf("write command\n");
	else
	{
		help();
		return 1;	    
	}

    strcpy(CMD,argv[2]);
   printf("CMD=%s\n",CMD);
   
   memset(buf,0, sizeof(buf));
   buf[0] = 0x3A;
   strcat(buf+1,CMD);
   if(argc > 3)
   {
	   
	   for(i=3;i<argc;i++)
		{
			buf[strlen(buf)] = 0x2C;
			printf("argv[%d]=%s ",i,argv[i]);
			strcat(buf+strlen(buf),argv[i]);
		}
		printf("\n");
	}
  		
	
	fd = open("/dev/ttyS0", O_RDWR| O_NOCTTY | O_NDELAY);
	if (fd == -1) {
		printf("Can't open serial port\n");
		exit(0);
	}
	
		if(setserial(fd,&cfg, 38400, 8 ,parity,1)<0){
			fprintf(stderr,"%s: could not initialize device\n",pname);
			return 1;
		}
	
	for(i=1;i< strlen(buf) ; i++)
		checksum += buf[i];
	
	checksum = ~checksum + 1;
	checksum &= 0xFF ;
	printf("checksum=0x%x\n",checksum);
	
	 buf[i] = lookup[( checksum & 0xf0) >> 4];
	 buf[i+1] = lookup[ checksum & 0xf];
     buf[i+2] =  0xd ;
	 buf[i+3] =  0xa ;
	 
	printf("buf=%s(len=%d)\n",buf,strlen(buf));
	
	if ( strchr(argv[1],'r' ) )
	{
		memset(readbuf, 0 , sizeof(readbuf));
		ret = read(fd, readbuf, sizeof(readbuf));
		if (ret < 0) {
			perror("serial read");
		}
		if (ret > 0) 
		{
			readbuf[ret] = '\0';
			printf("read from rs232(%s)\n",readbuf);
		}
	}
	else if ( strchr(argv[1],'w' ))
	{
		ret = write(fd, buf, strlen(buf));
	}
	
	
	if(fd)	close(fd);

	return 0;
}

