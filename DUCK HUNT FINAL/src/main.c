#include <stm32f031x6.h>
#include "display.h"
#include "Duck.h"
#include <stm32f0xx.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "sound.h"
#include "musical_notes.h"

// FUNCTION DECLARATIONS 
void initClock(void);              
// Chip defaults to 8MHz which is too slow for smooth graphics on 128x160 LCD
// Sets PLL to 48MHz (6x faster) by multiplying 4MHz internal oscillator by 12

void initSysTick(void);            
//  Need accurate millisecond timing for delays, frenzy duration, and animations
// Configures hardware timer to interrupt CPU every 1ms and increment counter

void SysTick_Handler(void);        
//  Hardware calls this automatically every millisecond via interrupt
// Just increments the milliseconds counter 

void delay(volatile uint32_t dly); 
//  Need to pause for button debouncing (prevent double-presses) and "HIT!" messages
// Uses WFI instruction to put CPU in low-power sleep while waiting

void setupIO();                    
// GPIO pins start undefined - must configure as inputs with pull-ups for buttons to work
// Also initializes LCD communication (SPI protocol) and enables port clocks

void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber); 
//  Buttons connect pin to ground when pressed  need pull-up to make it HIGH when released
// Without pull-up, unpressed buttons would read random values (floating input)

void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode); 
//  Each GPIO can be input (read buttons), output (control LED), analog, or special function
// Must set correct mode before using pin , writes to MODER register

void showMenu(void);               
//  Need menu interface so user can start game, view scores, or change crosshair
// Blocks in loop until user selects option, then returns to main

void showHighScores(void);         
// Displays top 10 scores from array until button pressed

void showTargetSelect(void);       
// Shows which colors available and lets user choose

void addHighScore(uint16_t newScore); 
// Checks if new score beats 10th place, if yes replaces it and re-sorts list

void sortHighScores(void);         
// Bubble sort - inefficient but only 10 elements so doesn't matter

//  GLOBAL VARIABLES 

volatile uint32_t milliseconds;    
//  volatile: Modified by interrupt handler, compiler mustn't cache this value
// Counts milliseconds since boot - used for ALL timing in game
// uint32_t can count to 4.2 billion ms = 49 days before overflow

uint16_t highScores[10] = {0,0,0,0,0,0,0,0,0,0}; 
//  Track top 10 scores for leaderboard
// Stored in RAM not flash because: (1) flash writes are slow and complex, (2) flash wears out after ~10k writes
// downside: Scores lost on power-off, but code is simpler and flash lasts longer

uint16_t maxScoreEver = 0;         
//  Need separate tracker for unlocking crosshairs , never decreases even if falls out of top 10
// Score 600 (unlocks purple), then only score 50s afterward - purple stays unlocked

uint8_t selectedTarget = 0;        
//  Stores which crosshair user chose (0=red, 1=blue, 2=purple, 3=pink)
// uint8_t = 1 byte, only need 0-3 so wastes less RAM than uint16_t

// SPRITE DATA (UNUSED DECORATIONS REMOVED)

const uint16_t TargetRed[]={
0,63488,63488,63488,63488,63488,63488,63488,63488,0,
63488,63488,65535,65535,65535,65535,65535,65535,63488,63488,
63488,65535,65535,63488,63488,63488,63488,65535,65535,63488,
63488,65535,63488,65535,65535,65535,65535,63488,65535,63488,
63488,65535,63488,65535,63488,63488,65535,63488,65535,63488,
63488,65535,63488,65535,63488,63488,65535,63488,65535,63488,
63488,65535,63488,65535,65535,65535,65535,63488,65535,63488,
63488,65535,65535,63488,63488,63488,63488,65535,65535,63488,
63488,63488,65535,65535,65535,65535,65535,65535,63488,63488,
0,63488,63488,63488,63488,63488,63488,63488,63488,0,
}; 
//  Default crosshair, always available
// 10x10 pixels = 100 uint16_t values

const uint16_t TargetBlue[]={
0,2047,2047,2047,2047,2047,2047,2047,2047,0,
2047,2047,65535,65535,65535,65535,65535,65535,2047,2047,
2047,65535,65535,2047,2047,2047,2047,65535,65535,2047,
2047,65535,2047,65535,65535,65535,65535,2047,65535,2047,
2047,65535,2047,65535,2047,2047,65535,2047,65535,2047,
2047,65535,2047,65535,2047,2047,65535,2047,65535,2047,
2047,65535,2047,65535,65535,65535,65535,2047,65535,2047,
2047,65535,65535,2047,2047,2047,2047,65535,65535,2047,
2047,2047,65535,65535,65535,65535,65535,65535,2047,2047,
0,2047,2047,2047,2047,2047,2047,2047,2047,0,
}; 
//  Unlocks at 50 points as first reward
// 2047 = 0000011111111111 = max blue+green, no red = cyan/sky blue

const uint16_t TargetGreen[]={
0,2016,2016,2016,2016,2016,2016,2016,2016,0,
2016,2016,65535,65535,65535,65535,65535,65535,2016,2016,
2016,65535,65535,2016,2016,2016,2016,65535,65535,2016,
2016,65535,2016,65535,65535,65535,65535,2016,65535,2016,
2016,65535,2016,65535,2016,2016,65535,2016,65535,2016,
2016,65535,2016,65535,2016,2016,65535,2016,65535,2016,
2016,65535,2016,65535,65535,65535,65535,2016,65535,2016,
2016,65535,65535,2016,2016,2016,2016,65535,65535,2016,
2016,2016,65535,65535,65535,65535,65535,65535,2016,2016,
0,2016,2016,2016,2016,2016,2016,2016,2016,0,
}; 
// Unlocks at 500 points for dedicated players
// NOTE: Named "Green" but displays purple due to LCD color calibration issue
// 2016 = 0000011111100000 = max green + some blue = purple on this display

