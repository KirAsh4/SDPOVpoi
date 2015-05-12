/*
* patterns.h
*
* Created: 6/5/2014 12:40:39 AM
*  Author: Ashley M. Kirchner
*/


#ifndef PATTERNS_H_
#define PATTERNS_H_

const    uint8_t  MAX_PATTERNS   = 12;
volatile uint8_t  pattern        = 0;
volatile uint32_t patternLastRun = 0;

static void HSVRainbowWheel() {
	static uint8_t hue = 0;
	static uint8_t off = 0;
	if ((millis() - patternLastRun > 10) && (off)) {
		LEDS.showColor(CHSV(hue, 255, 255));
		patternLastRun = millis();
		off = 0;
		hue += 10;
		} else if ((millis() - patternLastRun > 5) && (!off)) {
		LEDS.showColor(CHSV(0, 0, 0));
		patternLastRun = millis();
		off = 1;
	}
}

static void HSVRainbowSpiral() {
	static uint8_t hue = 0;
	static uint8_t cntr = 0;
	static uint8_t off = 0;
	if (millis() - patternLastRun > 10) {
		if (!off) {
			leds[0] = CHSV(hue, 255, 255);
			} else {
			leds[0] = CHSV(0, 0, 0);
		}
		LEDS.show();
		cntr++;
		if (cntr > 5) {
			cntr = 0;
			off = !off;
			hue += 15;
		}
		for (uint8_t px = NUM_LEDS - 1; px > 0; px--) {
			leds[px] = leds[px - 1];
		}
		patternLastRun = millis();
	}
}

static void HSVEdges() {
	static uint8_t hue = 0;
	if (millis() - patternLastRun > 27) {
		patternLastRun = millis();
		hue += 15;
		} else if (millis() - patternLastRun > 25) {
		fill_solid(&(leds[0]), 48, CHSV(0, 0, 0));
		} else if (millis() - patternLastRun > 5) {
		fill_solid(&(leds[0]), 48, CHSV(hue, 255, 50));
		} else {
		fill_solid(&(leds[0]), 48, CHSV(hue, 255, 255));
	}
	LEDS.show();
}

static void rainbowWheel() {
	static uint8_t hue      = 0;
	static uint8_t strBegin = 0;
	static uint8_t strEnd   = 0;
	fill_solid(&(leds[0]), strBegin, CHSV(hue, 255, 255));
	fill_solid(&(leds[0]), strEnd, CHSV(0, 0, 0));
	LEDS.show();
	if ((strBegin += 48) > 48) strBegin = 48;
	if ((strEnd += 1) > 48) {
		strBegin = 0;
		strEnd = 0;
		hue += 50;
		if (hue >= 250) hue = 0;
	}
}

static void RedWhiteBlue() {
	if (millis() - patternLastRun > 30) {
		patternLastRun = millis();
		} else if (millis() - patternLastRun > 16) {
		// Blue
		fill_solid(&(leds[0]), 48, CRGB(0, 0, 200));
		} else if (millis() - patternLastRun > 12) {
		// White
		fill_solid(&(leds[0]), 48, CRGB(200, 200, 200));
		} else if (millis() - patternLastRun > 4) {
		// Red
		fill_solid(&(leds[0]), 48, CRGB(200, 0, 0));
		} else {
		// White
		fill_solid(&(leds[0]), 48, CRGB(200, 200, 200));
	}
	LEDS.show();
}

static void flowerPower() {
	static int8_t  increment = 1;
	static uint8_t cntr      = 0;
	static uint8_t hue       = 0;
	fill_solid(&(leds[32 + cntr]), 3, CRGB::DarkBlue);
	fill_solid(&(leds[27 + cntr]), 4, CRGB::Maroon);
	fill_solid(&(leds[23 + cntr]), 4, CRGB::Black);
	fill_solid(&(leds[23 - cntr]), 2 * cntr, CHSV(hue++, 255, 255));
	fill_solid(&(leds[19 - cntr]), 4, CRGB::Black);
	fill_solid(&(leds[0]), 19 - cntr, CRGB::DarkBlue);
	LEDS.show();
	fill_solid(&(leds[0]), NUM_LEDS, CRGB::Black);
	cntr = cntr + increment;
	if ((cntr > 12) || (cntr < 1)) increment = -increment;
	//if (cntr < 1) increment = -increment;
}

static void flashers() {
	if (millis() - patternLastRun > 10) {
		leds[random8(NUM_LEDS)] = CHSV(random8(255), 255, 255);
		leds[random8(NUM_LEDS)] = CHSV(random8(255), 255, 255);
		leds[random8(NUM_LEDS)] = CHSV(random8(255), 255, 255);
		LEDS.show();
		fill_solid(&(leds[0]), NUM_LEDS, CRGB::Black);
		patternLastRun = millis();
	}
}

