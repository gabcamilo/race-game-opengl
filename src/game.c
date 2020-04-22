//versao 3.0 - Integracao da versao 2.3 com a versao J.4 -> sorteio da cor dos carrinhos "obstaculos"

//========================================================================================================================================
//versão J.4:
//			- velocidade das pistas (reta/curva) ajustada
//			- carrinho e obstáculos adicionados (sem desaceleração)
//			- tamanho do carrinho modificado / mudei umas coisas na fç q desenha o carrinho na parte da listra (ordem do código)
//			- sol adicionado :>
//========================================================================================================================================

//versao 2.3 - tela de game over ok
//versao 2.2 - tela inicial ok
//versao 2.1 - tela de selecao inicial (falta só alterar a fonte e deixar mais bonitinho)-->> game over
//versao 2.0 - adicao de movimento do carro nas curvas, adicao do carrinho --> ajuste da colisao na pista para o novo tamanho do carro


//versão J.3 - simulação das pistas pronta (falta por os obstáculos, tratar a desaceleração e ajustar as velocidades das pistas)
//versão J.2 - pistas curvas (andando, mas ainda sem obstáculos, e sem desaceleração)
//versão J.1 - com colisão & obstáculo anda ao contrário (se espaço não está pressionado) 

//versao 1.2.4 - implementacao de uma taxa de aceleracao para o carrinho
//versao 1.2.3 - implementacao do buffer das teclas
//versao 1.2.2 - xxxx
//versao 1.2.1 - pistas retas colocadas movimentacao funcionando
//versao 1.2 - adicao da pista (sem movimentos)
//versao 1.1 - movimento do carro e da camera

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <time.h>
#include <math.h>
#include <string.h>
float z_portal=5; //posição em z do portal
void portal(); //função que desenha o portal
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Constantes
#define PI 3.14159265358979323846
#define cpista 150	//comprimento da pista
#define lpista 10 	//largura da pista (valor*2)
#define lcar 2		//largura do carro
#define acar 6		//altura do carro
#define lcenario 400	//largura do cenario (valor*2)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//joão
// variáveis - Obstáculos & Colisão 
float posx_obstaculo; //posição em x do obstáculo (fixa quando a pista é reta) / (modifica quando a pista é curva)
float limitez_obstaculo = 100; //limite em z que o obstaculo pode sair da tela 
float posz_obstaculo= limitez_obstaculo+5; //inicialmente está fora da tela, então gera um novo
float ang_obstaculo=-11, ang_max_obstaculo=90; 
float v_obstaculo; //velocidade do obstáculo => tende a zero qdo a velocidade do carro é máxima (e vice versa)
float v_obstaculo_curva; //velocidade do obstáculo na pista curva

int colidiu_frente=0; //identifica colisão frontal
int colidiu_esq=0; //identifica colisão à esquerda
int colidiu_dir=0; //identifica colisão à direita

//Variáveis - Pistas Curvas
float ang_esq=0; //variável que controla o ângulo que rotaciona a pista esquerda (conforme aperta espaço)
float ang_dir=0; //variável que controla o ângulo que rotaciona a pista direita (conforme aperta espaço)
float v_curva=0;//0.2 //velocidade da pista curva
float cpista_curva = 9*cpista; //comprimento da pista curva
float raio = (cpista_curva)/(2*PI); //raio da pista curva
float fim_pista_curva=270; //ângulo que marca o fim da pista curva

//Pintura da pista
float cpintura=10; // comprimento da pintura branca da pista
float espacamento=30; //espaçamento entre cada pintura
float lpintura=0.25; //largura da pintura (2*lpintura)

//Pista atual e próxima
int pista_atual=0; //0. pista reta, 1. pista esquerda, 2. pista direita
int prox_pista=1; //0. pista reta, 1. pista esquerda, 2. pista direita

//Variáveis - Pista Reta
int n_retas; //contador de quantas pistas retas já apareceram
int max_retas=5; //número máximo de pistas retas que aparecem consecutivamente
float fim_pista_reta=-3*cpista-20; //variável que armazena o fim da pista reta

//Funções
void Gera_Obstaculos_colisao(); //Gera os obstáculos e trata a colisão
void gera_montanhas(); //Gera as montanhas
void pista_direita(); //gera a pista curva à direita
void pista_esquerda(); //gera a pista curva à esquerda
void pistas(); //função que faz a simulação/sorteio das pistas
void carrinho(int cor); //função que desenha o carrinho
void obstaculos_esquerda(); //gera os obstáculos na pista esquerda
void obstaculos_direita();	//gera os obstáculos da pista direita
void obstaculos_reta(); //gera os obstáculos da pista reta
int verifica_colisao(); //função que verifica se houve colisão entre os obstáculos e o carrinho
//joão
//============================================================================================================================

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool keyStates[256];//buffer para armazenar multi teclas pressionadas

//Declaração de Variáveis Globais
int projecao=0;			//Variável Lógica para Definir o Tipo de Projeção (Perspectiva ou Ortogonal)
int posx=0, posy=15, posz=40;	//Variáveis que definem a posição da câmera
float oy=8,ox=0,oz=0;		//Variáveis que definem para onde a câmera olha
int lx=0, ly=1,  lz=0;		//Variáveis que definem qual eixo estará na vertical do monitor.

float vcamx=0.5;		//velocidade da movimentacao da camera no eixo x (durante o jogo)
float vcarx=0.5, poscarx=0, poscarz=10;	//definem a velocidade e posicao do carrinho (eixo x e z)
float velocidade=0;		//velocidade do carrinho (na vdd da pista) em z
float aceleracao=1;	//taxa de aceleracao do carrinho no eixo z
//float velocidademax=2.5;	//velocidade maxima que o carrinho pode atingir
float velocidademax=1;


float pospistaz=0;//posicao da pista em z
int fimp3=-cpista;//marca o fim da pista 3 a fim de colocar a prxima pista
int npista=0;//numero de pistas geradas
float ang_carrinho=0; //ângulo que o carrinho está, dependendo da pista
//multi telas:
//========================================================================
//variaveis
int tela=0;//conta qual tela esta sendo exibida 0-menu inicial 1-jogo 3-game over
int cor_carro=0;//cor do carrinho player
int cor_sort=0;//cor do carrinho sorteado
int rodacar=90;
//funcoes menu inicial
void menu_inicio();
void carrinho_menu();
void gera_setas(int poscarini);
void DesenhaTextoStroke(void *font, char *string);
void DesenhaTexto(void *font, char *string);
char quote[9][80];
bool continua=true;
int pos_seta=4;
//funcoes game over
void game_over();


//========================================================================
void iniciajogo();


//Protótipos das Funções:

void Display();

//funcoes para interacao 

void keyPressed(unsigned char key, int x, int y);//tecla pressionada
void keyUp(unsigned char key, int x, int y);//tecla nao pressionada

void TeclasEspeciais1 (int key, int x, int y);//teclas direcionais durante o jogo
void keyboard1 (void);//funcao para ler teclas normais pressionadas, ira passar os parametros para a func keypressed
void Mouse(int botao, int estado, int x, int y);
//obs funcoes de interacao terminadas em 1 sao para interacao durante  jogo (nao menu e configuracoes)

void chao();//funcao que gera todo o cenario, menos as pistas
void pista_reta(int ini);//coloca no cenario a pista reta
void pista_esq(int ini);//coloca no cenario a pista virando a esquerda
void pista_dir(int ini);//coloca no cenario a pista virando a direita
void geraceu();
void sol();

void gera_player();//gera o carrinho do jogador


