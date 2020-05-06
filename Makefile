CC = clang
CFLAG = -std=c11 -Wall -Wextra -Werror -Wpedantic
SDL2 = -I ./resource/Frameworks/SDL2.framework/Versions/A/Headers -F ./resource/Frameworks -framework SDL2 -rpath ./resource/Frameworks
SDL2_IMAGE = -I ./resource/Frameworks/SDL2_image.framework/Versions/A/Headers -F ./resource/Frameworks/ -framework SDL2_image -rpath ./resource/Frameworks
NAME = endgame
all: install clean
install:
	cp src/*.c inc/*.h .
	${CC} ${CFLAG} *.c -o ${NAME} ${SDL2} ${SDL2_IMAGE} -rpath ./resource/frameworks
uninstall:
	rm -rf ${NAME}
clean:
	rm -rf *.c
	rm -rf *.h
	rm -rf *.o
reinstall: uninstall all
