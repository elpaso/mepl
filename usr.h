#define MODEMSPEAKER 1

int playvoicemessage(int,int,int); /* Play a voicemessage */


void sendstr(unsigned char *); /* Zeichenkette senden */
void clearmbuffer();
void clearkeyboard();


int playvoicemessage(int nr , int src , int fd) 
{
fd_set readfds;
struct timeval timeout;
char tmp[255];
if ((nr>255) || (nr<0)) return(-1);
sprintf(tmp,"%s%d",ATPLAYMESSAGE,nr);
clearmbuffer();
sendstr(tmp);
FD_ZERO(&readfds);  FD_SET(fd, &readfds); FD_SET(0, &readfds); 
timeout.tv_sec = 255; timeout.tv_usec = 0; /* 255 sec */
select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);
sendstr("\r");
clearmbuffer();
clearkeyboard();
return(0);
}

int recvoicemessage(int nr , int src , int fd) 
{
fd_set readfds;
struct timeval timeout;
char tmp[255];
if ((nr>1) || (nr<0)) return(-1);
if (nr==0) printf(RECMSG); else printf(RECMSG2);
sprintf(tmp,"%s%d",ATRECMESSAGE,nr);
clearmbuffer();
sendstr(tmp);
FD_ZERO(&readfds);  FD_SET(fd, &readfds); FD_SET(0, &readfds); 
timeout.tv_sec = 255; timeout.tv_usec = 0; /* 255 sec */
select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);
sendstr("\r");
clearmbuffer();
clearkeyboard();
return(0);
}

