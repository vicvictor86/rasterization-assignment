#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "ponto.h"
#include "line.h"

// Lista encadeada de pontos
// indica o primeiro elemento da lista
ponto * pontos = NULL;

// Lista encadeada de linhas
// Indica a extremidade de cada reta criada
line * everyLines = NULL;

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

line * pushLine(int x1, int y1, int x2, int y2){
	line * lines;
	lines = new line;
	lines->x1 = x1;
	lines->y1 = y1;
	lines->x2 = x2;
	lines->y2 = y2;
	lines->prox = everyLines;
	everyLines = lines;
	return lines;
}

line * popLine(){
	line * lines;
	lines = NULL;
	if(everyLines != NULL){
		lines = everyLines->prox;
		delete everyLines;
		everyLines = lines;
	}
	return lines;
}

void removeAllLines(){
    while(everyLines != NULL){
        popLine();
    }
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

void bresenham(int x1,int y1,int x2,int y2){
    pushLine(x1, y1, x2, y2);
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

void firstOctaveReduction(int x1, int y1, int x2, int y2, bool callByTransformation=false){
    printf("Reducao %d %d\n", x1, y1);
    printf("Reducao %d %d\n", x2, y2);
    if(!callByTransformation){
        pushLine(x1, y1, x2, y2);
    }
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
    pushLine(x1, y1, x2, y2);
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

