#include "TP5_funcs.h"
#include "../Utils/Util.h"
#include <stdlib.h>

ppm_file kmeans(ppm_file image, int K, int init, int stop, int location) {
    // printf("PLEASE IMPLEMENT kmeans\n");
    //array of K center
    pixel *centers = malloc(K * sizeof(pixel));
    //assign the location randomly to the centroids to start
    // if (init == 0) {

    for (int i = 0; i < K; i++) {
        if(init==0){// so we have to initialize the centers randomly
            centers[i].red = rand() % 256;
            centers[i].green = rand() % 256;
            centers[i].blue = rand() % 256;
        }
    }

    // label_image label_image = (*label_image) malloc(sizeof(label_image));
    int* label_image  = malloc (image.rows*image.cols*sizeof(int));
    int round_counter = 0;
    
    // or centers not move anymore
    while (round_counter < stop ){
        update_labels(image,label_image, K, centers);
        update_centers(image,label_image, K, centers);  
        round_counter++;
        /// check stop condition
        // do it with a have moved bool or if we got the max number of rounds
        ///////// where does the seed come from and how is it used in the main func
        //  ////meaning of stop condition?
    }

    ppm_file output_image;

    output_image = image;

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
            int distance = (current_pixel.red - current_center.red) * (current_pixel.red - current_center.red) +
                            (current_pixel.green - current_center.green) * (current_pixel.green - current_center.green) +
                            (current_pixel.blue - current_center.blue) * (current_pixel.blue - current_center.blue);
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


