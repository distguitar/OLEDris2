typedef struct ButtonState {
  bool PreviousState;
  bool CurrentState;
  int CurrentStateTimer;
  bool IsHolding;
  void (*OnClick)(void);
  void (*OnClickHold)(void);
};


typedef struct GamePiece {
  int Width;
  int Height;
  uint16_t Color;
  byte FigureMatrix[4][4];
};

enum GameMode {
  INTRO,
  GETREADY,
  PLAY,
  GAMEOVER
};