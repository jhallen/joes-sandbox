module tb;

reg clk;
reg reset_l;

reg force_error;

// Rx Packet FIFO

wire [63:0] rx_packet_fifo_wr_data;
wire rx_packet_fifo_we;
wire rx_packet_fifo_full;

wire [63:0] rx_packet_fifo_rd_data;
wire rx_packet_fifo_re;
wire rx_packet_fifo_ne;

fifo_sync #(.DATAWIDTH(64)) rx_packet_fifo
  (
  .clk (clk),
  .reset_l (reset_l),

  .wr_data (rx_packet_fifo_wr_data),
  .we (rx_packet_fifo_we),
  .full (rx_packet_fifo_full),

  .ns_rd_data (rx_packet_fifo_rd_data),
  .re (rx_packet_fifo_re),
  .ns_ne (rx_packet_fifo_ne)
  );

// Tx Packet FIFO

wire [63:0] tx_packet_fifo_wr_data;
wire tx_packet_fifo_we;
wire tx_packet_fifo_full;

wire [63:0] tx_packet_fifo_rd_data;
wire tx_packet_fifo_re;
wire tx_packet_fifo_ne;

fifo_sync #(.DATAWIDTH(64)) tx_packet_fifo
  (
  .clk (clk),
  .reset_l (reset_l),

  .wr_data (tx_packet_fifo_wr_data),
  .we (tx_packet_fifo_we),
  .full (tx_packet_fifo_full),

  .ns_rd_data (tx_packet_fifo_rd_data),
  .re (tx_packet_fifo_re),
  .ns_ne (tx_packet_fifo_ne)
  );

// Segment FIFO

wire [63:0] seg_fifo_wr_data;
wire [7:0] seg_fifo_wr_be;
wire seg_fifo_we;
wire seg_fifo_full;

wire [63:0] seg_fifo_rd_data;
wire [7:0] seg_fifo_rd_be;
wire seg_fifo_re;
wire seg_fifo_ne;

fifo_sync #(.DATAWIDTH(64+8)) seg_fifo
  (
  .clk (clk),
  .reset_l (reset_l),

  .wr_data ({ seg_fifo_wr_be, seg_fifo_wr_data }),
  .we (seg_fifo_we),
  .full (seg_fifo_full),

  .ns_rd_data ({ seg_fifo_rd_be, seg_fifo_rd_data }),
  .re (seg_fifo_re),
  .ns_ne (seg_fifo_ne)
  );

// Packet generator

reg tgen_enable;

tgen tgen
  (
  .clk (clk),
  .reset_l (reset_l),
  .enable (tgen_enable),
  .packet_fifo_wr_data (tx_packet_fifo_wr_data),
  .packet_fifo_full (tx_packet_fifo_full),
  .packet_fifo_we (tx_packet_fifo_we)
  );

// Break packets into peices

tbreak tbreak
  (
  .clk (clk),
  .reset_l (reset_l),

  .packet_fifo_rd_data (tx_packet_fifo_rd_data),
  .packet_fifo_re (tx_packet_fifo_re),
  .packet_fifo_ne (tx_packet_fifo_ne),

  .out_fifo_wr_data (seg_fifo_wr_data),
  .out_fifo_wr_be (seg_fifo_wr_be),
  .out_fifo_we (seg_fifo_we),
  .out_fifo_full (seg_fifo_full)
  );

// Merge peices together

tmerge tmerge
  (
  .clk (clk),
  .reset_l (reset_l),

  .in_fifo_rd_data (seg_fifo_rd_data),
  .in_fifo_rd_be (seg_fifo_rd_be),
  .in_fifo_ne (seg_fifo_ne),
  .in_fifo_re (seg_fifo_re),

  .packet_fifo_wr_data (rx_packet_fifo_wr_data),
  .packet_fifo_we (rx_packet_fifo_we),
  .packet_fifo_full (rx_packet_fifo_full)
  );

// Packet checker

wire [31:0] packet_count;
wire [31:0] error_count;
wire [31:0] content_count;
wire [31:0] short_count;
wire [31:0] junk_count;
wire [31:0] seq_count;

tcheck tcheck
  (
  .clk (clk),
  .reset_l (reset_l),
  .packet_fifo_ne (rx_packet_fifo_ne),
  .packet_fifo_rd_data (rx_packet_fifo_rd_data),
  .packet_fifo_re (rx_packet_fifo_re),

  .packet_count (packet_count),
  .error_count (error_count),
  .content_count (content_count),
  .short_count (short_count),
  .junk_count (junk_count),
  .seq_count (seq_count)
  );

always
  begin
    #50 clk <= !clk;
  end

integer x;

initial
  begin
    $shm_open("debug.shm",0,500971520,1);
    $shm_probe(tb, "AC");
  end

initial
  begin
//    $dumpvars(0);
//    $dumpon;
    $display("Hi there!\n");
    clk <= 0;
    reset_l <= 1;
    tgen_enable <= 0;
    force_error <= 0;

    @(posedge clk);
    @(posedge clk);
    reset_l <= 0;
    @(posedge clk);
    @(posedge clk);
    reset_l <= 1;
    @(posedge clk);
    @(posedge clk);
    tgen_enable <= 1;

    for (x = 0; x != 100000; x = x + 1)
      @(posedge clk);
    force_error <= 1;
    @(posedge clk);
    force_error <= 0;

    for (x = 0; x != 100000; x = x + 1)
      @(posedge clk);

    $finish;
  end

endmodule
