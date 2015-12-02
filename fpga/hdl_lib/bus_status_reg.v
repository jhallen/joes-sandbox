// A generic software "status register".
//
// This is actually three registers:
//   ADDR + 0: Status register with input synchronizers.
//   ADDR + 4: IRQ register
//   ADDR + 8: IRQ enable register
//
// Each input bit has a corresponding interrupt bit.  The interrupt bit is
// set whenever the input changes.


module bus_status_reg
  (
  bus_in,
  bus_out,
  in,
  irq_l
  );

parameter DATAWIDTH=32;
parameter ADDR=0;
parameter INV=0;
parameter IZ=0;
parameter OFFSET=0;

`include "bus_params.v"

input [BUS_IN_WIDTH-1:0] bus_in;

output [BUS_OUT_WIDTH-1:0] bus_out;

input [DATAWIDTH-1:0] in;

output irq_l;

`include "bus_decl.v"

// Input register

reg [DATAWIDTH-1:0] inreg_1;
reg [DATAWIDTH-1:0] inreg_2;
reg [DATAWIDTH-1:0] inreg_3;

always @(posedge bus_clk or negedge bus_reset_l)
  if(!bus_reset_l)
    begin
      inreg_1 <= IZ;
      inreg_2 <= IZ;
      inreg_3 <= IZ;
    end
  else
    begin
      inreg_1 <= inreg_2;
      inreg_2 <= inreg_3;
      inreg_3 <= (in^INV);
    end

// Status register

wire [BUS_OUT_WIDTH-1:0] status_out;
assign bus_out = status_out;

bus_reg #(.ADDR(ADDR), .DATAWIDTH(DATAWIDTH), .IZ(IZ), .OFFSET(OFFSET)) status
  (
  .bus_in (bus_in), .bus_out (status_out),

  .in (inreg_2) /* ATTR in xxx */
  );

// IRQ bit

wire [DATAWIDTH-1:0] irq_pending;	// Status
wire [DATAWIDTH-1:0] irq_rearm;		// Rearm source
wire [DATAWIDTH-1:0] out;

wire [BUS_OUT_WIDTH-1:0] status_reg_out;
assign bus_out = status_reg_out;

bus_irq_reg #(.ADDR(ADDR+4), .OFFSET(OFFSET), .DATAWIDTH(DATAWIDTH)) irq_status_reg
  (
  .bus_in (bus_in), .bus_out (status_reg_out),

  .in (out), /* ATTR in in .* &__pending */
  .out (irq_pending),
  .irq_rearm (irq_rearm)
  );

// Enable bit

wire [DATAWIDTH-1:0] enable;		// Enable

wire [BUS_OUT_WIDTH-1:0] enable_reg_out;
assign bus_out = enable_reg_out;

bus_reg #(.ADDR(ADDR+8), .DATAWIDTH(DATAWIDTH), .OFFSET(OFFSET)) irq_enable_reg
  (
  .bus_in (bus_in), .bus_out (enable_reg_out),

  .in (enable), /* ATTR in in .* &__en */
  .out (enable)
  );

// Interrupt source

irq_source #(.DATAWIDTH(DATAWIDTH)) irq_source
  (
  .clk (bus_clk),
  .reset_l (bus_reset_l),
  .trig (inreg_3^inreg_2),
  .irq_rearm (irq_rearm),
  .out (out)
  );

assign irq_l = ~|(enable & irq_pending);

endmodule
