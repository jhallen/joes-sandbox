// Parameterized single-port inferred RAM

module ram_sp
  (
  clk,
  wr_data,
  addr,
  we,
  rd_data
  );

parameter DATAWIDTH = 8;
parameter ADDRWIDTH = 9;

input clk;
input [DATAWIDTH-1:0] wr_data;
input [ADDRWIDTH-1:0] addr;
input we;
output reg [DATAWIDTH-1:0] rd_data;

reg [DATAWIDTH-1:0] ram[((1 << ADDRWIDTH) - 1) : 0];

always @(posedge clk)
  begin
    if (we)
      ram[addr] <= wr_data;
    rd_data <= ram[addr];
  end

endmodule
