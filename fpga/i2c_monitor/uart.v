// Improvements:
//
//   Use bresenham baud rate generator
//
//   Sample three times in middle of bit and vote for result (use 16x baud
//   rate or something for this).

module uart
  (
  input reset_l,
  input clk,

  // Baud rate divisor
  input [11:0] baud_rate,
  output reg tick,

  // Tx side
  input [7:0] tx_fifo_rd_data,
  output reg tx_fifo_re,
  input tx_fifo_ne,

  output reg uart_tx,

  // Rx side
  output reg [7:0] rx_fifo_wr_data,
  output reg rx_fifo_we,
  output reg uart_frame_error,

  input rx_en,
  input uart_rx
  );

// Transmit baud rate generator

reg [11:0] counter;

always @(posedge clk or negedge reset_l)
  if(!reset_l)
    begin
      counter <= 0;
      tick <= 0;
    end
  else
    begin
      tick <= 0;
      if(counter!=1)
        counter <= counter - 1;
      else
        begin
          counter <= baud_rate;
          tick <= 1;
        end
    end

// Transmitter

reg [8:0] tx_shift_reg;
reg [3:0] tx_counter;

always @(posedge clk or negedge reset_l)
  if(!reset_l)
    begin
      uart_tx <= 1;
      tx_fifo_re <= 0;
      tx_counter <= 0;
      tx_shift_reg <= 0;
    end
  else
    begin
      tx_fifo_re <= 0;
      if(tx_counter)
        begin
          // We're transmitting
          if(tick)
            begin
              tx_counter <= tx_counter - 1;
              tx_shift_reg <= { 1'd1, tx_shift_reg[8:1] };
              uart_tx <= tx_shift_reg[0];
            end
        end
      else
        begin
          // We're not: try to read FIFO
          if(tx_fifo_ne)
            begin
              tx_shift_reg <= { tx_fifo_rd_data, 1'd0 };
              tx_fifo_re <= 1;
              tx_counter <= 10;
            end
        end
    end

// Receiver

reg uart_rx_1;
reg uart_rx_2;

reg [8:0] rx_shift_reg;
reg [11:0] rx_counter;
reg rx_ing;

always @(posedge clk or negedge reset_l)
  if(!reset_l)
    begin
      uart_rx_1 <= 1;
      uart_rx_2 <= 1;
      rx_shift_reg <= 0;
      rx_counter <= 0;
      rx_ing <= 0;
      rx_fifo_we <= 0;
      rx_fifo_wr_data <= 0;
      uart_frame_error <= 0;
    end
  else
    begin
      // Synchronize input
      uart_rx_2 <= uart_rx;
      uart_rx_1 <= uart_rx_2;
      rx_fifo_we <= 0;
      uart_frame_error <= 0;

      if(rx_ing)
        begin
          if(rx_counter!=1)
            rx_counter <= rx_counter - 1;
          else if(!rx_shift_reg[0])
            // We've got start bit and stop bit
            begin
              rx_ing <= 0;
              rx_fifo_wr_data <= rx_shift_reg[8:1];
              rx_fifo_we <= rx_en;
              if(!uart_rx_1)
                uart_frame_error <= rx_en;
            end
          else
            // Shift in next bit
            begin
              rx_counter <= baud_rate;
              rx_shift_reg <= { uart_rx_1, rx_shift_reg[8:1] };
            end
        end
      else if(!uart_rx_1)
        begin
          // Leading edge of start bit
          // Delay 1/2 bit
          rx_counter <= { 1'd0, baud_rate[11:1] };
          rx_shift_reg <= 9'h1ff;
          rx_ing <= 1;
        end
    end

endmodule
