// PCI arbiter

module pci_arbiter
  (
  clk,
  wait_for_it,
  reset_l,
  frame_l,
  irdy_l,
  req_l,
  gnt_l
  );

parameter NCHANS = 7;

input clk;

input wait_for_it;

input reset_l;

input [NCHANS-1:0] req_l;

input frame_l;

input irdy_l;
       
output [NCHANS-1:0] gnt_l;
reg [NCHANS-1:0] gnt_l;

reg [2:0] state /* synthesis syn_encoding="onehot" */;

parameter
  RESET = 0,
  PARK = 1,
  GRANT = 2,
  ACCEPT = 3,
  CHECK = 4;

reg [2:0] chan;

integer x, y;

always @(posedge clk or negedge reset_l)
  if(!reset_l)
    begin
      gnt_l <= ~0;
      chan <= 0;
      state <= RESET;
    end
  else
    case(state)
      RESET:
        if(wait_for_it)
          begin
            chan <= chan + 1;
            if(chan==3'd7)
              begin
                gnt_l <= ~1;
                state <= PARK;
              end
          end

      PARK: // We are parked on device 0.
        if(!frame_l)
          begin
            state <= ACCEPT;
            chan <= 0;
          end
        else
          begin
            // Arbitrate.  Begin where we last left off.
            x = chan;
            for(y=0;y!=NCHANS;y=y+1)
              begin
                if(!req_l[x])
                  begin
                    state <= GRANT;
                    chan <= x;
                    gnt_l <= ~(1<<x);
                  end
                if(x==0)
                  x=NCHANS-1;
                else
                  x=x-1;
              end
          end
      
      GRANT:
        if(!frame_l)
          state <= ACCEPT;

      ACCEPT:
        begin
          gnt_l <= ~0;
          if(irdy_l)
            state <= CHECK;
        end

      CHECK:
        begin
          state <= PARK;
          gnt_l <= ~1;
          // Arbitrate.  Begin where we last left off.
          x = chan;
          for(y=0;y!=NCHANS;y=y+1)
            begin
              if(!req_l[x])
                begin
                  state <= GRANT;
                  chan <= x;
                  gnt_l <= ~(1<<x);
                end
              if(x==0)
                x=NCHANS-1;
              else
                x=x-1;
            end
        end
    endcase

endmodule
