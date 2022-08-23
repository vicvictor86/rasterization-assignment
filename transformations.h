#include <cmath>
#include "rasterizationAlgorithms.h"

void translation(ponto * point, int Tx, int Ty){
    while(point != NULL){
        point->x = point->x + Tx;
        point->y = point->y + Ty;
        point = point->prox;
    }
}

void translationLine(line * point, int Tx, int Ty){
    point->x1 = point->x1 + Tx;
    point->y1 = point->y1 + Ty;
    point->x2 = point->x2 + Tx;
    point->y2 = point->y2 + Ty;
    // point = point->prox;
}

ponto getCentroid(ponto * arrayToScale, int size){
    double area = 0;
    for(int i = 0; i < size; i++){
        ponto nextVertex;
        i + 1 >= size ? nextVertex = arrayToScale[0] : nextVertex = arrayToScale[i + 1];
        area += arrayToScale[i].x * nextVertex.y - nextVertex.x * arrayToScale[i].y;
    }
    area = area / 2;

    double x = 0;
    double y = 0;
    for(int i = 0; i < size; i++){
        ponto nextVertex;
        i + 1 >= size ? nextVertex = arrayToScale[0] : nextVertex = arrayToScale[i + 1];
        x += (arrayToScale[i].x + nextVertex.x) * (arrayToScale[i].x * nextVertex.y - nextVertex.x * arrayToScale[i].y);
    }
    x = round(x / (6 * area));
    // printf("Centroide x %.2f\n", x);
    for(int i = 0; i < size; i++){
        ponto nextVertex;
        i + 1 >= size ? nextVertex = arrayToScale[0] : nextVertex = arrayToScale[i + 1];
        y += (arrayToScale[i].y + nextVertex.y) * (arrayToScale[i].x * nextVertex.y - nextVertex.x * arrayToScale[i].y);
    }
    y = round(y / (6 * area));
    // printf("Centroide y %.2f\n", y);

    ponto centroid;
    centroid.x = (int)x;
    centroid.y = (int)y;

    return centroid;
}

void scale(line * arrayToScale, int Sx, int Sy){
    // ponto centroid = getCentroid(arrayToScale, size);
    while(arrayToScale != NULL){
        // translationLine(arrayToScale, -(arrayToScale->x1 + arrayToScale->x2)/2, -(arrayToScale->y1 + arrayToScale->y2)/2);
        arrayToScale->x1 *= Sx;
        arrayToScale->y1 *= Sy;
        arrayToScale->x2 *= Sx;
        arrayToScale->y2 *= Sy;
        // translationLine(arrayToScale, (arrayToScale->x1 + arrayToScale->x2)/2, (arrayToScale->y1 + arrayToScale->y2)/2);
        firstOctaveReduction(arrayToScale->x1, arrayToScale->y1, arrayToScale->x2, arrayToScale->y2, true);
        arrayToScale = arrayToScale->prox;
    }
}

void rotation(line * point, int angle){
    // ponto centroid = getCentroid(point, size);
    // translation(point, -centroid.x, -centroid.y);
    while(point != NULL){  
        int x1 = point->x1;
        int y1 = point->y1;
        int x2 = point->x2;
        int y2 = point->y2;

        double degreesToRadian = angle * 3.14 / 180;
        // translationLine(arrayToScale, -(arrayToScale->x1 + arrayToScale->x2)/2, -(arrayToScale->y1 + arrayToScale->y2)/2);

        double rotationX1 = x1 * cos(degreesToRadian) - y1 * sin(degreesToRadian);
        double rotationY1 = x1 * sin(degreesToRadian) + y1 * cos(degreesToRadian);
        double rotationX2 = x2 * cos(degreesToRadian) - y2 * sin(degreesToRadian);
        double rotationY2 = x2 * sin(degreesToRadian) + y2 * cos(degreesToRadian);

        point->x1 = round(rotationX1);
        point->y1 = round(rotationY1);
        point->x2 = round(rotationX2);
        point->y2 = round(rotationY2);
        // translationLine(arrayToScale, (arrayToScale->x1 + arrayToScale->x2)/2, (arrayToScale->y1 + arrayToScale->y2)/2);
        firstOctaveReduction(point->x1, point->y1, point->x2, point->y2, true); 

        point = point->prox;
    }
}

void shearX(line * arrayToShear, int S){
    while(arrayToShear != NULL){
        arrayToShear->x1 += S * arrayToShear->y1;
        arrayToShear->x2 += S * arrayToShear->y2;
        firstOctaveReduction(arrayToShear->x1, arrayToShear->y1, arrayToShear->x2, arrayToShear->y2, true);
        printf("%d %d\n", arrayToShear->x2, arrayToShear->y2);
        printf("%d %d\n", arrayToShear->x1, arrayToShear->y1);
        arrayToShear = arrayToShear->prox;
    }
}

void shearY(line * arrayToShear, int S){
    while(arrayToShear != NULL){
        arrayToShear->y1 += S * arrayToShear->x1;
        arrayToShear->y2 += S * arrayToShear->x2;
        firstOctaveReduction( arrayToShear->x2, arrayToShear->y2, arrayToShear->x1, arrayToShear->y1, true);
        printf("%d %d\n", arrayToShear->x2, arrayToShear->y2);
        printf("%d %d\n", arrayToShear->x1, arrayToShear->y1);
        arrayToShear = arrayToShear->prox;
    }
}

void shear(line * arrayToShear, int Sx, int Sy){
    while(arrayToShear != NULL){
        arrayToShear->x1 += Sx * arrayToShear->y1;
        arrayToShear->x2 += Sx * arrayToShear->y2;
        arrayToShear->y1 += Sy * arrayToShear->x1;
        arrayToShear->y2 += Sy * arrayToShear->x2;
        firstOctaveReduction(arrayToShear->x2, arrayToShear->y2, arrayToShear->x1, arrayToShear->y1, true);
        printf("%d %d\n", arrayToShear->x2, arrayToShear->y2);
        printf("%d %d\n", arrayToShear->x1, arrayToShear->y1);
        arrayToShear = arrayToShear->prox;
    }
}

void reflectionOverX(ponto * arrayToReflect){
    while(arrayToReflect != NULL){
        arrayToReflect->y *= -1;
        arrayToReflect = arrayToReflect->prox;
    }
}

void reflectionOverY(ponto * arrayToReflect){
    while(arrayToReflect != NULL){
        arrayToReflect->x *= -1;
        arrayToReflect = arrayToReflect->prox;
    }
}

void reflectionOverOrigin(ponto * arrayToReflect){
    while(arrayToReflect != NULL){
        arrayToReflect->x *= -1;
        arrayToReflect->y *= -1;
        arrayToReflect = arrayToReflect->prox;
    }
}