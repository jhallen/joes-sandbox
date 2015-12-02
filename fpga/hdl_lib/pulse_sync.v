// Move pulse from one clock domain to another.
// No false pulses from reset.

module pulse_sync
  (
  i,
  i_clk,
  i_reset_l,
  o,
  o_clk,
  o_reset_l
  );

input i;
input i_clk;
input i_reset_l;
output o;
reg o;
input o_clk;
input o_reset_l;

reg req;
reg req_2;
reg req_1;
reg req_0;
reg ack;
reg ack_2;
reg ack_1;

always @(posedge i_clk or negedge i_reset_l)
  if (!i_reset_l)
    begin
      req_2 <= 0;
      ack <= 0;
      ack_1 <= 0;
    end
  else
    begin
      if (i)
        req_2 <= 1;
      if (ack)
        req_2 <= 0;
      ack_1 = ack_2;
      ack <= ack_1;
    end

initial
  begin
    req_1 <= 0;
    req_0 <= 0;
    req <= 0;
    ack_2 <= 0;
    o <= 0;
  end

always @(posedge o_clk or negedge o_reset_l)
  if (!o_reset_l)
    begin
      req_1 <= 0;
      req_0 <= 0;
      req <= 0;
      ack_2 <= 0;
      o <= 0;
    end
  else
    begin
      req_1 <= req_2;
      req_0 <= req_1;
      req <= req_0;
      if (req_0 && !req)
        begin
          o <= 1;
          ack_2 <= 1;
        end
      else
        o <= 0;
      if (!req_0)
        ack_2 <= 0;
    end

endmodule
