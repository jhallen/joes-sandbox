// A simple direct mapped cache write-through cache
// Burst-size and line size fixed at 4 words.

module cache
  (
  reset_l,
  clk,

  client_req,
  client_gnt,
  client_write,
  client_addr, // Byte address
  client_wr_data,
  client_rd_data,

  mem_req,
  mem_gnt,
  mem_write, // Byte address
  mem_addr,
  mem_wr_data,
  mem_rd_data
  );

// You get to set these:
parameter LOG2BYTEWIDTH = 3; // Log2 Bytes per word
parameter LOG2CACHESIZE = 10; // Log2 Bytes of cache
parameter ADDRWIDTH = 20; // Byte address width: least significant LOG2BYTEWIDTH bits are always zero

// These are all derived
parameter BYTEWIDTH = (1 << LOG2BYTEWIDTH); // No. bytes per word (8)
parameter DATAWIDTH = (BYTEWIDTH * 8); // No. bits per word (64)
parameter TAGWIDTH = (ADDRWIDTH - LOG2CACHESIZE); // No. tag address bits (10)
parameter LOG2LINESIZE = (2 + LOG2BYTEWIDTH); // Log2 of line size in bytes (5) (line size = one burst of 4 words).

input reset_l;
input clk;

input client_req;

output client_gnt;
reg client_gnt;

input client_write;
input [ADDRWIDTH-1:0] client_addr;
input [DATAWIDTH-1:0] client_wr_data;

output [DATAWIDTH-1:0] client_rd_data;
reg [DATAWIDTH-1:0] client_rd_data, nxt_client_rd_data;

output mem_req;
reg mem_req, nxt_mem_req;
input mem_gnt;
output mem_write;
reg mem_write, nxt_mem_write;
output [ADDRWIDTH-1:0] mem_addr;
reg [ADDRWIDTH-1:0] mem_addr, nxt_mem_addr;
output [DATAWIDTH-1:0] mem_wr_data;
reg [DATAWIDTH-1:0] mem_wr_data, nxt_mem_wr_data;
input [DATAWIDTH-1:0] mem_rd_data;

// Here is the cache ram

reg [ADDRWIDTH-1:0] cache_addr, nxt_cache_addr;
reg [DATAWIDTH-1:0] cache_wr_data, nxt_cache_wr_data;
reg cache_we, nxt_cache_we;
wire [DATAWIDTH-1:0] cache_rd_data;

ram_blk_sp #(.DATAWIDTH(DATAWIDTH), .ADDRWIDTH(LOG2CACHESIZE - LOG2BYTEWIDTH)) cache_data
  (
  .reset_l (reset_l),
  .clk (clk),
  .wr_data (nxt_cache_wr_data),
  .addr (nxt_cache_addr[ADDRWIDTH-1:LOG2BYTEWIDTH]),
  .we (nxt_cache_we),
  .rd_data (cache_rd_data)
  );

// Here is the tag ram

reg [TAGWIDTH-1:0] cache_wr_tag, nxt_cache_wr_tag;
reg cache_wr_valid, nxt_cache_wr_valid;
wire [TAGWIDTH-1:0] cache_rd_tag;
wire cache_rd_valid;

ram_blk_sp #(.DATAWIDTH(TAGWIDTH + 1), .ADDRWIDTH(LOG2CACHESIZE - LOG2LINESIZE)) cache_tags
  (
  .reset_l (reset_l),
  .clk (clk),
  .wr_data ({ nxt_cache_wr_valid, nxt_cache_wr_tag }),
  .addr (nxt_cache_addr[(ADDRWIDTH - 1):LOG2LINESIZE]),
  .we (nxt_cache_we),
  .rd_data ({ cache_rd_valid, cache_rd_tag })
  );

// State machine
// Mealy ouputs go to cache and tag rams
// client_gnt is also a Mealy output

reg [3:0] state, nxt_state;

parameter
  RESET = 0,
  IDLE = 1,
  WRITE = 2,
  WR_BACK_READ = 3,
  WR_BACK_REQ = 4,
  WR_BACK_WAIT = 5,
  WR_BACK = 6,
  RD_CACHE = 7,
  RD_FILL_WAIT = 8,
  RD_FILL_1 = 9,
  RD_FILL = 10,
  READ = 11;

reg [1:0] count, nxt_count;

always @(posedge clk or negedge reset_l)
  if (!reset_l)
    begin
      state <= RESET;
      cache_we <= 0;
      cache_addr <= 0;
      cache_wr_data <= 0;
      cache_wr_tag <= 0;
      cache_wr_valid <= 0;
      mem_req <= 0;
      mem_write <= 0;
      mem_addr <= 0;
      mem_wr_data <= 0;
      client_rd_data <= 0;
      count <= 0;
    end
  else
    begin
      state <= nxt_state;
      cache_we <= nxt_cache_we;
      cache_addr <= nxt_cache_addr;
      cache_wr_data <= nxt_cache_wr_data;
      cache_wr_tag <= nxt_cache_wr_tag;
      cache_wr_valid <= nxt_cache_wr_valid;
      mem_req <= nxt_mem_req;
      mem_write <= nxt_mem_write;
      mem_addr <= nxt_mem_addr;
      mem_wr_data <= nxt_mem_wr_data;
      client_rd_data <= nxt_client_rd_data;
      count <= nxt_count;
    end


