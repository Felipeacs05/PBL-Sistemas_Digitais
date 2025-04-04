// Determinante 5x5
/////////////////////////////////////////18 DSP BLOCKS///////////////////////////////////////////////////////////////////////////////

module matriz_determ5x5 (
	input wire [199:0] matriz_A,
	input clk,
	input start,
	output reg done,
	output reg [31:0] det
);

	wire [7:0] mat [4:0][4:0]; 
	reg [31:0] tp [4:0];
	reg [2:0] count = 0, c2 = 0;

	genvar i, j;

	function [31:0] det_4x4;
		input [7:0] a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p;
		begin
			det_4x4 = a * det_3x3(f, g, h, j, k, l, n, o, p) - 
						 b * det_3x3(e, g, h, i, k, l, m, o, p) + 
						 c * det_3x3(e, f, h, i, j, l, m, n, p) - 
						 d * det_3x3(e, f, g, i, j, k, m, n, o);
		end
	endfunction
	
	function [31:0] det_3x3;
		input [7:0] a, b, c, d, e, f, g, h, i;
		begin
			det_3x3 = a * (e * i - f * h) -
						 b * (d * i - f * g) +
						 c * (d * h - e * g);
		end
	endfunction
	
	generate begin
		for (i = 0; i < 5; i = i + 1) begin : asdasd
			for (j = 0; j < 5; j = j + 1) begin: aksdkaskd
				 assign mat[i][j] = matriz_A[(i * 40) + (j * 8) +: 8];
		end end end
	endgenerate
	
	always @(posedge clk) begin
		  if(start) begin
			  if (count < 5) begin
					tp[count] = mat[0][count] * det_4x4(
							mat[1][(count+1)%5], mat[1][(count+2)%5], mat[1][(count+3)%5], mat[1][(count+4)%5],
							mat[2][(count+1)%5], mat[2][(count+2)%5], mat[2][(count+3)%5], mat[2][(count+4)%5],
							mat[3][(count+1)%5], mat[3][(count+2)%5], mat[3][(count+3)%5], mat[3][(count+4)%5],
							mat[4][(count+1)%5], mat[4][(count+2)%5], mat[4][(count+3)%5], mat[4][(count+4)%5]
			      );
				
			  end else begin
					det = tp[0] - tp[1] + tp[2] - tp[3] + tp[4];
					done = 1;
			  end
			  
			  count <= count + 1;
			  
		  end else begin
			done = 0;
			det = 0;
			count = 0;
			c2 = 0;
		  end
		
	end


endmodule
