// A parameterized FIFO

// One flop delay between re and rd_data/ne.

// Two flop delay from wr_data to rd_data, which is due to
// the ram.

module fifo_sync_late
  (
  clk,
  reset_l,

  wr_data, // Write data
  we, // Write enable
  ns_full,
  full, // Almost full
  ovf, // Overflow FATAL error (FIFO is broken once this is set)

  rd_data, // Read data registered
  re, // Read enable
  ne, // Not empty
  unf // Underflow FATAL error (FIFO is broken once this is set)
  );

parameter
  ADDRWIDTH = 5, // Address width: FIFO will have 2^ADDRWIDTH words
  DATAWIDTH = 18, // Data width
  SLOP = 4; // No. words between full and overflow

input clk;
input reset_l;

input [DATAWIDTH-1:0] wr_data; // Write data
input we; // Write enable
output ns_full;
reg ns_full;
output full; // Almost full
reg full;
output ovf; // Overflow FATAL error (FIFO is broken once this is set)
reg ovf;

output [DATAWIDTH-1:0] rd_data; // Read data registered
input re; // Read enable
output ne; // Not empty
reg ne;
output unf; // Underflow FATAL error (FIFO is broken once this is set)
reg unf;


reg [ADDRWIDTH-1:0] rd_addr, ns_rd_addr; // Points to oldest data
reg [ADDRWIDTH-1:0] wr_addr, ns_wr_addr;
reg [ADDRWIDTH:0] count, ns_count; // No. words in FIFO

ram_dp #(.DATAWIDTH(DATAWIDTH), .ADDRWIDTH(ADDRWIDTH)) ram
  (
  .clk (clk),
  .wr_data (wr_data),
  .wr_addr (wr_addr),
  .we (we),
  .rd_data (rd_data),
  .rd_addr (ns_rd_addr)
  );

// State machine

reg we_d; // Delay assertion of ne after we because ram has 2 cycle delay

always @(posedge clk or negedge reset_l)
  if (!reset_l)
    begin
      rd_addr <= 0;
      count <= 0;
      ovf <= 0;
      unf <= 0;
      full <= 0;
      wr_addr <= 0;
      we_d <= 0;
    end
  else
    begin
      we_d <= we;
      wr_addr <= ns_wr_addr;
      rd_addr <= ns_rd_addr;
      count <= ns_count;
      full <= ns_full;

      // Don't care about latency of these flags
      if (re && !ne)
        begin
          unf <= 1;
          $display("%m FIFO underflow error");
          $finish;
        end

      if (we_d && (count == (1 << ADDRWIDTH)))
        begin
          ovf <= 1;
          $display("%m FIFO overflow error");
          $finish;
        end
    end

// Read side state

always @(*)
  begin
    ns_rd_addr = rd_addr;
    ns_wr_addr = wr_addr;
    ns_count = count;

    if (re)
      ns_rd_addr = ns_rd_addr + 1'd1;

    if (we)
      ns_wr_addr = ns_wr_addr + 1'd1;

    if (we_d && !re)
      ns_count = ns_count + 1'd1;
    else if (re && !we_d)
      ns_count = ns_count - 1'd1;

    ns_full = (ns_count >= (1 << ADDRWIDTH) - SLOP);
    ne = (count != 0);
  end

endmodule
