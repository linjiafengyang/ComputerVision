all:main.cpp CImg.h
	rm -f main.out
	g++ -o main.out main.cpp ImageStitching.cpp ./siftpp/sift.cpp -g -I ./ -I ./siftpp -O2 -L/usr/X11R6/lib -lm -lpthread -lX11
