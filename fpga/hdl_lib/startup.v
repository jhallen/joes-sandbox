// Clean startup step

module startup
  (
  input clk,
  input reset_l,
  input wait_l,
  output startup
  );

reg [3:0] dly;

assign startup = dly[3];

always @(posedge clk or negedge reset_l or negedge wait_l)
  if(!reset_l)
    dly <= 0;
  else if(!wait_l)
    dly <= 0;
  else
    dly <= (dly << 1) | 1;

endmodule
