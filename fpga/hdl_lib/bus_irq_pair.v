// A single irq status/enable bit pair
//
// This is two registers:
//   ADDR+0: interrupt status register
//   ADDR+4: interrupt enable register
//
// Interrupt input signal names should have _trig postfix for rattr to work
// properly.

module bus_irq_pair
  (
  bus_in,
  bus_out,

  // Trigger interrupt (any clock domain).

  clk,	// Clock for interrupt triggers
  reset_l,
  trig,	// Interrupt trigger pulses
  out	// Interrupt status (in 'clk' domain).
  );

parameter ADDR = 0; // Address
parameter OFFSET = 0; // Starting bit
parameter DATAWIDTH = 1; // No. bits

`include "bus_params.v"

input [BUS_IN_WIDTH-1:0] bus_in;
output [BUS_OUT_WIDTH-1:0] bus_out;

input clk;
input reset_l;
input [DATAWIDTH-1:0] trig;
output [DATAWIDTH-1:0] out;

`include "bus_decl.v"

// IRQ bit

wire [DATAWIDTH-1:0] irq_pending;	// Status
wire [DATAWIDTH-1:0] irq_rearm;		// Rearm source

wire [BUS_OUT_WIDTH-1:0] status_reg_out;
assign bus_out = status_reg_out;

bus_irq_reg #(.ADDR(ADDR), .OFFSET(OFFSET), .DATAWIDTH(DATAWIDTH)) irq_status_reg
  (
  .bus_in (bus_in), .bus_out (status_reg_out),

  .in (out), /* ATTR trig trig .*_trig &0_pending */
  .out (irq_pending),
  .irq_rearm (irq_rearm)
  );

// Enable bit

wire [DATAWIDTH-1:0] enable;		// Enable

wire [BUS_OUT_WIDTH-1:0] enable_reg_out;
assign bus_out = enable_reg_out;

bus_reg #(.ADDR(ADDR+4), .DATAWIDTH(DATAWIDTH), .OFFSET(OFFSET)) irq_enable_reg
  (
  .bus_in (bus_in), .bus_out (enable_reg_out),

  .in (enable) /* ATTR trig trig .*_trig &0_en */,
  .out (enable)
  );

// Interrupt source

irq_source #(.DATAWIDTH(DATAWIDTH)) irq_source
  (
  .clk (clk),
  .reset_l (reset_l),
  .trig (trig),
  .irq_rearm (irq_rearm),
  .out (out)
  );

assign bus_out[BUS_FIELD_IRQ] = |(enable & irq_pending);

endmodule
