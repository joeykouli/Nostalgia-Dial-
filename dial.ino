// This program controls the rotary dial, SD card reader, the amplifier + speakers  through the arduino

#include <SD.h>                      // the SD card library
#define SD_ChipSelectPin 10			 // pin 10 needed for SD card functionality
#include <TMRpcm.h>					 // the library used to play wave files
#include <string.h>					 // including the string library for strcat() and strcpy()


TMRpcm tmrpcm;						 // creates an object of the TMRpcm library to use in this program for play/pause/stop of tracks of the SD card

// Rotary dial variables	
boolean finishedDialing = false;						// variable used for rotary dial 
int dialCounter = 10;						// counter used to display the number dialed
int const dialInputPin = 7;					// pin used for input from rotary dial
int lastState = LOW;						// used in rotary_dial_input() function
int trueState = LOW;						// used in rotary_dial_input() function
unsigned long lastStateChangeTime = 0;		// used in rotary_dial_input() function to store the time
int timeDialFinishedRotating = 200;			// the debounce delay time needed so that the dial finish rotating 
int dialDebounceDelay = 10;					// the debounce delay time needed for each dial count (shortcircuit)

// Other variables
int trackNumber = 0;
char trackName[] = "000.wav";
int year[4] = {-1,-1,-1,-1};				// array used to store the input (year) from the rotary dial. Using -1 as neutral initialization
int i = 0;									// year[] index
int year_prefix = -1;						// the first number (from left to right) in the track name that specifies the year choosen. Using -1 as neutral initialization
boolean paused = false;						// variable used for play/pause functions
int const PAUSE_pin = 6;					// pause button pin on arduino
int const PLAY_pin = 8;						// play button pin on arduino
int const STOP_pin = 3;						// stop button pin on arduino
int const NEXT_TRACK_pin = 5;				// next track button pin on arduino
int const PREVIOUS_TRACK_pin = 4;			// previous track button pin on arduino
int const maxTrackValue = 99;				// max number of tracks
int const minTrackValue = 0;				// min number of tracks
int const resetDisplayPin = 2;				// reset 7 segment display pin on arduino

// debounce delay variables for all multimedia buttons
unsigned long debounceDelay = 200;			
unsigned long previousNextTrackTime = 0;	
unsigned long previousPrevTrackTime = 0;
unsigned long previousStopTrackTime = 0;
unsigned long pausePrevTime = 0;
unsigned long playPrevTime = 0;
unsigned long previousDisplayYearTime = 0;


void setup()
{
	Serial.begin(115200);
	pinMode(dialInputPin, INPUT);
	pinMode(resetDisplayPin, OUTPUT);
	tmrpcm.speakerPin = 9; 
	pinMode(PAUSE_pin, INPUT);
	pinMode(NEXT_TRACK_pin, INPUT);
	pinMode(PREVIOUS_TRACK_pin, INPUT);
	pinMode(STOP_pin, INPUT);
	pinMode(PLAY_pin, INPUT);
	
	if (!SD.begin(SD_ChipSelectPin)) {  // check if the card is present and can be initialized:
		Serial.println("SD fail");
		return;   // don't do anything more if not
	}


}

void loop()
{
	rotary_dial_input();		// get input from the rotary dial
	stop_track();				// stops playing track
	playNext_ifFinished();		// plays next track when a track has finished playing
	pause_track();				// pause the track until PLAY button is pressed
	play_paused_track();		// resume playing track if paused
	play_track();				// plays a track based on the input from the rotary dial
	next_track();				// plays next track when the NEXT button is pushed
	previous_track();			// plays previous track when the PREVIOUS button is pushed

}



void pause_track() {

	if ((millis() - pausePrevTime) > debounceDelay) {	// debounces button
		pausePrevTime = millis();
		if (digitalRead(PAUSE_pin) == HIGH && paused == false) {	// check if pause button is pressed and if track hasn't been paused
			tmrpcm.pause();						// pause track if true
			paused = true;						// mark as paused
			Serial.print("Paused.");
		}
	}
}


