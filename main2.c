// main2.c

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "stb_image.h"
#include "stb_image_write.h"

static int comparar_imagens_diferenca(const char *imagem1_path, const char *imagem2_path, const char *diferenca_path) {
    int w1, h1, channels1;
    unsigned char *img1_data = stbi_load(imagem1_path, &w1, &h1, &channels1, 1);
    if (!img1_data) {
        fprintf(stderr, "Erro ao carregar imagem '%s'\n", imagem1_path);
        return -1;
    }

    int w2, h2, channels2;
    unsigned char *img2_data = stbi_load(imagem2_path, &w2, &h2, &channels2, 1);
    if (!img2_data) {
        fprintf(stderr, "Erro ao carregar imagem '%s'\n", imagem2_path);
        stbi_image_free(img1_data);
        return -1;
    }

    if (w1 != w2 || h1 != h2) {
        fprintf(stderr, "As imagens têm dimensões diferentes: Imagem1 (%dx%d) vs Imagem2 (%dx%d)\n", w1, h1, w2, h2);
        stbi_image_free(img1_data);
        stbi_image_free(img2_data);
        return -1;
    }

    unsigned char *diff_img_data = (unsigned char *)malloc(w1 * h1 * sizeof(unsigned char));
    if (!diff_img_data) {
        fprintf(stderr, "Erro ao alocar memória para a imagem de diferença.\n");
        stbi_image_free(img1_data);
        stbi_image_free(img2_data);
        return -1;
    }

    // Constante para a escala logarítmica: log(1 + valor_maximo_absoluto_da_diferenca_possivel)
    // A diferença absoluta máxima entre dois unsigned char (0-255) é 255.
    // log1p(x) é uma função que calcula log(1+x) de forma mais precisa para x pequeno.
    const double log_de_256 = log1p(255.0); // log(1 + 255) = log(256)

    for (int i = 0; i < w1 * h1; ++i) {
        // 1. Calcula a diferença e pega o módulo (valor absoluto)
        int diferenca_original = (int)img1_data[i] - (int)img2_data[i];
        int diferenca_absoluta = abs(diferenca_original); // Valor entre 0 e 255

        // 2. Aplica a escala logarítmica
        // A fórmula é: (log(1 + diferenca_abs) / log(1 + max_diferenca_abs_possivel)) * 255
        // Se diferenca_absoluta for 0, log1p(0) é 0, então valor_final_pixel será 0.
        double valor_final_pixel = (log1p((double)diferenca_absoluta) / log_de_256) * 255.0;
        
        // Garante que o valor está dentro dos limites de unsigned char (0-255)
        // e arredonda antes de converter para unsigned char.
        // O clamping aqui é mais uma segurança para flutuações de ponto flutuante,
        // pois a fórmula já mapeia para [0, 255].
        if (valor_final_pixel < 0.0) valor_final_pixel = 0.0;
        if (valor_final_pixel > 255.0) valor_final_pixel = 255.0;
        
        diff_img_data[i] = (unsigned char)(valor_final_pixel + 0.5); // Adiciona 0.5 para arredondamento
    }

    if (!stbi_write_png(diferenca_path, w1, h1, 1, diff_img_data, w1)) {
        fprintf(stderr, "Erro ao salvar a imagem de diferença '%s'\n", diferenca_path);
        // ... (liberação de memória em caso de erro) ...
        stbi_image_free(img1_data);
        stbi_image_free(img2_data);
        free(diff_img_data);
        return -1;
    }

    printf("Imagem de diferença (abs(A-B) com escala logarítmica) salva como '%s'\n", diferenca_path);
    printf("  Nesta imagem: Preto (0) = sem diferença.\n");
    printf("              Outros tons = diferença presente, com realce para pequenas diferenças.\n");

    stbi_image_free(img1_data);
    stbi_image_free(img2_data);
    free(diff_img_data);

    return 0; // Sucesso
}

// Roberts 2x2
int roberts(int m[2][5]){

    int mask0[2][2] ={
        {1, 0},
        {0, -1} 
    };

    int mask1[2][2] ={
        {0, 1},
        {-1, 0} 
    };

    int sumX = 0, sumY = 0;

    for (int i = 0; i < 2; i++){
        for (int j = 0; j < 2; j++){
            sumX = sumX + mask0[i][j] * m[i][j];
            sumY = sumY + mask1[i][j] * m[i][j]; 
        }
    }

    int x = sqrt(sumX*sumX + sumY*sumY);
    //printf("%f\n", x);
    return x;
}

