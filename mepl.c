#define VERSION "0.45 Don Nov 25 19:13:58 CET 1999"
/*  Version: @(#) mepl.c 0.45
    Autor: Frank Holtz <frank@hof-berlin.de>
    	   (C) 1998,99

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#include <fcntl.h>
// #include <signal.h> #If you have problems to compile remove the "//"
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

// File with the English, German and French messages.
 #include "msg.h"

// AT-commands for the USR and 3COM messagemodems
 #include "usrat.h"
// Modem-specific procedures
 #include "usr.h"
/*When you try to insert support for another modem, plase insert the 
  AT-commands and the modem-specific procedures to extra files. 
*/

#define OPT_INIT 1
#define OPT_DOWNLOAD 2
#define OPT_STATUS 4
#define OPT_RESTORE 8
#define OPT_PRINTINFO 16
#define OPT_DELMEM 32
#define OPT_DELMEM2 64
#define OPT_PLAY 128
#define OPT_REC 256

/* DEBUGS=Sendekontrolle DEBUGG=Empfangskontrolle */
//#define DEBUGS
//#define DEBUGE

/* REMOVEPAGE(delete 0x10 0x03) */
//#define REMOVEPAGE

/* Gibt Punkte bei Download aus */
//#define DISPMEM

#define BAUDRATE B38400

#ifndef MEPLCONFIG
 #define MEPLCONFIG "/etc/mepl.conf"
#endif

unsigned char datentyp[4][10]={"?    ","FAX  ","Voice","Data "};
#ifdef IT
unsigned char attr[6][18]=ATTR;
#else
unsigned char attr[6][13]=ATTR;
#endif

/* Prozeduren zur Modemkommunikation */
void sendch(unsigned char); /* Zeichen senden */
int getch(); /* Zeichen lesen */
void sendstr(unsigned char *); /* Zeichenkette senden */
unsigned char *getstr(); /* Zeichenkette lesen */
int sendcmd(unsigned char *); /* Kommando senden und auf ergebnis warten */
void clearmbuffer();
time_t readmtime(int);
void clearkeyboard();

/* Variablen für die Modemkommunikation */
int fd;

int help();
void exec(char *,char *,char *,char *);
void faxwrite(int, unsigned char);
void faxclear(int);

int val(unsigned char *);
int hex(unsigned char *);
int fget(int);