int main(int argc,char **argv)
{
		srand(time(NULL));	
		glutInit(&argc, argv); // Initializes glut
			
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); /*Define as características do espaço vetorial.
		                                                          //  Nesse caso, permite animações (sem cintilações), cores compostas por Verm. Verde e Azul,
		                                                          //  Buffer que permite trablhar com profundidade e elimina faces escondidas.*/
		
		glutInitWindowSize(720, 480);//resolucao 480p
		glutInitWindowPosition(341, 220);//inicializa a janela no meio da tela
		glutCreateWindow("Novo Enduro! =D");
		glutDisplayFunc(Display);
	
			
		glutSpecialFunc(TeclasEspeciais1);
		
		
		glutKeyboardFunc(keyPressed); // tecla pessionada
		glutKeyboardUpFunc(keyUp);
		
		//glutMouseFunc(Mouse);
		   
	  glutMainLoop();
	
   return 0;
}

void Display()
{
	keyboard1();
	// Inicializa parâmetros de rendering
	// Define a cor de fundo da janela de visualização como preta
	glClearColor(0, 0, 0, 0);//cor de fundo da janela (preta)
	


	glMatrixMode(GL_PROJECTION);/*glMatrixMode()- define qual matriz será alterada. SEMPRE defina o tipo de apresentação
                              (Ortogonal ou Perspectiva) na matriz PROJECTION.*/
	glLoadIdentity();//"Limpa" ou "transforma" a matriz em identidade, reduzindo possíveis erros.

		gluPerspective(45,1,1,0); //Define a projeção como perspectiva

	glMatrixMode(GL_MODELVIEW);	/*glMatrixMode()- define qual matriz será alterada. SEMPRE defina a câmera
					(Ortogonal ou Perspectiva) na matriz MODELVIEW (onde o desenho ocorrerá).*/

	glLoadIdentity(); ////"Limpa" ou "transforma" a matriz em identidade, reduzindo possíveis erros.

	gluLookAt(posx,posy,posz,ox,oy,oz,lx,ly,lz); //Define a pos da câmera, para onde olha e qual eixo está na vertical.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); /* "limpa" um buffer particular ou combinações de buffers,
                                                         onde buffer é uma área de armazenamento para informações da imagem.
                                                        Nesse caso, está "limpando os buffers para suportarem animações */
							//Chamada para Função  ou funções para desenhar o objeto/cena...
	//----------------------------------------------------------------

	switch(tela){
	case 0://menu inicial
		menu_inicio();
	break;

	case 1://inicia o jogo
		iniciajogo();
	break;

	case 2://game over
		game_over();
	break;

	}
	
	//glColor3ub(255, 255, 255); 
	
	//----------------------------------------------------------------

	glutSwapBuffers(); //Executa a Cena. SwapBuffers dá suporte para mais de um buffer, permitindo execução de animações sem cintilações.
}

void iniciajogo(){
	
	chao();
	pistas();
		
	geraceu();

	
	gera_montanhas();
	
	
	Gera_Obstaculos_colisao();
	gera_player();
}


void gera_player(){
	glPushMatrix(); //carrinho
		
		if(pista_atual==0)
		{
			if(ang_carrinho<=0) ang_carrinho+=0.1;
			if(ang_carrinho>=0) ang_carrinho-=0.1;
		}
		if(pista_atual==1 && ang_carrinho<=5)  ang_carrinho+=0.1;
		if(pista_atual==2 && ang_carrinho>=-5) ang_carrinho-=0.1;
		
		glTranslated(poscarx,0,poscarz); 
		glRotatef(ang_carrinho, 0, 1, 0);	
		carrinho(cor_carro);
		
	glPopMatrix();  //Retorna ao estado anterior da cena. O que for desenhado após o Push não influenciou o já representado
}


void geraceu(){
	glPushMatrix(); 
	
		//glutKeyboardFunc(keyboard1);;
		glBegin(GL_QUADS);	
			//cor mais clara do céu (degradê)
			glColor3ub(0, 191, 255); //glColor3ub(185, 228, 255);	
				glVertex3f(-lcenario, 0, -280);
				glVertex3f(lcenario, 0, -280);
			
			//cor mais escura do céu (degradê)
			glColor3ub(0, 191, 255);
				glVertex3f(lcenario, 100, -280);
				glVertex3f(-lcenario, 100, -280);
		glEnd();
		
	glPopMatrix();
	
	glPushMatrix();
		glScalef(0.5, 0.6, 1);
		glTranslated(120, 100, 0);
		sol();
	glPopMatrix();
}

void sol()
{
	int raio = 50;
	int n_pontos = 2*PI*raio; //número de pontos do círculo => circunferência = 2*PI*R
	double angulo=(2*PI)/n_pontos; //ângulo em radianos
	
	//desenho do degradê do sol
	glBegin(GL_TRIANGLE_STRIP);
	
		for(double i=0; i<=2*PI; i+=angulo)
		{ //i de 0º a 360º
		
		  //glColor3ub(0, 162, 232); //azul
			glColor3ub(0, 191, 255); //azul (tem que ser a mesma cor do céu)
			glVertex3d(cos(i)*raio, sin(i)*raio, -280); //imprime o ponto P do círculo: P(cos x, sen x) multiplicado pelo raio
			
			glColor3ub(255, 255, 0);
			glVertex3d(cos(i)*(raio-raio), sin(i)*(raio-raio), -280);
		}
	glEnd();
	
	//círculo do sol
	glBegin(GL_TRIANGLE_STRIP);
		for(double i=0; i<=2*PI; i+=angulo)
		{ 
			glColor3ub(255, 255, 15);
			glVertex3d(cos(i)*(raio-40), sin(i)*(raio-40), -280); //imprime o ponto P do círculo: P(cos x, sen x) multiplicado pelo raio
			glVertex3d(cos(i)*(raio-raio), sin(i)*(raio-raio), -280);
		}
	
	glEnd();

}


void chao(){
//-------------------------grama
	glPushMatrix(); 
		//DEFINE cor (R, G, B e percentual de transparência para o objeto)
		//glutKeyboardFunc(keyboard1);;
		glBegin(GL_QUADS);
		
		glColor3ub(0, 255, 127);
		
			glVertex3f(-lcenario, 0, 20);
			glVertex3f(lcenario, 0, 20);
			
		 glColor3ub(0, 174, 77); 
			glVertex3f(lcenario, 0, -280);
			glVertex3f(-lcenario, 0, -280);
		glEnd();
	glPopMatrix();

}

void TeclasEspeciais1 (int key, int x, int y)//teclas direcionais durante o jogo
{//Key - recebe o código ASCII da tecla
 //x, y - recebem respectivamente as posições mouse na tela (permite tratar os dois dispositivos)
	if (key==GLUT_KEY_LEFT && !colidiu_dir&&tela==1)
	{
		
		if(poscarx>=-lpista+lcar/2&&poscarx>=-lpista+lcar/2&&ox>=-lpista+lcar/2){//testa se o carrinho esta no limite da pista
		
			poscarx-=vcarx;
			posx-=vcarx;
			ox-=vcarx;
		
		}
		
		
	}
      
	else if (key==GLUT_KEY_RIGHT && !colidiu_esq&&tela==1)
	{
		
		if(poscarx<=lpista-lcar/2&&poscarx<=lpista-lcar/2&&ox<=lpista-lcar/2){
			poscarx+=vcarx;
			posx+=vcarx;
			ox+=vcarx;
		}

	}

	//acoes do menu inicial==========================
	if (key==GLUT_KEY_LEFT && tela==0 && cor_carro>0){
		cor_carro--;
	
	}

	if (key==GLUT_KEY_RIGHT && tela==0 && cor_carro<6){
		cor_carro++;
	}

	//===============================================
	//acoes da tela de game over=====================
	if (key==GLUT_KEY_UP && tela==2){
		continua=!continua;
		switch(pos_seta){
			case 4:
				pos_seta=1;
				break;
			case 1:
				pos_seta=4;
				break;
		}

	}

	if (key==GLUT_KEY_DOWN && tela==2){
		continua=!continua;
		switch(pos_seta){
			case 4:
				pos_seta=1;
				break;
			case 1:
				pos_seta=4;
				break;
		}
	}

//===============================================
	glutPostRedisplay();
}