// Sobel 3x3
int sobel(int m[3][5]){


    int mask0[3][5] = { // Na sua versão original, as máscaras eram [3][5]
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };
    
    int mask1[3][5] = { // Na sua versão original, as máscaras eram [3][5]
        {1, 2, 1},
        {0, 0, 0},
        {-1, -2, -1}
    };

    int sumX = 0, sumY = 0;

    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){ // Loops iteram 3x3
            sumX = sumX + mask0[i][j] * m[i][j];
            sumY = sumY + mask1[i][j] * m[i][j]; 
        }
    }

    int resultado = sqrt(sumX*sumX + sumY*sumY);

    return resultado;
}

// PreWitt 3x3 
int preWitt(int m[3][5]){

    int mask0[3][5] = { // Na sua versão original, as máscaras eram [3][5]
        {-1, 0, 1},
        {-1, 0, 1},
        {-1, 0, 1}
    };
    
    int mask1[3][5] = { // Na sua versão original, as máscaras eram [3][5]
        {-1,-1,-1},
        {0,0,0},
        {1,1,1}
    };

    int sumX = 0, sumY = 0;
    
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){ // Loops iteram 3x3
            sumX = sumX + mask0[i][j] * m[i][j];
            sumY = sumY + mask1[i][j] * m[i][j]; 
        }
    }

    int resultado = sqrt(sumX*sumX + sumY*sumY);

    return resultado;
}

// Sobel Expandido 5x5
int sobel_expandido(int m[5][5]) {

    int sobel5x5_X[5][5] = {
        { 2, 2, 4, 2, 2 },
        { 1, 1, 2, 1, 1 },
        { 0, 0, 0, 0, 0 },
        { -1, -1, -2, -1, -1 },
        { -2, -2, -4, -2, -2 }
    };

    int sobel5x5_Y[5][5] = {
        { 2, 1, 0, -1, -2 },
        { 2, 1, 0, -1, -2 },
        { 4, 2, 0, -2, -4 },
        { 2, 1, 0, -1, -2 },
        { 2, 1, 0, -1, -2 }
    };

    int sumX = 0, sumY = 0;

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            sumX += sobel5x5_X[i][j] * m[i][j];
            sumY += sobel5x5_Y[i][j] * m[i][j];
        }
    }

    int result = sqrt(sumX * sumX + sumY * sumY);

    //printf("\n %d", result);
    return result;
}

// Laplaciano 5x5
int laplaciano(int m[5][5]){
    
    int mask0[5][5] = {
    { 0,  0,  -1,  0,  0 },
    { 0, -1,  -2, -1,  0 },
    {-1, -2,  16, -2, -1 },
    { 0, -1,  -2, -1,  0 },
    { 0,  0,  -1,  0,  0 }
    };

    int sumG = 0;

    for (int i = 0; i < 5; i++){
        for (int j = 0; j < 5; j++){
            sumG = sumG + mask0[i][j] * m[i][j];
        }
    }

    return sumG;
}

// Geração da matriz 5x5 com tratamento de bordas (espelhamento)
// Esta é a versão da funcTeste5x5 que você enviou, com a parte do tratamento de bordas comentada
int funcTeste5x5(unsigned char *dados, int i, int j, int largura, int altura, int operacao) {
    /* int matriz_temp[5][5];

    for (int linha = 0; linha < 5; linha++) {
        for (int coluna = 0; coluna < 5; coluna++) {
            int y = i + linha - 2;
            int x = j + coluna - 2;

            // Tratamento de borda por replicação
            if (y < 0) y = 0;
            if (y >= altura) y = altura - 1;
            if (x < 0) x = 0;
            if (x >= largura) x = largura - 1;

            matriz_temp[linha][coluna] = dados[y * largura + x];
        }
    }

    return sobel_expandido(matriz_temp); // Esta linha estava dentro do comentário, pode precisar de ajuste
    */

    int matriz_temp[5][5];
    int linha = 0;
    int coluna = 0;  

    // printf("\n %d %d", i, j);
    for(int linhaTemp = i - 2; linhaTemp < (i + 3); linhaTemp++){
        for(int colunaTemp = j - 2; colunaTemp < (j + 3); colunaTemp++){
            // ATENÇÃO: esta linha pode causar acesso fora dos limites se i,j estiverem perto das bordas
            // e o tratamento de bordas acima estiver comentado.
            matriz_temp[linha][coluna] = dados[linhaTemp* largura + colunaTemp];
            coluna++;
        }
        coluna = 0;
        linha++;
    }

    if(operacao == 4){
        return sobel_expandido(matriz_temp);
    } else if(operacao == 5){
        return laplaciano(matriz_temp);
    }

    return 0;
}

