all:
	g++ main.cpp -o myrpal

clean:
	rm -f *.o myrpal

run: all
	./myrpal input.txt

lex: all
	./myrpal -lex input.txt

ast: all
	./myrpal -ast input.txt

st: all
	./myrpal -st input.txt

