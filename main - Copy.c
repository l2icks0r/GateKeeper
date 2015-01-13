#include <stdio.h>
#include <string.h>

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_exti.h"
#include "misc.h"
#include "system_stm32f4xx.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rng.h"
#include "core_cm4.h"

#include "codec.h"

#define STARTUP_SOUND
#define CADENCE
#define NUMBERS

#ifdef STARTUP_SOUND
#include "StartupSound.c"
#endif

#include "Square632Hz48k.c"
#include "Square680Hz48k.c"
#include "Square740Hz48k.c"
#include "Square1150Hz48k.c"

#ifdef CADENCE
#include "OkRiders24k.c"
#include "RandomStart24k.c"
#include "RidersReady24k.c"
#include "WatchTheGate24k.c"
#endif

#ifdef NUMBERS
#include "Zero.c"
#include "One.c"
#include "Two.c"
#include "Three.c"
#include "Four.c"
#include "Five.c"
#include "Six.c"
#include "Seven.c"
#include "Eight.c"
#include "Nine.c"
#include "Ten.c"

#include "Eleven.c"
#include "Twelve.c"
#include "Thir.c"
#include "Fif.c"
#include "Teen.c"

#include "Twenty.c"
#include "Thirty.c"
#include "Forty.c"
#include "Fifty.c"
#include "Sixty.c"
#include "Seventy.c"
#include "Eighty.c"
#include "Ninety.c"
#include "Hundred.c"

#include "Percent.c"
#include "Minute.c"
#include "Minutes.c"
#include "Second.c"
#include "Seconds.c"
#include "Point.c"
#include "VolumeAt.c"
#include "BatteryAt.c"
#include "LastTimeWas.c"
#include "MilesPerHour.c"
#include "At.c"
#endif


// play cadence, drive lights, power magnet/solenoid/air ram
void DropGate( void );

// causes the solenoid to be shut off after specified time in milliseconds
void PulseSolenoid( unsigned int pulse_time );
// turns magnet off after delay set in GATE_DROPS_ON
void TurnMagnetOff( void );

// F4 initialization
void InitPorts ( void );
void InitClocks( void );
void InitAudio ( void );

// LCD display
#define DISPLAY_WIDTH 20
#define DISPLAY_HEIGHT 2

static char display_memory		[ DISPLAY_HEIGHT ] [ DISPLAY_WIDTH ];
static char display_memory_cache[ DISPLAY_HEIGHT ] [ DISPLAY_WIDTH ];

void InitLCD		 		( void );
void DefineCustomCharacters	( void );
void WriteLCD_Command		( int data );
void WriteLCD_Data	 		( int data );
void WriteLCD_Line	 		( const char * pString, int line_no );
void WriteLCD_LineCentered	( const char * pString, int line_no );
void UpdateLCD		 		( void );

void PlayDropGateAnimation( void );

// timer (system ticks)
static int Timer_Tick = 0;

void StartTimer6( void );
void ShortDelay ( unsigned int ticks );
void Delay		( unsigned int milliseconds );

// Random number generator (RNG)
static unsigned int GetRandomNumber( void );

// play sample prototypes
void PlaySilence( unsigned int milliseconds, unsigned int dont_abort );
void PlayTone	( unsigned int milliseconds, const int16_t * data, unsigned int length, unsigned int dont_abort );

void PlaySample16khz( const int16_t * data, unsigned int start, unsigned int stop, unsigned int dont_abort );
void PlaySample24khz( const int16_t * data, unsigned int start, unsigned int stop, unsigned int dont_abort );

// Elapsed time: 999:59.999
void PrintElapsedTime( unsigned int milliseconds, unsigned int display_line );

// play speech for number sequence
enum PLAY_TYPE { PLAY_TIME, PLAY_PERCENT };

void PlayTimeOrPercent( unsigned int milliseconds, int play_type );
void PlaySpeed( int integer, int fractional );
void PlayDigit( int number );

// used for energizing the magnet and when the air ram is in action
void PlayGateUpTones( void );

// UI
void InitMenus( void );
void ItemCopy( int menu, int source_index, int dest_index, int no_indicators );
void SetMenuText( char * menu, char * text );
int ReadInputs( int * inputs );
void WaitForButtonUp();

// Audio
void DigiPotWrite( u16 data );

int ReadBatteryCondition( void );


enum INPUTS { 	BUTTON_A 		= 0x0001,
				BUTTON_B 		= 0x0010,
				BUTTON_C 		= 0x0020,
				BUTTON_D 		= 0x0100,

				BUTTON_E 		= 0x8000,

				AUX_1_SENSOR_1	= 0x0080,
				AUX_1_SENSOR_2	= 0x0800,

				AUX_2_SENSOR_1	= 0x1000,
				AUX_2_SENSOR_2	= 0x2000 };

enum MENUS	   { DROP_GATE = 0,
				 ENERGIZE_MAGNET,

				 GATE_START_DELAY,
				 GATE_DROPS_ON,

				 AUX_1_CONFIG, AUX_1_SENSOR_SPACING,
				 AUX_2_CONFIG, AUX_2_SENSOR_SPACING,

				 TIMER_HISTORY,

				 GATE_DROPS,

				 CADENCE_VOLUME,
				 AUDIO_IN_VOLUME,
				 BATTERY_CONDITION,

				 WIRELESS_REMOTE,
				 RELEASE_DEVICE,

				 MENUS_SIZE };

enum AUX_OPTIONS			{ AUX_DISABLED = 1, AUX_GATE_SWITCH = 2, AUX_TIME = 3, AUX_TIME_SPEED = 4, AUX_SENSOR_SPACING = 5 };
enum WIRELESS_REMOTE_OPTIONS{ WIRELESS_REMOTE_DISABLED = 1, WIRELESS_REMOTE_ENABLED = 2 };
enum RELEASE_DEVICE_OPTIONS { RELEASE_DEVICE_SOLENOID = 1,	RELEASE_DEVICE_MAGNET = 2, RELEASE_DEVICE_AIR_RAM = 3 };

enum MENU_TYPE { NO_INPUT, DISPLAY_VALUE, WAIT_FOR_BUTTON, EDIT_VALUE, EDIT_CHOICE, VIEW_LIST };

#define MAX_MENU_ITEMS 10

struct MENU_DEFINITION
{
	int		menu_type;
	int		context;

	char 	caption[ DISPLAY_WIDTH ];

	char	item[ MAX_MENU_ITEMS ][ DISPLAY_WIDTH ];
	int 	item_count;
	int		current_item;
};

//                        1
//              01234567890123456789
#define SPACES "                    "

enum DEVICE_STATE { STARTUP, WAIT_FOR_USER, WAIT_FOR_SENSORS, WAIT_FOR_SPEED_TRAP };


// 192k of RAM, 1000 entries = 78k
#define MAX_TIMING_HISTORIES 1000
// used as a counter for each timing entry for identification - one meeeeeleeeeeeown entries...ha
#define MAX_HISTORY_NUMBER 1000000

struct TIMING_DEFINITION	// should this keep track of what AUX port the measurement came in on? can scroll in main menu for two times
{
	unsigned int number;
	unsigned int timer_ticks;
	unsigned int speed_integer;
	unsigned int speed_fractional;

	short is_a_speed;

	char  time_string		[ DISPLAY_WIDTH ];
	char  speed_string		[ DISPLAY_WIDTH ];
	char  time_speed_string	[ DISPLAY_WIDTH ];
};

// reserve memory to store all menus
static struct MENU_DEFINITION Menu_Array[ MENUS_SIZE ];

// reserve memory to store history of all timings
static struct TIMING_DEFINITION Timer_History[ MAX_TIMING_HISTORIES ];
static int Timer_History_Index  = 0;
static int Timer_History_Number = 0;

// stores the index of the menu currently being processed
static int Device_State = STARTUP;

// variable to handle interrupting when a sample is being played
static int Cadence_Canceled = 0;

// variable so that gate won't drop unless magnet has been energized
static int Magnet_On = 0;

// variables used to communicated from main code to interrupt code
static unsigned int Gate_Drop_Delay		= 0;
static unsigned int Pulse_Solenoid 		= 0;
static unsigned int Solenoid_Pulse_Time = 0;
static unsigned int Turn_Magnet_Off		= 0;

// flag to not display counter when gate start has been canceled
static int Cancel_Timing = 0;

// sensor ticks set by timer 6 and cleared in DropGate()
static unsigned int Aux_1_Sensor_1_Tick = 0;
static unsigned int Aux_1_Sensor_2_Tick = 0;
static unsigned int Aux_2_Sensor_1_Tick = 0;
static unsigned int Aux_2_Sensor_2_Tick = 0;

// configuration for the distance between the two sensors when measuring speed
static unsigned int Aux_1_Sensor_Spacing = 12;
static unsigned int Aux_2_Sensor_Spacing = 12;


