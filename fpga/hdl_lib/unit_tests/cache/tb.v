// Cache test

module tb;

reg clk;
reg reset_l;

reg client_req;
wire client_gnt;
reg client_write;
reg [19:0] client_addr;
reg [63:0] client_wr_data;
wire [63:0] client_rd_data;
reg client_blast;

wire mem_req;
reg mem_gnt;
wire mem_write;
wire [19:0] mem_addr;
wire [63:0] mem_wr_data;
reg [63:0] mem_rd_data;

cache cache
  (
  .reset_l (reset_l),
  .clk (clk),

  .client_req (client_req),
  .client_gnt (client_gnt),
  .client_write (client_write),
  .client_blast (client_blast),
  .client_addr (client_addr), // Byte address
  .client_wr_data (client_wr_data),
  .client_wr_par (8'd0),
  .client_rd_data (client_rd_data),

  .mem_req (mem_req),
  .mem_gnt (mem_gnt),
  .mem_write (mem_write), // Byte address
  .mem_addr (mem_addr),
  .mem_wr_data (mem_wr_data),
  .mem_rd_data (mem_rd_data),
  .mem_rd_par (8'd0),

  .invalidate_and_wait (1'd0)
  );

// Simple memory transactor

reg [3:0] mem_state;

parameter
  MEM_IDLE = 0,
  MEM_WR_1 = 1,
  MEM_WR_2 = 2,
  MEM_WR_3 = 3,
  MEM_WR_4 = 4,
  MEM_RD_1 = 5,
  MEM_RD_2 = 6,
  MEM_RD_3 = 7,
  MEM_RD_4 = 8
  ;

always @(posedge clk or negedge reset_l)
  if (!reset_l)
    begin
      mem_gnt <= 0;
      mem_rd_data <= 0;
      mem_state <= MEM_IDLE;
    end
  else
    begin
      mem_gnt <= 0;
      case (mem_state)
        MEM_IDLE:
          if (mem_req)
            if (mem_write)
              begin // Write request
                mem_gnt <= 1;
                mem_state <= MEM_WR_1;
              end
            else
              begin // Read request
                mem_gnt <= 1;
                mem_state <= MEM_RD_1;
              end

        MEM_WR_1: // Grant high this cycle; take first write data
          begin
            mem_state <= MEM_WR_2;
          end

        MEM_WR_2:
          begin
            mem_state <= MEM_WR_3;
          end

        MEM_WR_3:
          begin
            mem_state <= MEM_WR_4;
          end

        MEM_WR_4:
          begin
            mem_state <= MEM_IDLE;
          end

        MEM_RD_1:
          begin
            mem_state <= MEM_RD_2;
            case (mem_addr[4:3])
              2'd0: mem_rd_data <= { mem_addr, 4'd1 };
              2'd1: mem_rd_data <= { mem_addr, 4'd2 };
              2'd2: mem_rd_data <= { mem_addr, 4'd3 };
              2'd3: mem_rd_data <= { mem_addr, 4'd4 };
            endcase
          end

        MEM_RD_2:
          begin
            mem_state <= MEM_RD_3;
            case (mem_addr[4:3])
              2'd0: mem_rd_data <= { mem_addr, 4'd2 };
              2'd1: mem_rd_data <= { mem_addr, 4'd3 };
              2'd2: mem_rd_data <= { mem_addr, 4'd4 };
              2'd3: mem_rd_data <= { mem_addr, 4'd1 };
            endcase
          end

        MEM_RD_3:
          begin
            mem_state <= MEM_RD_4;
            case (mem_addr[4:3])
              2'd0: mem_rd_data <= { mem_addr, 4'd3 };
              2'd1: mem_rd_data <= { mem_addr, 4'd4 };
              2'd2: mem_rd_data <= { mem_addr, 4'd1 };
              2'd3: mem_rd_data <= { mem_addr, 4'd2 };
            endcase
          end

        MEM_RD_4:
          begin
            mem_state <= MEM_IDLE;
            case (mem_addr[4:3])
              2'd0: mem_rd_data <= { mem_addr, 4'd4 };
              2'd1: mem_rd_data <= { mem_addr, 4'd1 };
              2'd2: mem_rd_data <= { mem_addr, 4'd2 };
              2'd3: mem_rd_data <= { mem_addr, 4'd3 };
            endcase
          end
      endcase
    end

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

task read;
input [19:0] addr;
  begin
    @(posedge clk);
    client_req <= 1;
    client_write <= 0;
    client_addr <= addr;
    begin: block
      forever
        begin
          @(posedge clk);
          if (client_gnt)
            disable block;
        end
    end
    client_req <= 0;
    @(posedge clk);
    $display("Read %x from %x",client_rd_data,addr);
    @(posedge clk);
    $display("Read %x from %x",client_rd_data,addr+8);
    @(posedge clk);
    $display("Read %x from %x",client_rd_data,addr+16);
    @(posedge clk);
    $display("Read %x from %x",client_rd_data,addr+24);
  end
endtask

task write;
input [19:0] addr;
input [255:0] data;
  begin
    @(posedge clk);
    client_req <= 1;
    client_write <= 1;
    client_addr <= addr;
    client_wr_data <= data[63:0];
    client_blast <= 1;
    begin: block
      forever
        begin
          @(posedge clk);
          if (client_gnt)
            disable block;
        end
    end
    client_req <= 0;
    client_wr_data <= data[127:64];
    @(posedge clk);
    client_wr_data <= data[191:128];
    @(posedge clk);
    client_wr_data <= data[255:192];
  end
endtask

initial
  begin
//    $dumpvars(0);
//    $dumpon;
    $display("Hi there!");
    clk <= 0;
    reset_l <= 1;
    client_req <= 0;
    client_write <= 0;
    client_addr <= 0;
    client_wr_data <= 0;

    @(posedge clk);
    @(posedge clk);
    reset_l <= 0;
    @(posedge clk);
    @(posedge clk);
    reset_l <= 1;
    @(posedge clk);
    @(posedge clk);

    read(20'h00040);
//    read(20'h00020); // Should be slow
//    read(20'h00020); // Should be fast
    write(20'h00038, 'h99); // Write through
    read(20'h00020);
    read(20'h00040);
    read(20'h10020);


    for (x = 0; x != 20; x = x + 1)
      @(posedge clk);


    $finish;
  end

endmodule
