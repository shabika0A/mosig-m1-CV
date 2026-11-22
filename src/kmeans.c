#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Utils/types.h"
#include "Utils/Util.h"
#include "TP5/TP5_funcs.h"

int main(int argc, char *argv[]) {
    if (argc != 8) {
        printf("\nUsage: %s input.ppm output.ppm K init stop location seed\n"
               "output.ppm==-1 for autonaming\n"
               "K is the number of clusters\n"
               "init is the initialization strategy\n"
               "stop is the end condition strategy\n"
               "location==0: do not use location\n"
               "seed==0: for random initialization\n",
            argv[0]);
        return 0;
    }

    /* Opening input file */
    ppm_file image = read_ppm(argv[1]);

    // Parsing arguments
    int K = atoi(argv[3]);
    int init = atoi(argv[4]);
    int stop = atoi(argv[5]);
    float location = atoi(argv[6]);
    int seed_value = atoi(argv[7]);

    // Initializing random seed
    srand(seed_value);

    // Create output filename if necessary
    char output_filename[512];
    if (strcmp(argv[2], "-1") == 0) {
        sprintf(output_filename, "results/%c_%d_%d_%d_%f_%d.ppm",
                argv[1][7], K, init, stop, location, seed_value);
    } else {
        snprintf(output_filename, sizeof(output_filename), "%s", argv[2]);
    }
    printf("PPM Result will be stored in %s\n", output_filename);

    pixel *centers = NULL;
    int *label_image = NULL;
    ppm_file kmeans_image = kmeans(image, K, init, stop, location, &centers, &label_image);

    write_ppm(kmeans_image, output_filename);

    // Save cluster centroids to CSV
    char cluster_csv_filename[512];
    sprintf(cluster_csv_filename, "results/%c_%d_%d_%d_%d_%d_clusters.csv",
            argv[1][7], K, init, stop, location, seed_value);

    save_clusters_in_csv(centers, label_image, K, cluster_csv_filename);
    printf("Cluster structure saved to %s\n", cluster_csv_filename);

    // Create mask image
    pgm_file mask;
    create_mask_image(&mask, label_image, image.rows, image.cols);
    printf("Mask image created with %d rows and %d cols\n", mask.rows, mask.cols);

    printf("Do you want to save the mask image as a PAM file? (y/n): ");
    char save_mask;
    if (scanf(" %c", &save_mask) != 1 || (save_mask != 'y' && save_mask != 'Y')) {
        printf("Mask image not saved.\n");
    } else {
        printf("Which cluster do you want to save as a PAM file? (0-%d): ", K - 1);
        int cluster_id;
        if (scanf("%d", &cluster_id) == 1) {
            printf("Saving mask image as PAM file...\n");

            pam_file output_pam;
            ppm_to_pam(image, mask, &output_pam, cluster_id);

            char output_pam_filename[512];
            sprintf(output_pam_filename, "results/%c_%d_%d_%d_%d_%d_mask_%d.pam",
                    argv[1][7], K, init, stop, location, seed_value, cluster_id);
            write_pam(output_pam, output_pam_filename);
            printf("PAM image saved to %s\n", output_pam_filename);

        }
    }


    return 0;
}