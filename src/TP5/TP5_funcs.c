#include "TP5_funcs.h"
#include "../Utils/Util.h"
#include <stdlib.h>
#include <limits.h>
#include <float.h>

ppm_file kmeans(ppm_file image, int K, int init, int stop, float location) {
    int num_pixels = image.rows * image.cols;
    centroid *centers = malloc(K * sizeof(centroid));
    int *label_image = malloc(num_pixels * sizeof(int));


    for (int i = 0; i < K; i++) {
        if (init == 0) {
            centers[i].pix.red = rand() % 256;
            centers[i].pix.green = rand() % 256;
            centers[i].pix.blue = rand() % 256;
            // including the location
            centers[i].i = rand() % image.rows;
            centers[i].j = rand() % image.cols;
        }
    }

    // 2. Run K-means iterations
    int max_iterations = (stop > 0) ? stop : 10;
    int round_counter = 0;

    while (round_counter < max_iterations) {
        centroid *prev_centers = malloc(K * sizeof(centroid));
        for (int j = 0; j < K; j++) {
            prev_centers[j] = centers[j];
        }

        update_labels(image, label_image, K, centers, location);
        update_centers(image, label_image, K, centers);
        round_counter++;

        // Stop early if no centroid changed
        int moved = 0;
        for (int j = 0; j < K; j++) {
            if (centers[j].pix.red != prev_centers[j].pix.red ||
                centers[j].pix.green != prev_centers[j].pix.green ||
                centers[j].pix.blue != prev_centers[j].pix.blue ||
                centers[j].i != prev_centers[j].i ||
                centers[j].j != prev_centers[j].j) {
                moved = 1;
                break;
            }
        }
        free(prev_centers);

        if (!moved) {
            break;
        }
    }

    // 3. Build output image 
    ppm_file output_image = image;
    output_image.pixmap = malloc(num_pixels * sizeof(pixel));
    for (int i = 0; i < num_pixels; i++) {
        int cluster = label_image[i];
        output_image.pixmap[i] = centers[cluster].pix;
    }

    free(label_image);
    free(centers);

    return output_image;
}

void update_labels(ppm_file image, int* label_image, int K, centroid* centers, float location) {
    //for each pixel in the image, we have to assign it to the closest center
    // image + centers -> label image.
    for (int i = 0; i < image.rows * image.cols; i++) {
        pixel current_pixel = image.pixmap[i];

        float pix_i = ((float)(i / image.cols) / image.rows) * 255.0f;
        float pix_j = ((float)(i % image.cols) / image.cols) * 255.0f;

        int closest_center_index = 0;
        float closest_distance = INT_MAX;
        
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
        //assign the pixel to the closest center cluster
        label_image[i] = closest_center_index;
    }
}

void update_centers(ppm_file image, int* label_image, int K, centroid *centers) {
    //calculate the new centers with mean of the clusters
    for (int j = 0; j < K; j++) {
        int sum_red = 0;
        int sum_green = 0;
        int sum_blue = 0;
        int sum_i = 0;
        int sum_j = 0;
        int cluster_size = 0;

        //loop through the cluster and calculate the mean
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