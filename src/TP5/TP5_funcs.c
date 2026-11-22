#include "TP5_funcs.h"
#include "../Utils/Util.h"
#include <stdlib.h>
#include <limits.h>

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

// Save the cluster structure into a CSV file
void save_clusters_in_csv(pixel *centers, int *clusters, int K, const char *filename) {
    // if file name is NULL, save in the results folder.
    if (filename == NULL) {
        filename = "results/clusters.csv";
    }
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        pm_error("Error opening file for writing cluster structure");
    }

    fprintf(fp, "Cluster,Red,Green,Blue\n");
    for (int i = 0; i < K; i++) {
        fprintf(fp, "%d,%d,%d,%d\n", i, centers[i].red, centers[i].green, centers[i].blue);
    }
    fclose(fp);
    // fprintf("Cluster structure saved to %s\n", filename);
}
void create_mask_image(pgm_file *mask, int* label_image, int rows, int cols) {
    mask->rows = rows;
    mask->cols = cols;
    mask->graymap = (gray *) malloc(rows * cols * sizeof(gray));
    for (int i = 0; i < rows * cols; i++) {
        mask->graymap[i] = (gray) label_image[i];
    }
}


void ppm_to_pam(ppm_file image, pgm_file mask, pam_file *image_pam, int cluster_id) {
    if(image.magic_number != '3' && image.magic_number != '6') {
        pm_error("Wrong file type");
    }
    image_pam->magic_number = '7';

    image_pam->rows = image.rows;
    image_pam->cols = image.cols;
    image_pam->maxval = image.maxval;
    image_pam->pixmap = (pixel_PAM *) malloc(image.cols * image.rows * sizeof(pixel_PAM));

    // float alpha = 255;

    for(int i = 0; i < image.rows; i++) {
        for(int j = 0; j < image.cols; j++) {
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


void write_pam ( pam_file image, char *filename) {

    FILE *ofp;
    int i, j;

    /* Opening output file */
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
    
    /* Closing output file */
    fclose(ofp);
    // fprintf("PAM image saved to %s\n", filename);
}



ppm_file kmeans(ppm_file image, int K, int init, int stop, int location, pixel **centers, int **label_image) {
    int num_pixels = image.rows * image.cols;
    *centers = malloc(K * sizeof(pixel));
    *label_image = malloc(num_pixels * sizeof(int));

    for (int i = 0; i < K; i++) {
        if (init == 0) {
            (*centers)[i].red = rand() % 256;
            (*centers)[i].green = rand() % 256;
            (*centers)[i].blue = rand() % 256;
        }
    }

    // 2. Run K-means iterations
    int max_iterations = (stop > 0) ? stop : 10;
    int round_counter = 0;

    while (round_counter < max_iterations) {
        pixel *prev_centers = malloc(K * sizeof(pixel));
        for (int j = 0; j < K; j++) {
            prev_centers[j] = (*centers)[j];
        }

        update_labels(image, *label_image, K, *centers);
        update_centers(image, *label_image, K, *centers);
        round_counter++;

        // Stop early if no centroid changed
        int moved = 0;
        for (int j = 0; j < K; j++) {
            if ((*centers)[j].red != prev_centers[j].red ||
                (*centers)[j].green != prev_centers[j].green ||
                (*centers)[j].blue != prev_centers[j].blue) {
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
        int cluster = (*label_image)[i];
        output_image.pixmap[i] = (*centers)[cluster];
    }

    return output_image;
}