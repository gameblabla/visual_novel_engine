CC = gcc

CFLAGS = -Wall -Wno-switch-enum -O2 -DLOG_FPS -std=gnu99 -I/usr/include/SDL2
LDFLAGS = -lc -lgcc -lSDL2 -lSDL2_mixer -lSDL2_image -lSDL2_ttf -lm

OUTPUT = example.elf

SOURCES = src/SGL.c src/array.c src/main.c src/text_vn.c
OBJS = ${SOURCES:.c=.o}

${OUTPUT}:${OBJS}
	${CC} ${CFLAGS} -o ${OUTPUT} ${OBJS} ${LDFLAGS}
	
clean:
	rm src/*.o
	rm ${OUTPUT}