// w00t!
int main( void )
{
	SystemInit();  // line 221 of system_stm32f4xx.c

	StartTimer6(); // system ticks

	InitClocks();

	InitPorts();

	InitLCD();

	InitAudio();

	InitMenus();


	// enable gates
	GPIO_ResetBits( GPIOA, GPIO_Pin_3 );	// gate drive enable

	static int inputs	  = 0;
	static int menu_index = 0;

	// called to ignore first result which is false
	ReadInputs( & inputs );
	ReadInputs( & inputs );


	// the one and only main loop
	while ( 1 )
	{
		switch ( Device_State )
		{
			case STARTUP:
			{
#ifdef STARTUP_SOUND
				// initialize attenuator
				DigiPotWrite( 0x41FF );			// write TCON (enable all)
				DigiPotWrite( 0x0038 );			// write pot 0 (pot 0 = aux in audio)
				DigiPotWrite( 0x1000 | 50 );	// write pot 1 (pot 1 = STM32 audio)
				DigiPotWrite( 0x1000 | 50 );	// write pot 1 (pot 1 = STM32 audio)
				GPIO_SetBits( GPIOC, GPIO_Pin_0 );

				Delay( 1300 );

				// write splash text
				WriteLCD_LineCentered( "** RRP BMX GATES **", 0 );
				WriteLCD_LineCentered( "Epicenter Prototype", 1 );
				UpdateLCD();

				PlaySilence( 100, 1 );
				PlaySample24khz( StartupSoundData, 0, STARTUP_SOUND_SIZE, 1 );
				PlaySilence( 100, 1 );
				InitAudio();

				const int volume = (128 * Menu_Array[CADENCE_VOLUME].context) / 100;
				DigiPotWrite( 0x1000 | volume );

				Delay( 1200 );
#endif
				// go to next menu
				Device_State = WAIT_FOR_USER;
				break;
			}

/*
 * Index through each menu without displaying anything other than captions of menus
 * Handle RF remote buttons & gate drop switch plugged into either aux port
 */
			case WAIT_FOR_USER:
			{
				int encoder_delta = 0;

				while( Device_State == WAIT_FOR_USER )
				{
					// write the menu first time through
					WriteLCD_LineCentered( Menu_Array[ menu_index ].caption, 0 );
					WriteLCD_LineCentered( Menu_Array[ menu_index ].item[0], 1 );
					UpdateLCD();

					do
					{	// update battery condition in realtime if battery condition menu is the current menu being displayed
						if( menu_index == BATTERY_CONDITION )
						{
							// TODO: figure out a better way to stop battery level jitter
							float battery_level = 0;
							int a = 0;
							for(; a < 10000; a++ )	battery_level += (float)ReadBatteryCondition() * 100.0f;
							battery_level /= 10000;

							battery_level /= 3600;
							battery_level = ( battery_level > 100 ) ? 100 : battery_level;

							Menu_Array[BATTERY_CONDITION].context = (int) battery_level;

							sprintf( Menu_Array[BATTERY_CONDITION].item[0], "%d%%", Menu_Array[BATTERY_CONDITION].context );

							// write condition to bottom line and update the display to show new condition
							WriteLCD_LineCentered( Menu_Array[ BATTERY_CONDITION ].item[0], 1 );
							UpdateLCD();
						}

						// check for any input being triggered, only returns one result
						encoder_delta = ReadInputs( &inputs );

						// if no input then there is nothing to do
						if( encoder_delta == 0 && inputs == 0 ) continue;

						menu_index += encoder_delta;

						// make sure that index is within the array of menus
						if( menu_index == MENUS_SIZE ) menu_index--;
						else if( menu_index < 0) menu_index = 0;

						// skip ENERGIZE MAGNET menu and RELEASE DEVICE menu if the magnet is on
						if( menu_index == ENERGIZE_MAGNET && (	Menu_Array[RELEASE_DEVICE].context == RELEASE_DEVICE_SOLENOID ||
																Menu_Array[RELEASE_DEVICE].context == RELEASE_DEVICE_AIR_RAM  ||
																Magnet_On == 1 )  )
						{
							if( encoder_delta > 0 ) menu_index = GATE_START_DELAY;
							else					menu_index = DROP_GATE;
						}

						if( menu_index == RELEASE_DEVICE && Magnet_On == 1 ) menu_index = WIRELESS_REMOTE;

						// if the encoder wheel moved then display tne new menu text
						if( encoder_delta != 0 )
						{
							// update the display with the currently selected menu
							WriteLCD_LineCentered( Menu_Array[ menu_index ].caption, 0 );
							WriteLCD_LineCentered( Menu_Array[ menu_index ].item[0], 1 );
							UpdateLCD();
						}

						// if any sensors trip and aux port is configured as a switch then drop the gate
						if( ( Menu_Array[ AUX_1_CONFIG ].context == AUX_GATE_SWITCH && ((inputs == AUX_1_SENSOR_1) || (inputs == AUX_1_SENSOR_2)) ) ||
						    ( Menu_Array[ AUX_2_CONFIG ].context == AUX_GATE_SWITCH && ((inputs == AUX_2_SENSOR_1) || (inputs == AUX_2_SENSOR_2)) ) )
						{
							menu_index = DROP_GATE;
						}
						else
						{	// filter sensor inputs
							if( inputs == AUX_1_SENSOR_1 || inputs == AUX_1_SENSOR_2 || inputs == AUX_2_SENSOR_1 || inputs == AUX_2_SENSOR_2 )
								inputs = 0;
						}

						// skip processing the wireless remote if it is disabled
						if( Menu_Array[WIRELESS_REMOTE].context == WIRELESS_REMOTE_DISABLED )
						{
							if( inputs == BUTTON_A || inputs == BUTTON_B || inputs == BUTTON_C || inputs == BUTTON_D )
								continue;
						}

					} while ( inputs == 0 );


					// process button presses from RF keyfob remote
					if( inputs == BUTTON_A )
					{
						menu_index = DROP_GATE;
					}
					else if( inputs == BUTTON_B )
					{	// say last time was: whatever
#ifdef NUMBERS
						if( Timer_History[ 0 ].timer_ticks != 0 )
						{
							PlaySilence( 100, 1 );
							PlaySample24khz( LastTimeWasData, 0, LAST_TIME_WAS_SIZE, 1 );
							PlayTimeOrPercent( Timer_History[ 0 ].timer_ticks / 10, PLAY_TIME );

							if( Timer_History[ 0 ].is_a_speed )
							{
								PlaySample24khz( AtData, 0, AT_SIZE, 1 );
								PlaySpeed( Timer_History[ 0 ].speed_integer, Timer_History[ 0 ].speed_fractional );
							}
							InitAudio();
						}
#endif
						continue;
					}
					else if( inputs == BUTTON_C )
					{
						// button C has no function unless in magnet mode, BUTTON_C is cancel once cadence starts
						if( Magnet_On == 0 && Menu_Array[RELEASE_DEVICE].context == RELEASE_DEVICE_MAGNET )
						{
							menu_index = ENERGIZE_MAGNET;
						}
						else
							continue;
					}
					else if( inputs == BUTTON_D )
					{
						// cycle through cadence volume levels of 10%, 20%, 40%, 60%, 80%, 100%
						const int volume = Menu_Array[CADENCE_VOLUME].context;

						if( volume < 10 ) Menu_Array[CADENCE_VOLUME].context	  = 10;
						else if( volume < 20 ) Menu_Array[CADENCE_VOLUME].context = 20;
						else if( volume < 40 ) Menu_Array[CADENCE_VOLUME].context = 40;
						else if( volume < 60 ) Menu_Array[CADENCE_VOLUME].context = 60;
						else if( volume < 80 ) Menu_Array[CADENCE_VOLUME].context = 80;
						else Menu_Array[CADENCE_VOLUME].context = 10;

						sprintf( Menu_Array[CADENCE_VOLUME].item[0], "%d%%", Menu_Array[CADENCE_VOLUME].context );

						int cadence_volume = (128 * Menu_Array[CADENCE_VOLUME].context) / 100;

						DigiPotWrite( 0x1000 | cadence_volume );
#ifdef NUMBERS
						PlaySilence( 100, 0 );
						PlaySample24khz( VolumeAtData, 0, VOLUME_AT_SIZE, 1 );
						PlayTimeOrPercent( Menu_Array[CADENCE_VOLUME].context * 60 * 1000, PLAY_PERCENT ); // todo: get rid of the * 60 * 100 in the called function
						InitAudio();
#endif
						break;
					}
					else if( inputs != 0 )	// handle encoder interface
					{
						WaitForButtonUp();
					}

/*
 * Handle driving each menu choice when encoder button is pressed for that menu
 */
					switch ( menu_index )
					{
						case DROP_GATE:
						{
							if( inputs != BUTTON_E && Menu_Array[WIRELESS_REMOTE].context == WIRELESS_REMOTE_DISABLED )
								break;

							if( Menu_Array[RELEASE_DEVICE].context == RELEASE_DEVICE_MAGNET && Magnet_On == 0 )
							{
								WriteLCD_LineCentered( "Magnet is not", 0 );
								WriteLCD_LineCentered( "energized", 1 );
							    UpdateLCD();

							    Delay( 3000 );
							    break;
							}

							if( Menu_Array[RELEASE_DEVICE].context == RELEASE_DEVICE_AIR_RAM )
							{
								WriteLCD_LineCentered( "Air ram is not", 0 );
								WriteLCD_LineCentered( "configured", 1 );
							    UpdateLCD();

							    Delay( 3000 );
							    break;
							}

							WriteLCD_LineCentered( "Gate dropping in", 0 );
							char line[DISPLAY_WIDTH] = SPACES;

							unsigned int end_time = Timer_Tick + Menu_Array[GATE_START_DELAY].context * 10000 + 10000;

							while( Timer_Tick < end_time )
							{
								const int seconds = ( (end_time - Timer_Tick) / 10000 );

								if( seconds == 0 ) break;

								if( seconds > 1 )
									sprintf( line, "%d seconds", seconds );
								else
									sprintf( line, "%d second", seconds );

								if( Menu_Array[GATE_START_DELAY].context > 0 )
								{
									WriteLCD_LineCentered( line, 1 );

									UpdateLCD();
								}

								ReadInputs( &inputs );

								// bypass delay if encoder wheel button
								if( inputs == BUTTON_E  )
									break;
							}

							// clear the display
							WriteLCD_Line(	SPACES, 0 );
							WriteLCD_Line(	SPACES, 1 );
							UpdateLCD();

							// keep track of how many gate drops - should this be an odometer or just reset every time?
							Menu_Array[GATE_DROPS].context += 1;

							// PlayDropGateAnimation(); // TODO: Implement this

							DropGate();

							break;
						}

						case ENERGIZE_MAGNET:
						{
							if( Menu_Array[RELEASE_DEVICE].context == RELEASE_DEVICE_MAGNET )
							{
								WriteLCD_LineCentered( Menu_Array[ENERGIZE_MAGNET].caption, 0 );
								WriteLCD_LineCentered( "magnet is on", 1 );
								UpdateLCD();

								GPIO_SetBits( GPIOA, GPIO_Pin_2 );	// gate 1 ON
								GPIO_SetBits( GPIOA, GPIO_Pin_1 );	// gate 2 ON

								Magnet_On = 1;

								PlayGateUpTones();

								menu_index = DROP_GATE;
							}
							break;
						}

						case GATE_START_DELAY:
						{
							while( 1 )
							{
								if( Menu_Array[GATE_START_DELAY].context == 0 )
								{
									sprintf( Menu_Array[GATE_START_DELAY].item[0], "\1 No Delay \2");
								}
								else if( Menu_Array[GATE_START_DELAY].context < 120 )
								{
									sprintf( Menu_Array[GATE_START_DELAY].item[0], "\1 %d seconds \2", Menu_Array[GATE_START_DELAY].context );
								}
								else
								{
									sprintf( Menu_Array[GATE_START_DELAY].item[0], "\1 %d minutes \2", Menu_Array[GATE_START_DELAY].context / 60 );
								}

								WriteLCD_LineCentered( Menu_Array[GATE_START_DELAY].item[0], 1 );
								UpdateLCD();

								// read controls
								do
								{	encoder_delta = ReadInputs( &inputs );

								} while ( encoder_delta == 0 && inputs == 0 );

								// change menu value
								if( encoder_delta != 0 )
								{
									if( Menu_Array[GATE_START_DELAY].context < 60 )
									{
										Menu_Array[GATE_START_DELAY].context += encoder_delta;
									}
									else if( Menu_Array[GATE_START_DELAY].context == 60 )
									{
										if( encoder_delta < 0 )
										{
											Menu_Array[GATE_START_DELAY].context += encoder_delta;
										}
										else
										{
											Menu_Array[GATE_START_DELAY].context += encoder_delta * 10;
										}
									}
									else if( Menu_Array[GATE_START_DELAY].context == 120 )
									{
										if( encoder_delta < 0 )
										{
											Menu_Array[GATE_START_DELAY].context += encoder_delta * 10;
										}
										else
										{
											Menu_Array[GATE_START_DELAY].context += encoder_delta * 60;
										}
									}
									else if( Menu_Array[GATE_START_DELAY].context >= 120 )
									{
										Menu_Array[GATE_START_DELAY].context += encoder_delta * 60;
									}
									else
									{
										Menu_Array[GATE_START_DELAY].context += encoder_delta * 10;
									}

									if( Menu_Array[GATE_START_DELAY].context < 0 )
										Menu_Array[GATE_START_DELAY].context = 0;

									continue;
								}

								// check for exit
								WaitForButtonUp();

								if( Menu_Array[GATE_START_DELAY].context == 0 )
								{
									sprintf( Menu_Array[GATE_START_DELAY].item[0], "No Delay");
								}
								else if( Menu_Array[GATE_START_DELAY].context < 120 )
								{
									sprintf( Menu_Array[GATE_START_DELAY].item[0], "%d seconds", Menu_Array[GATE_START_DELAY].context );
								}
								else
								{
									sprintf( Menu_Array[GATE_START_DELAY].item[0], "%d minutes", Menu_Array[GATE_START_DELAY].context / 60 );
								}

								WriteLCD_LineCentered( Menu_Array[ GATE_START_DELAY ].item[0], 1 );
								UpdateLCD();

								break;
							}
							break;
						}

						case GATE_DROPS_ON:
						{
							while( 1 )
							{
								unsigned int delay = Menu_Array[GATE_DROPS_ON].context;

								// there is no support for floating point numbers in sprintf, "%f" will not print anything
								// so do some retarded formatting
								if( delay < 120 )
								{
									if( delay == 0 )
										sprintf( Menu_Array[GATE_DROPS_ON].item[0], "\1 Red \2" );
									else
									{
										if( delay < 100 )
											sprintf( Menu_Array[GATE_DROPS_ON].item[0], "\1 Red + 0.0%d \2", delay );
										else
											sprintf( Menu_Array[GATE_DROPS_ON].item[0], "\1 Red + 0.%d \2", delay );
									}

									WriteLCD_LineCentered( Menu_Array[GATE_DROPS_ON].item[0], 1 );
								}
								else if( delay < 240 )
								{
									if( delay == 120 )
										sprintf( Menu_Array[GATE_DROPS_ON].item[0], "\1 1st Yellow \2" );
									else
									{
										if( (delay - 120) < 100 )
											sprintf( Menu_Array[GATE_DROPS_ON].item[0], "*1st Yellow + 0.0%d0", (delay - 120) );
										else
											sprintf( Menu_Array[GATE_DROPS_ON].item[0], "*1st Yellow + 0.%d*", (delay - 120) );
									}

									Menu_Array[GATE_DROPS_ON].item[0][0] = '\1';
									Menu_Array[GATE_DROPS_ON].item[0][ strlen( Menu_Array[GATE_DROPS_ON].item[0] ) - 1 ] = '\2';
								}
								else if( delay < 360 )
								{
									if( delay == 240 )
										sprintf( Menu_Array[GATE_DROPS_ON].item[0], "\1 2nd Yellow \2" );
									else
									{
										if( (delay - 240) < 100 )
											sprintf( Menu_Array[GATE_DROPS_ON].item[0], "*2nd Yellow + 0.0%d0", (delay - 240) );
										else
											sprintf( Menu_Array[GATE_DROPS_ON].item[0], "*2nd Yellow + 0.%d*", (delay - 240) );
									}

									Menu_Array[GATE_DROPS_ON].item[0][0] = '\1';
									Menu_Array[GATE_DROPS_ON].item[0][ strlen( Menu_Array[GATE_DROPS_ON].item[0] ) - 1 ] = '\2';
								}
								else
								{
									sprintf( Menu_Array[GATE_DROPS_ON].item[0], "\1 Green \2" );
								}

								WriteLCD_LineCentered( Menu_Array[GATE_DROPS_ON].item[0], 1 );
								UpdateLCD();

								// read controls
								do
								{	encoder_delta = ReadInputs( &inputs );

								} while ( encoder_delta == 0 && inputs == 0 );

								// change menu value
								if( encoder_delta != 0 )
								{
									Menu_Array[GATE_DROPS_ON].context += encoder_delta * 10;

									if( Menu_Array[GATE_DROPS_ON].context < 0 )
										Menu_Array[GATE_DROPS_ON].context = 0;

									else if( Menu_Array[GATE_DROPS_ON].context > 360 )
										Menu_Array[GATE_DROPS_ON].context = 360;

									WriteLCD_LineCentered( Menu_Array[GATE_DROPS_ON].item[0], 1 );
									UpdateLCD();
									continue;
								}

								// check for exit
								WaitForButtonUp();

								// strip indicators
								int i = 0;
								for(; i < strlen(Menu_Array[ GATE_DROPS_ON ].item[0]); i++)
								{
									if( Menu_Array[ GATE_DROPS_ON ].item[0][i] == '\1' || Menu_Array[ GATE_DROPS_ON ].item[0][i] == '\2' )
											Menu_Array[ GATE_DROPS_ON ].item[0][i] = 0x20;
								}
								WriteLCD_LineCentered( Menu_Array[ GATE_DROPS_ON ].item[0], 1 );
								UpdateLCD();

								break;
							}
							break;
						}

						case AUX_1_CONFIG:
						{
							while( 1 )
							{
								WriteLCD_LineCentered( Menu_Array[AUX_1_CONFIG].item[ Menu_Array[AUX_1_CONFIG].context ], 1 );
								UpdateLCD();

								// read controls
								do
								{	encoder_delta = ReadInputs( &inputs );

								} while (encoder_delta == 0 && inputs == 0 );

								// change menu value
								if( encoder_delta != 0 )
								{
									const int new_item = Menu_Array[AUX_1_CONFIG].current_item + encoder_delta;

									if( new_item > Menu_Array[AUX_1_CONFIG].item_count ) continue;
									if( new_item < 1 ) continue;

									Menu_Array[AUX_1_CONFIG].current_item = new_item;

									switch( new_item )
									{
										case 1: { Menu_Array[AUX_1_CONFIG].context = AUX_DISABLED; 		break;	}
										case 2: { Menu_Array[AUX_1_CONFIG].context = AUX_GATE_SWITCH;	break;	}
										case 3: { Menu_Array[AUX_1_CONFIG].context = AUX_TIME;			break;	}
										case 4: { Menu_Array[AUX_1_CONFIG].context = AUX_TIME_SPEED;	break;	}
										case 5: { Menu_Array[AUX_1_CONFIG].context = AUX_SENSOR_SPACING;break;	}
									}

									continue;
								}

								// check for exit
								WaitForButtonUp();

								ItemCopy( AUX_1_CONFIG, Menu_Array[AUX_1_CONFIG].context, 0, 1 );
								WriteLCD_LineCentered( Menu_Array[AUX_1_CONFIG].item[0], 1 );
								UpdateLCD();

								break;
							}
							break;
						}

						case AUX_2_CONFIG:
						{
							while( 1 )
							{
								WriteLCD_LineCentered( Menu_Array[AUX_2_CONFIG].item[ Menu_Array[AUX_2_CONFIG].context ], 1 );
								UpdateLCD();

								// read controls
								do
								{	encoder_delta = ReadInputs( &inputs );

								} while (encoder_delta == 0 && inputs == 0 );


								// change menu value
								if( encoder_delta != 0 )
								{
									const int new_item = Menu_Array[AUX_2_CONFIG].current_item + encoder_delta;

									if( new_item > Menu_Array[AUX_2_CONFIG].item_count ) continue;
									if( new_item < 1 ) continue;

									Menu_Array[AUX_2_CONFIG].current_item = new_item;

									switch( new_item )
									{
										case 1: { Menu_Array[AUX_2_CONFIG].context = AUX_DISABLED; 		break;	}
										case 2: { Menu_Array[AUX_2_CONFIG].context = AUX_GATE_SWITCH;	break;	}
										case 3: { Menu_Array[AUX_2_CONFIG].context = AUX_TIME;			break;	}
										case 4: { Menu_Array[AUX_2_CONFIG].context = AUX_TIME_SPEED;	break;	}
										case 5: { Menu_Array[AUX_2_CONFIG].context = AUX_SENSOR_SPACING;break;	}
									}

									continue;
								}

								// check for exit
								WaitForButtonUp();

								ItemCopy( AUX_2_CONFIG, Menu_Array[AUX_2_CONFIG].context, 0, 1 );
								WriteLCD_LineCentered( Menu_Array[AUX_2_CONFIG].item[0], 1 );
								UpdateLCD();

								break;
							}
							break;
						}

						case AUX_1_SENSOR_SPACING:
						case AUX_2_SENSOR_SPACING:
						{
							while( 1 )
							{
								// add cursor
								sprintf( Menu_Array[menu_index].item[0], "\1 %d inches \2", Menu_Array[menu_index].context );
								WriteLCD_LineCentered( Menu_Array[ menu_index ].item[0], 1 );
								UpdateLCD();

								// read controls
								do
								{	encoder_delta = ReadInputs( &inputs );

								} while ( encoder_delta == 0 && inputs == 0 );

								// change menu value
								if( encoder_delta != 0 )
								{
									int new_value = Menu_Array[menu_index].context + encoder_delta;

									if( new_value >= 0 && new_value <= 120 )	// 120"
										Menu_Array[menu_index].context = new_value;

									continue;
								}

								// check for exit
								WaitForButtonUp();

								// remove cursor
								sprintf( Menu_Array[menu_index].item[0], "%d inches", Menu_Array[menu_index].context );
								WriteLCD_LineCentered( Menu_Array[menu_index].item[0], 1 );
								UpdateLCD();

								break;
							}
							break;
						}

						case TIMER_HISTORY:
						{
							if( Timer_History_Index == 0 )
							{
								SetMenuText( Menu_Array[TIMER_HISTORY].item[0], "No history" );
								WriteLCD_LineCentered( "No history", 1 );
								UpdateLCD();
								break;
							}

							while( 1 )
							{
								char line_0[ DISPLAY_WIDTH ] = SPACES;
								char line_1[ DISPLAY_WIDTH ] = SPACES;

								unsigned int i, j;

								char number[ 6 ] = "      ";
								char time[ 10 ]	 = "          ";

								const int top_index		= Menu_Array[TIMER_HISTORY].context;
								const int bottom_index	= Menu_Array[TIMER_HISTORY].context + 1;

								// write time # to top left justified
								sprintf( number, "#%d", Timer_History[ top_index ].number );

								for( i = 0; i < strlen( number ); i++ ) line_0[ i ] = number[ i ];

								// TIME on top, TIME on bottom from TIME and SPEED element.
								if( !Timer_History[ top_index ].is_a_speed && Timer_History[ bottom_index ].is_a_speed )
								{
									// write time to top line right justified
									sprintf( time, "%s", Timer_History[ top_index ].time_string );
									for( i = 0, j = DISPLAY_WIDTH - strlen( time ) - 2; j < DISPLAY_WIDTH; j++, i++ ) line_0[ j ] = time[ i ];
									line_0[ DISPLAY_WIDTH - 2 ] = 0x20;

									// write time # to bottom line left justified
									sprintf( number, "#%d", Timer_History[ bottom_index ].number );
									for( i = 0; i < strlen( number ); i++ ) line_1[ i ] = number[ i ];

									// write time to bottom line right justified
									sprintf( time, "%s", Timer_History[ bottom_index ].time_string );
									for( i = 0, j = DISPLAY_WIDTH - strlen( time ) - 2; j < DISPLAY_WIDTH; j++, i++ ) line_1[ j ] = time[ i ];
									line_1[ DISPLAY_WIDTH - 2 ] = 0x20;
								}
								// TIME on top, TIME on bottom
								else if( !Timer_History[ top_index ].is_a_speed && !Timer_History[ bottom_index ].is_a_speed )
								{
									// write time to top line right justified
									sprintf( time, "%s", Timer_History[ top_index ].time_string );
									for( i = 0, j = DISPLAY_WIDTH - strlen( time ) - 2; j < DISPLAY_WIDTH; j++, i++ ) line_0[ j ] = time[ i ];
									line_0[ DISPLAY_WIDTH - 2 ] = 0x20;

									// if there is a time in the next element of the history array then write it since we have room
									if( Timer_History[ bottom_index ].number != 0 )
									{
										// write time # to bottom line left justified
										sprintf( number, "#%d", Timer_History[ bottom_index ].number );
										for( i = 0; i < strlen( number ); i++ ) line_1[ i ] = number[ i ];

										// write time to bottom line right justified
										sprintf( time, "%s", Timer_History[ bottom_index ].time_string );
										for( i = 0, j = DISPLAY_WIDTH - strlen( time ) - 2; j < DISPLAY_WIDTH; j++, i++ ) line_1[ j ] = time[ i ];
										line_1[ DISPLAY_WIDTH - 2 ] = 0x20;
									}
									else
									{	// clear out the bottom line of the display since there is no value to be displayed
										for( i = 0; i < DISPLAY_WIDTH; i++ ) line_1[ i ] = 0x20;
									}
								}
								// TIME on top, SPEED on bottom
								else if( Timer_History[ top_index ].is_a_speed )
								{
									// write time to top line right justified
									sprintf( time, "%s", Timer_History[ top_index ].time_string );
									for( i = 0, j = DISPLAY_WIDTH - strlen( time ) - 2; j < DISPLAY_WIDTH; j++, i++ ) line_0[ j ] = time[ i ];
									line_0[ DISPLAY_WIDTH - 2 ] = 0x20;

									// write Speed: left justified
									sprintf( line_1, "Speed:" );
									line_1[ 6 ] = 0x20;	// get rid of null termination that sprintf put on the end

									// write the speed right justified
									j = DISPLAY_WIDTH - 2 - strlen( Timer_History[ top_index ].speed_string );
									sprintf( &line_1[ j ], "%s", Timer_History[ top_index ].speed_string );
									line_1[ DISPLAY_WIDTH - 2 ] = 0x20;
								}
								// TIME on top, 0 on bottom
								else
								{
									// write time right justified
									sprintf( time, "%s", Timer_History[ top_index ].time_string );
									for( i = 0, j = DISPLAY_WIDTH - strlen( time ) - 2; j < DISPLAY_WIDTH; j++, i++ ) line_0[ j ] = time[ i ];
									line_0[ DISPLAY_WIDTH - 2 ] = 0x20;

									// nothing to right on bottom line so clear it out
									for( i = 0; i < DISPLAY_WIDTH; i++ ) line_1[ i ] = 0x20;
								}

								// determine what custom characters for up/down arrows should be to the left of the histories and write them
								if( Timer_History_Index != 1 )
								{
									if( Timer_History_Index > 3 && Timer_History[ top_index - 1 ].number != 0 && Timer_History[ bottom_index + 1 ].number != 0 )
									{	// write up & down arrow to right
										line_0[ DISPLAY_WIDTH - 1 ] = '\3';
										line_1[ DISPLAY_WIDTH - 1 ] = '\4';
									}
									else if( Timer_History_Index == 2 && top_index == 0 && Timer_History[ 0 ].is_a_speed )
									{	// write down arrow on bottom line
										line_1[ DISPLAY_WIDTH - 1 ] = '\4';
									}
									else if( (top_index == Timer_History_Index || bottom_index == Timer_History_Index) && Timer_History[ top_index ].is_a_speed )
									{	// write up arrow on top line
										line_0[ DISPLAY_WIDTH - 1 ] = '\3';
									}
									else if( Timer_History_Index >= 3 && top_index >= 1 && Timer_History[ top_index ].is_a_speed )
									{	// write up & down arrow to right - this is a different condition and can't be combined with first up & down arrow case
										line_0[ DISPLAY_WIDTH - 1 ] = '\3';
										line_1[ DISPLAY_WIDTH - 1 ] = '\4';
									}
									else if( top_index == 0 && Timer_History[ bottom_index ].number != 0 && Timer_History_Index != 2 )
									{	// write down arrow on bottom line - this is a different condition and can't be combined with first down arrow case
										line_1[ DISPLAY_WIDTH - 1 ] = '\4';
									}
									else if( !(top_index == 0 && bottom_index == 1) )
									{
										if( (bottom_index > top_index && Timer_History[ top_index - 1 ].number != 0) ||
											(Timer_History_Index - 1 == top_index && Timer_History[ Timer_History_Index - 2 ].number != 0) )
										{	// write up arrow on top line
											line_0[ DISPLAY_WIDTH - 1 ] = '\3';
										}
										else if( top_index == 0 && Timer_History[ 1 ].number != 0 )
										{	// write down arrow on bottom line
											line_1[ DISPLAY_WIDTH - 1 ] = '\4';
										}
									}
								}

								WriteLCD_Line( line_0, 0 );
								WriteLCD_Line( line_1, 1 );
								UpdateLCD();

								// wait for input
								do
								{	encoder_delta = ReadInputs( &inputs );

								} while (encoder_delta == 0 && inputs == 0 );

								// change menu value
								if( encoder_delta != 0 )
								{
									int new_value = Menu_Array[TIMER_HISTORY].context - encoder_delta;

									if( new_value >= 0 && new_value < Timer_History_Index )
									{
										Menu_Array[TIMER_HISTORY].context = new_value;
									}

									WriteLCD_Line( line_0, 0 );
									WriteLCD_Line( line_1, 1 );
									UpdateLCD();

									continue;
								}

								// check for exit
								WaitForButtonUp();

								break;
							}
							break;
						}

						case CADENCE_VOLUME:
						{
							while( 1 )
							{
								// display with cursor
								sprintf( Menu_Array[CADENCE_VOLUME].item[0], "\1 %d%% \2", Menu_Array[CADENCE_VOLUME].context );
								WriteLCD_LineCentered( Menu_Array[ menu_index ].item[0], 1 );
								UpdateLCD();

								// read controls
								do
								{	encoder_delta = ReadInputs( &inputs );

								} while (encoder_delta == 0 && inputs == 0 );

								// change menu value
								if( encoder_delta != 0 )
								{
									int new_value = Menu_Array[CADENCE_VOLUME].context += encoder_delta;

									if( new_value > 100 ) new_value = 100;
									if( new_value < 0 ) new_value = 0;

									Menu_Array[CADENCE_VOLUME].context = new_value;

									int volume = (128 * Menu_Array[CADENCE_VOLUME].context) / 100;

									DigiPotWrite( 0x1000 | volume );
									continue;
								}

								// check for exit
								WaitForButtonUp();

								// remove cursor and exit
								sprintf( Menu_Array[CADENCE_VOLUME].item[0], "%d%%", Menu_Array[CADENCE_VOLUME].context );
								WriteLCD_LineCentered( Menu_Array[ menu_index ].item[0], 1 );
								UpdateLCD();

								break;
							}

#ifdef NUMBERS
							PlaySilence( 100, 0 );
							PlaySample24khz( VolumeAtData, 0, VOLUME_AT_SIZE, 1 );
							PlayTimeOrPercent( Menu_Array[CADENCE_VOLUME].context * 60 * 1000, PLAY_PERCENT ); // todo: get rid of the * 60 * 1000 in the called function
							InitAudio();
#endif
							break;
						}

						case AUDIO_IN_VOLUME:
						{
							while( 1 )
							{
								// display with cursor
								sprintf( Menu_Array[AUDIO_IN_VOLUME].item[0], "\1 %d%% \2", Menu_Array[AUDIO_IN_VOLUME].context );
								WriteLCD_LineCentered( Menu_Array[ menu_index ].item[0], 1 );
								UpdateLCD();

								// read controls
								do
								{	encoder_delta = ReadInputs( &inputs );

								} while (encoder_delta == 0 && inputs == 0 );

								// change menu value
								if( encoder_delta != 0 )
								{
									int new_value = Menu_Array[AUDIO_IN_VOLUME].context += encoder_delta;

									if( new_value > 100 ) new_value = 100;
									if( new_value < 0 ) new_value = 0;

									Menu_Array[AUDIO_IN_VOLUME].context = new_value;

									int volume = (128 * Menu_Array[AUDIO_IN_VOLUME].context) / 100;

									DigiPotWrite( 0x0000 | volume );

									continue;
								}

								// check for exit
								WaitForButtonUp();

								// remove cursor and exit
								sprintf( Menu_Array[AUDIO_IN_VOLUME].item[0], "%d%%", Menu_Array[AUDIO_IN_VOLUME].context );
								WriteLCD_LineCentered( Menu_Array[ menu_index ].item[0], 1 );
								UpdateLCD();

								break;
							}

							break;
						}

						case WIRELESS_REMOTE:
						{
							while( 1 )
							{
								WriteLCD_LineCentered( Menu_Array[WIRELESS_REMOTE].item[ Menu_Array[WIRELESS_REMOTE].context ], 1 );
								UpdateLCD();

								// wait for sensor to trip
								do
								{	encoder_delta = ReadInputs( &inputs );

								} while (encoder_delta == 0 && inputs == 0 );

								// change menu value
								if( encoder_delta != 0 )
								{
									int new_value = Menu_Array[WIRELESS_REMOTE].context + encoder_delta;

									if( new_value >= 1 && new_value <= 2 )
										Menu_Array[WIRELESS_REMOTE].context = new_value;

									continue;
								}

								// check for exit
								WaitForButtonUp();

								ItemCopy( WIRELESS_REMOTE, Menu_Array[ WIRELESS_REMOTE ].context, 0, 1 );
								WriteLCD_LineCentered( Menu_Array[ WIRELESS_REMOTE ].item[0], 1 );
								UpdateLCD();

								break;
							}
							break;

						}

						case BATTERY_CONDITION:
						{
#ifdef NUMBERS
							PlaySilence( 100, 0 );
							PlaySample24khz( BatteryAtData, 0, BATTERY_AT_SIZE, 1 );
							PlayTimeOrPercent( Menu_Array[BATTERY_CONDITION].context * 60 * 1000, PLAY_PERCENT ); // todo: get rid of the * 60 * 1000 in the called function
							InitAudio();
#endif
							break;
						}

						case GATE_DROPS:
						{
							sprintf( Menu_Array[GATE_DROPS].item[0], "%d", Menu_Array[GATE_DROPS].context );

							WriteLCD_LineCentered( Menu_Array[ GATE_DROPS ].item[0], 1 );
							UpdateLCD();

							break;
						}

						case RELEASE_DEVICE:
						{
							while( 1 )
							{
								WriteLCD_LineCentered( Menu_Array[RELEASE_DEVICE].item[ Menu_Array[RELEASE_DEVICE].context ], 1 );
								UpdateLCD();

								// read controls
								do
								{	encoder_delta = ReadInputs( &inputs );

								} while (encoder_delta == 0 && inputs == 0 );


								// change menu value
								if( encoder_delta != 0 )
								{
									const int new_item = Menu_Array[RELEASE_DEVICE].current_item + encoder_delta;

									if( new_item > Menu_Array[RELEASE_DEVICE].item_count ) continue;
									if( new_item < 1 ) continue;

									Menu_Array[RELEASE_DEVICE].current_item = new_item;

									switch( new_item )
									{
										case 1: { Menu_Array[RELEASE_DEVICE].context = RELEASE_DEVICE_SOLENOID;	break;	}
										case 2: { Menu_Array[RELEASE_DEVICE].context = RELEASE_DEVICE_MAGNET;	break;	}
										case 3: { Menu_Array[RELEASE_DEVICE].context = RELEASE_DEVICE_AIR_RAM;	break;	}
									}

									continue;
								}

								// check for exit
								WaitForButtonUp();

								ItemCopy( RELEASE_DEVICE, Menu_Array[RELEASE_DEVICE].context, 0, 1 );
								WriteLCD_LineCentered( Menu_Array[RELEASE_DEVICE].item[0], 1 );
								UpdateLCD();

								break;
							}
							break;
						}
					}
				}
				break;
			}
/*
 * Block until sensors trip or encoder button is pressed to abort
 */
			case WAIT_FOR_SENSORS:
			{
				if( Cancel_Timing == 1 )
				{
					Device_State = WAIT_FOR_USER;
					break;
				}

				if( Timer_Trigger_Value == 0 )
				{
					WriteLCD_LineCentered( "TIMING", 0 );
				}

				if( Menu_Array[AUX_1_CONFIG].context == AUX_TIME_SPEED )
				{
					unsigned int first_time = 0;
					unsigned int final_time = 0;

					while( inputs != AUX_1_SENSOR_1 )	// AUX_1_SENSOR_1
					{
						if( Timer_Trigger_Value == 0 )
						{
							first_time = Timer_Tick;
							PrintElapsedTime( first_time / 10, 1 );
							UpdateLCD();
						}
						else
						{
							first_time = Timer_Trigger_Value;
						}

						ReadInputs( &inputs );
					}

					while( inputs != AUX_1_SENSOR_2 ) // AUX_1_SENSOR_2
					{
						final_time = Timer_Tick;

						ReadInputs( &inputs );

						if( inputs == AUX_1_SENSOR_2 ) // AUX_1_SENSOR_2
						{
							const float speed = (10000.0f * (25.4f * (float)Menu_Array[AUX_1_SENSOR_SPACING].context) ) / (304.8f * (float)(final_time - first_time));

							WriteLCD_LineCentered( "ELAPSED TIME", 0 );
							PrintElapsedTime( final_time / 10, 1 );
							UpdateLCD();

							// say time
							PlaySilence( 100, 0 );
							PlayTimeOrPercent( final_time / 10, PLAY_TIME );
							PlaySilence( 1, 0 );
							InitAudio();

							Delay( 2000 );

							const int minutes     = (final_time / 10 / 60000 );
							const int seconds     = (final_time / 10 / 1000	) % 60;
							const int tens_place  = (final_time / 10 / 100	) % 10;
							const int hund_place  = (final_time / 10 / 10 	) % 10;
							const int thou_place  = (final_time / 10 / 1	) % 10;

							char line[DISPLAY_WIDTH] = SPACES;

							if( minutes > 0 )
								sprintf( line, "%d:%d.%d%d%d", minutes, seconds, tens_place, hund_place, thou_place );
							else
								sprintf( line, "%d.%d%d%d", seconds, tens_place, hund_place, thou_place );

							// write speed
							char speed_line[DISPLAY_WIDTH] = SPACES;
							unsigned int speed_integer 	  = (int)speed;
							unsigned int speed_fractional = (int)(speed * 1000.0f) % 1000;

							sprintf( speed_line, "%d.%03d mph", speed_integer, speed_fractional );

							WriteLCD_LineCentered( "SPEED", 0 );
							WriteLCD_LineCentered( speed_line, 1 );
							UpdateLCD();

							PlaySpeed( speed_integer, speed_fractional );

							Delay( 2000 );

							// only store up to MAX_TIMING_HISTORIES
							// just scroll entire array down before each add and add new time at position 0 in the Timer_History array
							int j = 0, k;
							for( j = MAX_TIMING_HISTORIES - 1; j >= 0 ; j-- )
							{
								Timer_History[ j ].number			= Timer_History[ j - 1 ].number;
								Timer_History[ j ].timer_ticks		= Timer_History[ j - 1 ].timer_ticks;
								Timer_History[ j ].speed_integer	= Timer_History[ j - 1 ].speed_integer;
								Timer_History[ j ].speed_fractional	= Timer_History[ j - 1 ].speed_fractional;

								Timer_History[ j ].is_a_speed		= Timer_History[ j - 1 ].is_a_speed;

								for( k = 0; k < DISPLAY_WIDTH; k++ )
								{
									Timer_History[ j ].time_string[ k ]		  = Timer_History[ j - 1 ].time_string[ k ];
									Timer_History[ j ].speed_string[ k ]	  = Timer_History[ j - 1 ].speed_string[ k ];
									Timer_History[ j ].time_speed_string[ k ] = Timer_History[ j - 1 ].time_speed_string[ k ];
								}
							}

							// used by TIMER_HISTORY to know how many entries to display
							Timer_History_Index++;
							Timer_History_Index = ( Timer_History_Index > MAX_TIMING_HISTORIES ) ? MAX_TIMING_HISTORIES : Timer_History_Index;

							// reset history index at 10000000 (like anyone would store that many - EVER)
							Timer_History_Number++;
							Timer_History_Number = ( Timer_History_Number > MAX_HISTORY_NUMBER ) ? 1 : Timer_History_Number;

							// save this timer result
							Timer_History[ 0 ].number			= Timer_History_Number;
							Timer_History[ 0 ].timer_ticks		= final_time;
							Timer_History[ 0 ].speed_integer	= speed_integer;
							Timer_History[ 0 ].speed_fractional	= speed_fractional;

							Timer_History[ 0 ].is_a_speed		= ((speed_integer != 0) || (speed_fractional != 0));

							// copy timer strings to timer history
							int i;
							for( i = 0; i < DISPLAY_WIDTH; i++ ) Timer_History[ 0 ].time_string[ i ] = line[ i ];
							for( i = 0; i < DISPLAY_WIDTH; i++ ) Timer_History[ 0 ].speed_string[ i ] = speed_line[ i ];

							if( minutes < 1 )
							{
								sprintf( Timer_History[ 0 ].time_speed_string, "%d.%d%d%d @ %s", seconds, tens_place, hund_place, thou_place, speed_line );
							}
							else if( (minutes >  0 && minutes <  9 && seconds < 10 && speed > 9 ) ||
									 (minutes < 10 && seconds < 10 && speed > 9) ||
									 (minutes < 10 && seconds >  9 && speed < 10) ||
									 (minutes >  9 && seconds < 10 && speed < 10) )
							{
								sprintf( Timer_History[ 0 ].time_speed_string, "%d:%d.%d%d%d @ %s", minutes, seconds, tens_place, hund_place, thou_place, speed_line );
							}
							else if( (minutes < 10 && seconds > 9 && speed > 9) ||
									 (minutes >  9 && seconds > 9 && speed < 9)  ||
									 (minutes >  0 && minutes < 9 && seconds > 9 && speed > 10) )
							{
								sprintf( Timer_History[ 0 ].time_speed_string, "%d:%d.%d%d%d @%s", minutes, seconds, tens_place, hund_place, thou_place, speed_line );
							}
							else {
								sprintf( Timer_History[ 0 ].time_speed_string, "%d:%d.%d%d%d@%s", minutes, seconds, tens_place, hund_place, thou_place, speed_line );
							}

							SetMenuText( Menu_Array[TIMER_HISTORY].item[0], Timer_History[ 0 ].time_speed_string );
							WriteLCD_LineCentered( "TIME & SPEED", 0 );
							WriteLCD_LineCentered( Timer_History[ 0 ].time_speed_string, 1 );
							UpdateLCD();

							Delay( 3000 );

							break;
						}
						else if( inputs == BUTTON_E || ((inputs == BUTTON_A) && (Menu_Array[WIRELESS_REMOTE].context == WIRELESS_REMOTE_ENABLED))  )
						{
							WriteLCD_LineCentered( "TIMING CANCELED!", 0 );
							WriteLCD_LineCentered( "TIMING CANCELED!", 1 );
							UpdateLCD();

							Delay( 1 * 1000 );

							break;
						}
					}
				}
				else
				{
					while( inputs != AUX_1_SENSOR_1 || inputs != AUX_1_SENSOR_2 )
					{
						// Timer_Tick was reset to 0 in DropGate
						unsigned int final_time = 0;

						if( Timer_Trigger_Value == 0 )
						{
							final_time = Timer_Tick / 10;
							PrintElapsedTime( final_time, 1 );
							UpdateLCD();
						}
						else
						{
							final_time = Timer_Trigger_Value / 10;
						}

						ReadInputs( &inputs );

						if( inputs == AUX_1_SENSOR_1 || inputs == AUX_1_SENSOR_2 || Timer_Trigger_Value != 0 )
						{
							WriteLCD_LineCentered( "ELAPSED TIME", 0 );
							PrintElapsedTime( final_time, 1 );
							UpdateLCD();

							PlaySilence( 100, 0 );
							PlayTimeOrPercent( final_time, PLAY_TIME );
							PlaySilence( 1, 0 );
							InitAudio();

							// add to timer history
							const int minutes     = (final_time / 60000 );
							const int seconds     = (final_time / 1000	) % 60;
							const int tens_place  = (final_time / 100	) % 10;
							const int hund_place  = (final_time / 10 	) % 10;
							const int thou_place  = (final_time / 1		) % 10;

							char line[DISPLAY_WIDTH] = SPACES;

							if( minutes > 0 )
								sprintf( line, "%d:%d.%d%d%d", minutes, seconds, tens_place, hund_place, thou_place );
							else
								sprintf( line, "%d.%d%d%d", seconds, tens_place, hund_place, thou_place );

							// only store up to MAX_TIMING_HISTORIES
							// just scroll entire array down before each add and add new time at position 0 in the Timer_History array
							int j = 0, k;
							for( j = MAX_TIMING_HISTORIES - 1; j >= 0 ; j-- )
							{
								Timer_History[ j ].number			= Timer_History[ j - 1 ].number;
								Timer_History[ j ].timer_ticks		= Timer_History[ j - 1 ].timer_ticks;
								Timer_History[ j ].speed_integer	= Timer_History[ j - 1 ].speed_integer;
								Timer_History[ j ].speed_fractional	= Timer_History[ j - 1 ].speed_fractional;

								Timer_History[ j ].is_a_speed		= Timer_History[ j - 1 ].is_a_speed;

								for( k = 0; k < DISPLAY_WIDTH; k++ )
								{
									Timer_History[ j ].time_string[ k ]		  = Timer_History[ j - 1 ].time_string[ k ];
									Timer_History[ j ].speed_string[ k ]	  = Timer_History[ j - 1 ].speed_string[ k ];
									Timer_History[ j ].time_speed_string[ k ] = Timer_History[ j - 1 ].time_speed_string[ k ];
								}
							}

							// used by TIMER_HISTORY to know how many entries to display
							Timer_History_Index++;
							Timer_History_Index = ( Timer_History_Index > MAX_TIMING_HISTORIES ) ? MAX_TIMING_HISTORIES : Timer_History_Index;

							// reset history index at 100000 (like anyone would store that many - EVER)
							Timer_History_Number++;
							Timer_History_Number = ( Timer_History_Number > MAX_HISTORY_NUMBER ) ? 1 : Timer_History_Number;

							// save this timer result
							Timer_History[ 0 ].number			= Timer_History_Number;
							Timer_History[ 0 ].timer_ticks		= final_time * 10;
							Timer_History[ 0 ].speed_integer	= 0;
							Timer_History[ 0 ].speed_fractional	= 0;

							// copy timer strings to timer history
							int i;
							for( i = 0; i < DISPLAY_WIDTH; i++ ) Timer_History[ 0 ].time_string[ i ] = line[ i ];

							SetMenuText( Menu_Array[TIMER_HISTORY].item[0], Timer_History[ 0 ].time_string );
							WriteLCD_LineCentered( "ELAPSED TIME", 0 );
							WriteLCD_LineCentered( Timer_History[ 0 ].time_string, 1 );
							UpdateLCD();

							Delay( 2000 );
							break;
						}
						else if( inputs == BUTTON_E || ((inputs == BUTTON_A) && (Menu_Array[WIRELESS_REMOTE].context == WIRELESS_REMOTE_ENABLED)) )
						{
							WriteLCD_LineCentered( "TIMING CANCELED!", 0 );
							WriteLCD_LineCentered( "TIMING CANCELED!", 1 );
							UpdateLCD();

							Delay( 1 * 1000 );

							break;
						}
					}
				}

				// go to main menu
				Device_State = WAIT_FOR_USER;
				break;
			}
		}
	}
}

