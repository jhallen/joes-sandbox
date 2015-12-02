// Simple control register (output only register) with pad flip flops.
// Sense of outputs may be inverted with respect to programmable register

module bus_control_reg
  (
  bus_in,
  bus_out,
  out
  );

parameter ADDR=0;
parameter IZ=0;
parameter DATAWIDTH=1;
parameter INV=0;
parameter REG=0;

`include "bus_params.v"

input [BUS_IN_WIDTH-1:0] bus_in;
output [BUS_OUT_WIDTH-1:0] bus_out;

output [DATAWIDTH-1:0] out;
reg [DATAWIDTH-1:0] out;

`include "bus_decl.v"

wire [DATAWIDTH-1:0] q;

wire [BUS_OUT_WIDTH-1:0] bus_reg_out;
assign bus_out = bus_reg_out;

bus_reg #(.ADDR(ADDR), .IZ(IZ), .DATAWIDTH(DATAWIDTH)) bus_reg
  (
  .bus_in (bus_in), .bus_out (bus_reg_out),

  .in (q),
  .out (q)
  );

always @(posedge bus_clk or negedge bus_reset_l)
  if(!bus_reset_l)
    out <= INV ^ IZ;
  else
    out <= INV ^ q;

endmodule
