#ifndef TP5_FUNCS_H
#define TP5_FUNCS_H

#include "../Utils/types.h"

// TODO: implement the kmeans algorithm
ppm_file kmeans(ppm_file image, int K, int init, int stop, float location);
void update_labels (ppm_file image,int* label_image, int K, centroid* centers, float location);
void update_centers(ppm_file image,int* label_image, int K, centroid* centers);


#endif