// Async FIFO

module fifo_async_late
  (
  wr_clk,
  wr_reset_l,
  wr_data,
  we,
  full,
  ovf,

  rd_clk,
  rd_reset_l,
  rd_data,
  re,
  ne,
  unf
  );

parameter DATAWIDTH = 18;
parameter ADDRWIDTH = 5;
parameter SLOPBITS = 2; // Log2(SLOP)

input wr_clk;
input wr_reset_l;
input [DATAWIDTH-1:0] wr_data;
input we;
output full;
output ovf;

input rd_clk;
input rd_reset_l;
output [DATAWIDTH-1:0] rd_data;
input re;
output ne;
output unf;

// Convert binary to gray code

function [ADDRWIDTH-1:0] bin_to_gray;
input [ADDRWIDTH-1:0] i;
integer x;
  begin
    bin_to_gray[ADDRWIDTH-1] = i[ADDRWIDTH-1];
    for (x = 0; x != ADDRWIDTH - 1; x = x + 1)
      bin_to_gray[x] = i[x]^i[x+1];
  end
endfunction

// Convert gray code to binary

function [ADDRWIDTH-1:0] gray_to_bin;
input [ADDRWIDTH-1:0] i;
integer x;
  begin
    gray_to_bin = i;
    for (x = 1; x != ADDRWIDTH; x = x + 1)
      gray_to_bin = gray_to_bin ^ (i >> x);
  end
endfunction

// Write address
reg [ADDRWIDTH-1:0] wr_addr;

// Write address in gray code
wire [ADDRWIDTH-1:0] wr_addr_gray = bin_to_gray(wr_addr);

// Synchronizer to get write address in read domain
reg [ADDRWIDTH-1:0] wr_addr_gray_1;
reg [ADDRWIDTH-1:0] wr_addr_gray_2;

// Convert gray back to binary
wire [ADDRWIDTH-1:0] wr_addr_ungray = gray_to_bin(wr_addr_gray_2);

// Read address
reg [ADDRWIDTH-1:0] rd_addr;

// Read address, gray coded
wire [ADDRWIDTH-1:0] rd_addr_gray = bin_to_gray(rd_addr);

// Synchonizer to get read address in write domain
reg [ADDRWIDTH-1:0] rd_addr_gray_1;
reg [ADDRWIDTH-1:0] rd_addr_gray_2;

// Convert back to binary
wire [ADDRWIDTH-1:0] rd_addr_ungray = gray_to_bin(rd_addr_gray_2);

wire [ADDRWIDTH-1:0] ram_rd_addr = re ? rd_addr + 1'd1 : rd_addr;

ram_dc #(.ADDRWIDTH(ADDRWIDTH), .DATAWIDTH(DATAWIDTH)) ram
  (
  .wr_clk(wr_clk),
  .wr_addr (wr_addr),
  .wr_data (wr_data),
  .we (we),
  .rd_clk (rd_clk),
  .rd_addr (ram_rd_addr),
  .rd_data (rd_data)
  );

// Write clock

reg ovf;

always @(posedge wr_clk or negedge wr_reset_l)
  if (!wr_reset_l)
    begin
      wr_addr <= 0;
      rd_addr_gray_1 <= 0;
      rd_addr_gray_2 <= 0;
      ovf <= 0;
    end
  else
    begin
      if (we)
        wr_addr <= wr_addr + 1'd1;

      rd_addr_gray_1 <= rd_addr_gray;
      rd_addr_gray_2 <= rd_addr_gray_1;

      if (we && (wr_addr + 1'd1 == rd_addr_ungray))
        begin
          ovf <= 1;
          $display("%m FIFO overflow error");
          $finish;
        end
    end

// Full flag

wire full = (wr_addr[ADDRWIDTH-1:SLOPBITS] + 1'd1) == rd_addr_ungray[ADDRWIDTH-1:SLOPBITS];

// Read clock

reg unf;

always @(posedge rd_clk or negedge rd_reset_l)
  if (!rd_reset_l)
    begin
      rd_addr <= 0;
      wr_addr_gray_1 <= 0;
      wr_addr_gray_2 <= 0;
      unf <= 0;
    end
  else
    begin
      if (re)
        rd_addr <= rd_addr + 1'd1;

      wr_addr_gray_1 <= wr_addr_gray;
      wr_addr_gray_2 <= wr_addr_gray_1;

      if (re && !ne)
        begin
          unf <= 1;
          $display("%m FIFO underflow error");
          $finish;
        end
    end

// Not empty flag

wire ne = (rd_addr != wr_addr_ungray);

endmodule