static void diamonds() {
	static int8_t  cntr      = 0;
	static int8_t  increment = 1;
	static uint8_t hue       = 0;
	fill_solid(&(leds[23 + cntr]), 6, CHSV(hue + 127, 255, 255));
	fill_solid(&(leds[23 - cntr]), 2 * cntr, CHSV(hue, 255, 255));
	fill_solid(&(leds[17 - cntr]), 6, CHSV(hue + 127, 255, 255));
	LEDS.show();
	fill_solid(&(leds[0]), NUM_LEDS, CRGB::Black);
	cntr += increment;
	if (cntr > 12) increment = -increment;
	if (cntr < 1) {
		hue += 15;
		increment = -increment;
	}
}

static void rainbowArrows() {
	static int8_t  cntr  = 0;
	static int8_t  incr  = 1;
	static uint8_t hue   = 0;
	static uint8_t len   = 1;
	static uint8_t lcntr = 1;
	fill_solid(&(leds[24 + cntr]), len, CHSV(hue, 255, 255));
	fill_solid(&(leds[24 - (len < 6 ? lcntr : cntr + 6)]), len, CHSV(hue, 255, 255));
	LEDS.show();
	fill_solid(&(leds[0]), NUM_LEDS, CRGB::Black);
	if (len < 6) {
		len++;
		lcntr++;
		} else {
		cntr += incr;
	}
	if (cntr > 18) {
		incr = 1;
		cntr = 0;
		len = 1;
		lcntr = 1;
		hue += 25;
	}
	_delay_us(200);
}

static void randomSpokes() {
	static uint8_t hue     = 120;
	static int8_t  incr    = 10;
	static uint8_t prevhue = 0;
	static int16_t bright  = 0;

	for (uint8_t x = 0; x < 52; x++) {
		fill_solid (&(leds[0]), 48, CHSV(hue, 255, bright));
		LEDS.show();
		bright = bright + incr;
		if ((bright > 255) || (bright < 0)){
			bright = (bright < 0 ? 0 : 255);
			incr = -incr;
		}
	}
	bright = 0;
	incr = 25;
	for (uint8_t x = 0; x < 22; x++) {
		fill_solid (&(leds[0]), 48, CHSV(hue + 180, 255, bright));
		LEDS.show();
		bright = bright + incr;
		if ((bright > 255) || (bright < 0)){
			bright = (bright < 0 ? 0 : 255);
			incr = -incr;
		}
	}
	bright = 0;
	incr = 10;
	if (random16(10000) > 8000) {
		hue = random8(255);
		while (abs(hue - prevhue) < 20) hue = random8(255);
		prevhue = hue;
	}
}

static void randomStripes() {
	static uint8_t  done        = 0;
	static uint8_t  values[6]   = {};
	static uint8_t  hues[6]     = {};
	static uint8_t  delays[6]   = {};
	static uint32_t lastruns[6] = {};

	if (!done) {
		// Fill arrays
		for (uint8_t x = 0; x < 6; x++) {
			hues[x] = random8(255);
			if (x) {
				while (hues[x] == hues[x - 1]) hues[x] = random8(255);
			}
			delays[x] = 25 * (25 % (random8(1, 51)));
			lastruns[x] = millis() + delays[x];
			values[x] = random8(255);
		}
		done = 1;
	}
	for (uint8_t x = 0; x < 6; x++) {
		if (millis() - lastruns[x] > delays[x]) {
			hues[x] = random8(255);
			values[x] = random8(255);
			if (x) {
				while (hues[x] == hues[x - 1]) hues[x] = random8(255);
			}
			delays[x] = 25 * (25 % (random8(1, 51)));
			lastruns[x] = millis();
		}
	}
	for (uint8_t y = 0; y < 6; y++) {
		fill_solid(&(leds[y * 8]), 7, CHSV(hues[y], 255, values[y]));
	}
	LEDS.show();
}


#define ARRAY_LENGTH(A) (sizeof(A)/sizeof(*A))

CRGB wheelColors[] = {0xFF003B, 0x1600E8, 0x00FFCB, 0xA6E800, 0xFF9700, 0x000000};

CRGB getWheelColor() {
	uint8_t colorNumber = random8(ARRAY_LENGTH(wheelColors));
	return wheelColors[colorNumber];
}

static void wheelDivisions() {
	static uint8_t  split        = 0;
	static uint8_t  prevSplit    = 0;
	static uint8_t  off          = 1;
	static uint8_t  patternPause = 0;

	if ((millis() - patternLastRun > patternPause) && (off)) {
		// Pause black for a moment
		off = 0;
		patternPause = random8(1, 3) * 5;
		resetString(0);
		//memset(leds, 0, NUM_LEDS * sizeof(struct CRGB));
		patternLastRun = millis();
		} else if ((millis() - patternLastRun > patternPause) && (!off)) {
		off = 1;
		split = random8(4);
		while (split == prevSplit) split = random8(4);
		prevSplit = split;
		switch(split) {
			case 0: // Single color
			fill_solid(&(leds[0]), 48, getWheelColor());
			break;
			case 1: // Two colors, 1/3 - 2/3
			fill_solid(&(leds[0]), 15, getWheelColor());
			fill_solid(&(leds[17]), NUM_LEDS - 17, getWheelColor());
			break;
			case 2: // Two colors, 2/3 - 1/3
			fill_solid(&(leds[0]), 31, getWheelColor());
			fill_solid(&(leds[33]), NUM_LEDS - 33, getWheelColor());
			break;
			case 3: // Four colors, 1/4's
			fill_solid(&(leds[0]), 10, getWheelColor());
			fill_solid(&(leds[12]), 10, getWheelColor());
			fill_solid(&(leds[24]), 10, getWheelColor());
			fill_solid(&(leds[36]), 12, getWheelColor());
			break;
		}
		patternPause = random8(1, 3) * 50;
		patternLastRun = millis();
	}
	LEDS.show();
}

