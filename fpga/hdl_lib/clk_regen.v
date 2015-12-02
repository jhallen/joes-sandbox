// Re-create clock as logic instead of clock net

module clk_regen
  (
  reset_l,
  clk,
  clk_regen
  );

input reset_l;
input clk;
output clk_regen;

reg i;
reg q;

always @(posedge clk or negedge reset_l)
  if (!reset_l)
    i <= 0;
  else
    i <= ~i;

always @(negedge clk or negedge reset_l)
  if (!reset_l)
    q <= 0;
  else
    q <= i;

wire clk_regen = i ^ q;

endmodule