const uint16_t TargetYellow[]={
0,65504,65504,65504,65504,65504,65504,65504,65504,0,
65504,65504,65535,65535,65535,65535,65535,65535,65504,65504,
65504,65535,65535,65504,65504,65504,65504,65535,65535,65504,
65504,65535,65504,65535,65535,65535,65535,65504,65535,65504,
65504,65535,65504,65535,65504,65504,65535,65504,65535,65504,
65504,65535,65504,65535,65504,65504,65535,65504,65535,65504,
65504,65535,65504,65535,65535,65535,65535,65504,65535,65504,
65504,65535,65535,65504,65504,65504,65504,65535,65535,65504,
65504,65504,65535,65535,65535,65535,65535,65535,65504,65504,
0,65504,65504,65504,65504,65504,65504,65504,65504,0,
}; 
// Unlocks at 1000 points 
// NOTE: Named "Yellow" but displays pink due to LCD color issue  
// 65504 = 1111111111100000 = max red + max green = pink on this display

// Import duck data from Duck.c (defined there, used here)
extern int i; 
//  extern: Variable physically stored in Duck.c, but we need access here
// Duck's Y position, modified by DuckMove() each frame

extern int j; 
// extern: Duck's X position

extern const uint16_t Duck1Up[]; 
//  extern const: 19x13 pixel duck sprite array stored in Duck.c (247 bytes)
// Used for both main duck and all 3 frenzy ducks to save memory

extern int wallhit;
//  extern: variable to keep track how many times a duck has hit the left side of the screen

//  GAME STATE 

uint16_t score = 0;           
//  Current game score, resets to 0 each new game
// uint16_t max 65535 - more than enough (realistically u can't score that high)

int duckHit = 0;              
// Boolean flag (0=false, 1=true) to pause duck after shooting it
// Without pause, duck teleports to new position instantly (looks glitchy)
// Pause gives visual feedback that you actually hit it

// FRENZY MODE SYSTEM 
// frenzy: Makes game exciting at high scores, rewards skilled players
// Triggers every 200 points, spawns 3 bonus ducks for 30 seconds

int frenzyMode = 0;           
//  Boolean - is frenzy currently active? (0=no, 1=yes)

uint32_t frenzyEndTime = 0;   
//  Absolute timestamp (in milliseconds) when frenzy should end
// Set to: currentTime + 30000 when frenzy starts
// Check: if (milliseconds >= frenzyEndTime) then end frenzy

uint16_t lastFrenzyScore = 0; 
//  Prevents re-triggering frenzy on same milestone
// Without this: Hitting 200 points would trigger frenzy EVERY FRAME until score changes
// With this: Check (score >= lastFrenzyScore + 200) only triggers once per 200-point milestone

// Three bonus ducks during frenzy (simple independent movement kinda just bouncing around)
int frenzyDuckActive[3] = {0, 0, 0}; 
//  array of 3: Can have up to 3 bonus ducks alive simultaneously
// Each element: 0 = dead/not spawned, 1 = alive and should be drawn

int frenzyDuckX[3];           
//  X positions of 3 frenzy ducks (updated each frame)

int frenzyDuckY[3];           
//  Y positions of 3 frenzy ducks

int frenzyDuckDirX[3];        
//  Horizontal velocities (-1 = moving left, +1 = moving right)
// Multiplied by 2 each frame for actual movement speed

int frenzyDuckDirY[3];        
//  Vertical velocities (-1 = moving up, +1 = moving down)
// Flips when duck hits screen edge (simple bounce physics)