// CRGB HeatColor( uint8_t temperature)
// [to be included in the forthcoming FastLED v2.1]
//
// Approximates a 'black body radiation' spectrum for
// a given 'heat' level.  This is useful for animations of 'fire'.
// Heat is specified as an arbitrary scale from 0 (cool) to 255 (hot).
// This is NOT a chromatically correct 'black body radiation'
// spectrum, but it's surprisingly close, and it's extremely fast and small.
//
// On AVR/Arduino, this typically takes around 70 bytes of program memory,
// versus 768 bytes for a full 256-entry RGB lookup table.

CRGB HeatColor( uint8_t temperature, int8_t style) {
	CRGB heatcolor;

	// Scale 'heat' down from 0-255 to 0-191,
	// which can then be easily divided into three
	// equal 'thirds' of 64 units each.
	uint8_t t192 = scale8_video( temperature, 192);

	// calculate a value that ramps up from
	// zero to 255 in each 'third' of the scale.
	uint8_t heatramp = t192 & 0x3F; // 0..63
	heatramp <<= 2; // scale up to 0..252

	// now figure out which third of the spectrum we're in:
	if (t192 & 0x80) {
		// we're in the hottest third
		switch(style) {
			case -1: heatcolor = CRGB(255, 255, heatramp); break;
			case 1: heatcolor = CRGB(heatramp, 255, 255); break;
		}
		} else if( t192 & 0x40 ) {
		// we're in the middle third
		switch(style) {
			case -1: heatcolor = CRGB(255, heatramp, 0); break;
			case 1: heatcolor = CRGB(0, heatramp, 255); break;
		}
		} else {
		// we're in the coolest third
		switch(style) {
			case -1: heatcolor = CRGB(heatramp, 0, 0); break;
			case 1: heatcolor = CRGB(0, 0, heatramp); break;
		}
	}
	return heatcolor;
}

// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY
//
// This basic one-dimensional 'fire' simulation works roughly as follows:
// There's a underlying array of 'heat' cells, that model the temperature
// at each point along the line.  Every cycle through the simulation,
// four steps are performed:
//  1) All cells cool down a little bit, losing heat to the air
//  2) The heat from each cell drifts 'up' and diffuses a little
//  3) Sometimes randomly new 'sparks' of heat are added at the bottom
//  4) The heat from each cell is rendered as a color into the leds array
//     The heat-to-color mapping uses a black-body radiation approximation.
//
// Temperature is in arbitrary units from 0 (cold black) to 255 (white hot).
//
// This simulation scales it self a bit depending on NUM_LEDS; it should look
// "OK" on anywhere from 20 to 100 LEDs without too much tweaking.
//
// I recommend running this simulation at anywhere from 30-100 frames per second,
// meaning an interframe delay of about 10-35 milliseconds.
//
//
// There are two main parameters you can play with to control the look and
// feel of your fire: COOLING (used in step 1 above), and SPARKING (used
// in step 3 above).
//
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 55, suggested range 20-100
#define COOLING  55

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120


static void Fire2012(int8_t style) {
	// Array of temperature readings at each simulation cell
	static byte heat[NUM_LEDS];

	// Step 1.  Cool down every cell a little
	for( uint8_t i = 0; i < NUM_LEDS; i++) {
		heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
	}

	// Step 2.  Heat from each cell drifts 'up' and diffuses a little
	for( uint8_t k= NUM_LEDS - 3; k > 0; k--) {
		heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
	}

	// Step 3.  Randomly ignite new 'sparks' of heat near the bottom
	if( random8() < SPARKING ) {
		uint8_t y = random8(7);
		heat[y] = qadd8( heat[y], random8(160,255) );
	}

	// Step 4.  Map from heat cells to LED colors
	for( uint8_t j = 0; j < NUM_LEDS; j++) {
		leds[j] = HeatColor(heat[j], style);
	}
}

static void fire(int8_t style) {
	random16_add_entropy(random());
	Fire2012(style); // run simulation frame
	LEDS.show(); // display this frame

	#if defined(FASTLED_VERSION) && (FASTLED_VERSION >= 2001000)
	LEDS.delay(1000 / FRAMES_PER_SECOND);
	#else
	delay(1000 / FRAMES_PER_SECOND);
	#endif
}

#endif /* PATTERNS_H_ */
