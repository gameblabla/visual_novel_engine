CC = clang

CFLAGS = -Weverything -Wno-switch-enum -Werror -O0 -g -DLOG_FPS -std=gnu99 -I/usr/include/SDL2
LDFLAGS = -lc -lgcc -lSDL2 -lSDL2_mixer -lSDL2_image -lSDL2_ttf -lm

OUTPUT = example.elf

SOURCES = main.c SGL.c array.c
OBJS = ${SOURCES:.c=.o}

${OUTPUT}:${OBJS}
	${CC} ${CFLAGS} -o ${OUTPUT} ${OBJS} ${LDFLAGS}
	
clean:
	rm *.o
	rm ${OUTPUT}