void DropGate( void )
{
	// fade music volume out TODO: make this a logarithmic!
	int volume = (128 * Menu_Array[AUDIO_IN_VOLUME].context) / 100;
	int step = volume / 10;

	for( ; volume >= 0; volume -= step )
	{
		DigiPotWrite( 0x0000 | volume );
		Delay(120);
	}

	DigiPotWrite( 0x0000 );

	Cadence_Canceled = 0;
	Cancel_Timing	 = 0;

	WriteLCD_LineCentered( "PLAYING CADENCE!", 0 );
	WriteLCD_Line(	SPACES, 1 );
    UpdateLCD();

    PlaySilence( 100, 0 );

#ifdef CADENCE
	// start playing the voice part of the cadence
	PlaySample24khz( OkRidersData,		0, OK_RIDERS_SIZE,		0 );
	PlaySilence( 149, 0 );

	PlaySample24khz( RandomStartData,	0, RANDOM_START_SIZE,	0 );
	PlaySilence( 1810, 0 );

	PlaySample24khz( RidersReadyData,	0, RIDERS_READY_SIZE,	0 );
	PlaySilence( 179, 0 );

	PlaySample24khz( WatchTheGateData,	0, WATCH_THE_GATE_SIZE,	0 );
#endif

	// wait 0.10 to 2.7 seconds
	PlaySilence( GetRandomNumber() % 2600 + 100, 0 );

	if( Cadence_Canceled == 0 )
	{
		if( Menu_Array[RELEASE_DEVICE].context == RELEASE_DEVICE_SOLENOID )
		{
			PulseSolenoid( 800 );
		}
		else if( Menu_Array[RELEASE_DEVICE].context == RELEASE_DEVICE_MAGNET )
		{
			TurnMagnetOff();
		}
		else if( Menu_Array[RELEASE_DEVICE].context == RELEASE_DEVICE_AIR_RAM )
		{
			// TODO: implement this
		}
	}

	// play the cadence tones and light the lights
	if( Cadence_Canceled == 0 ) GPIO_SetBits( GPIOD, GPIO_Pin_12 );	// RED light ON
	PlayTone( 60, Square632HzData, SQUARE_632HZ_SIZE, 0 );
	PlaySilence( 60, 0 );

	if( Cadence_Canceled == 0 ) GPIO_SetBits( GPIOD, GPIO_Pin_13 );	// AMBER 1 light ON
	PlayTone( 60, Square632HzData, SQUARE_632HZ_SIZE, 0 );
	PlaySilence( 60, 0 );

	if( Cadence_Canceled == 0 ) GPIO_SetBits( GPIOD, GPIO_Pin_14 );	// AMBER 2 light ON
	PlayTone( 60, Square632HzData, SQUARE_632HZ_SIZE, 0 );
	PlaySilence( 60, 0 );

	if( Cadence_Canceled == 0 ) GPIO_SetBits( GPIOD, GPIO_Pin_15 );	// GREEN (red) light ON

	PlayTone( 2250, Square632HzData, SQUARE_632HZ_SIZE, 0 );

	// initialize the sensors so the tick value is set only once from the timer 6 interrupt
	Aux_1_Sensor_1_Tick = 0;
	Aux_1_Sensor_2_Tick = 0;
	Aux_2_Sensor_1_Tick = 0;
	Aux_2_Sensor_2_Tick = 0;

	// reset system ticks so delays don't roll over
	Timer_Tick 			= 0;

	// After gate drops turn solenoid off
	if( Menu_Array[RELEASE_DEVICE].context == RELEASE_DEVICE_SOLENOID )
	{
		// timer 6 handles turning the solenoid off
	}
	else if( Menu_Array[RELEASE_DEVICE].context == RELEASE_DEVICE_MAGNET )
	{
		// don't have to do anything for the magnet
	}

	if( Cadence_Canceled )
	{
		Cadence_Canceled = 0;
		Cancel_Timing 	 = 1;

		InitAudio();

		// play cancel tones
		PlaySilence( 100, 1 );
		PlayTone( 220, Square740HzData, SQUARE_740HZ_SIZE, 1 );
		PlayTone( 440, Square680HzData, SQUARE_680HZ_SIZE, 1 );

		// check for exit
		WaitForButtonUp();
	}

	// turn all the lights off
	GPIO_ResetBits( GPIOD, GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15 );

	// Get rid of the static that occurs intermittently
	InitAudio();

	// restore volume
	int max_volume = (128 * Menu_Array[AUDIO_IN_VOLUME].context) / 100;

	for( ; volume < max_volume; volume += step )
	{
		DigiPotWrite( 0x0000 | volume );
		Delay(40);
	}
}

