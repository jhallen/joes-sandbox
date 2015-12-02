// Provide access to Altera MAX-ii built-in flash memory

module flash
  (
  osc,
  start,

  reread,
  bank_select, // Bank select bit output

  bus_in, // 8-bits in
  data_out, // Data output

  load_addr, // Load 8-bits into address register (upper first)
  load_data, // Load 8-bits into data register (upper first)
  read_req, // Read from flash
  write_req, // Write to flash
  erase_req, // Erase sector
  busy // High if busy erasing or programming
  );

output osc;
output start;

input reread;

output bank_select;
reg bank_select;

input [7:0] bus_in;
output [15:0] data_out;
reg [15:0] data_out;
input load_addr;
input load_data;
input read_req;
input write_req;
input erase_req;

output  busy;

wire osc;

// Start pulse generator

reg start_2;
reg start_1;
reg start;

initial
  begin
    start_2 <= 0;
    start_1 <= 0;
    start <= 0;
  end

always @(posedge osc)
  begin
    start_2 <= 1;
    start_1 <= start_2;
    start <= start_1;
  end

// Flash memory

reg arclk;
reg ardin;
reg drclk;
reg drdin;
reg drshft;
reg erase;
reg prog;
wire flash_busy;
wire drdout;
wire rtpbusy;

ufm ufm
  (
  .arclk (arclk),
  .ardin (ardin), // 9-bits, MSB first.  Bit A8 is sector select.
  .arshft (1'd1), // Always shift
  .drclk (drclk),
  .drdin (drdin), // 16-bits, MSB first
  .drshft (drshft), // High means shift, low means read (on drclk rising edge)
  .erase (erase), // Erase on rising edge
  .oscena (1'd1), // Clock always enabled
  .program (prog), // Write on rising edge
  .busy (flash_busy), // High during program or erase (do not clock arclk drclk)
  .drdout (drdout),
  .osc (osc),
  .rtpbusy (rtpbusy) // High means real-time ISP is accessing flash
  );

// State machine

reg [2:0] state;
reg [3:0] count;

reg [6:0] tmp;

reg [1:0] control_busy;

parameter
  RESET = 0,
  CLEAR_AR = 1,
  READ_BANK = 2,
  IDLE = 3,
  LOAD_AR = 4,
  LOAD_DR = 5,
  READ_WORD = 6,
  READ_SHIFT = 7
  ;

initial
  begin
    arclk <= 0;
    ardin <= 0;
    drclk <= 0;
    drdin <= 0;
    drshft <= 0;
    erase <= 0;
    prog <= 0;
    count <= 0;
    state <= RESET;
    bank_select <= 0;
    tmp <= 0;
    data_out <= 0;
    control_busy <= 0;
  end

always @(posedge osc)
  begin
    case (state)
      RESET: // On reset, read the bank bit
        if (start)
          begin
            count <= 8; // Load 9 zeros into address register
            arclk <= 1;
            state <= CLEAR_AR;
          end

      CLEAR_AR:
        if (arclk)
          begin
            arclk <= 0;
          end
        else
          begin
            count <= count - 1'd1;
            if (count)
              begin
                arclk <= 1;
              end
            else
              begin
                drclk <= 1; // Address loaded in, now read...
                state <= READ_BANK;
              end
          end

      READ_BANK:
        begin
          state <= IDLE;
          drclk <= 0;
          bank_select <= drdout;
        end

      IDLE:
        begin
          if (control_busy)
            control_busy <= control_busy - 1'd1;
          drshft <= 0;
          prog <= 0;
          erase <= 0;
          ardin <= bus_in[7];
          drdin <= bus_in[7];
          tmp <= bus_in[6:0];
          count <= 7;
          if (reread)
            begin
              state <= RESET;
              ardin <= 0;
            end
          else if (load_addr)
            begin
              state <= LOAD_AR;
              control_busy <= 1;
            end
          else if (load_data)
            begin
              state <= LOAD_DR;
              drshft <= 1;
              control_busy <= 1;
            end
          else if (read_req)
            begin
              drclk <= 1; // drclk normally low... drshft normally low...
              state <= READ_WORD;
              control_busy <= 1;
            end
          else if (write_req)
            begin
              prog <= 1;
              control_busy <= 3;
            end
          else if (erase_req)
            begin
              erase <= 1;
              control_busy <= 3;
            end
        end

      LOAD_AR:
        begin
          if (arclk)
            begin
              arclk <= 0;
              { ardin, tmp[6:0] } <= { tmp[6:0], 1'd0 };
              count <= count - 1'd1;
              if (!count)
                state <= IDLE;
            end
          else
            begin
              arclk <= 1;
            end
        end

      LOAD_DR:
        begin
          if (drclk)
            begin
              drclk <= 0;
              { drdin, tmp[6:0] } <= { tmp[6:0], 1'd0 };
              count <= count - 1'd1;
              if (!count)
                state <= IDLE;
            end
          else
            begin
              drclk <= 1;
            end
        end

      READ_WORD:
        if (drclk)
          begin
            drclk <= 0;
            drshft <= 1;
            count <= 14;
            data_out <= { data_out[14:0], drdout };
            state <= READ_SHIFT;
          end

      READ_SHIFT:
        if (drclk)
          begin
            drclk <= 0;
            data_out <= { data_out[14:0], drdout };
            count <= count - 1'd1;
            if (!count)
              begin
                drshft <= 0;
                state <= IDLE;
              end
          end
        else
          begin
            drclk <= 1;
          end
    endcase
  end

wire busy = flash_busy || (control_busy != 0);

endmodule
