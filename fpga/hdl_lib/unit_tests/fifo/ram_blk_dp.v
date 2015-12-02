// Parameterized dual-port RAM

module ram_blk_dp #(parameter DATAWIDTH = 8, ADDRWIDTH = 9)
  (
  input clk,
  input [DATAWIDTH-1:0] wr_data,
  input [ADDRWIDTH-1:0] wr_addr,
  input we,
  output reg [DATAWIDTH-1:0] rd_data,
  input [ADDRWIDTH-1:0] rd_addr
  );

reg [DATAWIDTH-1:0] ram[((1 << ADDRWIDTH) - 1) : 0];

always @(posedge clk)
  begin
    if (we)
      ram[wr_addr] <= wr_data;
    rd_data <= ram[rd_addr];
  end

endmodule
