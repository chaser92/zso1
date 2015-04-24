all: 
	gcc --std=c99 -Wall -fPIC -shared allocator.c relocator.c elfhash.c elfreader.c loader.c -o libloader.so

clean:
	rm -f libloader.so
