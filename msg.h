/*  Version: @(#) mepl.c 0.42 Thu Jun 24 19:28:08 CEST 1999
    Autor: Frank Holtz <frank@hof-berlin.de>
    	   (C) 1998,99
    french translation : Georges Khaznadar <gekhajofour@netinfo.fr>

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

#ifdef GER
#define KONFDAT "Konfigurationsdatei %s fehlt!\n"
#define MODGESPRT "Das Modem ist gesperrt.\n"
#define LCKDAT "Die Lockdatei kann nicht angelegt werden.\n"
#define GERANTW "Das Gerät %s antwortet nicht.\n"
#define MODNUNTSZT "Das Modem wird nicht unterstützt.\n"
#define REPORTUHR "Letzter Reset der Modemuhr: %s\n"
#define REPORTTON "Tonnachrichten %d (%d Neu)\n"
#define REPORTFAX "Faxnachrichten %d (%d Neu)\n"
#define REPORTSPC "%d%% des Speichers (%dMB) belegt\n\n"
#define ATTR {"Alt         ","Neu(Checked)","            ","Neu         "}
#define NGEFUND "%s nicht gefuden\n"
#define MODBEREIT "Modem bereit.\n"
#define RECMSG "Die Ansage wird aufgenommen... Ende mit <cr>\n"
#define RECMSG2 "Die Speicher-voll-Ansage wird aufgenommen... Ende mit <cr>\n"
char *helptxt[]={
  "mepl v", VERSION, "\n",
  "Aufruf: mepl [optionen]\n",
  "-d     Startet den Download des Modemspeichers und bearbeitet die Nach-\n",
  "        richten, so wie in ", MEPLCONFIG, " angegeben.\n",
  "-f     Gibt einen Dateinamen zur Zwischenspeicherung des Modemspeichers\n",
  "        an. Ohne diesen Parameter wird die Datei /tmp/meplmem benutzt.\n",
  "-h     Dieser Text wird ausgegeben.\n",
  "-i     Initialisiert das Modem mit den in der Konfigurationsdatei\n",
  "        angegebenen Parametern. Der selbständige Modus wird aktiviert.\n",
  "-m     Nach dem Download wird der Modemspeicher gelöscht.\n",
  "-M     Löscht auch ohne Download den Speicher des Modems.\n",
  "-p     Beim Versenden/Speichern der Nachrichten wird eine Liste\n",
  "        ausgegeben.\n",
  "-P<nr> Spielt eine Tonnachricht ab (1-254 Nachricht; 0 Ansage;\n",
  "        255 Speicher voll-Ansage; n alle neuen Nachrichten;\n",
  "        a alle Nachrichten)\n",
  "-r     Die Nachrichten werden erneut aus der Datei /tmp/meplmem oder\n",
  "        der bei -f angegebenen erzeugt.\n",
  "-R<nr> Aufnahme der Ansage(nr=0) oder der Speicher voll Ansage(nr=1)\n",
  "-s     Ausgabe des Modemstatus.\n",
  "-t     In dem mit -t angegebenen Verzeichnis wird der Modemspeicher in\n",
  "        einer Datei, dessen Name sich aus Datum und Uhrzeit zusammen-\n",
  "        setzt, gespeichert. Das kann gut zur Archivierung der\n",
  "        Nachrichten eingesetzt werden.\n",
  "-v     Ausgabe der Programmversion.\n",
  NULL };

#elif defined FR
#define KONFDAT "fichier %s absent !\n"
#define MODGESPRT "Le modem est verrouillé.\n"
#define LCKDAT "Impossible de créer le fichier de verrouillage.\n"
#define GERANTW "Pas de réponse de %s.\n"
#define MODNUNTSZT "Ce modem n'est pas supporté.\n"
#define REPORTUHR "Dernière remise à l'heure du modem : %s\n"
#define REPORTTON "Messages vocaux : %d (%d nouveaux)\n"
#define REPORTFAX "Fax %d (%d nouveaux)\n"
#define REPORTSPC "%d%% de la mémoire (%dMB) utilisés.\n\n"
#define ATTR {"Ancien      ","New(Vérifié)","            ","Nouveau     "}
#define NGEFUND "%s pas trouvé\n"
#define MODBEREIT "Modem prêt.\n"
#define RECMSG "Le message d'accueil est enregistré... Tapez Entrée pour continuer.\n"
#define RECMSG2 "Le message de mémoire pleine est enregistré... Tapez Entrée pour continuer.\n"
char *helptxt[]={
  "mepl v", VERSION, "\n", 
  "Usage : mepl [options]\n",
  "-d     Récupère la mémoire du modem et traite les messages comme\n",
  "        spécifié dans ",MEPLCONFIG,"\n",
  "-f     Nom de fichier pour les sauvegardes temporaires de la mémoire\n",
  "        du modem. Par défaut, le fichier /tmp/meplmem est utilisé.\n",
  "-h     Affiche cet écran\n",
  "-i     Initialise le modem avec les paramètres du fichier de\n",
  "        configuration.Le mode autonome est activé.\n",
  "-m     La mémoire du modem est effacée après récupération.\n",
  "-M     Efface la mémoire du modem sans récupération.\n",
  "-p     Donne une liste de tous les fichiers récupérés pendant qu'ils\n",
  "        sont sauvés ou envoyés par e-mail.\n",
  "-P<no> Écoute d'un message vocal (1-254 message; 0 messge d'accueil;\n",
  "        255 message mémoire pleine; n tous les messages nouveaux;\n",
  "        a tous les messages)\n",
  "-r     Lit les messages du fichier /tmp/meplmem ou du fichier\n",
  "        définit par le paramètre -f.\n",
  "-R<nr> Chargement du message d'accueil (nr=0) ou du message de mémoire pleine (nr=1)\n",
  "-s     Donne le statut du modem.\n",
  "-t     Sauve la mémoire du modem dans un répertoire donné après -t,\n",
  "        dans un fichier dont le nom contient la date et l'heure.\n",
  "        On peut l'utiiser pour bien gérer une archive des messages.\n",
  "-v     Affiche la version du programme\n",
  NULL};
#elif defined IT
#define KONFDAT "manca %s\n"
#define MODGESPRT "Il device del modem e' bloccato.\n"
#define LCKDAT "Non posso creare il file di lock.\n"
#define GERANTW "Nessuna risposta da %s\n"
#define MODNUNTSZT "Questo modem non e' supportato\n"
#define REPORTUHR "Ultimo reset dell'orologio del modem: %s\n"
#define REPORTTON "Messaggi vocali %d (%d Nuovi)\n"
#define REPORTFAX "Fax %d (%d Nuovi)\n"
#define REPORTSPC "%d%% di memoria (%dMB) usati\n\n"
#define ATTR {"Vecchi      ","Nuovi(Controllati)","            ","Nuovi       "}
#define NGEFUND "%s non trovati\n"
#define MODBEREIT "Modem pronto.\n"
#define RECMSG "Il messaggio della segreteria e' stato registrato... Termina con <cr>\n"
#define RECMSG2 "Il messaggio di memoria piena e' stato registrato... Termina con <cr>\n"
char *helptxt[]={
  "mepl v", VERSION, "\n",
  "Usage: mepl [opzioni]\n",
  "-d     Scarica la memoria del modem e gestisci i messaggi come\n",
  "        configurato in ",MEPLCONFIG, "\n"
  "-f     Nome del file dove salvare la memoria del modem. Default e' /tmp/meplmem\n",
  "-h     Mostra questo messaggio.\n",
  "-i     Inizializza il modem\n",
  "-m     Cancella la memoria dopo il download (-d)\n",
  "-M     Cancella la memoria senza scaricarla\n",
  "-p     Stampa la lista di tutti i messaggi salvati/spediti durante lo scaricamento\n",
  "-P<no> Riproduci un messaggio (1-254 messaggio;0 segreteria;\n",
  "        255 memoria piena; n tutti i messaggi nuovi; a tutti i messaggi)\n",
  "-r     Leggi il messaggio di /tmp/meplmem o del file definito con -f\n",
  "-R<no> Registra il messaggio di segreteria(no=0) o di memoria piena(no=1)\n",
  "-s     Controlla lo stato del modem.\n",
  "-t     La memoria del modem viene salvata in un file nella directory\n",
  "       indicata da -t. Il nome del file consiste in data e ora.\n",
  "-v     Mostra versione del programma\n",
  NULL};
#else
#define KONFDAT "missing %s\n"
#define MODGESPRT "Device modem is locked.\n"
#define LCKDAT "Cannot create lockfile. Sorry.\n"
#define GERANTW "No answer from %s\n"
#define MODNUNTSZT "This modem is not supported\n"
#define REPORTUHR "Last reset of Modem clock: %s\n"
#define REPORTTON "Voice messages %d (%d New)\n"
#define REPORTFAX "Fax messages %d (%d New)\n"
#define REPORTSPC "%d%% of memory (%dMB) used\n\n"
#define ATTR {"Old         ","New(Checked)","            ","New         "}
#define NGEFUND "%s not found\n"
#define MODBEREIT "Modem ready.\n"
#define RECMSG "The greeting message is recorded... End with <cr>\n"
#define RECMSG2 "The memory-full message is recorded... End with <cr>\n"
char *helptxt[]={
  "mepl v", VERSION, "\n",
  "Usage: mepl [options]\n",
  "-d     Download the modem memory and handle the messages as\n",
  "        configured in ",MEPLCONFIG, "\n"
  "-f     Filename to save the modem memory to. Default is /tmp/meplmem\n",
  "-h     Display this text.\n",
  "-i     Initialize the modem\n",
  "-m     Delete memory after download (-d)\n",
  "-M     Delete memory without download\n",
  "-p     Print a list of all saved/mailed messages while downloading\n",
  "-P<no> Play a voicemessage (1-254 voicemessage;0 greetingmessage;\n",
  "        255 memory full message; n all new messages; a all messages)\n",
  "-r     Read the messages of /tmp/meplmem or the file defined with -f\n",
  "-R<no> Record the greeting(no=0) or memory full(no=1) message\n",
  "-s     View the modem status.\n",
  "-t     The modem memory gets saved in a file within the by directory\n",
  "       indicated by -t. The filename consists of date and time.\n",
  "-v     Display programm version\n",
  NULL};
#endif
