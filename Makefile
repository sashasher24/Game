CC = clang
CFLAG = -std=c11 -Wall -Wextra -Werror -Wpedantic
SDL2 = -I ./Frameworks/SDL2.framework/Versions/A/Headers -F ./Frameworks -framework SDL2
SDL2_IMAGE = -I ./Frameworks/SDL2_image.framework/Versions/A/Headers -F ./Frameworks/ -framework SDL2_image
NAME = endgame
all: install clean
install:
	cp src/*.c inc/*.h .
	${CC} ${CFLAG} *.c -o ${NAME} ${SDL2} ${SDL2_IMAGE} -rpath ./frameworks
uninstall:
	rm -rf ${NAME}
clean:
	rm -rf *.c
	rm -rf *.h
	rm -rf *.o
reinstall: uninstall all
