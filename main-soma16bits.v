module main (
	input wire rst,
	input wire sw,
	input wire clk,
	output reg [7:0] leds
	);

	reg [7:0] address = 0;
	reg clk2;
	reg clk3;
	reg [7:0] i = 0;
	
	reg [255:0] matriz_A;
	reg [255:0] matriz_B;
	wire [255:0] matriz_C;
	
	reg [15:0] matriz_A [12:0];
	reg [15:0] matriz_B [12:0];
	wire [255:0] matriz_C;
	reg [7:0] count;
	
	reg [1:0] base;
	
	reg read_finished = 0;
	reg write_enable = 0;
	reg [24:0] counter = 0;
	reg [24:0] counter2 = 0;
	reg[7:0] count = 0;
	wire [255:0] data;

	ram_1port_256bits (
	  address,
	  clk,
	  matriz_C,
	  write_enable,
	  data
	);

   
	matriz_soma (
	  matriz_A,
	  matriz_B,
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
        case (base)
            0: begin
                case (count)
                    0: begin
                        count <= 1;
                        // Primeiro ciclo ignora
                    end
                    1: begin
                        address <= base + complement;
                        count <= 2;
                    end
                    2: begin
                        matrizA[complement] <= data;
                        if (complement == 13) begin
                            complement <= 0;
                            base <= 1;
                        end else begin
                            complement <= complement + 1;
                            count <= 1;
                        end
                    end
                endcase
            end
            1: begin
                case (count)
                    0: begin
                        address <= base + complement;
                        count <= 1;
                    end
                    1: begin
                        matrizA[complement] <= data;
                        if (complement == 13) begin
                            complement <= 0;
                            base <= 2;
                        end else begin
                            complement <= complement + 1;
                            count <= 0;
                        end
                    end
                endcase
            end
            2: begin
                case (count)
                    0: begin
                        address <= base + complement;
                        count <= 1;
                    end
                    1: begin
                        matrizA[complement] <= data;
                        if (complement == 13) begin
                            complement <= 0;
                            base <= 3;
                        end else begin
                            complement <= complement + 1;
                            count <= 0;
                        end
                    end
                endcase
            end
            3: begin
                if (!rst) begin
                    base <= 0;
                    complement <= 0;
                    count <= 0;
                    address <= 0;
                end
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
