// Buflet allocation bitmap

module buflet_map
  (
  reset_l,
  clk,

  free_req,
  free_gnt,
  free_page,
  free_who,

  alloc_req,
  alloc_gnt,
  alloc_page,

  double_free_error,
  double_free_page,
  double_free_who,

  count
  );

input reset_l;
input clk;

input free_req;

output free_gnt;
reg free_gnt, nxt_free_gnt;

input [14:0] free_page;

input [3:0] free_who;

input alloc_req;

output alloc_gnt;
reg alloc_gnt, nxt_alloc_gnt;

output [14:0] alloc_page;
reg [14:0] alloc_page, nxt_alloc_page;

output double_free_error;
reg double_free_error, nxt_double_free_error;

output [14:0] double_free_page;
reg [14:0] double_free_page, nxt_double_free_page;

output [3:0] double_free_who;
reg [3:0] double_free_who, nxt_double_free_who;

output [15:0] count;
reg [15:0] count, nxt_count;

// Buflet bitmap.
// 0 means allocated, 1 means free

// Top-level index
reg [31:0] level_1, nxt_level_1;

// 2nd level index
reg [31:0] level_2_wr_data, nxt_level_2_wr_data;
reg [4:0] level_2_addr, nxt_level_2_addr;
reg level_2_we, nxt_level_2_we;
wire [31:0] level_2_rd_data;

ram_sp #(.DATAWIDTH(32), .ADDRWIDTH(5)) level_2
  (
  .clk (clk),
  .wr_data (nxt_level_2_wr_data),
  .addr (nxt_level_2_addr),
  .we (nxt_level_2_we),
  .rd_data (level_2_rd_data)
  );

// Free-map
reg [31:0] level_3_wr_data, nxt_level_3_wr_data;
reg [9:0] level_3_addr, nxt_level_3_addr;
reg level_3_we, nxt_level_3_we;
wire [31:0] level_3_rd_data;

ram_sp #(.DATAWIDTH(32), .ADDRWIDTH(10)) level_3
  (
  .clk (clk),
  .wr_data (nxt_level_3_wr_data),
  .addr (nxt_level_3_addr),
  .we (nxt_level_3_we),
  .rd_data (level_3_rd_data)
  );

parameter
  RESET = 0,
  RESET_1 = 1,
  IDLE = 2,
  FREE = 3,
  ALLOC_3_ADDR = 4,
  ALLOC_3_RD = 5,
  ALLOC = 6,
  UPDATE_2 = 7,
  UPDATE_1 = 8,
  UPDATE_1a = 9,
  UPDATE_0 = 10;

reg [3:0] state, nxt_state;

reg [3:0] who, nxt_who;
reg [14:0] page, nxt_page;

// Convert 1-hot to binary

reg [31:0] abit, nxt_abit; // Input
reg [4:0] idx; // Output