int funcTeste3x3(unsigned char *dados, int i, int j, int larg_dados, int tamanho, int operacao){

    int matriz_temp[3][5];
    tamanho--; // Esta era a sua lógica original com 'tamanho'
    
    int linha = 0, coluna = 0;

    // Montando 3x3 parcial
    for (int w = i - 1; w < (i + 2); w++){
        for (int z = j - 1; z < (j + 2); z++){
            if ((w < 0 || w > tamanho) && (z < 0 || z > tamanho)) matriz_temp[linha][coluna] = dados[i*larg_dados + j];
            else if (w < 0) matriz_temp[linha][coluna] = dados[(w+1)*larg_dados + z];
            else if (w > tamanho) matriz_temp[linha][coluna] = dados[(w-1)*larg_dados + z];
            else if (z < 0) matriz_temp[linha][coluna] = dados[w*larg_dados + z + 1];
            else if (z > tamanho) matriz_temp[linha][coluna] = dados[w*larg_dados + z - 1];
            else matriz_temp[linha][coluna] = dados[w*larg_dados + z];
            coluna++;
            //printf("%d/%d\n", w, z);
        }
        coluna = 0;
        linha++;
    }

    if(operacao == 2){
        return sobel(matriz_temp);
    } else if(operacao == 3){
        return preWitt(matriz_temp);
    }

    return 0;
}

int funcTeste2x2(unsigned char *dados, int i, int j, int larg_dados, int tamanho){

    int matriz_temp[2][5];    
    
    int linha = 0, coluna = 0;

    for (int w = i; w < (i + 2); w++){
        for (int z = j; z < (j + 2); z++){
            // ATENÇÃO: esta linha pode causar acesso fora dos limites se i,j estiverem perto das bordas
            matriz_temp[linha][coluna] = dados[w*larg_dados + z];
            coluna++;
            //printf("%d/%d\n", w, z);
        }
        coluna = 0;
        linha++;
    }

    return roberts(matriz_temp);
}

// Esta é a sua versão original de calcularGeratriz, com os 'return' faltando
int calcularGeratriz(unsigned char *dados, int i, int j, int larg_dados, int tamanho, int operacao){
    if(operacao == 2 || operacao == 3){
        funcTeste3x3(dados, i, j, larg_dados, tamanho, operacao); // FALTA RETURN
    }else if(operacao == 4 || operacao == 5){
        funcTeste5x5(dados, i, j, larg_dados, tamanho, operacao); // FALTA RETURN
    }else{
        funcTeste2x2(dados, i, j, larg_dados, tamanho); // FALTA RETURN
    }
    // Um valor deveria ser retornado aqui também, ou a função deveria ser void se nada é retornado.
    // O compilador provavelmente dará um aviso sobre "control reaches end of non-void function".
}


