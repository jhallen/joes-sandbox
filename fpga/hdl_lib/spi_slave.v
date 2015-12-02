// Simple SPI slave interface
//   - Generates internal chip bus

module spi_slave
  (
  reset_l,
  clk,
  spi_din,
  spi_dout,

  bus_addr,
  bus_wr_data,
  bus_rd_data,
  bus_we,
  bus_re
  );

input reset_l;
input clk;
input spi_din;
output spi_dout;

output [6:0] bus_addr; // Least significant 2 bits are always 0!
reg [6:0] bus_addr;

output [7:0] bus_wr_data;
reg [7:0] bus_wr_data;

input [7:0] bus_rd_data;

output bus_we;
reg bus_we;

output bus_re;
reg bus_re;

reg [8:0] spi_shift_reg; // Bit 8 used just for timing

reg [7:0] spi_out_reg;
assign spi_dout = spi_out_reg[7];

parameter
  IDLE = 0,
  MAYBE_READ = 1,
  READ = 2,
  WRITE = 3;

reg [1:0] state;

always @(posedge clk or negedge reset_l)
  if (!reset_l)
    begin
      spi_shift_reg <= 0;
      spi_out_reg <= 0;
      state <= IDLE;
      bus_addr <= 0;
      bus_wr_data <= 0;
      bus_re <= 0;
      bus_we <= 0;
    end
  else
    begin
      bus_re <= 0;
      bus_we <= 0;
      spi_shift_reg <= { spi_shift_reg[7:0], spi_din };
      spi_out_reg <= { spi_out_reg[6:0], 1'd0 };
      case (state)
        IDLE:
          if (spi_shift_reg[5]) // Start bit...
            begin
              bus_addr <= { spi_shift_reg[4:0], 2'd0 };
              // Assume it's a read...
              state <= MAYBE_READ;
              bus_re <= 1;
            end

        MAYBE_READ:
          if (spi_shift_reg[0])
            begin
              // Oops, it's a write
              spi_shift_reg <= 1'd1;
              state <= WRITE;
            end
          else
            // It really is a read...
            state <= READ;

        WRITE:
          if (spi_shift_reg[8]) // Wait for all 8 data bits...
            begin
              bus_we <= 1;
              bus_wr_data <= spi_shift_reg[7:0];
              spi_shift_reg <= { 8'd0, spi_din }; // Clear it so we don't think a data bit is a start bit
              state <= IDLE;
            end

        READ:
          begin
            spi_out_reg <= bus_rd_data;
            spi_shift_reg <= { 8'd0, spi_din };
            state <= IDLE;
          end
          
      endcase
    end

endmodule
