// Instruction cache which reads four words

// Read request happens when address on client_addr changes.
// Response is new address appearing on client_rd_addr along with data on client_rd_data.
// There are four words in client_rd_data with a valid bit in client_valid for each:
//    bit 0 means: word 0 is valid and is the data from address client_rd_addr.
//    bit 1 means: word 1 is valid and is the data from address client_rd_addr + 1.
//    bit 2 means: word 2 is valid and is the data from address client_rd_addr + 2.
//    etc.
// Word 0 is read first, then the rest in succession.  If a cache-line boundary is crossed,
// there will be a gap in the sequence of valid bits going high.

// Valid bits remain unchanged until new data is actually written to
// client_rd_data.

// This gives the client a chance to use the old data in case it changes its
// mind as to what address it wants to read from.

module icache
  (
  reset_l,
  clk,

  client_addr, // byte address
  client_rd_data, // four data words
  client_rd_par, // four parity words
  nxt_client_rd_data,
  nxt_client_rd_par,
  client_valid, // valid bits, one for each word
  nxt_client_valid,
  client_rd_addr,
  nxt_client_rd_addr,

  invalidate_and_wait, // invalidate at rising edge, and then do not fill until low

  mem_req,
  mem_gnt,
  mem_addr,
  mem_rd_data,
  mem_rd_par
  );

