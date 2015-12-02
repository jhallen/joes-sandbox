// A trigger-on-write command register

module bus_trig_reg
  (
  bus_in,
  bus_out,

  in,		// Acknowledgement bits
  out		// Operation should start on rising edge and stop on falling edge.
  );

parameter ADDR=0;
parameter OFFSET=0;
parameter DATAWIDTH=32;

`include "bus_params.v"

input [BUS_IN_WIDTH-1:0] bus_in;

output [BUS_OUT_WIDTH-1:0] bus_out;

input [DATAWIDTH-1:0] in;

output [DATAWIDTH-1:0] out;
reg [DATAWIDTH-1:0] out;

`include "bus_decl.v"

// Bus driver

reg bus_rtn;

assign bus_out[BUS_FIELD_ACK] = bus_rtn;

assign bus_out[BUS_DATA_END-1:BUS_DATA_START] = bus_rtn ? ( out << OFFSET ) : { BUS_DATA_WIDTH { 1'd0 } };

// State machine

wire [DATAWIDTH-1:0] det;

always @(posedge bus_clk or negedge bus_reset_l)
  if(!bus_reset_l)
    begin
      out <= 0;
      bus_rtn <= 0;
    end
  else
    begin
      bus_rtn <= 0;
      out <= out & ~det;
      if(bus_req && bus_addr == ADDR)
        begin
          bus_rtn <= 1;
          if(!bus_rd_wr_l)
            begin
              out <= (( bus_wr_data >> OFFSET ) & ~det);
            end
        end
    end

// Edge detectors

edge_det #(.DATAWIDTH(DATAWIDTH)) edge_det
  (
  .reset_l (bus_reset_l),
  .clk (bus_clk),
  .i (in),
  .o (det)
  );

endmodule
