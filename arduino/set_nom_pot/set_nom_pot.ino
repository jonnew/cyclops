#define WIPER_UD 12
#define CS 6

void setup() {
	
	// Set pin directions
	pinMode(WIPER_UD, OUTPUT);
	pinMode(CS, OUTPUT);
	
	// Bring ~CS HIGH to start
	cs_high();	
	
	// Set the A-W resitance to 5.6k and save
	set_5point6k();
}

void loop () {
  //set_5point6k();
}

void set_5point6k () {

	// This trimmer is 50k with 64 positions
	// Decrement the wiper 64 times to 0x00
	// ensures that the wiper is connected
	// to terminal B. Incrementing the wiper
	// 64 times ensures that the wiper is
	// connected to terminal A. 
	decrement_pot(64);

	// 5.6k = 50k * x/64. x ~ 7
	increment_pot(64-8);

	// Write to EEPROM
	save_pot_resistance();

}

void cs_low () {
	
	digitalWrite(CS, LOW);
}

void cs_high() {
	
	digitalWrite(CS, HIGH);
}

void unpulse_pot(int n) {

	// Increcment n times
	for (int i = 0; i < n; i++) {
		digitalWrite(WIPER_UD, LOW);
		digitalWrite(WIPER_UD, HIGH);
	}
}

void pulse_pot(int n) {

	// Decrement n times
	for (int i = 0; i < n; i++) {
		digitalWrite(WIPER_UD, HIGH);
		digitalWrite(WIPER_UD, LOW);
	}
}

void increment_pot(int n) {
	
	//Bring WIPER_UD HIGH
	digitalWrite(WIPER_UD, HIGH);

	// Select channel
	cs_low();

	// Provide n U/D pulses
	unpulse_pot(n);	

	// Deselect channel
	cs_high();
}

void decrement_pot(int n) {
	
	// Bring WIPER_UD LOW
	digitalWrite(WIPER_UD, LOW);

	// Select channel
	cs_low();

	// Provide n U/D pulses
	pulse_pot(n);	

	// Deselect channel
	cs_high();
}

void save_pot_resistance() {

	// Bring WIPER_UD HIGH
	digitalWrite(WIPER_UD, HIGH);

	// Select channel
	cs_low();

	// Delay for 10 us
	delayMicroseconds(10);

	// Deselect channel
	cs_high();
}
