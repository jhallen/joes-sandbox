// Merge peices together

module tmerge
  (
  reset_l,
  clk,

  in_fifo_rd_data,
  in_fifo_rd_be,
  in_fifo_ne,
  in_fifo_re,

  packet_fifo_wr_data,
  packet_fifo_we,
  packet_fifo_full
  );

input reset_l;
input clk;

input [63:0] in_fifo_rd_data;
input [7:0] in_fifo_rd_be;
input in_fifo_ne;
output in_fifo_re;
reg in_fifo_re;

output [63:0] packet_fifo_wr_data;
reg [63:0] packet_fifo_wr_data;

output packet_fifo_we;
reg packet_fifo_we;

input packet_fifo_full;

always @(posedge clk or negedge reset_l)
  if (!reset_l)
    begin
      in_fifo_re <= 0;
      packet_fifo_wr_data <= 0;
      packet_fifo_we <= 0;
    end
  else
    begin
      in_fifo_re <= 0;
      packet_fifo_we <= 0;
      if (in_fifo_ne && !packet_fifo_full)
        begin
          in_fifo_re <= 1;
          if (in_fifo_rd_be[0]) packet_fifo_wr_data[7:0] <= in_fifo_rd_data[7:0];
          if (in_fifo_rd_be[1]) packet_fifo_wr_data[15:8] <= in_fifo_rd_data[15:8];
          if (in_fifo_rd_be[2]) packet_fifo_wr_data[23:16] <= in_fifo_rd_data[23:16];
          if (in_fifo_rd_be[3]) packet_fifo_wr_data[31:24] <= in_fifo_rd_data[31:24];
          if (in_fifo_rd_be[4]) packet_fifo_wr_data[39:32] <= in_fifo_rd_data[39:32];
          if (in_fifo_rd_be[5]) packet_fifo_wr_data[47:40] <= in_fifo_rd_data[47:40];
          if (in_fifo_rd_be[6]) packet_fifo_wr_data[55:48] <= in_fifo_rd_data[55:48];
          if (in_fifo_rd_be[7])
            begin
              packet_fifo_wr_data[63:56] <= in_fifo_rd_data[63:56];
              packet_fifo_we <= 1;
            end
        end
    end

endmodule
