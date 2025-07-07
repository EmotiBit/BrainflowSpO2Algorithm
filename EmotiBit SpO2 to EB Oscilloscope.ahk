; Configurable parameters
currentOnesDigit := 0      ; Current ones digit of SpO2 level
firstClickX := 4500         ; X coordinate for the first click
firstClickY := 430          ; Y coordinate for the first click
secondClickX := 4420        ; X coordinate for the second click
secondClickY := 450        ; Y coordinate for the second click
delay := 2000        ; Delay in milliseconds

CoordMode "Mouse", "Screen"  ; Set mouse coordinates to screen reference

0::
{
  Global currentOnesDigit := 0
}

1::
{
  Global currentOnesDigit := 1
}

2::
{
  Global currentOnesDigit := 2
}

3::
{
  Global currentOnesDigit := 3
}

4::
{
  Global currentOnesDigit := 4
}

5::
{
  Global currentOnesDigit := 5
}

6::
{
  Global currentOnesDigit := 6
}

7::
{
  Global currentOnesDigit := 7
}

8::
{
  Global currentOnesDigit := 8
}

9::
{
  Global currentOnesDigit := 9
}

Backspace::
{
  ExitApp
}

MsgBox "Ready to start?"

Loop
{
  ; Left click at the first location
  Click firstClickX, firstClickY, "Left"
  Sleep delay

  SendText 9
  SendText currentOnesDigit

  ; Press Enter
  Send "{Enter}"

  ; Click at the second location
  Click secondClickX, secondClickY, "Left"
}