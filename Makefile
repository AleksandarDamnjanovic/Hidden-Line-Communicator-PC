build:
	g++ -g -Wall -fPIC -shared communicator.cpp -o communicator.so -lsfml-network -lsfml-system
	gcc -Wall -Wextra -rdynamic -g main.c -o hlc `pkg-config --libs --cflags gtk+-3.0 webkit2gtk-4.0` -ldl -lpthread -export-dynamic
	@echo "Done"