// Track valid items using a hash table

module tracking
  (
  clk,
  reset_l,

// Input to ping-pong arbiter.. each client
// must not write more than once every other
// cycle.

  track_fifo_we1,
  track_label1,
  track_mark1,

  track_fifo_we2,
  track_label2,
  track_mark2,

//  track_fifo_we,	// Write to FIFO
  track_fifo_full,	// High if FIFO is full...
//  track_label,		// Entry label
//  track_mark,		// High to create entry, low to remove it

  track_bad,		// Set high if there is a tracking problem detected
  track_bad_label,	// Label which is bad
  track_bad_mark,	// Set if tried to add a label which already exists, low if tried to remove a label which doesn't exist
  track_bad_time,
  track_bad_org_time,

  ovf,			// Out of free entries

  count			// No. labels in FIFO
  );

parameter LABELWIDTH = 20;
parameter HTWIDTH = 10;		// Hash table ram address width.  1/2 of ram is hash table, 1/2 of ram is free buckets
parameter EXTRAWIDTH = 32;	// Extra bits for time...

input clk;
input reset_l;
output track_fifo_full;

input track_fifo_we1;
input [LABELWIDTH-1:0] track_label1;
input track_mark1; // 1 = mark, 0 = clear

input track_fifo_we2;
input [LABELWIDTH-1:0] track_label2;
input track_mark2; // 1 = mark, 0 = clear

reg track_fifo_we;
reg [LABELWIDTH-1:0] track_label;
reg track_mark;

//input track_fifo_we;
//input [LABELWIDTH-1:0] track_label;
//input track_mark; // 1 = mark, 0 = clear

output track_bad;
reg track_bad;

output [LABELWIDTH-1:0] track_bad_label;
reg [LABELWIDTH-1:0] track_bad_label;

output [EXTRAWIDTH-1:0] track_bad_time;
reg [EXTRAWIDTH-1:0] track_bad_time;

output [EXTRAWIDTH-1:0] track_bad_org_time;
reg [EXTRAWIDTH-1:0] track_bad_org_time;

output track_bad_mark;
reg track_bad_mark;

output [HTWIDTH:0] count;
reg [HTWIDTH:0] count;

output ovf;
reg ovf;

// Request FIFO...

wire track_fifo_mark;
reg track_fifo_re;
wire [LABELWIDTH-1:0] track_fifo_label;
wire track_fifo_ne;

fifo_sync #(.DATAWIDTH(LABELWIDTH+1)) track_fifo
  (
  .clk (clk),
  .reset_l (reset_l),
  .wr_data ({ track_mark, track_label }),
  .we (track_fifo_we),
  .full (track_fifo_full),
  .ne (track_fifo_ne),
  .rd_data ({ track_fifo_mark, track_fifo_label }),
  .re (track_fifo_re)
  );

// Pointer ram
// Lower half is hash table.  0 means entry empty.
// Upper half is next pointers: one for each item

reg [HTWIDTH:0] next_ram_addr;
reg [HTWIDTH:0] ram_wr_next;
reg next_ram_we;
wire [HTWIDTH:0] ram_rd_next;

ram_sp #(.DATAWIDTH(HTWIDTH+1), .ADDRWIDTH(HTWIDTH+1)) next_ram
  (
  .clk (clk),
  .wr_data (ram_wr_next),
  .addr (next_ram_addr),
  .we (next_ram_we),
  .rd_data (ram_rd_next)
  );

// Item ram

reg [HTWIDTH-1:0] label_ram_addr;
reg label_ram_we;
reg [LABELWIDTH-1:0] ram_wr_label;
wire [LABELWIDTH-1:0] ram_rd_label;
reg [EXTRAWIDTH-1:0] ram_wr_extra;
wire [EXTRAWIDTH-1:0] ram_rd_extra;

ram_sp #(.DATAWIDTH(EXTRAWIDTH+LABELWIDTH), .ADDRWIDTH(HTWIDTH)) label_ram
  (
  .clk (clk),
  .wr_data ({ ram_wr_extra, ram_wr_label }),
  .addr (label_ram_addr),
  .we (label_ram_we),
  .rd_data ({ ram_rd_extra, ram_rd_label })
  );

parameter
  RESET = 0,
  INIT_FREELIST = 1,
  IDLE = 2,
  HASH_WAIT = 3,
  DEL = 4,
  ADD_READ = 5,
  ADD_WAIT = 6,
  ADD = 7,
  BAD = 8,
  CHECK = 9;

reg [3:0] state;

reg [HTWIDTH:0] next_free; // Address of first free item
reg [HTWIDTH:0] prev_addr; // Address of previous accessed pointer
reg [HTWIDTH:0] tmp;

reg [EXTRAWIDTH-1:0] current_time;

