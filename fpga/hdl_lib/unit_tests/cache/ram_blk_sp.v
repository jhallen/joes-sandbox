// Parameterized single-port RAM

module ram_blk_sp #(parameter DATAWIDTH = 8, ADDRWIDTH = 9)
  (
  input reset_l,
  input clk,
  input [DATAWIDTH-1:0] wr_data,
  input [ADDRWIDTH-1:0] addr,
  input we,
  output reg [DATAWIDTH-1:0] rd_data
  );

reg [DATAWIDTH-1:0] ram[((1 << ADDRWIDTH) - 1) : 0];

always @(posedge clk)
  begin
    if (we)
      ram[addr] <= wr_data;
    rd_data <= ram[addr];
  end

endmodule
