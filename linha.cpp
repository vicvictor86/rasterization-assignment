//*********************************************************************
//  Codigo exemplo para aula pratica de Computacao Grafica 
//  Assunto: Rasterizacao e Transformacoes Geometricas
//
//  Autor: Prof. Laurindo de Sousa Britto Neto
//*********************************************************************

// Bibliotecas utilizadas pelo OpenGL
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

// Biblioteca com funcoes matematicas
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include<iostream> 
#include <bits/stdc++.h>
using namespace std; 

int quantityClicks = 0;

int width = 512, height = 512; //Largura e altura da janela

string drawStatus = "TRIANGLE";

// Estrututa de dados para o armazenamento dinamico dos pontos
// selecionados pelos algoritmos de rasterizacao
struct ponto{
    int x;
    int y;
    ponto * prox;
};

ponto pointsToDrawnTriangle[3] = {};
ponto pointsToDrawnLine[2] = {};

// Lista encadeada de pontos
// indica o primeiro elemento da lista
ponto * pontos = NULL;


// Funcao para armazenar um ponto na lista
// Armazena como uma Pilha (empilha)
ponto * pushPonto(int x, int y){
	ponto * pnt;
	pnt = new ponto;
	pnt->x = x;
	pnt->y = y;
	pnt->prox = pontos;
	pontos = pnt;
	return pnt;
}

// Funcao para desarmazenar um ponto na lista
// Desarmazena como uma Pilha (desempilha)
ponto * popPonto(){
	ponto * pnt;
	pnt = NULL;
	if(pontos != NULL){
		pnt = pontos->prox;
		delete pontos;
		pontos = pnt;
	}
	return pnt;
}

// Declaracoes forward das funcoes utilizadas
void init(void);
void reshape(int w, int h);
void display(void);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);

// Funcao que implementa o Algoritmo Imediato para rasterizacao de retas
void retaImediata(double x1,double y1,double x2,double y2);
// Funcao que percorre a lista de pontos desenhando-os na tela
void drawPontos();
void bresenham(int x1,int y1,int x2,int y2);
void firstOctaveReduction(int x1,int y1,int x2,int y2);

// Funcao Principal do C
int main(int argc, char** argv){
    printf("Desenhar triangulo\n");
    glutInit(&argc, argv); // Passagens de parametro C para o glut
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB); // Selecao do Modo do Display e do Sistema de cor utilizado
    glutInitWindowSize (width, height);  // Tamanho da janela do OpenGL
    glutInitWindowPosition (100, 100); //Posicao inicial da janela do OpenGL
    glutCreateWindow ("Rasterizacao"); // Da nome para uma janela OpenGL
    init(); // Chama funcao init();
    glutReshapeFunc(reshape); //funcao callback para redesenhar a tela
    glutKeyboardFunc(keyboard); //funcao callback do teclado
    glutMouseFunc(mouse); //funcao callback do mouse
    glutDisplayFunc(display); //funcao callback de desenho
    glutMainLoop(); // executa o loop do OpenGL
    return 0; // retorna 0 para o tipo inteiro da funcao main();
}

// Funcao com alguns comandos para a inicializacao do OpenGL;
void init(void){
    glClearColor(1.0, 1.0, 1.0, 1.0); //Limpa a tela com a cor branca;
}

void reshape(int w, int h)
{
	// Reinicializa o sistema de coordenadas
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Definindo o Viewport para o tamanho da janela
	glViewport(0, 0, w, h);
	
	width = w;
	height = h;
    glOrtho (0, w, 0, h, -1 ,1);  

   // muda para o modo GL_MODELVIEW (n�o pretendemos alterar a projec��o
   // quando estivermos a desenhar na tela)
	glMatrixMode(GL_MODELVIEW);
}

// Funcao usada na funcao callback para utilizacao das teclas normais do teclado
void keyboard(unsigned char key, int x, int y){
    switch (key) { // key - variavel que possui valor ASCII da tecla precionada
        case 27: // codigo ASCII da tecla ESC
            exit(0); // comando pra finalizacao do programa
        break;
        case 84:
            drawStatus = "TRIANGLE";
            printf("Desenhar triangulo\n");
        break;
        case 108:
            drawStatus = "LINE_BRESENHAM";
            printf("Desenhar linha usando Bresenham\n");
        break;
        case 111:
            drawStatus = "LINE_REDUCTION_OCTAVE";
            printf("Desenhar linha usando reducao ao primeiro octante \n");
        break;
    }
}