int main() {
    const char *input_filename = "lenna.jpeg";
    char *output_filename = "foto.png"; // Sua variável original

    int width, height, channels;
    unsigned char *data = stbi_load(input_filename, &width, &height, &channels, 1);
    if (!data) {
        printf("Erro ao carregar imagem '%s'\n", input_filename);
        return 1;
    }

    double *temp_data = malloc(width * height * sizeof(double)); // Sua alocação original
    unsigned char *output_data = malloc(width * height * sizeof(unsigned char));

    if (!temp_data || !output_data) { // Sua checagem original
        printf("Erro ao alocar memória.\n");
        stbi_image_free(data);
        free(temp_data);
        free(output_data);
        return 1;
    }

    printf("\n%d %d", width, height);
    // Aplicar Sobel 5x5 (armazenando valores temporários) // Comentário original
    double max_value = 0.0; // Sua variável original
    int operacao = 0;
    int comp = 0; // Sua variável original, usada localmente no case 6 depois
    printf("\nDIGITE O FILTRO DESEJADO: ");
    printf("\nFILTROS:\n[1] Roberts(2x2) \n[2] Sobel(3x3) \n[3] Prewitt(3x3) \n[4] Sobel Expandido(5x5) \n[5] Laplaciano(5x5): \n[6] CompararC-FPGA: \n[7] Sair: ");
    scanf("%d", &operacao);

    switch(operacao){
            case 1:
                output_filename = "outputC/roberts.png";
                break;
            case 2:
                output_filename = "outputC/sobel.png";
                break;
            case 3:
                output_filename = "outputC/prewitt.png";
                break;
            case 4:
                output_filename = "outputC/sobel_expandido.png";
                break;
            case 5:
                output_filename = "outputC/laplaciano.png";
                break;
            case 6: { // Este bloco case 6 é a versão que ajustamos juntos e estava no seu último envio completo
                int filter_choice_for_comparison; 
                
                printf("\n--- Comparação de Imagens C vs FPGA ---\n");
                printf("Qual conjunto de resultados de filtro deseja comparar?\n");
                printf("As imagens devem estar em 'outputC/' e 'outputDafema/' respectivamente.\n");
                printf("A diferença será salva em 'outputDifC_FPGA/'.\n\n");
                printf("[1] Roberts\n");
                printf("[2] Sobel\n");
                printf("[3] Prewitt\n");
                printf("[4] Sobel Expandido\n");
                printf("[5] Laplaciano\n");
                printf("Digite o número do filtro para comparar: ");

                if (scanf("%d", &filter_choice_for_comparison) != 1) {
                    printf("Entrada inválida. Por favor, digite um número.\n");
                    while (getchar() != '\n'); 
                    break; 
                }

                char base_filename[64];
                int valid_choice = 1;

                switch (filter_choice_for_comparison) {
                    case 1: strcpy(base_filename, "roberts"); break;
                    case 2: strcpy(base_filename, "sobel"); break;
                    case 3: strcpy(base_filename, "prewitt"); break;
                    case 4: strcpy(base_filename, "sobel_expandido"); break;
                    case 5: strcpy(base_filename, "laplaciano"); break;
                    default:
                        printf("Opção de filtro inválida (%d).\n", filter_choice_for_comparison);
                        valid_choice = 0;
                        break;
                }

                if (valid_choice) {
                    char path_image_c[128];
                    char path_image_fpga[128];
                    char path_image_difference[128];

                    sprintf(path_image_c, "outputC/%s.png", base_filename);
                    sprintf(path_image_fpga, "outputDafema/%s.png", base_filename);
                    sprintf(path_image_difference, "outputDifC_FPGA/%s_diff.png", base_filename); 

                    printf("\nComparando:\n  Imagem C:     %s\n  Imagem FPGA:  %s\n", path_image_c, path_image_fpga);
                    printf("  Salvando diferença em: %s\n", path_image_difference);

                    if (comparar_imagens_diferenca(path_image_c, path_image_fpga, path_image_difference) == 0) {
                        // Mensagem de sucesso é impressa pela função
                    } else {
                        // Mensagem de erro é impressa pela função
                        printf("Falha ao comparar imagens. Verifique os caminhos e se as imagens existem com as mesmas dimensões.\n");
                    }
                }
                break; 
            } 
            default: // Seu default original
                break;
    }

    // Seu loop while original para aplicar filtros
    while (operacao > 0 && operacao < 6){
        for (int y = 0; y < height - 1; y++) { // Seus limites de loop originais
            for (int x = 0; x < width - 1; x++) { // Seus limites de loop originais
                int temporario = calcularGeratriz(data, y, x, width, height, operacao);
                if (temporario>255){
                    temporario=255;
                }else if (temporario<0){
                    temporario=0;
                }
                output_data[y * width + x] = temporario;
                
            }
        }
        
        stbi_write_png(output_filename, width, height, 1, output_data, width);
        printf("Imagem salva como '%s'\n", output_filename);

        // Seu menu interno original
        printf("\nDIGITE O FILTRO DESEJADO ");
        printf("\nFILTROS:\n[1] Roberts(2x2) \n[2] Sobel(3x3) \n[3] Prewitt(3x3) \n[4] Sobel Expandido(5x5) \n[5] Laplaciano(5x5): \n[6] Sair do Programa: ");
        scanf("%d", &operacao); // Leitura para a próxima iteração ou saída
    }

    // Liberar memória
    stbi_image_free(data);
    free(temp_data); // Sua liberação original
    free(output_data);

    return 0;
}