//***************************************************************************************************

void keyboard1 (void)
{//Key - recebe o código ASCII da tecla
 //x, y - recebem as posições do mouse na tela (permite tratar os dois dispositivos)
	int i;
	if (keyStates[' '] && !colidiu_frente&&tela ==1)//acelera o carro quando a tecla espaco e pressionada
	{
//========================================================================================================================
		//calcula as velocidades
		if(velocidade<=velocidademax)
		{
			velocidade+=aceleracao;
			v_curva = (velocidade*360)/cpista_curva; //calculo do ângulo correspondente à velocidade
		}
		v_obstaculo = velocidademax - velocidade; //velocidade do obstáculo
		v_obstaculo_curva = ((v_obstaculo)*360)/cpista_curva; //ângulo do obstáculo (pista curva)		
	
//=========================================================================================================
		//movimenta pista reta
		if(pista_atual == 0)
		{
			pospistaz+=velocidade; 
			z_portal+=velocidade; //atualiza posição z do portal
			
			if(n_retas >= max_retas)
			{
				//printf("fim pista reta: %.2f\n", fim_pista_reta);
				fim_pista_reta+=velocidade;
				//ang_obstaculo-=v_curva;
			}
			 
			 //movimenta obstáculo (pista_reta)
			if(v_obstaculo > 0 ) 
				posz_obstaculo += v_obstaculo; 
			else
				posz_obstaculo += velocidade;
				
			if(posz_obstaculo<=fim_pista_reta) //se a prox pista ultrapassou a tela
			{
				posz_obstaculo=fim_pista_reta+100; //+100 pra não sumir qdo trocar de pista
			
			} 
			
			//reseta a pista reta	
			if(pospistaz>=cpista+20 && n_retas<max_retas){
					pospistaz=0;
					n_retas++;
					//printf("n_retas = %d\n", n_retas);
			}
			else
			{
				if(pospistaz>=3*cpista+20)
				{
					pospistaz=0;
					n_retas++;
					//printf("n_retas = %d\n", n_retas);
				}
					
			}
		}
//========================================================================================================================		
		//movimenta pista esquerda
		if(pista_atual == 1 && ang_esq>=-270)
		{ 
			ang_esq-=v_curva;
			//ang_obstaculo-=v_obstaculo_curva;
			ang_obstaculo -= v_curva;//ang_obstaculo-=velocidademax/10; //-=0.1
				
			if(poscarx<=lpista-lcar/2){
				poscarx+=0.25*vcarx;
			}

			if(posx<=(lpista-lcar/2)&&ox<=(lpista-lcar/2)){
				posx+=0.25*vcamx;
				ox+=0.25*vcamx;			
			}	

			fim_pista_curva -= v_curva; //decrementa o fim da pista
			//if(fim_pista_curva<=90)
			//	ang_obstaculo = 90; //p/ impedir que apareça obstaculo quando outra pista entrou
		}
//========================================================================================================================		
		//movimenta pista direita
		if(pista_atual == 2 && ang_dir<=270)
		{
			ang_dir+=v_curva;
			ang_obstaculo -= v_curva;//ang_obstaculo-=velocidademax/10; //-=0.1
			
			if(pista_atual == 2 && ang_dir<=270)
			{
				ang_dir+=v_curva;
				if(poscarx>=-lpista+lcar/2){//testa se o carrinho esta no limite da pista
					poscarx-=0.25*vcarx;
		
				}	
				if(posx>=(-lpista+lcar/2)&&ox>=(-lpista+lcar/2)){
					posx-=0.25*vcamx;
					ox-=0.25*vcamx;			
				}
			
			}

			fim_pista_curva -= v_curva; //decrementa o fim da pista
		}
//========================================================================================================================		
	}

	else if (!keyStates[' '] && !colidiu_frente&&tela==1) {//(!keyStates[' '] && velocidade>0 && !colidiu_frente){//desacelera o carro quando a velocidade.0 e espaco(acelerador) nao esta pressionada
	/*
	//==========================================================================================	
		//pista reta
		if(pista_atual == 0)
		{
				if(velocidade>0) 
				{
					velocidade-=3*aceleracao; //taxa de desaceleracao e 3x maior que a de aceleracao
					//pospistaz+=velocidade;
					
					//if(pospistaz>=cpista+20){
					//	pospistaz=20;
						
					//reseta a pista
					if(pospistaz>=cpista+20 && n_retas<max_retas){
								pospistaz=0;
								n_retas++;
								printf("n_retas = %d\n", n_retas);
					}
					else
					{
							if(pospistaz>=3*cpista+20)
							{
								pospistaz=0;
								n_retas++;
								printf("n_retas = %d\n", n_retas);
							}
								
					}
						
				}
			
	////////////////////////////////////////////////////////////////////////////////////////////////
			v_obstaculo = velocidademax - velocidade;//calcula velocidade do obstáculo
			v_obstaculo_curva = ((v_obstaculo)*360)/cpista_curva;	
			
		//obstáculo vai no sentido contrário se espaço não está pressionado
			if(posz_obstaculo>=-400 && posz_obstaculo<=limitez_obstaculo && posz_obstaculo>fim_pista_reta)
			{
				posz_obstaculo -= v_obstaculo;
			}
			
			if(fim_pista_reta>=-2*cpista && prox_pista ==1 && ang_obstaculo<=40)
			{
				//posz_obstaculo -= v_obstaculo;
				ang_obstaculo+=v_obstaculo_curva;
			}
			
			if(posz_obstaculo<=fim_pista_reta) //se o obstáculo ultrapassou a o fim da pista
			{
				posz_obstaculo=fim_pista_reta;
				posx_obstaculo=0;
			} 
		
		}
	//==========================================================================================	
		if(pista_atual == 1 || pista_atual==2)
		{
			if(ang_obstaculo <= ang_max_obstaculo) ang_obstaculo+=0.1;
		}
	*/		
	}
//acoes do menu inicial==========================
	if (keyStates[' '] && tela==0){
		tela=1;
	}

	if (!keyStates['h'] && tela==0){
		if(rodacar==360)	
			rodacar=0;
		else rodacar++;
	}
/*//acoes de game over=============================
	if (keyStates['x'] && tela==2 && continua){
		tela=0;		
		poscarx=0;
		posx=0;
		colidiu_frente=0;
		colidiu_esq=0;
		colidiu_dir=0;
		pista_atual=0;
		prox_pista=1;
		ox=0;

		
	}*/

	if (keyStates['x'] && tela==2){
		exit(0);
	}
//===============================================
	
	
	if(keyStates['p']&&tela==1)
	{
		posy=1100; //olha de cima
	}
		 
	if(keyStates['o']&&tela==1)
	{
		posy=15; //posição normal
	}
	 
    glutPostRedisplay();
}


void keyPressed (unsigned char key, int x, int y) {  
keyStates[key] = true; // tecla pressionada
}  
  
void keyUp (unsigned char key, int x, int y) {  
keyStates[key] = false; // tecla nao pressionada
}  

//////////////////////////////////
void Gera_Obstaculos_colisao()
{
	//int n_obstaculos=3; //número de obstaculos em 1 pista

	if(pista_atual == 0)
		obstaculos_reta();
		
	if(pista_atual == 1)
		obstaculos_esquerda();

	if(pista_atual == 2)
		obstaculos_direita();	
}

//////////////////////////////////

