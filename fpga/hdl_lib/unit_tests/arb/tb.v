module tb;

reg clk;
reg reset_l;

reg req_0;
reg req_1;
reg req_2;
wire [2:0] nxt_gnt;
wire [2:0] maybe_nxt_gnt;
wire new_gnt;
wire [2:0] gnt;
reg last_0;
reg last_1;
reg last_2;
wire hazard = 0;
wire first;
wire own_0;
wire own_1;
wire own_2;
wire owned = (own_0 || own_1 || own_2);

arbiter #(3) arbiter
  (
  .req ({ req_2, req_1, req_0 }),
  .nxt_gnt (nxt_gnt),
  .maybe_nxt_gnt (maybe_nxt_gnt),
  .new_gnt (new_gnt),
  .gnt (gnt),
  
  .last ({ last_2, last_1, last_0 }),
  .hazard (hazard),
  .first (first),
  .vclk (clk),
  .reset_l (reset_l)
  );

// Comment this out to prevent clients from clearing last at end of transaction
// `define NO_LAZY_LAST 1

reg [2:0] do_req_0;
reg [1:0] state_0;
reg [2:0] count_0;
assign own_0 = (state_0 == 1 && gnt[0] || state_0 == 2);
always @(posedge clk or negedge reset_l)
  if (!reset_l)
    begin
      state_0 <= 0;
      req_0 <= 0;
      last_0 <= 0;
      count_0 <= 0;
    end
  else
    begin
      case (state_0)
        0:
          if (do_req_0)
            begin
              count_0 <= do_req_0;
              req_0 <= 1;
              state_0 <= 1;
              // Assert last along with req for one-cycle transactions
              if (do_req_0 == 1)
                last_0 <= 1;
              else
                last_0 <= 0;
            end

        1: // Wait for gnt
          if (gnt[0])
            begin
              // We own resource this cycle!
              count_0 <= count_0 - 1;
              if (count_0 == 1)
                begin // This is the last cycle!
                  state_0 <= 0;
`ifdef NO_LAZY_LAST
                  last_0 <= 0;
`endif
                  // Check for new requests...
                  if (do_req_0)
                    begin
                      // Leave req asserted
                      count_0 <= do_req_0;
                      state_0 <= 1;
                      // Assert last along with req for one-cycle transactions
                      if (do_req_0 == 1)
                        last_0 <= 1;
                      else
                        last_0 <= 0;
                    end
                  else
                    // OK, de-assert req
                    req_0 <= 0;
                end
              else
                begin
                  if (count_0 == 2)
                    // Next cycle is last cycle!
                    last_0 <= 1;
                  state_0 <= 2;
                end
            end

        2: // Continue until last cycle reached
          begin
            // Same code as above, but no waiting for gnt
            count_0 <= count_0 - 1;
            if (count_0 == 1)
              begin // This is the last cycle!
                state_0 <= 0;
`ifdef NO_LAZY_LAST
                  last_0 <= 0;
`endif
                // Check for new requests...
                if (do_req_0)
                  begin
                    count_0 <= do_req_0;
                    state_0 <= 1;
                    // Assert last along with req for one-cycle transactions
                    if (do_req_0 == 1)
                      last_0 <= 1;
                    else
                      last_0 <= 0;
                  end
                else
                  req_0 <= 0;
              end
            else
              begin
                if (count_0 == 2)
                  // Next cycle is last cycle!
                  last_0 <= 1;
                state_0 <= 2;
              end
          end
      endcase
    end

reg [2:0] do_req_1;
reg [1:0] state_1;
reg [2:0] count_1;
assign own_1 = (state_1 == 1 && gnt[1] || state_1 == 2);
always @(posedge clk or negedge reset_l)
  if (!reset_l)
    begin
      state_1 <= 0;
      req_1 <= 0;
      last_1 <= 0;
      count_1 <= 0;
    end
  else
    begin
      case (state_1)
        0:
          if (do_req_1)
            begin
              count_1 <= do_req_1;
              req_1 <= 1;
              state_1 <= 1;
              // Assert last along with req for one-cycle transactions
              if (do_req_1 == 1)
                last_1 <= 1;
              else
                last_1 <= 0;
            end

        1: // Wait for gnt
          if (gnt[1])
            begin
              // We own resource this cycle!
              count_1 <= count_1 - 1;
              if (count_1 == 1)
                begin // This is the last cycle!
                  state_1 <= 0;
`ifdef NO_LAZY_LAST
                  last_1 <= 0;
`endif
                  // Check for new requests...
                  if (do_req_1)
                    begin
                      count_1 <= do_req_1;
                      state_1 <= 1;
                      // Assert last along with req for one-cycle transactions
                      if (do_req_1 == 1)
                        last_1 <= 1;
                      else
                        last_1 <= 0;
                    end
                  else
                    req_1 <= 0;
                end
              else
                begin
                  if (count_1 == 2)
                    // Next cycle is last cycle!
                    last_1 <= 1;
                  state_1 <= 2;
                end
            end

        2: // Continue until last cycle reached
          begin
            // Same code as above, but no waiting for gnt
            count_1 <= count_1 - 1;
            if (count_1 == 1)
              begin // This is the last cycle!
                state_1 <= 0;