`ifdef PRISM

// Prism version of icache feeds only registered data to the RAM
// for better timing.

parameter LOG2BYTEWIDTH = 3; // Log2 bytes per word
parameter LOG2CACHESIZE = 10; // Log2 bytes of cache
parameter ADDRWIDTH = 29; // How many address bits (byte addressing)
parameter PARWIDTH = 8; // How many parity bits

parameter BYTEWIDTH = (1 << LOG2BYTEWIDTH);
parameter DATAWIDTH = (BYTEWIDTH * 8);
parameter LOG2LINESIZE = (2 + LOG2BYTEWIDTH);
parameter TAGWIDTH = (ADDRWIDTH - LOG2CACHESIZE);

input reset_l;
input clk;

input invalidate_and_wait;

input [ADDRWIDTH-1:0] client_addr;

output [(DATAWIDTH*4)-1:0] client_rd_data;
output [(DATAWIDTH*4)-1:0] nxt_client_rd_data;
reg [(DATAWIDTH*4)-1:0] client_rd_data, nxt_client_rd_data;

output [(PARWIDTH*4)-1:0] client_rd_par;
output [(PARWIDTH*4)-1:0] nxt_client_rd_par;
reg [(PARWIDTH*4)-1:0] client_rd_par, nxt_client_rd_par;

output [3:0] client_valid;
output [3:0] nxt_client_valid;
reg [3:0] client_valid, nxt_client_valid;

output [ADDRWIDTH-1:0] client_rd_addr;
output [ADDRWIDTH-1:0] nxt_client_rd_addr;
reg [ADDRWIDTH-1:0] client_rd_addr, nxt_client_rd_addr;

output mem_req;
reg mem_req, nxt_mem_req;
input mem_gnt;
output [ADDRWIDTH-1:0] mem_addr;
reg [ADDRWIDTH-1:0] mem_addr, nxt_mem_addr;
input [DATAWIDTH-1:0] mem_rd_data;
input [PARWIDTH-1:0] mem_rd_par;

// Here is the cache ram

reg [ADDRWIDTH-1:0] cache_addr, nxt_cache_addr;
reg cache_we, nxt_cache_we;
wire [DATAWIDTH-1:0] cache_rd_data;
wire [PARWIDTH-1:0] cache_rd_par;

wire [LOG2CACHESIZE-LOG2BYTEWIDTH-1:0] cache_data_addr = cache_addr[LOG2CACHESIZE-1:LOG2BYTEWIDTH];

// wire [DATAWIDTH+PARWIDTH-1:0] cache_data_wr_data = { mem_rd_par, mem_rd_data };
reg [DATAWIDTH+PARWIDTH-1:0] cache_data_wr_data;

wire [DATAWIDTH+PARWIDTH-1:0] cache_data_rd_data;
assign
  { cache_rd_par, cache_rd_data } = cache_data_rd_data;

ram_sp #(.DATAWIDTH(DATAWIDTH+PARWIDTH), .ADDRWIDTH(LOG2CACHESIZE - LOG2BYTEWIDTH)) cache_data
  (
  .clk (clk),
  .wr_data (cache_data_wr_data),
  .addr (cache_data_addr),
  .we (cache_we),
  .rd_data (cache_data_rd_data)
  );

// Here is the tag ram

reg cache_wr_valid, nxt_cache_wr_valid;
wire [TAGWIDTH-1:0] cache_rd_tag;
wire cache_rd_valid;

wire [TAGWIDTH:0] cache_tags_wr_data = { cache_wr_valid, cache_addr[ADDRWIDTH-1:LOG2CACHESIZE] };
wire [LOG2CACHESIZE-LOG2LINESIZE-1:0] cache_tags_addr = cache_addr[LOG2CACHESIZE-1:LOG2LINESIZE];
wire [TAGWIDTH:0] cache_tags_rd_data;

wire [LOG2CACHESIZE-LOG2LINESIZE-1:0] cache_tags_addr_2 = 0;
wire [TAGWIDTH:0] cache_tags_rd_data_2;


assign
  { cache_rd_valid, cache_rd_tag } = cache_tags_rd_data;

ram_da #(.DATAWIDTH(TAGWIDTH + 1), .ADDRWIDTH(LOG2CACHESIZE - LOG2LINESIZE)) cache_tags
  (
  .clk (clk),

  .wr_data_1 (cache_tags_wr_data),
  .addr_1 (cache_tags_addr),
  .we_1 (cache_we),
  .rd_data_1 (cache_tags_rd_data),

  .wr_data_2 (),
  .addr_2 (cache_tags_addr_2),
  .we_2 (1'b0),
  .rd_data_2 (cache_tags_rd_data_2)
  );

reg [ADDRWIDTH-1:0] old_addr, nxt_old_addr;
reg [ADDRWIDTH-1:0] cur_addr, nxt_cur_addr;

reg [3:0] state, nxt_state;

reg [1:0] count, nxt_count;

reg [1:0] word, nxt_word;

parameter
  RESET = 0,
  CLEAR = 1,
  IDLE = 2,
  READ = 3,
  READ_WAIT = 4,
  RD_FILL_FIRST = 5,
  RD_FILL_REST = 6,
  PAUSE = 7,
  RD_FILL_WAIT = 8
  ;

reg inv_old, nxt_inv_old;

reg [31:0] hit_count, nxt_hit_count;
reg [31:0] total_count, nxt_total_count;

reg new_addr, nxt_new_addr;

always @(posedge clk or negedge reset_l)
  if (!reset_l)
    begin
      new_addr <= 0;
      hit_count <= 0;
      total_count <= 0;
      old_addr <= { ADDRWIDTH { 1'd1 } };
      state <= IDLE;
      client_valid <= 0;
      client_rd_data <= 0;
      client_rd_par <= 0;
      mem_req <= 0;
      mem_addr <= 0;
      cache_addr <= 0;
      cache_we <= 0;
      cache_wr_valid <= 0;
      count <= 0;
      word <= 0;
      cur_addr <= 0;
      inv_old <= 0;
      client_rd_addr <= 0;
      cache_data_wr_data <= 0;
    end
  else
    begin
      cache_data_wr_data = { mem_rd_par, mem_rd_data };
      new_addr <= nxt_new_addr;
      hit_count <= nxt_hit_count;
      total_count <= nxt_total_count;
      state <= nxt_state;
      old_addr <= nxt_old_addr;
      client_valid <= nxt_client_valid;
      mem_req <= nxt_mem_req;
      mem_addr <= nxt_mem_addr;
      client_rd_data <= nxt_client_rd_data;
      client_rd_par <= nxt_client_rd_par;
      cache_addr <= nxt_cache_addr;
      cache_we <= nxt_cache_we;
      cache_wr_valid <= nxt_cache_wr_valid;
      count <= nxt_count;
      word <= nxt_word;
      cur_addr <= nxt_cur_addr;
      inv_old <= nxt_inv_old;
      client_rd_addr <= nxt_client_rd_addr;
    end

always @*
  begin
    nxt_new_addr = new_addr;
    nxt_client_rd_addr = client_rd_addr;
    nxt_total_count = total_count;
    nxt_hit_count = hit_count;
    nxt_cache_addr = cache_addr;
    nxt_cache_we = 0;
    nxt_cache_wr_valid = cache_wr_valid;
    nxt_old_addr = old_addr;
    nxt_cur_addr = cur_addr;
    nxt_state = state;
    nxt_client_valid = client_valid;
    nxt_client_rd_data = client_rd_data;
    nxt_client_rd_par = client_rd_par;
    nxt_mem_req = mem_req;
    nxt_mem_addr = mem_addr;
    nxt_count = count;
    nxt_word = word;
    nxt_inv_old = inv_old;
    case (state)
      RESET: // Clear all valid bits
        begin
          nxt_cache_addr = 0;
          nxt_cache_we = 1;
          nxt_cache_wr_valid = 0;
          nxt_state = CLEAR;
        end

      CLEAR:
        begin
          nxt_cache_we = 1;
          nxt_cache_addr = cache_addr + (1'd1 << LOG2LINESIZE);
          if (cache_addr + (1'd1 << LOG2LINESIZE) == (1'd1 << LOG2CACHESIZE) - (1'd1 << LOG2LINESIZE))
            nxt_state = IDLE;
        end

      IDLE:
        begin
          nxt_inv_old = invalidate_and_wait;
          // Do cache lookup when address is valid and different from old one or
          if (invalidate_and_wait && !inv_old)
            begin
              nxt_state = RESET;
              nxt_new_addr = 0;
              nxt_client_valid = 0;
            end
          else if (!invalidate_and_wait && (client_addr != old_addr || client_valid != 4'hf))
            begin
              nxt_new_addr = 0;
              nxt_old_addr = client_addr;
              nxt_cache_addr = client_addr;
              nxt_cur_addr = client_addr;
              nxt_state = READ_WAIT;
              nxt_word = 0;
            end
        end

      READ_WAIT:
        if (client_addr != old_addr) // Address changed?  Start again.
          begin
            nxt_new_addr = 0;
            nxt_word = 0;
            nxt_old_addr = client_addr;
            nxt_cache_addr = client_addr;
            nxt_cur_addr = client_addr;
            nxt_state = READ_WAIT;
          end
        else
          begin
            nxt_state = READ;
            nxt_cache_addr = cache_addr + (1'd1 << LOG2BYTEWIDTH);
          end

      READ:
        if (client_addr != old_addr) // Address changed?  Start again.
          begin
            nxt_new_addr = 0;
            nxt_word = 0;
            nxt_old_addr = client_addr;
            nxt_cache_addr = client_addr;
            nxt_cur_addr = client_addr;
            nxt_state = READ_WAIT;
          end
        else if (cache_rd_valid && cache_rd_tag == cur_addr[ADDRWIDTH-1:LOG2CACHESIZE]) // Cache entry valid?
          begin
            nxt_total_count = total_count + 1'd1;
            nxt_hit_count = hit_count + 1'd1;
            // It's valid
            if (!new_addr)
              begin
                nxt_new_addr = 1;
                nxt_client_valid = 0;
              end
            nxt_client_valid[word] = 1;
            nxt_client_rd_addr = old_addr; // Address of first word on client_rd_data
            case (word)
              2'd0: nxt_client_rd_data[DATAWIDTH-1:0] = cache_rd_data;
              2'd1: nxt_client_rd_data[(2*DATAWIDTH)-1:DATAWIDTH] = cache_rd_data;
              2'd2: nxt_client_rd_data[(3*DATAWIDTH)-1:(DATAWIDTH*2)] = cache_rd_data;
              2'd3: nxt_client_rd_data[(4*DATAWIDTH)-1:(DATAWIDTH*3)] = cache_rd_data;
            endcase
            case (word)
              2'd0: nxt_client_rd_par[PARWIDTH-1:0] = cache_rd_par;
              2'd1: nxt_client_rd_par[(2*PARWIDTH)-1:PARWIDTH] = cache_rd_par;
              2'd2: nxt_client_rd_par[(3*PARWIDTH)-1:(PARWIDTH*2)] = cache_rd_par;
              2'd3: nxt_client_rd_par[(4*PARWIDTH)-1:(PARWIDTH*3)] = cache_rd_par;
            endcase
            if (word == 3)
              begin
                // We're all done!
                nxt_state = IDLE;
              end
            else
              begin
                // Get next word
                nxt_word = word + 1'd1;
                nxt_cache_addr = cache_addr + (1'd1 << LOG2BYTEWIDTH);
                nxt_cur_addr = cur_addr + (1'd1 << LOG2BYTEWIDTH);
                nxt_state = READ;
              end
          end
        else
          begin
            nxt_total_count = total_count + 1'd1;
            // Not valid: fill
            nxt_mem_req = 1;
// Use this to start filling at beginning of cache line
//            nxt_mem_addr = { cur_addr[ADDRWIDTH-1:LOG2LINESIZE], { LOG2LINESIZE { 1'd0 } } };
// Otherwise use this to start filling at the current address: only useful if we are bypassing the fill to the client.
            nxt_mem_addr = cur_addr;
            nxt_state = RD_FILL_WAIT;
          end

      RD_FILL_WAIT:
        begin
          if (mem_gnt)
            begin
              nxt_mem_req = 0;
              nxt_state = RD_FILL_FIRST;
            end
        end

      RD_FILL_FIRST:
        begin
          nxt_cache_we = 1;
          nxt_cache_wr_valid = 1;
          nxt_cache_addr = mem_addr;
          nxt_count = 2;
          nxt_state = RD_FILL_REST;
          if (nxt_cache_addr == cur_addr) // Bypass fill data to client (take this out if you don't want it)
            begin
              nxt_total_count = total_count + 1'd1;
              // It's valid
              if (!new_addr)
                begin
                  nxt_new_addr = 1;
                  nxt_client_valid = 0;
                end
              nxt_client_valid[word] = 1;
              nxt_client_rd_addr = old_addr; // Address of first word on client_rd_data
              case (word)
                2'd0: nxt_client_rd_data[DATAWIDTH-1:0] = mem_rd_data;
                2'd1: nxt_client_rd_data[(2*DATAWIDTH)-1:DATAWIDTH] = mem_rd_data;
                2'd2: nxt_client_rd_data[(3*DATAWIDTH)-1:(DATAWIDTH*2)] = mem_rd_data;
                2'd3: nxt_client_rd_data[(4*DATAWIDTH)-1:(DATAWIDTH*3)] = mem_rd_data;
              endcase
              case (word)
                2'd0: nxt_client_rd_par[PARWIDTH-1:0] = mem_rd_par;
                2'd1: nxt_client_rd_par[(2*PARWIDTH)-1:PARWIDTH] = mem_rd_par;
                2'd2: nxt_client_rd_par[(3*PARWIDTH)-1:(PARWIDTH*2)] = mem_rd_par;
                2'd3: nxt_client_rd_par[(4*PARWIDTH)-1:(PARWIDTH*3)] = mem_rd_par;
              endcase
              if (word != 3)
                begin
                  nxt_word = word + 1'd1;
                  nxt_cur_addr = cur_addr + (1'd1 << LOG2BYTEWIDTH);
                end
            end
        end

      RD_FILL_REST:
        begin
          nxt_count = count - 1'd1;
          nxt_cache_we = 1;
          nxt_cache_wr_valid = 1;
          nxt_cache_addr[LOG2LINESIZE-1:0] = cache_addr[LOG2LINESIZE-1:0] + (1'd1 << LOG2BYTEWIDTH);
          if (!count)
            nxt_state = PAUSE;
          if (nxt_cache_addr == cur_addr && client_valid != 4'hf) // Bypass fill data to client (take out if you don't want it)
            begin
              nxt_total_count = total_count + 1'd1;
              // It's valid
              if (!new_addr)
                begin
                  nxt_new_addr = 1;
                  nxt_client_valid = 0;
                end
              nxt_client_valid[word] = 1;
              nxt_client_rd_addr = old_addr; // Address of first word on client_rd_data
              case (word)
                2'd0: nxt_client_rd_data[DATAWIDTH-1:0] = mem_rd_data;
                2'd1: nxt_client_rd_data[(2*DATAWIDTH)-1:DATAWIDTH] = mem_rd_data;
                2'd2: nxt_client_rd_data[(3*DATAWIDTH)-1:(DATAWIDTH*2)] = mem_rd_data;
                2'd3: nxt_client_rd_data[(4*DATAWIDTH)-1:(DATAWIDTH*3)] = mem_rd_data;
              endcase
              case (word)
                2'd0: nxt_client_rd_par[PARWIDTH-1:0] = mem_rd_par;
                2'd1: nxt_client_rd_par[(2*PARWIDTH)-1:PARWIDTH] = mem_rd_par;
                2'd2: nxt_client_rd_par[(3*PARWIDTH)-1:(PARWIDTH*2)] = mem_rd_par;
                2'd3: nxt_client_rd_par[(4*PARWIDTH)-1:(PARWIDTH*3)] = mem_rd_par;
              endcase
              if (word != 3)
                begin
                  nxt_word = word + 1'd1;
                  nxt_cur_addr = cur_addr + (1'd1 << LOG2BYTEWIDTH);
                end
            end
        end

      PAUSE:
        begin
          if (nxt_client_valid != 4'hf)
            begin
              nxt_state = READ_WAIT;
              nxt_cache_addr = nxt_cur_addr;
            end
          else
            nxt_state = IDLE;
        end
    endcase
  end

`else

