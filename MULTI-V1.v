module matrix_multil (
  input signed [199:0] matriz_a,
  input signed [199:0] matriz_b, 
  input clock,
  output reg signed [199:0] matriz_c = 0,
  output reg done
);

reg [2:0] row = 0; 
reg signed [15:0] temp [0:4];     

always @(posedge clock) begin
    integer i;
    for (i = 0; i < 5; i = i + 1) begin
        temp[i] = (matriz_a[(row * 40) + 7 -: 8]   * matriz_b[(i * 8) +: 8]) +
                  (matriz_a[(row * 40) + 15 -: 8]  * matriz_b[(i * 8) + 40 +: 8]) +
                  (matriz_a[(row * 40) + 23 -: 8]  * matriz_b[(i * 8) + 80 +: 8]) +
                  (matriz_a[(row * 40) + 31 -: 8]  * matriz_b[(i * 8) + 120 +: 8]) +
                  (matriz_a[(row * 40) + 39 -: 8]  * matriz_b[(i * 8) + 160 +: 8]);
    end
    
    for (i = 0; i < 5; i = i + 1) begin
        matriz_c[(row * 40) + (i * 8) + 7 -: 8] <= temp[i][7:0];
    end

    if (row == 4) begin
        row <= 0;
        done <= 1;
    end else begin
        row <= row + 1;
        done <= 0;
    end
end

endmodule

