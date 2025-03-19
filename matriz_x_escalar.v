module matriz_x_escalar(
    input wire [255:0] matriz_a,
    input wire [7:0] escalar,
    output wire [255:0] data_c
);
    
    genvar i;
    
    generate
        for (i = 0; i<256; i = i + 8) begin : multi_escalar
            assign data_c[i +: 8] = matriz_a[i +: 8] * escalar;
        end
    endgenerate

endmodule
