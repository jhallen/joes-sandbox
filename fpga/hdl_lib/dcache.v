// A simple direct mapped write-through cache
// Burst-size and line size fixed at 4 words.

// Notes:
//   Reads always return four words.  However, they return the first requested word within the line first (as specified by
//   address bits 4:3).  Read bursts only return data from a single cache line, so the address wraps at the next cache line
//   boundary.
//
//   Writes are 1 - 4 word bursts.  Last word is indicated with client_blast high (not needed for four-word burst).
//
//   Writes start at first requested word within the line first (as specified by address buts 4:3).  The address wraps
//   around to the start of the cache line (so a burst only effects a single cache line).
//
//   Memory reads are expected to return data in cache-line order (return first specified word of line first, wrap at
//   cache line boundary).  During a miss, reads are bypassed directly to the client as they fill the cache.
//
//   Memory writes always start at word 0 of the cache-line boundary.  Always one whole line it written.
//
//   Writes which miss always incur a full line fill.  It would be nice to avoid the fill if we know that the entire
//   line is going to be written, but we don't have this information up front (add as extension, modify clients to make
//   use of it..).
//
//   Write-through is really an immediate write back (cache started out as a write-back cache, but this would have added
//   DV work- it's nice that memory is always up to date).  This is inefficient since we always do a full line write back
//   on even a single word write by the client.

module dcache
  (
  reset_l,
  clk,

  client_req,
  client_gnt,
  client_write,
  client_blast,
  client_addr, // Byte address
  client_wr_data,
  client_wr_par,
  client_rd_data,
  client_rd_par,

  mem_req,
  mem_gnt,
  mem_write, // Byte address
  mem_addr,
  mem_wr_data,
  mem_wr_par,
  mem_rd_data,
  mem_rd_par,

  invalidate_and_wait,

  state
  );

output [3:0] state;

// You get to set these:
parameter LOG2BYTEWIDTH = 3; // Log2 Bytes per word
parameter LOG2CACHESIZE = 15; // Log2 Bytes of cache
parameter ADDRWIDTH = 29; // Byte address width: least significant LOG2LINESIZE bits are always zero
parameter PARWIDTH = 8;
parameter BASE = 0;	// Address of memory region to clear at startup
parameter SIZE = 0;	// Size of memory region to clear (must be multiple of (1<<LOG2LINESIZE)).

// These are all derived
parameter BYTEWIDTH = (1 << LOG2BYTEWIDTH); // No. bytes per word (8)
parameter DATAWIDTH = (BYTEWIDTH * 8); // No. bits per word (64)
parameter LOG2LINESIZE = (2 + LOG2BYTEWIDTH); // Log2 of line size in bytes (5) (line size = one burst of 4 words).
parameter TAGWIDTH = (ADDRWIDTH - LOG2CACHESIZE); // No. tag address bits (19)

input reset_l;
input clk;

input client_req;

output client_gnt;
reg client_gnt;

input client_write;
input client_blast;
input [ADDRWIDTH-1:0] client_addr;
input [DATAWIDTH-1:0] client_wr_data;
input [PARWIDTH-1:0] client_wr_par;

output [DATAWIDTH-1:0] client_rd_data;
reg [DATAWIDTH-1:0] client_rd_data, nxt_client_rd_data;

output [PARWIDTH-1:0] client_rd_par;
reg [PARWIDTH-1:0] client_rd_par, nxt_client_rd_par;

output mem_req;
reg mem_req, nxt_mem_req;
input mem_gnt;
output mem_write;
reg mem_write, nxt_mem_write;
output [ADDRWIDTH-1:0] mem_addr;
reg [ADDRWIDTH-1:0] mem_addr, nxt_mem_addr;
output [DATAWIDTH-1:0] mem_wr_data;
reg [DATAWIDTH-1:0] mem_wr_data, nxt_mem_wr_data;
output [PARWIDTH-1:0] mem_wr_par;
reg [PARWIDTH-1:0] mem_wr_par, nxt_mem_wr_par;
input [DATAWIDTH-1:0] mem_rd_data;
input [PARWIDTH-1:0] mem_rd_par;

input invalidate_and_wait;

// Here is the cache ram

reg [ADDRWIDTH-1:0] cache_addr, nxt_cache_addr;
reg [DATAWIDTH-1:0] cache_wr_data, nxt_cache_wr_data;
reg [PARWIDTH-1:0] cache_wr_par, nxt_cache_wr_par;
// reg cache_we;
reg nxt_cache_we;
wire [DATAWIDTH-1:0] cache_rd_data;
wire [PARWIDTH-1:0] cache_rd_par;

