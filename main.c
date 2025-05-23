#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "stb_image.h"
#include "stb_image_write.h"

int sobel_x[3][3] = {
    {-1, 0, 1},
    {-2, 0, 2},
    {-1, 0, 1}
};

int sobel_y[3][3] = {
    {-1, -2, -1},
    { 0,  0,  0},
    { 1,  2,  1}
};

int main() {
    int width, height, channels;
    unsigned char *img = stbi_load("entrada.png", &width, &height, &channels, 1);
    if (!img) {
        printf("Erro ao carregar imagem.\n");
        return 1;
    }

    unsigned char *out = malloc(width * height);
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int gx = 0, gy = 0;
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    int pixel = img[(y + i) * width + (x + j)];
                    gx += pixel * sobel_x[i + 1][j + 1];
                    gy += pixel * sobel_y[i + 1][j + 1];
                }
            }
            int g = sqrt(gx * gx + gy * gy);
            if (g > 255) g = 255;
            out[y * width + x] = (unsigned char)g;
        }
    }

    stbi_write_png("saida.png", width, height, 1, out, width);
    printf("Filtro aplicado e imagem salva.\n");

    stbi_image_free(img);
    free(out);
    return 0;
}