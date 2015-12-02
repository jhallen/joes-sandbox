// Test Stratix-III memory interfaces

`timescale 1ns/1ps

module tb;

reg clk;
reg reset_l;

wire pin;
wire pin_in;
reg pin_out;
reg pin_oe;

pad_iobuf_reg dut
  (
  .reset_l (reset_l),
  .clk (clk),
  .pin (pin),
  .i (pin_in),
  .o (pin_out),
  .oe (pin_oe)
  );

// Clocks

always
  begin
    #4 clk <= !clk;
  end

// Write debug data

initial
  begin
    $shm_open("debug.shm",0,500971520,1);
    $shm_probe(tb, "AC");
  end

// Reset

integer x;

initial
  begin
    reset_l <= 0;
    clk <= 0;
    pin_out <= 0;
    pin_oe <= 0;
//    $dumpvars(0);
//    $dumpon;
    $display("Hi there!");

    @(posedge clk);
    @(posedge clk);
    $display("Reset_l low...");
    reset_l <= 0;
    @(posedge clk);
    @(posedge clk);
    $display("Reset_l high...");
    reset_l <= 1;
    @(posedge clk);
    @(posedge clk);
    pin_oe <= 1;
    @(posedge clk);
    pin_out <= 1;
    @(posedge clk);
    pin_oe <= 0;
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);

    $finish;
  end

endmodule