//Funcao usada na funcao callback para a utilizacao do mouse
void mouse(int button, int state, int x, int y)
{
   switch (button) {
      case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN) {
            if(drawStatus == "TRIANGLE" && quantityClicks < 3){
                pointsToDrawnTriangle[quantityClicks].x = x;
                pointsToDrawnTriangle[quantityClicks].y = height - y;
                quantityClicks++;
                printf("Ponto registrado %d %d\n", x, y);

            } else if((drawStatus == "LINE_BRESENHAM" || drawStatus == "LINE_REDUCTION_OCTAVE") && quantityClicks < 2){
                pointsToDrawnLine[quantityClicks].x = x;
                pointsToDrawnLine[quantityClicks].y = height - y;
                quantityClicks++;
                printf("Ponto registrado %d %d\n", x, y);

            } else {
                printf("Numero maximo de pontos registrados\n");
            }
        }
        break;

        // case GLUT_MIDDLE_BUTTON:
        //     if (state == GLUT_DOWN) {
        //         glutPostRedisplay();
        //     }
        // break;

        case GLUT_RIGHT_BUTTON:
            if (state == GLUT_DOWN) {
                glutPostRedisplay();
            }
        break;

        default:
            break;
   }
}

// Funcao usada na funcao callback para desenhar na tela
void display(void){
    glClear(GL_COLOR_BUFFER_BIT); //Limpa o buffer de cores e reinicia a matriz
    glLoadIdentity();
    
    glColor3f (0.0, 0.0, 0.0); // Seleciona a cor default como preto

    if(quantityClicks > 0){
        if(drawStatus == "TRIANGLE"){
            firstOctaveReduction(pointsToDrawnTriangle[0].x, pointsToDrawnTriangle[0].y, pointsToDrawnTriangle[1].x, pointsToDrawnTriangle[1].y);
            firstOctaveReduction(pointsToDrawnTriangle[1].x, pointsToDrawnTriangle[1].y, pointsToDrawnTriangle[2].x, pointsToDrawnTriangle[2].y);
            firstOctaveReduction(pointsToDrawnTriangle[2].x, pointsToDrawnTriangle[2].y, pointsToDrawnTriangle[0].x, pointsToDrawnTriangle[0].y);
        } else if(drawStatus == "LINE_BRESENHAM"){
            bresenham(pointsToDrawnLine[0].x, pointsToDrawnLine[0].y, pointsToDrawnLine[1].x, pointsToDrawnLine[1].y);
        } else if(drawStatus == "LINE_REDUCTION_OCTAVE"){
            firstOctaveReduction(pointsToDrawnLine[0].x, pointsToDrawnLine[0].y, pointsToDrawnLine[1].x, pointsToDrawnLine[1].y);
        }

        // bresenham(x_1, y_1, x_2, y_2);
        // bresenham(x_2, y_2, x_3, y_3);
        // bresenham(x_3, y_3, x_1, y_1);

        // retaImediata(x_1, y_1, x_2, y_2);
        // retaImediata(x_2, y_2, x_3, y_3);
        // retaImediata(x_3, y_3, x_1, y_1);
        
        drawPontos();
        quantityClicks = 0;
    }

    glutSwapBuffers(); // manda o OpenGl renderizar as primitivas

}

//Funcao que desenha os pontos contidos em uma lista de pontos
void drawPontos(){
    ponto * pnt;
    pnt = pontos;
    glBegin(GL_POINTS); // Seleciona a primitiva GL_POINTS para desenhar
        while(pnt != NULL){
            glVertex2i(pnt->x,pnt->y);
            pnt = pnt->prox;
        }
    glEnd();  // indica o fim do desenho
}

