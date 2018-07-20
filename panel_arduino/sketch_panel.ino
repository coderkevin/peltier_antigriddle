/**
 * Panel Controller
 * Sends control data to peltier controllers.
 */
#include <Wire.h>

#define DEVICE_MIN 1
#define DEVICE_MAX 6

#define MSG_KNOB 0x01

void setup()
{
	Serial.begin( 115200 );
	Wire.begin();
	Serial.println( "Panel Ready" );
}

void sendKnob( int value ) {
	Serial.print( "Sending knob value: " );
	Serial.println( value );
	for ( int deviceNum = DEVICE_MIN; deviceNum <= DEVICE_MAX; deviceNum++ ) {
		Wire.beginTransmission( deviceNum );
		Wire.write( MSG_KNOB );
		Wire.write( value );
		Wire.endTransmission();
	}
}

void loop() {
	sendKnob( 50 );
	delay( 1000 );
}