//============================================================================================================================
//Gera as montanhas do fundo
void gera_montanhas()
{
	glColor3ub(0,255,0);//Define uma cor para a primitiva	

//desenha as montanhas com a função seno (com frequencias e amplitudes diferentes)
int freq1, size1;
int z_fundo = -350; //coordenada z do fundo
float S = 1.5; //fator de escala do eixo y (montanhas)

glPushMatrix();
glScalef (1, S, 1); //puxa as montanhas um pouco no eixo y
glTranslatef(-20,28,0); //translada para a posição do horizonte da pista
//=============================
//montanha 1.0
	freq1 = 20; //frequencia da montanha
	size1 = 15; //altura (amplitude) da montanha
	
glPushMatrix();
glColor3ub(34,177,76);
glTranslatef(10,5,0);
	
	glBegin(GL_TRIANGLE_STRIP);
		
		for(float i=-20*PI; i<=20*PI; i+=0.1)
		//for(float i=-PI/2; i<=3*PI/2; i+=0.1)
		{
			glVertex3d(i*freq1, sin(i)*size1, z_fundo);
			glVertex3d(i*freq1, -(size1+20), z_fundo);	//ponto embaixo p/ o triangulo fechar
		}
			
	glEnd();
	
glPopMatrix();

//=============================
//montanha 1.1
	freq1 = 20; //frequencia da montanha
	size1 = 30; //altura (amplitude) da montanha
	
glPushMatrix();
//glColor3ub(50,150,0);
glTranslatef(0,0,0);
	
	glBegin(GL_TRIANGLE_STRIP);
		
		for(float i=-20*PI; i<=20*PI; i+=0.1)
		//for(float i=-PI/2; i<=3*PI/2; i+=0.1)
		{
			glVertex3d(i*freq1, sin(i)*size1, z_fundo);
			glVertex3d(i*freq1, -(size1+1), z_fundo);	
		}
			
	glEnd();
	
glPopMatrix();	

//=============================
//montanha 2.0
	int freq2 = 20;
	int size2 = 20;
	
	glPushMatrix();
	
	glColor3ub(70,200,20);
	
	glTranslatef(30,-(size1-size2),0);
	glBegin(GL_TRIANGLE_STRIP);
		
		for(float i=-20*PI; i<=20*PI; i+=0.1)
		//for(float i=-PI/2; i<=3*PI/2; i+=0.1)
		{
			glVertex3d(i*freq2, sin(i)*size2, z_fundo); 
			glVertex3d(i*freq2, -(size2+1), z_fundo);	
		}
			
	glEnd();
	glPopMatrix();
	
//=============================	
//montanha 2.1
	freq2 = 7; //frequencia da montanha
	size2 = 5; //altura (amplitude) da montanha
	
	glPushMatrix();
	//glTranslatef(5,-15,0);
	glTranslatef(5,-10,0);
	
	glColor3ub(70,200,20);
	glBegin(GL_TRIANGLE_STRIP);
		
		for(float i=-20*PI; i<=20*PI; i+=0.1)
		//for(float i=-PI/2; i<=3*PI/2; i+=0.1)
		{
			glVertex3d(i*freq2, sin(i)*size2, z_fundo); 
			glVertex3d(i*freq2, -(size2+15), z_fundo);	
		}
			
	glEnd();
	glPopMatrix();

//=============================	
//montanha 3.0

	freq2 = 15; //frequencia da montanha
	size2 = 10; //altura (amplitude) da montanha
	
	glPushMatrix();
	glTranslatef(3,-(size1-size2),0);
	
	glColor3ub(100,223,47);
	glBegin(GL_TRIANGLE_STRIP);
		
		//for(float i=-PI/2; i<=3*PI/2; i+=0.1) // 1 montanha apenas
		for(float i=-20*PI; i<=20*PI; i+=0.1)
		{
			glVertex3d(i*freq2, sin(i)*size2, z_fundo); 
			glVertex3d(i*freq2, -(size2+1), z_fundo);	
		}
			
	glEnd();
	glPopMatrix();

//=============================	
//montanha 3.1

	freq2 = 15; //frequencia da montanha
	size2 = 6; //altura (amplitude) da montanha
	
	glPushMatrix();
	glTranslatef(30,-(size1-size2),0);
	
//	glColor3ub(0,250,0);
	glBegin(GL_TRIANGLE_STRIP);
		
		for(float i=-20*PI; i<=20*PI; i+=0.1)
		//for(float i=-PI/2; i<=3*PI/2; i+=0.1)
		{
			glVertex3d(i*freq2, sin(i)*size2, z_fundo); 
			glVertex3d(i*freq2, -(size2+1), z_fundo);	
		}
			
	glEnd();
	glPopMatrix();

//=============================
//montanha 3.2

	freq2 = 5; //frequencia da montanha
	size2 = 3; //altura (amplitude) da montanha
	
	glPushMatrix();
	glTranslatef(45,-(size1-size2)+4,0);
	
//	glColor3ub(0,250,0);
	glBegin(GL_TRIANGLE_STRIP);
		
		for(float i=-20*PI; i<=20*PI; i+=0.1)
		//for(float i=-PI/2; i<=3*PI/2; i+=0.1)
		{
			glVertex3d(i*freq2, sin(i)*size2, z_fundo); 
			glVertex3d(i*freq2, -(size2+1), z_fundo);	
		}
			
	glEnd();
	glPopMatrix();

//=============================
glPopMatrix();
}

//============================================================================================================================
//Gera a Pista Reta
void pista_reta(int ini){
	int i, beg;//beg dira onde é o comeco da pista
	if(ini==1)
		beg=20;
	else if(ini==2)
		beg=-cpista;
	else
		beg=-2*cpista;

	glPushMatrix(); //pista reta
	
		//glutKeyboardFunc(keyboard1);;

		//movimenta pista reta
		glTranslated(0,0,pospistaz); //se a pista atual é reta, translada em z
		
		//======================================================
		//posiciona e movimenta a próxima pista ==> pista à esquerda
			if(prox_pista == 1)
			{
				glPushMatrix();
					glTranslated(0,0, -3*cpista-20);
					pista_esquerda();
				glPopMatrix();
			}
		
		//posiciona e movimenta a próxima pista ==> pista à direita
			if(prox_pista == 2)
			{
				glPushMatrix();
					glTranslated(0,0, -3*cpista-19);
					pista_direita();
				glPopMatrix();
			}
			
		//======================================================
		
		
		//desenha pista reta
		glColor3ub(128, 128, 128); //DEFINE cor (R, G, B e percentual de transparência para o objeto)
			
		glBegin(GL_QUADS);
			glVertex3f(-lpista, 0.2, beg);
			glVertex3f(lpista, 0.2, beg);
			glVertex3f(lpista, 0.2, ini*-cpista);
			glVertex3f(-lpista, 0.2, ini*-cpista);
		glEnd();

		//pintura da pista reta
		glColor3ub(255, 255, 255);
		for(i=beg;i>-ini*cpista;i-=espacamento){
			glBegin(GL_QUADS);
				glVertex3f(-lpintura, 0.1, i);
				glVertex3f(lpintura, 0.1, i);
				glVertex3f(lpintura, 0.1, i-cpintura);
				glVertex3f(-lpintura, 0.1, i-cpintura);
			glEnd();
		}
		
	glPopMatrix();
	
}