void play_paused_track() {

	if ((millis() - playPrevTime) > debounceDelay) {   // debounce button
		playPrevTime = millis();
		if (digitalRead(PLAY_pin) == HIGH && paused == true) {		// check if play button is pressed and if track has been paused 
			tmrpcm.pause();						// play track if true
			paused = false;						// mark as not paused
			Serial.print("Continue playing.");
		}
	}
}

void stop_track() {

	if ((millis() - previousStopTrackTime) > debounceDelay) {  // debounce button
		previousStopTrackTime = millis();
		if (digitalRead(STOP_pin) == HIGH) {	// check if stop button is pressed
			tmrpcm.stopPlayback();		// stop track
			resetYear();				// reset year array
			// reset dial variables
			dialCounter = 10;			
			lastState = LOW;				
			trueState = LOW;
			i = 0;
			year_prefix = -1;			// reset year prefix
			digitalWrite(resetDisplayPin, HIGH);	// reset 7 segment display
		}
		else {
			digitalWrite(resetDisplayPin, LOW);		// don't reset 7 segment display if not
		}
	}
	

}

void play_track() {

	display_year(); // shows the current year on the serial monitor

			if (year[0] == 1 && year[1] == 9 && year[2] == 0) {   // check year array with input from the rotary dial is 1900
				if ( year[3] > -1) {
					trackNumber = 0;
					year_prefix = 0;						// sets year prefix variable to 0 (for the first 0 decimal in the 1900 number)
					itoa(year_prefix, &trackName[0], 10);	// puts in year_prefix at the begyning of the trackName
					strcpy(trackName, "000.wav");			// fills the trackName array with 000.wav to play the first track from 1900
					tmrpcm.play(trackName);					// play first track from year 1900 ("000.wav")
					Serial.print("Playing: ");				// prints to serial monitor the track name playing
					Serial.println(trackName);
					resetYear();							// reset year array to neutral values (-1, -1, -1, -1)
				}
			}

			if (year[0] == 1 && year[1] == 9 && year[2] == 1) {	  // check year array with input from the rotary dial is 1910
				if (year[3] > -1) {
					trackNumber = 0;
					year_prefix = 1;						// sets prefix variable to 1 (for the second 1 decimal in 1910 number)
					itoa(year_prefix, &trackName[0], 10);	// puts the first character in the trackName with the year prefix (eg. 100.wav)
															// but itoa delets .wav from trackName so we need to put it back using strcpy()
					strcpy(trackName, "100.wav");			// fills the trackName array with 100.wav to play the first track from 1910
					tmrpcm.play(trackName);					// play first track from year 1910 ("100.wav")
					Serial.print("Playing: ");				// prints to serial monitor the track
					Serial.println(trackName);
					resetYear();							// reset year array to neutral values (-1, -1, -1, -1)
			
				}
			}
			if (year[0] == 1 && year[1] == 9 && year[2] == 2) {   // check year array with input from the rotary dial is 1920
				if (year[3] > -1) {
					trackNumber = 0;
					year_prefix = 2;
					itoa(year_prefix, &trackName[0], 10);	
					strcpy(trackName, "200.wav");			// same as in the previous if() statement
					tmrpcm.play(trackName);
					Serial.print("Playing: ");
					Serial.println(trackName);
					resetYear();
				}
				
			}

			if (year[0] == 1 && year[1] == 9 && year[2] == 3) {   // check year array with input from the rotary dial 1930
				if (year[3] > -1 ) {
					trackNumber = 0;
					year_prefix = 3;
					itoa(year_prefix, &trackName[0], 10);
					strcpy(trackName, "300.wav");   // same as in the previous if() statement
					tmrpcm.play(trackName);
					Serial.print("Playing: ");
					Serial.println(trackName);
					resetYear();
				}
				
			}

			if (year[0] == 1 && year[1] == 9 && year[2] == 4) {   // check year array with input from the rotary dial is 1940
				if (year[3] > -1) {
					trackNumber = 0;
					year_prefix = 4;
					itoa(year_prefix, &trackName[0], 10);
					strcpy(trackName, "400.wav");	// same as in the previous if() statement
					tmrpcm.play(trackName);
					Serial.print("Playing: ");
					Serial.println(trackName);
					resetYear();
				}
			}

			if (year[0] == 1 && year[1] == 9 && year[2] == 5) {   // check year array with input from the rotary dial is 1950
				if (year[3] > -1) {
					trackNumber = 0;
					year_prefix = 5;
					itoa(year_prefix, &trackName[0], 10);
					strcpy(trackName, "500.wav");	// same as in the previous if() statement
					tmrpcm.play(trackName);
					Serial.print("Playing: ");
					Serial.println(trackName);
					resetYear();
				}
			}


			if (year[0] == 1 && year[1] == 9 && year[2] == 6) {   // check year array with input from the rotary dial is 1960
				if (year[3] > -1) {
					trackNumber = 0;
					year_prefix = 6;
					itoa(year_prefix, &trackName[0], 10);
					strcpy(trackName, "600.wav");	// same as in the previous if() statement
					tmrpcm.play(trackName);
					Serial.print("Playing: ");
					Serial.println(trackName);
					resetYear();
				}
			}

			if (year[0] == 1 && year[1] == 9 && year[2] == 7) {   // check year array with input from the rotary dial is 1970
				if (year[3] > -1) {
					trackNumber = 0;
					year_prefix = 7;
					itoa(year_prefix, &trackName[0], 10);
					strcat(trackName, "700.wav");	// same as in the previous if() statement
					tmrpcm.play(trackName);
					Serial.print("Playing: ");
					Serial.println(trackName);
					resetYear();
				}
			}

			if (year[0] == 1 && year[1] == 9 && year[2] == 8) {   // check year array with input from the rotary dial is 1980
				if (year[3] > -1) {
					trackNumber = 0;
					year_prefix = 8;
					itoa(year_prefix, &trackName[0], 10);
					strcpy(trackName, "800.wav");	// same as in the previous if() statement
					tmrpcm.play(trackName);
					Serial.print("Playing: ");
					Serial.println(trackName);
					resetYear();
				}
			}
			
			if (year[0] == 1 && year[1] == 9 && year[2] == 9) {   // check year array with input from the rotary dial is 1990
				if (year[3] > -1) {
					trackNumber = 0;
					year_prefix = 9;
					itoa(year_prefix, &trackName[0], 10);
					strcpy(trackName, "900.wav");	// same as in the previous if() statement
					tmrpcm.play(trackName);
					Serial.print("Playing: ");
					Serial.println(trackName);
					resetYear();
				}
			}
}


