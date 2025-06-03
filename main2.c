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
int roberts(int m[2][5]){ // O parâmetro m aqui é [2][5] mas a máscara é [2][2]
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
    return x;
}

// Sobel 3x3
int sobel(int m[3][5]){ // O parâmetro m aqui é [3][5] mas a máscara é [3][3]
    int mask0[3][3] = { // Corrigido para [3][3] para corresponder ao uso
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };
    int mask1[3][3] = { // Corrigido para [3][3]
        {1, 2, 1},
        {0, 0, 0},
        {-1, -2, -1}
    };
    int sumX = 0, sumY = 0;
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            sumX = sumX + mask0[i][j] * m[i][j];
            sumY = sumY + mask1[i][j] * m[i][j]; 
        }
    }
    int resultado = sqrt(sumX*sumX + sumY*sumY);
    return resultado;
}

// PreWitt 3x3 
int preWitt(int m[3][5]){ // O parâmetro m aqui é [3][5] mas a máscara é [3][3]
    int mask0[3][3] = { // Corrigido para [3][3]
        {-1, 0, 1},
        {-1, 0, 1},
        {-1, 0, 1}
    };
    int mask1[3][3] = { // Corrigido para [3][3]
        {-1,-1,-1},
        {0,0,0},
        {1,1,1}
    };
    int sumX = 0, sumY = 0;
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
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
        { 2,  2,  4,  2,  2 },
        { 1,  1,  2,  1,  1 },
        { 0,  0,  0,  0,  0 },
        { -1, -1, -2, -1, -1 },
        { -2, -2, -4, -2, -2 }
    };
    int sobel5x5_Y[5][5] = {
        {  2,  1,  0, -1, -2 },
        {  2,  1,  0, -1, -2 },
        {  4,  2,  0, -2, -4 },
        {  2,  1,  0, -1, -2 },
        {  2,  1,  0, -1, -2 }
    };
    long long sumX_ll = 0, sumY_ll = 0; // Usar long long para somas intermediárias
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            sumX_ll += (long long)sobel5x5_X[i][j] * m[i][j];
            sumY_ll += (long long)sobel5x5_Y[i][j] * m[i][j];
        }
    }
    // Converter de volta para int após a soma, se necessário, ou usar double para sqrt
    int sumX = (int)sumX_ll;
    int sumY = (int)sumY_ll;
    int result = sqrt((double)sumX * sumX + (double)sumY * sumY);
    return result;
}

// Laplaciano 5x5
int laplaciano(int m[5][5]){
    int mask0[5][5] = {
        { 0,  0, -1,  0,  0 },
        { 0, -1, -2, -1,  0 },
        {-1, -2, 16, -2, -1 },
        { 0, -1, -2, -1,  0 },
        { 0,  0, -1,  0,  0 }
    };
    long long sumG_ll = 0; // Usar long long para a soma
    for (int i = 0; i < 5; i++){
        for (int j = 0; j < 5; j++){
            sumG_ll = sumG_ll + (long long)mask0[i][j] * m[i][j];
        }
    }
    return (int)sumG_ll; // O resultado do Laplaciano pode ser negativo
}

// Geração da matriz 5x5 com tratamento de bordas (espelhamento/replicação)
// ATENÇÃO: A versão original desta função não tinha tratamento de bordas robusto.
// A versão abaixo inclui o tratamento de bordas por replicação que estava comentado.
int funcTeste5x5(unsigned char *dados, int current_y, int current_x, int largura, int altura, int operacao) {
    int matriz_temp[5][5];

    for (int r_kernel = 0; r_kernel < 5; r_kernel++) { // Linha do kernel
        for (int c_kernel = 0; c_kernel < 5; c_kernel++) { // Coluna do kernel
            int y_img = current_y + r_kernel - 2; // Coordenada y na imagem (centro do kernel em current_y)
            int x_img = current_x + c_kernel - 2; // Coordenada x na imagem

            // Tratamento de borda por replicação
            if (y_img < 0) y_img = 0;
            if (y_img >= altura) y_img = altura - 1;
            if (x_img < 0) x_img = 0;
            if (x_img >= largura) x_img = largura - 1;

            matriz_temp[r_kernel][c_kernel] = dados[y_img * largura + x_img];
        }
    }

    if(operacao == 4){
        return sobel_expandido(matriz_temp);
    } else if(operacao == 5){
        return laplaciano(matriz_temp);
    }
    return 0; // Caso de operação não reconhecida para 5x5
}