main(int argc, char *argv[])
{
 int   i,z,j,k,error,errz,ic,resin,resout,n,pid,opt;
 struct termios oldtio,newtio;
 unsigned char c;
 FILE *datei;
 unsigned char line[2048],tmp[255],device[255],voice[255],faxid[255],mailscript[255];
 unsigned char klingelzeichen[255],fax[255],code[255],mlen[255],resdat[255];
 unsigned char block[32768],s2[255],s3[255],s4[255],savdir[255],typ=0,pages=0;
 unsigned char *id,*s,lockfile[255];
 char options[]="df:himMpP:rR:st:v";
 time_t zeit,zeit2;
 struct tm *tmstr;
 long len;
 int option=0, resetclk=200, playmessagenr=-1, recmessagenr=-1;

/* Anfangswerte setzten */
for (i=0;i<sizeof(faxid);i++) lockfile[i]=savdir[i]=mailscript[i]=code[i]=device[i]=voice[i]=fax[i]=klingelzeichen[i]=faxid[i]=0;
strcpy(device,"/dev/modem"); strcpy(voice,"P");
strcpy(fax,"Mroot");strcpy(code," "); strcpy(resdat,"/tmp/meplmem");
strcpy(klingelzeichen,"4"); strcpy(savdir,"/tmp"); strcpy(lockfile,"/var/lock/LCK..modem");
zeit=time((time_t *)0);

/* read configfile einlesen */
if ((datei=fopen(MEPLCONFIG,"r"))==NULL) 
    { 
 	printf(KONFDAT,MEPLCONFIG);
	return(1);
    } 
   else {
    while ((fgets(line,sizeof(line),datei))!=(char*)0) {
    for (i=0;i<strlen(line);i++) if (line[i]<32) line[i]=0;
    if (line[0]=='c') strncpy(code,&line[1],strlen(&line[1]));
    if (line[0]=='d') strncpy(device,&line[1],strlen(&line[1]));
    if (line[0]=='f') strncpy(fax,&line[1],strlen(&line[1]));
    if (line[0]=='i') { for (i=0;i<20;i++) faxid[i]=32;
    		        if (strlen(&line[1])<20) 
    		           strcpy(&faxid[20-strlen(&line[1])],&line[1]); 
    		        else
    		           strncpy(faxid,&line[1],strlen(&line[1]));
    		      } 
    if (line[0]=='l') strncpy(lockfile,&line[1],strlen(&line[1]));
    if (line[0]=='m') strncpy(mailscript,&line[1],strlen(&line[1]));
    if (line[0]=='p') strncpy(savdir,&line[1],strlen(&line[1]));
    if (line[0]=='r') strncpy(klingelzeichen,&line[1],strlen(&line[1]));
    if (line[0]=='R') { resetclk=atoi(&line[1]); if (resetclk>254) resetclk=255; }
    if (line[0]=='s') strncpy(mlen,&line[1],strlen(&line[1]));
    if (line[0]=='v') strncpy(voice,&line[1],strlen(&line[1]));
    } 
   fclose(datei);
   }

/* commandline options */
while ((opt = getopt(argc, argv, options)) != -1)
    {
      switch (opt)
        {
	  case 'd': option=option | OPT_DOWNLOAD | OPT_RESTORE; break;
	  case 'f': if (optarg) strcpy(resdat,optarg); else return(help());
	            break;
	  case 'i': option=option | OPT_INIT; break;
	  case 'm': option=option | OPT_DELMEM; break;
	  case 'M': option=option | OPT_DELMEM2; break;
	  case 'p': option=option | OPT_PRINTINFO; break;
	  case 'P': if (optarg) 
	  		{ 
	  		 switch (optarg[0])
	  		  {
	  		   case 'n' : playmessagenr=256; break;
	  		   case 'a' : playmessagenr=257; break;
	  		   default : playmessagenr=atoi(optarg); 
	  		  }
	  		  option=option | OPT_PLAY;
	  		} else return(help());
	            break;
	  case 'r': option=option | OPT_RESTORE; break;
	  case 'R': if (optarg) recmessagenr=atoi(optarg); 
	  		else return(help());
	  	    option=option | OPT_REC;
	            break;
	  case 's': option=option | OPT_STATUS; break;
	  case 't': strncpy(line,ctime(&zeit),24);
	  	    for (j=0;j<24;j++) if (line[j]<33) line[j]='_';
	  	    line[24]=0;
		     if (optarg) strcpy(s2,optarg); else return(help());
	            sprintf(resdat,"%s/%s",s2,line);
	            break;
	  case 'v': printf("mepl v%s\n",VERSION); return(0); break;
        case 'h': default: return(help());
        }
    }
if ((option & (OPT_INIT+OPT_DOWNLOAD+OPT_STATUS+OPT_RESTORE+OPT_DELMEM2+OPT_PLAY+OPT_REC))==0) { return(help()); }

/* Nur wenn etwas mit dem modem gemacht werden soll, datei oeffnen */
if ((option & (OPT_INIT+OPT_DOWNLOAD+OPT_STATUS+OPT_RESTORE+OPT_PLAY+OPT_REC))!=OPT_RESTORE) {
z=0; 
do {
 /* Nachsehen, ob modem gesperrt ist */
 if ((fd = open(lockfile, O_RDONLY)) > 1) {
	n = read(fd, line, 127); close(fd);
	if (n > 0) { 
	  pid = -1;
	  if (n == 4)
	  /* Kermit-style lockfile. */
	  pid = *(int *)line;
	else {
	  /* Ascii lockfile. */
	  line[n] = 0;
	  sscanf(line, "%d", &pid);
	}
	if (pid > 0 && kill((pid_t)pid, 0) < 0)
	   {
	   //fprintf(stderr,"Lockfile is stale. Overriding it..\n");
	   //sleep(1);
	   unlink(lockfile);
	} else
	n = 0;
  }
 }
if ((n == 0) && (fd>0)) { z++; sleep(1); } else z=5;
} while (z<5); /* versuche 5 mal, ob modem verfügbar */
if ((z>4) && (n==0)) {
     fprintf(stderr,MODGESPRT);
    return(-1);
}
/* Create lockfile*/
#ifdef _COH3
	if ((fd = creat(lockfile, 0666)) < 0) {
#else
  	if ((fd = open(lockfile, O_WRONLY | O_CREAT | O_EXCL, 0666)) < 0) {
#endif
 	fprintf(stderr,LCKDAT);
	return(-1);
  	}
 snprintf(line, sizeof(line), "%10ld\n", (long)getpid());
 write(fd, line, strlen(line));
 close(fd);


/* modem öffnen und schnittstelle initialisieren*/
 fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
 if (fd <0) {perror(device); exit(-1); }

 tcgetattr(fd,&oldtio);
 newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
 newtio.c_iflag = IGNPAR;
 newtio.c_oflag = 0;
 newtio.c_lflag = 0;
 newtio.c_cc[VMIN]=1;
 newtio.c_cc[VTIME]=0;
 tcflush(fd, TCIFLUSH);
 tcsetattr(fd,TCSANOW,&newtio);

/* Testen, ob ein Modem angeschlossen ist, wenn nicht, dann ende */
sendstr("AT"); if (getch()<0) 
 { 
  printf(GERANTW,device); 
 kill(getpid(),2); exit(-1); 
 }
getstr();

/****************************************************************************
 Dieser Programmteil ist für die eigentlich Modembehandlung zuständig
****************************************************************************/
/* Ist das richtige Modem angeschlossen/Initialisierung */
if ((sendcmd(ATRESET)<0) || (sendcmd(ATINITMESSAGE)<0))
   { 
   printf(MODNUNTSZT);
   exit(-1); 
   }
//usleep(10000); /* damit wird ein übertragungsfehler vermieden, der */
//sendcmd("AT"); /* anscheinend nach der initialisierung gelegentlich
//                  auftritt. */
} /* ende !restore */

/* Status ausgeben -s */
if ((option & OPT_STATUS)!=0) {
zeit=readmtime(-1);
if (zeit>0)
     {
     sprintf(tmp,"%s",ctime(&zeit));
     for (i=0;i<strlen(tmp);i++) if (tmp[i]<' ') tmp[i]=0;
     } else sprintf(tmp,"?");

sendstr(ATSTATUSREPORT);
strcpy(line,getstr()); getstr(); /* OK einlesen */
printf(REPORTUHR,tmp);
printf(REPORTTON,val(line+8),val(line+12));
printf(REPORTFAX,val(line+16),val(line+20));
printf(REPORTSPC,val(line+4),val(line));

for (i=1;i<64;i++) 
    {
    sprintf(line,"%s%d",ATMSGSTAT,i); sendstr(line); strcpy(line,getstr());
    if (strstr(line,"ERROR")!=NULL) break;
    if (strlen(line)>20)
        {
         printf("%2d %s %3d",i,datentyp[val(line+4)],val(line+8));
         if (val(line+4)==1) printf("p"); else printf("s");
         if (val(line+20)<255) 
         {
          zeit2=zeit+(val(line+20)*86400)
         	   +(val(line+24)*3600)
         	   +(val(line+28)*60)
         	   +val(line+32);
          strcpy(tmp,ctime(&zeit2)); tmp[16]=0;
          printf(" %s ",tmp);
         } else printf("                  ");
         strcpy(tmp,line+32); tmp[20]=0;
         printf("\"%s\"",tmp);
         printf(" %s\n",attr[val(line+12) & 3]); 
        } 
    }
printf("\n"); fflush(stdout);
}

/* Play Voicemessages -P */
if ((option & OPT_PLAY)!=0) {
if (playmessagenr<256) playvoicemessage(playmessagenr,MODEMSPEAKER,fd); /* Play only one message */
else
 for (i=1;i<64;i++) 
    {
    sprintf(line,"%s%d",ATMSGSTAT,i); sendstr(line); 
    strcpy(line,getstr());
    if (strstr(line,"ERROR")!=NULL) break;
    if ((strlen(line)>20) && (val(line+4)==2))
        switch (playmessagenr)
        {
        case 256: if ((val(line+12) & 1)==1) playvoicemessage(i,MODEMSPEAKER,fd); break;
        case 257: playvoicemessage(i,MODEMSPEAKER,fd); break;
        defualt: playvoicemessage(i,MODEMSPEAKER,fd);
        } 
    }
printf("\n"); fflush(stdout);
}


/* Record voice message -R */
if ((option & OPT_REC)!=0) {
 recvoicemessage(recmessagenr,MODEMSPEAKER,fd);
}


/* Download */
if ((option & OPT_DOWNLOAD)!=0) {
sendstr(ATSTATUSREPORT);
strcpy(line,getstr()); getstr(); /* OK einlesen */
z=val(line+8)+val(line+16);
if (z>0) {
 /* Speicher des modems auslesen und abspeichern */
 errz=0; 
 do {
 error=0;
 resout=open(resdat,O_WRONLY | O_TRUNC | O_CREAT); /* In Datei zwischenspeichern */
 sendstr(ATSENDMEM);
 line[2]=0;
 /* Erstes Zeichen suchen */
 clearmbuffer(); do { c=getch(); } while(c<32);
 do {
  line[0]=c; line[1]=getch(); /* Checksumme */
  i=hex(line); j=0;
  /* Block lesen */
  for (k=0;((k<32768) && (ic>-200));k++) j=j+(block[k]=getch()); 
  write(resout,&block,k);
  if (i!=(j & 255)) error++;
//  if (error>0) printf("Checksumerror %x<>%x\n",i,j & 255);
     #ifdef DISPMEM
     else 
     { printf(".");fflush(stdout);}
     #endif 
  do { c=getch(); } while(c<32);
  } while (c!='O');
 /* Resetzeit der modemuhr ermitteln */
  zeit=readmtime(-1);
 /* Zeit in die Datei schreiben */
 write(resout,&zeit,sizeof(time_t));
 close(resout);
 errz++;
 } while ((error>0) && (errz<3)); /* Drei Versuche, wenn Fehler aufgetreten sind*/
// printf("Error: %d\n",error);
if ((error==0) && ((option & OPT_DELMEM)>0))
   { sendcmd(ATZ); sendcmd(ATINITMESSAGE); sendcmd(ATDELMEM); }
} /* Ende z>0 (nachrichten da) */
else option=option-OPT_RESTORE; /* Keine neuen Nachrichten also kein restore */
} /* Ende download */


/* Restore -r oder nach download -d */
if ((option & OPT_RESTORE )!=0)  {
 if ((resin=open(resdat,O_RDONLY))==0) 
 {
 printf(NGEFUND,resdat); 
 exit(-2); 
 }
 /* Uhrzeit einlesen */
 lseek(resin,-sizeof(time_t),SEEK_END);
 read(resin,&zeit,sizeof(time_t));
 lseek(resin,0,SEEK_SET);
 while (((i=fget(resin))!=EOF) && (i<255))
 {
  int nr;
  time_t msgtime;
  long nextmsg;
  unsigned char msgid[21];
  nr=i; /* Index */
  read(resin,&block,33);
  /* Nachrichtenzeit */
  msgtime=zeit+(block[4]*86400)+(block[5]*3600)+(block[6]*60); 
  /* Faxid */
  for (i=0;i<20;i++) msgid[i]=block[7+i]; msgid[20]=0;
  /* Position nächste Nachricht */
  nextmsg=((block[30]-4)*32768)+(block[31]*256)+(block[32]);
  /* Info */
  if ((option & OPT_PRINTINFO)!=0) {
  if ((msgtime-zeit)<22032001) { strncpy(s2,ctime(&msgtime),24); s2[24]=0; } 
     else strcpy(s2,"unknown time            ");
  switch (block[0])
  {
    case 1:  sprintf(line,"FAX   %3dp %s \"%s\"",block[1],s2,msgid); break;
    case 2:  sprintf(line,"Voice %3ds %s",block[1],s2); break;
    case 3:  sprintf(line,"Data      %s",s2); break;
    default: sprintf(line,"Unknown   %s",s2); 
  }
  printf("%2d: %s\n",nr,line); fflush(stdout);
  }
  /* Dateinamen generieren */
  if ((msgtime-zeit)<22032001) tmstr=localtime(&msgtime); 
     else { zeit2=time((time_t *)0); tmstr=localtime(&zeit2); }
  sprintf(line,"%s/%d.%d.%d_%d.%d_%d",savdir,tmstr->tm_mday,tmstr->tm_mon+1,tmstr->tm_year,tmstr->tm_hour,tmstr->tm_min,nr);
  /* Faxnummer anfügen */
  if (block[0]==1) 
    for (i=1;i<strlen(msgid);i++) 
      if ((msgid[i]=='+') || (((msgid[i]>='0') && (msgid[i]<='9')))) strncat(line,msgid+i,1);
  switch(block[0]) {
  case 1: strcat(line,".fax"); lseek(resin,32,SEEK_CUR); /* 32 byte auslassen, mach winsw auch*/ break;
  case 2: strcat(line,".gsm"); break;
  case 3: strcat(line,".dat"); break;
  }
  typ=block[0]; pages=block[1];
  /* Datei speichern */
   len=nextmsg-lseek(resin,0,SEEK_CUR);
   resout=open(line,O_WRONLY | O_CREAT | O_TRUNC,0644);
   n=32768;
   while (len>0) {
    if (len>32768) n=32768; else n=len;
    read(resin,&block,n); 
    for (i=0;i<n;i++) 
    {  
     if ((block[i]==0x10) && (block[i+1]==0x10) && (i<n)) i++;
     if (typ!=1) write(resout,&block[i],1); else faxwrite(resout,block[i]);
    }
    len=len-32768;
   }
   if (typ==1) faxclear(resout);
   close(resout);
   /* Mail oder nicht? */
  if (((typ==1) && (fax[0]=='M')) || ((typ==2) && (voice[0]=='M')))
     { /*mail schicken*/
       if ((msgtime-zeit)<22032001) { strncpy(s2,ctime(&msgtime),24); s2[24]=0; } 
            else s2[0]=0;
       s4[0]=0; for (i=1;i<strlen(msgid);i++) if (msgid[i]>32) strncat(s4,msgid+i,1);
       switch (typ) {
       case 1: sprintf(s3,"FAX from %s %dp %s",s4,pages,s2); 
               strcpy(s2,fax+1); 
               break;
       case 2: sprintf(s3,"Voice %ds %s",pages,s2); 
       	       strcpy(s2,voice+1); 
       	       break;
       }
     exec(mailscript,s2,line,s3);
     remove(line); 
     }
  lseek(resin,nextmsg,SEEK_SET);
 }
 close(resin);
}


/* Speicher Löschen (OPT_DELMEM2 aber nicht OPT_DELMEM) */
if ((option & OPT_DELMEM2)>0)
   { sendcmd(ATZ); sendcmd(ATINITMESSAGE); sendcmd(ATDELMEM); }

/* Modem initialisieren -i */
if ((option & OPT_INIT )!=0)  {
 sendcmd(ATINIT);
 zeit=readmtime(resetclk); 
 /* Passwort oder nicht ? Fernabfrage ein/aus;Test ob wert schon gesetzt*/
 if (strlen(code)>1) {  sendstr(ATREADCODE); strcpy(line,getstr());
 			if (strcmp(line,code)!=0) 
 			   {
 			   sprintf(line,"%s%s",ATSETCODE,code);
 			   sendcmd(line);
 			   }
 			sprintf(line,"%s1",ATSTARTCODE);
 			sendcmd(line); }
    else { sprintf(line,"%s0",ATSTARTCODE); sendcmd(line); }
 sprintf(line,"%s%s",ATRING,klingelzeichen); sendcmd(line);

 sendstr(ATREADFAXID); strcpy(line,getstr());
 if (strstr(line,faxid)==NULL) 
 {
  sprintf(line,"%s\"%s\"",ATFAXID,faxid);  sendcmd(line);
 }
 sendstr(ATRDVOICELENGTH); strcpy(line,getstr()); sendcmd("");
 if (atoi(line)!=atoi(mlen)) 
 {
  sprintf(line,"%s%s",ATVIOCELENGTH,mlen); sendcmd(line);
 }
 sendcmd(ATWRITESETTINGS);
  printf(MODBEREIT);
}

/* alle systemänderungen rückgängig machen */
if ((option & (OPT_INIT+OPT_DOWNLOAD+OPT_STATUS+OPT_RESTORE))!=OPT_RESTORE) {
  tcsetattr(fd,TCSANOW,&oldtio);
  close(fd); unlink(lockfile);
 }
} /* Ende main */

void sendch(unsigned char c)
{
fd_set writefds;
struct timeval timeout;
int err;
FD_ZERO(&writefds);  FD_SET(fd, &writefds);
timeout.tv_sec = 0; timeout.tv_usec = 500000; /* 500 msec */
err = select(FD_SETSIZE, NULL, &writefds, NULL, &timeout);
if (((err == -1) || (!err))) return;
write(fd,&c,1);
#ifdef DEBUGS
 if ((c>9) && (c<128)) printf("%c",c);  fflush(stdout);
#endif 
}

int getch()
{
fd_set readfds;
struct timeval timeout;
unsigned char c;
int z=0, err;
FD_ZERO(&readfds);  FD_SET(fd, &readfds);
timeout.tv_sec = 0; timeout.tv_usec = 4000000; /* 4 sec */
err = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);
if (((err == -1) || (!err))) return(-200);
z=read(fd,&c,1);
#ifdef DEBUGE
 if ((c>9) && (c<128)) printf("%c",c);  fflush(stdout);
#endif 
return(c);
}