always @(*)
  casex (abit) // synthesis parallel_case full_case
    32'bxxxx_xxxx_xxxx_xxxx_xxxx_xxxx_xxxx_xxx1: idx <= 0;
    32'bxxxx_xxxx_xxxx_xxxx_xxxx_xxxx_xxxx_xx1x: idx <= 1;
    32'bxxxx_xxxx_xxxx_xxxx_xxxx_xxxx_xxxx_x1xx: idx <= 2;
    32'bxxxx_xxxx_xxxx_xxxx_xxxx_xxxx_xxxx_1xxx: idx <= 3;

    32'bxxxx_xxxx_xxxx_xxxx_xxxx_xxxx_xxx1_xxxx: idx <= 4;
    32'bxxxx_xxxx_xxxx_xxxx_xxxx_xxxx_xx1x_xxxx: idx <= 5;
    32'bxxxx_xxxx_xxxx_xxxx_xxxx_xxxx_x1xx_xxxx: idx <= 6;
    32'bxxxx_xxxx_xxxx_xxxx_xxxx_xxxx_1xxx_xxxx: idx <= 7;

    32'bxxxx_xxxx_xxxx_xxxx_xxxx_xxx1_xxxx_xxxx: idx <= 8;
    32'bxxxx_xxxx_xxxx_xxxx_xxxx_xx1x_xxxx_xxxx: idx <= 9;
    32'bxxxx_xxxx_xxxx_xxxx_xxxx_x1xx_xxxx_xxxx: idx <= 10;
    32'bxxxx_xxxx_xxxx_xxxx_xxxx_1xxx_xxxx_xxxx: idx <= 11;

    32'bxxxx_xxxx_xxxx_xxxx_xxx1_xxxx_xxxx_xxxx: idx <= 12;
    32'bxxxx_xxxx_xxxx_xxxx_xx1x_xxxx_xxxx_xxxx: idx <= 13;
    32'bxxxx_xxxx_xxxx_xxxx_x1xx_xxxx_xxxx_xxxx: idx <= 14;
    32'bxxxx_xxxx_xxxx_xxxx_1xxx_xxxx_xxxx_xxxx: idx <= 15;

    32'bxxxx_xxxx_xxxx_xxx1_xxxx_xxxx_xxxx_xxxx: idx <= 16;
    32'bxxxx_xxxx_xxxx_xx1x_xxxx_xxxx_xxxx_xxxx: idx <= 17;
    32'bxxxx_xxxx_xxxx_x1xx_xxxx_xxxx_xxxx_xxxx: idx <= 18;
    32'bxxxx_xxxx_xxxx_1xxx_xxxx_xxxx_xxxx_xxxx: idx <= 19;

    32'bxxxx_xxxx_xxx1_xxxx_xxxx_xxxx_xxxx_xxxx: idx <= 20;
    32'bxxxx_xxxx_xx1x_xxxx_xxxx_xxxx_xxxx_xxxx: idx <= 21;
    32'bxxxx_xxxx_x1xx_xxxx_xxxx_xxxx_xxxx_xxxx: idx <= 22;
    32'bxxxx_xxxx_1xxx_xxxx_xxxx_xxxx_xxxx_xxxx: idx <= 23;

    32'bxxxx_xxx1_xxxx_xxxx_xxxx_xxxx_xxxx_xxxx: idx <= 24;
    32'bxxxx_xx1x_xxxx_xxxx_xxxx_xxxx_xxxx_xxxx: idx <= 25;
    32'bxxxx_x1xx_xxxx_xxxx_xxxx_xxxx_xxxx_xxxx: idx <= 26;
    32'bxxxx_1xxx_xxxx_xxxx_xxxx_xxxx_xxxx_xxxx: idx <= 27;

    32'bxxx1_xxxx_xxxx_xxxx_xxxx_xxxx_xxxx_xxxx: idx <= 28;
    32'bxx1x_xxxx_xxxx_xxxx_xxxx_xxxx_xxxx_xxxx: idx <= 29;
    32'bx1xx_xxxx_xxxx_xxxx_xxxx_xxxx_xxxx_xxxx: idx <= 30;
    32'b1xxx_xxxx_xxxx_xxxx_xxxx_xxxx_xxxx_xxxx: idx <= 31;
  endcase

always @(posedge clk or negedge reset_l)
  if (!reset_l)
    begin
      // Mark all bits from 3 - 31 as free
      // Mark all bits 0 - 2 as allocated
      // (multiply aobve bit numbers by 1024 to get page number)
      level_1 <= 32'hffff_fff8;

      count <= 32768 - (3 * 1024); // 29696 buflets

      level_2_wr_data <= 32'hffff_ffff;
      level_2_addr <= 0;
      level_2_we <= 0;

      level_3_wr_data <= 32'hffff_ffff;
      level_3_addr <= 0;
      level_3_we <= 0;

      state <= RESET;

      free_gnt <= 0;
      alloc_gnt <= 0;
      alloc_page <= 0;

      double_free_error <= 0;
      double_free_who <= 0;
      double_free_page <= 0;
      who <= 0;
      page <= 0;

      abit <= 0;
    end
  else
    begin
      level_1 <= nxt_level_1;

      count <= nxt_count;

      level_2_wr_data <= nxt_level_2_wr_data;
      level_2_addr <= nxt_level_2_addr;
      level_2_we <= nxt_level_2_we;

      level_3_wr_data <= nxt_level_3_wr_data;
      level_3_addr <= nxt_level_3_addr;
      level_3_we <= nxt_level_3_we;

      state <= nxt_state;

      free_gnt <= nxt_free_gnt;
      alloc_gnt <= nxt_alloc_gnt;
      alloc_page <= nxt_alloc_page;

      double_free_error <= nxt_double_free_error;
      double_free_who <= nxt_double_free_who;
      double_free_page <= nxt_double_free_page;
      who <= nxt_who;
      page <= nxt_page;

      abit <= nxt_abit;
    end


