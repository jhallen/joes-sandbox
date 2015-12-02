// Check random packets

module tcheck
  (
  clk,
  reset_l,

  packet_fifo_ne,
  packet_fifo_rd_data,
  packet_fifo_re,

  packet_count,
  error_count,
  content_count,
  short_count,
  junk_count,
  seq_count
  );

input clk;
input reset_l;

input packet_fifo_ne;
input [63:0] packet_fifo_rd_data;

output packet_fifo_re;
reg packet_fifo_re;

output [31:0] packet_count;
output [31:0] error_count;
output [31:0] content_count;
output [31:0] short_count;
output [31:0] junk_count;
output [31:0] seq_count;

// Packet checker

reg in_packet;
reg header_is_next;
reg [31:0] last_seq;
reg past_content_error;
reg [63:0] random_number;
reg [9:0] count;

reg packet;
reg error;
reg content_error;
reg short_error;
reg seq_error;
reg junk;

reg [31:0] packet_count;
reg [31:0] error_count;
reg [31:0] content_count;
reg [31:0] short_count;
reg [31:0] junk_count;
reg [31:0] seq_count;

always @(posedge clk or negedge reset_l)
  if (!reset_l)
    begin
      in_packet <= 0;
      header_is_next <= 0;
      past_content_error <= 0;
      random_number <= 0;
      count <= 0;
      last_seq <= 32'hffff_ffff;
      packet_fifo_re <= 0;

      packet <= 0;
      content_error <= 0;
      short_error <= 0;
      junk <= 0;
      seq_error <= 0;

      packet_count <= 0;
      error_count <= 0;
      content_count <= 0;
      short_count <= 0;
      junk_count <= 0;
      seq_count <= 0;
    end
  else
    begin
      packet_fifo_re <= 0;

      packet <= 0;
      content_error <= 0;
      short_error <= 0;
      seq_error <= 0;
      junk <= 0;

      // Update counters
      if (packet)
        packet_count <= packet_count + 1'd1;

      if (content_error || short_error || seq_error)
        error_count <= error_count + 1'd1;

      if (seq_error)
        seq_count <= seq_count + 1'd1;

      if (content_error)
        content_count <= content_count + 1'd1;

      if (short_error)
        short_count <= short_count + 1'd1;

      if (junk)
        junk_count <= junk_count + 1'd1;

      // Process data
      if (packet_fifo_ne)
        begin
          packet_fifo_re <= 1;
          if (packet_fifo_rd_data == 64'hffff_ffff_ffff_ffff)
            begin // Start of packet code
              in_packet <= 1;
              header_is_next <= 1;
              count <= 512 - 3;
              packet <= 1;
              past_content_error <= 0;
              if (in_packet)
                short_error <= 1;
            end
          else if (header_is_next)
            begin // Header
              header_is_next <= 0;
              last_seq <= packet_fifo_rd_data[31:0];
              if (last_seq + 1'd1 != packet_fifo_rd_data[31:0])
                seq_error <= 1;
              random_number <= { packet_fifo_rd_data[63:48] + 16'd4, packet_fifo_rd_data[63:48] + 16'd3, packet_fifo_rd_data[63:48] + 16'd2, packet_fifo_rd_data[63:48] + 16'd1 };
            end
          else if (in_packet)
            begin // Middle of packet
              random_number <= { random_number[63:48] + 16'd4, random_number[63:48] + 16'd3, random_number[63:48] + 16'd2, random_number[63:48] + 16'd1 };
              if (packet_fifo_rd_data != random_number)
                if (!past_content_error)
                  begin
                    past_content_error <= 1;
                    content_error <= 1;
                  end
              count <= count - 1'd1;
              if (!count)
                in_packet <= 0;
            end
          else
            junk <= 1;
        end
    end

endmodule
