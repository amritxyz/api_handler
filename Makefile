CC = cc
CFLAGS = -Wall -Wextra -std=c99 -lsqlite3 -lmicrohttpd
SRC = src/main.c
OUT = main.o

all:
	${CC} ${CFLAGS} ${SRC} -o ${OUT}
run:
	./${OUT}
clean:
	rm -f ${OUT}