int main()
{
	//  INITIALIZE HARDWARE 
	
	srand(time(0));   
	//  Seeds random number generator with current time (seconds since 1970)
	// Without seed: rand() returns SAME sequence every boot = predictable duck spawns
	// With seed: Each boot has different random sequence = unpredictable game
	
	initClock();      
	//  Chip boots at 8MHz (default) which is too slow for smooth 60Hz graphics
	// Needs 48MHz to: (1) send pixels to LCD fast enough, (2) run game logic without lag
	// How: Configures PLL to multiply 4MHz internal clock by 12 = 48MHz
	
	initSound();

	initSysTick();    
	//  Need accurate millisecond timer for: (1) delays without blocking, (2) frenzy duration, (3) smooth animation timing
	// Sets up hardware to interrupt CPU every 1ms automatically
	
	setupIO();        
	//  GPIO pins start in random/undefined states after boot
	// Must configure: (1) button pins as inputs with pull-ups, (2) LCD SPI pins, (3) enable port clocks
	
	// OUTER GAME LOOP 
	//  infinite loop: Embedded systems never "exit" - must run forever
	// Cycle: Menu - Game - Menu - Game - and repeat
	while(1) {
		showMenu(); 
		//  Blocks here until user presses button to start game
		// When returns, user selected "START GAME" option
		
		fillRectangle(0,0,128,160,0); 
		//  Clear entire screen to black before game starts
		// Removes menu graphics so duck doesn't appear on top of text which it was doing before
		// 128x160 = screen dimensions, 0 = black in RGB565
		
		// RESET GAME STATE FOR NEW GAME 
		//  Without reset, new game would inherit old score/frenzy from last game
		score = 0;              
		lastFrenzyScore = 0;    
		frenzyMode = 0;         
		for (int f = 0; f < 3; f++) {
			frenzyDuckActive[f] = 0; 
			// Mark all frenzy ducks as dead so they don't appear at start
		}
	
	//  PLAYER/CROSSHAIR LOCAL VARIABLES 
	// why local not global?: Each game starts fresh at center of screen
	
	int hmoved = 0;     
	//  Flag to track if crosshair moved horizontally THIS frame
	// Used to decide: should we erase old position and redraw?
	// Reset to 0 at start of each frame

	int vmoved = 0;     
	//  Same for vertical movement as above^

	uint16_t x = 50;    
	//  Crosshair X position, starts near center (screen is 128 wide)
	// uint16_t needed for screen coordinates (0-127)

	uint16_t y = 50;    
	//  Crosshair Y position (screen is 160 tall, 0-159)

	uint16_t oldx = x;  
	//  Remember previous X to erase old crosshair sprite before drawing new one
	// Without this: Leaves trail of crosshairs across screen (sprite doesn't erase itself)

	uint16_t oldy = y;  
	//  Previous Y position for erasing

	// SELECT CROSSHAIR SPRITE 
	const uint16_t *currentTarget;
	// why pointer?: Avoids copying entire 100-element array
	// Just stores address of selected sprite array (2 bytes vs 200 bytes)
	
	switch(selectedTarget) {
		case 1: currentTarget = TargetBlue; break;
		case 2: currentTarget = TargetGreen; break;   // Displays purple
		case 3: currentTarget = TargetYellow; break;  // Displays pink
		default: currentTarget = TargetRed; break;    // Safety fallback just give them red gtarget
	}
	//  switch: Clean mapping from 0-3 value to correct array pointer
	
	int gameRunning = 1; 
	//  Flag to exit inner loop and return to menu
	// Set to 0 when UP+DOWN buttons pressed together aka let you return to menu
	
	//  INNER GAME LOOP 
	//  Runs at ~20 FPS (50ms delay at end), handles all game logic each frame
	while(gameRunning)
	{

		//To turn off leds depending on the amount of times the left side has been hit
		switch(wallhit){
			case 1: //Turn off 1 led
				GPIOA->ODR &= ~(1 << 0);
				break;
			case 2: //Turn off 2 leds
				GPIOA->ODR &= ~(1 << 0);
				GPIOA->ODR &= ~(1 << 1);
				break;
			case 3: //Turn off 3 leds
				GPIOA->ODR &= ~(1 << 0);
				GPIOA->ODR &= ~(1 << 1);
				GPIOA->ODR &= ~(1 << 9);
				break;
		}
		// GAME OVER
		if(wallhit == 3){ //Checks if the ducks had hit the left side 3 times

			fillRectangle(0,0,128, 160, RGBToWord(0, 50, 100)); // create a blue background
			printTextX2("GAME OVER", 15, 20, RGBToWord(255, 255, 0), RGBToWord(0, 50, 100)); // Text to tell the user is game over
			addHighScore(score); // Save your score before leaving
			gameRunning = 0; // Change state to leave inner game loop
			delay(2500); // Delay the game over screen for a appropriate amount of time
			break;
		}

		//  CHECK IF FRENZY SHOULD END 
		if (frenzyMode && milliseconds >= frenzyEndTime) {
			//  Timer might skip exact value during processing
			// Example: frenzyEndTime=1000, but check happens at ms=1002
			// '>=' catches this, '==' would miss it and frenzy never ends
			
			frenzyMode = 0; 
			
			// Clear all remaining frenzy ducks from screen
			for (int f = 0; f < 3; f++) {
				if (frenzyDuckActive[f]) {
					fillRectangle(frenzyDuckX[f]-2, frenzyDuckY[f]-2, 23, 17, 0); 
					// -2 and 23x17: Duck is 19x13, +2 padding ensures complete erase
					// black (0): Erases duck by drawing black rectangle over it
					frenzyDuckActive[f] = 0;
				}
			}
		}
		
		//  REDRAW SCORE EVERY FRAME 
		// every frame: Duck movement could overwrite score text at top of screen and it was happening as duck moved
		fillRectangle(5, 5, 70, 10, 0); 
		//  Erase old score area (black rectangle at position 5,5 size 70x10)
		
		printText("SCORE:", 5, 5, RGBToWord(255, 255, 255), 0); 
		//  white (255,255,255): Clearly visible against black background
		// RGBToWord converts RGB (0-255 each) to RGB565 format (16-bit)
		
		printNumber(score, 50, 5, RGBToWord(255, 255, 0), 0);   
		//  yellow (255,255,0): Stands out from white text, draws eye to score
		// Position 50,5 places it after "SCORE:" text
		
		//  SHOW FRENZY COUNTDOWN TIMER
		if (frenzyMode) {
			// show timer: Players need to know how much bonus time remains
			uint32_t timeLeft = (frenzyEndTime - milliseconds) / 1000; 
			//  /1000: Convert milliseconds to seconds for readability
			// Example: 5234ms left -displays as "5"
			
			fillRectangle(5, 145, 60, 10, 0); 
			printText("FRENZY:", 5, 145, RGBToWord(255, 0, 255), 0); 
			//  purple (255,0,255): Thematic color for frenzy mode
			printNumber(timeLeft, 50, 145, RGBToWord(255, 0, 255), 0);
		} else {
			fillRectangle(5, 145, 70, 10, 0); 
			//  Remove "FRENZY: 0" text after timeout (looks cleaner)
		}
		
		//  FORCE DUCK ON SCREEn
		// WHY: Bug discovered during testing - duck flies off edges and disappears
		// Root cause: DuckMove() math can overshoot boundaries
		// Fix: Clamp position every frame as safety net
		if (i < 15) i = 15;     // Top (leave room for score text)
		if (i > 130) i = 130;   // Bottom (160 - 13 duck height - margin)
		if (j < 0) j = 0;       // Left edge
		if (j > 110) j = 110;   // Right (128 - 19 duck width = 109, rounded to 110)
		
		//  MOVE MAIN DUCK 
		if (!duckHit) {
			// check duckHit: Pause duck briefly after shooting for visual feedback
			// Without pause: Duck instantly teleports to new position (looks broken)
			// With pause: Shows "HIT!" message, then duck respawns
			
			DuckMove(110); 
			//  parameter 110: Tells DuckMove() the right screen boundary
			// Function in Duck.c handles diagonal bouncing with random Y targets
		}
		
		// MOVE FRENZY DUCKS 
		if (frenzyMode) {
			// only when active: Don't waste CPU checking inactive ducks
			
			for (int f = 0; f < 3; f++) {
				if (frenzyDuckActive[f]) {
					// check active: Duck might have been shot already
					
					fillRectangle(frenzyDuckX[f]-2, frenzyDuckY[f]-2, 23, 17, 0); 
					// erase first: Remove duck from old position before moving
					//  -2: Extra padding ensures clean erase even with rounding errors
					
					// Move duck 2 pixels in current direction
					frenzyDuckX[f] += frenzyDuckDirX[f] * 2; 
					frenzyDuckY[f] += frenzyDuckDirY[f] * 2;
					//  *2: Speed multiplier (1 pixel/frame too slow, 2 is good)
					
					// Bounce off screen edges
					if (frenzyDuckX[f] < 5 || frenzyDuckX[f] > 105) {
						frenzyDuckDirX[f] = -frenzyDuckDirX[f]; 
						//  negate: Reverses direction (1 becomes -1, -1 becomes 1) so they can bounce around
					}
					if (frenzyDuckY[f] < 20 || frenzyDuckY[f] > 120) {
						frenzyDuckDirY[f] = -frenzyDuckDirY[f];
					}
					
					// Force bounds (safety net if bounce math fails)
					if (frenzyDuckX[f] < 5) frenzyDuckX[f] = 5;
					if (frenzyDuckX[f] > 105) frenzyDuckX[f] = 105;
					if (frenzyDuckY[f] < 20) frenzyDuckY[f] = 20;
					if (frenzyDuckY[f] > 120) frenzyDuckY[f] = 120;
					
					putImage(frenzyDuckX[f], frenzyDuckY[f], 19, 13, Duck1Up, 0, 0); 
					//  Draw duck at new position
					// 19x13 = duck dimensions, Duck1Up = sprite data, 0,0 = no flipping
				}
			}
		}
		
		// Reset movement flags for this frame
		hmoved = vmoved = 0;
		
		//  READ BUTTON INPUTS 
		//  each frame: Need responsive controls (check buttons 20 times per second)
		// IMPORTANT: Buttons are ACTIVE LOW (pressed = 0, released = 1)
		//  active low: Button connects pin to ground, pull-up resistor makes it HIGH when open
		
		if ((GPIOB->IDR & (1 << 4))==0) 
		//  bit mask: IDR register has 16 bits (one per pin), check only bit 4
		// (1 << 4) = 0b00010000 = check pin PB4
		// & operation isolates this bit, ==0 checks if LOW (pressed)
		{					
			if (x < 110) 
			// 110: Screen is 128 wide, crosshair is 10 wide, so max = 118
			// Use 110 to leave margin (prevents hitting exact edge)
			{
				x = x + 2; 
				// +2: Move 2 pixels per frame = 40 pixels/sec at 20 FPS (feels responsive)
				// +1 would be too slow, +3 too fast
				hmoved = 1; // Mark that we moved (for redraw logic)
			}						
		}
		
		if ((GPIOB->IDR & (1 << 5))==0 && !hmoved) 
		//  check !hmoved: Left button is ALSO the shoot button so allows use of both 
		// Don't move if already moved right this frame (prevents diagonal jitter)
		{			
			if (x > 10) 
			//  >10: Leave margin from left edge
			{
				x = x - 2; 
				hmoved = 1;
			}			
		}
		
		if ( (GPIOA->IDR & (1 << 11)) == 0) 
		//  GPIOA not GPIOB: Up/Down buttons on different port than Left/Right
		// Bit 11 of port A = PA11 = down button
		{
			if (y < 140) 
			//  140: Screen is 160 tall, crosshair is 10 tall, max = 150
			// Use 140 to leave margin at bottom
			{
				y = y + 2; 
				vmoved = 1;
			}
		}
		
		if ( (GPIOA->IDR & (1 << 8)) == 0) 
		//  bit 8: PA8 = up button
		{			
			if (y > 16) 
			// >16: Leave room at top for score text (don't overlap)
			{
				y = y - 2; 
				vmoved = 1;
			}
		}
		
		//  REDRAW CROSSHAIR IF MOVED 
		// only if moved: Reduces screen flicker by not redrawing static sprites
		if ((vmoved) || (hmoved))
		{
			fillRectangle(oldx,oldy,10,10,0); 
			//  Erase crosshair at old position (black 10x10 rectangle)
			oldx = x; 
			oldy = y;					
			
			putImage(x,y,10,10,currentTarget,0,0);
			//  Draw crosshair at new position
			// 10x10 = crosshair size, currentTarget = selected sprite, 0,0 = no flip
		}
		
		// Redraw crosshair on top EVERY frame
		putImage(x,y,10,10,currentTarget,0,0);
		//  always: Fixes bug where duck would overlap and hide crosshair
		// Drawing crosshair last ensures it's always on top layer
		
		// ========== SHOOTING LOGIC ==========
		static int lastButtonState = 1; 
		//  static: Value persists between function calls (remembers last state)
		// WHY 1: Button starts unpressed (HIGH due to pull-up resistor)
		// Used for debouncing - only trigger on press edge (1-> 0 transition)
		
		int currentButtonState = (GPIOB->IDR & (1 << 3)) == 0; 
		//  Read current state of left/shoot button (PB5)
		// Result: 1 if pressed, 0 if not pressed
		
		// Trigger shot on button press edge (was HIGH, now LOW)
		if (currentButtonState && lastButtonState && !duckHit)
		// three conditions:
		// 1. currentButtonState: Button IS pressed now
		// 2. lastButtonState: Button WAS pressed last frame (prevents retriggering)
		// 3. !duckHit: Main duck not already hit (prevents double-hit during pause)
		{
			int hitDetected = 0;    // Did we hit main duck?
			int frenzyHit = -1;     // Which frenzy duck hit? (-1 = none)
			
			//  CHECK MAIN DUCK COLLISION
			//  check all 4 corners: Simple overlap isn't enough
			// Problem: 10x10 crosshair vs 19x13 duck -> need accurate detection
			// Solution: Check if ANY corner of crosshair is inside duck rectangle
			if ((x >= j && x <= j+19 && y >= i && y <= i+13) ||           // Top-left corner
			    (x+10 >= j && x+10 <= j+19 && y >= i && y <= i+13) ||      // Top-right corner  
			    (x >= j && x <= j+19 && y+10 >= i && y+10 <= i+13) ||      // Bottom-left corner
			    (x+10 >= j && x+10 <= j+19 && y+10 >= i && y+10 <= i+13))  // Bottom-right corner
			{
				hitDetected = 1;
			}
			
			//  CHECK FRENZY DUCK COLLISIONS
			//  only if missed main: Can only hit one duck per shot (prevents double-scoring)
			if (!hitDetected && frenzyMode) {
				for (int f = 0; f < 3; f++) {
					if (frenzyDuckActive[f]) {
						// Same 4-corner check for frenzy duck
						if ((x >= frenzyDuckX[f] && x <= frenzyDuckX[f]+19 && y >= frenzyDuckY[f] && y <= frenzyDuckY[f]+13) ||
						    (x+10 >= frenzyDuckX[f] && x+10 <= frenzyDuckX[f]+19 && y >= frenzyDuckY[f] && y <= frenzyDuckY[f]+13) ||
						    (x >= frenzyDuckX[f] && x <= frenzyDuckX[f]+19 && y+10 >= frenzyDuckY[f] && y+10 <= frenzyDuckY[f]+13) ||
						    (x+10 >= frenzyDuckX[f] && x+10 <= frenzyDuckX[f]+19 && y+10 >= frenzyDuckY[f] && y+10 <= frenzyDuckY[f]+13))
						{
							frenzyHit = f; 
							break; // Stop checking (only hit one duck)
						}
					}
				}
			}
			
			//  HANDLE MAIN DUCK HIT 
			if (hitDetected)
			{
				duckHit = 1;     
				//  Pause duck movement for 0.5 seconds while showing "HIT!" message
				score += 10;     
				//  10 points: Main duck base value (frenzy ducks worth 20)
				
				//  CHECK FOR FRENZY TRIGGER
				//  only if NOT in frenzy: Prevents triggering new frenzy during frenzy
				// Problem: Hitting 400 points during frenzy would trigger another frenzy immediately
				if (!frenzyMode) {
					if (score >= lastFrenzyScore + 200) {
						// WHY >=: Ensures trigger even if skip exactly 200 (e.g. 0 -> 210 in one shot)
						//  +200: Trigger every 200-point milestone (200, 400, 600, ...)
						
						frenzyMode = 1;
						frenzyEndTime = milliseconds + 30000; 
						//  +30000: Frenzy lasts 30 seconds (30000 milliseconds)
						lastFrenzyScore = score; 
						// update: Remember this score so we don't retrigger
						
						//  SPAWN 3 FRENZY DUCKS 
						for (int f = 0; f < 3; f++) {
							frenzyDuckActive[f] = 1; // Mark as alive
							frenzyDuckX[f] = 20 + (rand() % 80); 
							//  20+(0-79): Random X between 20-99 (keeps ducks on screen)
							frenzyDuckY[f] = 30 + (rand() % 60); 
							// 30+(0-59): Random Y between 30-89
							frenzyDuckDirX[f] = (rand() % 2) ? 1 : -1; 
							//  Random direction, 50% chance left or right
							// rand()%2 gives 0 or 1, ternary converts to -1 or 1
							frenzyDuckDirY[f] = (rand() % 2) ? 1 : -1;
						}
						
						// Show "FRENZY!" message
						fillRectangle(20, 70, 90, 20, 0);
						printTextX2("FRENZY!", 25, 70, RGBToWord(255, 0, 255), 0); 
						//  Double-size text so it's very visible
						// purple: Thematic frenzy color
						delay(1000); 
						//  1 second: Long enough to read, not too long (interrupts gameplay)
						fillRectangle(20, 70, 90, 20, 0); // Clear message
					}
				}
				
				// Update all-time high score
				if (score > maxScoreEver) {
					maxScoreEver = score;
					//  separate from highScores: Unlocks never revert even if score isn't top 10
				}
				
				fillRectangle(j-2, i-2, 23, 17, 0); 
				//  Erase main duck sprite (black rectangle)
				
				// Update score display
				fillRectangle(5, 5, 70, 10, 0);
				printText("SCORE:", 5, 5, RGBToWord(255, 255, 255), 0);
				printNumber(score, 50, 5, RGBToWord(255, 255, 0), 0);
				
				// Show "HIT!" message
				printTextX2("HIT!", 40, 70, RGBToWord(0, 255, 0), 0); 
				GPIOB->ODR |= (1 << 1);
				playNote(C1);
				delay(500);
				GPIOB->ODR &= ~(1 << 1);
				// green: Positive feedback color (success)
				delay(500); 
				// 0.5 seconds: Enough to see, but doesn't slow gameplay too much
				fillRectangle(40, 70, 70, 20, 0); // Clear message
				
				// Respawn duck at random position
				i = 30 + (rand() % 80); 
				//  random: Makes game unpredictable and challenging
				j = 20 + (rand() % 80);
				
				lastButtonState = 0; // Mark button as processed
				duckHit = 0; // Let duck move again
			}
			//  HANDLE FRENZY DUCK HIT 
			else if (frenzyHit >= 0)
			//  else if: Can't hit both main and frenzy duck in one shot
			{
				score += 20; 
				//  20: Double points for frenzy ducks 
				
				if (score > maxScoreEver) {
					maxScoreEver = score;
				}
				
				fillRectangle(frenzyDuckX[frenzyHit]-2, frenzyDuckY[frenzyHit]-2, 23, 17, 0);
				//  Erase frenzy duck
				frenzyDuckActive[frenzyHit] = 0; 
				// Mark as dead (won't be drawn or checked anymore)
				
				// Update score display
				fillRectangle(5, 5, 70, 10, 0);
				printText("SCORE:", 5, 5, RGBToWord(255, 255, 255), 0);
				printNumber(score, 50, 5, RGBToWord(255, 255, 0), 0);
				
				// Show "+20!" to indicate bonus
				printTextX2("+20!", 40, 70, RGBToWord(255, 0, 255), 0); 
				//  purple: Matches frenzy theme
				delay(300); 
				//  0.3 seconds: Shorter than main duck (keeps frenzy fast-paced and fits the whole bonus theme
				fillRectangle(40, 70, 70, 20, 0);
				
				lastButtonState = 0;
			}
		}
		
		// Update button state for next frame
		if (!currentButtonState) {
			lastButtonState = 1;
			//  Button released, ready for next press (debouncing complete)
		}
		
		//  EXIT TO MENU
		// UP+DOWN combo: Hard to press accidentally, won't trigger during normal play
		if ((GPIOA->IDR & (1 << 8)) == 0 && (GPIOA->IDR & (1 << 11)) == 0) {
			addHighScore(score); 
			//  Try to add current score to top 10 before exiting
			gameRunning = 0;     
			//  Exit game loop, returns to showMenu()
			delay(500); 
			//  Prevent accidental menu button press (debouncing)
		}
		
		delay(50); 
		// 50ms: Limits loop to ~20 FPS (1000ms/50ms = 20 iterations per second)
		// Fast enough for smooth gameplay, slow enough to not waste CPU
	}
  } // End outer while(1) - returns to menu
	return 0;
}