void bresenham(int x1,int y1,int x2,int y2){
    int delta_x = x2 - x1;
    int delta_y = y2 - y1;
    int distance = 2 * delta_y - delta_x;
    
    int incrementE = 2 * delta_y;
    int incrementNE = 2 * (delta_y - delta_x);
    
    int xi = x1;
    int yi = y1;

    bool firstExtremity = true;
    while(xi < x2){
        if(!firstExtremity){
            if(!firstExtremity){
            if(distance <= 0){
            distance += incrementE;
            }else {
                distance += incrementNE;
                yi++;
            }
            xi++;
        }

        pontos = pushPonto((int)xi, (int)yi);
        // printf("xiyi(%d,%d)\n", xi, yi);

        firstExtremity = false;
    }
}

void firstOctaveReduction(int x1, int y1, int x2, int y2){
    bool declive = false;
    bool simetric = false;

    int delta_x = x2 - x1;
    int delta_y = y2 - y1;

    int productDeltas = delta_x * delta_y;

    if(productDeltas < 0){
        y1 *= -1;
        y2 *= -1;
        delta_y = y2 - y1;
        simetric = true;
    }

    bool deltaXMinorDeltaY = abs(delta_x) < abs(delta_y);
    if(deltaXMinorDeltaY){
        int aux1 = x1;
        x1 = y1;
        y1 = aux1;

        int aux2 = x2;
        x2 = y2;
        y2 = aux2;

        int auxDelta = delta_x;
        delta_x = delta_y;
        delta_y = auxDelta;

        declive = true;
    }

    bool xFirstExtremityBiggerSecond = x1 > x2;
    if(xFirstExtremityBiggerSecond){
        int auxX = x1;
        x1 = x2;
        x2 = auxX;

        int auxY = y1;
        y1 = y2;
        y2 = auxY;

        delta_x = x2 - x1;
        delta_y = y2 - y1;
    }

    int distance = 2 * delta_y - delta_x;
    int incrementE = 2 * delta_y;
    int incrementNE = 2 * (delta_y - delta_x);

    int xi = x1;
    int yi = y1;

    int tempXi = xi;
    int tempYi = yi;

    bool firstExtremity = true;
    // printf("Primeiro xiyi(%d,%d)\n", tempXi, tempYi);

    while(xi < x2){
        if(!firstExtremity){
            if(!firstExtremity){
            if(distance <= 0){
                    distance += incrementE;
                }else {
                    distance += incrementNE;
                    yi++;
            }
            xi++;
            }
            xi++;
        }

        if(declive){
            tempXi = yi;
            tempYi = xi;
        } else {
            tempXi = xi;
            tempYi = yi;
        } else {
            tempXi = xi;
            tempYi = yi;
        }

        if(simetric){
            tempYi *= -1;
        } 
        
        pontos = pushPonto(tempXi, tempYi);
        // printf("xiyi(%d,%d)\n", tempXi, tempYi);

        firstExtremity = false;
    }
}

void retaImediata(double x1,double y1,double x2,double y2){
    double m, b, yd, xd;
    double xmin, xmax,ymin,ymax;
    //Armazenando os extremos para desenho
    pontos = pushPonto((int)x1,(int)y1);
    pontos = pushPonto((int)x2,(int)y2);

    if(x2-x1 != 0){ //Evita a divis�o por zero
        m = (y2-y1)/(x2-x1);
        b = y1 - (m*x1);

        if(m>=-1 && m <= 1){ // Verifica se o declive da reta tem tg de -1 a 1, se verdadeira calcula incrementando x
            xmin = (x1 < x2)? x1 : x2;
            xmax = (x1 > x2)? x1 : x2;

            for(int x = (int)xmin+1; x < xmax; x++){
                yd = (m*x)+b;
                yd = floor(0.5+yd);
                pontos = pushPonto(x,(int)yd);
            }
        }else{ // Se menor que -1 ou maior que 1, calcula incrementado os valores de y
            ymin = (y1 < y2)? y1 : y2;
            ymax = (y1 > y2)? y1 : y2;

            for(int y = (int)ymin + 1; y < ymax; y++){
                xd = (y - b)/m;
                xd = floor(0.5+xd);
                pontos = pushPonto((int)xd,y);
            }
        }

    }else{ // se x2-x1 == 0, reta perpendicular ao eixo x
        ymin = (y1 < y2)? y1 : y2;
        ymax = (y1 > y2)? y1 : y2;
        for(int y = (int)ymin + 1; y < ymax; y++){
            pontos = pushPonto((int)x1,y);
        }
    }
}

