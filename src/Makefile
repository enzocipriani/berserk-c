binario: main.o jogo.o hero.o enemy.o
	gcc main.o hero.o jogo.o enemy.o -o jogo -lraylib -lm

main.o: main.c jogo.h
	gcc -c main.c

jogo.o: jogo.c jogo.h
	gcc -c jogo.c 

hero.o: hero.c jogo.h
	gcc -c hero.c

enemy.o: enemy.c jogo.h
	gcc -c enemy.c

clean:
	rm *.o

run:
	./jogo