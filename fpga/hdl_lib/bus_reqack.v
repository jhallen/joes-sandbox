// Asynchronous bus interface using req/ack sequence.
//
// 'req' flips to indicate a request.  A synchronizer plus edge detector
// can be used to convert this into a request pulse in another clock
// domain (slower or faster).  All other signals (req_addr, wr_data,
// rd_wr_l) are stable after 'req' flips, so no synchronizers are needed for
// them.
//
// When transaction is complete, flip 'ack' (and keep rd_data stable from
// then on- no synchronizer necessary).  There is an edge detector in here
// to generate the bus_ack pulse in the bus clock domain.
//
// Also useful for lazy response to a request in same or different clock
// domain.  Declare a 'reg ack;' connected to this module's ack input.  When
// req!=ack, you have a pending request.  Whenever you're done responding to
// it, flip ack.

module bus_reqack
  (
  // Internal bus
  bus_in,
  bus_out,

  // Pins to other clock domain (typically a ram)
  req_addr,		// Address out (remember: lower two bits always 0)
  wr_data,		// Write data out
  rd_data,		// Read data in
  req,			// Flips for transaction request
  rd_wr_l,		// Direction
  ack			// Flip to indicate transaction complete (has synchronizer)
                        // Ok to connect req to ack.
  );

parameter ADDR = 0; // Address
parameter DATAWIDTH = 32; // No. bits (1..32)
parameter ADDRWIDTH = 20; // No. address bits (size will be (1<<ADDRWIDTH) bytes
parameter OFFSET = 0; // Bit position

`include "bus_params.v"

input [BUS_IN_WIDTH-1:0] bus_in;
output [BUS_OUT_WIDTH-1:0] bus_out;

output [ADDRWIDTH-1:0] req_addr;
reg [ADDRWIDTH-1:0] req_addr;

output [DATAWIDTH-1:0] wr_data;
reg [DATAWIDTH-1:0] wr_data;

input [DATAWIDTH-1:0] rd_data;

output req;
reg req;

output rd_wr_l;
reg rd_wr_l;

input ack;

`include "bus_decl.v"

// Bus driver

reg bus_rtn;
reg [DATAWIDTH-1:0] rtn_data;

assign bus_out[BUS_FIELD_ACK] = bus_rtn;

assign bus_out[BUS_DATA_END-1:BUS_DATA_START] = bus_rtn ? ( rtn_data << OFFSET ) : { BUS_DATA_WIDTH { 1'd0 } };

wire ack_synced;
reg ack_old;

// Synchronizers

sync ack_syncer
  (
  .reset_l (bus_reset_l),
  .clk (bus_clk),
  .i (ack),
  .o (ack_synced)
  );

always @(posedge bus_clk or negedge bus_reset_l)
  if(!bus_reset_l)
    begin
      bus_rtn <= 0;
      req <= 0;
      rd_wr_l <= 0;
      req_addr <= 0;
      rtn_data <= 0;
      wr_data <= 0;
      ack_old <= 0;
    end
  else
    begin
      bus_rtn <= 0;

      if (bus_req && (bus_addr & ~((1 << ADDRWIDTH) - 1)) == (ADDR & ~((1 << ADDRWIDTH) - 1)))
        begin
          req_addr <= bus_addr;
          req <= !req;
          wr_data <= bus_wr_data;
          rd_wr_l <= bus_rd_wr_l;
        end

      if (ack_synced != ack_old)
        begin
          ack_old <= ack_synced;
          rtn_data <= rd_data;
          bus_rtn <= 1;
        end
    end

endmodule
