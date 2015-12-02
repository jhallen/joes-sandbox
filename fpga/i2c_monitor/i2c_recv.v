module i2c_recv
  (
  clk,
  reset_l,

  scl_in,
  sda_in,

  scl,
  sda,

  mon_valid, // Pulse for each byte recevied
  mon_byte, // Byte
  mon_ack, // Ack bit after byte
  mon_short, // High along with mon_valid for incomplete byte

  mon_start, // Pulse for start condition
  mon_stop, // Pulse for stop condition
    // start and stop together for repeated start
  mon_notrans // High along with mon_stop for 0-length transaction
  );

input clk;
input reset_l;

input scl_in;
input sda_in;

output scl;
output sda;

output mon_valid;
reg mon_valid;

output [7:0] mon_byte;
reg [7:0] mon_byte;

output mon_ack;
reg mon_ack;

output mon_start;
reg mon_start;

output mon_stop;
reg mon_stop;

output mon_short;
reg mon_short;

output mon_notrans;
reg mon_notrans;

reg scl_4;
reg scl_3;
reg scl_2;
reg scl_1;
reg scl;
reg scl_old;

reg sda_4;
reg sda_3;
reg sda_2;
reg sda_1;
reg sda;
reg sda_old;

reg [1:0] i2c_state;
parameter
  I2C_IDLE = 0,
  I2C_WAIT_CLOCK = 1,
  I2C_RUN = 2;

reg [3:0] rx_count;

always @(posedge clk or negedge reset_l)
  if (!reset_l)
    begin
      scl_4 <= 1;
      scl_3 <= 1;
      scl_2 <= 1;
      scl_1 <= 1;
      scl <= 1;
      scl_old <= 1;
      sda_4 <= 1;
      sda_3 <= 1;
      sda_2 <= 1;
      sda_1 <= 1;
      sda <= 1;
      sda_old <= 1;

      i2c_state <= I2C_IDLE;

      rx_count <= 0;

      mon_valid <= 0;
      mon_byte <= 0;
      mon_ack <= 0;

      mon_start <= 0;
      mon_stop <= 0;
      mon_short <= 0;
      mon_notrans <= 0;
    end
  else
    begin
      scl_4 <= scl_in;
      scl_3 <= scl_4;
      scl_2 <= scl_3;
      scl_1 <= scl_2;
      scl <= (scl_3 ? (scl_2 || scl_1) : (scl_2 && scl_1)); // High if 2 or more bits high
      scl_old <= scl;

      sda_4 <= sda_in;
      sda_3 <= sda_4;
      sda_2 <= sda_3;
      sda_1 <= sda_2;
      sda <= (sda_3 ? (sda_2 || sda_1) : (sda_2 && sda_1)); // High if 2 or more bits high
      sda_old <= sda;

      mon_valid <= 0;
      mon_start <= 0;
      mon_stop <= 0;
      mon_short <= 0;
      mon_notrans <= 0;

      case (i2c_state)
      	I2C_IDLE: // Wait for falling SDA while SCL is high: start condition
      	  begin
            if (scl == 1 && sda == 0 && sda_old == 1) // Falling SDA while SCL is high
              begin
                i2c_state <= I2C_WAIT_CLOCK;
                mon_start <= 1;
              end
      	  end

      	I2C_WAIT_CLOCK: // Skip first falling SCL after start condition..
      	  begin
      	    if (scl == 1 && sda == 1 && sda_old == 0) // Rising SDA while SCL is high
      	      begin // Early stop?
      	        i2c_state <= I2C_IDLE;
      	        mon_notrans <= 1;
      	        mon_stop <= 1;
      	        mon_byte <= 0; // Stop w/o any transaction
      	      end
      	    else if (scl == 0 && scl_old == 1) // Falling SCL
      	      begin
      	        i2c_state <= I2C_RUN;
      	        mon_byte <= 0;
      	        mon_ack <= 0;
      	        rx_count <= 0;
      	      end
      	  end

      	I2C_RUN: // Clock data on falling SCLs, look for stop condition
      	  begin
            if (scl == 1 && sda == 1 && sda_old == 0) // Rising SDA while SCL is high
              begin // STOP detected
                i2c_state <= I2C_IDLE;
                mon_stop <= 1;

                if (rx_count)
                  begin
                    // Give incomplete byte/ack
                    mon_valid <= 1;
                    mon_short <= 1;
                    { mon_byte, mon_ack } <= { mon_byte[6:0], mon_ack, 1'd0 };
                  end
              end
            else if (scl == 1 && sda == 0 && sda_old == 1) // Falling SDA while SCL is high
              begin // Repeated START detected
                i2c_state <= I2C_WAIT_CLOCK;
                mon_stop <= 1;
                mon_start <= 1;

                if (rx_count)
                  begin
                    mon_valid <= 1;
                    mon_short <= 1; // Incomplete data
                    { mon_byte, mon_ack } <= { mon_byte[6:0], mon_ack, 1'd0 };
                  end
              end
            else if (scl == 0 && scl_old == 1) // Falling clock edge
              begin // Normal data bit transferred
                rx_count <= rx_count + 1'd1;
                { mon_byte, mon_ack } <= { mon_byte[6:0], mon_ack, sda };
                if (rx_count == 8)
                  begin
                    mon_valid <= 1;
                    rx_count <= 0;
                  end
              end
      	  end
      endcase
    end

endmodule