void clearmbuffer()
{
char puffer[1024];
fd_set readfds;
struct timeval timeout;
int err,i;
FD_ZERO(&readfds);  FD_SET(fd, &readfds);
timeout.tv_sec = 0; timeout.tv_usec = 100000; /* 100 msec */
err = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);
if (((err == -1) || (!err))) return;
err=read(fd,&puffer,1024);
#ifdef DEBUGE
 printf("<");
 for (i=0;i<err;i++) 
     if (puffer[i]>31) printf("%c",puffer[i]);  fflush(stdout);
 printf(">\n");
#endif 
}

void sendstr(unsigned char *s)
{
int i;
clearmbuffer();
for (i=0;i<strlen(s);i++) sendch(s[i]);
sendch(*"\r");
}


unsigned char getstrstr[1024];
unsigned char *getstr()
{
unsigned int c,i,w=0;
for (i=0;i<sizeof(getstrstr);i++) getstrstr[i]=0;
i=0;
do {
c=getch();
if (c<0) { if ((c=getch())<-199) return(NULL); }
if (c>31) { w=1; getstrstr[i]=c; if (i<sizeof(getstrstr)) i++; } 
	  else if (w==1) w=2;
} while (w<2);
return(getstrstr);
}

int sendcmd(unsigned char *s)
{
char *ret;
clearmbuffer();
sendstr(s);
do {
 ret=getstr(); 
 if (ret==NULL) return(-1);
} while ((strstr(ret,"OK")==NULL) && (strstr(ret,"ERROR")==NULL));
if (strstr(ret,"OK")!=NULL) return(1);
return(-1); 
}