// FPGA version of icache feeds combinatorial logic to the RAM for lower latency

parameter LOG2BYTEWIDTH = 3; // Log2 bytes per word
parameter LOG2CACHESIZE = 10; // Log2 bytes of cache
parameter ADDRWIDTH = 29; // How many address bits (byte addressing)
parameter PARWIDTH = 8; // How many parity bits

parameter BYTEWIDTH = (1 << LOG2BYTEWIDTH);
parameter DATAWIDTH = (BYTEWIDTH * 8);
parameter LOG2LINESIZE = (2 + LOG2BYTEWIDTH);
parameter TAGWIDTH = (ADDRWIDTH - LOG2CACHESIZE);

input reset_l;
input clk;

input invalidate_and_wait;

input [ADDRWIDTH-1:0] client_addr;

output [(DATAWIDTH*4)-1:0] client_rd_data;
output [(DATAWIDTH*4)-1:0] nxt_client_rd_data;
reg [(DATAWIDTH*4)-1:0] client_rd_data, nxt_client_rd_data;

output [(PARWIDTH*4)-1:0] client_rd_par;
output [(PARWIDTH*4)-1:0] nxt_client_rd_par;
reg [(PARWIDTH*4)-1:0] client_rd_par, nxt_client_rd_par;

output [3:0] client_valid;
output [3:0] nxt_client_valid;
reg [3:0] client_valid, nxt_client_valid;

