#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "queue.h"
#include "boolean.h"
#include "kbhit.h"

#define TAM_INI 3
#define ALT_LARG_MIN 7

typedef struct snake{
	struct snake* prev, *next;
	int x, y;
}snake;

typedef enum{
	ESQ, DIR, CIM, BAI
}Direcao;

boolean game_over;
boolean precisa_de_comida;
snake* cobra = NULL;
snake comida;
int altura, largura;
char ** tela;
int pontos = 0;

/*	FUNÇÕES DE TELA	 */

char** tela_ini(){
	int i;
	char** t = (char**) malloc(sizeof(char*)*altura);
	for(i=0; i<altura; i++)
		t[i] = (char*) malloc(sizeof(char)*largura);
	
	return t;
}

//faz a moldura com #
void plot_moldura(char** t){
	int i, j;
	for(i=0; i<altura; i++){
		for(j=0; j<largura; j++){
			if(i==0 || j==0 || i==(altura-1) || j==(largura-1))
				t[i][j] = '#';
			else
				t[i][j] = ' ';
		}
	}
}

//libera tela
void tela_libera(char** t){
	int i;
	for(i=0; i<altura; i++)
		free(t[i]);
	
	free(t);
}

//imprime tela
void tela_imprime(char** t){
	int i, j;
	for(i=0; i<altura; i++){
		for(j=0; j<largura; j++){
			printf("%c ", t[i][j]);	
		}
		printf("\n");
	}
}

void plot_snake(char** t, snake* s){
	snake* aux = s;
	
	do{
		t[aux->y][aux->x] = '*';
		aux = aux->next;
	}while(aux != s);
}


/*		FUNÇÕES DA COBRA	*/
void snake_inicia(int tam){
	
	int i, count;
	int l = largura/2, a = altura/2;
	for(i=0; i<tam; i++){
		snake* s = malloc(sizeof(snake));
		s->prev = s->next = NULL;
		s->x = l;
		s->y = a;
		l++;
		
		queue_append((queue_t**)&cobra, (queue_t*)s);
	}
}

void snake_libera(){
	snake *aux;
	snake *s = cobra;
		
	do{
		aux = s->next;
		free(s);
		s = aux;
	}while(aux != cobra);
}

//função que movimentará a cobra
void mov_cobra(Direcao d){
	
	snake* i = cobra->prev;
	
	while(i!=cobra){
		i->x = i->prev->x;
		i->y = i->prev->y;
		
		i = i->prev;
	}
	
	switch(d){
		case CIM:
			cobra->y--;
			break;
		case ESQ:
			cobra->x--;
			break;
		case BAI:
			cobra->y++;
			break;
		case DIR:
			cobra->x++;
			break;
	}
}

/*JOGO*/

void gera_comida(){
	int n_livre = 0; //armazena quantos espaços livres estão disponíveis
	int i, j;
	
	int *ys = (int*)malloc(sizeof(int)*altura*largura);
	int *xs = (int*)malloc(sizeof(int)*largura*altura);
	
	for(i=1; i<altura-1; i++){
		for(j=1; j<largura-1; j++){
			if(tela[i][j] == ' '){
				ys[n_livre++] = i;
				xs[n_livre] = j;
			}
		}
	}
	
	if(n_livre == 0){
		printf("Parabéns, você ganhou o jogo!!!!!!!!\nNão sabia que era possível...\n");
		exit(0);
	}else{
		int n = rand()%n_livre;
		comida.x = xs[n];
		comida.y = ys[n];
	}	
	
	
	free(ys);
	free(xs);
	precisa_de_comida = false;
}

void game_conditions(){
	//A cobra bateu em alguma das paredes?
	if(cobra->x <= 0 || cobra->x >= (largura-1) || cobra->y <= 0 || cobra->y >= (altura-1)){
		game_over = true;
		return;
	}
	
	//se a cobra bateu nela mesma
	snake* i = cobra->next;
	
	do{
		if(cobra->x == i->x && cobra->y == i->y){
			game_over = true;
			return;
		}
		i = i->next;
	}while(i != cobra);
	
	//gera comida
	if(precisa_de_comida){
		gera_comida();
	}else{
		if(cobra->x == comida.x && cobra->y == comida.y){
			pontos++;
			precisa_de_comida = true;
			snake* new = malloc(sizeof(snake));
			new->next = new->prev = NULL;
			new->x = cobra->prev->x;
			new->y = cobra->prev->y;
			queue_append((queue_t**)&cobra, (queue_t*)new);
		}
	}
	
	
}

void plot_comida(char** t){
	t[comida.y][comida.x] = '@';
}



char ultima_tecla(){
	char c;
	int ch;
	do{
		struct termios oldt, newt;
		
		int oldf;
		tcgetattr(STDIN_FILENO, &oldt);
		newt = oldt;
		newt.c_lflag &= ~(ICANON | ECHO);
		tcsetattr(STDIN_FILENO, TCSANOW, &newt);
		oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
		fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
		ch = getchar();
		tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
		fcntl(STDIN_FILENO, F_SETFL, oldf);
		if(ch != EOF)
			c = ch;
	}while(ch != EOF);
	
	return c;
}

int main(int argc, char *argv[]){
	srand(time(NULL));
	game_over = false;
	precisa_de_comida = true;
	
	printf("Altura: "); scanf("%d", &altura);
	printf("Largura: "); scanf("%d", &largura);
	
	if(altura <= ALT_LARG_MIN || largura <= ALT_LARG_MIN){
		printf("ERRO, tamanho mt pequeno!\n");
		exit(1);
	}

	tela = tela_ini();
	plot_moldura(tela);
		
	snake_inicia(TAM_INI);

	Direcao dir = ESQ;
	do{

		//alguma tecla foi digitada?
		if(kbhit()){
			
			//deixa pronta somente a ultima tecla digitada
			ultima_tecla();

			//qual é essa tecla?
			switch(ultima_tecla()){
				case 'w':
					if(dir != BAI)
						dir = CIM;
					break;
				case 'a':
					if(dir!=DIR)
						dir = ESQ;
					break;
				case 's':
					if(dir != CIM)
						dir = BAI;
					break;
				case 'd':
					if(dir != ESQ)
						dir = DIR;
					break;
			}
		}
		
		//movimenta a cobra
		mov_cobra(dir);
		
		//condições de "morte"
		game_conditions();
		
		//desenha a moldura
		plot_moldura(tela);
		
		//desenha a cobra
		plot_snake(tela, cobra);
		
		//desenha comida
		plot_comida(tela);
		
		system("clear");
		tela_imprime(tela);	
			
		usleep(100000);		
		
	}while(!game_over);
	
	printf("Game Over\n");
	printf("Pontos: %d/%d\n", pontos, largura*altura);
	printf("%.2lf%% do jogo concluído\n", ((double)pontos/(double)(largura*altura))*100.0);
	
	snake_libera(cobra);
	tela_libera(tela);
	return 0;
}