void playNext_ifFinished() {	// this function plays the next track when the playing track finished playing

	if (year_prefix != -1) {							// checks year prefix to decide from which year to play the next track based on the previous played track
		if (!tmrpcm.isPlaying()) {						// checks if a song is playing now
			if (trackNumber == maxTrackValue) {			// checks if it reached value 99
				trackNumber = minTrackValue;			// reset track to 0 if true
				itoa(trackNumber, &trackName[1], 10);	// converts an int value to string and puts it into position 1 in the trackName array
			}
			else {
				trackNumber++;							// increases track counter
			}
			if (trackNumber < 10) {						// check if trackNumber is 1 decimal in length
				itoa(trackNumber, &trackName[2], 10);	// put it in the 2 position in the trackName array if true
			}
			else {
				itoa(trackNumber, &trackName[1], 10);	// put it in the 1 position in the trackName array if not
			}

			strcat(trackName, ".wav");					// add the .wav at the end of the track number since itoa function delets the ".wav" after insertion of trackNumber
			tmrpcm.play(trackName);						// play track
			Serial.print("Playing: ");					// prints track name to serial monitor
			Serial.println(trackName);
		}
	}
	
}

void next_track() {		// this function plays the next track in the list if the "next" button is pressed

	if ((millis() - previousNextTrackTime) > 200) {		// debounce delay
		previousNextTrackTime = millis();
		if (digitalRead(NEXT_TRACK_pin) == HIGH) {	// check if "next" button is pressed
			if (trackNumber == maxTrackValue) {			// if trackNumber reached 99
				trackNumber = minTrackValue;			// reset it to 0 if true
				itoa(0, &trackName[1], 10);				// reset also trackName array by placing a 0 on index 1
				itoa(0, &trackName[2], 10);				// replace number on index 2 in trackName with 0 also
			}
			else {
				trackNumber++;							// increase counter if trackNumber hasn't reach 99 yet
			}
			if (trackNumber < 10) {						// if trackNumber is less than 10 
				itoa(trackNumber, &trackName[2], 10);	// put the trackNumber starting from possition 2 in the trackName array 
			}
			else {
				itoa(trackNumber, &trackName[1], 10);	// if not than put it in position 1 in the trackName
			}

			strcat(trackName, ".wav");					// add .wav at the end
			tmrpcm.play(trackName);						// play track
			Serial.print("Playing: ");
			Serial.println(trackName);
		}
	}
}