void InitPorts( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef	 SPI_InitStructure;
	ADC_InitTypeDef	 ADC_InitStructure;

	// Enable Port A signals
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE );

	GPIO_ResetBits( GPIOA, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 );
	GPIO_SetBits  ( GPIOA, GPIO_Pin_5 | GPIO_Pin_7 );

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd	= 0;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_0 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd	= 0;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7; // SPI 1 pins
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	GPIO_PinAFConfig( GPIOA, GPIO_PinSource5, GPIO_AF_SPI1 );
	GPIO_PinAFConfig( GPIOA, GPIO_PinSource6, GPIO_AF_SPI1 );
	GPIO_PinAFConfig( GPIOA, GPIO_PinSource7, GPIO_AF_SPI1 );

	// Enable Port B signals
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE );

	GPIO_ResetBits( GPIOB, GPIO_Pin_11 | GPIO_Pin_13 );
	GPIO_SetBits( GPIOB, GPIO_Pin_10 | GPIO_Pin_12 );

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd	= 0;
	GPIO_Init( GPIOB, &GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd	= 0;
	GPIO_Init( GPIOB, &GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_0 | GPIO_Pin_1; // ADC 1, 2 pins
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	GPIO_Init( GPIOB, &GPIO_InitStructure );

	//GPIO_PinAFConfig( GPIOB, GPIO_PinSource0, GPIO_AF_ADC1 );
	//GPIO_PinAFConfig( GPIOB, GPIO_PinSource1, GPIO_AF_ADC1 );

	// Enable Port C signals
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE );

	GPIO_ResetBits( GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_5 | GPIO_Pin_14 | GPIO_Pin_15 );
	GPIO_SetBits( GPIOC, GPIO_Pin_4 );

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd	= 0;
	GPIO_Init( GPIOC, &GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_3 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd	= 0;
	GPIO_Init( GPIOC, &GPIO_InitStructure );

	// Enable Port D signals
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOD, ENABLE );

	GPIO_ResetBits( GPIOD, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 );

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd	= 0;
	GPIO_Init( GPIOD, &GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_5 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd	= 0;
	GPIO_Init( GPIOD, &GPIO_InitStructure );

	// Enable Port E signals
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOE, ENABLE );

	GPIO_SetBits( GPIOE, GPIO_Pin_3 );

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_3 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd	= 0;
	GPIO_Init( GPIOE, &GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd	= 0;
	GPIO_Init( GPIOE, &GPIO_InitStructure );

	// enable SPI 1
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1, ENABLE );

	SPI_InitStructure.SPI_Direction 			= SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode					= SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize				= SPI_DataSize_16b;
	SPI_InitStructure.SPI_CPOL 					= SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA 					= SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS 					= SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler 	= SPI_BaudRatePrescaler_16;
	SPI_InitStructure.SPI_FirstBit 				= SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial 		= 7;
	SPI_Init( SPI1, &SPI_InitStructure );

	SPI_CalculateCRC( SPI1, DISABLE );
	SPI_Cmd( SPI1, ENABLE );

	// enable ADC 1 (bat)
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1, ENABLE );

	ADC_InitStructure.ADC_DataAlign				= ADC_DataAlign_Right;
	ADC_InitStructure.ADC_Resolution 			= ADC_Resolution_12b;
	ADC_InitStructure.ADC_ContinuousConvMode 	= ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv 		= ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_ExternalTrigConvEdge 	= ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_NbrOfConversion 		= 1;
	ADC_InitStructure.ADC_ScanConvMode 			= DISABLE;
	ADC_Init( ADC1, &ADC_InitStructure );

	ADC_Cmd( ADC1, ENABLE );
	ADC_RegularChannelConfig( ADC1, ADC_Channel_9, 1, ADC_SampleTime_144Cycles );

	// enable ADC 2 (aux)
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC2, ENABLE );

	ADC_InitStructure.ADC_DataAlign				= ADC_DataAlign_Right;
	ADC_InitStructure.ADC_Resolution			= ADC_Resolution_12b;
	ADC_InitStructure.ADC_ContinuousConvMode	= ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv 		= ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_ExternalTrigConvEdge 	= ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_NbrOfConversion 		= 1;
	ADC_InitStructure.ADC_ScanConvMode 			= DISABLE;
	ADC_Init( ADC2, &ADC_InitStructure );

	ADC_Cmd( ADC2, ENABLE );
	ADC_RegularChannelConfig( ADC2, ADC_Channel_8, 1, ADC_SampleTime_144Cycles );

	// initialize random number generator
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
	RNG_Cmd( ENABLE );
}