always @(*)
  begin
    nxt_level_1 <= level_1;
    nxt_count <= count;
    nxt_level_2_wr_data <= level_2_wr_data;
    nxt_level_2_addr <= level_2_addr;
    nxt_level_3_wr_data <= level_3_wr_data;
    nxt_level_3_addr <= level_3_addr;
    nxt_state <= state;
    nxt_alloc_page <= alloc_page;
    nxt_double_free_error <= double_free_error;
    nxt_double_free_who <= double_free_who;
    nxt_double_free_page <= double_free_page;
    nxt_who <= who;
    nxt_page <= page;

    nxt_level_2_we <= 0;
    nxt_level_3_we <= 0;
    nxt_alloc_gnt <= 0;
    nxt_free_gnt <= 0;

    nxt_abit <= (level_1 & -level_1); // Get ready...

    case (state)
      RESET:
        begin
          // Mark all bits from 03 - 1F as free
          // Mark all bits from 00 - 02 as allocated
          // (multiply above addresses by 1024 to get page number)
          nxt_level_2_we <= 1;
          nxt_level_2_addr <= level_2_addr - 1'd1;
          if (level_2_addr == 3)
            nxt_level_2_wr_data <= 0;
          if (level_2_addr == 1)
            nxt_state <= RESET_1;
        end

      RESET_1:
        begin
          // Mark all bits from 060 - 3FF as free
          // Mark all bits from 000 - 05F as allocated
          // (multiply above addresses by 32 to get page number)
          nxt_level_3_we <= 1;
          nxt_level_3_addr <= level_3_addr - 1'd1;
          if (level_3_addr == 10'h60)
            nxt_level_3_wr_data <= 0;
          if (level_3_addr == 1)
            nxt_state <= IDLE;
          nxt_level_2_addr <= idx;
        end

      IDLE:
        begin
          if (free_req)
            begin
              nxt_level_3_addr <= free_page[14:5];
              nxt_level_3_wr_data <= (32'd1 << free_page[4:0]);

              nxt_level_2_addr <= free_page[14:10];
              nxt_level_2_wr_data <= (32'd1 << free_page[9:5]);

              nxt_level_1 <= level_1 | (32'd1 << free_page[14:10]);

              nxt_free_gnt <= 1;

              nxt_who <= free_who;
              nxt_page <= free_page;

              nxt_state <= FREE;
            end
          else if (alloc_req && count != 0)
            begin
              // Search for a free page
              // We've already scanned level_1...
              // We've already set level_2_addr to idx...
              // (everyone who jumps to IDLE should do this)

              nxt_abit <= (level_2_rd_data & -level_2_rd_data);
              nxt_state <= ALLOC_3_ADDR;
            end
        end

      FREE: // New level_1
        begin
          if ((level_3_rd_data & level_3_wr_data) != 0 && !double_free_error)
            begin
              nxt_double_free_error <= 1;
              nxt_double_free_page <= page;
              nxt_double_free_who <= who;
            end
          else
            begin
              nxt_count <= count + 1'd1;
            end
          nxt_level_3_wr_data <= (level_3_wr_data | level_3_rd_data);
          nxt_level_2_wr_data <= (level_2_wr_data | level_2_rd_data);
          nxt_level_3_we <= 1;
          nxt_level_2_we <= 1;

          nxt_state <= UPDATE_0;
        end

      ALLOC_3_ADDR:
        begin
          nxt_level_3_addr <= { level_2_addr, idx };
          nxt_state <= ALLOC_3_RD;
        end

      ALLOC_3_RD:
        begin
          nxt_abit <= (level_3_rd_data & -level_3_rd_data);
          nxt_state <= ALLOC;
        end

      ALLOC:
        begin
          nxt_alloc_page <= { level_3_addr, idx };
          nxt_alloc_gnt <= 1;
          nxt_count <= count - 1;
          nxt_level_3_wr_data <= (level_3_rd_data & ~abit);
          nxt_level_3_we <= 1;
          nxt_state <= UPDATE_2;
        end

      UPDATE_2:
        if (level_3_wr_data == 0)
          begin
            nxt_level_2_wr_data <= (level_2_rd_data & ~(1 << level_3_addr[4:0]));
            nxt_level_2_we <= 1;
            nxt_state <= UPDATE_1;
          end
        else
          begin
            nxt_level_2_addr <= idx;
            nxt_state <= IDLE;
          end

      UPDATE_1:
        if (level_2_wr_data == 0)
          begin
            nxt_level_1 <= (level_1 & ~(1 << level_2_addr));
            nxt_state <= UPDATE_1a;
          end
        else
          begin
            nxt_level_2_addr <= idx;
            nxt_state <= IDLE;
          end

      UPDATE_1a: // New level_1
        begin
          nxt_state <= UPDATE_0;
        end

      UPDATE_0: // New idx
        begin
          nxt_level_2_addr <= idx;
          nxt_state <= IDLE;
        end
    endcase
  end

endmodule
