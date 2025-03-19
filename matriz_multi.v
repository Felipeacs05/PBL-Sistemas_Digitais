module matriz_multi(
   input wire rst,
	input wire clk,
	input wire [2:0] size,
	input wire [255:0] matriz_a,
	input wire [255:0] matriz_b,
	output reg [255:0] data_c
);

	wire [255:0] data_c_2x2;
	
	multi2x2(
		clk,
		rst,
		matriz_a,
		matriz_b,
		data_c_2x2
	);
	
	always @(*) begin
		if (size == 0 || size == 1 || size == 6 || size == 7) begin
			data_c = 256'b1;
		end else begin
			case (size)
				3'd2: data_c = data_c_2x2;
				default: data_c = 256'b0;
			endcase
		end
	end


endmodule
