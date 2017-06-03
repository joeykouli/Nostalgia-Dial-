
// This program controls the 4 digit 7 segment display using the rotary dial


// rotary dial variables
boolean finishedDialing = false;
int dialCounter = 10;
int const dialInputPin = 13;				// pin used for rotary dial input
int const resetDisplayPin = A5;				// pin used for communication between the 2 arduinos
int const foreward_dialInputPin = A0;		// pin used to send the input of the rotary dial from arduino nr. 1 to arduino nr. 2
int lastState = LOW;
int trueState = LOW;
long lastStateChangeTime = 0;
int dialHasFinishedRotatingAfterMs = 200;
int dialDebounceDelay = 10;
int i = 0;									// digit array index

// debounce variables
unsigned long previousDispYearTime = 0;
unsigned long prevTimeDig1 = 0;
unsigned long prevTimeDig2 = 0;
unsigned long prevTimeDig3 = 0;
unsigned long prevTimeDig4 = 0;

int const digitDebounceDelay = 10;
boolean displayDigits = false;




// the variables that control the four 7 segments
int A_pin = 2;		//                 A
int B_pin = 3;		//              _______
int C_pin = 4;		//            |         |
int D_pin = 5;		//       F    |         |  B
int E_pin = 6;		//            |    G    |
int F_pin = 7;		//            | _______ |
int G_pin = 8;		//            |         |
int GND1 = 9;		//		      |         |
int GND2 = 10;		//		 E    |         |   C
int GND3 = 11;		//            | _______ |
int GND4 = 12;		//       
int num;			//				   D

int digit[4] = { -1, -1, -1, -1 };	// array that stores the value of each digit to display on the 7 segment. Using -1 as initialization
int const digitDelayTime = 1;
unsigned long previousResetTime = 0;


void setup()
{
	pinMode(resetDisplayPin, INPUT);
	pinMode(foreward_dialInputPin, OUTPUT);
	pinMode(dialInputPin, INPUT);
	pinMode(A_pin, OUTPUT);
	pinMode(B_pin, OUTPUT);
	pinMode(C_pin, OUTPUT);
	pinMode(D_pin, OUTPUT);
	pinMode(E_pin, OUTPUT);
	pinMode(F_pin, OUTPUT);
	pinMode(G_pin, OUTPUT);
	pinMode(GND1, OUTPUT);
	pinMode(GND2, OUTPUT);
	pinMode(GND3, OUTPUT);
	pinMode(GND4, OUTPUT);
	Serial.begin(9600);
}
void loop()
{
	reset_display();
	rotary_dial_input();
	//display_year();

	digitalWrite(GND1, HIGH);
	digitalWrite(GND2, HIGH);
	digitalWrite(GND3, HIGH);
	digitalWrite(GND4, HIGH);



	if (Serial.available() > 0)	 // checks if something came across the serial
	{
		num = Serial.parseInt(); // returns the first valid (long) integer number from the serial buffer 
		Serial.println(num);
		// prepares format for displaying the digits in correct order/position on the four 7 segments
		digit[0] = num / 1000;
		num = num - (digit[0] * 1000);
		digit[1] = num / 100;
		num = num - (digit[1] * 100);
		digit[2] = num / 10;
		digit[3] = num - (digit[2] * 10);
	}

	digitalWrite(GND4, LOW);    // displays digit 4 on the 7 segment 
	pickNumber(digit[3]);
	delay(digitDelayTime);
	digitalWrite(GND4, HIGH);

	digitalWrite(GND3, LOW);    // displays  digit 3
	pickNumber(digit[2]);
	delay(digitDelayTime);
	digitalWrite(GND3, HIGH);

	digitalWrite(GND2, LOW);    // displays digit 2
	pickNumber(digit[1]);
	delay(digitDelayTime);
	digitalWrite(GND2, HIGH);

	digitalWrite(GND1, LOW);    // displays digit 1
	pickNumber(digit[0]);
	delay(digitDelayTime);
	digitalWrite(GND1, HIGH);

}


void reset_display() {	// this function resets the display by setting the digit array to -1 and the esential rotary dial variables

	if (millis() - previousResetTime > 200) {	// debounce
		previousResetTime = millis();
		if (digitalRead(resetDisplayPin) == HIGH) {
			for (int j = 0; j < 4; j++) {
				digit[j] = -1;
			}
			lastState = LOW;
			trueState = LOW;
			i = 0;
			finishedDialing = false;
			dialCounter = 10;
		}
	}
}

void pickNumber(int x) {	// this function chooses what number (or "_") to display on a 7 segment
	switch (x) {
	case 1: one(); break;
	case 2: two(); break;
	case 3: three(); break;
	case 4: four(); break;
	case 5: five(); break;
	case 6: six(); break;
	case 7: seven(); break;
	case 8: eight(); break;
	case 9: nine(); break;
	case -1: empty(); break;
	default: zero(); break;
	}
}

void clearLEDs()
{
	for (int i = 2; i < 9; i++) {
		digitalWrite(i, LOW);		// turns all segments off (A-G)
	}
}

void one()	// displays the number 1 on a 7 segment
{
	digitalWrite(A_pin, LOW);
	digitalWrite(B_pin, HIGH);
	digitalWrite(C_pin, HIGH);
	digitalWrite(D_pin, LOW);
	digitalWrite(E_pin, LOW);
	digitalWrite(F_pin, LOW);
	digitalWrite(G_pin, LOW);
}

void two()	// displays the number 2 on a 7 segment
{
	digitalWrite(A_pin, HIGH);
	digitalWrite(B_pin, HIGH);
	digitalWrite(C_pin, LOW);
	digitalWrite(D_pin, HIGH);
	digitalWrite(E_pin, HIGH);
	digitalWrite(F_pin, LOW);
	digitalWrite(G_pin, HIGH);
}