output [ADDRWIDTH-1:0] client_rd_addr;
output [ADDRWIDTH-1:0] nxt_client_rd_addr;
reg [ADDRWIDTH-1:0] client_rd_addr, nxt_client_rd_addr;

output mem_req;
reg mem_req, nxt_mem_req;
input mem_gnt;
output [ADDRWIDTH-1:0] mem_addr;
reg [ADDRWIDTH-1:0] mem_addr, nxt_mem_addr;
input [DATAWIDTH-1:0] mem_rd_data;
input [PARWIDTH-1:0] mem_rd_par;

// Here is the cache ram

reg [ADDRWIDTH-1:0] cache_addr, nxt_cache_addr;
reg cache_we, nxt_cache_we;
wire [DATAWIDTH-1:0] cache_rd_data;
wire [PARWIDTH-1:0] cache_rd_par;

wire [LOG2CACHESIZE-LOG2BYTEWIDTH-1:0] cache_data_addr = nxt_cache_addr[LOG2CACHESIZE-1:LOG2BYTEWIDTH];

wire [DATAWIDTH+PARWIDTH-1:0] cache_data_wr_data = { mem_rd_par, mem_rd_data };
wire [DATAWIDTH+PARWIDTH-1:0] cache_data_rd_data;
assign
  { cache_rd_par, cache_rd_data } = cache_data_rd_data;

