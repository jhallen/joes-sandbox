// A simple 32-bit clear on read counter readable by software
// Counter can be in different clock domain

module bus_counter
  (
  // Internal bus
  bus_in,
  bus_out,

  // Counter
  clk,
  reset_l,
  en,
  count_val
  );

parameter ADDR = 0;	// Address of this register

`include "bus_params.v"

input [BUS_IN_WIDTH-1:0] bus_in;
output [BUS_OUT_WIDTH-1:0] bus_out;

input clk;
input reset_l;
input en;
output [31:0] count_val;

`include "bus_decl.v"

// Raw counter

reg [16:0] count_lo;
reg [16:0] count_lo_d;
reg [15:0] count_hi;

assign count_val = { count_hi, count_lo_d[15:0] };

reg [31:0] xfer;
wire req;
wire req_synced;
reg ack;

sync req_syncer
  (
  .reset_l (reset_l),
  .clk (clk),
  .i (req),
  .o (req_synced)
  );

reg clear_high;

always @(posedge clk or negedge reset_l)
  if(!reset_l)
    begin
      count_lo <= 0;
      count_lo_d <= 0;
      count_hi <= 0;
      clear_high <= 0;
      ack <= 0;
      xfer <= 0;
    end
  else
    begin
      clear_high <= 0;

      count_lo_d <= count_lo;

      // Carry

      if(clear_high)
        count_hi <= 0;
      else if(count_lo_d[16]!=count_lo[16])
        count_hi <= count_hi + 1'd1;

      // Count, count and clear, or clear

      if(en)
        if(req_synced!=ack)
          begin // Count and clear
            xfer <= count_val;
            count_lo <= 1;
            clear_high <= 1;
            ack <= !ack;
          end
        else
          begin // Count
            count_lo <= count_lo + 1'd1;
          end
      else if(req_synced!=ack)
        begin // Clear
          xfer <= count_val;
          count_lo <= 0;
          clear_high <= 1;
          ack <= !ack;
        end
    end

// Interface to bus

wire [BUS_OUT_WIDTH-1:0] bus_reg_out;
assign bus_out = bus_reg_out;

bus_reqack #(.ADDR(ADDR), .ADDRWIDTH(4)) bus_reg
  (
  .bus_in (bus_in), .bus_out (bus_reg_out),

  .rd_data (xfer),
  .req (req),
  .ack (ack)
  );

endmodule
