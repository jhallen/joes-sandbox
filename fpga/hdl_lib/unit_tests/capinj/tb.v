module tb;

reg clk;
reg reset_l;

reg [7:0] wr_data;
reg [4:0] wr_addr;
reg we;

wire [7:0] rd_data;
reg [4:0] rd_addr;

wire [7:0] data;
wire sot;
wire eot;
wire valid;

reg capture_trig;
wire capture_armed;
wire [4:0] capture_wr_addr;

capture #(.DATAWIDTH(8)) capture
  (
  .reset_l (reset_l),
  .clk (clk),
  .sample_data_in (data),
  .sample_sot_in (sot),
  .sample_eot_in (eot),
  .trig (capture_trig),
  .armed (capture_armed),
  .ram_wr_addr (capture_wr_addr),
  .rd_addr (rd_addr),
  .rd_data (rd_data)
  );

reg inject_trig;

inject #(.DATAWIDTH(8)) inject
  (
  .reset_l (reset_l),
  .clk (clk),
  .wr_addr (wr_addr),
  .wr_data (wr_data),
  .we (we),
  .trig (inject_trig),
  .inject_data_out (data),
  .inject_sop (sot),
  .inject_eop (eot),
  .inject_valid (valid)
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
    wr_addr <= 0;
    rd_addr <= 0;
    capture_trig <= 0;
    inject_trig <= 0;

    @(posedge clk);
    @(posedge clk);
    reset_l <= 0;
    @(posedge clk);
    @(posedge clk);
    reset_l <= 1;
    @(posedge clk);
    @(posedge clk);

    wr_data <= 1;
    wr_addr <= 0;
    we <= 1;

    @(posedge clk);
    wr_data <= 2;
    wr_addr <= 1;
    @(posedge clk);
    wr_data <= 3;
    wr_addr <= 2;
    @(posedge clk);
    wr_data <= 4;
    wr_addr <= 3;
    @(posedge clk);
    wr_data <= 5;
    wr_addr <= 4;
    @(posedge clk);
    wr_data <= 6;
    wr_addr <= 5;
    @(posedge clk);
    we <= 0;
    wr_addr <= 6;

    for (x = 0; x != 10; x = x + 1)
      @(posedge clk);

    capture_trig <= 1;
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    inject_trig <= 1;
    @(posedge clk);
    inject_trig <= 0;
    for (x = 0; x != 20; x = x + 1)
      @(posedge clk);
    capture_trig <= 0;

    @(posedge clk);
    rd_addr <= 0;
    @(posedge clk);
    rd_addr <= 1;
    @(posedge clk);
    rd_addr <= 2;
    @(posedge clk);
    rd_addr <= 3;
    @(posedge clk);
    rd_addr <= 4;
    @(posedge clk);
    rd_addr <= 5;
    @(posedge clk);
    rd_addr <= 6;
    @(posedge clk);
    rd_addr <= 7;
    for (x = 0; x != 20; x = x + 1)
      @(posedge clk);

    wr_data <= 8'h55;
    wr_addr <= 0;
    we <= 1;

    @(posedge clk);
    we <= 0;
    wr_addr <= 1;
    @(posedge clk);
    for (x = 0; x != 10; x = x + 1)
      @(posedge clk);

    capture_trig <= 1;
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    inject_trig <= 1;
    @(posedge clk);
    inject_trig <= 0;
    for (x = 0; x != 20; x = x + 1)
      @(posedge clk);
    capture_trig <= 0;

    @(posedge clk);
    rd_addr <= 0;
    @(posedge clk);
    rd_addr <= 1;
    @(posedge clk);
    rd_addr <= 2;
    @(posedge clk);
    rd_addr <= 3;
    @(posedge clk);
    rd_addr <= 4;
    @(posedge clk);
    rd_addr <= 5;
    @(posedge clk);
    rd_addr <= 6;
    @(posedge clk);
    rd_addr <= 7;
    for (x = 0; x != 20; x = x + 1)
      @(posedge clk);

    wr_data <= 8'h55;
    wr_addr <= 0;
    we <= 1;
    @(posedge clk);
    wr_addr <= 1;
    wr_data <= 1;
    @(posedge clk);
    wr_addr <= 2;
    wr_data <= 2;
    @(posedge clk);
    wr_addr <= 3;
    wr_data <= 3;
    @(posedge clk);
    wr_addr <= 4;
    wr_data <= 4;
    @(posedge clk);
    wr_addr <= 5;
    wr_data <= 5;
    @(posedge clk);
    wr_addr <= 6;
    wr_data <= 6;
    @(posedge clk);
    wr_addr <= 7;
    wr_data <= 7;
    @(posedge clk);
    wr_addr <= 8;
    wr_data <= 8;
    @(posedge clk);
    wr_addr <= 9;
    wr_data <= 9;
    @(posedge clk);
    wr_addr <= 10;
    wr_data <= 10;
    @(posedge clk);
    wr_addr <= 11;
    wr_data <= 11;
    @(posedge clk);
    wr_addr <= 12;
    wr_data <= 12;
    @(posedge clk);
    wr_addr <= 13;
    wr_data <= 13;
    @(posedge clk);
    wr_addr <= 14;
    wr_data <= 14;
    @(posedge clk);
    wr_addr <= 15;
    wr_data <= 15;
    @(posedge clk);
    wr_addr <= 16;
    wr_data <= 16;
    @(posedge clk);
    wr_addr <= 17;
    wr_data <= 17;
    @(posedge clk);
    wr_addr <= 18;
    wr_data <= 18;
    @(posedge clk);
    wr_addr <= 19;
    wr_data <= 19;
    @(posedge clk);
    wr_addr <= 20;
    wr_data <= 20;
    @(posedge clk);
    wr_addr <= 21;
    wr_data <= 21;
    @(posedge clk);
    wr_addr <= 22;
    wr_data <= 22;
    @(posedge clk);
    wr_addr <= 23;
    wr_data <= 23;
    @(posedge clk);
    wr_addr <= 24;
    wr_data <= 24;
    @(posedge clk);
    wr_addr <= 25;
    wr_data <= 25;
    @(posedge clk);
    wr_addr <= 26;
    wr_data <= 26;
    @(posedge clk);
    wr_addr <= 27;
    wr_data <= 27;
    @(posedge clk);
    wr_addr <= 28;
    wr_data <= 28;
    @(posedge clk);
    wr_addr <= 29;
    wr_data <= 29;
    @(posedge clk);
    wr_addr <= 30;
    wr_data <= 30;
    @(posedge clk);
    wr_addr <= 31;
    wr_data <= 31;
    @(posedge clk);
    we <= 0;
    wr_addr <= 0;
    @(posedge clk);
    for (x = 0; x != 10; x = x + 1)
      @(posedge clk);

    capture_trig <= 1;
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    inject_trig <= 1;
    @(posedge clk);
    inject_trig <= 0;
    for (x = 0; x != 50; x = x + 1)
      @(posedge clk);
    capture_trig <= 0;

    @(posedge clk);
    rd_addr <= 0;
    @(posedge clk);
    rd_addr <= 1;
    @(posedge clk);
    rd_addr <= 2;
    @(posedge clk);
    rd_addr <= 3;
    @(posedge clk);
    rd_addr <= 4;
    @(posedge clk);
    rd_addr <= 5;
    @(posedge clk);
    rd_addr <= 6;
    @(posedge clk);
    rd_addr <= 7;
    @(posedge clk);
    rd_addr <= 8;
    @(posedge clk);
    rd_addr <= 9;
    @(posedge clk);
    rd_addr <= 10;
    @(posedge clk);
    rd_addr <= 11;
    @(posedge clk);
    rd_addr <= 12;
    @(posedge clk);
    rd_addr <= 13;
    @(posedge clk);
    rd_addr <= 14;
    @(posedge clk);
    rd_addr <= 15;
    @(posedge clk);
    rd_addr <= 16;
    @(posedge clk);
    rd_addr <= 17;
    @(posedge clk);
    rd_addr <= 18;
    @(posedge clk);
    rd_addr <= 19;
    @(posedge clk);
    rd_addr <= 20;
    @(posedge clk);
    rd_addr <= 21;
    @(posedge clk);
    rd_addr <= 22;
    @(posedge clk);
    rd_addr <= 23;
    @(posedge clk);
    rd_addr <= 24;
    @(posedge clk);
    rd_addr <= 25;
    @(posedge clk);
    rd_addr <= 26;
    @(posedge clk);
    rd_addr <= 27;
    @(posedge clk);
    rd_addr <= 28;
    @(posedge clk);
    rd_addr <= 29;
    @(posedge clk);
    rd_addr <= 30;
    @(posedge clk);
    rd_addr <= 31;
    for (x = 0; x != 20; x = x + 1)
      @(posedge clk);

    $finish;
  end

endmodule