ram_sp #(.DATAWIDTH(DATAWIDTH+PARWIDTH), .ADDRWIDTH(LOG2CACHESIZE - LOG2BYTEWIDTH)) cache_data
  (
  .clk (clk),
  .wr_data (cache_data_wr_data),
  .addr (cache_data_addr),
  .we (nxt_cache_we),
  .rd_data (cache_data_rd_data)
  );

// Here is the tag ram

reg cache_wr_valid, nxt_cache_wr_valid;
wire [TAGWIDTH-1:0] cache_rd_tag;
wire cache_rd_valid;

wire [TAGWIDTH:0] cache_tags_wr_data = { nxt_cache_wr_valid, nxt_cache_addr[ADDRWIDTH-1:LOG2CACHESIZE] };
wire [LOG2CACHESIZE-LOG2LINESIZE-1:0] cache_tags_addr = nxt_cache_addr[LOG2CACHESIZE-1:LOG2LINESIZE];
wire [TAGWIDTH:0] cache_tags_rd_data;

wire [LOG2CACHESIZE-LOG2LINESIZE-1:0] cache_tags_addr_2 = 0;
wire [TAGWIDTH:0] cache_tags_rd_data_2;


assign
  { cache_rd_valid, cache_rd_tag } = cache_tags_rd_data;