//  MENU FUNCTION
void showMenu(void) {
	uint8_t menuItem = 0; 
	//  Currently selected option (0=start, 1=scores, 2=targets)
	
	// Button state tracking (for debouncing)
	static int lastUpState = 1;     
	static int lastDownState = 1;
	static int lastSelectState = 1;
	//  static: Persist between menu calls
	//  1: Buttons start unpressed (HIGH)
	
	int needsRedraw = 1; 
	//  Only redraw screen when something changes (prevents constant flicker)
	
	while(1) {
		if (needsRedraw) {
			// Draw menu once
			fillRectangle(0,0,128, 160, RGBToWord(0, 50, 100)); 
			//  dark blue: Looks better than black, easy on eyes
			
			printTextX2("DUCK HUNT", 15, 20, RGBToWord(255, 255, 0), RGBToWord(0, 50, 100)); 
			//  X2: Big title 
			//  yellow: Stands out against blue background, white looked a bit odd
			
			// Menu options - selected one is red, others white
			printText("START GAME", 20, 60, 
				(menuItem == 0) ? RGBToWord(255, 0, 0) : RGBToWord(255, 255, 255), 
				RGBToWord(0, 50, 100));
			printText("HIGH SCORES", 20, 75, 
				(menuItem == 1) ? RGBToWord(255, 0, 0) : RGBToWord(255, 255, 255), 
				RGBToWord(0, 50, 100));
			printText("TARGET SELECT", 20, 90, 
				(menuItem == 2) ? RGBToWord(255, 0, 0) : RGBToWord(255, 255, 255), 
				RGBToWord(0, 50, 100));
			// ternary: Conditional color (red if selected, white if not)
			
			// Instructions
			printText("UP/DOWN:Move", 10, 130, RGBToWord(200, 200, 200), 0);
			printText("RIGHT:Select", 10, 140, RGBToWord(200, 200, 200), 0);
			// gray: Less prominent than menu options (secondary info)
			
			needsRedraw = 0; // Don't redraw until something changes
		}
		
		// Read button states
		int upPressed = (GPIOA->IDR & (1 << 8)) == 0;
		int downPressed = (GPIOA->IDR & (1 << 11)) == 0;
		int selectPressed = (GPIOB->IDR & (1 << 4)) == 0;
		
		// Handle up button (move selection up)
		if (upPressed && lastUpState) {
			if (menuItem > 0) menuItem--; 
			//  >0: Can't go above first option
			lastUpState = 0; // Mark as processed
			needsRedraw = 1; // Need to redraw to show new selection
			delay(200); // Debounce delay
		}
		if (!upPressed) lastUpState = 1; // Button released
		
		// Handle down button (move selection down)
		if (downPressed && lastDownState) {
			if (menuItem < 2) menuItem++; 
			//  <2: Can't go below third option (0,1,2 = 3 options)
			lastDownState = 0;
			needsRedraw = 1;
			delay(200);
		}
		if (!downPressed) lastDownState = 1;
		
		// Handle select button (execute selected option)
		if (selectPressed && lastSelectState) {
			lastSelectState = 0;
			delay(200);
			
			if (menuItem == 0) {

				GPIOA->ODR |= (1 << 0);	// Turn on the LEDS
				GPIOA->ODR |= (1 << 1);
				GPIOA->ODR |= (1 << 9);
				wallhit = 0; // Resets trys that a player has
				return; 
				//  return: Exit menu function, starts game in main()
			} else if (menuItem == 1) {
				showHighScores(); 
				//  Show scores screen, then return here
				needsRedraw = 1; // Redraw menu when back
			} else if (menuItem == 2) {
				showTargetSelect(); 
				needsRedraw = 1;
			}
		}
		if (!selectPressed) lastSelectState = 1;
		
		delay(50); // Small loop delay
	}
}

