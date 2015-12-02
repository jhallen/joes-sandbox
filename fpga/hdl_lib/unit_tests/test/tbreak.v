// Break packets into pieces

module tbreak
  (
  clk,
  reset_l,

  // Packets in
  packet_fifo_rd_data,
  packet_fifo_re,
  packet_fifo_ne,

  // Packets out
  out_fifo_wr_data,
  out_fifo_wr_be, // Byte enables
  out_fifo_we,
  out_fifo_full
  );

input clk;
input reset_l;

input [63:0] packet_fifo_rd_data;

output packet_fifo_re;
reg packet_fifo_re;

input packet_fifo_ne;

output [63:0] out_fifo_wr_data;
reg [63:0] out_fifo_wr_data;

output [7:0] out_fifo_wr_be;
reg [7:0] out_fifo_wr_be;

output out_fifo_we;
reg out_fifo_we;

input out_fifo_full;

reg [63:0] out_fifo_wr_data_1;
reg [7:0] out_fifo_wr_be_1;
reg out_fifo_we_1;

reg [9:0] word_count;
reg [12:0] count;
reg [11:0] offset;
reg [11:0] last_offset;
reg [11:0] seg_len;
reg [7:0] first_mask;
reg [7:0] last_mask;
reg [11:0] random_val;

reg [2:0] state;

parameter
  IDLE = 0,
  NEXT_SEG = 1,
  MASKS = 2,
  FIRST = 3,
  REST = 4;

always @(posedge clk or negedge reset_l)
  if (!reset_l)
    begin
      state <= IDLE;
      packet_fifo_re <= 0;
      out_fifo_wr_data <= 0;
      out_fifo_wr_be <= 0;
      out_fifo_we <= 0;
      out_fifo_wr_data_1 <= 0;
      out_fifo_wr_be_1 <= 0;
      out_fifo_we_1 <= 0;
      count <= 0;
      offset <= 0;
      seg_len <= 0;
      first_mask <= 0;
      last_mask <= 0;
      random_val <= 1;
    end
  else
    begin
      packet_fifo_re <= 0;
      out_fifo_we_1 <= 0;

      out_fifo_we <= out_fifo_we_1;
      out_fifo_wr_be <= out_fifo_wr_be_1;
      out_fifo_wr_data[7:0] <= out_fifo_wr_be_1[0] ? out_fifo_wr_data_1[7:0] : 8'hff;
      out_fifo_wr_data[15:8] <= out_fifo_wr_be_1[1] ? out_fifo_wr_data_1[15:8] : 8'hff;
      out_fifo_wr_data[23:16] <= out_fifo_wr_be_1[2] ? out_fifo_wr_data_1[23:16] : 8'hff;
      out_fifo_wr_data[31:24] <= out_fifo_wr_be_1[3] ? out_fifo_wr_data_1[31:24] : 8'hff;
      out_fifo_wr_data[39:32] <= out_fifo_wr_be_1[4] ? out_fifo_wr_data_1[39:32] : 8'hff;
      out_fifo_wr_data[47:40] <= out_fifo_wr_be_1[5] ? out_fifo_wr_data_1[47:40] : 8'hff;
      out_fifo_wr_data[55:48] <= out_fifo_wr_be_1[6] ? out_fifo_wr_data_1[55:48] : 8'hff;
      out_fifo_wr_data[63:56] <= out_fifo_wr_be_1[7] ? out_fifo_wr_data_1[63:56] : 8'hff;

      case (state)
        IDLE:
          if (packet_fifo_ne)
            begin
              // We have a packet
              count <= 4096; // It's 4KB long...
              offset <= 0; // Starting offset
              state <= NEXT_SEG;
            end

        NEXT_SEG: // Send next segment of packet
          if (count) // Any more data to send?
            begin
              if (random_val > count)
                begin
                  seg_len <= count;
                  last_offset <= 12'hfff;
                end
              else
                begin
                  seg_len <= random_val;
                  last_offset <= offset + random_val[11:0] - 12'd1;
                end
              random_val <= { random_val[10:0], random_val[11]^random_val[10]^random_val[9]^random_val[3] };
              state <= MASKS;
            end
          else
            state <= IDLE;

        MASKS: // Calculate masks
          begin
            first_mask <= 8'b1111_1111 << offset[2:0];
            last_mask <= 8'b1111_1111 >> (7 - last_offset[2:0]);
            word_count <= (last_offset[11:3] - offset[11:3] + 1'd1);
            state <= FIRST;
          end

        FIRST:
          if (packet_fifo_ne && !out_fifo_full)
            begin
              out_fifo_wr_data_1 <= packet_fifo_rd_data;
              out_fifo_we_1 <= 1;
              word_count <= word_count - 1'd1;
              if (word_count == 1)
                begin
                  out_fifo_wr_be_1 <= (first_mask & last_mask);
                  offset <= offset + seg_len;
                  count <= count - seg_len;
                  seg_len <= 0;
                  state <= NEXT_SEG;
                  if (first_mask[7] && last_mask[7])
                    packet_fifo_re <= 1;
                end
              else
                begin
                  out_fifo_wr_be_1 <= first_mask;
                  offset <= { offset[11:3] + 1'd1, 3'd0 };
                  seg_len <= seg_len - (4'd8 - offset[2:0]);
                  count <= count - (4'd8 - offset[2:0]);
                  packet_fifo_re <= 1;
                  state <= REST;
                end
            end

        REST:
          if (packet_fifo_ne && !out_fifo_full)
            begin
              out_fifo_wr_data_1 <= packet_fifo_rd_data;
              out_fifo_we_1 <= 1;
              word_count <= word_count - 1'd1;
              if (word_count == 1)
                begin
                  out_fifo_wr_be_1 <= last_mask;
                  offset <= offset + seg_len;
                  count <= count - seg_len;
                  seg_len <= 0;
                  state <= NEXT_SEG;
                  if (last_mask[7])
                    packet_fifo_re <= 1;
                end
              else
                begin
                  out_fifo_wr_be_1 <= 8'b1111_1111;
                  count <= count - 4'd8;
                  offset <= offset + 4'd8;
                  seg_len <= seg_len - 4'd8;
                  packet_fifo_re <= 1;
                end
            end
      endcase
    end

endmodule