void InitClocks( void )
{
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq( &RCC_Clocks );
}

void InitAudio( void )
{
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE ); // enable PORT C for audio codec

	codec_init();
	codec_ctrl_init( );

	// enable I2C serial interface
	I2S_Cmd( CODEC_I2S, ENABLE );
}

//	LCD hardware configuration
//
//	PE8 - PE15  = LCD Data Bus
//	PE7			= Busy Flag (1 = busy)
//	PA1			= Register Select (1 = data write / read, 0 = instruction write / busy read)
//	PA2			= Read / Write (1 = read / 0 = write)
//	PA3			= E Clock (idle low, latch on high to low)
//
void InitLCD( void )
{
	Delay( 100 );
	WriteLCD_Command( 0x0030 );
	Delay( 10 );
	WriteLCD_Command( 0x0038 ); // 8-bit data, 2-lines, 5x8 font
	Delay( 5 );
	WriteLCD_Command( 0x0008 ); // display off
	Delay( 5 );
	WriteLCD_Command( 0x0001 ); // clear display
	Delay( 10 );
	WriteLCD_Command( 0x0006 ); // cursor moves right
	Delay( 5 );
	WriteLCD_Command( 0x000C ); // display on, cursor off, cursor no blink
	Delay( 5 );
	WriteLCD_Command( 0x0002 ); // cursor home
	Delay( 5 );

	DefineCustomCharacters();

	// clear display memory arrays
	int i;

	for( i = 0; i < DISPLAY_WIDTH; i++ )
	{
		display_memory[0][ i ] = 0x20;
		display_memory[1][ i ] = 0x20;

		display_memory_cache[0][i] = 0x20;
		display_memory_cache[1][i] = 0x20;
	}
}

void DefineCustomCharacters( void )
{
	WriteLCD_Command( 0x40 ); // Character code 0x00, start of CGRAM

	// \0
	// block
	WriteLCD_Data( 0x1f );
	WriteLCD_Data( 0x1f );
	WriteLCD_Data( 0x1f );
	WriteLCD_Data( 0x1f );
	WriteLCD_Data( 0x1f );
	WriteLCD_Data( 0x1f );
	WriteLCD_Data( 0x1f );
	WriteLCD_Data( 0x1f );

	// \1				   //   43210
	WriteLCD_Data( 0x18 ); // 0 **
	WriteLCD_Data( 0x1C ); // 1 ***
	WriteLCD_Data( 0x1E ); // 2 ****
	WriteLCD_Data( 0x1F ); // 3 *****
	WriteLCD_Data( 0x1E ); // 4 ****
	WriteLCD_Data( 0x1C ); // 5 ***
	WriteLCD_Data( 0x18 ); // 6 **
	WriteLCD_Data( 0x00 ); // 7

	// \2				   //   43210
	WriteLCD_Data( 0x03 ); // 0    **
	WriteLCD_Data( 0x07 ); // 1   ***
	WriteLCD_Data( 0x0F ); // 2  ****
	WriteLCD_Data( 0x1F ); // 3 *****
	WriteLCD_Data( 0x0F ); // 4  ****
	WriteLCD_Data( 0x07 ); // 5   ***
	WriteLCD_Data( 0x03 ); // 6    **
	WriteLCD_Data( 0x00 ); // 7

	// \3				   //   43210
	WriteLCD_Data( 0x04 ); // 0   *
	WriteLCD_Data( 0x0E ); // 1  ***
	WriteLCD_Data( 0x1F ); // 2 *****
	WriteLCD_Data( 0x04 ); // 3   *
	WriteLCD_Data( 0x04 ); // 4   *
	WriteLCD_Data( 0x04 ); // 5   *
	WriteLCD_Data( 0x04 ); // 6   *
	WriteLCD_Data( 0x04 ); // 7   *

	// \4				   //   43210
	WriteLCD_Data( 0x04 ); // 0   *
	WriteLCD_Data( 0x04 ); // 1   *
	WriteLCD_Data( 0x04 ); // 2   *
	WriteLCD_Data( 0x04 ); // 3   *
	WriteLCD_Data( 0x04 ); // 4   *
	WriteLCD_Data( 0x1F ); // 5 *****
	WriteLCD_Data( 0x0E ); // 6  ***
	WriteLCD_Data( 0x04 ); // 7   *
}

void PlayDropGateAnimation( void )	// TODO: Figure out why subsequent writes to CGRAM fail to change custom characters
{
	char D[8] = { 0x1C, 0x12, 0x11, 0x11, 0x11, 0x12, 0x1C, 0x00 };
	char R[8] = { 0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11, 0x00 };
	char O[8] = { 0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E, 0x00 };
	char P[8] = { 0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10, 0x00 };

	char G[8] = { 0x0E, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0F, 0x00 };
	char A[8] = { 0x0E, 0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x00 };
	char T[8] = { 0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00 };
	char E[8] = { 0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F, 0x00 };

	WriteLCD_Command( 0x40 ); // Character code 0x00, start of CGRAM
	Delay( 10 );

	WriteLCD_Data( (int)D );
	WriteLCD_Data( (int)R );
	WriteLCD_Data( (int)O );
	WriteLCD_Data( (int)P );
	WriteLCD_Data( (int)G );
	WriteLCD_Data( (int)A );
	WriteLCD_Data( (int)T );
	WriteLCD_Data( (int)E );

	WriteLCD_Line( "\0\1\2\3 \4\5\6\7", 1 );
	UpdateLCD();
}

void WriteLCD_Command( int data )
{
	GPIO_ResetBits	( GPIOB, GPIO_Pin_11 ); // E clock low
	GPIO_ResetBits	( GPIOB, GPIO_Pin_13 ); // RS low (command)
	GPIO_ResetBits	( GPIOB, GPIO_Pin_12 ); // R/W low (write)
	GPIO_SetBits	( GPIOB, GPIO_Pin_11 ); // E clock high

	ShortDelay( 1 );

	// write data
	GPIOE->ODR = ((GPIOE->ODR & 0x00FF) | (data << 8));

	ShortDelay( 1 );

	GPIO_ResetBits( GPIOB, GPIO_Pin_3 ); // E clock low
}

void WriteLCD_Data( int data )
{
	GPIO_ResetBits	( GPIOB, GPIO_Pin_11 ); // E clock low
	GPIO_SetBits	( GPIOB, GPIO_Pin_13 ); // RS high (data)
	GPIO_ResetBits	( GPIOB, GPIO_Pin_12 ); // R/W low (write)
	GPIOE->ODR = 0x20 << 8; // TODO: Not sure why this fixes initial garbage character, find out why
	GPIO_SetBits	( GPIOB, GPIO_Pin_11 ); // E clock high

	ShortDelay( 1 );

	// write data
	GPIOE->ODR = ((GPIOE->ODR & 0x00FF) | (data << 8));

	ShortDelay( 1 );

	GPIO_ResetBits( GPIOB, GPIO_Pin_3 ); // E clock low
}

void WriteLCD_Line( const char * pString, int line_number )
{
	int i, j;

	for( i = 0, j = 0; j < DISPLAY_WIDTH; i++, j++ )
		display_memory[line_number][i] = pString[j];
}

void WriteLCD_LineCentered( const char * pString, int line_number )
{
	int i = 0;

	// fill display memory with spaces
	for( ; i < DISPLAY_WIDTH; i++ )
		display_memory[line_number][i] = 0x20;

	// skip leading spaces in string to write if there are any
	int line_len = strlen( pString );

	line_len = (line_len > DISPLAY_WIDTH ) ? DISPLAY_WIDTH : line_len;

	const int indent_index = (DISPLAY_WIDTH / 2 - (line_len / 2 - 1 )) - 1;

	// copy to centered position
	for( i = 0; i < line_len; i++ )
	{
		display_memory[ line_number ][ indent_index + i ] = pString[ i ];
	}
}

void UpdateLCD( void )
{
	int i;

	// update line 1
	WriteLCD_Command( 0x80 ); // top left of display data

	for( i = 0 ; i < DISPLAY_WIDTH; i++ )
	{
		if( display_memory[0][i] != display_memory_cache[0][i] )
		{
			// new character is different than old so write a new one out
			WriteLCD_Data( display_memory[0][i] );
			display_memory_cache[0][i] = display_memory[0][i];
		}
		else
		{
			// don't write a new character, just advance cursor to next DDRAM address
			WriteLCD_Command( 0x80 + 1 + i );
		}
	}

	// update line 2
	WriteLCD_Command( 0xC0 ); // bottom left of display data

	for( i = 0; i < DISPLAY_WIDTH; i++ )
	{
		if( display_memory[1][i] != display_memory_cache[1][i] )
		{
			// new character is different than old so write a new one out
			WriteLCD_Data( display_memory[1][i] );
			display_memory_cache[1][i] = display_memory[1][i];
		}
		else
		{
			// don't write a new character, just advance cursor to next DDRAM address
			WriteLCD_Command( 0xC0 + 1 + i );
		}
	}
}

void StartTimer6( void )
{
    RCC->APB1ENR	|= RCC_APB1ENR_TIM6EN;  	// Enable TIM6 clock

    //TIM6->PSC		= 8399;                		// 84,000,000 Hz / 8,400 = 10,000 timer ticks per second
    TIM6->PSC		= 839;                		// 84,000,000 Hz / 8,40 = 100,000 timer ticks per second
    TIM6->ARR		= 9;                   		// 10,000 Hz / 10 = 1000 timer ints per second

    //TIM6->CR1		|= TIM_CR1_OPM;             // One pulse mode
    TIM6->EGR		|= TIM_EGR_UG;              // Force update
    TIM6->SR		&= ~TIM_SR_UIF;             // Clear the update flag
    TIM6->DIER		|= TIM_DIER_UIE;            // Enable interrupt on update event
    NVIC_EnableIRQ(TIM6_DAC_IRQn);              // Enable TIM6 IRQ
    TIM6->CR1 		|= TIM_CR1_CEN;             // Enable TIM6 counter

    Timer_Tick = 0;
}

void PulseSolenoid( unsigned int pulse_time )
{
	Gate_Drop_Delay	= Menu_Array[ GATE_DROPS_ON ].context * 10;

	Solenoid_Pulse_Time = pulse_time * 10;

	Pulse_Solenoid  = 1;
}

void TurnMagnetOff( void )
{
	Gate_Drop_Delay	= Menu_Array[ GATE_DROPS_ON ].context * 10;

	Turn_Magnet_Off = 1;
}

void TIM6_DAC_IRQHandler()
{
    TIM6->SR &= ~TIM_SR_UIF;   // Interrupt has been handled

	Timer_Tick += 1;

	// read all the auxililary inputs and set their times for later use as timer sensors
	// If it is zero it can be set, if it's not? then it can't be changed until it's value is used and reset
	if( (GPIOB->IDR & 0x0080) == 0 && Aux_1_Sensor_1_Tick == 0 ) Aux_1_Sensor_1_Tick = Timer_Tick;
	if( (GPIOB->IDR & 0x0100) == 0 && Aux_1_Sensor_2_Tick == 0 ) Aux_1_Sensor_2_Tick = Timer_Tick;
	if( (GPIOB->IDR & 0x0010) == 0 && Aux_2_Sensor_1_Tick == 0 ) Aux_2_Sensor_1_Tick = Timer_Tick;
	if( (GPIOB->IDR & 0x0020) == 0 && Aux_2_Sensor_2_Tick == 0 ) Aux_2_Sensor_2_Tick = Timer_Tick;

	// Handle aborting the cadence when BUTTON_C is pressed from the RF remote
	if( (GPIOB->IDR & 0x8000) && Menu_Array[WIRELESS_REMOTE].context == WIRELESS_REMOTE_ENABLED )
	{
		Cadence_Canceled = 1;

		if( Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_SOLENOID )
		{
			Pulse_Solenoid	 = 0;

			Solenoid_Pulse_Time = 0;

			GPIO_ResetBits( GPIOA, GPIO_Pin_2 ); // gate 1 OFF
			GPIO_ResetBits( GPIOA, GPIO_Pin_1 ); // gate 2 OFF
		}
		if( Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_MAGNET )
		{
			Magnet_On 		= 1;

			Turn_Magnet_Off = 0;
		}
		if( Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_AIR_RAM )
		{
			// TODO: implement this
		}

		return;
	}

	// handle dropping the gate
	if( Pulse_Solenoid == 1 )
	{
		if( Gate_Drop_Delay != 0 )
		{
			Gate_Drop_Delay -= 1;
		}
		else
		{
			if( Solenoid_Pulse_Time != 0 )
			{
				Solenoid_Pulse_Time -= 1;

				GPIO_SetBits( GPIOA, GPIO_Pin_2 );	// gate 1 ON
				GPIO_SetBits( GPIOA, GPIO_Pin_1 );	// gate 2 ON
			}
			else
			{
				// handle pulsing the solenoid
				GPIO_ResetBits( GPIOA, GPIO_Pin_2 ); // gate 1 OFF
				GPIO_ResetBits( GPIOA, GPIO_Pin_1 ); // gate 2 OFF

				Pulse_Solenoid = 0;
			}
		}
	}
	else if( Turn_Magnet_Off == 1 )
	{
		if( Gate_Drop_Delay != 0 )
		{
			Gate_Drop_Delay -= 1;
		}
		else
		{
			GPIO_ResetBits( GPIOA, GPIO_Pin_2 ); // gate 1 OFF
			GPIO_ResetBits( GPIOA, GPIO_Pin_1 ); // gate 2 OFF

			Turn_Magnet_Off = 0;
			Magnet_On 		= 0;
		}
	}
}

void EXTI1_IRQHandler()
{
	if ( EXTI_GetITStatus( EXTI_Line1 ) != RESET )
	{
		EXTI_ClearFlag( EXTI_Line1 );
	}
}

void ShortDelay( unsigned int ticks )
{
	const unsigned int end_time = Timer_Tick + ticks;

	while( Timer_Tick < end_time );
}

void Delay( unsigned int milliseconds )
{
	const unsigned int end_time = Timer_Tick + (milliseconds * 10);

	while( Timer_Tick < end_time );
}

static unsigned int GetRandomNumber( void )
{
	static unsigned int last_value	= 0;
	static unsigned int new_value	= 0;
	unsigned int 		error_bits	= 0;

	error_bits = RNG_SR_SEIS | RNG_SR_CEIS;

	while ( new_value == last_value )
	{
		// Check for error flags and if data is ready.
		if ( ((RNG->SR & error_bits) == 0) && ( (RNG->SR & RNG_SR_DRDY) == 1 ) )
		{
		  new_value = RNG_GetRandomNumber();
		}
	}

	last_value = new_value;

	return new_value;
}

void DigiPotWrite( u16 data )
{
	GPIO_ResetBits( GPIOC, GPIO_Pin_4 );

	while( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET );
	SPI_I2S_SendData( SPI1, data );

	while( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET );
	SPI_I2S_ReceiveData( SPI1 );

	GPIO_SetBits( GPIOC, GPIO_Pin_4 );
}

