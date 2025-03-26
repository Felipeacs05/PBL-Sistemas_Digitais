module MultiplicadorMatriz (
    input signed [199:0] matriz_a, // Matriz 5x5 achatada (cada elemento com 8 bits)
    input signed [199:0] matriz_b, // Matriz 5x5 achatada (cada elemento com 8 bits)
    input [7:0] tamanho,
    input clock,
    output reg signed [199:0] resultado // Matriz 5x5 achatada (cada elemento com 16 bits)
);

    reg [7:0] linha;
    integer i, j;

    function integer indice;
        input integer linha, coluna;
        indice = 8 * (coluna + 5 * linha);
    endfunction

    always @(posedge clock) begin
        for (j = 0; j < 5; j = j + 1) begin
            resultado[indice(linha, j) +: 8] = 0;
            for (i = 0; i < 5; i = i + 1) begin
                resultado[indice(linha, j) +: 8] = resultado[indice(linha, j) +: 8] +
                    matriz_a[indice(linha, i) +: 8] * matriz_b[indice(i, j) +: 8];
            end
        end
        
        if (linha == 4) begin
            linha <= 0;
        end else begin
            linha <= linha + 1;
        end
    end
endmodule