//  HIGH SCORES SCREEN 
void showHighScores(void) {
	fillRectangle(0,0,128, 160, 0); // Black background
	printTextX2("HIGH SCORES", 10, 10, RGBToWord(255, 255, 0), 0); 
	
	// Display all 10 scores
	for (int i = 0; i < 10; i++) {
		char buffer[20];
		// Build rank string: "01. ", "02. ", etc
		buffer[0] = (i+1) / 10 + '0'; 
		//  /10: Gets tens digit (12/10=1)
		buffer[1] = (i+1) % 10 + '0'; 
		//  %10: Gets ones digit (12%10=2)
		//  +'0': Converts number to ASCII character (0->'0', 1 ->'1')
		buffer[2] = '.';
		buffer[3] = ' ';
		buffer[4] = 0; // Null terminator (end of string)
		
		printText(buffer, 10, 35 + (i * 12), RGBToWord(255, 255, 255), 0); 
		//  i*12: Space ranks 12 pixels apart vertically
		printNumber(highScores[i], 35, 35 + (i * 12), RGBToWord(255, 255, 0), 0); 
	}
	
	printText("RIGHT to go back", 5, 150, RGBToWord(200, 200, 200), 0);
	
	// Wait for button press
	while ((GPIOB->IDR & (1 << 4)) != 0) { delay(10); } 
	//  Wait until button pressed (loop while HIGH)
	while ((GPIOB->IDR & (1 << 4)) == 0) { delay(10); } 
	//  Wait until button released (prevents double-trigger)
	delay(200); // Final debounce
}

