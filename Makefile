all: macroed manual_pipes

clean:
	rm -f manual_pipes macroed

macroed:
	g++ macroed.cpp -o macroed \
		-I . \
		-I $(PREFIX)/include \
		-l GLEW -l glut

manual_pipes:
	g++ manual_pipes.cpp -o manual_pipes \
		-I . \
		-I $(PREFIX)/include \
		-l GLEW -l glut
