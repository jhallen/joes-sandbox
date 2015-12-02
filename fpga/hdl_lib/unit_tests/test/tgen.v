// Generate packets with random data

module tgen
  (
  clk,
  reset_l,

  enable,
  packet_fifo_wr_data,
  packet_fifo_full,
  packet_fifo_we
  );

input clk;
input reset_l;

input enable;

output [63:0] packet_fifo_wr_data;
reg [63:0] packet_fifo_wr_data;

input packet_fifo_full;

output packet_fifo_we;
reg packet_fifo_we;

// Random number generator

function [95:0] next_random;
input [31:0] w;
input [31:0] z;
reg [31:0] nw;
reg [31:0] nz;
reg [31:0] nx;
  begin
    nz = 32'h00009069 * { 16'd0, z[15:0] } + z[31:16];
    nw = 32'h00004650 * { 16'd0, w[15:0] } + z[31:16];
    nx = { nz[15:0], 16'd0 } + nw;
    next_random = { nw, nz, nx };
  end
endfunction

// Packet generator

reg [1:0] state;

parameter
  FIRST = 0,
  REST = 1;

reg [31:0] seq_no; // Sequence number
reg [12:0] count;
reg [63:0] random_number;

always @(posedge clk or negedge reset_l)
  if (!reset_l)
    begin
      state <= FIRST;
      seq_no <= 0;
      count <= 0;
      random_number <= 0;
      packet_fifo_we <= 0;
      packet_fifo_wr_data <= 0;
    end
  else
    begin
      packet_fifo_we <= 0;
      case (state)
        FIRST:
          if (enable && !packet_fifo_full)
            begin
              packet_fifo_wr_data <= 64'hFFFF_FFFF_FFFF_FFFF; // Unique start of packet code
              packet_fifo_we <= 1;
              state <= REST;
              random_number <= { random_number[63:48], 16'd0, seq_no }; // Header with sequence number and salt (All 1s not allowed)
              seq_no <= seq_no + 1'd1;
              count <= 512 - 2; // 8192 * 8B = 64KB Packets
            end

        REST:
          if (!packet_fifo_full)
            begin
              // random_number <= { random_number[62:0], ~random_number[63]^random_number[62]^random_number[60]^random_number[59] }; // Maximal length LFSR
              random_number <= { random_number[63:48] + 16'd4, random_number[63:48] + 16'd3, random_number[63:48] + 16'd2, random_number[63:48] + 16'd1 };
              packet_fifo_we <= 1;
              packet_fifo_wr_data <= random_number;
              count <= count - 1'd1;
              if (!count)
                state <= FIRST;
            end
      endcase
    end

endmodule
