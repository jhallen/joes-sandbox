// Simple hash table

module hash
  (
  clk,
  reset_l,

  key,		// The key
  value,	// The value associated with key
  add,		// Set to add new entry
  update,	// Set to update existing entry
            // If both add and update are set, then
            // entry is added if it doesn't exist or updated
            // if it does.
  del,		// Set to delete
  req,		// High to request access

  ack,		// High for access granted
  found,	// Set if entry was found during search
  found_value,	// Value associated with found entry
  ovf,		// Could not add because hash table is full

  count		// No. entries in table
  );

parameter KEYWIDTH = 20;
parameter HTWIDTH = 10;		// Hash table ram address width.  1/2 of ram is hash table, 1/2 of ram is free buckets
parameter VALUEWIDTH = 2;	// Extra bits for time...

input clk;
input reset_l;

input [KEYWIDTH-1:0] key;
input [VALUEWIDTH-1:0] value;
input add;
input update;
input del;
input req;

output ack;
reg ack;

output found;
reg found;

output [VALUEWIDTH-1:0] found_value;
reg [VALUEWIDTH-1:0] found_value;

output ovf;
reg ovf;

output [HTWIDTH:0] count;
reg [HTWIDTH:0] count;

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
reg [KEYWIDTH-1:0] ram_wr_label;
wire [KEYWIDTH-1:0] ram_rd_label;
reg [VALUEWIDTH-1:0] ram_wr_extra;
wire [VALUEWIDTH-1:0] ram_rd_extra;

ram_sp #(.DATAWIDTH(VALUEWIDTH+KEYWIDTH), .ADDRWIDTH(HTWIDTH)) label_ram
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
  CHECK = 9;

reg [3:0] state;

reg [HTWIDTH:0] next_free; // Address of first free item
reg [HTWIDTH:0] prev_addr; // Address of previous accessed pointer
reg [HTWIDTH:0] tmp;

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
      found <= 0;
      found_value <= 0;
      next_free <= (1 << HTWIDTH);
      prev_addr <= 0;
      tmp <= 0;
      count <= 0;
      ovf <= 0;
    end
  else
    begin
      next_ram_we <= 0;
      label_ram_we <= 0;
      ack <= 0;
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
          if (req && !ack)
            begin
              state <= HASH_WAIT;
              next_ram_addr <= { 1'd0, key[HTWIDTH-1:0] }; // Compute hash of key
              ram_wr_label <= key;
              ram_wr_extra <= value;
            end

        HASH_WAIT: // Waiting for RAM...
          state <= CHECK;

        CHECK: // Did we find the label?
          begin
            if (next_ram_addr[HTWIDTH] && ram_rd_label == key)
              begin // Found item
                found <= 1;
                found_value <= ram_rd_extra;
                ack <= 1;
                if (add || update) // We're supposed to add
                  begin
                    state <= IDLE;
                    if (update) // Replace value
                      label_ram_we <= 1;
                  end
                else if (del) // We're supposed to delete
                  begin
                    tmp <= ram_rd_next;
                    next_free <= next_ram_addr;
                    next_ram_we <= 1;
                    ram_wr_next <= next_free;
                    state <= DEL;
                  end
                else
                  begin // We're supposed to just read
                    state <= IDLE;
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
              begin
                found <= 0;
                ack <= 1;
                if (add) // We're supposed to add
                  begin
                    if (!next_free)
                      begin // Table is full
                        state <= IDLE;
                        ovf <= 1;
                      end
                    else
                      begin // Add
                        ram_wr_next <= next_free;
                        next_ram_we <= 1;
                        state <= ADD_READ;
                      end
                  end
                else // We're supposed to delete, read or update
                  begin
                    state <= IDLE;
                  end
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
            next_ram_we <= 1;
            label_ram_we <= 1;
            count <= count + 1'd1;
            state <= IDLE;
          end
      endcase
    end

endmodule
