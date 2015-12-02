// Parameterized dual-port inferred RAM

// This RAM has a one flop latency (delay from wr_data to rd_data with wr_addr and rd_addr
// held the same).  Bypass logic is inferred for both Xilinx and Altera.

module ram_dp_bypassed
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
output [DATAWIDTH-1:0] rd_data;
input [ADDRWIDTH-1:0] rd_addr;

reg [DATAWIDTH-1:0] ram[((1 << ADDRWIDTH) - 1) : 0];

// Can force ram type with: synth* syn_ramstyle="m512"

integer x;

initial
  begin
    for (x = 0; x != (1 << ADDRWIDTH); x = x + 1)
      begin
        ram[x] = { DATAWIDTH { 1'b1 } };
      end
  end

reg [ADDRWIDTH-1:0] rd_addr_reg;
assign rd_data = ram[rd_addr_reg];

always @(posedge clk)
  begin
    if (we)
      ram[wr_addr] <= wr_data;
    rd_addr_reg <= rd_addr;
  end

endmodule
