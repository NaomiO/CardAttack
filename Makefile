
hackCard: main.o
	g++ -o hackCard main.o -lnfc

all: hackCard

clean:
	rm -rf main.o hackCard
