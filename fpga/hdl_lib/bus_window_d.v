// Addressing window with one pipeline delay

// Create a new sub-bus which is focused on a particular address range.
// Address bits not needed for the range are set to constants which reduces
// the size of address decoders on the sub-bus.

// Registers on the sub-bus should use the full register address, not an
// offset relative to the window's address.

module bus_window_d
  (
  bus_in,
  bus_out,

  sub_bus_in,
  sub_bus_out
  );

`include "bus_params.v"

parameter ADDR = 0;		// Base address
parameter ADDRWIDTH = 8;	// Size (1<<ADDRWIDTH) bytes

input [BUS_IN_WIDTH-1:0] bus_in;

output [BUS_OUT_WIDTH-1:0] bus_out;

output [BUS_IN_WIDTH-1:0] sub_bus_in;

input [BUS_OUT_WIDTH-1:0] sub_bus_out;

`include "bus_decl.v"

// Outputs

reg [BUS_OUT_WIDTH-1:0] sub_bus_out_d;

assign bus_out = sub_bus_out_d;

always @(posedge bus_clk or negedge bus_reset_l)
  if(!bus_reset_l)
    sub_bus_out_d <= 0;
  else
    sub_bus_out_d <= sub_bus_out;

// Inputs

assign sub_bus_in[BUS_FIELD_RESET_L] = bus_reset_l;
assign sub_bus_in[BUS_FIELD_CLK] = bus_clk;
assign sub_bus_in[BUS_FIELD_STARTUP] = bus_startup;

wire decode = (bus_addr & ((1 << ADDRWIDTH) - 1)) == (ADDR & ((1 << ADDRWIDTH) - 1));

reg bus_req_d;
reg bus_rd_wr_l_d;
reg [BUS_DATA_WIDTH-1:0] bus_wr_data_d;
reg [BUS_ADDR_WIDTH-1:0] bus_addr_d;

always @(posedge bus_clk or negedge bus_reset_l)
  if(!bus_reset_l)
    begin
      bus_req_d <= 0;
      bus_rd_wr_l_d <= 0;
      bus_wr_data_d <= 0;
      bus_addr_d <= 0;
    end
  else
    begin
      bus_req_d <= bus_req && decode;
      bus_rd_wr_l_d <= bus_rd_wr_l;
      bus_wr_data_d <= bus_wr_data;
      bus_addr_d <= (ADDR & ~((1 << ADDRWIDTH) - 1)) | (bus_addr & ((1 << ADDRWIDTH) - 1));
    end

assign sub_bus_in[BUS_FIELD_REQ] = bus_req_d;
assign sub_bus_in[BUS_FIELD_RD_WR_L] = bus_rd_wr_l_d;
assign sub_bus_in[BUS_DATA_END-1:BUS_DATA_START] = bus_wr_data_d;
assign sub_bus_in[BUS_ADDR_END-1:BUS_ADDR_START] = bus_addr_d;

endmodule
