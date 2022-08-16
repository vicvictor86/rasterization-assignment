#include <cmath>

struct ponto{
    int x;
    int y;
    ponto * prox;
};

void translation(ponto * arrayToTranslate, int size, int Tx, int Ty){
    for(int i = 0; i < size; i++){
        arrayToTranslate[i].x += Tx;
        arrayToTranslate[i].y += Ty;
    }
}

void scale(ponto * arrayToScale, int size, int Sx, int Sy){
    for(int i = 0; i < size; i++){
        arrayToScale[i].x *= Sx;
        arrayToScale[i].y *= Sy;
    }
    translation(arrayToScale, size, (Sx/2), (Sy/2));
}

void rotation(ponto * arrayToRotation, int size, int angle){
    translation(arrayToRotation, size, -(arrayToRotation[0].x), -(arrayToRotation[0].y));
    for(int i = 0; i < size; i++){
        arrayToRotation[i].x = arrayToRotation[i].x * cos(angle) - arrayToRotation[i].y * sin(angle);
        arrayToRotation[i].y = arrayToRotation[i].x * sin(angle) + arrayToRotation[i].y * cos(angle);
    }
    translation(arrayToRotation, size, (arrayToRotation[0].x), (arrayToRotation[0].y));
}

void shearX(ponto * arrayToShear, int size, int S){
    for(int i = 0; i < size; i++){
        arrayToShear[i].x += S * arrayToShear[i].y;
    }
}

void shearY(ponto * arrayToShear, int size, int S){
    for(int i = 0; i < size; i++){
        arrayToShear[i].y += S * arrayToShear[i].x;
    }
}

void shear(ponto * arrayToShear, int size, int Sx, int Sy){
    for(int i = 0; i < size; i++){
        arrayToShear[i].x += Sx * arrayToShear[i].y;
        arrayToShear[i].y += Sy * arrayToShear[i].x;
    }
}

void reflectionOverX(ponto * arrayToReflect, int size){
    for(int i = 0; i < size; i++){
        arrayToReflect[i].y *= -1;
    }
}

void reflectionOverY(ponto * arrayToReflect, int size){
    for(int i = 0; i < size; i++){
        arrayToReflect[i].x *= -1;
    }
}

void reflectionOverOrigin(ponto * arrayToReflect, int size){
    for(int i = 0; i < size; i++){
        arrayToReflect[i].x *= -1;
        arrayToReflect[i].y *= -1;
    }
}