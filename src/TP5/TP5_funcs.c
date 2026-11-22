#include "TP5_funcs.h"
#include "../Utils/Util.h"
#include <stdlib.h>
#include <limits.h>

void update_labels(ppm_file image, int *label_image, int K, centroid *centers, float location) {
    // For each pixel in the image, assign it to the closest center
    for (int i = 0; i < image.rows * image.cols; i++) {
        pixel current_pixel = image.pixmap[i];

        float pix_i = ((float)(i / image.cols) / image.rows) * 255.0f;
        float pix_j = ((float)(i % image.cols) / image.cols) * 255.0f;

        int closest_center_index = 0;
        float closest_distance = (float)INT_MAX;

        for (int j = 0; j < K; j++) {
            centroid current_center = centers[j];
            float di = location * (pix_i - ((float)current_center.i / image.rows) * 255.0f);
            float dj = location * (pix_j - ((float)current_center.j / image.cols) * 255.0f);
            float distance = ((int)current_pixel.red - (int)current_center.pix.red) * ((int)current_pixel.red - (int)current_center.pix.red) +
                             ((int)current_pixel.green - (int)current_center.pix.green) * ((int)current_pixel.green - (int)current_center.pix.green) +
                             ((int)current_pixel.blue - (int)current_center.pix.blue) * ((int)current_pixel.blue - (int)current_center.pix.blue) +
                             di * di + dj * dj;
            if (distance < closest_distance) {
                closest_distance = distance;
                closest_center_index = j;
            }
        }
        label_image[i] = closest_center_index;
    }
}

void update_centers(ppm_file image, int *label_image, int K, centroid *centers) {
    // Calculate the new centers using the mean of the clusters
    for (int j = 0; j < K; j++) {
        int sum_red = 0;
        int sum_green = 0;
        int sum_blue = 0;
        int sum_i = 0;
        int sum_j = 0;
        int cluster_size = 0;

        for (int i = 0; i < image.rows * image.cols; i++) {
            if (label_image[i] == j) {
                sum_red += image.pixmap[i].red;
                sum_green += image.pixmap[i].green;
                sum_blue += image.pixmap[i].blue;
                sum_i += i / image.cols;
                sum_j += i % image.cols;
                cluster_size++;
            }
        }

        if (cluster_size > 0) {
            centers[j].pix.red = sum_red / cluster_size;
            centers[j].pix.green = sum_green / cluster_size;
            centers[j].pix.blue = sum_blue / cluster_size;
            centers[j].i = sum_i / cluster_size;
            centers[j].j = sum_j / cluster_size;
        }
    }
}

// Save the cluster structure into a CSV file
void save_clusters_in_csv(centroid *centers, int *clusters, int K, const char *filename) {
    if (filename == NULL) {
        filename = "results/clusters.csv";
    }
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        pm_error("Error opening file for writing cluster structure");
    }

    fprintf(fp, "Cluster,Red,Green,Blue\n");
    for (int i = 0; i < K; i++) {
        fprintf(fp, "%d,%d,%d,%d\n", i, centers[i].pix.red, centers[i].pix.green, centers[i].pix.blue);
    }
    fclose(fp);
}

void create_mask_image(pgm_file *mask, int *label_image, int rows, int cols) {
    mask->rows = rows;
    mask->cols = cols;
    mask->graymap = (gray *)malloc(rows * cols * sizeof(gray));
    for (int i = 0; i < rows * cols; i++) {
        mask->graymap[i] = (gray)label_image[i];
    }
}

void ppm_to_pam(ppm_file image, pgm_file mask, pam_file *image_pam, int cluster_id) {
    if (image.magic_number != '3' && image.magic_number != '6') {
        pm_error("Wrong file type");
    }
    image_pam->magic_number = '7';

    image_pam->rows = image.rows;
    image_pam->cols = image.cols;
    image_pam->maxval = image.maxval;
    image_pam->pixmap = (pixel_PAM *)malloc(image.cols * image.rows * sizeof(pixel_PAM));

    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            image_pam->pixmap[i * image.cols + j].red = image.pixmap[i * image.cols + j].red;
            image_pam->pixmap[i * image.cols + j].green = image.pixmap[i * image.cols + j].green;
            image_pam->pixmap[i * image.cols + j].blue = image.pixmap[i * image.cols + j].blue;

            if (mask.graymap[i * image.cols + j] == cluster_id) {
                image_pam->pixmap[i * image.cols + j].alpha = (bit)(255);
            } else {
                image_pam->pixmap[i * image.cols + j].alpha = (bit)(0);
            }
        }
    }
}

void write_pam(pam_file image, char *filename) {
    FILE *ofp;
    int i, j;

    ofp = fopen(filename, "w");
    if (ofp == NULL) {
        printf("error in opening file %s\n", filename);
        pm_error(" FILE OPEN ERROR");
    }
    fprintf(ofp, "P7\n");
    fprintf(ofp, "WIDTH %d\n", image.cols);
    fprintf(ofp, "HEIGHT %d\n", image.rows);
    fprintf(ofp, "DEPTH 4\n");
    fprintf(ofp, "MAXVAL %d\n", image.maxval);
    fprintf(ofp, "TUPLTYPE RGB_ALPHA\n");
    fprintf(ofp, "ENDHDR\n");

    for (i = 0; i < image.rows; i++) {
        for (j = 0; j < image.cols; j++) {
            fprintf(ofp, "%c", image.pixmap[i * image.cols + j].red);
            fprintf(ofp, "%c", image.pixmap[i * image.cols + j].green);
            fprintf(ofp, "%c", image.pixmap[i * image.cols + j].blue);
            fprintf(ofp, "%c", image.pixmap[i * image.cols + j].alpha);
        }
    }

    fclose(ofp);
}

ppm_file kmeans(ppm_file image, int K, int init, int stop, float location, centroid **centers, int **label_image) {
    int num_pixels = image.rows * image.cols;
    *centers = malloc(K * sizeof(centroid));
    *label_image = malloc(num_pixels * sizeof(int));

    for (int i = 0; i < K; i++) {
        if (init == 0) {
            (*centers)[i].pix.red = rand() % 256;
            (*centers)[i].pix.green = rand() % 256;
            (*centers)[i].pix.blue = rand() % 256;
            (*centers)[i].i = rand() % image.rows;
            (*centers)[i].j = rand() % image.cols;
        }
    }

    int max_iterations = (stop > 0) ? stop : 10;
    int round_counter = 0;

    while (round_counter < max_iterations) {
        centroid *prev_centers = malloc(K * sizeof(centroid));
        for (int j = 0; j < K; j++) {
            prev_centers[j] = (*centers)[j];
        }

        update_labels(image, *label_image, K, *centers, location);
        update_centers(image, *label_image, K, *centers);
        round_counter++;

        // Stop early if no centroid changed
        int moved = 0;
        for (int j = 0; j < K; j++) {
            if ((*centers)[j].pix.red != prev_centers[j].pix.red ||
                (*centers)[j].pix.green != prev_centers[j].pix.green ||
                (*centers)[j].pix.blue != prev_centers[j].pix.blue ||
                (*centers)[j].i != prev_centers[j].i ||
                (*centers)[j].j != prev_centers[j].j) {
                moved = 1;
                break;
            }
        }
        free(prev_centers);

        if (!moved) {
            break;
        }
    }

    ppm_file output_image = image;
    output_image.pixmap = malloc(num_pixels * sizeof(pixel));
    for (int i = 0; i < num_pixels; i++) {
        int cluster = (*label_image)[i];
        output_image.pixmap[i] = (*centers)[cluster].pix;
    }

    return output_image;
}