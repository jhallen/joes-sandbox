// Simple I2C monitor

module test
  (
  clk,
  reset_l,
  led0, // pin 3, "D2": on when low.  Blinks at 1 Hz.
  led1, // pin 7, "D4": on when low.  On if SDA is high
  led2, // pin 9, "D5": on when low.  On if SCL is high
  push1_in, // pin 144, "push1": low when button pushed, needs FPGA to pull up

  scl_in,
  sda_in,

  rx_in,
  tx
  );

input clk;

input reset_l;

output led0;
reg led0;

output led1;
reg led1;

output led2;
reg led2;

input push1_in;

input scl_in;
input sda_in;

input rx_in;
output tx;
wire tx;

// UART

wire [7:0] tx_fifo_rd_data;
wire tx_fifo_re;
wire tx_fifo_ne;

wire [7:0] rx_fifo_wr_data;
wire rx_fifo_we;
wire uart_frame_error;
wire rx_en = 1'd1;

wire baud_pulse;

reg tx_enable;

uart uart
  (
  .reset_l (reset_l),
  .clk (clk),
//  .baud_rate (12'd434), // 50,000,000 / 115,200
//  .baud_rate (12'd217), // 50,000,000 / 230,400
//  .baud_rate (12'd109), // 50,000,000 / 460,800
  .baud_rate (12'd054), // 50,000,000 / 921,600
  .tick (baud_pulse),

  .tx_fifo_rd_data (tx_fifo_rd_data),
  .tx_fifo_re (tx_fifo_re),
  .tx_fifo_ne (tx_fifo_ne && tx_enable),

  .uart_tx (tx),

  .rx_fifo_wr_data (rx_fifo_wr_data),
  .rx_fifo_we (rx_fifo_we),
  .uart_frame_error (uart_frame_error),
  .rx_en (rx_en),
  .uart_rx (rx_in)
  );

// Serial output FIFO

reg [7:0] tx_fifo_wr_data;
reg tx_fifo_we;
wire tx_fifo_full;

fifo_sync_late #(.DATAWIDTH(8), .ADDRWIDTH(13)) tx_fifo
  (
  .clk (clk),
  .reset_l (reset_l),

//  .wr_data (rx_fifo_we ? rx_fifo_wr_data : tx_fifo_wr_data),
//  .we (tx_fifo_we || rx_fifo_we),
  .wr_data (tx_fifo_wr_data),
  .we (tx_fifo_we),
  .full (tx_fifo_full),

  .rd_data (tx_fifo_rd_data),
  .re (tx_fifo_re),
  .ne (tx_fifo_ne)
  );

// Xon/Xoff flow control

always @(posedge clk or negedge reset_l)
  if (!reset_l)
    tx_enable <= 1;
  else if (rx_fifo_we)
    begin
      if (rx_fifo_wr_data == 8'h13) // Ctrl-S
        tx_enable <= 0; // Stop sending
      else // Any other character
        tx_enable <= 1; // Resume sending
    end

// I2C monitor

wire mon_valid;
wire mon_short;
wire [7:0] mon_byte;
wire mon_ack;

wire mon_start;
wire mon_stop;
wire mon_notrans;

wire scl;
wire sda;

i2c_recv i2c_recv
  (
  .clk (clk),
  .reset_l (reset_l),

  .scl_in (scl_in),
  .sda_in (sda_in),

  .scl (scl),
  .sda (sda),

  .mon_valid (mon_valid),
  .mon_byte (mon_byte),
  .mon_ack (mon_ack),
  .mon_short (mon_short),

  .mon_start (mon_start),
  .mon_stop (mon_stop),
  .mon_notrans (mon_notrans)
  );

// Real time clock

reg [22:0] long_counter;
reg tick10;
reg [19:0] rtc; // 999.9 seconds
reg time_check;

always @(posedge clk or negedge reset_l)
  if (!reset_l)
    begin
      long_counter <= 0;
      rtc <= 0;
      tick10 <= 0;
      led0 <= 0;
      led1 <= 0;
      led2 <= 0;
      time_check <= 0;
    end
  else
    begin
      led1 <= !sda;
      led2 <= !scl;
      tick10 <= 0;
      if (long_counter == 23'd4_999_999) // Divide 50 MHz by 5 M to get 10 Hz
        begin
          long_counter <= 0;
          tick10 <= 1;
        end
      else
        long_counter <= long_counter + 1'd1;

      led0 <= rtc[3]; // 1 Hz led blinking...

      if (tick10)
        begin
          if (rtc[7:0] == 8'h99)
            time_check <= !time_check;

          // Increment decimal digits...
          if (rtc[3:0] != 9)
            rtc[3:0] <= rtc[3:0] + 1'd1;
          else
            begin
              rtc[3:0] <= 0;
              if (rtc[7:4] != 9)
                rtc[7:4] <= rtc[7:4] + 1'd1;
              else
                begin
                  rtc[7:4] <= 0;
                  if (rtc[11:8] != 9)
                    rtc[11:8] <= rtc[11:8] + 1'd1;
                  else
                    begin
                      rtc[11:8] <= 0;
                      if (rtc[15:12] != 9)
                        rtc[15:12] <= rtc[15:12] + 1'd1;
                      else
                        begin
                          rtc[15:12] <= 0;
                          if (rtc[19:16] != 0)
                            rtc[19:16] <= rtc[19:16] + 1'd1;
                          else
                            rtc[19:16] <= 0;
                        end
                    end
                end
            end
        end
    end

// Format recieved I2C data...

reg [4:0] fmt_state;

parameter
  FMT_IDLE = 0,
  FMT_T_0 = 1,
  FMT_T_1 = 2,
  FMT_T_2 = 3,
  FMT_T_3 = 4,
  FMT_T_4 = 5,
  FMT_T_5 = 6,
  FMT_T_6 = 7,
  FMT_T_7 = 8,
  FMT_DATA = 9,
  FMT_LOW = 10,
  FMT_ACK = 11,
  FMT_SPC = 12,
  FMT_CR = 13,
  FMT_LF = 14,
  FMT_RPT = 15,
  FMT_A = 16,
  FMT_B = 17;

reg [19:0] fmt_buf;
reg fmt_flag;
reg fmt_time_check;

always @(posedge clk or negedge reset_l)
  if (!reset_l)
    begin
      tx_fifo_wr_data <= 0;
      tx_fifo_we <= 0;
      fmt_state <= FMT_IDLE;
      fmt_buf <= 0;
      fmt_flag <= 0;
      fmt_time_check <= 0;
    end
  else
    begin
      tx_fifo_we <= 0;

      case (fmt_state)
        FMT_IDLE:
          if (mon_start) // We have data..
            if (!tx_fifo_full)
              begin
                fmt_flag <= 0;
                fmt_buf <= rtc;
                tx_fifo_wr_data <= 8'h5b;
                tx_fifo_we <= 1;
                fmt_state <= FMT_T_0;
              end
            else
              begin
                tx_fifo_wr_data <= 8'h56;
                tx_fifo_we <= 1;
                fmt_state <= FMT_CR;
              end
          else if (time_check != fmt_time_check)
            begin // Print time check
              fmt_time_check <= time_check;
              fmt_buf <= rtc;
              tx_fifo_wr_data <= 8'h5b; // [
              tx_fifo_we <= 1;
              fmt_state <= FMT_T_0;
              fmt_flag <= 1;
            end

        FMT_T_0:
          begin
            tx_fifo_wr_data <= 8'h30 + fmt_buf[19:16];
            tx_fifo_we <= 1;
            fmt_state <= FMT_T_1;
          end

        FMT_T_1:
          begin
            tx_fifo_wr_data <= 8'h30 + fmt_buf[15:12];
            tx_fifo_we <= 1;
            fmt_state <= FMT_T_2;
          end

        FMT_T_2:
          begin
            tx_fifo_wr_data <= 8'h30 + fmt_buf[11:8];
            tx_fifo_we <= 1;
            fmt_state <= FMT_T_3;
          end

        FMT_T_3:
          begin
            tx_fifo_wr_data <= 8'h30 + fmt_buf[7:4];
            tx_fifo_we <= 1;
            fmt_state <= FMT_T_4;
          end
          
        FMT_T_4:
          begin
            tx_fifo_wr_data <= 8'h2E; // .
            tx_fifo_we <= 1;
            fmt_state <= FMT_T_5;
          end

        FMT_T_5:
          begin
            tx_fifo_wr_data <= 8'h30 + fmt_buf[3:0];
            tx_fifo_we <= 1;
            fmt_state <= FMT_T_6;
          end

        FMT_T_6:
          begin
            tx_fifo_wr_data <= 8'h5d; // ]
            tx_fifo_we <= 1;
            fmt_state <= FMT_T_7;
          end

        FMT_T_7:
          begin
            tx_fifo_wr_data <= 8'h20;
            tx_fifo_we <= 1;
            if (fmt_flag)
              fmt_state <= FMT_A;
            else
              fmt_state <= FMT_DATA;
          end

        FMT_A:
          begin
            if (scl)
              tx_fifo_wr_data <= 8'h43; // C
            else
              tx_fifo_wr_data <= 8'h63; // c
            tx_fifo_we <= 1;
            fmt_state <= FMT_B;
          end

        FMT_B:
          begin
            if (sda)
              tx_fifo_wr_data <= 8'h44; // D
            else
              tx_fifo_wr_data <= 8'h64; // d
            tx_fifo_we <= 1;
            fmt_state <= FMT_CR;
          end

        FMT_DATA:
          begin
            // Record data
            fmt_buf <= { 7'd0, mon_notrans, mon_start, mon_stop, mon_short, mon_ack, mon_byte };

            if (mon_valid)
              begin
                // We have data, so print it
                if (mon_byte[7:4] >= 10)
                  tx_fifo_wr_data <= 8'h41 + mon_byte[7:4] - 8'd10;
                else
                  tx_fifo_wr_data <= 8'h30 + mon_byte[7:4];
                tx_fifo_we <= 1;
                fmt_state <= FMT_LOW;
              end
            else if (mon_stop)
              // No data, but we have a stop
              fmt_state <= FMT_SPC;
          end

        FMT_LOW:
          begin
            if (fmt_buf[3:0] >= 10)
              tx_fifo_wr_data <= 8'h41 + fmt_buf[3:0] - 8'd10;
            else
              tx_fifo_wr_data <= 8'h30 + fmt_buf[3:0];
            tx_fifo_we <= 1;
            fmt_state <= FMT_ACK;
          end

        FMT_ACK:
          begin
            if (fmt_buf[9])
              tx_fifo_wr_data <= 8'h3f; // Short byte: print ?
            else if (fmt_buf[8])
              tx_fifo_wr_data <= 8'h2d; // NAK
            else
              tx_fifo_wr_data <= 8'h2b; // ACK
            tx_fifo_we <= 1;
            
            fmt_state <= FMT_SPC;
          end

        FMT_SPC:
          if (fmt_buf[10]) // Stop
            begin
              if (fmt_buf[11]) // Repeated start
                begin
                  tx_fifo_wr_data <= 8'h52;
                  tx_fifo_we <= 1;
                  fmt_state <= FMT_RPT;
                end
              else if (fmt_buf[12]) // Normal top, but empty transaction
                begin
                  tx_fifo_wr_data <= 8'h45;
                  tx_fifo_we <= 1;
                  fmt_state <= FMT_CR;
                end
              else // Normal stop
                fmt_state <= FMT_CR;
            end
          else
            begin // No stop..
              tx_fifo_wr_data <= 8'h20;
              tx_fifo_we <= 1;
              fmt_state <= FMT_DATA;
            end

        FMT_RPT:
          begin
            tx_fifo_wr_data <= 8'h20;
            tx_fifo_we <= 1;
            fmt_state <= FMT_DATA;
          end

        FMT_CR:
          begin
            tx_fifo_wr_data <= 'h0d;
            tx_fifo_we <= 1;
            fmt_state <= FMT_LF;
          end

        FMT_LF:
          begin
            tx_fifo_wr_data <= 'h0A;
            tx_fifo_we <= 1;
            fmt_state <= FMT_IDLE;
          end
      endcase
    end

endmodule
