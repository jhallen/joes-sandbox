// An interrupt source
// Use with bus_irq_reg.v

// When a pulse on trig is detected, out is set high.

// When async input irq_rearm flips, out is flipped.  Allows sw to
// generate a fake interrupt.

module irq_source
  (
  clk,
  reset_l,
  trig,
  irq_rearm,
  out
  );

parameter DATAWIDTH=1;

input clk;
input reset_l;
input [DATAWIDTH-1:0] trig;
input [DATAWIDTH-1:0] irq_rearm;

output [DATAWIDTH-1:0] out;
reg [DATAWIDTH-1:0] out;

wire [DATAWIDTH-1:0] rearm_pulse;

edge_det #(.DATAWIDTH(DATAWIDTH)) edge_det
  (
  .reset_l (reset_l),
  .clk (clk),
  .i (irq_rearm),
  .o (rearm_pulse)
  );

always @(posedge clk or negedge reset_l)
  if(!reset_l)
    out <= 0;
  else
    out <= (trig | (out ^ rearm_pulse));

endmodule
