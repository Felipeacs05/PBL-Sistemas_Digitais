`define INTEGER_8 7:0

module MatrizMulti #(parameter SIZE = 5)(
input signed [(8SIZESIZE)-1:0] matriz_a,
input signed [(8SIZESIZE)-1:0] matriz_b,
input clock,
output reg signed [(8SIZESIZE)-1:0] resultado
);

reg [`INTEGER_8] linha;  
integer i, j;  

function integer indice;  
    input integer linha, coluna;  
    indice = 8 * (coluna + SIZE * linha);  
endfunction  

always @(posedge clock) begin  
    for (j = 0; j < SIZE; j = j + 1) begin  
        resultado[indice(linha, j) +: 8] = 0;  
        for (i = 0; i < SIZE; i = i + 1) begin  
            resultado[indice(linha, j) +: 8] = resultado[indice(linha, j) +: 8] +  
                matriz_a[indice(linha, i) +: 8] * matriz_b[indice(i, j) +: 8];  
        end  
    end  

    if (linha == SIZE - 1) begin  
        linha <= 0;  
    end else begin  
        linha <= linha + 1;  
    end  
end

endmodule

