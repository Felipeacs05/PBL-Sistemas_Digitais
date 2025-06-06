module main_multi (
	input wire rst,
	input wire sw,
	input wire clk,
	output reg [7:0] leds
	);

	reg [7:0] address = 0;
	reg clk2;
	reg clk3;
	reg [7:0] i = 0;
	
	reg [199:0] matriz_A;
	reg [199:0] matriz_B;
	wire [199:0] matriz_C;
	
	reg read_finished = 0;
	reg write_enable = 0;
	
	reg [24:0] counter = 0;
	reg [24:0] counter2 = 0;
	reg[7:0] count = 0;
	
	wire [255:0] data;

	RamGigante (
	  address,
	  clk,
	  matriz_C,
	  write_enable,
	  data
	);
	
	//

   debounce (
	  rst,
	  clk,
	  rstdebounce
	);
	
	matriz_multi (
	  matriz_A,
	  matriz_B,
	  clk3,
	  matriz_C
	);
   
	always @(posedge clk) begin
	  if (counter == 25'd24_999_999) begin  
			counter <= 0;
			clk2 <= 1;
	  end else begin
			counter <= counter + 1;
			clk2 <= 0;
	  end
	end

	always @(posedge clk) begin
		  if (counter2 == 3) begin  
				counter2 <= 0;
				clk3 <= 1;
		  end else begin
				counter2 <= counter2 + 1;
				clk3 <= 0;
		  end
	 end


    always @(posedge clk3) begin
		if (sw) begin  
		  case (count)
				0: begin
					 count <= count + 1;
				end
				1: begin
					 matriz_A <= data;
					 count <= count + 1;
				end
				2: begin
					 address <= 1;
					 count <= count + 1;
				end
				3: begin
					 matriz_B <= data;
					 count <= count + 1;
				end
				4: begin
					 address <= 2;
					 count <= count + 1;
				end
				5: begin
					 write_enable <= 1;
					 count <= count + 1;
				end
				6: begin
					 count <= count + 1;
				end
				7: begin
					count <= count + 1;
				end
				8: begin
					 write_enable <= 0;
					 read_finished <= 1;
				end
				endcase
		  end
	 end

    always @(posedge clk2) begin
        if (read_finished) begin
            leds <= data[i +: 8];
            i <= i + 8;
            if (i >= 255) begin
                i <= 0;  
            end
        end
    end

endmodule
