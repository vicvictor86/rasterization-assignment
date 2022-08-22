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
#include <cstdio>
#include <cstdlib>

#include<iostream> 
#include <bits/stdc++.h>
#include <queue>
using namespace std; 

#include "transformations.h"

int quantityClicks = 0;

int width = 512, height = 512; //Largura e altura da janela

string drawStatus = "TRIANGLE";

// Estrututa de dados para o armazenamento dinamico dos pontos
// selecionados pelos algoritmos de rasterizacao


ponto pointsToDrawnTriangle[3] = {};
ponto pointsToDrawnLine[2] = {};
ponto pointsToDrawnPolygon[2] = {};

ponto firstPointPolygon = {};

ponto squareVertices[4] = {};
ponto triangleVertices[3] = {};

ponto centerCircle = {};
int circleRadius = 0;

queue<ponto> pointsToDrawnPolygonQueue;

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

void removeAllPoints(){
    while(pontos != NULL){
        popPonto();
    }
}

// Declaracoes forward das funcoes utilizadas
void init(void);
void reshape(int w, int h);
void display(void);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void SpecialFuncInput(int key, int x, int y);

// Funcao que implementa o Algoritmo Imediato para rasterizacao de retas
void retaImediata(double x1,double y1,double x2,double y2);
// Funcao que percorre a lista de pontos desenhando-os na tela
void drawPontos();
void bresenham(int x1,int y1,int x2,int y2);
void firstOctaveReduction(int x1,int y1,int x2,int y2);
void circumferenceRasterization(int radius, int x, int y);

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
    glutSpecialFunc(SpecialFuncInput);
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
    glOrtho (-(w/2), (w/2), -(w/2), (w/2), -1 ,1); // Centro da tela no meio

    // glOrtho (0, w, 0, h, -1 ,1);  

   // muda para o modo GL_MODELVIEW (n�o pretendemos alterar a projec��o
   // quando estivermos a desenhar na tela)
	glMatrixMode(GL_MODELVIEW);
}

template <typename T>
void clearQueue(queue<T> * queueToClear){
    while(!queueToClear->empty()){
        queueToClear->pop();
    }
}

void resetDrawnInformations() {
    quantityClicks = 0;
    clearQueue(&pointsToDrawnPolygonQueue);
}

void SpecialFuncInput(int key, int x, int y){
    switch(key) {
        case GLUT_KEY_UP:
            translation(pontos, 0, 20);
            glutPostRedisplay();
            printf("Translacao para cima\n");
        break; 
        case GLUT_KEY_DOWN:
            translation(pontos, 0, -20);
            glutPostRedisplay();
            printf("Translacao para baixo\n");
        break;  
        case GLUT_KEY_LEFT:
            translation(pontos, -20, 0);
            glutPostRedisplay();
            printf("Translacao para esquerda\n");
        break;  
        case GLUT_KEY_RIGHT:
            translation(pontos, 20, 0);
            glutPostRedisplay();
            printf("Translacao para direita\n");
        break;   
    }
}

// Funcao usada na funcao callback para utilizacao das teclas normais do teclado
void keyboard(unsigned char key, int x, int y){
    switch (key) { // key - variavel que possui valor ASCII da tecla precionada
        case 27: // codigo ASCII da tecla ESC
            exit(0); // comando pra finalizacao do programa
        break;
        case 116: // codigo ASCII da tecla 't'
            drawStatus = "TRIANGLE";
            resetDrawnInformations();

            printf("Desenhar triangulo\n");
        break;
        case 108: // codigo ASCII da tecla 'l'
            drawStatus = "LINE_BRESENHAM";
            resetDrawnInformations();

            printf("Desenhar linha usando Bresenham\n");
        break;
        case 111: // codigo ASCII da tecla 'o'
            drawStatus = "LINE_REDUCTION_OCTAVE";
            resetDrawnInformations();

            printf("Desenhar linha usando reducao ao primeiro octante \n");
        break;
        case 115: // codigo ASCII da tecla 's'
            drawStatus = "SQUARE";
            resetDrawnInformations();

            printf("Desenhar quadrado\n");
        break;
        case 112: // codigo ASCII da tecla 'p'
            drawStatus = "POLYGON";
            resetDrawnInformations();

            printf("Desenhar poligono\n");
        break;
        case 99: // codigo ASCII da tecla 'c'
            drawStatus = "CIRCLE";
            resetDrawnInformations();

            printf("Desenhar circulo\n");
        break;
        case 100: // codigo ASCII da tecla 'd'
            ponto pontosToRotate[5] = {{5,4},{4,3},{4,1},{6,1},{6,3}};
            rotation(pontos, 5, 45);
            glutPostRedisplay();
            printf("Gira\n");
        break;
    }
}

void offSetCenterWindow(int * x, int * y){
    *x -= width/2;
    *y += height/2;
}

