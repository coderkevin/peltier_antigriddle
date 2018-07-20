/**
 * Peltier Cell Controller
 * Controls 2 Peltier Units
 */
#include <avr/io.h>
#include <Wire.h>

#define DEVICE_ID 2

#define MSG_KNOB 0x01

/*
#define TEMP1_PIN 2 // A2, 10ktherm & 10k resistor as divider
#define TEMP2_PIN 3 // A3, 10ktherm & 10k resistor as divider
#define TEMP3_PIN 6 // A6, 10ktherm & 10k resistor as divider
#define TEMP4_PIN 7 // A7, 10ktherm & 10k resistor as divider
*/

#define PWM1_OUT OCR2B
#define PWM2_OUT OCR0B

#define MIN_PWM 0
#define MAX_PWM 127
#define MIN_PWM_RANGE 80
#define MAX_PWM_RANGE 127

// Temporary sinusoidal peltier driver code.
const double secondsPerCycle = 10;
const int updateDelay = 100;
const double cycleIncrement = (1000/ updateDelay) * secondsPerCycle;
const double radianIncrement = (2 * PI) / cycleIncrement;  

double rads = 0;

/*
double calculateThermistorTemp( int rawADCValue, bool useFahrenheit = false ) {
	double temp;
	temp = log( 10000.0 * ( ( 1024.0 / rawADCValue - 1 ) ) ); 
	temp = 1 / ( 0.001129148 + ( 0.000234125 + ( 0.0000000876741 * temp * temp ) ) * temp );
	temp = temp - 273.15; // Convert Kelvin to Celcius
	if ( useFahrenheit ) {
		temp = ( temp * 9.0 ) / 5.0 + 32.0; // Convert Celcius to Fahrenheit
	}
	return temp;
}
*/

void setup()
{
	pinMode( 3, OUTPUT ); // PWM1
	pinMode( 5, OUTPUT ); // PWM2
/*
	pinMode( 16, INPUT ); // TEMP1, A2
	pinMode( 17, INPUT ); // TEMP2, A3
	pinMode( 20, INPUT ); // TEMP3, A6
	pinMode( 21, INPUT ); // TEMP4, A7
*/

	Serial.begin( 115200 );
	Wire.begin( DEVICE_ID );
	Wire.onReceive( receiveMessage );

	// Set up the 125 kHz output for pin 3 / PWM1
	TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
	TCCR2B = _BV(WGM22) | _BV(CS20);
	OCR2A = MAX_PWM;
	OCR2B = MIN_PWM;

	// Set up the 125 kHz output for pin 5 / PWM2
	TCCR0A = _BV(COM0A1) | _BV(COM0B1) | _BV(WGM01) | _BV(WGM00);
	TCCR0B = _BV(WGM02) | _BV(CS00);
	OCR0A = MAX_PWM;
	OCR0B = MIN_PWM;

	Serial.print( "Peltier Controller #" );
	Serial.print( DEVICE_ID );
	Serial.println( " Ready" );
}

void loop() {
/*
  double temp1 = calculateThermistorTemp( analogRead( TEMP1_PIN ) );
  double temp2 = calculateThermistorTemp( analogRead( TEMP2_PIN ) );
  double temp3 = calculateThermistorTemp( analogRead( TEMP3_PIN ) );
  double temp4 = calculateThermistorTemp( analogRead( TEMP4_PIN ) );

  Serial.print( temp1, 2 );
  Serial.print( ' ' );
  Serial.print( temp2, 2 );
  Serial.print( ' ' );
  Serial.print( temp3, 2 );
  Serial.print( ' ' );
  Serial.print( temp4, 2 );
  Serial.println();
*/

  rads += radianIncrement;

  double value = (sin(rads) + 1) / 2;
  int pwmValue = ((MAX_PWM_RANGE - MIN_PWM_RANGE) * value) + MIN_PWM_RANGE;

  PWM1_OUT = pwmValue;
  PWM2_OUT = pwmValue;

  delay( updateDelay );
}

void receiveMessage( int howMany ) {
	while( Wire.available() ) {
		int msgType = Wire.read();
		int value = 0;
		switch( msgType ) {
			case MSG_KNOB:
				value = Wire.read();
				Serial.print( "Knob value: " );
				Serial.println( value );
				return;
			default:
				Serial.print( "Unrecognized message type: " );
				Serial.println( msgType );
				return;
		}
	}
}
