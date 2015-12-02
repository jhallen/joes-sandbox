// Raw interrupt register
// Use with irq_source.v

module bus_irq_reg
  (
  // Internal bus
  bus_in,
  bus_out,

  in,		// Interrupt status in (async)
  out,		// Current raw interrupt status (1 means interrupt pending)
  irq_rearm	// Flips to rearm interrupt sources
  );

parameter ADDR=0;
parameter OFFSET=0;
parameter DATAWIDTH=1;
parameter REG=0;

`include "bus_params.v"

input [BUS_IN_WIDTH-1:0] bus_in;
output [BUS_OUT_WIDTH-1:0] bus_out;

input [DATAWIDTH-1:0] in;

output [DATAWIDTH-1:0] out;

output [DATAWIDTH-1:0] irq_rearm;
reg [DATAWIDTH-1:0] irq_rearm;

`include "bus_decl.v"

// Bus driver

reg bus_rtn;

assign bus_out[BUS_FIELD_ACK] = bus_rtn;
assign bus_out[BUS_DATA_END-1:BUS_DATA_START] = bus_rtn ? ( out << OFFSET ) : { BUS_DATA_WIDTH { 1'd0 } };

always @(posedge bus_clk or negedge bus_reset_l)
  if(!bus_reset_l)
    begin
      irq_rearm <= 0;
      bus_rtn <= 0;
    end
  else
    begin
      bus_rtn <= 0;
      if(bus_req && bus_addr == ADDR)
        begin
          bus_rtn <= 1;
          if(!bus_rd_wr_l)
            begin
              irq_rearm <= irq_rearm ^ ( bus_wr_data >> OFFSET );
            end
        end
    end

// Synchronizer

sync #(.DATAWIDTH(DATAWIDTH)) irq_syncer
  (
  .reset_l (bus_reset_l),
  .clk (bus_clk),
  .i (in),
  .o (out)
  );

endmodule