int help()
{
int i;
for (i=0;helptxt[i]!=NULL;i++) printf("%s",helptxt[i]);
return(-1);
}

int val(unsigned char *s) { 
int wert,i; 
if (strlen(s)==0) return(-1); wert=0;
for (i=0;i<3;i++) if ((s[i]>='0') && (s[i]<='9')) {
	wert=wert*10;
	wert=wert+(s[i]-48); 
	} else return(-1);
return(wert);
}

int hex(unsigned char *s) { 
int wert,i; 
if (strlen(s)==0) return(-1); wert=0;
for (i=0;i<2;i++) {
if ((s[i]>='0') && (s[i]<='9')) {
	wert=wert*16;
	wert=wert+(s[i]-48); 
	} 
if ((s[i]>='A') && (s[i]<='F')) {
	wert=wert*16;
	wert=wert+(s[i]-55);
	} 
}
return(wert);
}

int fget(int fc)
{
char c;
int z;
z=read(fc,&c,1);
if (z==1) return(c);
return(EOF);
}

void exec(char *name,char* parameter,char* parameter2, char* parameter3)
{
int pid,statuse;
fflush(stdout);
pid=fork();
if (pid==0) { execlp(name,name,parameter,parameter2,parameter3,(char *)0);
 		  #ifdef GER
		  printf("%s nicht gefuden\n",name); 
		  #else
		  #ifdef FRA
                  printf("%s pas trouvé\n",name);
                  #else
                  #ifdef IT
                  printf("%s non trovato\n",name);
                  #else
		  printf("%s not found\n",name);
                  #endif
                  #endif
		  #endif
              exit(-1);
              }
wait(&statuse);
fflush(stdout);
}

