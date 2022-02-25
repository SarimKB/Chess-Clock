#include <TM1637Display.h>


/* DEFINE PINS */

  //For white
#define CLK1 4
#define DIO1 5

  //For black
#define CLK2 6
#define DIO2 7

  //Options Buttons
#define START 8
#define CHANGE_TIME_CTRL 10
#define WHITE_PB 2
#define BLACK_PB 3

//---------------------------------


// Define time controls
#define ONE_MIN 60000
#define TWO_MIN 2*ONE_MIN
#define THREE_MIN 3*ONE_MIN
#define FIVE_MIN 5*ONE_MIN
#define TEN_MIN 10*ONE_MIN
#define FIFTEEN_MIN 15*ONE_MIN
#define THIRTY_MIN 30*ONE_MIN

//Time Conersions ("time" is in ms)
#define timeToSeconds(time) ((time / 1000) %60)
#define timeToMinutes(time) (((time / 1000) / 60) % 60)
  
//Displaying "Lets"
const uint8_t Lets[] = {
  SEG_D | SEG_E | SEG_F,                          //L
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,          //E
  SEG_D | SEG_E | SEG_F | SEG_G,                  //t
  SEG_A | SEG_C | SEG_D | SEG_F | SEG_G           //S
};

//Displaying "Play"
const uint8_t Play[] = {
  SEG_A | SEG_B | SEG_E | SEG_F | SEG_G,          //P
  SEG_D | SEG_E | SEG_F,                          //L
  SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G,  //A
  SEG_B | SEG_C | SEG_D | SEG_F | SEG_G           //y
};


/* GLOBAL VARIABLES */

    // Display Object for White
TM1637Display display_white = TM1637Display(CLK1, DIO1);

    // Display Object for Black
TM1637Display display_black = TM1637Display(CLK2, DIO2);

    // Times for both sides in ms
unsigned long timeLeft_white = 0;
unsigned long timeLeft_black = 0;

  // Keeps track of whose turn it is. 1 = White, 0 = Black
  // Declared volatile due to value changing within ISR
volatile bool currentTurn = 1;

  // Keeping track of whether the game has begun or not
int gameStarted = 2;

  // State variables
int timeCtrlOption = 1;
int timeCtrlBtn_prevState = 0;
long timeCtrl_prevMillis = millis();

int debounce_ms = 100;

void setup() {
  display_white.setBrightness(5);
  display_black.setBrightness(5);

  // Setting input buttons
  pinMode(START, INPUT);
  pinMode(CHANGE_TIME_CTRL, INPUT);
  pinMode(WHITE_PB, INPUT);
  pinMode(BLACK_PB, INPUT);

  // Attaching interrupts
  attachInterrupt(digitalPinToInterrupt(WHITE_PB), ISR_turnChange_W, RISING);
  attachInterrupt(digitalPinToInterrupt(BLACK_PB), ISR_turnChange_B, RISING);
}

void loop() {

  // Intro
  if(gameStarted == 2){

    // Startup Message
    display_white.setSegments(Lets);
    display_black.setSegments(Play);

    delay(2000);
    gameStarted = 0;
    setTimeControl(display_white, display_black, ONE_MIN);
  }

  // Picking time control
  if(gameStarted == 0){

    switch (timeCtrlOption){

      case 1:
        setTimeControl(display_white, display_black, ONE_MIN);
        if(digitalRead(START) == 1){
          gameStarted = 1;
        }

        //Changing time control (with debouncing)
        changeTimeCtrl(2);
        break;

      case 2:
        setTimeControl(display_white, display_black, TWO_MIN);
        if(digitalRead(START) == 1){
          gameStarted = 1;
        }

        //Changing time control (with debouncing)
        changeTimeCtrl(3);
        break;

      case 3:
        setTimeControl(display_white, display_black, THREE_MIN);
        if(digitalRead(START) == 1){
          gameStarted = 1;
        }

        //Changing time control (with debouncing)
        changeTimeCtrl(5);
        break;

      case 5:
        setTimeControl(display_white, display_black, FIVE_MIN);
        if(digitalRead(START) == 1){
          gameStarted = 1;
        }

        //Changing time control (with debouncing)
        changeTimeCtrl(10);
        break;

      case 10:
        setTimeControl(display_white, display_black, TEN_MIN);
        if(digitalRead(START) == 1){
          gameStarted = 1;
        }

        //Changing time control (with debouncing)
        changeTimeCtrl(15);
        break;

      case 15:
        setTimeControl(display_white, display_black, FIFTEEN_MIN);
        if(digitalRead(START) == 1){
          gameStarted = 1;
        }

        //Changing time control (with debouncing)
        changeTimeCtrl(30);
        break;

      case 30:
        setTimeControl(display_white, display_black, THIRTY_MIN);
        if(digitalRead(START) == 1){
          gameStarted = 1;
        }

        //Changing time control (with debouncing)
        changeTimeCtrl(1);
        break;

      default:
        break;
    }


  }

  // Game begin and respective timers running
  if(gameStarted == 1){

    if(timeLeft_white == 0 || timeLeft_black == 0){
      gameStarted = 2;
      delay(5000);
    }
    else{
      countTimer();
    }

  }

} // END OF LOOP