ram_da #(.DATAWIDTH(TAGWIDTH + 1), .ADDRWIDTH(LOG2CACHESIZE - LOG2LINESIZE)) cache_tags
  (
  .clk (clk),

  .wr_data_1 (cache_tags_wr_data),
  .addr_1 (cache_tags_addr),
  .we_1 (nxt_cache_we),
  .rd_data_1 (cache_tags_rd_data),

  .wr_data_2 (),
  .addr_2 (cache_tags_addr_2),
  .we_2 (1'b0),
  .rd_data_2 (cache_tags_rd_data_2)
  );

reg [ADDRWIDTH-1:0] old_addr, nxt_old_addr;
reg [ADDRWIDTH-1:0] cur_addr, nxt_cur_addr;

reg [2:0] state, nxt_state;

reg [1:0] count, nxt_count;

reg [1:0] word, nxt_word;

parameter
  RESET = 0,
  CLEAR = 1,
  IDLE = 2,
  READ = 3,
  RD_FILL_WAIT = 4,
  RD_FILL_FIRST = 5,
  RD_FILL_REST = 6,
  PAUSE = 7
  ;

reg inv_old, nxt_inv_old;

reg [31:0] hit_count, nxt_hit_count;
reg [31:0] total_count, nxt_total_count;

reg new_addr, nxt_new_addr;

always @(posedge clk or negedge reset_l)
  if (!reset_l)
    begin
      new_addr <= 0;
      hit_count <= 0;
      total_count <= 0;
      old_addr <= { ADDRWIDTH { 1'd1 } };
      state <= IDLE;
      client_valid <= 0;
      client_rd_data <= 0;
      client_rd_par <= 0;
      mem_req <= 0;
      mem_addr <= 0;
      cache_addr <= 0;
      cache_we <= 0;
      cache_wr_valid <= 0;
      count <= 0;
      word <= 0;
      cur_addr <= 0;
      inv_old <= 0;
      client_rd_addr <= 0;
    end
  else
    begin
      new_addr <= nxt_new_addr;
      hit_count <= nxt_hit_count;
      total_count <= nxt_total_count;
      state <= nxt_state;
      old_addr <= nxt_old_addr;
      client_valid <= nxt_client_valid;
      mem_req <= nxt_mem_req;
      mem_addr <= nxt_mem_addr;
      client_rd_data <= nxt_client_rd_data;
      client_rd_par <= nxt_client_rd_par;
      cache_addr <= nxt_cache_addr;
      cache_we <= nxt_cache_we;
      cache_wr_valid <= nxt_cache_wr_valid;
      count <= nxt_count;
      word <= nxt_word;
      cur_addr <= nxt_cur_addr;
      inv_old <= nxt_inv_old;
      client_rd_addr <= nxt_client_rd_addr;
    end

always @*
  begin
    nxt_new_addr = new_addr;
    nxt_client_rd_addr = client_rd_addr;
    nxt_total_count = total_count;
    nxt_hit_count = hit_count;
    nxt_cache_addr = cache_addr;
    nxt_cache_we = 0;
    nxt_cache_wr_valid = cache_wr_valid;
    nxt_old_addr = old_addr;
    nxt_cur_addr = cur_addr;
    nxt_state = state;
    nxt_client_valid = client_valid;
    nxt_client_rd_data = client_rd_data;
    nxt_client_rd_par = client_rd_par;
    nxt_mem_req = mem_req;
    nxt_mem_addr = mem_addr;
    nxt_count = count;
    nxt_word = word;
    nxt_inv_old = inv_old;
    case (state)
      RESET: // Clear all valid bits
        begin
          nxt_cache_addr = 0;
          nxt_cache_we = 1;
          nxt_cache_wr_valid = 0;
          nxt_state = CLEAR;
        end

      CLEAR:
        begin
          nxt_cache_we = 1;
          nxt_cache_addr = cache_addr + (1'd1 << LOG2LINESIZE);
          if (cache_addr + (1'd1 << LOG2LINESIZE) == (1'd1 << LOG2CACHESIZE) - (1'd1 << LOG2LINESIZE))
            nxt_state = IDLE;
        end

      IDLE:
        begin
          nxt_inv_old = invalidate_and_wait;
          // Do cache lookup when address is valid and different from old one or
          if (invalidate_and_wait && !inv_old)
            begin
              nxt_state = RESET;
              nxt_new_addr = 0;
              nxt_client_valid = 0;
            end
          else if (!invalidate_and_wait && (client_addr != old_addr || client_valid != 4'hf))
            begin
              nxt_new_addr = 0;
              nxt_old_addr = client_addr;
              nxt_cache_addr = client_addr;
              nxt_cur_addr = client_addr;
              nxt_state = READ;
              nxt_word = 0;
            end
        end

      READ:
        if (client_addr != old_addr) // Address changed?  Start again.
          begin
            nxt_new_addr = 0;
            nxt_word = 0;
            nxt_old_addr = client_addr;
            nxt_cache_addr = client_addr;
            nxt_cur_addr = client_addr;
            nxt_state = READ;
          end
        else if (cache_rd_valid && cache_rd_tag == cur_addr[ADDRWIDTH-1:LOG2CACHESIZE]) // Cache entry valid?
          begin
            nxt_total_count = total_count + 1'd1;
            nxt_hit_count = hit_count + 1'd1;
            // It's valid
            if (!new_addr)
              begin
                nxt_new_addr = 1;
                nxt_client_valid = 0;
              end
            nxt_client_valid[word] = 1;
            nxt_client_rd_addr = old_addr; // Address of first word on client_rd_data
            case (word)
              2'd0: nxt_client_rd_data[DATAWIDTH-1:0] = cache_rd_data;
              2'd1: nxt_client_rd_data[(2*DATAWIDTH)-1:DATAWIDTH] = cache_rd_data;
              2'd2: nxt_client_rd_data[(3*DATAWIDTH)-1:(DATAWIDTH*2)] = cache_rd_data;
              2'd3: nxt_client_rd_data[(4*DATAWIDTH)-1:(DATAWIDTH*3)] = cache_rd_data;
            endcase
            case (word)
              2'd0: nxt_client_rd_par[PARWIDTH-1:0] = cache_rd_par;
              2'd1: nxt_client_rd_par[(2*PARWIDTH)-1:PARWIDTH] = cache_rd_par;
              2'd2: nxt_client_rd_par[(3*PARWIDTH)-1:(PARWIDTH*2)] = cache_rd_par;
              2'd3: nxt_client_rd_par[(4*PARWIDTH)-1:(PARWIDTH*3)] = cache_rd_par;
            endcase
            if (word == 3)
              begin
                // We're all done!
                nxt_state = IDLE;
              end
            else
              begin
                // Get next word
                nxt_word = word + 1'd1;
                nxt_cache_addr = cur_addr + (1'd1 << LOG2BYTEWIDTH);
                nxt_cur_addr = cur_addr + (1'd1 << LOG2BYTEWIDTH);
                nxt_state = READ;
              end
          end
        else
          begin
            nxt_total_count = total_count + 1'd1;
            // Not valid: fill
            nxt_mem_req = 1;
// Use this to start filling at beginning of cache line
//            nxt_mem_addr = { cur_addr[ADDRWIDTH-1:LOG2LINESIZE], { LOG2LINESIZE { 1'd0 } } };
// Otherwise use this to start filling at the current address: only useful if we are bypassing the fill to the client.
            nxt_mem_addr = cur_addr;
            nxt_state = RD_FILL_WAIT;
          end

      RD_FILL_WAIT:
        begin
          if (mem_gnt)
            begin
              nxt_mem_req = 0;
              nxt_state = RD_FILL_FIRST;
            end
        end

      RD_FILL_FIRST:
        begin
          nxt_cache_we = 1;
          nxt_cache_wr_valid = 1;
          nxt_cache_addr = mem_addr;
          nxt_count = 2;
          nxt_state = RD_FILL_REST;
          if (nxt_cache_addr == cur_addr) // Bypass fill data to client (take this out if you don't want it)
            begin
              nxt_total_count = total_count + 1'd1;
              // It's valid
              if (!new_addr)
                begin
                  nxt_new_addr = 1;
                  nxt_client_valid = 0;
                end
              nxt_client_valid[word] = 1;
              nxt_client_rd_addr = old_addr; // Address of first word on client_rd_data
              case (word)
                2'd0: nxt_client_rd_data[DATAWIDTH-1:0] = mem_rd_data;
                2'd1: nxt_client_rd_data[(2*DATAWIDTH)-1:DATAWIDTH] = mem_rd_data;
                2'd2: nxt_client_rd_data[(3*DATAWIDTH)-1:(DATAWIDTH*2)] = mem_rd_data;
                2'd3: nxt_client_rd_data[(4*DATAWIDTH)-1:(DATAWIDTH*3)] = mem_rd_data;
              endcase
              case (word)
                2'd0: nxt_client_rd_par[PARWIDTH-1:0] = mem_rd_par;
                2'd1: nxt_client_rd_par[(2*PARWIDTH)-1:PARWIDTH] = mem_rd_par;
                2'd2: nxt_client_rd_par[(3*PARWIDTH)-1:(PARWIDTH*2)] = mem_rd_par;
                2'd3: nxt_client_rd_par[(4*PARWIDTH)-1:(PARWIDTH*3)] = mem_rd_par;
              endcase
              if (word != 3)
                begin
                  nxt_word = word + 1'd1;
                  nxt_cur_addr = cur_addr + (1'd1 << LOG2BYTEWIDTH);
                end
            end
        end

      RD_FILL_REST:
        begin
          nxt_count = count - 1'd1;
          nxt_cache_we = 1;
          nxt_cache_wr_valid = 1;
          nxt_cache_addr[LOG2LINESIZE-1:0] = cache_addr[LOG2LINESIZE-1:0] + (1'd1 << LOG2BYTEWIDTH);
          if (!count)
            nxt_state = PAUSE;
          if (nxt_cache_addr == cur_addr && client_valid != 4'hf) // Bypass fill data to client (take out if you don't want it)
            begin
              nxt_total_count = total_count + 1'd1;
              // It's valid
              if (!new_addr)
                begin
                  nxt_new_addr = 1;
                  nxt_client_valid = 0;
                end
              nxt_client_valid[word] = 1;
              nxt_client_rd_addr = old_addr; // Address of first word on client_rd_data
              case (word)
                2'd0: nxt_client_rd_data[DATAWIDTH-1:0] = mem_rd_data;
                2'd1: nxt_client_rd_data[(2*DATAWIDTH)-1:DATAWIDTH] = mem_rd_data;
                2'd2: nxt_client_rd_data[(3*DATAWIDTH)-1:(DATAWIDTH*2)] = mem_rd_data;
                2'd3: nxt_client_rd_data[(4*DATAWIDTH)-1:(DATAWIDTH*3)] = mem_rd_data;
              endcase
              case (word)
                2'd0: nxt_client_rd_par[PARWIDTH-1:0] = mem_rd_par;
                2'd1: nxt_client_rd_par[(2*PARWIDTH)-1:PARWIDTH] = mem_rd_par;
                2'd2: nxt_client_rd_par[(3*PARWIDTH)-1:(PARWIDTH*2)] = mem_rd_par;
                2'd3: nxt_client_rd_par[(4*PARWIDTH)-1:(PARWIDTH*3)] = mem_rd_par;
              endcase
              if (word != 3)
                begin
                  nxt_word = word + 1'd1;
                  nxt_cur_addr = cur_addr + (1'd1 << LOG2BYTEWIDTH);
                end
            end
        end

      PAUSE:
        begin
          if (nxt_client_valid != 4'hf)
            begin
              nxt_state = READ;
              nxt_cache_addr = nxt_cur_addr;
            end
          else
            nxt_state = IDLE;
        end
    endcase
  end

`endif

endmodule