always @*
  begin
    nxt_state = state;
    nxt_cache_we = 0;
    nxt_cache_addr = cache_addr;
    nxt_cache_wr_data = cache_wr_data;
    nxt_cache_wr_tag = cache_wr_tag;
    nxt_cache_wr_valid = cache_wr_valid;
    nxt_mem_req = mem_req;
    nxt_mem_write = mem_write;
    nxt_mem_addr = mem_addr;
    nxt_mem_wr_data = mem_wr_data;
    nxt_client_rd_data = 'bx;
    nxt_count = count;
    client_gnt = 0;

    case (state)
      RESET:
        begin
          // Clear all valid bits
          nxt_cache_we = 1;
          nxt_cache_addr[LOG2CACHESIZE-1:0] = cache_addr + (1 << LOG2LINESIZE);
          if (nxt_cache_addr == 0)
            nxt_state = IDLE;
        end

      IDLE: // Wait for requests
        begin
          nxt_cache_addr = client_addr;
          nxt_mem_addr = client_addr;
          nxt_cache_wr_tag = client_addr[ADDRWIDTH-1:LOG2LINESIZE];
          nxt_cache_wr_valid = 1;
          nxt_cache_wr_data = client_wr_data;
          nxt_count = 2;
          if (client_req)
            if (client_write)
              begin // Write request
                // Write data into cache...
                // copy it to memory afterwards
                client_gnt = 1;
                nxt_cache_we = 1;
                nxt_state = WRITE;
              end
            else
              begin // Read request
                // Read from cache ram...
                nxt_state = RD_CACHE;
              end
        end

      WRITE: // Write data into cache...
        begin
          nxt_count = count - 1;
          nxt_cache_addr[LOG2LINESIZE-1:0] = cache_addr + (1 << LOG2BYTEWIDTH);
          nxt_cache_wr_tag = client_addr[ADDRWIDTH-1:LOG2LINESIZE];
          nxt_cache_wr_valid = 1;
          nxt_cache_wr_data = client_wr_data;
          nxt_cache_we = 1;
          if (!count)
            nxt_state = WR_BACK_READ;
        end

      WR_BACK_READ:
        begin
          nxt_cache_addr[LOG2LINESIZE-1:0] = 0;
          nxt_state = WR_BACK_REQ;
        end

      WR_BACK_REQ: // Write-through: copy data from cache to memory
        begin
          nxt_mem_req = 1;
          nxt_cache_addr[LOG2LINESIZE-1:0] = cache_addr + (1 << LOG2BYTEWIDTH);
          nxt_mem_write = 1;
          nxt_mem_wr_data = cache_rd_data;
          nxt_state = WR_BACK_WAIT;
        end

      WR_BACK_WAIT: // Wait for memory write grant
        if (mem_gnt)
          begin
            nxt_mem_req = 0;
            nxt_mem_wr_data = cache_rd_data; // 2nd word to memory
            nxt_cache_addr[LOG2LINESIZE-1:0] = cache_addr + (1 << LOG2BYTEWIDTH);
            nxt_count = 2;
            nxt_state = WR_BACK;
          end

      WR_BACK:
        begin
          nxt_count = count - 1;
          nxt_mem_wr_data = cache_rd_data;
          nxt_cache_addr[LOG2LINESIZE-1:0] = cache_addr + (1 << LOG2BYTEWIDTH);
          if (!count)
            nxt_state = IDLE;
        end

      RD_CACHE: // Check valid bit and tag
        begin
          // Start bursting...
          nxt_cache_addr[LOG2LINESIZE-1:0] = cache_addr + (1 << LOG2BYTEWIDTH);
          if (cache_rd_valid && cache_rd_tag == client_addr[ADDRWIDTH-1:LOG2LINESIZE])
            begin // Cache line is valid
              // Send first word to client
              client_gnt = 1;
              nxt_client_rd_data = cache_rd_data;
              nxt_state = READ;
              nxt_count = 3;
            end
          else
            begin
              // Request data from memory
              nxt_mem_req = 1;
              nxt_mem_write = 0;
              nxt_state = RD_FILL_WAIT;
            end
        end

      RD_FILL_WAIT:
        if (mem_gnt)
          begin
            nxt_mem_req = 0;
            nxt_state = RD_FILL_1;
          end

      RD_FILL_1:
        begin
          client_gnt = 1;
          nxt_client_rd_data = mem_rd_data;
          nxt_cache_wr_data = mem_rd_data;
          nxt_cache_we = 1;
          nxt_cache_addr[LOG2LINESIZE-1:0] = 0;
          nxt_cache_wr_valid = 1;
          nxt_count = 2;
          nxt_state = RD_FILL;
        end

      RD_FILL:
        begin
          nxt_count = count - 1;
          nxt_client_rd_data = mem_rd_data;
          nxt_cache_wr_data = mem_rd_data;
          nxt_cache_we = 1;
          nxt_cache_addr[LOG2LINESIZE-1:0] = cache_addr + (1 << LOG2BYTEWIDTH);
          nxt_cache_wr_valid = 1;
          if (!count)
            nxt_state = IDLE;
        end

      READ:
        begin
          // Send second word to client
          nxt_cache_addr[LOG2LINESIZE-1:0] = cache_addr + (1 << LOG2BYTEWIDTH);
          nxt_client_rd_data = cache_rd_data;
          nxt_count = count - 1;
          if (!count)
            state = IDLE;
        end
    endcase
  end

endmodule
