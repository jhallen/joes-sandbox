// A parameterized FIFO

// One flop delay between re and rd_data/ne.

module fifo_sync_late
  #(parameter ADDRWIDTH = 5, // Address width: FIFO will have 2^ADDRWIDTH words
              DATAWIDTH = 18, // Data width
              SLOP = 4) // No. words between full and overflow
  (
  input clk,
  input reset_l,

  input [DATAWIDTH-1:0] wr_data, // Write data
  input we, // Write enable
  output reg ns_full,
  output reg full, // Almost full
  output reg ovf, // Overflow FATAL error (FIFO is broken once this is set)

  output [DATAWIDTH-1:0] rd_data, // Read data registered
  input re, // Read enable
  output reg ne, // Not empty
  output reg unf // Underflow FATAL error (FIFO is broken once this is set)
  );

reg [ADDRWIDTH-1:0] rd_addr, ns_rd_addr; // Points to oldest data
reg [ADDRWIDTH-1:0] wr_addr, ns_wr_addr;
reg [ADDRWIDTH:0] count, ns_count; // No. words in FIFO

ram_blk_dp #(.DATAWIDTH(DATAWIDTH), .ADDRWIDTH(ADDRWIDTH)) ram
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
        unf <= 1;

      if (count == (1 << ADDRWIDTH))
        ovf <= 1;
    end

// Read side state

always @(*)
  begin
    ns_rd_addr = rd_addr;
    ns_wr_addr = wr_addr;
    ns_count = count;

    if (re)
      ns_rd_addr = ns_rd_addr + 1;

    if (we)
      ns_wr_addr = ns_wr_addr + 1;

    if (we_d && !re)
      ns_count = ns_count + 1;
    else if (re && !we_d)
      ns_count = ns_count - 1;

    ns_full = (ns_count >= (1 << ADDRWIDTH) - SLOP);
    ne = (count != 0);
  end

endmodule
