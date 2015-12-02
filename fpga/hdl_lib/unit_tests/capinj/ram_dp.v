// Parameterized dual-port inferred RAM

// Note that this RAM has a two flop latency (delay from wr_data to rd_data with wr_addr and
// rd_addr held the same).

module ram_dp
  (
  clk,
  wr_data,
  wr_addr,
  we,
  rd_data,
  rd_addr
  );

parameter DATAWIDTH = 9;
parameter ADDRWIDTH = 9;

input clk;
input [DATAWIDTH-1:0] wr_data;
input [ADDRWIDTH-1:0] wr_addr;
input we;
output reg [DATAWIDTH-1:0] rd_data;
input [ADDRWIDTH-1:0] rd_addr;

reg [DATAWIDTH-1:0] ram[((1 << ADDRWIDTH) - 1) : 0];

// can force type with: synth* syn_ramstyle="m512"

always @(posedge clk)
  begin
    if (we)
      ram[wr_addr] <= wr_data;
    rd_data <= ram[rd_addr];
  end

endmodule
