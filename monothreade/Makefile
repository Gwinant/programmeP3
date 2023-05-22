CC=gcc
CFLAGS=-Wall -Werror -g
LIBS=-lcunit -lpthread
INCLUDE_HEADERS_DIRECTORY=-Iinclude
#-lcunit
main:    # add your other object files needed to compile your program here. !! The ordering is important !! if file_a.o depends on file_b.o, file_a.o must be placed BEFORE file_b.o in the list !
	$(CC) $(INCLUDE_HEADERS_DIRECTORY) $(CFLAGS)  src/graph.c  main.c -o main $(LIBS)    # this will run the following command: gcc -Wall -Werror -g -o sp sc.c other_object_filespresent_above.o ... -lcunit -lpthread


timer:
	$(CC) $(CFLAGS) timer.c -o timer -lm
	$(CC) $(INCLUDE_HEADERS_DIRECTORY) $(CFLAGS)  src/graph.c  main.c -o main $(LIBS) # IMPORTANT pour l'executable timer !


%.o: %.c                  # if for example you want to compute example.c this will create an object file called example.o in the same directory as example.c. Don't forget to clean it in your "make clean"
	$(CC) $(INCLUDE_HEADERS_DIRECTORY) $(CFLAGS) -o $@ -c $<

clean:
	rm -f src/*.o
	rm -f main
	rm -f Tests
	rm -f timer
	# ...

tests:
	# Do not forget the -lcunit flag
	$(CC) $(INCLUDE_HEADERS_DIRECTORY) $(CFLAGS) src/graph.c Tests.c -o Tests $(LIBS)
	./Tests
	make clean
	echo "TODO: compile and execute the tests!"

# a .PHONY target forces make to execute the command even if the target already exists
.PHONY: clean tests
