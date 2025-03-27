module matriz_determ4x4 (
    input [199:0] matriz_A, 
    output reg [31:0] det 
);
    reg [7:0] mat [3:0][3:0]; 
    integer i, j;

    function [31:0] det_3x3;
        input [7:0] m0, m1, m2; 
        begin
            det_3x3 = m0[7:4] * (m1[3:0] * m2[7:4] - m1[7:4] * m2[3:0]) - m0[3:0] * (m1[7:4] * m2[7:4] - m1[7:4] * m2[3:0]);
        end
    endfunction

    always @(*) begin
        for (i = 0; i < 4; i = i + 1) begin
            for (j = 0; j < 4; j = j + 1) begin
                mat[i][j] = matriz_A[(i * 32) + (j * 8) +: 8];
            end
        end

        det = 
            mat[0][0] * det_3x3(mat[1][0], mat[2][0], mat[3][0]) -
            mat[0][1] * det_3x3(mat[1][1], mat[2][1], mat[3][1]) +
            mat[0][2] * det_3x3(mat[1][2], mat[2][2], mat[3][2]) -
            mat[0][3] * det_3x3(mat[1][3], mat[2][3], mat[3][3]);
    end
endmodule

