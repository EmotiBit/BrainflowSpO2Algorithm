; Configurable parameters
defaultTensDigit := 9      ; Default tens digit of SpO2 level
repeatCount := 10           ; Number of times to repeat the loop
firstClickX := 4500         ; X coordinate for the first click
firstClickY := 430          ; Y coordinate for the first click
secondClickX := 4420        ; X coordinate for the second click
secondClickY := 450        ; Y coordinate for the second click
promptDelay := 2000        ; Delay in milliseconds

CoordMode "Mouse", "Screen"  ; Set mouse coordinates to screen reference

repeatInput := InputBox("How many times do you want to repeat the loop?", "Repeat Count", "w200 h120")
if (repeatInput.Result = "OK" && repeatInput.Value ~= "^\d+$" && repeatInput.Value > 0)
{
  repeatCount := repeatInput.Value
}
else
{
  MsgBox "Please enter a valid positive integer.", "Invalid Input", 48
  ExitApp
}

Loop repeatCount
{
  ; Left click at the first location
  Click firstClickX, firstClickY, "Left"
  Sleep promptDelay

  ; Prompt user for a single digit between 0-9
  userInput := InputBox("Please enter a single digit (0-9):", "Enter Digit", "w150 h120")
  if (userInput.Result = "OK" && userInput.Value ~= "^[0-9]$")
  {
    SendText defaultTensDigit
    SendText userInput.Value
  }
  else
  {
    MsgBox "Please enter a single digit between 0 and 9.", "Invalid Input", 48
    continue
  }

  ; Press Enter
  Send "{Enter}"

  ; Click at the second location
  Click secondClickX, secondClickY, "Left"
}
MsgBox "Data collection is finished.", "Done", 64