//============================================================================================================================
//Gera a Pista Curva à esquerda
void pista_esquerda()
{
	
	glPushMatrix();	
	
		glTranslated(-raio,0, 20); //posiciona a pista curva na origem
		glRotatef(ang_esq, 0, 1, 0); //"movimento" da pista curva
	
	//posiciona e movimenta a próxima pista ==> pista reta	
		if(prox_pista==0) 
		{
			glPushMatrix();
				glRotatef(270, 0, 1, 0);
				glTranslated(raio,0, -18);
				pista_reta(1);
				pista_reta(2);
				pista_reta(3);	
			glPopMatrix();
		}
	
	//posiciona e movimenta a próxima pista ==> pista à esquerda
		if(prox_pista==1)
		{
			if(ang_esq<=-180)
				ang_esq=0; //reseta o ângulo
		}
	
	//posiciona e movimenta a próxima pista ==> pista à direita
		if(prox_pista==2)
		{
			glPushMatrix();
				glRotatef(270, 0, 1, 0);
				glTranslated(raio,0, -18);
				pista_direita();
			glPopMatrix();	
		}
		
				
	//desenha a pista curva (esquerda)
	glColor3ub(128, 128, 128);
	
		glBegin(GL_TRIANGLE_STRIP);
			//for(float i=0; i<=2*PI; i+=0.01)
			for(float i=PI; i>=-PI/2; i-=0.01)
			{
				glVertex3d(-cos(i)*(raio+lpista), 0.1, -sin(i)*(raio+lpista));
				glVertex3d(-cos(i)*(raio-lpista), 0.1, -sin(i)*(raio-lpista));
			}
		glEnd();
	
	//pintura branca da pista curva
	glColor3ub(255, 255, 255);
	
	float ang_cpintura = (cpintura*2*PI)/cpista_curva; //angulo correspondente ao comprimento da pintura branca (3)
	float ang_espacamento = (espacamento*2*PI)/cpista_curva; //angulo correspondente ao espaçamento da pintura branca (10)
	
		//for(float j=0; j<=2*PI; j+=ang_espacamento)
		for(float j=PI; j>=-PI/2; j-=ang_espacamento)
		{
			glBegin(GL_TRIANGLE_STRIP);
			for(float i=j; i<=j+ang_cpintura; i+=0.01)
			{
				glVertex3d(-cos(i)*(raio+lpintura), 0.2, -sin(i)*(raio+lpintura));
				glVertex3d(-cos(i)*(raio-lpintura), 0.2, -sin(i)*(raio-lpintura));
			}
			glEnd();
		}
	
	glPopMatrix();
}

//============================================================================================================================
//Gera a Pista Curva à direita
void pista_direita()
{

	glPushMatrix();	
	
		glTranslated(raio,0, 20); //posiciona a pista curva na origem
		glRotatef(ang_dir, 0, 1, 0); //"movimento" da pista curva
	
	//posiciona e movimenta a próxima pista ==> pista reta	
	if(prox_pista==0)
	{
			glPushMatrix();
				glRotatef(90, 0, 1, 0);
				glTranslated(-raio,0, -18);
				pista_reta(1);
				pista_reta(2);
				pista_reta(3);	
			glPopMatrix();
	}

	//posiciona e movimenta a próxima pista ==> pista à esquerda
		if(prox_pista==1)
		{
			glPushMatrix();
				glRotatef(90, 0, 1, 0);
				glTranslated(-raio,0, -18);
				pista_esquerda();
			glPopMatrix();		
		}
		
	//posiciona e movimenta a próxima pista ==> pista à direita	
		if(prox_pista==2)
		{
			if(ang_dir>=180)
				ang_dir=0; //reseta o ângulo
		}
	
	//desenha a pista curva (direita)
	glColor3ub(128, 128, 128);

		glBegin(GL_TRIANGLE_STRIP);
			for(float i=3*PI/2; i>=0; i-=0.01)
			{
				glVertex3d(-cos(i)*(raio+lpista), 0.1, -sin(i)*(raio+lpista));
				glVertex3d(-cos(i)*(raio-lpista), 0.1, -sin(i)*(raio-lpista));
			}
		glEnd();
	
	//pintura branca da pista curva (direita)
		glColor3ub(255, 255, 255);
		
		float ang_cpintura = (cpintura*2*PI)/cpista_curva; //angulo correspondente ao comprimento da pintura branca
		float ang_espacamento = (espacamento*2*PI)/cpista_curva; //angulo correspondente ao espaçamento da pintura branca
		
			//for(float j=3*PI/2; j>=0; j-=ang_espacamento)
			for(float j=0; j<=3*PI/2; j+=ang_espacamento)
			{
				glBegin(GL_TRIANGLE_STRIP);
				for(float i=j; i<=j+ang_cpintura; i+=0.01)
				{
					glVertex3d(-cos(i)*(raio+0.25), 0.2, -sin(i)*(raio+0.25));
					glVertex3d(-cos(i)*(raio-0.25), 0.2, -sin(i)*(raio-0.25));
				}
				glEnd();
			}
		
	glPopMatrix();

}

//============================================================================================================================
//Sorteia as pistas
void pistas()
{

	switch(pista_atual)
	{
		case 0: //pista reta
			
			pista_reta(1);
			pista_reta(2);
			pista_reta(3);
			
			if(n_retas==max_retas+1) //pista chegou ao fim
			{
				posz_obstaculo = -400;
				fim_pista_reta = -3*cpista-20;
				n_retas=0;
				pista_atual = prox_pista;
				prox_pista = rand()%3; //sorteia a próxima pista
				//printf("prox pista: [%d]\n\n", prox_pista);	
			}
			else
			if(prox_pista==0 && n_retas==max_retas) //se a próxima é reta também
			{		
				n_retas=0;
				prox_pista = rand()%3;
				//printf("prox pista: [%d]\n\n", prox_pista);	
			}
			break;
			
		case 1: //pista esquerda
			pista_esquerda();
			
			if(ang_esq<=-270) //quando a pista curva chegou ao fim (rotacionou -270º), troca p/ a próxima
			{
				
				ang_esq=0; //reseta o ângulo da pista
				fim_pista_curva=270; //reseta o fim da curva
				ang_obstaculo=90;
			
				//printf("prox pista: [%d]\n\n", prox_pista);	
				pista_atual = prox_pista;
				prox_pista = rand()%3;
						
			}
			else
			{
				if(prox_pista == 1 && ang_esq<=-179)
				{
					//printf("prox pista: [%d]\n\n", prox_pista);
					//fim_pista_curva=180;
					prox_pista = rand()%3;
				}
			}
			break;
			
		case 2: //pista direita
			pista_direita();
			
			if(ang_dir>=270)  //quando a pista curva chegou ao fim (rotacionou +270º), troca p/ a próxima
			{
				//printf("prox pista: [%d]\n\n", prox_pista);
				
				ang_dir=0; //reseta o ângulo da pista
				fim_pista_curva=270; //reseta o fim da curva
				ang_obstaculo=90;
				
				pista_atual = prox_pista; //próxima pista assume a atual
				prox_pista = rand()%3; //sorteia a próxima pista
			}
			else
			{
				if(prox_pista == 2 && ang_dir>=179)
				{
					//printf("prox pista: [%d]\n\n", prox_pista);
					//fim_pista_curva=180;	
					prox_pista = rand()%3;
				}
			}
				
			break;
	}	
}


