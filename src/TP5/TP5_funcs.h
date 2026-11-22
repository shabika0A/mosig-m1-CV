#ifndef TP5_FUNCS_H
#define TP5_FUNCS_H

#include "../Utils/types.h"

// TODO: implement the kmeans algorithm
ppm_file kmeans(ppm_file image, int K, int init, int stop, int location, pixel **centers, int **label_image);
void update_labels (ppm_file image,int* label_image, int K, pixel* centers);
void update_centers(ppm_file image,int* label_image, int K, pixel *centers);
void save_clusters_in_csv(pixel *centers, int *clusters, int K, const char *filename);
void create_mask_image(pgm_file *mask, int* label_image, int rows, int cols);
void ppm_to_pam(ppm_file image, pgm_file mask, pam_file *output_pam, int alpha_value);  
void write_pam(pam_file image, char *filename);
#endif