// bit-banged JTAG controller port

module bus_jtag
  (
  bus_in,
  bus_out,

  tms_out,
  tck_out,
  tdo_out,
  tdi_in
  );

parameter ADDR = 0;

`include "bus_params.v"

input [BUS_IN_WIDTH-1:0] bus_in;
output [BUS_OUT_WIDTH-1:0] bus_out;

output tms_out;
reg tms_out;

output tck_out;
reg tck_out;

output tdo_out;
reg tdo_out;

input tdi_in;

`include "bus_decl.v"

wire tdi;

sync tdi_syncer
  (
  .reset_l (bus_reset_l),
  .clk (bus_clk),
  .i (tdi_in),
  .o (tdi)
  );

wire tdo;
wire tms;
wire tck;

always @(posedge bus_clk or negedge bus_reset_l)
  if(!bus_reset_l)
    begin
      tms_out <= 0;
      tck_out <= 0;
      tdo_out <= 0;
    end
  else
    begin
      tms_out <= tms;
      tck_out <= tck;
      tdo_out <= tdo;
    end

wire dummy;

wire [BUS_OUT_WIDTH-1:0] jtag_reg_out;
assign bus_out = jtag_reg_out;

bus_reg #(.ADDR(ADDR), .DATAWIDTH(4)) jtag_reg
  (
  .bus_in (bus_in), .bus_out (jtag_reg_out),

  .in ({ tdi, tdo, tms, tck }),
  .out ({ dummy, tdo, tms, tck })
  );

endmodule