// --------------------------FUNCTIONS---------------------------


// REQUIRES
// N/A
// PROMISES
// Switches the current turn to black
void ISR_turnChange_W(){
  if(currentTurn == 1 && gameStarted == 1){
    currentTurn = !currentTurn;
  }
}

// REQUIRES
// N/A
// PROMISES
// Switches the current turn to white
void ISR_turnChange_B(){
  if(currentTurn == 0 && gameStarted == 1){
    currentTurn = !currentTurn;
  }
}

// REQUIRES
// N/A
// PROMISES
// Counts down the timers for whichever player's turn it currently is
void countTimer(){

    // White turn timer
    if(currentTurn == 1){
        
        while(timeLeft_white > 0){
            
            // White submits move
            if(currentTurn == 0){
              return;
            }

            // Ticking down one second and updating the display for white
            timeLeft_white = timeLeft_white - 1000;
            int seconds = timeToSeconds(timeLeft_white);
            int minutes = timeToMinutes(timeLeft_white);
            display_white.showNumberDecEx(seconds, 0, true, 2, 2);
            display_white.showNumberDecEx(minutes, 0x40, true, 2, 0);
            delay(1000);
        }
    }

    // Black turn timer
    if(currentTurn == 0){
        
        while(timeLeft_black > 0){

            // Black submits move
            if(currentTurn == 1){
              return;
            }

            // Ticking down one second and updating the display for black
            timeLeft_black = timeLeft_black - 1000;
            int seconds = timeToSeconds(timeLeft_black);
            int minutes = timeToMinutes(timeLeft_black);
            display_black.showNumberDecEx(seconds, 0, true, 2, 2);
            display_black.showNumberDecEx(minutes, 0x40, true, 2, 0);
            delay(1000);
        }
    }

}

// REQUIRES
// 2 TM1637Display Objects and a time in milliseconds
// PROMISES
// Updates the display modules according to the input time control
void setTimeControl(TM1637Display display_w, TM1637Display display_b, unsigned long time){ 

    int seconds = timeToSeconds(time);
    int minutes = timeToMinutes(time);

    display_w.showNumberDecEx(seconds, 0, true, 2, 2);
    display_w.showNumberDecEx(minutes, 0x40, true, 2, 0);

    display_b.showNumberDecEx(seconds, 0, true, 2, 2);
    display_b.showNumberDecEx(minutes, 0x40, true, 2, 0);

    timeLeft_white = time;
    timeLeft_black = time;
    currentTurn = 1;
}

// REQUIRES
// An integer indicating the next time control
// PROMISES
// If the corresponding input button is pushed, it will browse to a different time control.
void changeTimeCtrl(int next_timeCtrl){

  // Accounting for a debounce effect
  if( (millis() - timeCtrl_prevMillis) > debounce_ms ){
        timeCtrl_prevMillis = millis();
        if( (digitalRead(CHANGE_TIME_CTRL) == 1) && (timeCtrlBtn_prevState == 0)){
          timeCtrlOption = next_timeCtrl;
          timeCtrlBtn_prevState = 1;
        }
        else{
          timeCtrlBtn_prevState = 0;
        }
      }
}