`ifdef NO_LAZY_LAST
                  last_1 <= 0;
`endif
                // Check for new requests...
                if (do_req_1)
                  begin
                    count_1 <= do_req_1;
                    state_1 <= 1;
                    // Assert last along with req for one-cycle transactions
                    if (do_req_1 == 1)
                      last_1 <= 1;
                    else
                      last_1 <= 0;
                  end
                else
                  req_1 <= 0;
              end
            else
              begin
                if (count_1 == 2)
                  // Next cycle is last cycle!
                  last_1 <= 1;
                state_1 <= 2;
              end
          end
      endcase
    end


reg [2:0] do_req_2;
reg [1:0] state_2;
reg [2:0] count_2;
assign own_2 = (state_2 == 1 && gnt[2] || state_2 == 2);
always @(posedge clk or negedge reset_l)
  if (!reset_l)
    begin
      state_2 <= 0;
      req_2 <= 0;
      last_2 <= 0;
      count_2 <= 0;
    end
  else
    begin
      case (state_2)
        0:
          if (do_req_2)
            begin
              count_2 <= do_req_2;
              req_2 <= 1;
              state_2 <= 1;
              // Assert last along with req for one-cycle transactions
              if (do_req_2 == 1)
                last_2 <= 1;
              else
                last_2 <= 0;
            end

        1: // Wait for gnt
          if (gnt[2])
            begin
              // We own resource this cycle!
              count_2 <= count_2 - 1;
              if (count_2 == 1)
                begin // This is the last cycle!
                  state_2 <= 0;
`ifdef NO_LAZY_LAST
                  last_2 <= 0;
`endif
                  // Check for new requests...
                  if (do_req_2)
                    begin
                      count_2 <= do_req_2;
                      state_2 <= 1;
                      // Assert last along with req for one-cycle transactions
                      if (do_req_2 == 1)
                        last_2 <= 1;
                      else
                        last_2 <= 0;
                    end
                  else
                    req_2 <= 0;
                end
              else
                begin
                  if (count_2 == 2)
                    // Next cycle is last cycle!
                    last_2 <= 1;
                  state_2 <= 2;
                end
            end

        2: // Continue until last cycle reached
          begin
            // Same code as above, but no waiting for gnt
            count_2 <= count_2 - 1;
            if (count_2 == 1)
              begin // This is the last cycle!
                state_2 <= 0;
`ifdef NO_LAZY_LAST
                  last_2 <= 0;
`endif
                // Check for new requests...
                if (do_req_2)
                  begin
                    count_2 <= do_req_2;
                    state_2 <= 1;
                    // Assert last along with req for one-cycle transactions
                    if (do_req_2 == 1)
                      last_2 <= 1;
                    else
                      last_2 <= 0;
                  end
                else
                  req_2 <= 0;
              end
            else
              begin
                if (count_2 == 2)
                  // Next cycle is last cycle!
                  last_2 <= 1;
                state_2 <= 2;
              end
          end
      endcase
    end



always
  begin
    #50 clk <= !clk;
  end

always @(posedge clk)
  if (own_0 && own_1 ||
      own_0 && own_2 ||
      own_1 && own_2)
      $display("Fatal error: town clients think they own resource at same time!");

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
    $display("Hi there!");
    clk <= 0;
    reset_l <= 1;
    do_req_0 <= 0;
    do_req_1 <= 0;
    do_req_2 <= 0;

    @(posedge clk);
    @(posedge clk);
    reset_l <= 0;
    @(posedge clk);
    @(posedge clk);
    reset_l <= 1;
    @(posedge clk);
    @(posedge clk);

    // Some single requests
    do_req_0 <= 1;
    @(posedge clk);
    do_req_0 <= 0;
    do_req_1 <= 2;
    do_req_2 <= 1;
    @(posedge clk);
    do_req_0 <= 0;
    do_req_1 <= 0;
    do_req_2 <= 0;
    @(posedge clk);

    do_req_0 <= 1;
    @(posedge clk);
    do_req_0 <= 0;
    do_req_1 <= 1;
    do_req_2 <= 1;
    @(posedge clk);
    @(posedge clk);
    do_req_0 <= 0;
    do_req_1 <= 0;
    do_req_2 <= 0;
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    do_req_0 <= 4;
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    do_req_1 <= 1;
    @(posedge clk);
    do_req_1 <= 0;
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    do_req_0 <= 0;
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    do_req_0 <= 1;
    @(posedge clk);
    do_req_0 <= 0;
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    do_req_0 <= 1;
    @(posedge clk);
    do_req_0 <= 0;

    for (x = 0; x != 20; x = x + 1)
      @(posedge clk);


    $finish;
  end

endmodule
