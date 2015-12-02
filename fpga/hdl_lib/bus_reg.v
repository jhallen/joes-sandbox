// A programmable register

module bus_reg
  (
  bus_in,
  bus_out,

  in,	// Register inputs
  out,	// Register outputs

  read,	// Pulses during a read
  write	// Pulses during a write
  );

parameter DATAWIDTH = 32; // No. bits (1..32)
parameter OFFSET = 0; // Bit position (0..31)
parameter IZ = 0; // Initial value
parameter ADDR = 0; // Address
parameter REG = 0; // Tell rattr that this is a register

`include "bus_params.v"

input [BUS_IN_WIDTH-1:0] bus_in;

output [BUS_OUT_WIDTH-1:0] bus_out;

input [DATAWIDTH-1:0] in;

output [DATAWIDTH-1:0] out;
reg [DATAWIDTH-1:0] out;

output read;
reg read;

output write;
reg write;

`include "bus_decl.v"

// Bus driver

reg bus_rtn;

assign bus_out[BUS_DATA_END-1:BUS_DATA_START] = bus_rtn ? ( in << OFFSET) : { BUS_DATA_WIDTH { 1'd0 } };

assign bus_out[BUS_FIELD_ACK] = bus_rtn;

always @(posedge bus_clk or negedge bus_reset_l)
  if(!bus_reset_l)
    begin
      out <= IZ;
      bus_rtn <= 0;
      read <= 0;
      write <= 0;
    end
  else
    begin
      bus_rtn <= 0;
      read <= 0;
      write <= 0;
      if(bus_req && bus_addr == ADDR)
        begin
          bus_rtn <= 1;
          if(bus_rd_wr_l)
            begin
              read <= 1;
            end
          else
            begin
              out <= ( bus_wr_data >> OFFSET );
              write <= 1;
            end
        end
    end

endmodule
