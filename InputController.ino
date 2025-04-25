int gpIndex = 0;

////////////////////////////////////////////////////////////////////////////////
// Configure Intro Buttons
////////////////////////////////////////////////////////////////////////////////
void ConfigureIntroButtons()
{
  Serial.println("Starting INTRO configuration");
  _button1 = {false,false,0,false,&PressAnyButton,&PressAnyButton};
  _button2 = {false,false,0,false,&PressAnyButton,&PressAnyButton};
}

////////////////////////////////////////////////////////////////////////////////
// Configure Play Buttons
////////////////////////////////////////////////////////////////////////////////
void ConfigurePlayButtons()
{
  Serial.println("Starting GAME configuration");
  _button1 = {false,false,0,false,&GameBtn1Click,&GameBtn1Hold};
  _button2 = {false,false,0,false,&GameBtn2Click,&GameBtn2Hold};
}

////////////////////////////////////////////////////////////////////////////////
// Configure Get Ready Buttons
////////////////////////////////////////////////////////////////////////////////
void ConfigureGetReadyButtons()
{
  Serial.println("Starting GAME configuration");
  _button1 = {false,false,0,false,&IgnoreBtn,&IgnoreBtn};
  _button2 = {false,false,0,false,&IgnoreBtn,&IgnoreBtn};
}

////////////////////////////////////////////////////////////////////////////////
// Configure Game Over Buttons
////////////////////////////////////////////////////////////////////////////////
void ConfigureGameOverButtons()
{
  Serial.println("Starting GAME configuration");
  _button1 = {false,false,0,false,&RestartGameBtn,&RestartGameBtn};
  _button2 = {false,false,0,false,&RestartGameBtn,&RestartGameBtn};
}


////////////////////////////////////////////////////////////////////////////////
// Game Mode: INTRO. Press any key
////////////////////////////////////////////////////////////////////////////////
void PressAnyButton()
{
  _currentGameMode = GETREADY;
  ConfigureGetReadyButtons();
}

////////////////////////////////////////////////////////////////////////////////
// Check Button State and Route to Appropriate Handler
////////////////////////////////////////////////////////////////////////////////
void checkButtonState(ButtonState &button, bool isPressed) {
    // Update button states
    button.PreviousState = button.CurrentState;
    button.CurrentState = isPressed;

    // If the button was pressed and is being held
    if (button.CurrentState) {
        button.CurrentStateTimer += debounceDelay; // Simulating elapsed time

        if (button.CurrentStateTimer >= holdInterval) {
            button.OnClickHold();
            button.CurrentStateTimer = 0; // Reset timer for subsequent holds
            button.IsHolding = true;    // Mark as holding
        }
    } else {
        // If the button is released
        if (!button.IsHolding && button.PreviousState) {
            button.OnClick(); // Trigger OnClick ONLY if it was not a hold
        }

        // Reset timer and holding flag
        button.CurrentStateTimer = 0;
        button.IsHolding = false;
    }
}
