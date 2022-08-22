#include <cmath>

struct ponto{
    int x;
    int y;
    ponto * prox;
};

void translation(ponto * point, int Tx, int Ty){
    while(point != NULL){
        point->x = point->x + Tx;
        point->y = point->y + Ty;
        point = point->prox;
    }
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

// void scale(ponto * arrayToScale, int size, int Sx, int Sy){
//     for(int i = 0; i < size; i++){
//         arrayToScale[i].x *= Sx;
//         arrayToScale[i].y *= Sy;
//     }
//     translation(arrayToScale, size, (Sx/2), (Sy/2));
// }

void rotation(ponto * point, int size, int angle){
    ponto centroid = getCentroid(point, size);
    translation(point, -centroid.x, -centroid.y);
    while(point != NULL){   
        int x = point->x;
        int y = point->y;

        double degreesToRadian = angle*3.14159/180;
        double rotationX = x * cos(degreesToRadian) - y * sin(degreesToRadian);
        double rotationY = x * sin(degreesToRadian) + y * cos(degreesToRadian);

        point->x = round(rotationX);
        point->y = round(rotationY);
        point = point->prox;

        printf("%.2f %.2f\n", rotationX, rotationY);
    }
    // for(int i = 0; i < size; i++){   
    //     int x = point[i].x;
    //     int y = point[i].y;
    //     double degreesToRadian = angle*3.14159/180;

    //     double rotationX = x * cos(degreesToRadian) - y * sin(degreesToRadian);
    //     double rotationY = x * sin(degreesToRadian) + y * cos(degreesToRadian);

    //     point[i].x = (int)round(rotationX);
    //     point[i].y = (int)round(rotationY);
    //     printf("%.2f %.2f\n", rotationX, rotationY);
    // }
    translation(point, centroid.x, centroid.y);
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