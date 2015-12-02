// Generic byte-wide SPI interface
// SPI clock is 1/4 of clk

// Used to talk to SPI flash chips

module spi
  (
  clk,
  reset_l,

  spi_clk,
  spi_dout,
  spi_din,

  data_in,
  data_out,
  start,
  busy
  );

input clk;
input reset_l;

output spi_clk;
output spi_dout;
input spi_din;

input [7:0] data_in;
output [7:0] data_out;
input start;
output busy;

reg spi_clk;
reg [4:0] count;
reg spi_din_1;
reg spi_din_2;
reg [7:0] spi_shift_reg;
reg busy;

wire spi_dout = spi_shift_reg[7];
wire [7:0] data_out = spi_shift_reg;

always @(posedge clk or negedge reset_l)
  if (!reset_l)
    begin
      spi_clk <= 0;
      count <= 0;
      busy <= 0;
      spi_shift_reg <= 0;
      spi_din_1 <= 0;
      spi_din_2 <= 0;
    end
  else
    begin
      spi_din_1 <= spi_din;
      spi_din_2 <= spi_din_1;
      if (busy)
        begin
          count <= count + 1'd1;
          if (count[1:0] == 3)
            spi_shift_reg <= { spi_shift_reg[6:0], spi_din_2 };
          if (count[1:0] == 1)
            spi_clk <= 1;
          else if (count[1:0] == 3)
            spi_clk <= 0;
          if (count == 31)
            // We're done.
            busy <= 0;
        end
      else if (start)
        begin
          busy <= 1;
          spi_shift_reg <= data_in;
        end
    end

endmodule