void registerPoint(ponto * pointsToDrawn, int x, int y){
    pointsToDrawn[quantityClicks].x = x;
    pointsToDrawn[quantityClicks].y = height - y;
    quantityClicks++;
    printf("Ponto registrado %d %d\n", x, y);
}

void registerPolygonPoint(int x, int y){
    pointsToDrawnPolygonQueue.push({x, height - y, NULL});
    quantityClicks++;
    printf("Ponto registrado %d %d\n", x, y);
}

//Funcao usada na funcao callback para a utilizacao do mouse
void mouse(int button, int state, int x, int y)
{
    offSetCenterWindow(&x, &y);

    switch (button) {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN) {
                bool readyToDraw = false;
                if(drawStatus == "TRIANGLE" && quantityClicks < 3){
                    registerPoint(pointsToDrawnTriangle, x, y);
                    readyToDraw = quantityClicks >= 3 ? true : false;

                } else if((drawStatus == "LINE_BRESENHAM" || drawStatus == "LINE_REDUCTION_OCTAVE" || drawStatus == "SQUARE") && quantityClicks < 2){
                    registerPoint(pointsToDrawnLine, x, y);

                    readyToDraw = quantityClicks >= 2 ? true : false;

                } else if(drawStatus == "POLYGON"){
                    if(pointsToDrawnPolygonQueue.empty()){
                        firstPointPolygon.x = x;
                        firstPointPolygon.y = height - y;
                        
                        registerPolygonPoint(x, y);
                    } else {
                        registerPolygonPoint(x, y);

                        glutPostRedisplay();
                    }
                } else if(drawStatus == "CIRCLE"){
                    if(quantityClicks == 0){
                        centerCircle.x = x;
                        centerCircle.y = height - y;
                        quantityClicks++;

                        printf("Centro do circulo %d %d registrado\n", x, centerCircle.y);
                    } else {
                        int yWithOffset = height - y;
                        int powX = pow(x - centerCircle.x, 2);
                        int powY = pow(yWithOffset - centerCircle.y, 2);

                        circleRadius = sqrt(powX + powY);

                        printf("Circulo com raio %d registrado\n", circleRadius);
                        readyToDraw = true;
                    }
                }

                if(readyToDraw){
                    glutPostRedisplay();
                    printf("Objeto rasterizado\n");
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
                    removeAllPoints();
                    quantityClicks = 0;

                    clearQueue(&pointsToDrawnPolygonQueue);
                    
                    glClear(GL_COLOR_BUFFER_BIT); // Clean the screen and the depth buffer
                    glLoadIdentity(); // Reset The Projection Matrix
                    glutPostRedisplay();
                    
                    printf("Tela limpada\n");
                }
            break;

            default:
                break;
    }
}

void defineVertexTriangle(){
    triangleVertices[0].x = pointsToDrawnTriangle[0].x;
    triangleVertices[0].y = pointsToDrawnTriangle[0].y;
    triangleVertices[1].x = pointsToDrawnTriangle[1].x;
    triangleVertices[1].y = pointsToDrawnTriangle[1].y;
    triangleVertices[2].x = pointsToDrawnTriangle[2].x;
    triangleVertices[2].y = pointsToDrawnTriangle[2].y;
}

void drawnTriangle(){
    firstOctaveReduction(pointsToDrawnTriangle[0].x, pointsToDrawnTriangle[0].y, pointsToDrawnTriangle[1].x, pointsToDrawnTriangle[1].y);
    firstOctaveReduction(pointsToDrawnTriangle[1].x, pointsToDrawnTriangle[1].y, pointsToDrawnTriangle[2].x, pointsToDrawnTriangle[2].y);
    firstOctaveReduction(pointsToDrawnTriangle[2].x, pointsToDrawnTriangle[2].y, pointsToDrawnTriangle[0].x, pointsToDrawnTriangle[0].y);
    defineVertexTriangle();
}

void defineVertexSquare(){
    squareVertices[0].x = pointsToDrawnLine[0].x;
    squareVertices[0].y = pointsToDrawnLine[0].y;
    squareVertices[1].x = pointsToDrawnLine[0].x;
    squareVertices[1].y = pointsToDrawnLine[1].y;
    squareVertices[2].x = pointsToDrawnLine[1].x;
    squareVertices[2].y = pointsToDrawnLine[1].y;
    squareVertices[3].x = pointsToDrawnLine[1].x;
    squareVertices[3].y = pointsToDrawnLine[0].y;
}

