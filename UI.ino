// Permanent Screen Matrix (with GamePieces that have completed fall)
uint16_t backgroundScreenMatrix[GRIDHEIGHT][GRIDWIDTH] = {0};
// Temp Screen Matrix (With GamePieces that are currently falling down)
uint16_t tempScreenMatrix[GRIDHEIGHT][GRIDWIDTH] = {0};

////////////////////////////////////////////////////////////////////////////////
// Setup Display
////////////////////////////////////////////////////////////////////////////////
void SetupDisplay()
{
  gfx->begin();
  gfx->fillScreen(BLACK);
  gfx->flush();
  setBrightness(255);
}

////////////////////////////////////////////////////////////////////////////////
// Setup Screen Brightness
////////////////////////////////////////////////////////////////////////////////
void setBrightness(uint8_t value)
{
    bus->beginWrite();
    bus->writeC8D8(0x51, value);
    bus->endWrite();
}

////////////////////////////////////////////////////////////////////////////////
// Game Mode: INTRO. Handle Main Loop
////////////////////////////////////////////////////////////////////////////////
int _introR = 255;
int _introG = 255;
int _introB = 255;
uint16_t _hue = 0;

#define INTRO_LINE_SPACING 50

////////////////////////////////////////////////////////////////////////////////
// Process Intro
////////////////////////////////////////////////////////////////////////////////
void ProcessIntro()
{
  _hue = (_hue + 2) % 360; // Adjust the speed with this value
  uint16_t color = hsvToRgb565(_hue, 200, 200); // Full saturation & brightness

  gfx->setCursor(20, 180);
  gfx->setTextColor(color);
  gfx->setTextSize(5 /* x scale */, 5 /* y scale */, 1 /* pixel_margin */);
  gfx->setCursor(48, 180);
  gfx->println("Press");
  gfx->setCursor(78, 180 + INTRO_LINE_SPACING);
  gfx->println("Any");
  gfx->setCursor(78, 180 + INTRO_LINE_SPACING * 2);
  gfx->println("Key");
  gfx->flush();
  delay(100);
}

////////////////////////////////////////////////////////////////////////////////
// Convert HSV to RGB565
////////////////////////////////////////////////////////////////////////////////
uint16_t hsvToRgb565(uint16_t h, uint8_t s, uint8_t v) {
  float r, g, b;

  int i = h / 60;
  float f = (h / 60.0) - i;
  float p = v * (1 - s / 255.0);
  float q = v * (1 - f * s / 255.0);
  float t = v * (1 - (1 - f) * s / 255.0);

  switch(i % 6) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    case 5: r = v; g = p; b = q; break;
  }

  // Convert to RGB565
  uint8_t R = (uint8_t)r;
  uint8_t G = (uint8_t)g;
  uint8_t B = (uint8_t)b;
  return ((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3);
}


////////////////////////////////////////////////////////////////////////////////
// Game Mode: READY. Process Get Ready Delay
////////////////////////////////////////////////////////////////////////////////
// Show Get Ready
void ProcessGetReady()
{
  for (int i=3; i>0; i--)
  {
    DrawBackgroundGrid(true);
    gfx->setCursor(100, 210);
    gfx->setTextColor(gfx->color565(255,255,255));
    gfx->setTextSize(8 /* x scale */, 8 /* y scale */, 1 /* pixel_margin */);
    gfx->println(i);
    gfx->flush();
    delay(1500);
  }
    DrawBackgroundGrid(true);
    gfx->flush();
    ConfigurePlayButtons();
    _currentGameMode=PLAY;
}

////////////////////////////////////////////////////////////////////////////////
// Game Mode: GAMEOVER. Show end splash screen
////////////////////////////////////////////////////////////////////////////////
bool processedGameOver = false;
void ProcessGameover()
{
  processedGameOver = true;
  gfx->setCursor(30, 180);
  gfx->setTextColor(gfx->color565(255,255,255));
  gfx->setTextSize(8 /* x scale */, 8 /* y scale */, 1 /* pixel_margin */);
  gfx->println("GAME");
  gfx->println("OVER");
  gfx->flush();
 }

