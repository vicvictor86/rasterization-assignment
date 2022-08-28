#include "rasterizationAlgorithms.h"

bool lastDrawnCircle = false;

void translation(ponto * point, int Tx, int Ty){
    line * linesToTranslate = everyLines;
    while(linesToTranslate != NULL){
        linesToTranslate->x1 = linesToTranslate->x1 + Tx;
        linesToTranslate->y1 = linesToTranslate->y1 + Ty;
        linesToTranslate->x2 = linesToTranslate->x2 + Tx;
        linesToTranslate->y2 = linesToTranslate->y2 + Ty;

        linesToTranslate = linesToTranslate->prox;
    }
    while(point != NULL){
        pixelsMap[point->x][point->y].value = 1;
        point->x = point->x + Tx;
        point->y = point->y + Ty;
        pixelsMap[point->x][point->y].value = 0;
        point = point->prox;
    }
}

void translationLine(line * point, int Tx, int Ty, int firstPointPolygonToModify, int lastPointPolygonToModify){
    int pointToModify = 0;
    while(point != NULL){
        if(pointToModify >= firstPointPolygonToModify && pointToModify < lastPointPolygonToModify){
            point->x1 = point->x1 + Tx;
            point->y1 = point->y1 + Ty;
            point->x2 = point->x2 + Tx;
            point->y2 = point->y2 + Ty;
        }
        point = point->prox;
        pointToModify++;
    }
}

ponto getCentroid(vector<line> lineVectors){
    if(lineVectors.size() == 1){
        ponto centroid;
        centroid.x = int((lineVectors.at(0).x1 + lineVectors.at(0).x2)/2);
        centroid.y = int((lineVectors.at(0).y1 + lineVectors.at(0).y2)/2);
        return centroid;
    }

    double area = 0;
    int size = lineVectors.size();
    for(int i = 0; i < size; i++){
        line nextVertex;
        i + 1 >= size ? nextVertex = lineVectors.at(0) : nextVertex = lineVectors.at(i + 1);
        area += lineVectors.at(i).x1 * nextVertex.y1 - nextVertex.x1 * lineVectors.at(i).y1;
    }
    area = area / 2;

    double x = 0;
    double y = 0;
    for(int i = 0; i < size; i++){
        line nextVertex;
        i + 1 >= size ? nextVertex = lineVectors.at(0) : nextVertex = lineVectors.at(i + 1);
        x += (lineVectors.at(i).x1 + nextVertex.x1) * (lineVectors.at(i).x1 * nextVertex.y1 - nextVertex.x1 * lineVectors.at(i).y1);
    }
    x = round(x / (6 * area));
    // printf("Centroide x %.2f\n", x);
    for(int i = 0; i < size; i++){
        line nextVertex;
        i + 1 >= size ? nextVertex = lineVectors.at(0) : nextVertex = lineVectors.at(i + 1);
        y += (lineVectors.at(i).y1 + nextVertex.y1) * (lineVectors.at(i).x1 * nextVertex.y1 - nextVertex.x1 * lineVectors.at(i).y1);
    }
    y = round(y / (6 * area));
    // printf("Centroide y %.2f\n", y);

    ponto centroid;
    centroid.x = (int)x;
    centroid.y = (int)y;

    return centroid;
}

void scaleCircle(int radius, int centerX, int centerY){
    ponto * tempArrayPoints = pontos;

    while(tempArrayPoints != NULL && tempArrayPoints->firstPointPolygon != true){
        tempArrayPoints = tempArrayPoints->prox;
        popPonto();
    }

    circumferenceRasterization(radius, centerX, centerY);
}

//Faz a transformação de escala apenas no último poligono desenhado
void scale(line * arrayToScale, int Sx, int Sy, int sizePolygon){
    line * startLinkedList = arrayToScale;
    line * tempLinkedList = startLinkedList;

    vector<line> subPolygonArray;
    int firstPointPolygonToModify = 0;

    int lastPointPolygonToModify = sizePolygon;

    while(arrayToScale != NULL){
        line actualLine = {arrayToScale->x1, arrayToScale->y1, arrayToScale->x2, arrayToScale->y2, arrayToScale->prox, arrayToScale->endPolygon};
        subPolygonArray.push_back(actualLine);

        tempLinkedList = startLinkedList;
        if(subPolygonArray.back().endPolygon){
            ponto centroid = getCentroid(subPolygonArray);

            translationLine(startLinkedList, -centroid.x, -centroid.y, firstPointPolygonToModify, lastPointPolygonToModify);
            int pointToModify = 0;
            while(tempLinkedList != NULL){
                if(pointToModify >= firstPointPolygonToModify && pointToModify < lastPointPolygonToModify){
                    pixelsMap[tempLinkedList->x1][tempLinkedList->y1].value = 1;
                    pixelsMap[tempLinkedList->x2][tempLinkedList->y2].value = 1;
                    tempLinkedList->x1 *= Sx;
                    tempLinkedList->y1 *= Sy;
                    tempLinkedList->x2 *= Sx;
                    tempLinkedList->y2 *= Sy;
                    pointToModify++;
                }
                tempLinkedList = tempLinkedList->prox;
            }
            translationLine(startLinkedList, centroid.x, centroid.y, firstPointPolygonToModify, lastPointPolygonToModify);
            
            tempLinkedList = startLinkedList;
            pointToModify = 0;

            removeAllPoints();
            while(tempLinkedList != NULL){
                bresenham(tempLinkedList->x1, tempLinkedList->y1, tempLinkedList->x2, tempLinkedList->y2, true);
                tempLinkedList = tempLinkedList->prox;
            }
            return;
        }

        arrayToScale = arrayToScale->prox;
    }
}

