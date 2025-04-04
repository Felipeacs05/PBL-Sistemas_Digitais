// Determinante 5x5
/////////////////////////////////////////18 DSP BLOCKS///////////////////////////////////////////////////////////////////////////////

module matriz_determ5x5 (
	input wire [199:0] matriz_A,
	input clk,
	input start,
	output reg done,
	output reg [7:0] det
);

	wire [7:0] mat [4:0][4:0]; 
	reg [31:0] tp [4:0];
	reg [4:0] count = 0;
	
	reg [199:0] bitouni [4:0];
	
	reg [199:0] sob;
	
	reg start2 = 0;
	
	matriz_determ4x4(
		sob,
		clk,
		start2,
		done2,
		saidadet4x4
	);
	

	genvar i,j;
	
	generate begin
		for (i = 0; i < 5; i = i + 1) begin : asdasd
			for (j = 0; j < 5; j = j + 1) begin: aksdkaskd
				 assign mat[i][j] = matriz_A[(i * 40) + (j * 8) +: 8];
		end end end
	endgenerate
	
	always @(posedge clk) begin
		  if(start) begin
			  if (!done2 & count < 5) begin
					start2 = 1;
					bitouni[count] = {mat[4][(count+4)%5], mat[4][(count+3)%5], mat[4][(count+2)%5], mat[4][(count+1)%5],
											8'b0, mat[3][(count+4)%5], mat[3][(count+3)%5], mat[3][(count+2)%5], mat[3][(count+1)%5],
											8'b0, mat[2][(count+4)%5], mat[2][(count+3)%5], mat[2][(count+2)%5], mat[2][(count+1)%5],
											8'b0, mat[1][(count+4)%5], mat[1][(count+3)%5], mat[1][(count+2)%5], mat[1][(count+1)%5]};
					sob = bitouni[count];
					tp[count] = mat[0][count] * saidadet4x4;
			  end else if(done2) begin
					start2 <= 0;
			      count <= count + 1;
			  end else begin
					det = tp[0] - tp[1] + tp[2] - tp[3] + tp[4];
					done = 1;
			  end
			  
		  end else begin
			done = 0;
			det = 0;
			count = 0;
		  end
    end


endmodule
