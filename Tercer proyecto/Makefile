all: medios.out

%.o: %.cc
	g++ -g -c $<

%.o: %.c
	gcc -g -c $<

pruebaPuntos.out: Punto.o VectorPuntos.o pruebaPuntos.o
	g++ -g -lm pruebaPuntos.o VectorPuntos.o Punto.o -o pruebaPuntos.out

medios.out: Punto.o VectorPuntos.o medios.o
	g++ -g -lm medios.o VectorPuntos.o Punto.o -o medios.out

mediosOMP.out: Punto.o VectorPuntos.o mediosOMP.o
	g++ -g -fopenmp mediosOMP.o VectorPuntos.o Punto.o -o mediosOMP.out

clean:
	rm -f *.o *.out