void PlaySilence( unsigned int milliseconds, unsigned int dont_abort )
{
	unsigned int i = (( 48000 * milliseconds ) / 1000);	// 48,000 samples/sec * length / 1000
	int16_t sample = 0;

	do
	{
		while ( ! SPI_I2S_GetFlagStatus( CODEC_I2S, SPI_I2S_FLAG_TXE ) );
		SPI_I2S_SendData(CODEC_I2S, sample);	// output left channel

		while ( ! SPI_I2S_GetFlagStatus( CODEC_I2S, SPI_I2S_FLAG_TXE ) );
		SPI_I2S_SendData(CODEC_I2S, sample);	// output right channel
	}
	while( i-- > 0 && !(Cadence_Canceled == 1 && dont_abort == 0) );
}

void PlayTone( unsigned int milliseconds, const int16_t * tone_data, unsigned int length, unsigned int dont_abort )
{
	if( Cadence_Canceled == 1 && dont_abort == 0 ) return;

	int i = (( 48000 * milliseconds ) / 1000);	// 48,000 samples/sec * length / 1000
	int j = 0;
	int scale = 1;
	int16_t sample;

	do
	{
		// Play the tone data
		sample = tone_data[ j++ % (length - 1) ] / scale; // get sample from table

		while ( ! SPI_I2S_GetFlagStatus( CODEC_I2S, SPI_I2S_FLAG_TXE ) );
		SPI_I2S_SendData(CODEC_I2S, sample);	// output left channel

		GPIO_SetBits( GPIOC, GPIO_Pin_14 ); 	// why do I need this?

		if ( (i - 6) < 0 ) scale *= 4;

		while ( ! SPI_I2S_GetFlagStatus( CODEC_I2S, SPI_I2S_FLAG_TXE ) );
		SPI_I2S_SendData(CODEC_I2S, sample);	// output right channel

		GPIO_ResetBits( GPIOC, GPIO_Pin_14 );	// why do I need this?
	}
	while( i-- > 0 && !(Cadence_Canceled == 1 && dont_abort == 0) );
}

void PlaySample16khz( const int16_t * pSampleData, unsigned int start, unsigned int stop, unsigned int dont_abort )
{
	int16_t sample;
	unsigned int i;

	for( i = start; i < stop; i++ )
	{
		if( Cadence_Canceled == 1 && dont_abort == 0 ) return;

		sample = pSampleData[ i ];

		while ( ! SPI_I2S_GetFlagStatus( CODEC_I2S, SPI_I2S_FLAG_TXE ) );
		SPI_I2S_SendData(CODEC_I2S, sample);	// output left channel
		GPIO_SetBits( GPIOC, GPIO_Pin_14 ); 	// why do I need this?

		while ( ! SPI_I2S_GetFlagStatus( CODEC_I2S, SPI_I2S_FLAG_TXE ) );
		SPI_I2S_SendData(CODEC_I2S, sample);	// output right channel
		GPIO_ResetBits( GPIOC, GPIO_Pin_14 );	// why do I need this?

		while ( ! SPI_I2S_GetFlagStatus( CODEC_I2S, SPI_I2S_FLAG_TXE ) );
		SPI_I2S_SendData(CODEC_I2S, sample);	// output left channel
		GPIO_SetBits( GPIOC, GPIO_Pin_14 ); 	// why do I need this?

		while ( ! SPI_I2S_GetFlagStatus( CODEC_I2S, SPI_I2S_FLAG_TXE ) );
		SPI_I2S_SendData(CODEC_I2S, sample);	// output right channel
		GPIO_ResetBits( GPIOC, GPIO_Pin_14 );	// why do I need this?

		while ( ! SPI_I2S_GetFlagStatus( CODEC_I2S, SPI_I2S_FLAG_TXE ) );
		SPI_I2S_SendData(CODEC_I2S, sample);	// output left channel
		GPIO_SetBits( GPIOC, GPIO_Pin_14 ); 	// why do I need this?

		while ( ! SPI_I2S_GetFlagStatus( CODEC_I2S, SPI_I2S_FLAG_TXE ) );
		SPI_I2S_SendData(CODEC_I2S, sample);	// output right channel
		GPIO_ResetBits( GPIOC, GPIO_Pin_14 );	// why do I need this?
	}
}

void PlaySample24khz( const int16_t * pSampleData, unsigned int start, unsigned int stop, unsigned int dont_abort )
{
	int16_t sample = 0;
	int16_t previous = 0;

	unsigned int i;

	for( i = start; i < stop; i++ )
	{
		if( Cadence_Canceled == 1 && dont_abort == 0 ) return;

		previous = sample;
		sample = pSampleData[ i ];

		while ( ! SPI_I2S_GetFlagStatus( CODEC_I2S, SPI_I2S_FLAG_TXE ) );
		SPI_I2S_SendData( CODEC_I2S, previous + ((sample - previous) / 2) );	// output left
		GPIO_SetBits( GPIOC, GPIO_Pin_14 ); 	// why do I need this?

		while ( ! SPI_I2S_GetFlagStatus( CODEC_I2S, SPI_I2S_FLAG_TXE ) );
		SPI_I2S_SendData( CODEC_I2S, previous + ((sample - previous) / 2) );	// output right
		GPIO_ResetBits( GPIOC, GPIO_Pin_14 );	// why do I need this?

		while ( ! SPI_I2S_GetFlagStatus( CODEC_I2S, SPI_I2S_FLAG_TXE ) );
		SPI_I2S_SendData(CODEC_I2S, sample);	// output left channel
		GPIO_SetBits( GPIOC, GPIO_Pin_14 ); 	// why do I need this?

		while ( ! SPI_I2S_GetFlagStatus( CODEC_I2S, SPI_I2S_FLAG_TXE ) );
		SPI_I2S_SendData(CODEC_I2S, sample);	// output right channel
		GPIO_ResetBits( GPIOC, GPIO_Pin_14 );	// why do I need this?
	}
}

void PrintElapsedTime( unsigned int ticks, unsigned int display_line )
{
	int minutes     = (ticks / 60000	 );
    int seconds     = (ticks / 1000	) % 60;
    int tens_place  = (ticks / 100	) % 10;
    int hund_place  = (ticks / 10 	) % 10;
    int thou_place  = (ticks / 1	) % 10;

    char line[DISPLAY_WIDTH] = SPACES;

    if( minutes > 0 )
    	sprintf( line, "%d:%d.%d%d%d", minutes, seconds, tens_place, hund_place, thou_place );
    else
    	sprintf( line, "%d.%d%d%d", seconds, tens_place, hund_place, thou_place );

    WriteLCD_LineCentered( line, display_line );
}

void PlayGateUpTones( void )
{
	PlayTone( 250, Square1150HzData, SQUARE_1150HZ_SIZE, 1 );
	PlaySilence( 250, 1 );

	PlayTone( 250, Square1150HzData, SQUARE_1150HZ_SIZE, 1 );
	PlaySilence( 250, 1 );

	PlayTone( 250, Square1150HzData, SQUARE_1150HZ_SIZE, 1 );
	PlaySilence( 250, 1 );

	PlayTone( 250, Square1150HzData, SQUARE_1150HZ_SIZE, 1 );
	PlaySilence( 250, 1 );

	PlaySilence( 1, 1);
	PlayTone( 250, Square1150HzData, SQUARE_1150HZ_SIZE, 1 );
	PlaySilence( 250, 1 );
	InitAudio();
}

static int16_t prev = 0xFFFF;
static int16_t last_index = 0;

int ReadInputs( int * inputs )
{
	const unsigned int PortA_IDR = GPIOA->IDR;
	const unsigned int PortB_IDR = GPIOB->IDR;
	const unsigned int PortD_IDR = GPIOD->IDR;

	// check for timer sensors
	if( (PortB_IDR & 0x0080) == 0 ) { *inputs = AUX_1_SENSOR_1; return 0; }
	if( (PortB_IDR & 0x0100) == 0 ) { *inputs = AUX_1_SENSOR_2; return 0; }
	if( (PortB_IDR & 0x0010) == 0 ) { *inputs = AUX_2_SENSOR_1; return 0; }
	if( (PortB_IDR & 0x0020) == 0 ) { *inputs = AUX_2_SENSOR_2; return 0; }

	// check for RF remote buttons
	if( PortA_IDR & 0x0001 ) { *inputs = BUTTON_A; return 0; }
	if( PortB_IDR & 0x4000 ) { *inputs = BUTTON_B; return 0; }
	if( PortB_IDR & 0x8000 ) { *inputs = BUTTON_C; return 0; }
	if( PortD_IDR & 0x0100 ) { *inputs = BUTTON_D; return 0; }

	int16_t current_index = 0;

	// process encoder turns
	if ( prev != (GPIOE->IDR & 0x0070) )
	{
		prev = (GPIOE->IDR & 0x0070);

		if ( 0x0000 == (prev & 0x0010) )
		{
			*inputs = BUTTON_E;
			return 0;
		}
		else
		{
			if ( 0x0000 == (prev & 0x0060) )	  { current_index = 2; }
			else if ( 0x0020 == (prev & 0x0060) ) { current_index = 3; }
			else if ( 0x0040 == (prev & 0x0060) ) { current_index = 1; }
			else if ( 0x0060 == (prev & 0x0060) ) { current_index = 0; }

			if(( current_index == 1 && last_index == 0 ) ||
			   ( current_index == 2 && last_index == 1 ) ||
			   ( current_index == 3 && last_index == 2 ) ||
			   ( current_index == 0 && last_index == 3 ))
			{
				last_index = current_index;
				return 1;
			}

			if(( current_index == 2 && last_index == 3 ) ||
			   ( current_index == 1 && last_index == 2 ) ||
			   ( current_index == 0 && last_index == 1 ) ||
			   ( current_index == 3 && last_index == 0 ))
			{
				last_index = current_index;
				return -1;
			}
		}
	}

	*inputs = 0;

	return 0;
}

void WaitForButtonUp()
{
	int inputs = 0;

	do { ReadInputs( &inputs ); } while ( inputs != 0 );
}

int ReadBatteryCondition( void )
{
	ADC_SoftwareStartConv( ADC1 );
	while( !ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) );

	int battery_level = ADC_GetConversionValue( ADC1 );

	return battery_level;
}

void SetMenuText( char * menu, char * text )
{
	int i;

	for( i = 0; i < DISPLAY_WIDTH; i++ ) menu[i] = text[i];
}