////////////////////////////////////////////////////////////////////////////////
// Rendering Methods
////////////////////////////////////////////////////////////////////////////////
void RenderCurrentGamePiece(GamePiece gamePiece, int px, int py)
{
  CopyBackgroundScreenMatrixToTemp();
  for (int y=0; y<gamePiece.Height; y++)
  {
    for (int x=0; x<gamePiece.Width; x++)
    {
      if (gamePiece.FigureMatrix[y][x] == 1)
      {
        tempScreenMatrix[y+py][x+px] = gamePiece.Color;
      }
    }
  }
  RenderTempScreenMatrix();
}

////////////////////////////////////////////////////////////////////////////////
// Rotate GamePiece Clockwise
////////////////////////////////////////////////////////////////////////////////
void rotatePieceClockwise(GamePiece* src, GamePiece* dst) {
  dst->Width = src->Height;
  dst->Height = src->Width;
  dst->Color = src->Color;

  for (int row = 0; row < src->Height; row++) {
    for (int col = 0; col < src->Width; col++) {
      dst->FigureMatrix[col][src->Height - 1 - row] = src->FigureMatrix[row][col];
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// Rotate GamePiece Counterclockwise
////////////////////////////////////////////////////////////////////////////////
void rotatePieceCounterClockwise(GamePiece* src, GamePiece* dst) {
  dst->Width = src->Height;
  dst->Height = src->Width;
  dst->Color = src->Color;

  for (int row = 0; row < src->Height; row++) {
    for (int col = 0; col < src->Width; col++) {
      dst->FigureMatrix[src->Width - 1 - col][row] = src->FigureMatrix[row][col];
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// Test if gamepiece at current position can fall down one more level
////////////////////////////////////////////////////////////////////////////////
bool CanGamePieceFall(GamePiece gp, int x, int y)
{
  // Is game piece at the bottom?
  if ((y + gp.Height) >= GRIDHEIGHT) 
  {
    return false;
  }
  y++;

  if (IsGamePieceOverlapping(gp,x,y))
  {
    return false;
  }
  return true;
}

////////////////////////////////////////////////////////////////////////////////
// Is Game Piece Overlapping
////////////////////////////////////////////////////////////////////////////////
bool IsGamePieceOverlapping(GamePiece gamePiece, int px, int py)
{
  for (int y=0; y<gamePiece.Height; y++)
  {
    for (int x=0; x<gamePiece.Width; x++)
    {
      if (gamePiece.FigureMatrix[y][x] == 1)
      {
        if (backgroundScreenMatrix[y+py][x+px] != 0) return true;
      }
    }
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// Copy Background Screen Matrix to Temp Matrix
////////////////////////////////////////////////////////////////////////////////
void CopyBackgroundScreenMatrixToTemp()
{
  for(int y=0; y<GRIDHEIGHT; y++)
  {
    for (int x=0; x<GRIDWIDTH; x++)
    {
      tempScreenMatrix[y][x] = backgroundScreenMatrix[y][x];
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// Copy Temp Screen Matrix to Background Matrix
////////////////////////////////////////////////////////////////////////////////
void CopyTempScreenMatrixToBackground()
{
  for(int y=0; y<GRIDHEIGHT; y++)
  {
    for (int x=0; x<GRIDWIDTH; x++)
    {
      backgroundScreenMatrix[y][x] = tempScreenMatrix[y][x];
    }
  }
}


////////////////////////////////////////////////////////////////////////////////
// Render Temp Screen Matrix
////////////////////////////////////////////////////////////////////////////////
void RenderTempScreenMatrix()
{
  DrawBackgroundGrid(true);
  for (int y=0; y<GRIDHEIGHT; y++)
  {
    for (int x=0; x<GRIDWIDTH; x++)
    {
      if (tempScreenMatrix[y][x] != 0)
      {
        gfx->fillRect(x*SQUAREWIDTH,y*SQUAREHEIGHT,SQUAREWIDTH,SQUAREHEIGHT,tempScreenMatrix[y][x]);
        gfx->drawRect(x*SQUAREWIDTH,y*SQUAREHEIGHT,SQUAREWIDTH,SQUAREHEIGHT,GridColor);
      }
    }
  }
  DisplayScore(_currentScore);
  gfx->flush();
}

////////////////////////////////////////////////////////////////////////////////
// Draw Background Grid
////////////////////////////////////////////////////////////////////////////////
void DrawBackgroundGrid(bool fillSquares)
{
  if (fillSquares) 
  {
    gfx->fillScreen(BLACK);
  }

  for(int x=0;x<GRIDWIDTH;x++)
  {
    for (int y=0;y<GRIDHEIGHT;y++)
    {
      gfx->drawRect(x*SQUAREWIDTH,y*SQUAREHEIGHT,SQUAREWIDTH,SQUAREHEIGHT,GridColor);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// Scan game matrix to see if any rows were completed
////////////////////////////////////////////////////////////////////////////////
int ProcessCompletedRows()
{
  int numCompletedRows = 0;
  bool rowStatus[GRIDHEIGHT]; //0: Not completed, 1: Completed

  for (int row = 0; row < GRIDHEIGHT; row++)
  {
    for (int col = 0; col < GRIDWIDTH; col++)
    {
      rowStatus[row] = true;
      if (backgroundScreenMatrix[row][col]==0)
      {
        rowStatus[row] = false;
        break;
      }
    }
      if (rowStatus[row] == true)
      {
        numCompletedRows++;
      }
  }

  // No rows were completed
  if (numCompletedRows == 0)
  {
    return numCompletedRows;
  }

  // Animate rows to be removed
  FlashCompletedRows(rowStatus);
  
  // Remove Completed Rows
  RemoveCompletedRows(rowStatus);

  return numCompletedRows;
}

////////////////////////////////////////////////////////////////////////////////
// Visually flash completed rows
////////////////////////////////////////////////////////////////////////////////
void FlashCompletedRows(bool rowStatus[])
{
  // Flash rows 5 times
  for (int i=0; i<5; i++)
  {
    HighLightCompletedRows(rowStatus);
    digitalWrite(LED, 0);
    delay(ROWFLASHDELAY);
    RenderTempScreenMatrix();
    digitalWrite(LED, 1);
    delay(ROWFLASHDELAY);
  }
  digitalWrite(LED, 0);
}

////////////////////////////////////////////////////////////////////////////////
// Highlight completed rows
////////////////////////////////////////////////////////////////////////////////
void HighLightCompletedRows(bool rowStatus[])
{
  for (int i=0; i<GRIDHEIGHT; i++)
  {
    if (rowStatus[i])
    {
      gfx->fillRect(0,i*SQUAREHEIGHT,SCREENW,SQUAREHEIGHT,gfx->color565(0,0,0));
    }
  }
  gfx->flush();
}

////////////////////////////////////////////////////////////////////////////////
// Remove Completed Rows
////////////////////////////////////////////////////////////////////////////////
void RemoveCompletedRows(bool rowStatus[])
{

  ClearOutTempScreenMatrix();
  int realRow = GRIDHEIGHT-1;
  // Clear matrix, then redraw it by skipping completed rows.
  for(int row=GRIDHEIGHT-1; row>=0; row--)
  {
    Serial.print ("Row: ");
    Serial.print (row);
    Serial.print ("Status ");
    Serial.println(rowStatus[row]);

    //Skip
    if (rowStatus[row] == true) {
      Serial.print("Removing row ");
      Serial.println(row);
      continue;
    }
    for (int col=0; col<GRIDWIDTH; col++)
    {
      tempScreenMatrix[realRow][col] = backgroundScreenMatrix[row][col];
    }
    realRow--;
  }

  CopyTempScreenMatrixToBackground();
}

////////////////////////////////////////////////////////////////////////////////
// Clear Out Temp Screen Matrix
////////////////////////////////////////////////////////////////////////////////
void ClearOutTempScreenMatrix()
{
  for (int row = 0; row<GRIDHEIGHT; row++)
  {
    for (int col = 0; col<GRIDWIDTH; col++)
    {
      tempScreenMatrix[row][col] = 0;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// Clear Out Background Screen Matrix
////////////////////////////////////////////////////////////////////////////////
void ClearOutBackgroundScreenMatrix()
{
  for (int row = 0; row<GRIDHEIGHT; row++)
  {
    for (int col = 0; col<GRIDWIDTH; col++)
    {
      backgroundScreenMatrix[row][col] = 0;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// Display Game Score
////////////////////////////////////////////////////////////////////////////////
void DisplayScore(int score)
{
  gfx->setCursor(0, SCREENH-40);
  gfx->setTextColor(ScoreColor);
  gfx->setTextSize(4 /* x scale */, 4 /* y scale */, 1 /* pixel_margin */);
  gfx->println(score);
}
