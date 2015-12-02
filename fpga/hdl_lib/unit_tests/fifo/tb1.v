module tb;

reg clk;
reg reset_l;
reg [19:0] wr_data;
reg wr_mark;
reg we;
wire [19:0] rd_data;
reg re;
wire ne;
wire full;

wire track_bad;
wire [19:0] track_bad_label;
wire track_bad_mark;

wire [10:0] count;

tracking #(.LABELWIDTH(20), .HTWIDTH(3)) tracking
  (
  .clk (clk),
  .reset_l (reset_l),

  .track_label2 (wr_data),
  .track_mark2 (wr_mark),
  .track_fifo_we2 (we),

  .track_label1 (20'd0),
  .track_mark1 (1'd0),
  .track_fifo_we1 (1'd0),

  .track_fifo_full (full),

  .track_bad (track_bad),
  .track_bad_label (track_bad_label),
  .track_bad_mark (track_bad_mark),

  .count (count)
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
    wr_data <= 0;
    we <= 0;
    reset_l <= 1;

    @(posedge clk);
    @(posedge clk);
    reset_l <= 0;
    @(posedge clk);
    @(posedge clk);
    reset_l <= 1;
    @(posedge clk);
    @(posedge clk);

    wr_data <= 20'h00011;
    wr_mark <= 1;
    we <= 1;

    @(posedge clk);
    wr_data <= 20'h00021;
    @(posedge clk);
    wr_data <=20'h00031;
    @(posedge clk);
    wr_data <= 20'h00041;
    @(posedge clk);
    wr_data <= 20'h00051;
    @(posedge clk);
    wr_data <= 20'h00061;
    @(posedge clk);
    wr_data <= 20'h00071;
    @(posedge clk);
    wr_data <=20'h00031;
    @(posedge clk);
    we <= 0;

    for (x = 0; x != 10; x = x + 1)
      @(posedge clk);

    wr_data <= 20'h00061;
    wr_mark <= 0;
    we <= 1;
    @(posedge clk);
    wr_data <= 20'h00031;
    @(posedge clk);
    wr_data <= 20'h00011;
    @(posedge clk);
    wr_data <= 20'h00051;
    @(posedge clk);
    wr_data <= 20'h00041;
    @(posedge clk);
    wr_data <= 20'h00021;
    @(posedge clk);
    we <= 0;

    for (x = 0; x != 200; x = x + 1)
      @(posedge clk);

    $finish;
  end

endmodule
