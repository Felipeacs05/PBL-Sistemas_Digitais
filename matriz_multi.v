module matriz_multi(
    input wire [255:0] matriz_a,
    input wire [255:0] matriz_b,
    output wire [255:0] data_c
);

    wire [15:0] parcial[0:4][0:4][0:4]; // Armazena os produtos parciais
    wire [15:0] soma[0:4][0:4]; // Armazena as somas intermediárias

    genvar i, j, k;
    
    generate
        for (i = 0; i < 5; i = i + 1) begin : linha
            for (j = 0; j < 5; j = j + 1) begin : coluna
                for (k = 0; k < 5; k = k + 1) begin : multiplicacao
                    assign parcial[i][j][k] = 
                        matriz_a[(8*k) + (40*i) +: 8] * matriz_b[(40*k) + (8*j) +: 8];
                end
                assign soma[i][j] = 
                    parcial[i][j][0] + parcial[i][j][1] + 
                    parcial[i][j][2] + parcial[i][j][3] + 
                    parcial[i][j][4];
                assign data_c[(8*j) + (40*i) +: 8] = soma[i][j][7:0]; // Pega apenas 8 bits
            end
        end
    endgenerate

endmodule