void menu_inicio(){

	int poscarini=-10,lenghOfQuote, i;

	//fundo===================================
	chao();
	pistas();	
	geraceu();
	gera_montanhas();

	//========================================
	glPushMatrix();
		glTranslated(poscarx,0,poscarini);
		glRotatef(rodacar,0,1,0);
		carrinho(cor_carro);
	glPopMatrix();

	//setas para indicacao do comando de mudanca da cor
	gera_setas(poscarini);
	glPushMatrix();
		glTranslated(0,5,0);
		glColor3ub(50, 50, 50);
	
	glPopMatrix();

	//texto

	//"NOVO ENDURO" - sombra
	glPushMatrix();
		glTranslatef(-14.7,15.2,0);
		glColor3ub(50, 50, 50);
		glScalef(0.03, 0.03, 0.03); // diminui o tamanho do fonte
	    
		lenghOfQuote = (int)strlen(quote[0]);
	

		for (i = 0; i < lenghOfQuote; i++)
		{
		    glutStrokeCharacter(GLUT_STROKE_ROMAN, quote[0][i]);
		}
	glPopMatrix();

	//"NOVO ENDURO"
	glPushMatrix();
		glTranslatef(-14.5,15,0);
		glColor3ub(255, 0, 0);
		glScalef(0.03, 0.03, 0.03); // diminui o tamanho do fonte

		lenghOfQuote = (int)strlen(quote[0]);
	

		for (i = 0; i < lenghOfQuote; i++)
		{
		    glutStrokeCharacter(GLUT_STROKE_ROMAN, quote[0][i]);
		}
	glPopMatrix();
	    
	//"USE AS SETAS PARA ESCOLHER A COR DO CARRO"
	glPushMatrix();
		glTranslatef(-8.5,0,0);
		glScalef(0.0048, 0.0048, 0.0048);
		glColor3ub(50, 50, 50);
		glLineWidth(2); 
	
		lenghOfQuote = (int)strlen(quote[1]);
		
		for (i = 0; i < lenghOfQuote; i++)
		{
		    glutStrokeCharacter(GLUT_STROKE_ROMAN, quote[1][i]);
		}
	glPopMatrix();

	//"PRESSIONE \"ESPACO\" PARA INICIAR O JOGO" - sombra
	glPushMatrix();
		glColor3ub(50, 50, 50);
		glTranslatef(-13.4,-3.9,0);
		glScalef(0.009, 0.009, 0.009); // diminui o tamanho do fonte
		glLineWidth(3); 
	
		lenghOfQuote = (int)strlen(quote[2]);
		
		for (i = 0; i < lenghOfQuote; i++)
		{
		    glutStrokeCharacter(GLUT_STROKE_ROMAN, quote[2][i]);
		}
	glPopMatrix();
	

	//"PRESSIONE \"ESPACO\" PARA INICIAR O JOGO" - sombra
	glPushMatrix();
		glColor3ub(255, 255, 255);
		glTranslatef(-13.3,-4,0);
		glScalef(0.009, 0.009, 0.009); // diminui o tamanho do fonte
		glLineWidth(3); 
	
		lenghOfQuote = (int)strlen(quote[2]);
		
		for (i = 0; i < lenghOfQuote; i++)
		{
		    glutStrokeCharacter(GLUT_STROKE_ROMAN, quote[2][i]);
		}
	glPopMatrix();

		strcpy(quote[0],"NOVO ENDURO");
		strcpy(quote[1],"USE AS SETAS PARA ESCOLHER A COR DO CARRO");
		strcpy(quote[2],"PRESSIONE \"ESPACO\" PARA INICIAR O JOGO");


}//fim menu inicio

void game_over(){
	int lenghOfQuote, i;

//texto==================================================================
//"GAME OVER"
	glPushMatrix();
		glColor3ub(255, 255, 255);
		glTranslatef(-10,13,0);
		glScalef(0.02, 0.02, 0.02); // diminui o tamanho do fonte
		glLineWidth(4); 
	
		lenghOfQuote = (int)strlen(quote[3]);
		
		for (i = 0; i < lenghOfQuote; i++)
		{
		    glutStrokeCharacter(GLUT_STROKE_ROMAN, quote[3][i]);
		}
	glPopMatrix();
// "Pressione [x] para sair"
	glPushMatrix();
		glColor3ub(255, 255, 255);
		glTranslatef(-6,3.5,0);
		//glRotatef(270,0,0,1);
		glScalef(0.008, 0.008, 0.008); // diminui o tamanho do fonte
		glLineWidth(2); 
	
		lenghOfQuote = (int)strlen(quote[4]);
		
		for (i = 0; i < lenghOfQuote; i++)
		{
		    glutStrokeCharacter(GLUT_STROKE_ROMAN, quote[4][i]);
		}
	glPopMatrix();

/*// "Sair"
	glPushMatrix();
		glColor3ub(255, 255, 255);
		glTranslatef(-2,0.5,0);
		//glRotatef(270,0,0,1);
		glScalef(0.008, 0.008, 0.008); // diminui o tamanho do fonte
		glLineWidth(2); 
	
		lenghOfQuote = (int)strlen(quote[5]);
		
		for (i = 0; i < lenghOfQuote; i++)
		{
		    glutStrokeCharacter(GLUT_STROKE_ROMAN, quote[5][i]);
		}
	glPopMatrix();*/


		strcpy(quote[3],"GAME OVER :'(");
		strcpy(quote[4],"Pressione X para sair");
		//strcpy(quote[5],"Sair");

	/*glPushMatrix();
		glColor3ub(255, 255, 255);
		glTranslatef(-3,pos_seta,0);
		glBegin(GL_TRIANGLES);
		        glVertex3f(0,-0.5,0);
		        glVertex3f(0,0.5,0);
		        glVertex3f(0.5,0,0);
		        
		glEnd();*/
	glPopMatrix();

}


void gera_setas(int poscarini){
	glColor3ub(50, 50, 50);
	glPushMatrix();//seta esquerda
		glTranslated(6,0,poscarini);
		glBegin(GL_QUADS);
		        glVertex3f(0,0,0);
		        glVertex3f(2,0,0);
		        glVertex3f(2,1,0);
		        glVertex3f(0,1,0);
		glEnd();

		glBegin(GL_TRIANGLES);
		        glVertex3f(2,1.5,0);
		        glVertex3f(2,-0.5,0);
		        glVertex3f(3,0.5,0);
		glEnd();
		
	glPopMatrix();


	glPushMatrix();//seta direita
		
		glTranslated(-6,0,poscarini);
		glRotatef(180,0,1,0);
		
		glBegin(GL_QUADS);
		        glVertex3f(0,0,0);
		        glVertex3f(2,0,0);
		        glVertex3f(2,1,0);
		        glVertex3f(0,1,0);
		glEnd();

		glBegin(GL_TRIANGLES);
		        glVertex3f(2,1.5,0);
		        glVertex3f(2,-0.5,0);
		        glVertex3f(3,0.5,0);
		glEnd();
		
	glPopMatrix();

}

