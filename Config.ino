////////////////////////////////////////////////////////////////////////////////
// Initial Setup: Inputs, Display, Gamepieces, Serial
////////////////////////////////////////////////////////////////////////////////
void InitialSetup()
{
  SetupInputs();
  SetupDisplay();
  SetupGamePieces();
  DrawBackgroundGrid(true);
  gfx->flush();
  Serial.begin(115200);

  // This will show intro, but for now, let's go straight into the game
  ConfigureIntroButtons();
  _currentGameMode = INTRO; // Showing intro and waiting for user input    
}

void SetupInputs()
{
  pinMode(LED, OUTPUT);
  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  digitalWrite(LED, 0);
}