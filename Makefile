OBJ = main.o queue.o
CFLAGS = -Wall --std=c99
shell: $(OBJ)
	gcc $(OBJ) -o shell $(CFLAGS)
.PHONY: clean
clean:
	rm *.o shell
