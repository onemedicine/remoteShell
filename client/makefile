obj= obj/client_main.o obj/client_menu.o obj/client_read.o 
in= -I include -lpthread -Wall
bin/client:$(obj)
	gcc $(obj) -o bin/client $(in)
obj/client_main.o:	src/client_main.c
	gcc -c src/client_main.c -o obj/client_main.o $(in)
obj/client_menu.o:	src/client_menu.c
	gcc -c src/client_menu.c -o obj/client_menu.o $(in)
obj/client_read.o:	src/client_read.c
	gcc -c src/client_read.c -o obj/client_read.o $(in)
.PHONY:clean
clean:
	rm obj/* bin/*
