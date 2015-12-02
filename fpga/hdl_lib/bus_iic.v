// I^2C register (for bit-bang I2C device driver)

module iic
  (
  bus_in,
  bus_out,

  scl_pin,
  sda_pin
  );

parameter ADDR = 0;

`include "bus_params.v"

input [BUS_IN_WIDTH-1:0] bus_in;
output [BUS_OUT_WIDTH-1:0] bus_out;

inout scl_pin;
inout sda_pin;

`include "bus_decl.v"

// Drivers
 
wire scl_drive_l;

assign scl_pin = scl_drive_l ? 1'bz : 1'b0;

wire sda_drive_l;

assign sda_pin = sda_drive_l ? 1'bz : 1'b0;

// Input registers

reg scl;
reg sda;

always @(posedge bus_clk or negedge reset_l)
  if(!reset_l)
    begin
      scl <= 1;
      sda <= 1;
    end
  else
    begin
      scl <= scl_pin;
      sda <= sda_pin;
    end

wire [BUS_OUT_WIDTH-1:0] iic_reg_out;
assign bus_out = iic_reg_out;

bus_reg #(.ADDR(ADDR), .DATAWIDTH(2), .IZ(3)) iic_reg
  (
  .bus_in (bus_in), .bus_out (iic_reg_out),

  .in ({ scl, sda }),
  .out ({ scl_drive_l, sda_drive_l })
  );

endmodule