// Geração da matriz 3x3 com tratamento de bordas
int funcTeste3x3(unsigned char *dados, int current_y, int current_x, int largura, int altura_img, int operacao){
    int matriz_temp[3][5]; // Declarado como [3][5], mas usado como [3][3]
    // O parâmetro 'tamanho' original foi substituído por 'altura_img' para clareza
    // e o 'tamanho--' foi removido pois a lógica de borda foi ajustada.
    
    int linha_mat = 0;
    for (int r_offset = -1; r_offset <= 1; r_offset++){ // Offset da linha do kernel: -1, 0, 1
        int coluna_mat = 0;
        for (int c_offset = -1; c_offset <= 1; c_offset++){ // Offset da coluna do kernel: -1, 0, 1
            int y_img = current_y + r_offset;
            int x_img = current_x + c_offset;

            // Tratamento de borda por replicação simples
            if (y_img < 0) y_img = 0;
            if (y_img >= altura_img) y_img = altura_img - 1;
            if (x_img < 0) x_img = 0;
            if (x_img >= largura) x_img = largura - 1;
            
            matriz_temp[linha_mat][coluna_mat] = dados[y_img * largura + x_img];
            coluna_mat++;
        }
        linha_mat++;
    }

    if(operacao == 2){
        return sobel(matriz_temp);
    } else if(operacao == 3){
        return preWitt(matriz_temp);
    }
    return 0; // Caso de operação não reconhecida para 3x3
}

// Geração da matriz 2x2
int funcTeste2x2(unsigned char *dados, int current_y, int current_x, int largura, int altura_img){
    int matriz_temp[2][5]; // Declarado como [2][5], usado como [2][2]
    
    // Para Roberts 2x2, (current_y, current_x) é o canto superior esquerdo da janela 2x2.
    // Precisa garantir que não saia dos limites para current_y+1 e current_x+1.
    if (current_y + 1 >= altura_img || current_x + 1 >= largura) {
        // Se estiver na borda onde uma janela 2x2 não pode ser formada completamente,
        // retorna 0 (ou outro tratamento de borda, como replicação do último pixel válido).
        // Isso pode criar uma borda preta na imagem de saída se o loop principal não for ajustado.
        return 0; 
    }

    matriz_temp[0][0] = dados[current_y * largura + current_x];
    matriz_temp[0][1] = dados[current_y * largura + (current_x + 1)];
    matriz_temp[1][0] = dados[(current_y + 1) * largura + current_x];
    matriz_temp[1][1] = dados[(current_y + 1) * largura + (current_x + 1)];

    return roberts(matriz_temp);
}


// =======================================================================================
// ATENÇÃO: Corrigido para incluir os 'return' statements que estavam faltando.
// O parâmetro 'tamanho' foi renomeado para 'altura_dados' para maior clareza.
// =======================================================================================
int calcularGeratriz(unsigned char *dados, int i, int j, int larg_dados, int altura_dados, int operacao){
    if(operacao == 1){ // Roberts
        return funcTeste2x2(dados, i, j, larg_dados, altura_dados);
    } else if(operacao == 2 || operacao == 3){ // Sobel 3x3, Prewitt 3x3
        return funcTeste3x3(dados, i, j, larg_dados, altura_dados, operacao);
    } else if(operacao == 4 || operacao == 5){ // Sobel 5x5, Laplaciano 5x5
        return funcTeste5x5(dados, i, j, larg_dados, altura_dados, operacao);
    }
    // Caso padrão se 'operacao' não for um valor esperado (embora o menu deva prevenir isso)
    fprintf(stderr, "Aviso: Operacao desconhecida em calcularGeratriz: %d\n", operacao);
    return 0; // Retorno padrão para valor de operação não tratado
}

