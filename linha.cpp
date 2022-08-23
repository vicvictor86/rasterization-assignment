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

#include<iostream> 
#include <bits/stdc++.h>
#include <queue>
using namespace std; 

#include "transformations.h"

int quantityClicks = 0;

int width = 512, height = 512; //Largura e altura da janela

string drawStatus = "TRIANGLE";

ponto pointsToDrawnTriangle[3] = {};
ponto pointsToDrawnLine[2] = {};

ponto firstPointPolygon = {};

ponto squareVertices[4] = {};
ponto triangleVertices[3] = {};

ponto centerCircle = {};
int circleRadius = 0;

queue<ponto> pointsToDrawnPolygonQueue;

// Declaracoes forward das funcoes utilizadas
void init(void);
void reshape(int w, int h);
void display(void);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void SpecialFuncInput(int key, int x, int y);

// Funcao que percorre a lista de pontos desenhando-os na tela
void drawPontos();

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
        case 't': 
            drawStatus = "TRIANGLE";
            resetDrawnInformations();

            printf("Desenhar triangulo\n");
        break;
        case 'l': 
            drawStatus = "LINE_BRESENHAM";
            resetDrawnInformations();

            printf("Desenhar linha usando Bresenham\n");
        break;
        case 'o': 
            drawStatus = "LINE_REDUCTION_OCTAVE";
            resetDrawnInformations();

            printf("Desenhar linha usando reducao ao primeiro octante \n");
        break;
        case 's': 
            drawStatus = "SQUARE";
            resetDrawnInformations();

            printf("Desenhar quadrado\n");
        break;
        case 'p': 
            drawStatus = "POLYGON";
            resetDrawnInformations();

            printf("Desenhar poligono\n");
        break;
        case 'c': 
            drawStatus = "CIRCLE";
            resetDrawnInformations();

            printf("Desenhar circulo\n");
        break;
        case 'd': 
            rotation(everyLines, 45);
            glutPostRedisplay();
            printf("Rotacao do poligono\n");
        break;
        case 'x': 
            reflectionOverX(pontos);
            glutPostRedisplay();
            printf("Reflexao sobre eixo x\n");
        break;
        case 'y': 
            reflectionOverY(pontos);
            glutPostRedisplay();
            printf("Reflexao sobre eixo y\n");
        break;
        case 'z': 
            reflectionOverOrigin(pontos);
            glutPostRedisplay();
            printf("Reflexao sobre a origem\n");
        break;
        case 'e': 
            scale(everyLines, 2, 2);
            glutPostRedisplay();
            printf("Escalonando o poligono\n");
        break;
        case 'v': 
            shearX(everyLines, 2);
            glutPostRedisplay();
            printf("Cisalhamento sobre eixo x\n");
        break;
        case 'b': 
            shearY(everyLines, 2);
            glutPostRedisplay();
            printf("Cisalhamento sobre eixo y\n");
        break;
        case 'n': 
            shear(everyLines, 2, 2);
            glutPostRedisplay();
            printf("Cisalhamento sobre eixo x e y\n");
        break;
    }
}

void offSetCenterWindow(int * x, int * y){
    *x -= width/2;
    *y += height/2;
}

void registerPoint(ponto * pointsToDrawn, int x, int y){
    int yWithOffset = height - y;

    pointsToDrawn[quantityClicks].x = x;
    pointsToDrawn[quantityClicks].y = yWithOffset;

    quantityClicks++;
    printf("Ponto registrado %d %d\n", x, yWithOffset);
}

void registerPolygonPoint(int x, int y){
    int yWithOffset = height - y;

    pointsToDrawnPolygonQueue.push({x, yWithOffset, NULL});
    quantityClicks++;

    printf("Ponto registrado %d %d\n", x, yWithOffset);
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

            case GLUT_RIGHT_BUTTON:
                if (state == GLUT_DOWN) {
                    removeAllPoints();
                    removeAllLines();

                    quantityClicks = 0;

                    clearQueue(&pointsToDrawnPolygonQueue);
                    
                    glClear(GL_COLOR_BUFFER_BIT); 
                    glLoadIdentity(); 
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

// void floodFill(int x, int y, double color, double newColor){
//     float pixels[4];
//     glReadPixels(x, y, width, height, GL_RGB, GL_FLOAT, pixels);
// }
