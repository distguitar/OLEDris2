#include <Arduino.h>
#include <Arduino_GFX_Library.h> // https://github.com/moononournation/Arduino_GFX.git
#include "esp_system.h"
#include "types.h"

// Hardware
#define BTN1 0
#define BTN2 21
#define LED 38

// Button debounce delays
const int debounceDelay = 50;  // Adjust as needed
const int holdInterval = 1000; // Trigger OnClickHold every 1 second

// Game Definitions
#define SQUAREWIDTH 24
#define SQUAREHEIGHT 24
#define GRIDHEIGHT 20
#define GRIDWIDTH 10
#define SCREENH 536
#define SCREENW 240
#define ROWFLASHDELAY 100
#define GAMEPIECESPEED 5  // Game Pieces falling speed

// For Display 536 x 240
Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    6 /* cs */, 47 /* sck */, 18 /* d0 */, 7 /* d1 */, 48 /* d2 */, 5 /* d3 */);
Arduino_GFX *gfx_base = new Arduino_RM67162(bus, 17 /* RST */, 0 /* rotation */);
Arduino_Canvas *gfx = new Arduino_Canvas(SCREENW, SCREENH, gfx_base);

// Color Settings
uint16_t GridColor = gfx->color565(50,50,50);
uint16_t ScoreColor = gfx->color565(200,200,200);

// Buttons
ButtonState _button1;
ButtonState _button2;

// Current Game Mode
GameMode _currentGameMode;

////////////////////////////////////////////////////////////////////////////////
// Main Setup Entry Point
////////////////////////////////////////////////////////////////////////////////
void setup() {
  InitialSetup();
}

////////////////////////////////////////////////////////////////////////////////
// Main Loop
////////////////////////////////////////////////////////////////////////////////
void loop() {
    HandleButtons();
    
    switch (_currentGameMode)
    {
      case INTRO:
        ProcessIntro();
        break;
      case GETREADY:
        ProcessGetReady();
        break;
      case PLAY:
        ProcessGame();
        break;
      case GAMEOVER:
        ProcessGameover();
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Handle Buttons
////////////////////////////////////////////////////////////////////////////////
void HandleButtons()
{
    bool button1IsPressed = (digitalRead(BTN1) == LOW); // Button pressed
    bool button2IsPressed = (digitalRead(BTN2) == LOW); // Button pressed

    // Update button states
    checkButtonState(_button1, button1IsPressed);
    checkButtonState(_button2, button2IsPressed);

    delay(debounceDelay); // Prevent excessive polling
}