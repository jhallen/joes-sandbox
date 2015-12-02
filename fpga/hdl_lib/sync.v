// Synchronizer or tracking pipeline

module sync
  (
  reset_l,
  clk,
  i,
  o
  );

parameter DATAWIDTH=1;
parameter NSTAGES=2;

input reset_l;
input clk;
input [DATAWIDTH-1:0] i;
output [DATAWIDTH-1:0] o;
wire [DATAWIDTH-1:0] o;

reg [DATAWIDTH-1:0] resync[NSTAGES-1:0];

integer x;

always @(posedge clk or negedge reset_l)
  if(!reset_l)
    for(x=0;x!=NSTAGES;x=x+1)
      resync[x] <= 0;
  else
    begin
      for(x=1;x!=NSTAGES;x=x+1)
        resync[x] <= resync[x-1];
      resync[0] <= i;
    end

assign o = resync[NSTAGES-1];

endmodule
