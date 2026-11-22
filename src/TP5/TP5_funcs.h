#ifndef TP5_FUNCS_H
#define TP5_FUNCS_H

#include "../Utils/types.h"

// TODO: implement the kmeans algorithm
void update_labels(ppm_file image, int *label_image, int K, centroid *centers, float location);
void update_centers(ppm_file image, int *label_image, int K, centroid *centers);
void save_clusters_in_csv(centroid *centers, int *clusters, int K, const char *filename);
void create_mask_image(pgm_file *mask, int *label_image, int rows, int cols);
void ppm_to_pam(ppm_file image, pgm_file mask, pam_file *image_pam, int cluster_id);
void write_pam(pam_file image, char *filename);
ppm_file kmeans(ppm_file image, int K, int init, int stop, float location, centroid **centers, int **label_image);
#endif