// Parameterized dual-address inferred RAM

// Note that this RAM has a two flop latency (delay from wr_data to rd_data with wr_addr and
// rd_addr held the same).

// If both ports write to the same address in the same cycle, the results are not defined.

module ram_da
  (
  clk,

  addr_1,
  wr_data_1,
  rd_data_1,
  we_1,

  addr_2,
  wr_data_2,
  rd_data_2,
  we_2
  );

parameter DATAWIDTH = 9;
parameter ADDRWIDTH = 9;

input clk;

input [ADDRWIDTH-1:0] addr_1;
input [DATAWIDTH-1:0] wr_data_1;
output [DATAWIDTH-1:0] rd_data_1;
input we_1;

input [ADDRWIDTH-1:0] addr_2;
input [DATAWIDTH-1:0] wr_data_2;
output [DATAWIDTH-1:0] rd_data_2;
input we_2;

ram_alt #(.DATAWIDTH_1 (DATAWIDTH), 
          .DATAWIDTH_2 (DATAWIDTH),
          .ADDRWIDTH_1 (ADDRWIDTH),
          .ADDRWIDTH_2 (ADDRWIDTH)) ram
  (
  .clk_1 (clk),
  .addr_1 (addr_1),
  .wr_data_1 (wr_data_1),
  .rd_data_1 (rd_data_1),
  .we_1 (we_1),
  .clk_2 (clk),
  .addr_2 (addr_2),
  .wr_data_2 (wr_data_2),
  .rd_data_2 (rd_data_2),
  .we_2 (we_2)
  );

`ifdef junk
// quartus is having trouble with this

reg [DATAWIDTH-1:0] rd_data_1;
reg [DATAWIDTH-1:0] rd_data_2;

reg [DATAWIDTH-1:0] ram[((1 << ADDRWIDTH) - 1) : 0];

always @(posedge clk)
  if (we_1)
    begin
      ram[addr_1] <= wr_data_1;
      rd_data_1 <= wr_data_1;
    end
  else
    rd_data_1 <= ram[addr_1];

always @(posedge clk)
  if (we_2)
    begin
      ram[addr_2] <= wr_data_2;
      rd_data_2 <= wr_data_2;
    end
  else
    rd_data_2 <= ram[addr_2];
`endif

endmodule
