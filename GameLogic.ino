// Gamepieces
GamePiece gamePieces[7] = {0};
GamePiece _currentGamePiece;
int _currentGamePieceIdx = -1;
int _currentGamePieceX;
int _currentGamePieceY;
int _currentScore = 0;

////////////////////////////////////////////////////////////////////////////////
// Setup Game Pieces
////////////////////////////////////////////////////////////////////////////////
void SetupGamePieces()
{
  gamePieces[0] = {1,4,gfx->color565(125,0,0),{{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0}}};       // I
  gamePieces[1] = {3,2,gfx->color565(0,125,0),{{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}};       // T
  gamePieces[2] = {2,2,gfx->color565(0,0,125),{{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}};       // O
  gamePieces[3] = {3,2,gfx->color565(125,125,0),{{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}};     // Z
  gamePieces[4] = {3,2,gfx->color565(125,0,125),{{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}};     // S
  gamePieces[5] = {3,2,gfx->color565(125,125,125),{{1,1,1,0},{0,0,1,0},{0,0,0,0},{0,0,0,0}}};   // L
  gamePieces[6] = {3,2,gfx->color565(0,125,125),{{1,1,1,0},{1,0,0,0},{0,0,0,0},{0,0,0,0}}};     // L2
}


////////////////////////////////////////////////////////////////////////////////
// Game Mode: PLAY. Main Game Loop
////////////////////////////////////////////////////////////////////////////////
int gpFallDelay = 0;
void ProcessGame()
{
  if (_currentGamePieceIdx == -1)  // No active piece. Randomly select one and initialize
  {
    InitializeRandomGamePiece();
    return;
  }
  
  // Return control back to button handler before it's time to shift GamePiece again
  if (gpFallDelay++ <= GAMEPIECESPEED)
  {
    return;
  }
  gpFallDelay = 0;

  if (!CanGamePieceFall(_currentGamePiece, _currentGamePieceX, _currentGamePieceY))
  {
    Serial.println("Can't move");
    // 1. Commit current gamepiece to screen matrix
    //CommitGamePieceToMatrix(_currentGamePiece, _currentGamePieceX, _currentGamePieceY);
    CopyTempScreenMatrixToBackground();
    
    // 2. Check for any completed rows
    int completedRows = ProcessCompletedRows();
    
    // 3. Calculate game points
    _currentScore += (completedRows * 100);

    // 4. Clear current GamePiece so that new one can be generated
    _currentGamePieceIdx = -1;
    
    Serial.print("_currY: ");
    Serial.println(_currentGamePieceY);
    Serial.print("Grid: ");
    Serial.println(GRIDHEIGHT);
    // 5. Check for "Game Over" condition (filled all the way up)
    if (_currentGamePieceY == 0)
    {
      Serial.println("Game Over");
      ConfigureGameOverButtons();
      _currentGameMode = GAMEOVER;
      return;
    }

    //tmpStop = true;
    return;
  }
  _currentGamePieceY++;
  RenderCurrentGamePiece(_currentGamePiece, _currentGamePieceX, _currentGamePieceY);
}


////////////////////////////////////////////////////////////////////////////////
// Button Handlers
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Move Gamepiece to the left
////////////////////////////////////////////////////////////////////////////////
void GameBtn1Click()
{
  if (_currentGamePieceX > 0)
  {
    _currentGamePieceX--;
    RenderCurrentGamePiece(_currentGamePiece, _currentGamePieceX, _currentGamePieceY);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Move Gamepiece to the right
////////////////////////////////////////////////////////////////////////////////
void GameBtn2Click()
{
  if (_currentGamePieceX < GRIDWIDTH - _currentGamePiece.Width)
  {
    _currentGamePieceX++;
    RenderCurrentGamePiece(_currentGamePiece, _currentGamePieceX, _currentGamePieceY);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Rotate Gamepiece Counterclockwise
////////////////////////////////////////////////////////////////////////////////
void GameBtn1Hold()
{
  GamePiece gp = _currentGamePiece;
  rotatePieceCounterClockwise(&_currentGamePiece, &gp);
  _currentGamePiece = gp;
  RenderCurrentGamePiece(_currentGamePiece, _currentGamePieceX, _currentGamePieceY);
}

////////////////////////////////////////////////////////////////////////////////
// Rotate Gamepiece Clockwise
////////////////////////////////////////////////////////////////////////////////
void GameBtn2Hold()
{
  GamePiece gp = _currentGamePiece;
  rotatePieceClockwise(&_currentGamePiece, &gp);
  _currentGamePiece = gp;
  RenderCurrentGamePiece(_currentGamePiece, _currentGamePieceX, _currentGamePieceY);
}

////////////////////////////////////////////////////////////////////////////////
// Ignore All Buttons
////////////////////////////////////////////////////////////////////////////////
void IgnoreBtn()
{
  return;
}

////////////////////////////////////////////////////////////////////////////////
// Restart Game Button
////////////////////////////////////////////////////////////////////////////////
void RestartGameBtn()
{
  ClearOutTempScreenMatrix();
  ClearOutBackgroundScreenMatrix();
  _currentScore = 0;
  ConfigureGetReadyButtons();
  _currentGameMode = GETREADY;
}

////////////////////////////////////////////////////////////////////////////////
// Initializers and Misc stuff
////////////////////////////////////////////////////////////////////////////////
void InitializeRandomGamePiece()
{
  uint32_t randomNumber = esp_random() % 7;
  Serial.print("Rand piece: ");
  Serial.println(randomNumber);
  _currentGamePieceX = 4;
  _currentGamePieceY = 0;
  _currentGamePieceIdx = (int)randomNumber;
  _currentGamePiece = gamePieces[_currentGamePieceIdx];
  RenderCurrentGamePiece(_currentGamePiece, _currentGamePieceX, _currentGamePieceY);
  delay(GAMEPIECESPEED);
}