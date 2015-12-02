// Parameterized single-port inferred RAM
// This RAM has async read port: Altera has no such RAM

module ram_sp_async
  (
  reset_l,
  clk,
  wr_data,
  addr,
  we,
  rd_data
  );

parameter DATAWIDTH = 8;
parameter ADDRWIDTH = 9;

input reset_l;
input clk;
input [DATAWIDTH-1:0] wr_data;
input [ADDRWIDTH-1:0] addr;
input we;
output [DATAWIDTH-1:0] rd_data;

reg [DATAWIDTH-1:0] ram[((1 << ADDRWIDTH) - 1) : 0];

always @(posedge clk)
  if (we)
    ram[addr] <= wr_data;

assign rd_data = ram[addr];

endmodule