void PlayTimeOrPercent( unsigned int ticks, int play_type )
{
#ifdef NUMBERS

	int minutes     = (ticks / 60000);
    int seconds     = (ticks / 1000	) % 60;
    int tens_place  = (ticks / 100	) % 10;
    int hund_place  = (ticks / 10 	) % 10;
    int thou_place  = (ticks / 1	) % 10;

    int play_seconds	= 0;
    int play_tens		= 0;
    int play_hund		= 0;
    int play_thou		= 0;

    // skip playing elapsed time if time is 1000 minutes or above
    if( minutes > 999 ) return;


    // play zero seconds or percent and exit
    if( ticks == 0 )
    {
        PlaySilence( 100, 0 );
    	PlaySample24khz( ZeroData, 0, ZERO_SIZE, 1 );

    	if( play_type == PLAY_TIME ) 		 PlaySample24khz( SecondsData,	0, SECONDS_SIZE, 1 );
    	else if( play_type == PLAY_PERCENT ) PlaySample24khz( PercentData,	0, PERCENT_SIZE, 1 );

    	InitAudio();

    	return;
    }

    // play 1 - 999 minute(s)
    if( minutes > 0 )
    {
    	int play_hundred = 0;

    	int play_minute = ( minutes == 1 ) ? 1 : 0;

		// hundreds place
		if		( minutes >= 900 )	{ PlaySample24khz( NineData,	0, NINE_SIZE 	, 1 ); minutes -=  900; play_hundred = 1; }
		else if	( minutes >= 800 )	{ PlaySample24khz( EightData,	0, EIGHT_SIZE 	, 1 ); minutes -=  800; play_hundred = 1; }
		else if	( minutes >= 700 )	{ PlaySample24khz( SevenData,	0, SEVEN_SIZE	, 1 ); minutes -=  700; play_hundred = 1; }
		else if	( minutes >= 600 )	{ PlaySample24khz( SixData,		0, SIX_SIZE 	, 1 ); minutes -=  600; play_hundred = 1; }
		else if ( minutes >= 500 )	{ PlaySample24khz( FiveData,	0, FIVE_SIZE	, 1 ); minutes -=  500; play_hundred = 1; }
		else if ( minutes >= 400 )	{ PlaySample24khz( FourData,	0, FOUR_SIZE	, 1 ); minutes -=  400; play_hundred = 1; }
		else if ( minutes >= 300 )	{ PlaySample24khz( ThreeData,	0, THREE_SIZE	, 1 ); minutes -=  300; play_hundred = 1; }
		else if ( minutes >= 200 )	{ PlaySample24khz( TwoData,		0, TWO_SIZE		, 1 ); minutes -=  200; play_hundred = 1; }
		else if ( minutes >= 100 )	{ PlaySample24khz( OneData,		0, ONE_SIZE		, 1 ); minutes -=  100; play_hundred = 1; }

		if ( play_hundred == 1 )	{ PlaySample24khz( HundredData,	0, HUNDRED_SIZE, 1 ); }

		// tens place
		if		( minutes >= 90  )	{ PlaySample24khz( NinetyData,	0, NINETY_SIZE 	, 1 ); minutes -=  90; }
		else if	( minutes >= 80  )	{ PlaySample24khz( EightyData,	0, EIGHTY_SIZE 	, 1 ); minutes -=  80; }
		else if	( minutes >= 70  )	{ PlaySample24khz( SeventyData,	0, SEVENTY_SIZE	, 1 ); minutes -=  70; }
		else if	( minutes >= 60  )	{ PlaySample24khz( SixtyData,	0, SIXTY_SIZE 	, 1 ); minutes -=  60; }
		else if ( minutes >= 50  )	{ PlaySample24khz( FiftyData,	0, FIFTY_SIZE	, 1 ); minutes -=  50; }
		else if ( minutes >= 40  )	{ PlaySample24khz( FortyData,	0, FORTY_SIZE	, 1 ); minutes -=  40; }
		else if ( minutes >= 30  )	{ PlaySample24khz( ThirtyData,	0, THIRTY_SIZE	, 1 ); minutes -=  30; }
		else if ( minutes >= 20  )	{ PlaySample24khz( TwentyData,	0, TWENTY_SIZE	, 1 ); minutes -=  20; }

		// teens
		else if ( minutes >= 19  )	{ PlaySample24khz( NineData,	0, NINE_SIZE	, 1 ); minutes -=  19;
									  PlaySample24khz( TeenData,	0, TEEN_SIZE	, 1 );  }

		else if ( minutes >= 18  )	{ PlaySample24khz( EightData,	0, EIGHT_SIZE	, 1 ); minutes -=  18;
		 	 	 	 	 	 	 	  PlaySample24khz( TeenData,	0, TEEN_SIZE	, 1 );  }

		else if ( minutes >= 17  )	{ PlaySample24khz( SevenData,	0, SEVEN_SIZE	, 1 ); minutes -=  17;
									  PlaySample24khz( TeenData,	0, TEEN_SIZE	, 1 );  }

		else if ( minutes >= 16  )	{ PlaySample24khz( SixData,		0, SIX_SIZE		, 1 ); minutes -=  16;
		  	  	  	  	  	  	  	  PlaySample24khz( TeenData,	0, TEEN_SIZE	, 1 );  }

		else if ( minutes >= 15  )	{ PlaySample24khz( FifData,		0, FIF_SIZE		, 1 ); minutes -=  15;
		  	  	  	  	  	  	  	  PlaySample24khz( TeenData,	0, TEEN_SIZE	, 1 );  }

		else if ( minutes >= 14  )	{ PlaySample24khz( FourData,	0, FOUR_SIZE	, 1 ); minutes -=  14;
		  	  	  	  	  	  	  	  PlaySample24khz( TeenData,	0, TEEN_SIZE	, 1 );  }

		else if ( minutes >= 13  )	{ PlaySample24khz( ThirData,	0, THIR_SIZE	, 1 ); minutes -=  13;
									  PlaySample24khz( TeenData,	0, TEEN_SIZE	, 1 );  }
		// 10-12
		else if ( minutes >= 12  )	{ PlaySample24khz( TwelveData,	0, TWELVE_SIZE	, 1 ); minutes -=  12; }
		else if ( minutes >= 11  )	{ PlaySample24khz( ElevenData,	0, ELEVEN_SIZE	, 1 ); minutes -=  11; }
		else if ( minutes >= 10  )	{ PlaySample24khz( TenData,		0, TEN_SIZE		, 1 ); minutes -=  10; }

		// ones place
		if		( minutes >= 9  )	{ PlaySample24khz( NineData,	0, NINE_SIZE	, 1 ); minutes -= 9; }
		else if ( minutes >= 8  )	{ PlaySample24khz( EightData,	0, EIGHT_SIZE	, 1 ); minutes -= 8; }
		else if ( minutes >= 7  )	{ PlaySample24khz( SevenData,	0, SEVEN_SIZE	, 1 ); minutes -= 7; }
		else if ( minutes >= 6  )	{ PlaySample24khz( SixData,		0, SIX_SIZE		, 1 ); minutes -= 6; }
		else if ( minutes >= 5  )	{ PlaySample24khz( FiveData,	0, FIVE_SIZE	, 1 ); minutes -= 5; }
		else if ( minutes >= 4  )	{ PlaySample24khz( FourData,	0, FOUR_SIZE	, 1 ); minutes -= 4; }
		else if ( minutes >= 3  )	{ PlaySample24khz( ThreeData,	0, THREE_SIZE	, 1 ); minutes -= 3; }
		else if ( minutes >= 2  )	{ PlaySample24khz( TwoData,		0, TWO_SIZE		, 1 ); minutes -= 2; }
		else if ( minutes >= 1  )	{ PlaySample24khz( OneData,		0, ONE_SIZE		, 1 ); minutes -= 1; }

		if ( play_type == PLAY_TIME )
		{
			if( play_minute == 1 )PlaySample24khz( MinuteData,	0, MINUTE_SIZE, 1 );
			else 				  PlaySample24khz( MinutesData, 0, MINUTES_SIZE, 1 );
		}
    }

    // play whole seconds
    if( seconds > 0 )
    {
    	play_seconds = 1;

    	// tens place
		if		( seconds >= 90  )	{ PlaySample24khz( NinetyData,	0, NINETY_SIZE 	, 1 ); seconds -=  90; }
		else if	( seconds >= 80  )	{ PlaySample24khz( EightyData,	0, EIGHTY_SIZE 	, 1 ); seconds -=  80; }
		else if	( seconds >= 70  )	{ PlaySample24khz( SeventyData,	0, SEVENTY_SIZE	, 1 ); seconds -=  70; }
		else if	( seconds >= 60  )	{ PlaySample24khz( SixtyData,	0, SIXTY_SIZE 	, 1 ); seconds -=  60; }
		else if ( seconds >= 50  )	{ PlaySample24khz( FiftyData,	0, FIFTY_SIZE	, 1 ); seconds -=  50; }
		else if ( seconds >= 40  )	{ PlaySample24khz( FortyData,	0, FORTY_SIZE	, 1 ); seconds -=  40; }
		else if ( seconds >= 30  )	{ PlaySample24khz( ThirtyData,	0, THIRTY_SIZE	, 1 ); seconds -=  30; }
		else if ( seconds >= 20  )	{ PlaySample24khz( TwentyData,	0, TWENTY_SIZE	, 1 ); seconds -=  20; }

		// teens
		else if ( seconds >= 19  )	{ PlaySample24khz( NineData,	0, NINE_SIZE	, 1 ); seconds -=  19;
									  PlaySample24khz( TeenData,	0, TEEN_SIZE	, 1 );  }

		else if ( seconds >= 18  )	{ PlaySample24khz( EightData,	0, EIGHT_SIZE	, 1 ); seconds -=  18;
		 	 	 	 	 	 	 	  PlaySample24khz( TeenData,	0, TEEN_SIZE	, 1 );  }

		else if ( seconds >= 17  )	{ PlaySample24khz( SevenData,	0, SEVEN_SIZE	, 1 ); seconds -=  17;
									  PlaySample24khz( TeenData,	0, TEEN_SIZE	, 1 );  }

		else if ( seconds >= 16  )	{ PlaySample24khz( SixData,		0, SIX_SIZE		, 1 ); seconds -=  16;
		  	  	  	  	  	  	  	  PlaySample24khz( TeenData,	0, TEEN_SIZE	, 1 );  }

		else if ( seconds >= 15  )	{ PlaySample24khz( FifData,		0, FIF_SIZE		, 1 ); seconds -=  15;
		  	  	  	  	  	  	  	  PlaySample24khz( TeenData,	0, TEEN_SIZE	, 1 );  }

		else if ( seconds >= 14  )	{ PlaySample24khz( FourData,	0, FOUR_SIZE	, 1 ); seconds -=  14;
		  	  	  	  	  	  	  	  PlaySample24khz( TeenData,	0, TEEN_SIZE	, 1 );  }

		else if ( seconds >= 13  )	{ PlaySample24khz( ThirData,	0, THIR_SIZE	, 1 ); seconds -=  13;
									  PlaySample24khz( TeenData,	0, TEEN_SIZE	, 1 );  }
		// 10-12
		else if ( seconds >= 12  )	{ PlaySample24khz( TwelveData,	0, TWELVE_SIZE	, 1 ); seconds -=  12; }
		else if ( seconds >= 11  )	{ PlaySample24khz( ElevenData,	0, ELEVEN_SIZE	, 1 ); seconds -=  11; }
		else if ( seconds >= 10  )	{ PlaySample24khz( TenData,		0, TEN_SIZE		, 1 ); seconds -=  10; }

    	// ones place
		if 		( seconds >= 9  )	PlaySample24khz( NineData,		0, NINE_SIZE	, 1 );
		else if ( seconds >= 8  )	PlaySample24khz( EightData,		0, EIGHT_SIZE	, 1 );
		else if ( seconds >= 7  )	PlaySample24khz( SevenData,		0, SEVEN_SIZE	, 1 );
		else if ( seconds >= 6  )	PlaySample24khz( SixData,		0, SIX_SIZE		, 1 );
		else if ( seconds >= 5  )	PlaySample24khz( FiveData,		0, FIVE_SIZE	, 1 );
		else if ( seconds >= 4  )	PlaySample24khz( FourData,		0, FOUR_SIZE	, 1 );
		else if ( seconds >= 3  )	PlaySample24khz( ThreeData,		0, THREE_SIZE	, 1 );
		else if ( seconds >= 2  )	PlaySample24khz( TwoData,		0, TWO_SIZE		, 1 );
		else if ( seconds >= 1  )	PlaySample24khz( OneData,		0, ONE_SIZE		, 1 );
    }

    // play fractional seconds
    if( tens_place != 0 || hund_place != 0 || thou_place != 0 )
    {
		PlaySample24khz( PointData,	0, POINT_SIZE, 1 );

		// play tens place
		if 		( tens_place == 9  )	{ PlaySample24khz( NineData,	0, NINE_SIZE	, 1 );	play_tens = 1; }
		else if ( tens_place == 8  )	{ PlaySample24khz( EightData,	0, EIGHT_SIZE	, 1 );	play_tens = 1; }
		else if ( tens_place == 7  )	{ PlaySample24khz( SevenData,	0, SEVEN_SIZE	, 1 );	play_tens = 1; }
		else if ( tens_place == 6  )	{ PlaySample24khz( SixData,		0, SIX_SIZE		, 1 );	play_tens = 1; }
		else if ( tens_place == 5  )	{ PlaySample24khz( FiveData,	0, FIVE_SIZE	, 1 );	play_tens = 1; }
		else if ( tens_place == 4  )	{ PlaySample24khz( FourData,	0, FOUR_SIZE	, 1 );	play_tens = 1; }
		else if ( tens_place == 3  )	{ PlaySample24khz( ThreeData,	0, THREE_SIZE	, 1 );	play_tens = 1; }
		else if ( tens_place == 2  )	{ PlaySample24khz( TwoData,		0, TWO_SIZE		, 1 );	play_tens = 1; }
		else if ( tens_place == 1  )	{ PlaySample24khz( OneData,		0, ONE_SIZE		, 1 );	play_tens = 1; }
		else if ( tens_place == 0  )	{ PlaySample24khz( ZeroData,	0, ZERO_SIZE	, 1 );	play_tens = 1; }

		if( thou_place !=0 || hund_place != 0 )
		{
			// play hundredths place
			if 		( hund_place == 9  )	{ PlaySample24khz( NineData,	0, NINE_SIZE	, 1 ); play_hund = 1; }
			else if ( hund_place == 8  )	{ PlaySample24khz( EightData,	0, EIGHT_SIZE	, 1 ); play_hund = 1; }
			else if ( hund_place == 7  )	{ PlaySample24khz( SevenData,	0, SEVEN_SIZE	, 1 ); play_hund = 1; }
			else if ( hund_place == 6  )	{ PlaySample24khz( SixData,		0, SIX_SIZE		, 1 ); play_hund = 1; }
			else if ( hund_place == 5  )	{ PlaySample24khz( FiveData,	0, FIVE_SIZE	, 1 ); play_hund = 1; }
			else if ( hund_place == 4  )	{ PlaySample24khz( FourData,	0, FOUR_SIZE	, 1 ); play_hund = 1; }
			else if ( hund_place == 3  )	{ PlaySample24khz( ThreeData,	0, THREE_SIZE	, 1 ); play_hund = 1; }
			else if ( hund_place == 2  )	{ PlaySample24khz( TwoData,		0, TWO_SIZE		, 1 ); play_hund = 1; }
			else if ( hund_place == 1  )	{ PlaySample24khz( OneData,		0, ONE_SIZE		, 1 ); play_hund = 1; }
			else if ( hund_place == 0  )	{ PlaySample24khz( ZeroData,	0, ZERO_SIZE	, 1 ); play_hund = 1; }
		}

		if( thou_place != 0 )
		{
			// play thousandths place
			if 		( thou_place == 9  )	{ PlaySample24khz( NineData,	0, NINE_SIZE	, 1 ); play_thou = 1; }
			else if ( thou_place == 8  )	{ PlaySample24khz( EightData,	0, EIGHT_SIZE	, 1 ); play_thou = 1; }
			else if ( thou_place == 7  )	{ PlaySample24khz( SevenData,	0, SEVEN_SIZE	, 1 ); play_thou = 1; }
			else if ( thou_place == 6  )	{ PlaySample24khz( SixData,		0, SIX_SIZE		, 1 ); play_thou = 1; }
			else if ( thou_place == 5  )	{ PlaySample24khz( FiveData,	0, FIVE_SIZE	, 1 ); play_thou = 1; }
			else if ( thou_place == 4  )	{ PlaySample24khz( FourData,	0, FOUR_SIZE	, 1 ); play_thou = 1; }
			else if ( thou_place == 3  )	{ PlaySample24khz( ThreeData,	0, THREE_SIZE	, 1 ); play_thou = 1; }
			else if ( thou_place == 2  )	{ PlaySample24khz( TwoData,		0, TWO_SIZE		, 1 ); play_thou = 1; }
			else if ( thou_place == 1  )	{ PlaySample24khz( OneData,		0, ONE_SIZE		, 1 ); play_thou = 1; }
		}
    }

    // play percent, second, or seconds
    if( play_type == PLAY_PERCENT )
    {
    	PlaySample24khz( PercentData, 0, PERCENT_SIZE, 1 );
    }
    else if( play_seconds == 1 || play_tens == 1 || play_hund == 1 || play_thou == 1 )
    {
    	// handle the one and only one second case
    	if( seconds == 1 && tens_place == 0 && hund_place ==0 && thou_place == 0 )
    	{
    		if( play_type == PLAY_TIME ) PlaySample24khz( SecondData, 0, SECOND_SIZE, 1 );
    	}
    	else
    	{
    		if( play_type == PLAY_TIME ) PlaySample24khz( SecondsData, 0, SECONDS_SIZE, 1 );
    	}
    }
#endif
}

void PlaySpeed( int integer, int fractional )
{
	PlaySilence( 100, 1 );

#ifdef NUMBERS
    // play integer part of number first
	const int hund = (integer / 100) % 10;

	int integer_played = 0;

    if( hund > 0 )
    {
		switch( hund )
		{
			case 9:	{ PlaySample24khz( NineData,	0, NINE_SIZE,	1 ); break; }
			case 8: { PlaySample24khz( EightData,	0, EIGHT_SIZE,	1 ); break; }
			case 7: { PlaySample24khz( SevenData,	0, SEVEN_SIZE,	1 ); break; }
			case 6: { PlaySample24khz( SixData,		0, SIX_SIZE,	1 ); break; }
			case 5: { PlaySample24khz( FiveData,	0, FIVE_SIZE,	1 ); break; }
			case 4: { PlaySample24khz( FourData,	0, FOUR_SIZE,	1 ); break; }
			case 3: { PlaySample24khz( ThreeData,	0, THREE_SIZE,	1 ); break; }
			case 2: { PlaySample24khz( TwoData,		0, TWO_SIZE,	1 ); break; }
			case 1: { PlaySample24khz( OneData,		0, ONE_SIZE,	1 ); break; }
		}

		PlaySample24khz( HundredData, 0, HUNDRED_SIZE, 0 );

		integer -= hund * 100;
		integer_played = ( integer == 0 ) ? 1 : 0;
    }

    if( integer > 19 )
    {
    	const int multiple = integer / 10;

		switch( multiple )
		{
			case 9:	{ PlaySample24khz( NinetyData,	0, NINETY_SIZE,	 1 ); break; }
			case 8: { PlaySample24khz( EightyData,	0, EIGHTY_SIZE,  1 ); break; }
			case 7: { PlaySample24khz( SeventyData,	0, SEVENTY_SIZE, 1 ); break; }
			case 6: { PlaySample24khz( SixtyData,	0, SIXTY_SIZE,	 1 ); break; }
			case 5: { PlaySample24khz( FiftyData,	0, FIFTY_SIZE,	 1 ); break; }
			case 4: { PlaySample24khz( FortyData,	0, FORTY_SIZE,	 1 ); break; }
			case 3: { PlaySample24khz( ThirtyData,	0, THIRTY_SIZE,  1 ); break; }
			case 2: { PlaySample24khz( TwentyData,	0, TWENTY_SIZE,  1 ); break; }
		}

		integer -= multiple * 10;
		integer_played = ( integer == 0 ) ? 1 : 0;
    }

    if( integer_played == 0 )
    {
		switch( integer )
		{
			case 19:{ PlaySample24khz( NineData,	0, NINE_SIZE, 	1 ); PlaySample24khz( TeenData,	0, TEEN_SIZE, 	1 ); break;	}
			case 18:{ PlaySample24khz( EightData,	0, EIGHT_SIZE,	1 ); PlaySample24khz( TeenData,	0, TEEN_SIZE,	1 ); break;	}
			case 17:{ PlaySample24khz( SevenData,	0, SEVEN_SIZE,	1 ); PlaySample24khz( TeenData,	0, TEEN_SIZE,	1 ); break; }
			case 16:{ PlaySample24khz( SixData,		0, SIX_SIZE,	1 ); PlaySample24khz( TeenData,	0, TEEN_SIZE,	1 ); break;	}
			case 15:{ PlaySample24khz( FifData,		0, FIF_SIZE, 	1 ); PlaySample24khz( TeenData,	0, TEEN_SIZE,	1 ); break;	}
			case 14:{ PlaySample24khz( FourData,	0, FOUR_SIZE,	1 ); PlaySample24khz( TeenData,	0, TEEN_SIZE,	1 ); break;	}
			case 13:{ PlaySample24khz( ThirData,	0, THIR_SIZE,	1 ); PlaySample24khz( TeenData,	0, TEEN_SIZE,	1 ); break;	}
			case 12:{ PlaySample24khz( TwelveData,	0, TWELVE_SIZE, 1 ); break;	}
			case 11:{ PlaySample24khz( ElevenData,	0, ELEVEN_SIZE, 1 ); break;	}
			case 10:{ PlaySample24khz( TenData,		0, TEN_SIZE,	1 ); break;	}
			case 9:	{ PlaySample24khz( NineData,	0, NINE_SIZE,	1 ); break; }
			case 8: { PlaySample24khz( EightData,	0, EIGHT_SIZE,	1 ); break; }
			case 7: { PlaySample24khz( SevenData,	0, SEVEN_SIZE,	1 ); break; }
			case 6: { PlaySample24khz( SixData,		0, SIX_SIZE,	1 ); break; }
			case 5: { PlaySample24khz( FiveData,	0, FIVE_SIZE,	1 ); break; }
			case 4: { PlaySample24khz( FourData,	0, FOUR_SIZE,	1 ); break; }
			case 3: { PlaySample24khz( ThreeData,	0, THREE_SIZE,	1 ); break; }
			case 2: { PlaySample24khz( TwoData,		0, TWO_SIZE,	1 ); break; }
			case 1: { PlaySample24khz( OneData,		0, ONE_SIZE,	1 ); break; }
			case 0: { PlaySample24khz( ZeroData,	0, ZERO_SIZE,	1 ); break; }
		}
    }

    // play fractional part of number up to 3 digits if there is one
	if( fractional > 0 )
	{
		PlaySample24khz( PointData,	0, POINT_SIZE, 0 );

		char number[ 3 ];
		sprintf( number, "%03d", fractional );

		const int a = number[ 0 ];
		const int b = number[ 1 ];
		const int c = number[ 2 ];

		if		( a == 0x30 && b == 0x30 && c == 0x30 ) { PlayDigit( c - 0x30 ); } // 000
		else if ( a == 0x30 && b == 0x30 && c >= 0x31 ) { PlayDigit( 0 ); PlayDigit( 0 ); PlayDigit( c - 0x30 ); } // 001
		else if ( a == 0x30 && b >= 0x31 && c == 0x30 ) { PlayDigit( 0 ); PlayDigit( b - 0x30 ); } // 010
		else if ( a == 0x30 && b >= 0x31 && c >= 0x31 ) { PlayDigit( 0 ); PlayDigit( b - 0x30 ); PlayDigit( c - 0x30 ); } // 011
		else if ( a >= 0x31 && b >= 0x31 && c == 0x30 ) { PlayDigit( a - 0x30 ); PlayDigit( b - 0x30 ); } // 110
		else if ( a >= 0x31 && b == 0x30 && c >= 0x31 ) { PlayDigit( a - 0x30 ); PlayDigit( b - 0x30 ); PlayDigit( c - 0x30 ); } // 101
		else { PlayDigit( a - 0x30 ); PlayDigit( b - 0x30 ); PlayDigit( c - 0x30 ); } // 111
	}

	PlaySample24khz( MilesPerHourData,	0, MILES_PER_HOUR_SIZE, 0 );

	InitAudio();
#endif
}

