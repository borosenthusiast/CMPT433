default: sorter.c
	arm-linux-gnueabihf-gcc -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -pthread sorter.c bubble.c network.c threadmgr.c bbg.c -o $(HOME)/cmpt433/public/myApps/sorter

noworky: noworky.c
	arm-linux-gnueabihf-gcc -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror noworky.c -o $(HOME)/cmpt433/public/myApps/noworky

all: default noworky