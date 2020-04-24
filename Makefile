CC = gcc

CFLAGS = -O2 -fno-omit-frame-pointer -std=gnu99 -I/usr/include/SDL2 -DHOMEDIR -Isoil
LDFLAGS = -lc -lgcc -lSDL2 -lSDL2_mixer -lSDL2_image -lSDL2_ttf -lm

DEFINES = -Wall
OUTPUT = example.elf

SOURCES = main.c SGL.c
OBJS = ${SOURCES:.c=.o}

${OUTPUT}:${OBJS}
	${CC} -o ${OUTPUT} ${OBJS} ${CFLAGS} ${LDFLAGS} ${DEFINES} 
	
clean:
	rm *.o
	rm ${OUTPUT}
