PUBDIR = $(HOME)/cmpt433/public/myApps/beatbox
LFLAGS = -L$(HOME)/cmpt433/public/asound_lib_BBB
CFILES = beatbox.c audiogen.c audioMixer.c gpio.c network.c threadmgr.c accelerometer.c

all: beatbox wav node

default: all

beatbox: beatbox.c
	mkdir -p $(PUBDIR)
	arm-linux-gnueabihf-gcc -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -pthread $(CFILES) -o $(HOME)/cmpt433/public/myApps/beatbox/beatbox $(LFLAGS) -lasound 

wav:
	mkdir -p $(PUBDIR)/beatbox-wav-files/ 
	cp -R ./beatbox-wav-files/* $(PUBDIR)/beatbox-wav-files/ 
node:
	mkdir -p $(PUBDIR)/beatbox-server-copy/ 
	cp -R as3-server/* $(PUBDIR)/beatbox-server-copy/ 
	cd $(PUBDIR)/beatbox-server-copy/ && npm install
