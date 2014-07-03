// edis controller code.
// pip is every tooth, so 36 times per revolution.  At 6000 rpm, that's 3600 pulses per second.
// Strategy is to make a 2d array, one being rpm, the other being load, with no hard-coded rpm or load information, just
// a slope/offset/digitization into each one.  (maybe compute as a 0-1 function, then multiply by the number of rows and columns, 
// then turn into an integer and read that row or column.)

// program continuously reads throttle position.  When a negative interrupt is caught, store microseconds, then the array number 
//  (which is the pause time in microseconds) is retrieved.  Pin set high, delay for pause time, pin set low, exit interrupt.

// SAW ranges from 64 to 1792 microseconds with max resolution of 4 microseconds.  That's 432 steps.  If I use 8 microsecond resolution
// I can just use ints to store my table (although I have so much space I could use longs.)
// I would have to multiply the result by four if I go with ints.  If I go with longs I get to use all the resolution and don't have
// to multiply -- values would directly reflect the pause times.

// I could also store the degrees advance in the table, then multiply to generate the values, but why?  I can back-calculate that
// if necessary.


volatile long PIP_period;    // this is 36 interrupts per revolution
volatile long microseconds;  // time between PIP signals
volatile int throttle_position;   // rotary encoder on throttle
int output_pin = 1;          // SAW output

void setup()
{
  attachInterrupt(0, count_interrupt, FALLING);
  pinMode(output_pin, OUTPUT);
}

void count_interrupt()
{
  int delaytime;
  PIP_period = micros() - microseconds;
  delaytime = lookup_delay(PIP_period, throttle_position);
  digitalWrite(output_pin, HIGH);
  delayMicroseconds(delaytime);
  digitalWrite(output_pin, LOW);
  microseconds = micros();
}

int lookup_delay(long PIP_period, int throttle_position)
{
  int advance[2][8] = 
  {{10,11,12,13,14,15,16,17}, 
   {20,21,22,23,24,25,26,27}};
  int rows, columns;
  float slope, offset;
  slope = 3.7;
  offset = 1.6;
  
  rows = int((slope * PIP_period) + offset);
  columns = throttle_position;
  if (rows < 0) rows = 0;
  if (rows > 7) rows = 7;
  return(advance[rows] [columns]);
  
}

int throttle_position_read()
{
  int angle;
  angle = AS5040_read();
  return angle;
}

int AS5040_read()
{
  return 50;
}

void loop()
{
  throttle_position = throttle_position_read();
}
