/*  Version: @(#) updconf.c 0.1 for mepl 0.40
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
                                                        
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#ifndef MEPLCONFIG
 #define MEPLCONFIG "/etc/mepl.conf"
#endif

main()
{
 FILE *datei;
 unsigned char line[2048],tmp[255];
 int i;

for(i=0;i<sizeof(tmp);i++) tmp[i]=0;
printf("Checking %s...",MEPLCONFIG);

/* Konfiguration einlesen */
if ((datei=fopen(MEPLCONFIG,"r+"))==NULL) 
    { 
	printf("\nmissing %s\n",MEPLCONFIG);
	return(0);
    } 
while ((fgets(line,sizeof(line),datei))!=(char*)0) 
          if (line[1]<255) tmp[line[0]]++;

/* R suchen */
if (tmp['R']==0) { printf(" adding R... "); fprintf(datei,"\n#Reset Modemclock after x days (0=everey time when run mepl -i)\nR200\n"); }

printf(" OK\n");
fclose(datei);
return(0);
} /*main*/