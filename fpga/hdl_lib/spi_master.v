// SPI-bus master interface
//  Generates external spi bus
//  Talks to another chip with spi_slave.v interface

module spi_master
  (
  reset_l,
  clk,

  bus_addr,
  bus_wr_data,
  bus_rd_data,
  bus_ack,
  bus_re,
  bus_we,
  leds_in,

  spi_reset_l,
  spi_clk, // spi_clk is 1/4 clk
  spi_dout,
  spi_din,
  led_dout
  );

input reset_l;
input clk;

input [6:0] bus_addr; // Least significant two bits always 0

input [7:0] bus_wr_data;

output [7:0] bus_rd_data;
reg [7:0] bus_rd_data;

output bus_ack;
reg bus_ack;

input bus_re;

input bus_we;

output spi_reset_l;
reg spi_reset_l;

output spi_clk;
reg spi_clk;

reg [1:0] spi_clk_cnt;

output spi_dout;

input spi_din;

input [9:0] leds_in;
output led_dout;

reg [7:0] spi_shift_reg;

reg [7:0] spi_out_reg;
assign spi_dout = spi_out_reg[7];

reg [1:0] state;
reg [3:0] count;

parameter
  IDLE = 0,
  WRITE = 1,
  READ = 2,
  WAIT_DONE = 3;

reg bus_we_d;
reg bus_we_d1;
reg bus_we_d2;

reg bus_re_d;
reg bus_re_d1;
reg bus_re_d2;

reg [6:0] led_count;
reg [10:0] led_shift;

assign led_dout = led_shift[10];

reg spi_reset_l_1;
reg spi_reset_l_2;

always @(posedge clk or negedge reset_l)
  if (!reset_l)
    begin
      bus_rd_data <= 0;
      bus_ack <= 0;
      spi_clk <= 0;
      spi_clk_cnt <= 0;
      spi_shift_reg <= 0;
      spi_out_reg <= 0;
      count <= 0;
      state <= IDLE;
      led_count <= 0;
      led_shift <= 0;
      spi_reset_l <= 0;
      spi_reset_l_1 <= 0;
      spi_reset_l_2 <= 0;
      bus_re_d <= 0;
      bus_re_d1 <= 0;
      bus_re_d2 <= 0;
      bus_we_d <= 0;
      bus_we_d1 <= 0;
      bus_we_d2 <= 0;
    end
  else
    begin
      spi_clk <= spi_clk_cnt[1];
      spi_clk_cnt <= spi_clk_cnt + 1'd1;
      spi_reset_l <= spi_reset_l_1;
      spi_reset_l_1 <= spi_reset_l_2;
      spi_reset_l_2 <= 1;
      // Launch data one cycle earlier than rising edge
      if (spi_clk_cnt == 1)
        begin
          led_count <= led_count + 1'd1;
          led_shift <= { led_shift[9:0], 1'd0 };
          if (led_count == 5)
            led_shift <= { 1'd1, leds_in };
        end

      bus_we_d <= bus_we;
      bus_we_d1 <= bus_we_d;
      bus_we_d2 <= bus_we_d1;
      bus_re_d <= bus_re;
      bus_re_d1 <= bus_re_d;
      bus_re_d2 <= bus_re_d1;
      bus_ack <= 0;
      bus_rd_data <= 0;
      if (spi_clk_cnt == 1) // Launch on falling edge
        begin
          count <= count - 4'h1;
          spi_shift_reg <= { spi_shift_reg[6:0], spi_din };
          spi_out_reg <= { spi_out_reg[6:0], 1'd0 };
          case (state)
            IDLE:
              if (bus_we || bus_we_d || bus_we_d1 || bus_we_d2)
                begin
                  spi_out_reg <= { 1'd1, bus_addr[6:2], 1'd1, 1'd0 };
                  count <= 7;
                  state <= WRITE;
                end
              else if  (bus_re || bus_re_d || bus_re_d1 || bus_re_d2)
                begin
                  spi_out_reg <= { 1'd1, bus_addr[6:2], 1'd0, 1'd0 };
                  count <= 7;
                  state <= READ;
                end

            WRITE:
              if (!count)
                begin
                  count <= 7;
                  spi_out_reg <= bus_wr_data;
                  state <= WAIT_DONE;
                end

            READ:
              if (!count)
                begin
                  count <= 8;
                  state <= WAIT_DONE;
                end

            WAIT_DONE:
              if (!count)
                begin
                  bus_ack <= 1;
                  bus_rd_data <= spi_shift_reg[7:0];
                  state <= IDLE;
                end
          endcase
        end
    end

endmodule