//============================================================================================================================
//Desenha o carrinho
void carrinho(int cor)
{
glScalef(0.5,1,0.8); 
glPushMatrix(); //carrinho
    glColor3ub(255, 255, 255); //DEFINE cor (R, G, B e percentual de transparência para o objeto)

    glScalef(1,0.5,1.5); 	
    glPushMatrix();
        glRotatef(90,0,1,0);

//=======================================

//======================================		
		
        //pneu traseiro esquerdo
        glPushMatrix(); 
	        glColor3ub(0, 0, 0);      
            glTranslatef(-1.5,-1,-2);  
            glRotatef(90,0,0,1);
            glutSolidTorus(0.3, 0.4, 20, 20);
        glPopMatrix(); 

        //pneu dianteiro esquerdo
        glPushMatrix(); 
	        glColor3ub(0, 0, 0);      
            glTranslatef(1.5,-1,-2);  
            glRotatef(90,0,0,1);
            glutSolidTorus(0.3, 0.4, 20, 20);
        glPopMatrix();

        //pneu dianteiro direito
        glPushMatrix(); 
	        glColor3ub(0, 0, 0);      
            glTranslatef(1.5,-1,2);  
            glRotatef(90,0,0,1);
            glutSolidTorus(0.3, 0.4, 20, 20);
        glPopMatrix(); 

        //pneu traseiro direito
        glPushMatrix(); 
	        glColor3ub(0, 0, 0);      
            glTranslatef(-1.5,-1,2);  
            glRotatef(90,0,0,1);
            glutSolidTorus(0.3, 0.4, 20, 20);
        glPopMatrix(); 
//==============================================================
        //farol        
        glColor3ub(255,165,0);
        glBegin(GL_QUADS);
            //esquerda
            glVertex3f(3,-0.5,-2);
            glVertex3f(3,-0.5,-1);
            glVertex3f(3,0.5,-1);
            glVertex3f(3,0.5,-2);
        glEnd();
        
        glBegin(GL_QUADS);
            //direita
            glVertex3f(3,-0.5,1);
            glVertex3f(3,-0.5,2);
            glVertex3f(3,0.5,2);
            glVertex3f(3,0.5,1);
        glEnd();

//==============================================================
        //listras dianteira
        glColor3ub(0, 0, 0);
        glLineWidth(10);  
        glBegin(GL_LINES);
           glVertex3f(3,0.4,-1);
           glVertex3f(3,0.4,1);

           glVertex3f(3,0,-1);
           glVertex3f(3,0,1);

           glVertex3f(3,-0.4,-1);
           glVertex3f(3,-0.4,1); 
        glEnd();


//==============================================================
        //retângulo principal do carrinho
        glPushMatrix(); 
		switch(cor){
		case 0://vermelho
			glColor3ub(255, 0, 0); 
		break;

		case 1://azul
			glColor3ub(0, 160, 255); 
		break;

		case 2://verde
			glColor3ub(0, 100, 0);
		break;

		case 3://roxo
			glColor3ub(148, 0, 211);
		break;

		case 4://laranja
			glColor3ub(255, 140, 0);
		break;

		case 5://rosa
			glColor3ub(255, 105, 180);
		break;

		case 6://amarelo
			glColor3ub(255, 255, 0);
		break;
	}
	        glScalef(1.5,0.5,1); 
       	    glutSolidCube(4);
        glPopMatrix(); 
    
//============================================================== 
//=================================
		//faixas 
		glColor3ub(255, 255, 255);    
	    glBegin(GL_QUADS);
	
		//no capo  
	    glVertex3f(1,1,-1.25);
            glVertex3f(3,1,-1.25);
            glVertex3f(3,1,-0.25);
            glVertex3f(1,1,-0.25);

            glVertex3f(1,1,0.25);
            glVertex3f(3,1,0.25);
            glVertex3f(3,1,1.25);
            glVertex3f(1,1,1.25);
       glEnd();  

//=================================
//==============================================================    	    
        glBegin(GL_QUADS);
             //"vidro" esquerdo
            	switch(cor){
		case 0://vermelho
			glColor3ub(255, 0, 0); 
		break;

		case 1://azul
			glColor3ub(0, 160, 255); 
		break;

		case 2://verde
			glColor3ub(0, 100, 0);
		break;

		case 3://roxo
			glColor3ub(148, 0, 211);
		break;

		case 4://laranja
			glColor3ub(255, 140, 0);
		break;

		case 5://rosa
			glColor3ub(255, 105, 180);
		break;

		case 6://amarelo
			glColor3ub(255, 255, 0);
		break;
	}       
            glVertex3f(-3,1,-2);
            glVertex3f(-2,3,-1.5);
            glVertex3f(0,3,-1.5);
            glVertex3f(1,1,-2); 
                             
            //vidro esquerdo
            glColor3ub(0, 0, 0);   
            glVertex3f(-2.5,1,-2);
            glVertex3f(-1.5,3,-1.5);
            glVertex3f(-0.5,3,-1.5);
            glVertex3f(0.5,1,-2);

            //"vidro" direito
            	switch(cor){
		case 0://vermelho
			glColor3ub(255, 0, 0); 
		break;

		case 1://azul
			glColor3ub(0, 160, 255); 
		break;

		case 2://verde
			glColor3ub(0, 100, 0);
		break;

		case 3://roxo
			glColor3ub(148, 0, 211);
		break;

		case 4://laranja
			glColor3ub(255, 140, 0);
		break;

		case 5://rosa
			glColor3ub(255, 105, 180);
		break;

		case 6://amarelo
			glColor3ub(255, 255, 0);
		break;
	}       
            glVertex3f(-3,1,2);
            glVertex3f(-2,3,1.5);
            glVertex3f(0,3,1.5);
            glVertex3f(1,1,2);     

           
            //vidro direito
            glColor3ub(0, 0, 0);   
            glVertex3f(-2.5,1,2);
            glVertex3f(-1.5,3,1.5);
            glVertex3f(-0.5,3,1.5);
            glVertex3f(0.5,1,2);      
//==============================================================  

            glColor3ub(0, 0, 0); 
            //vidro dianteiro
            glVertex3f(0,3,1);
            glVertex3f(0,3,-1);
            glVertex3f(1,1,-1.5);
            glVertex3f(1,1,1.5);

            	switch(cor){
		case 0://vermelho
			glColor3ub(255, 0, 0); 
		break;

		case 1://azul
			glColor3ub(0, 160, 255); 
		break;

		case 2://verde
			glColor3ub(0, 100, 0);
		break;

		case 3://roxo
			glColor3ub(148, 0, 211);
		break;

		case 4://laranja
			glColor3ub(255, 140, 0);
		break;

		case 5://rosa
			glColor3ub(255, 105, 180);
		break;

		case 6://amarelo
			glColor3ub(255, 255, 0);
		break;
	}      
            //"vidro" dianteiro
            glVertex3f(0,3,1.5);
            glVertex3f(0,3,-1.5);
            glVertex3f(1,1,-2);
            glVertex3f(1,1,2);            

            //"vidro" traseiro
            glVertex3f(-3,1,2);
            glVertex3f(-3,1,-2);
            glVertex3f(-2,3,-1.5);
            glVertex3f(-2,3,1.5);

            //teto
            glVertex3f(-2,3,-1.5);
            glVertex3f(-2,3,1.5);
            glVertex3f(0,3,1.5);
            glVertex3f(0,3,-1.5);

            glColor3ub(0, 0, 0); 
        
            //vidro traseiro
            glVertex3f(-3,1,1.5);
            glVertex3f(-3,1,-1.5);
            glVertex3f(-2,3,-1);
            glVertex3f(-2,3,1);
        glEnd();

//==============================================================     
 /*
        //faixas 
        glColor3ub(255, 255, 255);    
        glBegin(GL_QUADS);
            //no teto
        /*  glVertex3f(-2,3,-1.25);
            glVertex3f(0,3,-1.25);
            glVertex3f(0,3,-0.25);
            glVertex3f(-2,3,-0.25);

            glVertex3f(-2,3,0.25);
            glVertex3f(0,3,0.25);
            glVertex3f(0,3,1.25);
            glVertex3f(-2,3,1.25);
         */
 /*       
            //no capo  
		    glVertex3f(1,1,-1.25);
            glVertex3f(3,1,-1.25);
            glVertex3f(3,1,-0.25);
            glVertex3f(1,1,-0.25);

            glVertex3f(1,1,0.25);
            glVertex3f(3,1,0.25);
            glVertex3f(3,1,1.25);
            glVertex3f(1,1,1.25);
       glEnd();     
*/
//============================================================== 
       //luz traseira        
        glColor3ub(0, 0, 0);
        glBegin(GL_QUADS);
            //esquerda
            glVertex3f(-3,-0.5,-2);
            glVertex3f(-3,-0.5,-1);
            glVertex3f(-3,0.5,-1);
            glVertex3f(-3,0.5,-2);
        glEnd();
        
        glBegin(GL_QUADS);
            //direita
            glVertex3f(-3,-0.5,1);
            glVertex3f(-3,-0.5,2);
            glVertex3f(-3,0.5,2);
            glVertex3f(-3,0.5,1);
        glEnd();
 
//============================================================== 
             
    glPopMatrix();
glPopMatrix(); 
	
}

