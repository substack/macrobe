all: manual_pipes

clean:
	rm -f manual_pipes

manual_pipes:
	g++ manual_pipes.cpp -o manual_pipes \
		-I . \
		-I $(PREFIX)/include \
		-l GLEW -l glut
