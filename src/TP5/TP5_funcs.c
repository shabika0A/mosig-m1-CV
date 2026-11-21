#include "TP5_funcs.h"
#include "../Utils/Util.h"
#include <stdlib.h>
#include <limits.h>

ppm_file kmeans(ppm_file image, int K, int init, int stop, int location) {
    int num_pixels = image.rows * image.cols;
    pixel *centers = malloc(K * sizeof(pixel));
    int *label_image = malloc(num_pixels * sizeof(int));


    for (int i = 0; i < K; i++) {
        if (init == 0) {
            centers[i].red = rand() % 256;
            centers[i].green = rand() % 256;
            centers[i].blue = rand() % 256;
        }
    }

    // 2. Run K-means iterations
    int max_iterations = (stop > 0) ? stop : 10;
    int round_counter = 0;

    while (round_counter < max_iterations) {
        pixel *prev_centers = malloc(K * sizeof(pixel));
        for (int j = 0; j < K; j++) {
            prev_centers[j] = centers[j];
        }

        update_labels(image, label_image, K, centers);
        update_centers(image, label_image, K, centers);
        round_counter++;

        // Stop early if no centroid changed
        int moved = 0;
        for (int j = 0; j < K; j++) {
            if (centers[j].red != prev_centers[j].red ||
                centers[j].green != prev_centers[j].green ||
                centers[j].blue != prev_centers[j].blue) {
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
        output_image.pixmap[i] = centers[cluster];
    }

    free(label_image);
    free(centers);

    return output_image;
}

void update_labels (ppm_file image,int* label_image, int K, pixel* centers){
    //for each pixel in the image, we have to assign it to the closest center
   // image + centers -> label image.
    for (int i = 0; i < image.rows * image.cols; i++){
        pixel current_pixel = image.pixmap[i];
        int closest_center_index = 0;
        int closest_distance = 256*256*3; //square of max distance possible in RGB space
        for (int j = 0; j < K; j++){
            pixel current_center = centers[j];
            int distance = ((int)current_pixel.red - (int)current_center.red) * ((int)current_pixel.red - (int)current_center.red) +
                            ((int)current_pixel.green - (int)current_center.green) * ((int)current_pixel.green - (int)current_center.green) +
                            ((int)current_pixel.blue - (int)current_center.blue) * ((int)current_pixel.blue - (int)current_center.blue);
            if (distance < closest_distance){
                closest_distance = distance;
                closest_center_index = j;
            }
        }
        //assign the pixel to the closest center cluster
        label_image[i] = closest_center_index;
    }
}
void update_centers(ppm_file image,int* label_image, int K, pixel *centers){
    //calculate the new centers with mean of the clusters
    for (int j = 0; j < K; j++){
        int sum_red = 0;
        int sum_green = 0;
        int sum_blue = 0;
        int cluster_size = 0;

        //loop through the cluster and calculate the mean
        for (int i =0;i<image.rows * image.cols;i++){
            if(label_image[i]==j){
                sum_red += image.pixmap[i].red;
                sum_green += image.pixmap[i].green;
                sum_blue += image.pixmap[i].blue;
                cluster_size++;
            }
        }
        
        if (cluster_size > 0){
            centers[j].red = sum_red / cluster_size;
            centers[j].green = sum_green / cluster_size;
            centers[j].blue = sum_blue / cluster_size;
        }
    }
}


