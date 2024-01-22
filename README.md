# ChessCheater
A sadly inefficient chess cheating device. 

![Connecting to WiFi](https://github.com/radio-satellites/ChessCheater/blob/main/ChessCheater.gif?raw=true)

<i>Trying to connect to WiFi...</i>

<b>Please do not use this device to cheat in tournaments or in formal events. I am not responsible for anything you do with this. </b>

### Backstory

I needed to beat my brother at chess. 

Problem: <b>I am bad at chess. </b>

So why invest time in learning and practicing chess puzzles? Let's just write 400-something lines of code to make this work...

## Hardware
The firmware runs on an Adafruit MagTag. This is currently out of stock on the Adafruit store, however I had around for years and never really did anything proper with it. 

The only extra hardware required is some sort of battery pack. I didn't have any (usable) LiPos, so I used two 2xAA battery packs plugged together to make 6V. The theoretical maximum is 10v for the input, so 6v is fine. 

## Theory of Operation
The device connects to the server, which runs StockFish and computes all of the moves on a larger machine. The device then displays the optimal move, and the user can select the opponent's move later. There are also more extras such as board display, some GUI, etc. The device also tries to look inconspicious and secret:

![Nothing suspicious...](https://github.com/radio-satellites/ChessCheater/blob/main/WIN_20240121_20_09_06_Pro.jpg?raw=true)



