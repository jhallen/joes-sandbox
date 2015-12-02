// Array of pulse stretchers.

module stretch
  (
  reset_l,
  clk,
  i,
  o
  );

parameter DATAWIDTH=1;
parameter N=6;	// Output pulse width in clock cycles

input reset_l;
input clk;

input [DATAWIDTH-1:0] i;
output [DATAWIDTH-1:0] o;
reg [DATAWIDTH-1:0] o;

integer x;

reg [2:0] counters[DATAWIDTH-1:0];

always @(posedge clk or negedge reset_l)
  if(!reset_l)
    begin
      o <= 0;
      for(x = 0; x != DATAWIDTH; x = x + 1) counters[x] <= 0;
    end
  else
    begin
      o <= o | i;
      for(x = 0; x != DATAWIDTH; x = x + 1)
        if(i[x])
          counters[x] <= N-1;
        else if(counters[x])
          counters[x] <= counters[x] - 1;
        else
          o[x] <= 0;
    end

endmodule