// TARGET SELECTION SCREEN 
void showTargetSelect(void) {
	static int lastSelectState = 1;
	int needsRedraw = 1;
	
	while(1) {
		if (needsRedraw) {
			fillRectangle(0,0,128, 160, 0);
			printTextX2("TARGETS", 20, 10, RGBToWord(255, 255, 0), 0);
			
			// Red target (always available)
			printText("RED", 10, 40, 
				(selectedTarget == 0) ? RGBToWord(255, 0, 0) : RGBToWord(255, 255, 255), 0);
			putImage(60, 38, 10, 10, TargetRed, 0, 0); 
			//  Show preview of crosshair
			
			// Blue target (unlocks at 50)
			if (maxScoreEver >= 50) {
				printText("BLUE", 10, 60, 
					(selectedTarget == 1) ? RGBToWord(255, 0, 0) : RGBToWord(255, 255, 255), 0);
				putImage(60, 58, 10, 10, TargetBlue, 0, 0);
			} else {
				printText("BLUE (50pts)", 10, 60, RGBToWord(100, 100, 100), 0); 
				//  gray: Shows locked status
			}
			
			// Purple target (unlocks at 500)
			if (maxScoreEver >= 500) {
				printText("PURPLE", 10, 80, 
					(selectedTarget == 2) ? RGBToWord(255, 0, 0) : RGBToWord(255, 255, 255), 0);
				putImage(60, 78, 10, 10, TargetGreen, 0, 0);
			} else {
				printText("PURPLE (500pts)", 10, 80, RGBToWord(100, 100, 100), 0);
			}
			
			// Pink target (unlocks at 1000)
			if (maxScoreEver >= 1000) {
				printText("PINK", 10, 100, 
					(selectedTarget == 3) ? RGBToWord(255, 0, 0) : RGBToWord(255, 255, 255), 0);
				putImage(60, 98, 10, 10, TargetYellow, 0, 0);
			} else {
				printText("PINK (1000pts)", 10, 100, RGBToWord(100, 100, 100), 0);
			}
			
			// Instructions, pretty seld explanatory
			printText("UP/DOWN:Select", 5, 130, RGBToWord(200, 200, 200), 0);
			printText("RIGHT:Confirm", 5, 140, RGBToWord(200, 200, 200), 0);
			printText("LEFT:Back", 5, 150, RGBToWord(200, 200, 200), 0);
			
			needsRedraw = 0;
		}
		
		// Read buttons
		int upPressed = (GPIOA->IDR & (1 << 8)) == 0;
		int downPressed = (GPIOA->IDR & (1 << 11)) == 0;
		int selectPressed = (GPIOB->IDR & (1 << 4)) == 0;
		int backPressed = (GPIOB->IDR & (1 << 5)) == 0;
		
		static int lastUpState = 1;
		static int lastDownState = 1;
		static int lastBackState = 1;
		
		// Move selection up (skip locked targets)
		if (upPressed && lastUpState) {
			if (selectedTarget > 0) {
				selectedTarget--;
				// Skip locked targets
				if (selectedTarget == 1 && maxScoreEver < 50) selectedTarget = 0;
				if (selectedTarget == 2 && maxScoreEver < 500) selectedTarget = 1;
				if (selectedTarget == 3 && maxScoreEver < 1000) selectedTarget = 2;
			}
			lastUpState = 0;
			needsRedraw = 1;
			delay(200);
		}
		if (!upPressed) lastUpState = 1;
		
		// Move selection down (skip locked targets)
		if (downPressed && lastDownState) {
			if (selectedTarget < 3) {
				selectedTarget++;
				// Skip locked targets
				if (selectedTarget == 1 && maxScoreEver < 50) selectedTarget = 0;
				if (selectedTarget == 2 && maxScoreEver < 500) selectedTarget = (maxScoreEver >= 50) ? 1 : 0;
				if (selectedTarget == 3 && maxScoreEver < 1000) selectedTarget = (maxScoreEver >= 500) ? 2 : (maxScoreEver >= 50) ? 1 : 0;
			}
			lastDownState = 0;
			needsRedraw = 1;
			delay(200);
		}
		if (!downPressed) lastDownState = 1;
		
		// Confirm selection
		if (selectPressed && lastSelectState) {
			lastSelectState = 0;
			delay(200);
			return; // Go back to menu
		}
		if (!selectPressed) lastSelectState = 1;
		
		// Back button
		if (backPressed && lastBackState) {
			lastBackState = 0;
			delay(200);
			return; // Go back to menu without saving
		}
		if (!backPressed) lastBackState = 1;
		
		delay(50);
	}
}

