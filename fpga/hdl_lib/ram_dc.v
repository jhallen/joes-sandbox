// Parameterized two-clock inferred RAM

module ram_dc 
  (
  wr_addr,
  rd_addr,
  wr_clk,
  rd_clk,
  wr_data,
  we,
  rd_data
  );

parameter
  DATAWIDTH = 18,
  ADDRWIDTH = 5;

input [ADDRWIDTH-1:0] wr_addr; 
input [ADDRWIDTH-1:0] rd_addr;
input wr_clk;
input rd_clk;
input [DATAWIDTH-1:0] wr_data;
input we;
output reg [DATAWIDTH-1:0] rd_data;

reg [DATAWIDTH-1:0] ram[((1 << ADDRWIDTH) - 1) : 0];

always @(posedge wr_clk)
  if (we)
    ram[wr_addr] <= wr_data;

always @(posedge rd_clk)
  rd_data <= ram[rd_addr];

endmodule