wire [DATAWIDTH+PARWIDTH-1:0] cache_data_wr_data = { nxt_cache_wr_par, nxt_cache_wr_data };
wire [DATAWIDTH+PARWIDTH-1:0] cache_data_rd_data;
assign
  { cache_rd_par, cache_rd_data } = cache_data_rd_data;
wire [LOG2CACHESIZE-LOG2BYTEWIDTH-1:0] cache_data_addr = nxt_cache_addr[LOG2CACHESIZE-1:LOG2BYTEWIDTH];

ram_sp #(.DATAWIDTH(DATAWIDTH+PARWIDTH), .ADDRWIDTH(LOG2CACHESIZE - LOG2BYTEWIDTH)) cache_data
  (
  .clk (clk),
  .wr_data (cache_data_wr_data),
  .addr (cache_data_addr),
  .we (nxt_cache_we),
  .rd_data (cache_data_rd_data)
  );

// Here is the tag ram

reg [TAGWIDTH-1:0] cache_wr_tag, nxt_cache_wr_tag;
reg cache_wr_valid, nxt_cache_wr_valid;
wire [TAGWIDTH-1:0] cache_rd_tag;
wire cache_rd_valid;

wire [TAGWIDTH:0] cache_tags_wr_data = { nxt_cache_wr_valid, nxt_cache_wr_tag };
wire [TAGWIDTH:0] cache_tags_rd_data;
wire [LOG2CACHESIZE-LOG2LINESIZE-1:0] cache_tags_addr = nxt_cache_addr[LOG2CACHESIZE-1:LOG2LINESIZE];
assign
  { cache_rd_valid, cache_rd_tag } = cache_tags_rd_data;

