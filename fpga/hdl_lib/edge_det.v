// Generate a pulse for an async rising or falling edge detected on input

module edge_det
  (
  reset_l,
  clk,
  i,
  o
  );

parameter DATAWIDTH=1;

input reset_l;
input clk;
input [DATAWIDTH-1:0] i;
output [DATAWIDTH-1:0] o;
wire [DATAWIDTH-1:0] o;

wire [DATAWIDTH-1:0] synced;

sync #(.DATAWIDTH(DATAWIDTH)) syncer
  (
  .reset_l (reset_l),
  .clk (clk),
  .i (i),
  .o (synced)
  );

reg [DATAWIDTH-1:0] old;

always @(posedge clk or negedge reset_l)
  if(!reset_l)
    old <= 0;
  else
    old <= synced;

assign o = old ^ synced;

endmodule
