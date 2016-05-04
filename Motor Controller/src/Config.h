//PIN Configuration
#define LED 3
#define SOUND 13

#define DIR 5
#define STEP 6
#define SLEEP 7
#define RESET 8
#define M2 9
#define M1 10
#define M0 11
#define DISABLE 12
#define FAULT 4
#define ENDSTOP_START A3
#define ENDSTOP_END A2

#define BUTTON_CLOSE A4
#define BUTTON_OPEN A5

//Movement Resolution
#define RESOLUTION 1024

//Direction Configuration
#define FORWARD true
#define BACKWARD false

//Motorspeed Configuration
#define STEPSIZE 4
#define MAXSPEED 2250
#define MINSPEED 25
#define DEFAULTSPEED 500
#define ACCELDECELPORTION 3
#define MAXACCELERATION 50

//Interrupt Configuration
#define INTERRUPT_INTERVAL 40

//Click speeds
#define LONGCLICKTIME 750

//Bridge Config
#define SERIAL_SPEED 115200
#define COMMAND_TARGET "T"
#define COMMAND_STATE "S"
#define COMMAND_PAIR_TARGET "P"
#define COMMAND_ALL_TARGET "A"
#define COMMAND_RESET "RESET"
#define COMMAND_TOTAL_STEPS "STEPS"
#define BROADCAST_INTERVAL 1000
