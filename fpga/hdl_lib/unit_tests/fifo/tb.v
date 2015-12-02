module tb;

reg clk;
reg reset_l;
reg [7:0] wr_data;
reg we;
wire [7:0] rd_data;
reg re;
wire ne;

bus b();

fred it(.b(b));

assign b.clk = clk;
assign b.reset_l = reset_l;

fifo_sync #(.DATAWIDTH(8)) fifo
  (
  .clk (clk),
  .reset_l (reset_l),
  .wr_data (wr_data),
  .we (we),
  .ns_rd_data (rd_data),
  .re (re),
  .ns_ne (ne)
  );

typedef enum { RESET, FIRST, SECOND } states;

// synthesis translate_off
`ifndef FOO
states state;
`else
// synthesis translate_on
reg [2:0] state;
// synthesis translate_off
`endif
// synthesis translate_on

always @(posedge clk)
  if (!reset_l)
    begin
      state <= RESET;
    end
  else
    begin
      case (state)
        RESET:
          state <= FIRST;
        FIRST:
          state <= SECOND;
        SECOND:
          state <= FIRST;
      endcase
    end

always @(posedge clk)
  if (!reset_l)
    re <= 0;
  else if (ne)
    re <= 1;
  else
    re <= 0;

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

    wr_data <= 1;
    we <= 1;

    @(posedge clk);
    wr_data <= 2;
    @(posedge clk);
    wr_data <= 3;
    @(posedge clk);
    wr_data <= 4;
    @(posedge clk);
    wr_data <= 5;
    @(posedge clk);
    wr_data <= 6;
    @(posedge clk);
    we <= 0;

    for (x = 0; x != 10; x = x + 1)
      @(posedge clk);

    wr_data <= 1;
    we <= 1;
    @(posedge clk);
    wr_data <= 2;
    @(posedge clk);
    wr_data <= 3;
    @(posedge clk);
    wr_data <= 4;
    @(posedge clk);
    we <= 0;

    for (x = 0; x != 10; x = x + 1)
      @(posedge clk);

    $finish;
  end

endmodule
