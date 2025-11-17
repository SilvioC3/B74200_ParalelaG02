all: sequences.out

%.o: %.cc
	g++ -g -c $<

%.o: %.c
	gcc -g -c $<

sequences.out: sequences.o adn.o lcs.o
	g++ -g sequences.o adn.o lcs.o -o sequences.out

clean:
	rm -f *.o *.out