/* Ich übernehme absolut keine Garantie, das dies richtig funktioniert.
   Um aus dem, was das Modem von sich gibt eine lesbare Datei zu machen, 
   habe ich die Ausgabe des Windows-Programmes mit der des Modems verglichen.
   Ich habe nach mustern gesucht, um dort einige Bytes zu entfernen. So ergab
   sich bei mir eine exakt gleiche Ausgabe. Wie die Sache mit anderen 
   Faxgeräten/Kompresionen aussieht konnte ich nur bedingt testen. */
unsigned char faxpuffer[1024];
int faxpos=0;
/* Faxdaten nach 00 10 01 durchsuchen und 32 Byte abtrennen. Innerhalb der 
   nächsten Bytes 10 03 entfernen */
void faxwrite(int fd, unsigned char b)
{
int i,j;
#ifdef REMOVEPAGE
static char chg=0;
#endif 
faxpuffer[faxpos]=b; faxpos++;
if (faxpos==1024) 
   { 
   for (i=0;i<512;i++) 
       {
       /* Bei 00 10 01 nachfolgende 32 Byte entfernen */
       if ((faxpuffer[i]==0) && (faxpuffer[i+1]==0x10) && (faxpuffer[i+2]==0x01)
          && (faxpuffer[i+3]==0) && (faxpuffer[i+4]==0x10) && (faxpuffer[i+5]==0x01) )
          {
          faxpos=faxpos-32;
          for (j=i+3;j<faxpos;j++) faxpuffer[j]=faxpuffer[j+32];
#ifdef REMOVEPAGE
          chg++;
#endif 
          } 
       /* Bei 40 04 00 nachfolgende 33 Byte entfernen */
       if ((faxpuffer[i]==0x40) && (faxpuffer[i+1]==0x04) && (faxpuffer[i+2]==0x00)
          && (faxpuffer[i+3]==0x40) && (faxpuffer[i+4]==0x04) && (faxpuffer[i+5]==0x00) )
          {
          faxpos=faxpos-33;
          for (j=i+3;j<faxpos;j++) faxpuffer[j]=faxpuffer[j+33];
#ifdef REMOVEPAGE
          chg++;
#endif 
          } 
       /* Bei 10 03 2 Byte entfernen */
#ifdef REMOVEPAGE
       if ((chg>0) 
          && (faxpuffer[i  ]==0x10) && (faxpuffer[i+1]==0x03))
          {
          faxpos=faxpos-2;
          for (j=i;j<faxpos;j++) faxpuffer[j]=faxpuffer[j+2];
          chg=0;
          } 
#endif 
       }
   if (faxpos>512) 
      {
      write(fd,&faxpuffer,512);
      for (j=512;j<faxpos;j++) faxpuffer[j-512]=faxpuffer[j];
      faxpos=faxpos-512;
      } else { write(fd,&faxpuffer,faxpos);  faxpos=0; }
   }
}              

