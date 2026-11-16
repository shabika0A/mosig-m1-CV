#include "TP5_funcs.h"
#include "../Utils/Util.h"

ppm_file kmeans(ppm_file image, int K, int init, int stop, int location) {
    // printf("PLEASE IMPLEMENT kmeans\n");
    //array of K center
    pixel *centers = malloc(K * sizeof(pixel));
    //assign the location randomly to the centroids to start
    if (init == 0) {

    for (int i = 0; i < K; i++) {
        if(init==0){// so we have to initialize the centers randomly
            centers[i].red = rand() % 256;
            centers[i].green = rand() % 256;
            centers[i].blue = rand() % 256;
        }
    }
    int round_counter = 0;
    pixel** clusters = malloc(K * sizeof(pixel*));
    for (int i = 0; i < K; i++) {
        clusters[i] = malloc(sizeof(pixel)*image.cols*image.rows); // allocate memory for each cluster
    }
    // or centers not move anymore
    while (round_counter < stop ){
        //for each pixel in the image, we have to assign it to the closest center
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
            // increase the size of the cluster array for the closest center
            /// add it somehow!
            clusters[closest_center_index][index?] = current_pixel;
        }
        //calculate the new centers with mean of the clusters
        for (int j = 0; j < K; j++){
            int sum_red = 0;
            int sum_green = 0;
            int sum_blue = 0;
            int cluster_size = 0;
            //loop through the cluster and calculate the mean
            for (int i = 0; i < {{{cluster[j].size}}}; i++){
                sum_red += clusters[j][i].red;
                sum_green += clusters[j][i].green;
                sum_blue += clusters[j][i].blue;
                cluster_size++;
            }
            if (cluster_size > 0){
                centers[j].red = sum_red / cluster_size;
                centers[j].green = sum_green / cluster_size;
                centers[j].blue = sum_blue / cluster_size;
            }
        }
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


