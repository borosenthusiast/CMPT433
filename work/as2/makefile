default: sorter.c
	arm-linux-gnueabihf-gcc -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -pthread sorter.c bubble.c network.c threadmgr.c bbg.c -o $(HOME)/cmpt433/public/myApps/sorter
pctest: sorter.c
	gcc -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -pthread sorter.c bubble.c network.c threadmgr.c bbg.c -o $(HOME)/cmpt433/work/as2/as2test/sorter