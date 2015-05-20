/* SDPovpoi by Ashley M. Kirchner, April 2014
* http://www.ka4designs.com
* email: info@ka4designs.com
* Copyright (c) 2015 KA4 Designs
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#define __DELAY_BACKWARD_COMPATIBLE__
#include <avr/io.h>
#include <util/delay.h>

#include "globals.h"
#include "revisions.h";

// Battery check
// This will show a battery voltage gauge when the unit is first turned on
// Turn on gauge display
#define BATTERY_GAUGE
// Turn off gauge diaplsy
// #undef BATTERY_GUAGE

#ifdef BATTERY_GAUGE
#include "batteryStatus.h"
#endif

// OneButton
#include <OneButton.h>
OneButton button(9, false);
volatile bool run  = false;
volatile bool mode = true;

// FastLED
#include "FastLED.h"
#define NUM_LEDS 48
CRGB leds[NUM_LEDS + 1];
// For Rev. 1, uncomment this line
//#define DAT_PIN 11
// For Rev. 2, uncomment this line
#define DAT_PIN 6
#define CLK_PIN 12
#define FRAMES_PER_SECOND 60

// SdFat
#include <SdFat.h>
SdFile   cntlFile;
SdFile   myFile;
SdFat    sd;

// File variables
#define CNTRL_FILE "order.ctl"
volatile uint32_t  lastChange       = 0;
volatile float     imgRefresh       = 0.0;
volatile float     imgTimeout       = 0.0;
volatile float     displayPause     = 0.0;
char      fileData[100]    = {};
char      fileName[12]     = {};

// Program structs and unions
struct imageHeader {
	uint16_t imageCols;
};

union SD_buffer_t {
	uint16_t receiveBuffer[1];
	imageHeader hdr;
} SD_buffer;

// Include dynamic patterns
#include "patterns.h"

// Program variables
#define SECOND 1000
const uint8_t      SdChipSelect     = SS;
const uint8_t      errorLED         = 5;
const uint8_t      STRIP_PWR        = 8;

volatile uint8_t   myError          = 0;
volatile uint8_t   cntr             = 0;
volatile uint8_t   brightnessLevel  = 1;
volatile uint8_t   fireState        = 0;
volatile int8_t    fireStyle        = 1;
volatile uint16_t  battIdx          = 0;
volatile uint16_t  pause            = 100;
volatile uint16_t  myRefresh        = 0;
volatile uint16_t  fileStatus       = 0;
volatile uint32_t  builtinLastRun   = 0;
volatile uint32_t  refreshLastRun   = 0;
volatile uint32_t  errorLastRun     = 0;
volatile bool      SDCARD           = 1;  // assume card is present and readable, can catch errors later

// Available error codes:
// 1 - Low battery
// 2 - Can't open file
// 3 - Unknown error reading file

void setup() {
	// Interrupt button for brightness
	attachInterrupt(4, intHandler, RISING);
	// Status button
	button.setClickTicks(200);
	button.attachClick(singleClick);       // On/Off
	button.attachPress(longPress);         // Built-in or bitmap on SD
	button.attachDoubleClick(doubleClick); // Fire!

	pinMode(STRIP_PWR, OUTPUT);
	// Pull PWR_ON pin low to keep strips turned off
	digitalWrite(STRIP_PWR, LOW);
	pinMode(battPin, INPUT);

	// Uncomment this for use with FastLED2.0-2.1 (NOT dithering) library
	//LEDS.addLeds<LPD8806, DAT_PIN, CLK_PIN, RGB>(leds, NUM_LEDS);
	// Uncomment this for use with FastLED2.1-dithering library
	LEDS.addLeds<LPD8806, DAT_PIN, CLK_PIN, RGB>(leds, NUM_LEDS + 1).setDither(DISABLE_DITHER);
	resetString(1);
	//memset(leds, 0, NUM_LEDS * sizeof(struct CRGB));
	//LEDS.show();
	_delay_ms(5);

	#ifdef BATTERY_GAUGE
	battAvg = batteryCheck();
	if (battAvg < 569) {
		myError = 1;
	}
	else {
		// 5V   = 1023   -> 204.60 per volt
		// 4.2V = 859.32 -> round to 859  ! with 10K series resistor, this reading drops to 814 !
		// 3.0V = 613.80 -> round to 614  ! with 10K series resistor, this reading drops to 569 !  <-- 3.0V safe cut-off
		// 2.8V = 572.88 -> round to 573  ! with 10K series resistor, this reading drops to 528 !
		battTop = map(battAvg, 569, 814, 0, 48);
		// Set PWR_ON pin high, turn strips on to display power level
		digitalWrite(STRIP_PWR, HIGH);
		_delay_ms(5);
		fill_rainbow(&(leds[0]), battTop, 0, 2);
		LEDS.setBrightness(16);
		LEDS.show();
		_delay_ms(SECOND);
		LEDS.setBrightness(64);
		resetString(1);
		// Set PWR_ON pin low, turn strips off
		// THIS IS CURRENTLY NOT WORKING, CHECK HARDWARE
		//_delay_ms(5);
		//digitalWrite(STRIP_PWR, LOW);
	}
	#endif

	if (!sd.begin(SdChipSelect, SPI_FULL_SPEED)) {
		// Can't read SD card - is it inserted?
		SDCARD = 0;
		mode = 0; // Force built-in mode
	}
	random16_set_seed(analogRead(A5));

	// This eats up 24 bytes!
	// Disable ADC by setting ADEN bit to zero
	ADCSRA = ADCSRA & B01111111;
	// Disable analog comparator by setting ACD bit to one
	ACSR = B10000000;
	// Disable digital buffers on ADC0, ADC1, ADC4, ADC5, ADC6, ADC7
	DIDR0 = DIDR0 | B11110011;
	// (DIDR2 Register)ADC8, ADC9, ADC10, ADC11, ADC12, ADC13 are all used as digital pins
}

static void resetString(uint8_t state) {
	memset(leds, 0, NUM_LEDS * sizeof(struct CRGB));
	if (state) {
		LEDS.show();
	}
}

static void brightness() {
	brightnessLevel++;
	if (brightnessLevel > 6) brightnessLevel = 1;
	switch (brightnessLevel) {
		case 1: LEDS.setBrightness(64);  break;  // 25% - default
		case 2: LEDS.setBrightness(128); break;  // 50%
		case 3: LEDS.setBrightness(255); break;  // 100%
		case 4: LEDS.setBrightness(8);   break;  // 3%
		case 5: LEDS.setBrightness(16);  break;  // 6%
		case 6: LEDS.setBrightness(32);  break;  // 12%
	}
}

static void displayPattern(uint8_t pattern) {
	// patterns.h:MAX_PATTERNS must match the same number of patterns listed here
	switch (pattern) {
		case  1: HSVRainbowWheel();		break;
		case  2: RedWhiteBlue();		break;
		case  3: HSVEdges();			break;
		case  4: flashers();			break;
		case  5: HSVRainbowSpiral();	break;
		case  6: flowerPower();			break;
		case  7: randomSpokes();		break;
		case  8: colorTriangles();		break;
		case  9: rainbowArrows();		break;
		case 10: randomStripes();		break;
		case 11: diamonds();			break;
		case 12: wheelDivisions();		break;
	}
}

static void getFileData() {
	if (!cntlFile.isOpen()) {
		if (!cntlFile.open(CNTRL_FILE, O_READ)) {
			myError = 2;
			return;
		}
	}

	// Fetch filename from control file
	fileStatus = cntlFile.fgets(fileData, sizeof(fileData));

	if (fileStatus < 0) {
		myError = 3;
		return;
	}
	else if (fileStatus == 0) {
		// End of file reached, rewind to beginning of file
		cntlFile.seekSet(0);
		// Read filename again
		fileStatus = cntlFile.fgets(fileData, sizeof(fileData));
	}
	if (fileData[fileStatus - 1] == '\n') fileData[fileStatus - 1] = 0;
	return;
}

void loop() {

	button.tick();

	if ((run || myError > 1) && !(digitalRead(STRIP_PWR))) {
		_delay_ms(5);
		digitalWrite(STRIP_PWR, HIGH);
		} else if (((!run) && (!myError)) && digitalRead(STRIP_PWR)) {
		//_delay_ms(5);
		//digitalWrite(STRIP_PWR, LOW);
	}

	if (myError) {
		// Make sure run is turned off!
		run = false;
		// Close control and bitmap data files if open
		if (myFile.isOpen()) {
			myFile.close();
		}
		if (cntlFile.isOpen()) {
			cntlFile.close();
			memset(fileData, 0, sizeof(fileData));
		}
		// Display error blink
		LEDS.setBrightness(32);
		if (millis() - errorLastRun > pause) {
			if (!digitalRead(errorLED)) {
				digitalWrite(errorLED, HIGH);
				if (myError > 1) {
					leds[0].r = 255;
					LEDS.show();
				}
				pause = 100;
				} else {
				digitalWrite(errorLED, LOW);
				if (myError > 1) {
					resetString(1);
					//memset(leds, 0, NUM_LEDS * sizeof(struct CRGB));
					//LEDS.show();
				}
				if (myError > 1) {
					cntr++;
					if (cntr >= myError) {
						cntr = 0;
						pause = 750;
					}
				}
			}
			errorLastRun = millis();
		}
	}

	if (run) {
		if (fireState) {
			fire(fireStyle);
			} else if (!mode || !SDCARD) {
			// Use build-in routines with fixed 5 seconds timeout
			if (millis() - builtinLastRun > SECOND * 5) {
				pattern++;
				if (pattern > MAX_PATTERNS) pattern = 1;
				builtinLastRun = millis();
				resetString(0);
			}
			displayPattern(pattern);
			} else {
			// Use bitmap data on SD card
			getFileData();

			// Make sure we have a valid filename, not a comment or empty line
			while ((fileData[0] == '#') || (fileData[0] == '\n') || (fileData[0] == NULL)) {
				getFileData();
			}

			// Get tokens from control fileData
			// [0] = fileName
			// [1] = imgRefresh - PAUSE BETWEEN THE SAME IMAGE (in seconds)
			// [2] = imgTimeout - TIME BEFORE CHANGING TO NEXT IMAGE (in seconds)
			// [3] = displayPause - TIME TO PAUSE WHEN MOVING TO NEXT IMAGE (in seconds)
			char *fileName = strtok(fileData, "|\t ");
			imgRefresh = atof(strtok(NULL, "|\t "));
			imgTimeout = atof(strtok(NULL, "|\t "));
			displayPause = atof(strtok(NULL, "\r"));

			if (imgRefresh > 0) {
				myRefresh = (int)(imgRefresh * SECOND);
				} else {
				myRefresh = 0;
			}
			uint16_t myTimeout = (int)(imgTimeout * SECOND);
			if (myTimeout < 100) {
				// No sense in the image timeout being less than 100 ms
				// Reset it to at least 1 second
				myTimeout = SECOND;
			}
			uint16_t myDisplayPause = (int)(displayPause * SECOND);

			if (!myFile.open(fileName, O_READ)) {
				// Can't open or find file as listed in control file
				myError = 2;
				return;
				} else {
				// Read file header to get columns
				myFile.read(SD_buffer.receiveBuffer, 2);
				uint16_t imgColumns = SD_buffer.hdr.imageCols;
				// Display image, one column at a time
				lastChange = millis();
				while(millis() - lastChange < (imgTimeout * SECOND)) {
					if (millis() - refreshLastRun > myRefresh) {
						for (uint16_t tmpCols = 0; tmpCols < imgColumns; tmpCols++) {
							myFile.read((char*)leds, NUM_LEDS * 3);
							LEDS.show();
							button.tick();
							if (!run || !mode) {
								// STOP or MODE button press detected
								// Close file, reset string, close control file, reset fileData array
								myFile.close();
								resetString(1);
								cntlFile.close();
								memset(fileData, 0, sizeof(fileData));
								return;
							}
							_delay_us(10);  // Really not needed but added here just to slow things down a tad
						}
						// Rewind back to the beginning of the file, AFTER the uint16_t (2 bytes) header
						myFile.seekSet(2);
						refreshLastRun = millis();
						if (myRefresh > 0) {
							resetString(1);
						}
					}
				}
				// Timeout reached!  Reset string and close file
				resetString(1);
				myFile.close();

				// Check for display pause before looping to next image
				if (myDisplayPause > 0) {
					_delay_ms(myDisplayPause);
				}
			}
		}
		} else {
		pattern = 0;
		builtinLastRun = 0;
		refreshLastRun = 0;
		if (!myError) {
			if (digitalRead(errorLED)) {
				digitalWrite(errorLED, LOW);
			}
			resetString(1);
		}
	}
}

static void intHandler() {
	// Interrupt for brightness button
	static unsigned long last_interrupt_time = 0;
	unsigned long interrupt_time = millis();
	// deBounce the button!
	if (interrupt_time - last_interrupt_time > 200) {
		brightness();
		last_interrupt_time = interrupt_time;
	}
}

static void singleClick() {
	// Turn on or off
	fireState = 0;
	if (myError > 1) {
		myError = 0;
		run = 0;
		} else if (myError != 1) {
		run = !run;
		} else {
		// Battery too low, keep error status!
	}
}

static void longPress() {
	// Switch between built-in or SD card
	fireState = 0;
	mode = !mode;
	if (!run) run = !run;
}

static void doubleClick() {
	// Flames!
	// Double clicking alternates between red or blue flames
	fireStyle = -fireStyle;
	fireState = 1;
	if (!run) run = !run;
}