void rotation(line * arrayToRotate, int angle, int sizePolygon){
    vector<line> subPolygonArray;
    int firstPointPolygonToModify = 0;
    line * startLinkedList = arrayToRotate;

    int lastPointPolygonToModify = sizePolygon;

    while(arrayToRotate != NULL && !lastDrawnCircle){  
        line actualLine = {arrayToRotate->x1, arrayToRotate->y1, arrayToRotate->x2, arrayToRotate->y2, arrayToRotate->prox, arrayToRotate->endPolygon};
        subPolygonArray.push_back(actualLine);

        line * tempLinkedList = startLinkedList;

        if(subPolygonArray.back().endPolygon){
            ponto centroid = getCentroid(subPolygonArray);
            translationLine(startLinkedList, -centroid.x, -centroid.y, firstPointPolygonToModify, lastPointPolygonToModify);
            int pointToModify = 0;

            while(tempLinkedList != NULL){
                if(pointToModify >= firstPointPolygonToModify && pointToModify < lastPointPolygonToModify){
                    int x1 = tempLinkedList->x1;
                    int y1 = tempLinkedList->y1;
                    int x2 = tempLinkedList->x2;
                    int y2 = tempLinkedList->y2;

                    double degreesToRadian = angle * 3.14 / 180;

                    double rotationX1 = x1 * cos(degreesToRadian) - y1 * sin(degreesToRadian);
                    double rotationY1 = x1 * sin(degreesToRadian) + y1 * cos(degreesToRadian);
                    double rotationX2 = x2 * cos(degreesToRadian) - y2 * sin(degreesToRadian);
                    double rotationY2 = x2 * sin(degreesToRadian) + y2 * cos(degreesToRadian);

                    pixelsMap[tempLinkedList->x1][tempLinkedList->y1].value = 1;
                    pixelsMap[tempLinkedList->x2][tempLinkedList->y2].value = 1;
                    tempLinkedList->x1 = round(rotationX1);
                    tempLinkedList->y1 = round(rotationY1);
                    tempLinkedList->x2 = round(rotationX2);
                    tempLinkedList->y2 = round(rotationY2);

                    pointToModify++;
                }
                tempLinkedList = tempLinkedList->prox;
            }
            translationLine(startLinkedList, centroid.x, centroid.y, firstPointPolygonToModify, lastPointPolygonToModify);

            tempLinkedList = startLinkedList;
            pointToModify = 0;

            removeAllPoints();
            while(tempLinkedList != NULL){
                bresenham(tempLinkedList->x1, tempLinkedList->y1, tempLinkedList->x2, tempLinkedList->y2, true);
                tempLinkedList = tempLinkedList->prox;
            }
            return;
        }

        arrayToRotate = arrayToRotate->prox;
    }
}

void shearX(line * arrayToShear, int S){
    while(arrayToShear != NULL){
        pixelsMap[arrayToShear->x1][arrayToShear->y1].value = 1;
        pixelsMap[arrayToShear->x2][arrayToShear->y2].value = 1;
        arrayToShear->x1 += S * arrayToShear->y1;
        arrayToShear->x2 += S * arrayToShear->y2;
        bresenham(arrayToShear->x1, arrayToShear->y1, arrayToShear->x2, arrayToShear->y2, true);
        arrayToShear = arrayToShear->prox;
    }
}

void shearY(line * arrayToShear, int S){
    while(arrayToShear != NULL){
        pixelsMap[arrayToShear->x1][arrayToShear->y1].value = 1;
        pixelsMap[arrayToShear->x2][arrayToShear->y2].value = 1;
        arrayToShear->y1 += S * arrayToShear->x1;
        arrayToShear->y2 += S * arrayToShear->x2;
        bresenham( arrayToShear->x2, arrayToShear->y2, arrayToShear->x1, arrayToShear->y1, true);
        arrayToShear = arrayToShear->prox;
    }
}

void shear(line * arrayToShear, int Sx, int Sy){
    while(arrayToShear != NULL){
        pixelsMap[arrayToShear->x1][arrayToShear->y1].value = 1;
        pixelsMap[arrayToShear->x2][arrayToShear->y2].value = 1;
        arrayToShear->x1 += Sx * arrayToShear->y1;
        arrayToShear->x2 += Sx * arrayToShear->y2;
        arrayToShear->y1 += Sy * arrayToShear->x1;
        arrayToShear->y2 += Sy * arrayToShear->x2;
        bresenham(arrayToShear->x2, arrayToShear->y2, arrayToShear->x1, arrayToShear->y1, true);
        arrayToShear = arrayToShear->prox;
    }
}

void reflectionOverX(ponto * arrayToReflect){
    while(arrayToReflect != NULL){
        pixelsMap[arrayToReflect->x][arrayToReflect->y].value = 1;
        arrayToReflect->y *= -1;
        arrayToReflect = arrayToReflect->prox;
    }
}

void reflectionOverY(ponto * arrayToReflect){
    while(arrayToReflect != NULL){
        pixelsMap[arrayToReflect->x][arrayToReflect->y].value = 1;
        arrayToReflect->x *= -1;
        arrayToReflect = arrayToReflect->prox;
    }
}

void reflectionOverOrigin(ponto * arrayToReflect){
    while(arrayToReflect != NULL){
        pixelsMap[arrayToReflect->x][arrayToReflect->y].value = 1;
        arrayToReflect->x *= -1;
        arrayToReflect->y *= -1;
        arrayToReflect = arrayToReflect->prox;
    }
}