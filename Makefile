# Makefile v0.8 Thu Jun 24 19:28:08 CEST 1999 for mepl
prefix = /usr
bindir = ${prefix}/bin
initdir = /etc/rc.d/init.d
initdir2 = /etc/init.d
mandir = ${prefix}/man
man7dir = $(mandir)/man7
CONFIGFILE = /etc/mepl.conf
INSTALL = /usr/bin/install -c
#Directory for init scripts
#  initdir is for RedHat and Suse
#  initdir2 is for Debian
initdir = /etc/rc.d/init.d
initdir2 = /etc/init.d

CC	= gcc
CFLAGS	= -DMEPLCONFIG=\"$(CONFIGFILE)\"

en:	mepl.c
	@echo "Type \"make de\" or \"make installde\" for german version."
	@echo "Taper \"make fr\" ou \"make installfr\" pour la version française."
	@echo "Scrivi \"make it\" or \"make installit\" per la versione italiana."
	$(CC) $(CFLAGS) -o mepl mepl.c 

de:	mepl.c
	@echo "Type \"make en\" or \"make installen\" for english version."
	$(CC) $(CFLAGS) -o mepl mepl.c -DGER

fr:	mepl.c
	@echo "Type \"make en\" or \"make installen\" for english version."
	@echo "mepl version fran¼aise."
	$(CC) $(CFLAGS) -o mepl mepl.c -DFR

it:     mepl.c
	@echo "Type \"make en\" or \"make installen\" for english version."
	@echo "mepl versione italiana."
	$(CC) $(CFLAGS) -o mepl mepl.c -DIT

updconf: updconf.c
	$(CC) $(CFLAGS) -o updconf updconf.c 
	./updconf
	rm updconf

man:
	@$(INSTALL) -m 644 mepl.en $(man7dir)/mepl.7
	@gzip -f $(man7dir)/mepl.7

deman:
	@$(INSTALL) -m 644 mepl.de $(man7dir)/mepl.7
	@gzip -f $(man7dir)/mepl.7

frman:
	@$(INSTALL) -m 644 mepl.fr $(man7dir)/mepl.7
	@gzip -f $(man7dir)/mepl.7

inst:
	@$(INSTALL) mepl $(bindir)/mepl
	@$(INSTALL) meplmail $(bindir)/meplmail
	@if test ! -e ${CONFIGFILE}; then $(INSTALL) -m 644 mepl.conf ${CONFIGFILE}; fi;
#	@test -d $(initdir) && $(INSTALL) meplinit $(initdir)/mepl || echo ""
#	@test -d $(initdir2) && $(INSTALL) meplinit $(initdir2)/mepl || echo ""

install: en updconf inst man

installde: de updconf inst deman

installfr: fr updconf inst frman

installit: it updconf inst man

all:	en

clean:
	@rm mepl
