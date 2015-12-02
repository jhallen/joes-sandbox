// Interrupt bridge
//
// This is two registers:
//    ADDR+0: bridge status register
//    ADDR+4: bridge enable regsiter

module bus_irq_bridge
  (
  bus_in,
  bus_out,

  irqs_in_l,
  irq_out_l
  );

parameter ADDR = 0;
parameter DATAWIDTH = 1;

`include "bus_params.v"

input [BUS_IN_WIDTH-1:0] bus_in;
output [BUS_OUT_WIDTH-1:0] bus_out;

input [DATAWIDTH-1:0] irqs_in_l;

output irq_out_l;
reg irq_out_l;

`include "bus_decl.v"

// Bridge registers

wire [DATAWIDTH-1:0] bridge_enables;
wire [DATAWIDTH-1:0] irqs;

sync #(.DATAWIDTH(DATAWIDTH)) irq_syncer
  (
  .reset_l (bus_reset_l),
  .clk (bus_clk),
  .i (~irqs_in_l),
  .o (irqs)
  );

always @(posedge bus_clk or negedge bus_reset_l)
  if(!bus_reset_l)
    irq_out_l <= 1;
  else
    irq_out_l <= !(|(irqs & bridge_enables));

// Bridge IRQ Status

wire [BUS_OUT_WIDTH-1:0] status_reg_out;
assign bus_out = status_reg_out;

bus_reg #(.ADDR(ADDR), .DATAWIDTH(DATAWIDTH)) irq_bridge_status_reg
  (
  .bus_in (bus_in), .bus_out (status_reg_out),

  .in (irqs) /* ATTR irqs_in_l xxx .*_l &0_pending */
  );

// Bridge IRQ Enable

wire [BUS_OUT_WIDTH-1:0] enable_reg_out;
assign bus_out = enable_reg_out;

bus_reg #(.ADDR(ADDR+4), .DATAWIDTH(DATAWIDTH)) irq_bridge_enable_reg
  (
  .bus_in (bus_in), .bus_out (enable_reg_out),

  .in (bridge_enables) /* ATTR irqs_in_l irqs_in_l .*_l &0_en */,
  .out (bridge_enables)
  );

endmodule