/* Rest des Faxes nach 00 10 01 00/00 40 04 00 abschneiden */
void faxclear(int fd)
{
int i;
for (i=0;i<faxpos;i++) 
       /* Bei 00 10 01 00 abschneiden */
       if ((faxpuffer[i]==0) && (faxpuffer[i+1]==0x10) && 
           (faxpuffer[i+2]==0x01) && (faxpuffer[i+3]==0) && 
           (faxpuffer[i+4]==0x10) && (faxpuffer[i+5]==0x01) ) faxpos=i;
       /* Bei 00 40 04 00 abschneiden */
       if ((faxpuffer[i]==0) && (faxpuffer[i+1]==0x40) && 
           (faxpuffer[i+2]==0x04) && (faxpuffer[i+3]==0)) faxpos=i;
faxpos=faxpos+4;           
write(fd,&faxpuffer,faxpos);
faxpos=0;
}

time_t readmtime(int days)
{
time_t zeit=time((time_t *)0);
char line[2048];
clearmbuffer(); sendstr(ATREADCLOCK); strcpy(line,getstr()); clearmbuffer();
if ((days>0) && (val(line)>=days)) sendcmd(ATRESETCLOCK);
if (val(line)==255) return(0);
else {
     zeit=zeit-(val(line)*86400); /* Tage */
     zeit=zeit-(val(line+4)*3600); /* Stunden */
     zeit=zeit-(val(line+8)*60); /* Minuten */
     zeit=zeit-(val(line+12)); /* Sekunden */
     }
return(zeit);
}

void clearkeyboard()
{
char puffer[1024];
fd_set readfds;
struct timeval timeout;
int err,i;
FD_ZERO(&readfds);  FD_SET(0, &readfds);
timeout.tv_sec = 0; timeout.tv_usec = 1; 
err = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);
if (((err == -1) || (!err))) return;
err=read(0,&puffer,sizeof(puffer));
}

