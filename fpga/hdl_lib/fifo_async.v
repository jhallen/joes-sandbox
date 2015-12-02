// A parameterized Async FIFO

// Zero flop delay between re and ns_rd_data/ns_ne

// Three flop delay betwen wr_data and rd_data.

module fifo_async
  (
  wr_clk,
  wr_reset_l,

  rd_clk,
  rd_reset_l,

  wr_data,	// Write data
  we,		// Write enable
  full,		// Almost full registered
  ovf,		// Overflow

  ns_rd_data,
  rd_data,	// Read data registered
  re,		// Read enable
  ns_ne,
  ne,		// Not empty registered
  unf		// Underflow FATAL error (FIFO is broken once this is set)
  );

parameter
  ADDRWIDTH = 5, // Address width: FIFO will have 2^ADDRWIDTH words
  DATAWIDTH = 18, // Data width
  SLOPBITS = 3; // No. words between full and overflow

input wr_clk;
input wr_reset_l;

input rd_clk;
input rd_reset_l;

input [DATAWIDTH-1:0] wr_data;	// Write data
input we; // Write enable
output full; // Almost full registered
output ovf;

output [DATAWIDTH-1:0] ns_rd_data;
reg [DATAWIDTH-1:0] ns_rd_data;
output [DATAWIDTH-1:0] rd_data;	// Read data registered
reg [DATAWIDTH-1:0] rd_data;
input re; // Read enable
output ns_ne;
reg ns_ne;
output ne; // Not empty registered
reg ne;
output unf; // Underflow FATAL error (FIFO is broken once this is set)
reg unf;


// One-cycle latency FIFO

wire [DATAWIDTH-1:0] late_rd_data;
wire late_re;
wire late_ne;

fifo_async_late #(.DATAWIDTH(DATAWIDTH), .ADDRWIDTH(ADDRWIDTH), .SLOPBITS(SLOPBITS)) late_fifo
  (
  .wr_clk (wr_clk),
  .wr_reset_l (wr_reset_l),
  .wr_data (wr_data),
  .we (we),
  .full (full),
  .ovf (ovf),

  .rd_clk (rd_clk),
  .rd_reset_l (rd_reset_l),
  .rd_data (late_rd_data),
  .re (late_re),
  .ne (late_ne),
  .unf ()
  );

// Low latency 2-word FIFO

reg [DATAWIDTH-1:0] reg0;
reg [DATAWIDTH-1:0] reg1;

reg wr_addr, ns_wr_addr;
reg rd_addr, ns_rd_addr;
reg [1:0] count, ns_count;

reg ll_we_d;
reg ns_ll_full;

always @(posedge rd_clk or negedge rd_reset_l)
  if (!rd_reset_l)
    begin
      wr_addr <= 0;
      rd_addr <= 0;
      count <= 0;
      reg0 <= 0;
      reg1 <= 0;
      ne <= 0;
      rd_data <= 0;
      ll_we_d <= 0;
      unf <= 0;
    end
  else
    begin
      wr_addr <= ns_wr_addr;
      rd_addr <= ns_rd_addr;
      ne <= ns_ne;
      rd_data <= ns_rd_data;
      ll_we_d <= late_re;
      count <= ns_count;
      if (late_re)
        if (wr_addr)
          reg1 <= late_rd_data;
        else
          reg0 <= late_rd_data;
      if (re && !ne)
        begin
          unf <= 1;
          $display("%m FIFO underflow error");
          $finish;
        end
    end

always @*
  begin
    ns_rd_addr = rd_addr;
    ns_wr_addr = wr_addr;
    ns_count = count;

    if (late_re)
      ns_wr_addr = ns_wr_addr + 1'd1;

    if (re)
      ns_rd_addr = ns_rd_addr + 1'd1;

    if (re && !ll_we_d)
      ns_count = ns_count - 1'd1;
    else if (!re && ll_we_d)
      ns_count = ns_count + 1'd1;

    ns_ne = (ns_count != 0);
    ns_ll_full = (ns_count == 2);

    ns_rd_data = ns_rd_addr ? reg1 : reg0;
  end

// Transfer data from 1-cycle latency FIFO to 0-cycle latency FIFO

assign late_re = !ns_ll_full && late_ne;

endmodule
