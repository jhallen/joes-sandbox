// BAUD rate generator

module baud
  (
  reset_l,
  clk,

  tick
  );

parameter OSC = 50000000; // Oscillator frequency
parameter BAUD = 115200; // Desired baud rate

input reset_l;
input clk;

output tick;
reg tick;

reg [26:0] accu; // large enough to hold -OSC
// divide OSC and BAUD by GCD(OSC, BAUD) to reduce number
// of bits needed for this.

always @(posedge clk or negedge reset_l)
  if (!reset_l)
    begin
      accu <= 0;
      tick <= 0;
    end
  else
    if (accu[26]) // Negative
      begin
        accu <= accu + BAUD;
        tick <= 0;
      end
    else // Positive
      begin
        accu <= accu + BAUD - OSC;
        tick <= 1;
      end

endmodule
