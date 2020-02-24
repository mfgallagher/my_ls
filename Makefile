myls: myls.c
	gcc -g -Wall -pedantic -o myls myls.c

.PHONY: clean
clean:
	rm myls