int main() {
    const char *input_filename = "lenna.jpeg";
    char *output_filename_ptr = "foto.png"; // Renomeado para evitar conflito com nome de buffer

    int width, height, channels;
    unsigned char *data = stbi_load(input_filename, &width, &height, &channels, 1); // Carrega como grayscale
    if (!data) {
        printf("Erro ao carregar imagem '%s'\n", input_filename);
        return 1;
    }

    // temp_data não estava sendo usado no pipeline de filtro que você mostrou.
    // Se você não o usa, pode remover a alocação e liberação.
    // double *temp_data = malloc(width * height * sizeof(double)); 
    unsigned char *output_data = (unsigned char *)malloc(width * height * sizeof(unsigned char));

    if (/*!temp_data ||*/ !output_data) { // Removido temp_data da checagem se não for usado
        printf("Erro ao alocar memória.\n");
        stbi_image_free(data);
        // free(temp_data); // Liberar se for alocado
        free(output_data); // Liberar output_data se a alocação dele falhou (improvável aqui, mas boa prática)
        return 1;
    }

    printf("\nImagem carregada: %dx%d\n", width, height);
    
    int operacao = 0;
    // 'comp' não estava sendo usado no escopo mais amplo da main, é local para o case 6.
    // double max_value = 0.0; // Não estava sendo usado.

    // Loop principal do menu
    do {
        printf("\n--------------------------------MENU--------------------------------\n");
        printf("Escolha uma operacao:\n");
        printf("[1] Aplicar filtro Roberts (2x2)\n");
        printf("[2] Aplicar filtro Sobel (3x3)\n");
        printf("[3] Aplicar filtro Prewitt (3x3)\n");
        printf("[4] Aplicar filtro Sobel Expandido (5x5)\n");
        printf("[5] Aplicar filtro Laplaciano (5x5)\n");
        printf("[6] Comparar Imagens C vs FPGA (usando a funcao de compararFPGA_C.h)\n");
        printf("[7] Sair\n");
        printf("Opcao: ");

        if (scanf("%d", &operacao) != 1) {
            printf("Entrada invalida. Por favor, digite um numero.\n");
            while (getchar() != '\n'); // Limpar buffer de entrada
            operacao = 0; // Forçar repetição do menu
            continue;
        }
        
        char current_output_filename_buffer[128]; // Buffer para nomes de arquivo de saída

        if (operacao >= 1 && operacao <= 5) { // Aplicar filtro
            const char *filter_name_str = "";
            switch(operacao){
                case 1: filter_name_str = "roberts"; break;
                case 2: filter_name_str = "sobel"; break;
                case 3: filter_name_str = "prewitt"; break;
                case 4: filter_name_str = "sobel_expandido"; break;
                case 5: filter_name_str = "laplaciano"; break;
            }
            sprintf(current_output_filename_buffer, "outputC/%s.png", filter_name_str);
            output_filename_ptr = current_output_filename_buffer; // Atualiza o ponteiro

            printf("\nAplicando filtro '%s' e salvando em '%s'...\n", filter_name_str, output_filename_ptr);

            // ATENÇÃO aos limites do loop e ao tratamento de bordas nas funcTeste*
            // Se funcTeste* lidam com todas as bordas para i,j de 0 a height-1/width-1,
            // então o loop pode ir até height e width.
            // Se, por exemplo, Roberts produz uma imagem (W-1)x(H-1), o loop e o stbi_write_png precisam ser ajustados.
            // Assumindo por agora que output_data é preenchido completamente.
            for (int y = 0; y < height; y++) { // Loop até height (ajuste se necessário)
                for (int x = 0; x < width; x++) { // Loop até width (ajuste se necessário)
                    int temporario = calcularGeratriz(data, y, x, width, height, operacao);
                    
                    // Clamping do resultado do filtro para o range 0-255
                    if (temporario > 255) temporario = 255;
                    else if (temporario < 0) temporario = 0; // Importante para Laplaciano
                    
                    output_data[y * width + x] = (unsigned char)temporario;
                }
            }
            
            if (stbi_write_png(output_filename_ptr, width, height, 1, output_data, width)) {
                 printf("Imagem salva como '%s'\n", output_filename_ptr);
            } else {
                 printf("Erro ao salvar imagem '%s'\n", output_filename_ptr);
            }

        } else if (operacao == 6) { // Comparar Imagens C vs FPGA
            int filter_choice_for_comparison;
            
            printf("\n--- Comparacao de Imagens C vs FPGA ---\n");
            printf("Qual conjunto de resultados de filtro deseja comparar?\n");
            printf("As imagens devem estar em 'outputC/' e 'outputDafema/' respectivamente.\n");
            printf("A diferenca sera salva em 'outputDifC_FPGA/'.\n\n");
            printf("[1] Roberts\n");
            printf("[2] Sobel\n");
            printf("[3] Prewitt\n");
            printf("[4] Sobel Expandido\n");
            printf("[5] Laplaciano\n");
            printf("Digite o numero do filtro para comparar: ");

            if (scanf("%d", &filter_choice_for_comparison) != 1) {
                printf("Entrada invalida. Por favor, digite um numero.\n");
                while (getchar() != '\n'); 
                // Não faz nada, volta ao menu principal
            } else {
                char base_filename_str[64];
                int valid_choice = 1;

                switch (filter_choice_for_comparison) {
                    case 1: strcpy(base_filename_str, "roberts"); break;
                    case 2: strcpy(base_filename_str, "sobel"); break;
                    case 3: strcpy(base_filename_str, "prewitt"); break;
                    case 4: strcpy(base_filename_str, "sobel_expandido"); break;
                    case 5: strcpy(base_filename_str, "laplaciano"); break;
                    default:
                        printf("Opcao de filtro invalida (%d).\n", filter_choice_for_comparison);
                        valid_choice = 0;
                        break;
                }

                if (valid_choice) {
                    char path_image_c[128];
                    char path_image_fpga[128];
                    char path_image_difference[128];

                    sprintf(path_image_c, "outputC/%s.png", base_filename_str);
                    sprintf(path_image_fpga, "outputDafema/%s.png", base_filename_str);
                    sprintf(path_image_difference, "outputDifC_FPGA/%s_diff.png", base_filename_str); 

                    printf("\nComparando:\n  Imagem C:     %s\n  Imagem FPGA:  %s\n", path_image_c, path_image_fpga);
                    printf("  Salvando diferenca em: %s\n", path_image_difference);

                    if (comparar_imagens_diferenca(path_image_c, path_image_fpga, path_image_difference) == 0) {
                        // Mensagem de sucesso já é impressa pela função de comparação
                    } else {
                        // Mensagem de erro já é impressa pela função de comparação
                        printf("Falha ao comparar imagens. Verifique os caminhos e se as imagens existem com as mesmas dimensoes.\n");
                    }
                }
            }
        } else if (operacao == 7) {
            printf("Saindo do programa...\n");
        } else {
            printf("Opcao invalida. Tente novamente.\n");
        }
        printf("--------------------------------------------------------------------\n");

    } while (operacao != 7);


    // Liberar memória
    stbi_image_free(data);
    // free(temp_data); // Liberar se for alocado e usado
    free(output_data);

    printf("Programa finalizado.\n");
    return 0;
}
