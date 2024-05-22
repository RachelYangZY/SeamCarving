#include "seamcarving.h"
# include "c_img.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void calc_energy(struct rgb_img *im, struct rgb_img **grad)
{
    create_img(grad, im->height, im->width);

    int x_right;
    int x_left;
    int y_down;
    int y_up;

    for(int x = 0; x < im->width; x++){
        for(int y = 0; y < im->height; y++){

            if(x == 0){
                x_right = x + 1;
                x_left = im->width - 1;
            } else if(x == (im->width - 1)){
                x_right = 0;
                x_left = x - 1;
            } else{
                x_right = x + 1;
                x_left = x - 1;
            }

            if(y == 0){
                y_down = y + 1;
                y_up = im->height - 1;
            } else if(y == (im->height - 1)){
                y_down = 0;
                y_up = y - 1;
            } else{
                y_down = y + 1;
                y_up = y - 1;
            }

            int red_x = get_pixel(im, y, x_right, 0) - get_pixel(im, y, x_left, 0);
            int green_x = get_pixel(im, y, x_right, 1) - get_pixel(im, y, x_left, 1);
            int blue_x = get_pixel(im, y, x_right, 2) - get_pixel(im, y, x_left, 2);

            int red_y = get_pixel(im, y_down, x, 0) - get_pixel(im, y_up, x, 0);
            int green_y = get_pixel(im, y_down, x, 1) - get_pixel(im, y_up, x, 1);
            int blue_y = get_pixel(im, y_down, x, 2) - get_pixel(im, y_up, x, 2);

            int delta_x = red_x * red_x + green_x * green_x + blue_x * blue_x;
            int delta_y = red_y * red_y + green_y * green_y + blue_y * blue_y;
            int energy = sqrt(delta_x + delta_y);

            //put the energy into the corresponding pixel spots
            uint8_t dual_grad_energy = (uint8_t)(energy/10);
            set_pixel(*grad, y, x, dual_grad_energy, dual_grad_energy, dual_grad_energy);
            
        }
    }
}

void dynamic_seam(struct rgb_img *grad, double **best_arr)
{
    *best_arr = (double *)malloc(sizeof(double)*(grad->height)*(grad->width));
    for(int i = 0; i < grad->height; i++){ 
        for(int j = 0; j < grad->width; j++){
            if(i == 0){
                (*best_arr)[j] = (grad->raster)[3 * j];
            }else{
                int k;
                int v;
                if(j == 0){
                    k = 0;
                    v = 2;
                }else if(j == (grad->width - 1)){
                    k = -1;
                    v = 1;
                }else{
                    k = -1;
                    v = 2;
                }
                uint8_t opt = (*best_arr)[(i - 1)*(grad->width)+j + k];
                uint8_t prev_opt;
                for(k; k < v; k++){
                    prev_opt = (*best_arr)[(i-1)*(grad->width)+j + k];
                    if(prev_opt < opt){
                        opt = prev_opt;
                    }
                }
                (*best_arr)[i*(grad->width)+j] = opt + (grad->raster)[3*(i*(grad->width)+j)];  
            }
        }
    }
}

void recover_path(double *best, int height, int width, int **path)
{
    /*

    *path = (int *)malloc(sizeof(int)*height);

    for(int i = (height - 1); i > -1; i--){
        int best_path = best[i*width];
        int temp_path;
        for(int j = 0; j < width; j++){
            temp_path = best[i*width + j];
            if(temp_path <= best_path){
                (*path)[i] = j;
                best_path = temp_path;
            }
        }
    }
    */
    // new and improved code
    *path = (int *)malloc(sizeof(int)*height);

    int i = (height - 1);
    int best_path = best[i*width];
    int temp_path;
    for(int j = 0; j < width; j++){
        temp_path = best[i*width + j];
        if(temp_path <= best_path){
            (*path)[i] = j;
            best_path = temp_path;
        }        
    }
  
    for(int k = (height - 2); k > -1; k--){
        int best_path = best[k*width + (*path)[k+1]];
        for(int m = -1; m < 2; m++){
            if(((*path)[k+1] + m) < 0 ||((*path)[k+1] + m) >= width ){
                continue;
            }
            temp_path = best[k*width + (*path)[k+1] + m];
            if (temp_path <= best_path){
                (*path)[k] = (*path)[k+1] + m;
                best_path = temp_path;
            }    
        }
    }

}

void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path)
{
    create_img(dest, src->height, src->width - 1);
    for(int i = 0; i < src->height; i++){
        int k = 0;
        for(int j = 0; j < src->width; j++){
            if(j != path[i]){
                uint8_t red = get_pixel(src, i, j, 0);
                uint8_t green = get_pixel(src, i, j, 1);
                uint8_t blue = get_pixel(src, i, j, 2);
                set_pixel(*dest, i, k, red, green, blue);
                k++;
            }
        }
    }
}

int main(void)
{
/*
    // to make sure you read in the image right
    struct rgb_img *im;
    read_in_img(&im, "6x5.bin");


    
    // to test calc_energy
    struct rgb_img *grad;
    calc_energy(im, &grad);
    

    // to test dynamic _seam
    double *best_arr;
    dynamic_seam(grad, &best_arr);

*/

    // to test recover_path
    double best_arr[15] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 5, 5, 5};

    int height = 3;
    int width = 5;

    int *path;
    recover_path(best_arr, height, width, &path);

    for(int k = 0; k < height; k++){
        printf("%d \t", path[k]);
    }

}