void previous_track() {   // this function plays the previous track in the list if the "previous" button is pressed

	if ((millis() - previousPrevTrackTime) > 200) {		// debounce delay
		previousPrevTrackTime = millis();
		if (digitalRead(PREVIOUS_TRACK_pin) == HIGH) {	// same analogy as in next_track() function
			if (trackNumber == minTrackValue) {
				trackNumber = maxTrackValue;
				itoa(trackNumber, &trackName[1], 10);
			}
			else {
				trackNumber--;
			}
			if (trackNumber < 10) {
				itoa(trackNumber, &trackName[2], 10);
			}
			else {
				itoa(trackNumber, &trackName[1], 10);
			}
			strcat(trackName, ".wav");
			tmrpcm.play(trackName);
			Serial.print("Playing: ");
			Serial.println(trackName);
		}
	}
}

void resetYear() {   // this function resets the year array to (-1 -1 -1 -1)
	for (int j = 0; j < 4; j++)
		year[j] = -1;
}

void display_year() {	// this function displays the year array on the serial monitor for debuging

	if (millis() - previousDisplayYearTime > 5000) {
		for (int j = 0; j < 4; j++) {
			Serial.print(year[j], DEC);
			Serial.print(" ");
		}
		Serial.println();
		previousDisplayYearTime = millis();
	}
}

void rotary_dial_input() {	// this function reads input from the rotary dial and puts it into the year array

	int reading = digitalRead(dialInputPin);
	if ((millis() - lastStateChangeTime) > timeDialFinishedRotating) {  // time needed to wait for the rotary dial to finish rotating
		if (finishedDialing) {  // if it's only just finished being dialed, send the input to the year array 		
			Serial.println(dialCounter); // prints the dialed number to serial monitor
			if (i < 3) {
				year[i] = dialCounter;
				i++;
			}
			else if (i == 3) {
				year[i] = dialCounter;
				i = 0;
			}

			finishedDialing = false;	// reset after finished sending the input through
			dialCounter = 10;	// dialCounter will be reseted to 10 instead of 0 since 0 will give 10 pulses 
			delay(1);
		}
	}

	if (reading != lastState) {	// checks if the dial is being rotated

		lastStateChangeTime = millis();
	}
	if ((millis() - lastStateChangeTime) > dialDebounceDelay) {  // debounce once it's stablized	
		if (reading != trueState) {
			// this means that the switch has either just gone from closed->open or vice versa.
			trueState = reading;
			if (trueState == HIGH) {
				// decrement the count of pulses if it's gone high.
				dialCounter--;
				finishedDialing = true; // we'll need to print/send the number dialed when the dial has finished rotating)
			}
		}
	}
	lastState = reading;

}