ram_sp #(.DATAWIDTH(TAGWIDTH + 1), .ADDRWIDTH(LOG2CACHESIZE - LOG2LINESIZE)) cache_tags
  (
  .clk (clk),
  .wr_data (cache_tags_wr_data),
  .addr (cache_tags_addr),
  .we (nxt_cache_we),
  .rd_data (cache_tags_rd_data)
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
  READ = 11,
  WRITE_FIRST = 12,
  CLEAR = 13,
  CLEAR_WAIT = 14;

reg [1:0] count, nxt_count;

reg read_for_write, nxt_read_for_write;

reg [31:0] hit_count, nxt_hit_count;
reg [31:0] total_count, nxt_total_count;

reg inv_old, nxt_inv_old;

always @(posedge clk or negedge reset_l)
  if (!reset_l)
    begin
      state <= RESET;
//      cache_we <= 0;
      cache_addr <= 0;
      cache_wr_data <= 0;
      cache_wr_par <= 0;
      cache_wr_tag <= 0;
      cache_wr_valid <= 0;
      mem_req <= 0;
      mem_write <= 0;
      mem_addr <= 0;
      mem_wr_data <= 0;
      client_rd_data <= 0;
      mem_wr_par <= 0;
      client_rd_par <= 0;
      count <= 0;
      read_for_write <= 0;
      hit_count <= 0;
      total_count <= 0;
      inv_old <= 0;
    end
  else
    begin
      hit_count <= nxt_hit_count;
      total_count <= nxt_total_count;
      state <= nxt_state;
//      cache_we <= nxt_cache_we;
      cache_addr <= nxt_cache_addr;
      cache_wr_data <= nxt_cache_wr_data;
      cache_wr_par <= nxt_cache_wr_par;
      cache_wr_tag <= nxt_cache_wr_tag;
      cache_wr_valid <= nxt_cache_wr_valid;
      mem_req <= nxt_mem_req;
      mem_write <= nxt_mem_write;
      mem_addr <= nxt_mem_addr;
      mem_wr_data <= nxt_mem_wr_data;
      client_rd_data <= nxt_client_rd_data;
      mem_wr_par <= nxt_mem_wr_par;
      client_rd_par <= nxt_client_rd_par;
      count <= nxt_count;
      read_for_write <= nxt_read_for_write;
      inv_old <= nxt_inv_old;
    end


always @*
  begin
    nxt_state = state;
    nxt_cache_we = 0;
    nxt_cache_addr = cache_addr;
    nxt_cache_wr_data = cache_wr_data;
    nxt_cache_wr_par = cache_wr_par;
    nxt_cache_wr_tag = cache_wr_tag;
    nxt_cache_wr_valid = cache_wr_valid;
    nxt_mem_req = mem_req;
    nxt_mem_write = mem_write;
    nxt_mem_addr = mem_addr;
    nxt_mem_wr_data = mem_wr_data;
    nxt_mem_wr_par = mem_wr_par;
    nxt_client_rd_data = { DATAWIDTH { 1'bx } };
    nxt_client_rd_par = { PARWIDTH { 1'bx } };
    nxt_count = count;
    nxt_read_for_write = read_for_write;
    client_gnt = 0;
    nxt_total_count = total_count;
    nxt_hit_count = hit_count;
    nxt_inv_old = inv_old;

    case (state)
      RESET:
        begin
          // Clear all valid bits
          nxt_cache_we = 1;
          nxt_cache_addr[LOG2CACHESIZE-1:0] = cache_addr[LOG2CACHESIZE-1:0] + (1'd1 << LOG2LINESIZE);
          if (nxt_cache_addr == 0)
            begin
              nxt_state = CLEAR;
              nxt_mem_addr = BASE;
              nxt_mem_wr_data = 0;
              nxt_mem_wr_par = { PARWIDTH { 1'b1 } };
            end
        end

      CLEAR: // Clear a memory region (SIZE could be zero!)
        if (mem_addr == BASE+SIZE)
          begin
            nxt_state = IDLE;
          end
        else
          begin
            nxt_mem_req = 1;
            nxt_mem_write = 1;
            nxt_state = CLEAR_WAIT;
            nxt_count = 0;
          end

      CLEAR_WAIT: // Clear memory
        if (mem_gnt || count)
          begin
            nxt_count = count + 1'd1;
            nxt_mem_req = 0;
            if (count == 3)
              begin
                nxt_mem_addr = mem_addr + (1'd1 << LOG2LINESIZE); // Clear 32 bytes at a time...
                nxt_state = CLEAR;
                nxt_mem_req = 0;
              end
          end

      IDLE: // Wait for requests
        begin
          nxt_cache_addr = client_addr;
          nxt_mem_addr = client_addr;
          nxt_cache_wr_tag = client_addr[ADDRWIDTH-1:LOG2CACHESIZE];
          nxt_cache_wr_valid = 1;
          nxt_cache_wr_data = client_wr_data;
          nxt_cache_wr_par = client_wr_par;
          nxt_count = 2;
          nxt_inv_old = invalidate_and_wait;
          if (invalidate_and_wait && !inv_old)
            begin
              nxt_cache_addr = 0;
              nxt_cache_wr_tag = 0;
              nxt_cache_wr_valid = 0;
              nxt_state = RESET;
            end
          else if (!invalidate_and_wait && client_req)
            if (client_write)
              begin // Write request
                if (1) // For now always read before write
// We can't look a client_blast because we only get it after client_gnt.  Duh!
// (in case where there is an arbiter between the cache and the client)
//                if (client_blast /* 1 */) // It looks like qa_sm only asserts blast when he's writing header only, otherwise he writes whole entry
                  begin
                    // He wants to do a masked write... we have to read first.
                    nxt_read_for_write = 1;
                    nxt_state = RD_CACHE;
                  end
                else
                  begin
                    // Write data into cache...
                    // copy it to memory afterwards
                    // nxt_state = WRITE_FIRST;
                    client_gnt = 1;
                    nxt_cache_we = 1;
                    nxt_state = WRITE;
                  end
              end
            else
              begin // Read request
                // Read from cache ram...
                nxt_state = RD_CACHE;
              end
        end

      WRITE_FIRST: // Write data to cache, first word
        begin
          nxt_cache_wr_data = client_wr_data;
          nxt_cache_wr_par = client_wr_par;
          // nxt_cache_addr[LOG2LINESIZE-1:0] = 0;
          // We are comming from RD_FILL.. wrap address back to original client address..
          nxt_cache_addr[LOG2LINESIZE-1:0] = cache_addr[LOG2LINESIZE-1:0] + (1'd1 << LOG2BYTEWIDTH);
          nxt_read_for_write = 0;
          client_gnt = 1;
          nxt_cache_we = 1;
          nxt_count = 2;
          if (client_blast)
            nxt_state = WR_BACK_READ;
          else
            nxt_state = WRITE;
        end

      WRITE: // Write data into cache, other words
        begin
          nxt_count = count - 1'd1;
          nxt_cache_addr[LOG2LINESIZE-1:0] = cache_addr[LOG2LINESIZE-1:0] + (1'd1 << LOG2BYTEWIDTH);
          nxt_cache_wr_tag = cache_addr[ADDRWIDTH-1:LOG2CACHESIZE];
          nxt_cache_wr_valid = 1;
          nxt_cache_wr_data = client_wr_data;
          nxt_cache_wr_par = client_wr_par;
          nxt_cache_we = 1;
          if (!count || client_blast)
            nxt_state = WR_BACK_READ;
        end

      WR_BACK_READ:
        begin
          nxt_cache_addr[LOG2LINESIZE-1:0] = 0;
          nxt_mem_addr[LOG2LINESIZE-1:0] = 0;
          nxt_state = WR_BACK_REQ;
        end

      WR_BACK_REQ: // Write-through: copy data from cache to memory
        begin
          nxt_mem_req = 1;
          nxt_cache_addr[LOG2LINESIZE-1:0] = cache_addr[LOG2LINESIZE-1:0] + (1'd1 << LOG2BYTEWIDTH);
          nxt_mem_write = 1;
          nxt_mem_wr_data = cache_rd_data;
          nxt_mem_wr_par = cache_rd_par;
          nxt_state = WR_BACK_WAIT;
        end

      WR_BACK_WAIT: // Wait for memory write grant
        if (mem_gnt)
          begin
            nxt_mem_req = 0;
            nxt_mem_wr_data = cache_rd_data; // 2nd word to memory
            nxt_mem_wr_par = cache_rd_par; // 2nd word to memory
            nxt_cache_addr[LOG2LINESIZE-1:0] = cache_addr[LOG2LINESIZE-1:0] + (1'd1 << LOG2BYTEWIDTH);
            nxt_count = 2;
            nxt_state = WR_BACK;
          end

      WR_BACK:
        begin
          nxt_count = count - 1'd1;
          nxt_mem_wr_data = cache_rd_data;
          nxt_mem_wr_par = cache_rd_par;
          nxt_cache_addr[LOG2LINESIZE-1:0] = cache_addr[LOG2LINESIZE-1:0] + (1'd1 << LOG2BYTEWIDTH);
          if (!count)
            nxt_state = IDLE;
        end

      RD_CACHE: // Check valid bit and tag
        begin
          nxt_total_count = total_count + 1'd1;
          // Start bursting...
          if (cache_rd_valid && cache_rd_tag == cache_addr[ADDRWIDTH-1:LOG2CACHESIZE])
            begin // Cache line is valid
              nxt_hit_count = hit_count + 1'd1;
              // Send first word to client
              if (read_for_write)
                begin
                  nxt_cache_wr_data = client_wr_data;
                  nxt_cache_wr_par = client_wr_par;
                  nxt_count = 2;
                  nxt_read_for_write = 0;
                  client_gnt = 1;
                  nxt_cache_we = 1;
                  if (client_blast)
                    nxt_state = WR_BACK_READ;
                  else
                    nxt_state = WRITE;
                end
              else
                begin
                  nxt_cache_addr[LOG2LINESIZE-1:0] = cache_addr[LOG2LINESIZE-1:0] + (1'd1 << LOG2BYTEWIDTH);
                  client_gnt = 1;
                  nxt_client_rd_data = cache_rd_data;
                  nxt_client_rd_par = cache_rd_par;
                  nxt_state = READ;
                  nxt_count = 3;
                end
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
          if (!read_for_write)
            client_gnt = 1;
          nxt_client_rd_data = mem_rd_data;
          nxt_cache_wr_data = mem_rd_data;
          nxt_client_rd_par = mem_rd_par;
          nxt_cache_wr_par = mem_rd_par;
          nxt_cache_we = 1;
          // nxt_cache_addr[LOG2LINESIZE-1:0] = 0;
          nxt_cache_wr_valid = 1;
          nxt_count = 2;
          nxt_state = RD_FILL;
        end

      RD_FILL:
        begin
          nxt_count = count - 1'd1;
          nxt_client_rd_data = mem_rd_data;
          nxt_cache_wr_data = mem_rd_data;
          nxt_client_rd_par = mem_rd_par;
          nxt_cache_wr_par = mem_rd_par;
          nxt_cache_we = 1;
          nxt_cache_addr[LOG2LINESIZE-1:0] = cache_addr[LOG2LINESIZE-1:0] + (1'd1 << LOG2BYTEWIDTH);
          nxt_cache_wr_valid = 1;
          if (!count)
            if (read_for_write)
              nxt_state = WRITE_FIRST;
            else
              nxt_state = IDLE;
        end

      READ:
        begin
          // Send second word to client
          nxt_cache_addr[LOG2LINESIZE-1:0] = cache_addr[LOG2LINESIZE-1:0] + (1'd1 << LOG2BYTEWIDTH);
          nxt_client_rd_data = cache_rd_data;
          nxt_client_rd_par = cache_rd_par;
          nxt_count = count - 1'd1;
          if (!count)
            nxt_state = IDLE;
        end
    endcase
  end

`ifdef junk
qcode_mon qcode_mon
  (
  .clk (clk),
  .reset_l (reset_l),
  .client_gnt (client_gnt),
  .client_write (client_write),
  .client_lblast (client_blast),
  .client_addr (client_addr),
  .client_wr_data (client_wr_data),
  .client_rd_data (client_rd_data)
  );
`endif

endmodule