always @(posedge clk or negedge reset_l)
  if (!reset_l)
    begin
      next_ram_we <= 0;
      label_ram_we <= 0;
      next_ram_addr <= (1 << HTWIDTH);
      label_ram_addr <= 0;
      ram_wr_next <= 0;
      ram_wr_label <= 0;
      state <= RESET;
      track_fifo_re <= 0;
      track_bad <= 0;
      track_bad_label <= 0;
      track_bad_mark <= 0;
      next_free <= (1 << HTWIDTH);
      prev_addr <= 0;
      tmp <= 0;
      count <= 0;
      ovf <= 0;
      track_bad_time <= 0;
      track_bad_org_time <= 0;
      current_time <= 0;
    end
  else
    begin
      current_time <= current_time + 1'd1;
      track_fifo_re <= 0;
      next_ram_we <= 0;
      label_ram_we <= 0;
      case (state)
        RESET: // Clear hash table...
          begin
            next_ram_we <= 1;
            next_ram_addr <= next_ram_addr - 1'd1;
            if (next_ram_addr == 0)
              state <= INIT_FREELIST;
          end

        INIT_FREELIST: // Initialize free-list...
          begin
            next_ram_we <= 1;
            next_ram_addr <= next_ram_addr - 1'd1;
            ram_wr_next <= next_ram_addr;
            if (next_ram_addr == (1 << HTWIDTH) + 1)
              state <= IDLE;
          end

        IDLE: // Process requests...
          begin
            if (track_fifo_ne)
              begin
                track_bad_label <= track_fifo_label;
                track_bad_mark <= track_fifo_mark;
                track_bad_time <= current_time;
                track_fifo_re <= 1;
                state <= HASH_WAIT;
                next_ram_addr <= { 1'd0, track_fifo_label[HTWIDTH-1:0] }; // Compute hash
              end
          end

        HASH_WAIT: // Waiting for RAM...
          state <= CHECK;

        CHECK: // Did we find the label?
          begin
            if (next_ram_addr[HTWIDTH] && ram_rd_label == track_bad_label)
              begin // Found item
                if (track_bad_mark) // We're supposed to add
                  begin
                    track_bad <= 1;
                    state <= BAD;
                    track_bad_org_time <= ram_rd_extra;
                  end
                else // We're supposed to delete
                  begin
                    tmp <= ram_rd_next;
                    next_free <= next_ram_addr;
                    next_ram_we <= 1;
                    ram_wr_next <= next_free;
                    state <= DEL;
                  end
              end
            else if (ram_rd_next) // Valid pointer
              begin
                prev_addr <= next_ram_addr;
                next_ram_addr <= ram_rd_next;
                label_ram_addr <= ram_rd_next;
                state <= HASH_WAIT;
              end
            else // End of chain.
              if (track_bad_mark) // We're supposed to add
                begin
                  if (!next_free)
                    begin
                      track_bad <= 1;
                      ovf <= 1;
                    end
                  else
                    begin
                      ram_wr_next <= next_free;
                      next_ram_we <= 1;
                      state <= ADD_READ;
                    end
                end
              else // We're supposed to delete
                begin
                  track_bad <= 1;
                  state <= BAD;
                end
          end

        DEL:
          begin
            next_ram_addr <= prev_addr;
            next_ram_we <= 1;
            ram_wr_next <= tmp;
            count <= count - 1'd1;
            state <= IDLE;
          end

        ADD_READ:
          begin
            next_ram_addr <= next_free;
            label_ram_addr <= next_free;
            state <= ADD_WAIT;
          end

        ADD_WAIT:
          state <= ADD;

        ADD:
          begin
            next_free <= ram_rd_next;
            ram_wr_next <= 0;
            ram_wr_label <= track_bad_label;
            ram_wr_extra <= track_bad_time;
            next_ram_we <= 1;
            label_ram_we <= 1;
            count <= count + 1'd1;
            state <= IDLE;
          end

        BAD:
          begin // Just sit here.
            track_fifo_re <= track_fifo_ne && !track_fifo_re;
          end
      endcase
    end

// Ping-pong arbiter.. each client must not write more often than once every other cycle

reg did;
reg did_mark;
reg [LABELWIDTH-1:0] did_label;

always @(posedge clk or negedge reset_l)
  if (!reset_l)
    begin
      track_fifo_we <= 0;
      track_label <= 0;
      track_mark <= 0;
      did <= 0;
      did_mark <= 0;
      did_label <= 0;
    end
  else
    begin
      if (did)
        begin
          track_fifo_we <= 1;
          track_label <= did_label;
          track_mark <= did_mark;
          if (track_fifo_we1)
            begin
              did_label <= track_label1;
              did_mark <= track_mark1;
            end
          else if (track_fifo_we2)
            begin
              did_label <= track_label2;
              did_mark <= track_mark2;
            end
          else
            begin
              did <= 0;
            end
        end
      else if (track_fifo_we1)
        begin
          track_fifo_we <= 1;
          track_label <= track_label1;
          track_mark <= track_mark1;
          did_label <= track_label2;
          did_mark <= track_mark2;
          did <= track_fifo_we2;
        end
      else if (track_fifo_we2)
        begin
          track_fifo_we <= 1;
          track_label <= track_label2;
          track_mark <= track_mark2;
          did_label <= track_label1;
          did_mark <= track_mark1;
          did <= track_fifo_we1;
        end
      else
        begin
          track_fifo_we <= 0;
          did <= 0;
        end
    end

endmodule