//============================================================================================================================
//Gera os obstáculos da pista reta
void obstaculos_reta()
{
	
			if(posz_obstaculo>=limitez_obstaculo) // se o obstaculo saiu da tela, gera um novo
			{
					posx_obstaculo=9;
					posx_obstaculo = rand()%(lpista*2 - 2) - lpista+1; //sorteia de -9 a 9 (largura da pista)
					//printf("x_obstaculo = %.2f\n", posx_obstaculo);
					posz_obstaculo = -400; //posição em z que o obstáculo gerado aparece (atrás do céu)
					
					/*
					
						Sortear a cor do obstáculo aqui 
					*/	
					cor_sort=rand()%7;
			}	
			else //se existe um obstáculo na tela, verifica colisão e posiciona na tela
			{
					verifica_colisao();
					
					if(posz_obstaculo > -280)
					{	
						//movimenta o obstáculo
						glPushMatrix();	
							glTranslated(posx_obstaculo, 0, posz_obstaculo);
							carrinho(cor_sort); 
						glPopMatrix();	
					}	
			}
}

//========================================================================================================== 
//Gera os obstáculos da pista esquerda
void obstaculos_esquerda()
{
	if(ang_obstaculo<=-10 && fim_pista_curva>=100) //gera um novo obstáculo se o atual saiu da tela (e se a próxima pista não entrou)
	{
		ang_obstaculo=ang_max_obstaculo;
		posx_obstaculo= rand()%(lpista*2 - 2) - lpista+1; //sorteia de -9 a 9 (largura da pista)
		//printf("gerou novo obstaculo (esquerda).\n fim_pista_curva = %.2f\nprox_pista = %d\n\n", fim_pista_curva, prox_pista);
		
		/*
					
			Sortear a cor do obstáculo aqui 
					
		*/	
		cor_sort=rand()%7;
	}
	
	else //existe um obstáculo na tela
	{
		verifica_colisao();
	
		//desenha e posiciona o obstáculo na tela
			glPushMatrix();
					glTranslated(posx_obstaculo, 0, 0);
					glPushMatrix();
					
						glTranslated(-raio,0, 20); //posiciona o obstáculo na origem da pista curva (esquerda)
						glRotatef(ang_obstaculo, 0, 1, 0); //"movimento" do obstáculo na pista curva (esquerda)
						
						glPushMatrix();
							glTranslated(raio, 0, 0);
							carrinho(cor_sort);
					    glPopMatrix();
						
					glPopMatrix();
			glPopMatrix();			
	}
	
}

//==========================================================================================================
//Gera os obstáculos da pista direita 
void obstaculos_direita()
{

	if(ang_obstaculo<=-10  && fim_pista_curva>=100) //gera um novo obstáculo se o atual saiu da tela
	{
		ang_obstaculo=ang_max_obstaculo;
		posx_obstaculo = rand()%(lpista*2 - 2) - lpista+1; //sorteia de -9 a 9 (largura da pista)
		//printf("gerou novo obstaculo (direita).\nfim_pista_curva = %.2f\nprox_pista = %d\n\n", fim_pista_curva, prox_pista);
		
		/*
					
			Sortear a cor do obstáculo aqui 
					
		*/			
		cor_sort=rand()%7;
	}
	
	else //existe um obstáculo na tela
	{
		verifica_colisao();
	
		//desenha e posiciona o obstáculo na tela
			glPushMatrix();
					glTranslated(posx_obstaculo, 0, 0);
					glPushMatrix();
					
						glTranslated(raio,0, 20); //posiciona o obstáculo na origem da pista curva (esquerda)
						glRotatef(-ang_obstaculo, 0, 1, 0); //"movimento" do obstáculo na pista curva (esquerda)
						
						glPushMatrix();
							glTranslated(-raio, 0, 0);
							carrinho(cor_sort);
					    glPopMatrix();
						
					glPopMatrix();
			glPopMatrix();			
	}
}

//========================================================================================================== 
//Verifica se o carrinho colidiu com o obstáculo
int verifica_colisao()
{
	float x_min, x_max;
	float z_min, z_max;
	
	//verifica colisão na pista reta
	if(pista_atual == 0)
	{
			//limites do obstáculo - colisão
			x_min = posx_obstaculo-lcar;
			x_max = posx_obstaculo+lcar;
			z_min = posz_obstaculo+acar;
			z_max = posz_obstaculo-acar;
			
			
				if(poscarx>=x_min  && poscarx<=x_max && z_min>=poscarz && z_max<=poscarz+2) //houve colisão
				{
					if (z_min >= poscarz){  //colidiu de frente
						colidiu_frente=1;
						posx=0;//reseta a camera
						ox=0;
						poscarx=0;//reseta a posicao do carro
						tela=2;
					}	//printf("(frente) ");
				
					if (poscarx <= x_min){//colidiu à esquerda
						colidiu_esq=1;
						posx=0;
						ox=0;
						poscarx=0;
						tela=2;
					}	  //printf("(esq) ");
					
					if (poscarx >= x_max){ //colidiu à direita
						colidiu_dir=1;
						posx=0;
						ox=0;
						poscarx=0;
						tela=2;
					}	//printf("(dir) ");
											
					//printf("colidiu [%d]\n", cont);			
				}
					
				else //não colidiu
				{
					colidiu_frente=0;	colidiu_dir=0;	colidiu_esq=0;
				}	
	}

	//verifica colisão nas pistas curvas
	if(pista_atual == 1 || pista_atual == 2)
	{
		x_min = posx_obstaculo-lcar;
		x_max = posx_obstaculo+lcar;
		z_min = (ang_obstaculo*cpista_curva)/360 + acar;
		z_max = (ang_obstaculo*cpista_curva)/360 - acar;
		
		if(poscarx>=x_min  && poscarx<=x_max && z_min>=poscarz && z_max<=poscarz+2)
		{
			colidiu_frente=1; //colidiu_esq=1; //colidiu_dir=1;
			posx=0;//reseta a camera
			ox=0;
			poscarx=0;
			tela=2;

			return 1; //colidiu
		}	
		else
		{
			colidiu_frente=0; //colidiu_esq=0; //colidiu_dir=0;
			return 0; //não colidiu
		}		
	}
}

//============================================================================================================================
//Desenha o Portal
void portal()
{
	//Portal
	glPushMatrix();
		 
		int lp = 2; //largura do portal
		int altura_p=20; //altura do portal
		float ang_rad; 

		if(z_portal>=20)	z_portal=-280;
		
	//===================================================================
		/*
		if(pista_atual == 1 || 2)
		{
			z_portal = -raio;
			if(pista_atual==1)
			{
				glTranslated(-raio,0, 20); //posiciona o portal
				glRotatef(ang_esq, 0, 1, 0); //rotaciona o portal
			
			}	
			if(pista_atual==2)
			{
				glTranslated(raio,0, 20); //posiciona o portal
				glRotatef(ang_dir, 0, 1, 0); //rotaciona o portal
			}	
		}
		//if(pista_atual == 1 || 2) z_portal=0;
		*/
	//===================================================================
		//faixa branca no chão
		glPushMatrix();
			glColor3ub(255, 255, 255);
			
			for(float i=-lpista-lp+0.1; i<=lpista+lp-0.1; i++)
			{
				glPushMatrix();
					glTranslated(i, 0.3, z_portal+2); 
					glutSolidCube(1);
				glPopMatrix();
			}
		glPopMatrix();

		//parte vertical do portal
		glColor3ub(255, 0, 0);
		for(int i=0; i<altura_p; i++)
		{
			glPushMatrix();
				glTranslated(-lpista-lp, i ,z_portal); 
				glutSolidCube(lp);
			glPopMatrix();
			
			glPushMatrix();
				glTranslated(lpista+lp, i ,z_portal); 
				glutSolidCube(lp);
			glPopMatrix();
		}
		
		//parte curva do portal
		for(int i=0; i<=180; i+=1)
		{
			ang_rad = i*PI/180; //converte o ângulo para radianos
			glPushMatrix();
				glTranslated(cos(ang_rad)*(lpista+2), sin(ang_rad)*(lpista+2)+altura_p, z_portal); 
				glRotatef(i, 0, 0, 1);
				glutSolidCube(lp);
			glPopMatrix();
		}
					
	glPopMatrix();
	
}
//============================================================================================================================