void drawnSquare(){
    firstOctaveReduction(pointsToDrawnLine[0].x, pointsToDrawnLine[0].y, pointsToDrawnLine[0].x, pointsToDrawnLine[1].y);
    firstOctaveReduction(pointsToDrawnLine[0].x, pointsToDrawnLine[1].y, pointsToDrawnLine[1].x, pointsToDrawnLine[1].y);
    firstOctaveReduction(pointsToDrawnLine[1].x, pointsToDrawnLine[1].y, pointsToDrawnLine[1].x, pointsToDrawnLine[0].y);
    firstOctaveReduction(pointsToDrawnLine[1].x, pointsToDrawnLine[0].y, pointsToDrawnLine[0].x, pointsToDrawnLine[0].y);
    defineVertexSquare();
}

// Funcao usada na funcao callback para desenhar na tela
void display(void){
    glClear(GL_COLOR_BUFFER_BIT); //Limpa o buffer de cores e reinicia a matriz
    glLoadIdentity();
    
    glColor3f (0.0, 0.0, 0.0); // Seleciona a cor default como preto

    if(quantityClicks > 0){
        if(drawStatus == "TRIANGLE" && quantityClicks >= 3){
            drawnTriangle();
            
        } else if(drawStatus == "LINE_BRESENHAM" && quantityClicks >= 2){
            bresenham(pointsToDrawnLine[0].x, pointsToDrawnLine[0].y, pointsToDrawnLine[1].x, pointsToDrawnLine[1].y);

        } else if(drawStatus == "LINE_REDUCTION_OCTAVE" && quantityClicks >= 2){
            firstOctaveReduction(pointsToDrawnLine[0].x, pointsToDrawnLine[0].y, pointsToDrawnLine[1].x, pointsToDrawnLine[1].y);

        } else if(drawStatus == "SQUARE" && quantityClicks >= 2){
            drawnSquare();

        } else if(drawStatus == "POLYGON") {
            bool clickToClosePolygonX = abs(pointsToDrawnPolygonQueue.back().x - firstPointPolygon.x) < 15;
            bool clickToClosePolygonY = abs(pointsToDrawnPolygonQueue.back().y - firstPointPolygon.y) < 15;

            if(clickToClosePolygonX && clickToClosePolygonY){
                firstOctaveReduction(pointsToDrawnPolygonQueue.front().x, pointsToDrawnPolygonQueue.front().y, firstPointPolygon.x, firstPointPolygon.y); 
                pointsToDrawnPolygonQueue.pop();

            } else {
                firstOctaveReduction(pointsToDrawnPolygonQueue.front().x, pointsToDrawnPolygonQueue.front().y, pointsToDrawnPolygonQueue.back().x, pointsToDrawnPolygonQueue.back().y);
            }

            pointsToDrawnPolygonQueue.pop();

        } else if (drawStatus == "CIRCLE") {
            circumferenceRasterization(circleRadius, centerCircle.x, centerCircle.y);
            // circumferenceRasterization(8, 0, 0);
        } else {
            printf("Quantidade de pontos marcados insuficiente\n");
            return;
        }
    
        // retaImediata(x_1, y_1, x_2, y_2);
        // retaImediata(x_2, y_2, x_3, y_3);
        // retaImediata(x_3, y_3, x_1, y_1);
        
        quantityClicks = 0;
    }
    drawPontos();

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

void clearArray(ponto arrayToClean[], int size){
    for(int i = 0; i < size; i++){
        arrayToClean[i].x = 0;
        arrayToClean[i].y = 0;
    }
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

// void floodFill(int x, int y, double color, double newColor){
//     float pixels[4];
//     glReadPixels(x, y, width, height, GL_RGB, GL_FLOAT, pixels);
// }

void circumferenceRasterization(int radius, int x, int y){
    int distance = 1 - radius;
    int deltaE = 3;
    int deltaSE = 5 - 2 * radius;
    int xi = 0;
    int yi = radius;

    pontos = pushPonto(x, radius + y);
    pontos = pushPonto(x, -radius + y);
    pontos = pushPonto(radius + x, y);
    pontos = pushPonto(-radius + x, y);

    bool firstExtremity = true;
    while(yi > xi){
        if(!firstExtremity){
            if(distance < 0){
                distance += deltaE;
                deltaE += 2;
                deltaSE += 2;
            } else {
                distance += deltaSE;
                deltaE += 2;
                deltaSE += 4;
                yi--;
            }
        }
        xi++;

        pontos = pushPonto(xi + x, yi + y);
        pontos = pushPonto(yi + x, xi + y);
        pontos = pushPonto(yi + x, -xi + y);
        pontos = pushPonto(xi + x, -yi + y);
        pontos = pushPonto(-xi + x, -yi + y);
        pontos = pushPonto(-yi + x, -xi + y);
        pontos = pushPonto(-yi + x, xi + y);
        pontos = pushPonto(-xi + x, yi + y);
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
            if(distance <= 0){
                distance += incrementE;
            }else {
                distance += incrementNE;
                yi++;
            }
            xi++;
        }

        if(declive){
            tempXi = yi;
            tempYi = xi;
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

