objects 		= MM_client.o keyin.o
include_libpq	:= -I$(shell pg_config --includedir)
lib_libpq		:= -L$(shell pg_config --libdir) -lpq

MM_client : $(objects)
	gcc -g $(include_libpq) -o MM_client $(objects) $(lib_libpq) -ansi \
	-pedantic -Wall -Wextra

MM_client.o : keyin.h
	gcc -g $(include_libpq) -c MM_client.c $(lib_libpq) -ansi \
	-pedantic -Wall -Wextra
	
keyin.o :
	gcc -g -c keyin.c -ansi -pedantic -Wall -Wextra
	
.PHONY : clean

clean :
	rm MM_client $(objects)
