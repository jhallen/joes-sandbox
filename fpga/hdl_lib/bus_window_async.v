// Addressing window which crosses clock domains

// Create a new sub-bus which is focused on a particular address range.
// Address bits not needed for the range are set to constants which reduces
// the size of address decoders on the sub-bus.

// Registers on the sub-bus should use the full register address, not an
// offset relative to the window's address.

// optimize:
//  4 FFs for reqack.
//  2 FFs for irq sync.
//  3 FFs for req edge det.
// 32 for sub_rd_data.

module bus_window_async
  (
  sub_clk,		// Clock for sub bus
  sub_startup,		// Startup for sub bus
  sub_reset_l,		// Reset for sub bus
  
  bus_in,
  bus_out,

  sub_bus_in,
  sub_bus_out
  );

`include "bus_params.v"

parameter ADDR = 0;		// Base address
parameter ADDRWIDTH = 8;	// Size (1<<ADDRWIDTH) bytes

input sub_clk;
input sub_startup;
input sub_reset_l;

input [BUS_IN_WIDTH-1:0] bus_in;
output [BUS_OUT_WIDTH-1:0] bus_out;

output [BUS_IN_WIDTH-1:0] sub_bus_in;
input [BUS_OUT_WIDTH-1:0] sub_bus_out;

`include "bus_decl.v"

// The sub bus

wire [ADDRWIDTH-1:0] req_addr;

wire sub_req;
wire sub_rd_wr_l;
wire [BUS_DATA_WIDTH-1:0] sub_wr_data;
wire [BUS_ADDR_WIDTH-1:0] sub_addr = (ADDR & ~((32'd1 << ADDRWIDTH) - 1)) | { { (32 - ADDRWIDTH) { 1'b0 } }, req_addr };
wire sub_irq;
reg [BUS_DATA_WIDTH-1:0] sub_rd_data;

assign bus_out[BUS_FIELD_IRQ] = sub_irq;

assign sub_bus_in[BUS_FIELD_RESET_L] = sub_reset_l;
assign sub_bus_in[BUS_FIELD_CLK] = sub_clk;
assign sub_bus_in[BUS_FIELD_STARTUP] = sub_startup;
assign sub_bus_in[BUS_FIELD_REQ] = sub_req;
assign sub_bus_in[BUS_FIELD_RD_WR_L] = sub_rd_wr_l;
assign sub_bus_in[BUS_DATA_END-1:BUS_DATA_START] = sub_wr_data;
assign sub_bus_in[BUS_ADDR_END-1:BUS_ADDR_START] = sub_addr;

// Main side of bus

sync sub_irq_syncer
  (
  .reset_l (bus_reset_l),
  .clk (bus_clk),
  .i (sub_bus_out[BUS_FIELD_IRQ]),
  .o (sub_irq)
  );

wire [BUS_DATA_WIDTH-1:0] rd_data;

wire req;
reg ack;

wire [BUS_OUT_WIDTH-1:0] reqack_out;
assign bus_out = reqack_out;

bus_reqack #(.ADDR(ADDR), .DATAWIDTH(BUS_DATA_WIDTH), .ADDRWIDTH(ADDRWIDTH)) reqack
  (
  .bus_in (bus_in), .bus_out (reqack_out),

  .req_addr (req_addr),
  .wr_data (sub_wr_data),
  .rd_data (sub_rd_data),
  .rd_wr_l (sub_rd_wr_l),
  .req (req),
  .ack (ack)
  );

// Sub bus side

edge_det req_det
  (
  .reset_l (sub_reset_l),
  .clk (sub_clk),
  .i (req),
  .o (sub_req)
  );

always @(posedge sub_clk or negedge sub_reset_l)
  if(!sub_reset_l)
    begin
      sub_rd_data <= 0;
      ack <= 0;
    end
  else
    if(sub_bus_out[BUS_FIELD_ACK])
      begin
        sub_rd_data <= sub_bus_out[BUS_DATA_END-1:BUS_DATA_START];
        ack <= !ack;
      end

endmodule