//  HIGH SCORE MANAGEMENT 
void addHighScore(uint16_t newScore) {
	// Only add if beats 10th place (worst score in top 10)
	if (newScore > highScores[9]) {
		highScores[9] = newScore;   
		//  [9]: Array is 0-indexed, 10th element is index 9
		sortHighScores();           
		//  Re-sort so new score moves to correct position
	}
}

void sortHighScores(void) {
	//  bubble sort: Simple to implement, only 10 elements so speed doesn't matter
	// Sorts highest to lowest
	for (int i = 0; i < 9; i++) {
		//  <9 not <10: Last iteration compares elements 8 and 9, don't need i=9
		for (int j = 0; j < 9 - i; j++) {
			//  -i: After each outer loop, largest element "bubbles" to end
			// Don't need to check already-sorted elements
			if (highScores[j] < highScores[j + 1]) {
				// <: Want descending order (highest first)
				// Swap adjacent elements
				uint16_t temp = highScores[j];
				highScores[j] = highScores[j + 1];
				highScores[j + 1] = temp;
			}
		}
	}
}

//  HARDWARE FUNCTIONS 

void initSysTick(void)
{
	SysTick->LOAD = 48000;    
	// 48000: At 48MHz, CPU executes 48 million cycles per second
	// 48000 cycles = 1 millisecond, so timer overflows every 1ms
	
	SysTick->CTRL = 7;        
	// 7 = 0b111: Enables timer (bit 0), enables interrupt (bit 1), uses CPU clock (bit 2)
	
	SysTick->VAL = 10;        
	//  Initial counter value (gets overwritten immediately, just needs non-zero)
	
	__asm(" cpsie i ");       
	//  Enable global interrupts in CPU
	// Assembly instruction: Change Processor State, Interrupt Enable
}