void three()  // displays the number 3 on a 7 segment
{
	digitalWrite(A_pin, HIGH);
	digitalWrite(B_pin, HIGH);
	digitalWrite(C_pin, HIGH);
	digitalWrite(D_pin, HIGH);
	digitalWrite(E_pin, LOW);
	digitalWrite(F_pin, LOW);
	digitalWrite(G_pin, HIGH);
}

void four()  // displays the number 4 on a 7 segment
{
	digitalWrite(A_pin, LOW);
	digitalWrite(B_pin, HIGH);
	digitalWrite(C_pin, HIGH);
	digitalWrite(D_pin, LOW);
	digitalWrite(E_pin, LOW);
	digitalWrite(F_pin, HIGH);
	digitalWrite(G_pin, HIGH);
}

void five()  // displays the number 5 on a 7 segment
{
	digitalWrite(A_pin, HIGH);
	digitalWrite(B_pin, LOW);
	digitalWrite(C_pin, HIGH);
	digitalWrite(D_pin, HIGH);
	digitalWrite(E_pin, LOW);
	digitalWrite(F_pin, HIGH);
	digitalWrite(G_pin, HIGH);
}

void six()  // displays the number 6 on a 7 segment
{
	digitalWrite(A_pin, HIGH);
	digitalWrite(B_pin, LOW);
	digitalWrite(C_pin, HIGH);
	digitalWrite(D_pin, HIGH);
	digitalWrite(E_pin, HIGH);
	digitalWrite(F_pin, HIGH);
	digitalWrite(G_pin, HIGH);
}

void seven()  // displays the number 7 on a 7 segment
{
	digitalWrite(A_pin, HIGH);
	digitalWrite(B_pin, HIGH);
	digitalWrite(C_pin, HIGH);
	digitalWrite(D_pin, LOW);
	digitalWrite(E_pin, LOW);
	digitalWrite(F_pin, LOW);
	digitalWrite(G_pin, LOW);
}

void eight()  // displays the number 8 on a 7 segment
{
	digitalWrite(A_pin, HIGH);
	digitalWrite(B_pin, HIGH);
	digitalWrite(C_pin, HIGH);
	digitalWrite(D_pin, HIGH);
	digitalWrite(E_pin, HIGH);
	digitalWrite(F_pin, HIGH);
	digitalWrite(G_pin, HIGH);
}

void nine()  // displays the number 9 on a 7 segment
{
	digitalWrite(A_pin, HIGH);
	digitalWrite(B_pin, HIGH);
	digitalWrite(C_pin, HIGH);
	digitalWrite(D_pin, HIGH);
	digitalWrite(E_pin, LOW);
	digitalWrite(F_pin, HIGH);
	digitalWrite(G_pin, HIGH);
}

void zero()  // displays the number 0 on a 7 segment
{
	digitalWrite(A_pin, HIGH);
	digitalWrite(B_pin, HIGH);
	digitalWrite(C_pin, HIGH);
	digitalWrite(D_pin, HIGH);
	digitalWrite(E_pin, HIGH);
	digitalWrite(F_pin, HIGH);
	digitalWrite(G_pin, LOW);
}

void empty()  // displays _ on a 7 segment
{

	digitalWrite(A_pin, LOW);
	digitalWrite(B_pin, LOW);
	digitalWrite(C_pin, LOW);
	digitalWrite(D_pin, HIGH);
	digitalWrite(E_pin, LOW);
	digitalWrite(F_pin, LOW);
	digitalWrite(G_pin, LOW);
}

void rotary_dial_input() {   // this function reads input from the rotary dial and puts it into the digit array

	int reading = digitalRead(dialInputPin);
	digitalWrite(foreward_dialInputPin, reading);	// this line of code sends the input of the rotary dial from one arduino to the other
	if ((millis() - lastStateChangeTime) > dialHasFinishedRotatingAfterMs) {  // time needed to wait for the rotary dial to finish rotating

		if (finishedDialing) {	 // if it's only just finished being dialed, send the input to the digit array 		

			Serial.print("dial Counter: ");	// prints the dialed number to serial monitor for debuging
			Serial.println(dialCounter);
			if (i < 3) {
				digit[i] = dialCounter;
				i++;
			}
			else if (i == 3) {
				digit[i] = dialCounter;
				i = 0;
			}
			finishedDialing = false;		//	reset after finished sending the input through
			dialCounter = 10;		//	reset the counter also ( 10 is used since 0 is equivalent to 10 pulses from the dial) 
			delay(1);
		}
	}

	if (reading != lastState) {
		lastStateChangeTime = millis();
	}
	if ((millis() - lastStateChangeTime) > dialDebounceDelay) {
		// debounce - this happens once it's stablized
		if (reading != trueState) {
			// this means that the switch has either just gone from closed->open or vice versa.
			trueState = reading;
			if (trueState == HIGH) {
				// decrement the count of pulses if it's gone high.
				dialCounter--;
				finishedDialing = true; // we'll need to print this number (once the dial has finished rotating)
			}
		}
	}
	lastState = reading;
}



void display_year() {	// this function displays the digit array on the serial monitor for debuging
	int j;
	if (millis() - previousDispYearTime > 1000) {
		previousDispYearTime = millis();
		for (j = 0; j < 4; j++) {
			Serial.print(digit[j], DEC);
			Serial.print(" ");
		}
		Serial.println();
		Serial.print("reading: ");
		Serial.println(digitalRead(13));
	}
}
