module tb;

reg clk;
reg reset_l;

reg free_req;
wire free_gnt;
reg [14:0] free_page;
reg [3:0] free_who;

reg alloc_req;
wire alloc_gnt;
wire [14:0] alloc_page;

wire double_free_error;
wire [14:0] double_free_page;
wire [3:0] double_free_who;

wire [15:0] count;

buflet_map buflet_map
  (
  .clk (clk),
  .reset_l (reset_l),

  .free_req (free_req),
  .free_gnt (free_gnt),
  .free_page (free_page),
  .free_who (free_who),

  .alloc_req (alloc_req),
  .alloc_gnt (alloc_gnt),
  .alloc_page (alloc_page),

  .double_free_error (double_free_error),
  .double_free_page (double_free_page),
  .double_free_who (double_free_who),

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
    $display("%d %d %d\n", 1==1, 5 + ~(1==1), 5 + ~(1!=1));
    clk <= 0;
    reset_l <= 1;
    alloc_req <= 0;
    free_req <= 0;
    free_page <= 0;
    free_who <= 0;

    @(posedge clk);
    @(posedge clk);
    reset_l <= 0;
    @(posedge clk);
    @(posedge clk);
    reset_l <= 1;
    @(posedge clk);
    @(posedge clk);

    // Allocate everything.
    for (x = 16'h0c00; x != 16'h8000; x = x + 1)
      begin

        alloc_req <= 1;
        do @(posedge clk); while (!alloc_gnt);
        alloc_req <= 0;
        if (x != alloc_page)
          begin
            $display("Alloc problem for %x", x);
          end
        @(posedge clk);
      end

    // Free everything
/*
    // Forward
    for (x = 16'h0c00; x != 16'h8000; x = x + 1)
      begin
        free_req <= 1;
        free_page <= x;
        do @(posedge clk); while (!free_gnt);
        free_req <= 0;
        @(posedge clk);
      end
*/
    // Reverse
/*
    for (x = 16'h7fff; x != 16'h0bff; x = x - 1)
      begin
        free_req <= 1;
        free_page <= x;
        do @(posedge clk); while (!free_gnt);
        free_req <= 0;
        @(posedge clk);
      end
*/

    // Try a double-free
/*
    free_req <= 1;
    free_page <= 16'h1234;
    do @(posedge clk); while (!free_gnt);
    free_req <= 0;
    @(posedge clk);
*/

    // Allocate everything.
/*
    for (x = 16'h0c00; x != 16'h8000; x = x + 1)
      begin

        alloc_req <= 1;
        do @(posedge clk); while (!alloc_gnt);
        alloc_req <= 0;
        if (x != alloc_page)
          begin
            $display("Alloc problem for %x", x);
          end
        @(posedge clk);
      end
*/

    // Free one, then alloc it
    // Forward
    for (x = 16'h0c00; x != 16'h8000; x = x + 1)
      begin

        free_req <= 1;
        free_page <= x;
        do @(posedge clk); while (!free_gnt);
        free_req <= 0;
        @(posedge clk);

        alloc_req <= 1;
        do @(posedge clk); while (!alloc_gnt);
        alloc_req <= 0;
        if (x != alloc_page)
          begin
            $display("Alloc problem for %x", x);
          end
        @(posedge clk);
      end

    // Reverse
    for (x = 16'h7fff; x != 16'h0bff; x = x - 1)
      begin

        free_req <= 1;
        free_page <= x;
        do @(posedge clk); while (!free_gnt);
        free_req <= 0;
        @(posedge clk);

        alloc_req <= 1;
        do @(posedge clk); while (!alloc_gnt);
        alloc_req <= 0;
        if (x != alloc_page)
          begin
            $display("Alloc problem for %x", x);
          end
        @(posedge clk);
      end
    

/*
    alloc_req <= 1;
    do @(posedge clk); while (!alloc_gnt);
    alloc_req <= 0;
    @(posedge clk);

    alloc_req <= 1;
    do @(posedge clk); while (!alloc_gnt);
    alloc_req <= 0;
    @(posedge clk);

    alloc_req <= 1;
    do @(posedge clk); while (!alloc_gnt);
    alloc_req <= 0;
    @(posedge clk);
*/

    for (x = 0; x != 20; x = x + 1)
      @(posedge clk);

    $finish;
  end

endmodule