void PlayDigit( int number )
{
#ifdef NUMBERS
	switch( number )
	{
		case 9:	{ PlaySample24khz( NineData,	0, NINE_SIZE,	1 ); break; }
		case 8: { PlaySample24khz( EightData,	0, EIGHT_SIZE,	1 ); break; }
		case 7: { PlaySample24khz( SevenData,	0, SEVEN_SIZE,	1 ); break; }
		case 6: { PlaySample24khz( SixData,		0, SIX_SIZE,	1 ); break; }
		case 5: { PlaySample24khz( FiveData,	0, FIVE_SIZE,	1 ); break; }
		case 4: { PlaySample24khz( FourData,	0, FOUR_SIZE,	1 ); break; }
		case 3: { PlaySample24khz( ThreeData,	0, THREE_SIZE,	1 ); break; }
		case 2: { PlaySample24khz( TwoData,		0, TWO_SIZE,	1 ); break; }
		case 1: { PlaySample24khz( OneData,		0, ONE_SIZE,	1 ); break; }
		case 0: { PlaySample24khz( ZeroData, 	0, ZERO_SIZE,	1 ); break; }
	}
#endif
}

void InitMenus( void )
{
	Menu_Array[DROP_GATE].menu_type		= WAIT_FOR_BUTTON;
	Menu_Array[DROP_GATE].context		= 0;
	Menu_Array[DROP_GATE].item_count	= 1;
	Menu_Array[DROP_GATE].current_item	= 0;
	SetMenuText( Menu_Array[DROP_GATE].caption,		"DROP GATE" );
	SetMenuText( Menu_Array[DROP_GATE].item[0],	"Press button!" );

	Menu_Array[ENERGIZE_MAGNET].menu_type	= WAIT_FOR_BUTTON;
	Menu_Array[ENERGIZE_MAGNET].context		= 0;
	Menu_Array[ENERGIZE_MAGNET].item_count	= 1;
	Menu_Array[ENERGIZE_MAGNET].current_item= 0;
	SetMenuText( Menu_Array[ENERGIZE_MAGNET].caption,	  "ENERGIZE MAGNET" );
	SetMenuText( Menu_Array[ENERGIZE_MAGNET].item[0], "Press button" );

	Menu_Array[GATE_DROPS].menu_type	= NO_INPUT;
	Menu_Array[GATE_DROPS].context		= 0;
	Menu_Array[GATE_DROPS].item_count	= 0;
	Menu_Array[GATE_DROPS].current_item = 0;
	SetMenuText( Menu_Array[GATE_DROPS].caption, "TOTAL GATE DROPS" );
	sprintf( Menu_Array[GATE_DROPS].item[0], "%d", Menu_Array[GATE_DROPS].context );

	Menu_Array[GATE_START_DELAY].menu_type		= EDIT_VALUE;
	Menu_Array[GATE_START_DELAY].context		= 0;
	Menu_Array[GATE_START_DELAY].item_count		= 1;
	Menu_Array[GATE_START_DELAY].current_item	= 0;
	SetMenuText( Menu_Array[GATE_START_DELAY].caption, "GATE START DELAY" );
	SetMenuText( Menu_Array[GATE_START_DELAY].item[0], "No Delay" );

	Menu_Array[GATE_DROPS_ON].menu_type		= EDIT_VALUE;
	Menu_Array[GATE_DROPS_ON].context		= 360;
	Menu_Array[GATE_DROPS_ON].item_count	= 1;
	Menu_Array[GATE_DROPS_ON].current_item	= 0;
	SetMenuText( Menu_Array[GATE_DROPS_ON].caption, "GATE DROPS ON" );
	sprintf( Menu_Array[GATE_DROPS_ON].item[0], "Green" );

	Menu_Array[AUX_1_CONFIG].menu_type		= EDIT_CHOICE;
	Menu_Array[AUX_1_CONFIG].context 		= AUX_DISABLED;
	Menu_Array[AUX_1_CONFIG].item_count 	= 5;
	Menu_Array[AUX_1_CONFIG].current_item	= 1;
	SetMenuText( Menu_Array[AUX_1_CONFIG].caption, 	"AUXILIARY 1" );
	SetMenuText( Menu_Array[AUX_1_CONFIG].item[0],	SPACES );
	SetMenuText( Menu_Array[AUX_1_CONFIG].item[AUX_DISABLED],	   "\1 Disabled \2" );
	SetMenuText( Menu_Array[AUX_1_CONFIG].item[AUX_GATE_SWITCH],   "\1 Gate Drop Switch \2" );
	SetMenuText( Menu_Array[AUX_1_CONFIG].item[AUX_TIME],		   "\1 Time \2" );
	SetMenuText( Menu_Array[AUX_1_CONFIG].item[AUX_TIME_SPEED],	   "\1 Time + Speed \2" );
	SetMenuText( Menu_Array[AUX_1_CONFIG].item[AUX_SENSOR_SPACING],"\1 Sensor Spacing \2" );
	ItemCopy( AUX_1_CONFIG, Menu_Array[AUX_1_CONFIG].context, 0, 1 );
	Aux_1_Sensor_Spacing = 12;

	Menu_Array[AUX_2_CONFIG].menu_type		= EDIT_CHOICE;
	Menu_Array[AUX_2_CONFIG].context 		= AUX_DISABLED;
	Menu_Array[AUX_2_CONFIG].item_count 	= 5;
	Menu_Array[AUX_2_CONFIG].current_item	= 1;
	SetMenuText( Menu_Array[AUX_2_CONFIG].caption, 	"AUXILIARY 2" );
	SetMenuText( Menu_Array[AUX_2_CONFIG].item[0],	SPACES );
	SetMenuText( Menu_Array[AUX_2_CONFIG].item[AUX_DISABLED],	   "\1 Disabled \2" );
	SetMenuText( Menu_Array[AUX_2_CONFIG].item[AUX_GATE_SWITCH],   "\1 Gate Drop Switch \2" );
	SetMenuText( Menu_Array[AUX_2_CONFIG].item[AUX_TIME],		   "\1 Time \2" );
	SetMenuText( Menu_Array[AUX_2_CONFIG].item[AUX_TIME_SPEED],	   "\1 Time + Speed \2" );
	SetMenuText( Menu_Array[AUX_2_CONFIG].item[AUX_SENSOR_SPACING],"\1 Sensor Spacing \2" );
	ItemCopy( AUX_2_CONFIG, Menu_Array[AUX_2_CONFIG].context, 0, 1 );
	Aux_2_Sensor_Spacing = 12;

	Menu_Array[AUX_1_SENSOR_SPACING].menu_type	= EDIT_CHOICE;
	Menu_Array[AUX_1_SENSOR_SPACING].context 		= 12;
	Menu_Array[AUX_1_SENSOR_SPACING].item_count 	= 1;
	Menu_Array[AUX_1_SENSOR_SPACING].current_item	= 1;
	SetMenuText( Menu_Array[AUX_1_SENSOR_SPACING].caption,	"AUX 1 SENSOR SPACING" );
	SetMenuText( Menu_Array[AUX_1_SENSOR_SPACING].item[0],"%d inches" );
	sprintf( Menu_Array[AUX_1_SENSOR_SPACING].item[0],    "%d inches", Menu_Array[AUX_1_SENSOR_SPACING].context );

	Menu_Array[AUX_2_SENSOR_SPACING].menu_type	= EDIT_CHOICE;
	Menu_Array[AUX_2_SENSOR_SPACING].context 		= 12;
	Menu_Array[AUX_2_SENSOR_SPACING].item_count 	= 1;
	Menu_Array[AUX_2_SENSOR_SPACING].current_item	= 1;
	SetMenuText( Menu_Array[AUX_2_SENSOR_SPACING].caption,	"AUX 2 SENSOR SPACING" );
	SetMenuText( Menu_Array[AUX_2_SENSOR_SPACING].item[0],"%d inches" );
	sprintf( Menu_Array[AUX_2_SENSOR_SPACING].item[0],    "%d inches", Menu_Array[AUX_2_SENSOR_SPACING].context );

	Menu_Array[TIMER_HISTORY].menu_type		= VIEW_LIST;
	Menu_Array[TIMER_HISTORY].context		= 0;
	Menu_Array[TIMER_HISTORY].item_count	= 1;
	Menu_Array[TIMER_HISTORY].current_item	= 0;
	SetMenuText( Menu_Array[TIMER_HISTORY].caption,     "TIMER HISTORY" );
	SetMenuText( Menu_Array[TIMER_HISTORY].item[0], "Nothing recorded" );
	ItemCopy( TIMER_HISTORY, Menu_Array[TIMER_HISTORY].context, 0, 1 );

	Menu_Array[CADENCE_VOLUME].menu_type	= EDIT_VALUE;
	Menu_Array[CADENCE_VOLUME].context		= 2;
	Menu_Array[CADENCE_VOLUME].item_count	= 1;
	Menu_Array[CADENCE_VOLUME].current_item = 0;
	SetMenuText( Menu_Array[CADENCE_VOLUME].caption, "CADENCE VOLUME" );
	sprintf( Menu_Array[CADENCE_VOLUME].item[0], "%d%%", Menu_Array[CADENCE_VOLUME].context );

	Menu_Array[AUDIO_IN_VOLUME].menu_type	= EDIT_VALUE;
	Menu_Array[AUDIO_IN_VOLUME].context		= 25;
	Menu_Array[AUDIO_IN_VOLUME].item_count	= 1;
	Menu_Array[AUDIO_IN_VOLUME].current_item = 0;
	SetMenuText( Menu_Array[AUDIO_IN_VOLUME].caption, "AUDIO IN VOLUME" );
	sprintf( Menu_Array[AUDIO_IN_VOLUME].item[0], "%d%%", Menu_Array[AUDIO_IN_VOLUME].context );

	Menu_Array[BATTERY_CONDITION].menu_type		= DISPLAY_VALUE;
	Menu_Array[BATTERY_CONDITION].context		= 100.0f * ReadBatteryCondition() / 3600;
	Menu_Array[BATTERY_CONDITION].item_count	= 1;
	Menu_Array[BATTERY_CONDITION].current_item	= 0;
	SetMenuText( Menu_Array[BATTERY_CONDITION].caption,	"BATTERY LEVEL" );
	sprintf( Menu_Array[BATTERY_CONDITION].item[0],	"%d%%", Menu_Array[BATTERY_CONDITION].context );

	Menu_Array[WIRELESS_REMOTE].menu_type		= EDIT_CHOICE;
	Menu_Array[WIRELESS_REMOTE].context 		= WIRELESS_REMOTE_ENABLED;
	Menu_Array[WIRELESS_REMOTE].item_count		= 2;
	Menu_Array[WIRELESS_REMOTE].current_item	= 1;
	SetMenuText( Menu_Array[WIRELESS_REMOTE].caption,	  "WIRELESS REMOTE" );
	SetMenuText( Menu_Array[WIRELESS_REMOTE].item[0], SPACES );
	SetMenuText( Menu_Array[WIRELESS_REMOTE].item[WIRELESS_REMOTE_DISABLED], "\1 Disabled \2" );
	SetMenuText( Menu_Array[WIRELESS_REMOTE].item[WIRELESS_REMOTE_ENABLED] , "\1 Enabled \2" );
	ItemCopy( WIRELESS_REMOTE, Menu_Array[WIRELESS_REMOTE].context, 0, 1 );

	Menu_Array[RELEASE_DEVICE].menu_type	= EDIT_CHOICE;
	Menu_Array[RELEASE_DEVICE].context 		= RELEASE_DEVICE_SOLENOID;
	Menu_Array[RELEASE_DEVICE].item_count 	= 3;
	Menu_Array[RELEASE_DEVICE].current_item	= RELEASE_DEVICE_SOLENOID;
	SetMenuText( Menu_Array[RELEASE_DEVICE].caption, 	"RELEASE DEVICE" );
	SetMenuText( Menu_Array[RELEASE_DEVICE].item[0], SPACES );
	SetMenuText( Menu_Array[RELEASE_DEVICE].item[RELEASE_DEVICE_SOLENOID], "\1 Solenoid \2" );
	SetMenuText( Menu_Array[RELEASE_DEVICE].item[RELEASE_DEVICE_MAGNET]  , "\1 Magnet \2" );
	SetMenuText( Menu_Array[RELEASE_DEVICE].item[RELEASE_DEVICE_AIR_RAM] , "\1 Air Ram \2" );
	ItemCopy( RELEASE_DEVICE, Menu_Array[RELEASE_DEVICE].context, 0, 1 );

	// initialize timing histories array
	int j, k;
	for( j = 0; j < MAX_TIMING_HISTORIES; j++ )
	{
		Timer_History[ j ].number			= 0;
		Timer_History[ j ].timer_ticks		= 0;
		Timer_History[ j ].speed_integer	= 0;
		Timer_History[ j ].speed_fractional	= 0;

		Timer_History[ j ].is_a_speed = 0;

		for( k = 0; k < DISPLAY_WIDTH; k++ )
		{
			Timer_History[ j ].time_string[ k ]		  = 0;
			Timer_History[ j ].speed_string[ k ]	  = 0;
			Timer_History[ j ].time_speed_string[ k ] = 0;
		}
	}
}

void ItemCopy( int menu, int source_index, int dest_index, int no_indicators )
{
	int i;

	for( i = 0; i < DISPLAY_WIDTH; i++ )
	{
		char source_char = Menu_Array[ menu ].item[ source_index ][ i ];

		if( no_indicators == 1 )
		{
			if( source_char == '\1' || source_char == '\2' )
				source_char = 0x20;
		}

		Menu_Array[ menu ].item[ dest_index ][ i ] = source_char;
	}
}
