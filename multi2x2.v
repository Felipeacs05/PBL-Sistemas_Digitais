module multi2x2 (
    input clk,  
    input rst,
    input [255:0] matrizA,  
    input [255:0] matrizB,  
    output reg [255:0] matrizC  
);

    reg [7:0] A00, A01, A10, A11;
    reg [7:0] B00, B01, B10, B11;

    reg [15:0] P0, P1, P2, P3; 
    reg [15:0] C00, C01, C10, C11; 

    parameter IDLE = 3'b000, FETCH = 3'b001, MULTIPLY = 3'b010, SUM = 3'b011, OUTPUT = 3'b100;
    reg [2:0] state;

    always @(posedge clk or posedge rst) begin
        if (rst) begin 
            state <= IDLE;
            matrizC <= 256'b0;
        end else begin
            case (state)
                IDLE: begin
                    state <= FETCH;
                end

                FETCH: begin
                    A00 <= matrizA[7:0]; A01 <= matrizA[15:8];
                    A10 <= matrizA[23:16]; A11 <= matrizA[31:24];

                    B00 <= matrizB[7:0]; B01 <= matrizB[15:8];
                    B10 <= matrizB[23:16]; B11 <= matrizB[31:24];

                    state <= MULTIPLY;
                end

                MULTIPLY: begin
                    P0 <= A00 * B00 + A01 * B10;
                    P1 <= A00 * B01 + A01 * B11;
                    P2 <= A10 * B00 + A11 * B10;
                    P3 <= A10 * B01 + A11 * B11;

                    state <= SUM;
                end

                SUM: begin
                    C00 <= P0;
                    C01 <= P1;
                    C10 <= P2;
                    C11 <= P3;

                    state <= OUTPUT;
                end

                OUTPUT: begin
                    matrizC <= {224'b0, C11, C10, C01, C00};
                end

            endcase
        end
    end

endmodule
