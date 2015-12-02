`include "intf.vh"

module fred
  (
  bus.sink b
  );

reg [3:0] thereg;

assign b.rd_data = b.re ? thereg : 4'hz;

always @(posedge b.clk or negedge b.reset_l)
  if (!b.reset_l)
    thereg <= 0;
  else if (b.we)
    thereg <= b.wr_data;

endmodule