void SysTick_Handler(void)
{
	milliseconds++; 
	//  Hardware calls this every 1ms automatically
	// Just increment counter - keep this fast since it interrupts normal code
}

void initClock(void)
{
	// Chip boots at 8MHz (internal RC oscillator) which is too slow
	// Need to configure PLL (Phase-Locked Loop) to multiply frequency to 48MHz
	
	RCC->CR &= ~(1u<<24);        
	//  Turn off PLL before reconfiguring (bit 24 = PLL ON)
	// Must be off to change settings safely
	
	while( (RCC->CR & (1 <<25))); 
	//  Wait for PLL to actually turn off (bit 25 = PLL READY)
	// Hardware needs time to stop oscillator
	
	FLASH->ACR |= (1 << 0);      
	// Add 1 wait state for flash memory
	// At 48MHz, CPU is too fast for flash to respond in 1 cycle
	// Wait state gives flash 2 cycles to respond
	
	FLASH->ACR &=~((1u << 2) | (1u<<1));
	FLASH->ACR |= (1 << 4);      
	// Enable flash prefetch buffer
	// Speeds up code execution by fetching next instruction while current one executes
	
	RCC->CFGR &= ~((1u<<21) | (1u<<20) | (1u<<19) | (1u<<18)); 
	//  Clear PLL multiplier bits (bits 18-21)
	
	RCC->CFGR |= ((1<<21) | (1<<19) ); 
	// Set PLL multiplier to 12 (bits 21 and 19)
	// Internal oscillator is 4MHz, 4MHz x 12 = 48MHz
	
	RCC->CFGR |= (1<<14);        
	//  Set ADC prescaler 
	
	RCC->CR |= (1<<24);          
	//  Turn PLL back on with new settings
	
	RCC->CFGR |= (1<<1);         
	//  Switch system clock source from internal oscillator to PLL
	// Bit 1 = use PLL as system clock, now running at 48MHz
}

void delay(volatile uint32_t dly)
{
	uint32_t end_time = dly + milliseconds; 
	//  Calculate absolute target time (not relative)
	// Example: delay(100) at time 500 -> target = 600
	
	while(milliseconds != end_time)         
	//  !=: Loop until time reached
		__asm(" wfi ");                      
		//  WFI: Wait For Interrupt - puts CPU in low-power sleep mode
		// CPU wakes on next interrupt (SysTick every 1ms) to check condition
		// Saves power of the CPU
}

void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber)
{
	//  Buttons connect pin to ground when pressed
	// Without pull-up, unpressed pin would float (random HIGH/LOW readings)
	// Pull-up resistor makes unpressed = HIGH, pressed = LOW (active low)
	
	Port->PUPDR = Port->PUPDR &~(3u << BitNumber*2); 
	//  Each pin uses 2 bits in PUPDR register
	// Clear both bits for this pin (00 = no pull, 01 = pull-up, 10 = pull-down)
	// BitNumber*2 because pin N uses bits (2N, 2N+1)
	
	Port->PUPDR = Port->PUPDR | (1u << BitNumber*2); 
	//  Set to pull-up mode (01 in the 2-bit field)
}

void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode)
{
	//  Each pin can be: 00=input, 01=output, 10=alternate function, 11=analog
	
	uint32_t mode_value = Port->MODER;                  
	//  Read current register value
	
	Mode = Mode << (2 * BitNumber);                     
	//  Shift mode to correct position (each pin uses 2 bits)
	
	mode_value = mode_value & ~(3u << (BitNumber * 2)); 
	//  Clear old mode bits for this pin (AND with inverted mask)
	
	mode_value = mode_value | Mode;                     
	//  Set new mode bits (OR in new value)
	
	Port->MODER = mode_value;                           
	// Write back to register
}

void setupIO()
{
	RCC->AHBENR |= (1 << 18) + (1 << 17); 
	// Enable clocks for GPIOA and GPIOB peripherals
	// Peripherals default to OFF to save power, must enable before use
	// Bit 17 = GPIOA clock, bit 18 = GPIOB clock
	
	display_begin();                       
	//  Initialize LCD (configure SPI, reset display, send init commands)
	
	// Configure button pins as inputs
	pinMode(GPIOB,4,0);  // Right button (PB4), Mode 0 = input
	pinMode(GPIOB,5,0);  // Left button (PB5)
	pinMode(GPIOA,8,0);  // Up button (PA8)
	pinMode(GPIOA,11,0); // Down button (PA11)
	pinMode(GPIOB,3,0); // Shoot button (PB3)
	pinMode(GPIOB,1,1); // Buzzer (PB1)

	pinMode(GPIOA, 0, 1); // LEDS
	pinMode(GPIOA, 1, 1);
	pinMode(GPIOA, 9, 1);


	// Enable pull-ups so buttons read HIGH when not pressed
	enablePullUp(GPIOB,4);
	enablePullUp(GPIOB,5);
	enablePullUp(GPIOA,11);
	enablePullUp(GPIOA,8);
	enablePullUp(GPIOB,3);
}