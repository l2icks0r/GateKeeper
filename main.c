#include <stdio.h>
#include <string.h>
#include <math.h>

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
#include "stm32f4xx_flash.h"

#define SPLASH_TEXT
#define CADENCE
#define NUMBERS
#define MEASURE_BATTERY
//#define BATTERY_LOG

#include "TextStrings.h"

#include "FlashMemoryReserve.c"

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
//#include "VolumeAt.c"	// no room...
//#include "BatteryAt.c"
#include "LastTimeWas.c"
#include "MilesPerHour.c"
#include "At.c"
#endif



// LCD display
#define DISPLAY_WIDTH 20
#define DISPLAY_HEIGHT 2

static char display_memory		[ DISPLAY_HEIGHT ] [ DISPLAY_WIDTH ];
static char display_memory_cache[ DISPLAY_HEIGHT ] [ DISPLAY_WIDTH ];


// F4 initialization
void InitClocks				( void );
void InitPorts 				( void );
void InitAudio 				( void );
void InitLCD		 		( void );
void DefineCustomCharacters	( void );

void WriteLCD_Command		( const int data );
void WriteLCD_Data	 		( const int data );
void WriteLCD_Line	 		( const char * pString, const unsigned int line_no );
void WriteLCD_LineCentered	( const char * pString, const unsigned int line_no );
void UpdateLCD		 		( void );


// timers
void SysTick_Init( void );	static volatile uint64_t	SystemTick;
void StartTimers ( void );	static volatile uint32_t	Timer6_Tick;

// support functions for using input capture to measure speed
void EnableSpeedMeasurement	( void );
void DisableSpeedMeasurement( void );
void ResetSpeedMeasurement	( void );

void ShortDelay ( const unsigned int ticks );
void Delay		( const unsigned int milliseconds );


// random number generator (RNG)
static unsigned int GetRandomNumber( void );

// battery
int	  ReadBatteryCondition( void );
float BatteryLevel		  ( const unsigned int display_level );

// UI
void InitMenus( void );
void SetGateMenuState( void ); // sets appropriate menu upon power up or after gate drop based on release device
void ItemCopy( const unsigned int menu, const unsigned int source_index, const unsigned int dest_index, const unsigned int no_indicators );
void SetMenuText( char * menu, const char * text );
int  ReadInputs( int * inputs, int read_sensors );
void WaitForButtonUp();

// audio
void SetAttenuator( u16 data );
void PlaySilence  ( const unsigned int milliseconds, const unsigned int dont_abort );
void PlayTone	  ( const unsigned int milliseconds, const int16_t * data, const unsigned int length, const unsigned int dont_abort );

void PlaySample16khz( const int16_t * data, const unsigned int start, const unsigned int stop, const unsigned int dont_abort );
void PlaySample24khz( const int16_t * data, const unsigned int start, const unsigned int stop, const unsigned int dont_abort );

// play speech for number sequence
enum PLAY_TYPE { PLAY_TIME, PLAY_PERCENT };

void PlayTimeOrPercent( const unsigned int milliseconds, const unsigned int play_type );
void PlaySpeed( unsigned int integer, const unsigned int fractional );
void PlayDigit( const unsigned int number );

// play cadence, drive lights, power magnet/solenoid/air ram
void DropGate( int test );
void PlayRaiseGateAnimation( void );
void PlayDropGateAnimation( void );
// used for energizing the magnet and when the air ram is in action
void PlayGateUpTones( void );

// causes the solenoid to be shut off after specified time in milliseconds
void PulseSolenoid( const unsigned int pulse_time );
// turns power off after delay set in GATE_DROPS_ON
void StartGatePowerOffTimer( void );

// directly set gate ports
void SetGatePowerOff( void );
void SetGatePowerOn ( void );
void SetGatePowerUpProGate	( void );
void SetGatePowerDownProGate( void );

// reaction game
void DoReactionGame( const unsigned int player_count );

// timing
unsigned int CheckSensor( const unsigned int sensor );

void GetTimeFromTicks( const unsigned int ticks, unsigned int *minutes, unsigned int *seconds, unsigned int *tens_place, unsigned int *hund_place, unsigned int *thou_place );
void GetTimeString	 ( const unsigned int sensor_ticks, char *time_string );
void GetTimesString	 ( const unsigned int sensor_1, const unsigned int sensor_2, char *sensor_1_string, char *sensor_2_string );

void CalculateSpeed( const uint32_t speed_tick_delta, const unsigned int sensor_A, const unsigned int sensor_B, const unsigned int sensor_spacing,
					 unsigned int * elapsed_time, float * speed , unsigned int * speed_integer, unsigned int * speed_fractional );

void ProcessTimeAndSpeed( const unsigned int aux_config, const unsigned int elapsed_time, const int speed, const unsigned int speed_integer, const unsigned int speed_fractional );
void ProcessElapsedTimer( unsigned int aux_config, unsigned int sensor_A, unsigned int sensor_B );

int DoTimeAndDisabled ( const unsigned int aux_config, unsigned int sensor_A, unsigned int sensor_B, const char * timing_string, const char * elapsed_string );
int DoTimeAndSpeed	  ( const unsigned int aux1_option,
						const unsigned int sensor_1A, const unsigned int sensor_1B, const unsigned int sensor_2A, const unsigned int sensor_2B, unsigned int * timeout );
int DoSpeedAndDisabled( const unsigned int aux_config, const unsigned int sensor_A, const unsigned int sensor_B, const unsigned int sensor_spacing,
						const char * timing_string, const char * elapsed_string, unsigned int * timeout );
void DoSprintTimer	  ( const unsigned int aux_config );

void CopyTimerHistoryDown ( void );
void AddTimeToTimerHistory( const unsigned int aux_config, const unsigned int elapsed_time, const char *time_string );

void PrintElapsedTime( const unsigned int milliseconds, const unsigned int fractional, const unsigned int display_line );

void ClearTimingHistories( void );

// flash support
void SaveEverythingToFlashMemory	( int write_UUID_mask );
int  ReadEverythingFromFlashMemory	( void );

// get the flash sector number to use with FLASH_EraseSector
uint32_t GetFlashSector( uint32_t Address );

// startup light test
int LightTestCycle( const unsigned int allow_abort );
void ReadLightExitControlInput( int light_context );

void ValidateUUIDMask( void );



enum INPUTS { 	BUTTON_A 		= 0x0001,
				BUTTON_B 		= 0x0010,
				BUTTON_C 		= 0x0020,
				BUTTON_D 		= 0x0100,

				BUTTON_E 		= 0x8000,

				AUX_1_SENSOR_A	= 0x0080,
				AUX_1_SENSOR_B	= 0x0800,

				AUX_2_SENSOR_A	= 0x1000,
				AUX_2_SENSOR_B	= 0x2000 };

enum MENUS	   { DROP_GATE = 0,

				 RAISE_GATE,

				 ENERGIZE_MAGNET,

				 GATE_DROPS_ON,
				 GATE_START_DELAY,
				 GATE_START_WARNING,

				 AUX_1_CONFIG,
				 AUX_2_CONFIG,

				 TIMER_HISTORY,
				 CLEAR_TIMER_HISTORY,

				 AUTO_ANNOUNCE_TIMES,

				 TOTAL_GATE_DROPS,
				 ZERO_TOTAL_GATE_DROPS,
				 SAVE_TOTAL_GATE_DROPS,

				 CADENCE_VOLUME,
				 AUDIO_IN_VOLUME,

				 BATTERY_CONDITION,

				 TEST_LIGHTS,

				 REACTION_GAME,

				 WIRELESS_REMOTE,

				 RELEASE_DEVICE,

				 ANTI_SLAM,

				 MENUS_SIZE };

enum AUX_OPTIONS				{ AUX_DISABLED = 1, AUX_TIME = 2, AUX_TIME_SPEED = 3, AUX_SPRINT_TIMER = 4, AUX_GATE_SWITCH = 5, AUX_SENSOR_SPACING = 6, AUX_SENSOR_TYPE = 7,
								  AUX_ALIGN_SENSOR_A = 8, AUX_ALIGN_SENSOR_B = 9, AUX_LAP_COUNT = 10 };
enum REACTION_GAME_OPTIONS		{ REACTION_GAME_ONE_PLAYER = 1, REACTION_GAME_TWO_PLAYER = 2, REACTION_GAME_VIEW_STATS = 3, REACTION_GAME_CLEAR_STATS = 4 };
enum AUTO_ANNOUNCE_TIMES_OPTIONS{ AUTO_ANNOUNCE_TIMES_DISABLED = 1, AUTO_ANNOUNCE_TIMES_ENABLED = 2 };
enum WIRELESS_REMOTE_OPTIONS	{ WIRELESS_REMOTE_DISABLED = 1, WIRELESS_REMOTE_ENABLED = 2 };
enum RELEASE_DEVICE_OPTIONS 	{ RELEASE_DEVICE_SOLENOID = 1,	RELEASE_DEVICE_MAGNET = 2, RELEASE_DEVICE_AIR_RAM = 3, RELEASE_DEVICE_PROGATE = 4 };
enum TEST_LIGHTS_OPTIONS		{ TEST_LIGHTS_ALL_ON = 1, TEST_LIGHTS_RED = 2, TEST_LIGHTS_AMBER1 = 3, TEST_LIGHTS_AMBER2 = 4, TEST_LIGHTS_GREEN = 5, TEST_LIGHTS_UP_DOWN = 6, TEST_LIGHTS_DARK_LIGHT = 7, TEST_LIGHTS_RANDOM = 8 };
enum ANTI_SLAM_OPTIONS			{ ANTI_SLAM_DISABLED = 1, ANTI_SLAM_ENABLED = 2, ANTI_SLAM_RAISE_GATE = 3, ANTI_SLAM_DROP_GATE = 4, ANTI_SLAM_START_DELAY = 5, ANTI_SLAM_PULSE_WIDTH = 6, ANTI_SLAM_INCREMENT = 7 };


// sub-menu options for auxiliary sensor type
enum SENSOR_OPTIONS			{ RIBBON_SWITCH = 0, INFRARED = 1, LASER = 2, PROXIMITY = 3, WIRELESS = 4, SENSOR_OPTIONS_SIZE = 5 };

enum MENU_TYPE { NO_INPUT, DISPLAY_VALUE, WAIT_FOR_BUTTON, EDIT_VALUE, EDIT_CHOICE, VIEW_LIST };

#define MAX_MENU_ITEMS 16

struct MENU_DEFINITION
{
	int		menu_type;
	int		context;
	int		sub_context_1;
	int		sub_context_2;
	int		sub_context_3;
	int		sub_context_4;

	char 	caption[ DISPLAY_WIDTH ];

	char	item[ MAX_MENU_ITEMS ][ DISPLAY_WIDTH ];
	int 	item_count;
	int		current_item;
};

//                        1
//              01234567890123456789
#define SPACES "                    "
#define ZEROS { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }

enum DEVICE_STATE { STARTUP, RECHARGE, WAIT_FOR_USER, WAIT_FOR_SENSORS, WAIT_FOR_SPEED_TRAP };

enum GATE_POWER_STATE { POWER_OFF, POWER_ON };

// timing history size is 80 bytes
struct TIMING_DEFINITION
{
	unsigned int number;
	unsigned int elapsed_time;
	unsigned int speed_integer;
	unsigned int speed_fractional;

	char is_a_speed;
	char aux_port;

	char time_string		[ DISPLAY_WIDTH ];
	char speed_string		[ DISPLAY_WIDTH ];
	char time_speed_string	[ DISPLAY_WIDTH ];
};

// starting addresses for each sector of flash memory
#define ADDR_FLASH_SECTOR_0   0x08000000 // starting address of sector 0,  16 K
#define ADDR_FLASH_SECTOR_1   0x08004000 // starting address of sector 1,  16 K
#define ADDR_FLASH_SECTOR_2   0x08008000 // starting address of sector 2,  16 K
#define ADDR_FLASH_SECTOR_3   0x0800C000 // starting address of sector 3,  16 K
#define ADDR_FLASH_SECTOR_4   0x08010000 // starting address of sector 4,  64 K
#define ADDR_FLASH_SECTOR_5   0x08020000 // starting address of sector 5,  128 K
#define ADDR_FLASH_SECTOR_6   0x08040000 // starting address of sector 6,  128 K
#define ADDR_FLASH_SECTOR_7   0x08060000 // starting address of sector 7,  128 K
#define ADDR_FLASH_SECTOR_8   0x08080000 // starting address of sector 8,  128 K
#define ADDR_FLASH_SECTOR_9   0x080A0000 // starting address of sector 9,  128 K
#define ADDR_FLASH_SECTOR_10  0x080C0000 // starting address of sector 10, 128 K
#define ADDR_FLASH_SECTOR_11  0x080E0000 // starting address of sector 11, 128 K
#define ADDR_UUID ((uint32_t *)0x1FFF7A10)

// from http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/Babieigh.html
#define ADDR_SYST_CSR ((uint32_t *)0xE000E010) // address of SysTick Status Register
#define ADDR_SYST_RVR ((uint32_t *)0xE000E014) // address of SysTick Relload Value Register
#define ADDR_SYST_CVR ((uint32_t *)0xE000E018) // address of SysTick Current Value Register

#define UUID_VALIDATE_CHECKS  5

// define actual hardware memory addresses to read/write from flash (sector 1)
#define FLASH_SAVE_MEMORY_START 0x08004000
#define FLASH_SAVE_MEMORY_END   0x08007FFF

// 200 entries = 16000 bytes, using size to correspond to flash module sector size of 16k
#define MAX_TIMING_HISTORIES 	150
// used as a counter for each timing entry for identification - reset at 1 million
#define MAX_HISTORY_NUMBER	 	999999

// maximum time to wait between speed sensors
#define SENSOR_TIMEOUT_PERIOD	15000	// wait 1.5 seconds after speed sensor has triggered

// delay to wait before reading a sensor again after target has passed through when counting more than one lap
#define SENSOR_RESCAN_DELAY 	20000	// wait 2.0 seconds after target trips sensor

// index for main switch statement
static int Menu_Index = 0;

// reserve memory to store all menus
static struct MENU_DEFINITION Menu_Array[ MENUS_SIZE ];

// array to reference text from auxiliary options
static char Sensor_Text[ SENSOR_OPTIONS_SIZE ][ DISPLAY_WIDTH ];

// reserve memory to store history of all timings
static struct TIMING_DEFINITION Timer_History[ MAX_TIMING_HISTORIES ];
static unsigned int Timer_History_Index  = 0;
static unsigned int Timer_History_Number = 0;

// stores the index of the menu currently being processed
static int Device_State = STARTUP;

// variable to handle interrupting when a sample is being played
static unsigned int Cadence_Cancelled = 0;
static unsigned int Cancel_Timing	  = 0;

// variables used to communicated from main code to interrupt code
static unsigned int Gate_Drop_Delay			= 0;
static unsigned int Pulse_Solenoid 			= 0;
static unsigned int Solenoid_Pulse_Time 	= 0;
static unsigned int Gate_Power_Off_Pending	= 0;
static unsigned int Gate_Power_State 		= 0;

// special flag set at beginning of DropGate(), cleared at end of DropGate() - used in Timer 6 IRQ
static unsigned int Dropping_Gate = 0;

// anti-slam variables used by timer 6
static int	 Anti_Slam_Active		= 0;
static float Anti_Slam_Start_Delay	= 0.0f;
static float Anti_Slam_Pulse_Width  = 0.0f;
static float Anti_Slam_Increment 	= 0.0f;
static int	 Anti_Slam_Gate_Up 		= 0;

// timing ticks at 84MHz for speeds
static uint32_t Aux_1_Speed_Tick_Delta = 0;
static uint32_t Aux_2_Speed_Tick_Delta = 0;

// sensor ticks set by timer 6 and cleared in DropGate()
static unsigned int Timing_Active = 0;

static unsigned int Aux_1_Sensor_A_Tick = 0;
static unsigned int Aux_1_Sensor_B_Tick = 0;
static unsigned int Aux_2_Sensor_A_Tick = 0;
static unsigned int Aux_2_Sensor_B_Tick = 0;

// configuration for the distance between the two sensors when measuring speed
static unsigned int Aux_1_Sensor_Spacing = 0;
static unsigned int Aux_2_Sensor_Spacing = 0;

// counter for number of times that auxiliary is polled
static unsigned int Aux_1_Infrared_Poll	 = 0;
static unsigned int Aux_2_Infrared_Poll	 = 0;

// counter to compare against the number of laps that is set in UI
static unsigned int Aux_1_Lap_Counter	 = 0;
static unsigned int Aux_2_Lap_Counter	 = 0;

// amount of time to wait after a sensor has been tripped (used for lap mode)
static unsigned int Aux_1_Rescan_Delay	 = 0;
static unsigned int Aux_2_Rescan_Delay	 = 0;

// battery related
static float Starting_Charge_Level = 0.0f;
static float Charge_Change 		   = 0.0f;

// reaction game
static unsigned int Start_Reaction_Timing  = 0;

static unsigned int Reaction_Game_P1_BEST  = 0;
static unsigned int Reaction_Game_P1_AVG   = 0;
static unsigned int Reaction_Game_P1_WORST = 0;

static unsigned int Reaction_Game_P2_BEST  = 0;
static unsigned int Reaction_Game_P2_AVG   = 0;
static unsigned int Reaction_Game_P2_WORST = 0;

// debug for light test
static unsigned short Light_Code = 0x8888;

static unsigned int UUID_Mask  = 0;
static unsigned int UUID_Check = 0;

#ifdef BATTERY_LOG
// temp code to capture battery decay profile
static unsigned int Battery_Levels[ 100 ];
#endif



int main( void )
{
#ifdef BATTERY_LOG
	int i = 0;
	for(; i < 100; i++ ) Battery_Levels[ i ] = 0;
#endif

	SystemInit();  // line 221 of system_stm32f4xx.c

	InitClocks();

	InitPorts();

	StartTimers(); // system ticks

	InitLCD();

	InitTextStrings();

	InitMenus();

	Starting_Charge_Level = 0;
	Charge_Change = 0;

	SysTick_Init();

	InitAudio();

	// do initial save of settings if this is the first time the code has ran
	if( ReadEverythingFromFlashMemory() == -1 ) SaveEverythingToFlashMemory( 1 );

	ValidateUUIDMask();

#ifdef MEASURE_BATTERY
	BatteryLevel( 1 );
#endif

	// enable gates
 	GPIO_ResetBits( GPIOA, GPIO_Pin_3 );	// gate drive enable

	static int inputs = 0;

	SetGateMenuState();

	// called to ignore first result which is false
	ReadInputs( & inputs, 1 );
	ReadInputs( & inputs, 1 );


	// the one and only main loop
	while ( 1 )
	{
		switch ( Device_State )
		{
			case STARTUP:
			{
				// do not permit usage with battery voltage at less than ~12vdc
#ifdef MEASURE_BATTERY
				if( BatteryLevel( 0 ) < 290000.0f )
				{
					Device_State = RECHARGE;
					break;
				}
#endif
				// initialize attenuator
				SetAttenuator( 0x41FF );		// write TCON (enable all)
				SetAttenuator( 0x1000 | 50 );	// write pot 1 (pot 1 = STM32 audio)
				SetAttenuator( 0x1000 | 50 );	// write pot 1 (pot 1 = STM32 audio)
				SetAttenuator( 0x0000 | (128 * Menu_Array[ AUDIO_IN_VOLUME ].context) / 100 ); // write pot 0 (pot 0 = aux in audio)

				GPIO_SetBits( GPIOC, GPIO_Pin_0 );

#ifdef SPLASH_TEXT
				// decoy text
				const char splash_0[] = "** RRP BMX GATES **";
				const char splash_1[] = "Epicenter v0.9.5";
				int blah = strlen( splash_0 ) + strlen( splash_1 );
				blah++;

				// write splash text
				WriteLCD_LineCentered( Splash_Text_0, 0 );
				WriteLCD_LineCentered( Splash_Text_1, 1 );
				UpdateLCD();

				LightTestCycle( 0 );

				Delay( 2000 );
#endif

				const int volume = (128 * Menu_Array[ CADENCE_VOLUME ].context) / 100;
				SetAttenuator( 0x1000 | volume );

				// go to next menu
				Device_State = WAIT_FOR_USER;
				break;
			}

			case RECHARGE:
			{
				Starting_Charge_Level = 0;
				Charge_Change = 0;

				SetAttenuator( 0x1000 );
				SetAttenuator( 0x0000 );

				while( BatteryLevel( 1 ) < 310000.0f ); // don't function until battery is at least 40% charge

				Device_State = STARTUP;
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
					WriteLCD_LineCentered( Menu_Array[ Menu_Index ].caption, 0 );
					WriteLCD_LineCentered( Menu_Array[ Menu_Index ].item[ 0 ], 1 );
					UpdateLCD();

					do
					{
#ifdef MEASURE_BATTERY
						if( ReadBatteryCondition() < 2900 ) // permit running battery down to 0%
						{
							Device_State = RECHARGE;
							break;
						}
#endif
						// update battery condition in realtime if battery condition menu is the current menu being displayed
						if( Menu_Index == BATTERY_CONDITION )
						{
							float battery_level = 0;
							int a = 0;
							for(; a < 300; a++ )	battery_level += (float)ReadBatteryCondition() * 100.0f;
							battery_level /= 300;

							float voltage 	 = 11.5f + ((13.2f - 11.5f) * (((100000.0f - ( 360000.0f - battery_level )) / 1000.0f) / 100.0f));

							float adjustment = (0.0725f * ((70000.0f - ( 356000.0f - battery_level )) / 70000.0f));

							voltage += adjustment;

							int voltage_integer		= (int)voltage;
							int voltage_fractional	= (int)(voltage * 100.0f) % 100;

							battery_level = (70000.0f - ( 356000.0f - battery_level )) / 700.0f;
							battery_level = ( battery_level > 100 ) ? 100 : battery_level;
							battery_level = ( battery_level <   0 ) ?   0 : battery_level;

							Menu_Array[ BATTERY_CONDITION ].context = (int) battery_level;

							if( voltage_fractional > 9 )
								sprintf( Menu_Array[ BATTERY_CONDITION ].item[ 0 ], "%d%% = %d.%dvdc", Menu_Array[ BATTERY_CONDITION ].context, voltage_integer, voltage_fractional );
							else
								sprintf( Menu_Array[ BATTERY_CONDITION ].item[ 0 ], "%d%% = %d.0%dvdc", Menu_Array[ BATTERY_CONDITION ].context, voltage_integer, voltage_fractional );

#ifdef BATTERY_LOG
							int battery_index = 100 - (int)battery_level;
							Battery_Levels[ battery_index ] += 1;

							if( (GPIOB->IDR & 0x0080) == 0 || ( ReadBatteryCondition() < 2500 ) )
							{
								FLASH_Unlock();

								FLASH_ClearFlag( FLASH_FLAG_EOP	   | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
											  	 FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR| FLASH_FLAG_PGSERR );

								FLASH_EraseSector( GetFlashSector( FLASH_SAVE_MEMORY_START ), VoltageRange_3 );

								uint32_t write_address = FLASH_SAVE_MEMORY_START;

								uint32_t *p_data = (uint32_t *) Battery_Levels;

								uint32_t stop_address = write_address + (sizeof( Battery_Levels ));

								for( i = 0; write_address < stop_address; i++ )
								{
									FLASH_Status flash_status = FLASH_ProgramWord( write_address, *p_data );

									if( flash_status == FLASH_ERROR_PROGRAM )
									{
										WriteLCD_LineCentered( "Flash_ProgramWord", 0 );
										WriteLCD_LineCentered( "failed", 0 );
										UpdateLCD();
									}

									if( flash_status == FLASH_COMPLETE )
									{
										p_data++;
										write_address += 4;
									}
								}

								FLASH_Lock();

								WriteLCD_LineCentered( "Battery Levels", 0 );
								WriteLCD_LineCentered( "Saved", 1 );
								UpdateLCD();

								while(1);
							}
#endif

							// write condition to bottom line and update the display to show new condition
							WriteLCD_LineCentered( Menu_Array[ BATTERY_CONDITION ].item[ 0 ], 1 );
							UpdateLCD();
						}

						// check for any input being triggered, only returns one result
						encoder_delta = ReadInputs( &inputs, 0 );

						if( inputs == AUX_1_SENSOR_A || inputs == AUX_1_SENSOR_B || inputs == AUX_2_SENSOR_A || inputs == AUX_2_SENSOR_B )
							inputs = 0;

						// if no input then there is nothing to do
						if( encoder_delta == 0 && inputs == 0 ) continue;

						Menu_Index += encoder_delta;

						// make sure that index is within the array of menus
						if( Menu_Index == MENUS_SIZE )
							Menu_Index--;
						else if( Menu_Index < 0)
							Menu_Index = 0;

						// determine what menu to be one based on if the gate is up/down and what release device is being used
						if( Gate_Power_State == POWER_OFF )
						{
							if( encoder_delta < 0 )
							{
								if( Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_MAGNET )
								{
									if( Menu_Index == DROP_GATE || Menu_Index == RAISE_GATE )
									{
										Menu_Index = ENERGIZE_MAGNET;
									}
								}
								else if( (Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_AIR_RAM) || (Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_PROGATE) )
								{
									if( Menu_Index == DROP_GATE || Menu_Index == ENERGIZE_MAGNET )
									{
										Menu_Index = RAISE_GATE;
									}
								}
								else if( Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_SOLENOID )
								{
									if( Menu_Index == RAISE_GATE || Menu_Index == ENERGIZE_MAGNET )
									{
										Menu_Index = DROP_GATE;
									}
								}
							}
							else if( encoder_delta > 0 )
							{
								if( Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_MAGNET )
								{
									if( Menu_Index == DROP_GATE || Menu_Index == RAISE_GATE )
									{
										Menu_Index = GATE_DROPS_ON;
									}
								}
								else if( (Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_AIR_RAM) || (Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_PROGATE) )
								{
									if( Menu_Index == DROP_GATE || Menu_Index == ENERGIZE_MAGNET )
									{
										Menu_Index = GATE_DROPS_ON;
									}
								}
								else if( Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_SOLENOID )
								{
									if( Menu_Index == RAISE_GATE || Menu_Index == ENERGIZE_MAGNET )
									{
										Menu_Index = GATE_DROPS_ON;
									}
								}
							}
						}
						else if( Gate_Power_State == POWER_ON )
						{
							if( encoder_delta < 0 )
							{
								if( Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_MAGNET )
								{
									if( Menu_Index == RAISE_GATE || Menu_Index == ENERGIZE_MAGNET )
									{
										Menu_Index = DROP_GATE;
									}
								}
								else if( (Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_AIR_RAM) || (Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_PROGATE) )
								{
									if( Menu_Index == RAISE_GATE || Menu_Index == ENERGIZE_MAGNET )
									{
										Menu_Index = DROP_GATE;
									}
								}
								else if( Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_SOLENOID )
								{
									if( Menu_Index == RAISE_GATE || Menu_Index == ENERGIZE_MAGNET )
									{
										Menu_Index = DROP_GATE;
									}
								}
							}
							else if( encoder_delta > 0 )
							{
								if( Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_MAGNET )
								{
									if( Menu_Index == RAISE_GATE || Menu_Index == ENERGIZE_MAGNET )
									{
										Menu_Index = GATE_DROPS_ON;
									}
								}
								else if( (Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_AIR_RAM) || (Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_PROGATE) )
								{
									if( Menu_Index == RAISE_GATE || Menu_Index == ENERGIZE_MAGNET )
									{
										Menu_Index = GATE_DROPS_ON;
									}
								}
								else if( Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_SOLENOID )
								{
									if( Menu_Index == RAISE_GATE || Menu_Index == ENERGIZE_MAGNET )
									{
										Menu_Index = GATE_DROPS_ON;
									}
								}
							}
						}

						// skip CLEAR_TIMER_HISTORY if timer history is empty
						if( Menu_Index == CLEAR_TIMER_HISTORY && Timer_History_Index == 0 )
						{
							if( encoder_delta > 0 ) Menu_Index = AUTO_ANNOUNCE_TIMES;
							else					Menu_Index = TIMER_HISTORY;
						}
						// skip ZERO_TOTAL_GATE_DROPS if gate drop count is zero
						else if( Menu_Index == ZERO_TOTAL_GATE_DROPS && Menu_Array[ TOTAL_GATE_DROPS ].context == 0 )
						{
							if( encoder_delta > 0 ) Menu_Index = SAVE_TOTAL_GATE_DROPS;
							else					Menu_Index = TOTAL_GATE_DROPS;
						}
						else if( Menu_Index == RELEASE_DEVICE && Gate_Power_State == POWER_ON )
						{
							Menu_Index = WIRELESS_REMOTE;
						}
						else if( Menu_Index == ANTI_SLAM )
						{
							if( (Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_AIR_RAM) || (Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_PROGATE) )
							{
								Menu_Index = ANTI_SLAM;
							}
							else if( Gate_Power_State == POWER_ON )
							{
								Menu_Index = WIRELESS_REMOTE;
							}
							else
							{
								Menu_Index = RELEASE_DEVICE;
							}
						}

						// if the encoder wheel moved then display the new menu text
						if( encoder_delta != 0 )
						{
							// update the display with the currently selected menu
							WriteLCD_LineCentered( Menu_Array[ Menu_Index ].caption, 0 );
							WriteLCD_LineCentered( Menu_Array[ Menu_Index ].item[ 0 ], 1 );
							UpdateLCD();
						}

						// if any sensors trip and aux port is configured as a switch then drop/raise the gate
						if( ( Menu_Array[ AUX_1_CONFIG ].context == AUX_GATE_SWITCH && ((inputs == AUX_1_SENSOR_A) || (inputs == AUX_1_SENSOR_B)) ) ||
						    ( Menu_Array[ AUX_2_CONFIG ].context == AUX_GATE_SWITCH && ((inputs == AUX_2_SENSOR_A) || (inputs == AUX_2_SENSOR_B)) ) )
						{
							SetGateMenuState();
						}
						else
						{	// filter sensor inputs
							if( inputs == AUX_1_SENSOR_A || inputs == AUX_1_SENSOR_B || inputs == AUX_2_SENSOR_A || inputs == AUX_2_SENSOR_B )
								inputs = 0;
						}

						// skip processing the wireless remote if it is disabled
						if( Menu_Array[ WIRELESS_REMOTE ].context == WIRELESS_REMOTE_DISABLED )
						{
							if( ((inputs^BUTTON_A) == 0) || ((inputs^BUTTON_B) == 0) || ((inputs^BUTTON_C) == 0) || ((inputs^BUTTON_D) == 0) )
							{
								inputs = 0;
								continue;
							}
						}

					} while ( inputs == 0 );

					// process button presses from RF keyfob remote
					if( inputs == BUTTON_A )
					{
						SetGateMenuState();
					}
					else if( inputs == BUTTON_B )
					{	// say last time was time, last time was time + mph, or just mph
#ifdef NUMBERS
						Cadence_Cancelled = 0; // TODO: THIS IS A BAND AID FIX!!!! Figure out what the fuck is going on here!

						if( Timer_History[ 0 ].elapsed_time > 0 )
						{
							PlaySilence( 100, 1 );
							PlaySample24khz( LastTimeWasData, 0, LAST_TIME_WAS_SIZE, 1 );
							PlayTimeOrPercent( Timer_History[ 0 ].elapsed_time / 10, PLAY_TIME );

							if( Timer_History[ 0 ].is_a_speed != 0 )
							{
								PlaySample24khz( AtData, 0, AT_SIZE, 1 );
								PlaySpeed( Timer_History[ 0 ].speed_integer, Timer_History[ 0 ].speed_fractional );
							}
						}
						else if( Timer_History[ 0 ].is_a_speed != 0 )
						{
							PlaySpeed( Timer_History[ 0 ].speed_integer, Timer_History[ 0 ].speed_fractional );
						}
						InitAudio();
#endif
						continue;
					}
					else if( inputs == BUTTON_C )
					{
						// TODO: Add functionality for button C, decrease cadence volume by 10% ?
						continue;
					}
					else if( inputs == BUTTON_D )
					{
						// cycle through cadence volume levels of 10%, 20%, 40%, 60%, 80%, 100%
						const int volume = Menu_Array[ CADENCE_VOLUME ].context;

						if( volume < 10 ) Menu_Array[ CADENCE_VOLUME ].context	  	= 10;
						else if( volume < 20 ) Menu_Array[ CADENCE_VOLUME ].context = 20;
						else if( volume < 40 ) Menu_Array[ CADENCE_VOLUME ].context = 40;
						else if( volume < 60 ) Menu_Array[ CADENCE_VOLUME ].context = 60;
						else if( volume < 80 ) Menu_Array[ CADENCE_VOLUME ].context = 80;
						else Menu_Array[ CADENCE_VOLUME ].context = 10;

						sprintf( Menu_Array[ CADENCE_VOLUME ].item[ 0 ], "%d%%", Menu_Array[ CADENCE_VOLUME ].context );

						char line1[ DISPLAY_WIDTH ];
						sprintf( line1, "%d%%", Menu_Array[ CADENCE_VOLUME ].context );
						WriteLCD_LineCentered( line1, 1 );
						UpdateLCD();

						int cadence_volume = (128 * Menu_Array[ CADENCE_VOLUME ].context) / 100;

						SetAttenuator( 0x1000 | cadence_volume );
#ifdef NUMBERS
						PlaySilence( 100, 0 );
//						PlaySample24khz( VolumeAtData, 0, VOLUME_AT_SIZE, 1 );
						PlayTimeOrPercent( Menu_Array[ CADENCE_VOLUME ].context * 60 * 1000, PLAY_PERCENT );
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
					switch ( Menu_Index )
					{
						case DROP_GATE:
						{
							if( inputs != BUTTON_E && Menu_Array[ WIRELESS_REMOTE ].context == WIRELESS_REMOTE_DISABLED )
								break;

							if( Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_MAGNET && Gate_Power_State == POWER_OFF )
							{
								WriteLCD_LineCentered( "Magnet is not", 0 );
								WriteLCD_LineCentered( "energized", 1 );
							    UpdateLCD();

							    Delay( 3000 );
							    break;
							}

							if( Menu_Array[ GATE_START_DELAY ].context > 0 )
							{
								WriteLCD_LineCentered( "Gate dropping in", 0 );
								char line[ DISPLAY_WIDTH ] = SPACES;

								unsigned int end_time = Timer6_Tick + Menu_Array[ GATE_START_DELAY ].context * 10000 + 9999;
#ifdef NUMBERS
								int warning_played = 0;
#endif
								while( Timer6_Tick < end_time )
								{
									const int seconds = ( (end_time - Timer6_Tick) / 10000 );

									if( seconds == 0 ) break;

									if( seconds > 60 )
									{
										PrintElapsedTime( end_time - Timer6_Tick, 0, 1 );
									}
									else if( seconds > 1 )
									{
										sprintf( line, "%d seconds", seconds );
										WriteLCD_LineCentered( line, 1 );
									}
									else
									{
										sprintf( line, "%d second", seconds );
										WriteLCD_LineCentered( line, 1 );
									}

									UpdateLCD();
#ifdef NUMBERS
									const int warning_time = Menu_Array[ GATE_START_WARNING ].context;

									if( warning_played == 0 && warning_time != 0 && seconds == warning_time )
									{
										PlaySilence( 100, 1 );
										PlayTimeOrPercent( 1000 * warning_time, PLAY_TIME  );
										InitAudio();

										warning_played = 1;
									}
#endif
									ReadInputs( &inputs, 0 );

									// bypass delay if encoder wheel button
									if( inputs == BUTTON_E  )
										break;
								}
							}

							PlayDropGateAnimation();
							DropGate( 0 );

// Todo: address saving the number of gate drops in next hardware revision, for now save memory handles this
// the only reason to call this is to save the total gate drops
//							SaveEverythingToFlashMemory( 0 );
							ReadInputs( &inputs, 0 );
							inputs = 0;

							ValidateUUIDMask();

							// display timing infos if either AUX port is set to measure time
							if( Menu_Array[ AUX_1_CONFIG ].context == AUX_TIME ||
								Menu_Array[ AUX_1_CONFIG ].context == AUX_TIME_SPEED ||
								Menu_Array[ AUX_2_CONFIG ].context == AUX_TIME ||
								Menu_Array[ AUX_2_CONFIG ].context == AUX_TIME_SPEED )
							{
								Device_State = WAIT_FOR_SENSORS;
							}

							SetGateMenuState();

							break;
						}

						case RAISE_GATE:
						{
							WriteLCD_LineCentered( "RAISE GATE", 0 );
							WriteLCD_LineCentered( "Playing Warning", 1 );
							UpdateLCD();

							PlayGateUpTones();

							if( Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_PROGATE )
								SetGatePowerUpProGate();
							else
								SetGatePowerOn();

							PlayRaiseGateAnimation();

							Delay( 400 );

							Menu_Index = DROP_GATE;

							break;
						}

						case ENERGIZE_MAGNET:
						{
							if( Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_MAGNET )
							{
								WriteLCD_LineCentered( Menu_Array[ ENERGIZE_MAGNET ].caption, 0 );
								WriteLCD_LineCentered( "magnet is on", 1 );
								UpdateLCD();

								SetGatePowerOn();

								PlayGateUpTones();

								Menu_Index = DROP_GATE;
							}
							break;
						}

						case GATE_DROPS_ON:
						{
							while( 1 )
							{
								unsigned int delay = Menu_Array[ GATE_DROPS_ON ].context;

								// there is no support for floating point numbers in sprintf, "%f" will not print anything
								// so do some retarded formatting
								if( delay < 120 )
								{
									if( delay == 0 )
										sprintf( Menu_Array[ GATE_DROPS_ON ].item[ 0 ], "\1 Red \2" );
									else
									{
										if( delay == 5 )
											sprintf( Menu_Array[ GATE_DROPS_ON ].item[ 0 ], "\1 Red + 0.00%d \2", delay );
										else if( delay < 100 )
											sprintf( Menu_Array[ GATE_DROPS_ON ].item[ 0 ], "\1 Red + 0.0%d \2", delay );
										else
											sprintf( Menu_Array[ GATE_DROPS_ON ].item[ 0 ], "\1 Red + 0.%d \2", delay );
									}

									WriteLCD_LineCentered( Menu_Array[ GATE_DROPS_ON ].item[ 0 ], 1 );
								}
								else if( delay < 240 )
								{
									if( delay == 120 )
										sprintf( Menu_Array[ GATE_DROPS_ON ].item[ 0 ], "\1 1st Yellow \2" );
									else
									{
										if( delay == 125 )
											sprintf( Menu_Array[ GATE_DROPS_ON ].item[ 0 ], "*1st Yellow + 0.00%d\2", (delay - 120) );
										else if( (delay - 120) < 100 )
											sprintf( Menu_Array[ GATE_DROPS_ON ].item[ 0 ], "*1st Yellow + 0.0%d0", (delay - 120) );
										else
											sprintf( Menu_Array[ GATE_DROPS_ON ].item[ 0 ], "*1st Yellow + 0.%d*", (delay - 120) );
									}

									Menu_Array[ GATE_DROPS_ON ].item[ 0 ][ 0 ] = '\1';
									Menu_Array[ GATE_DROPS_ON ].item[ 0 ][ strlen( Menu_Array[ GATE_DROPS_ON ].item[ 0 ] ) - 1 ] = '\2';
								}
								else if( delay < 360 )
								{
									if( delay == 240 )
										sprintf( Menu_Array[ GATE_DROPS_ON ].item[ 0 ], "\1 2nd Yellow \2" );
									else
									{
										if( delay == 245 )
											sprintf( Menu_Array[ GATE_DROPS_ON ].item[ 0 ], "*2nd Yellow + 0.00%d\2", (delay - 240) );
										else if( (delay - 240) < 100 )
											sprintf( Menu_Array[ GATE_DROPS_ON ].item[ 0 ], "*2nd Yellow + 0.0%d0", (delay - 240) );
										else
											sprintf( Menu_Array[ GATE_DROPS_ON ].item[ 0 ], "*2nd Yellow + 0.%d*", (delay - 240) );
									}

									Menu_Array[ GATE_DROPS_ON ].item[ 0 ][ 0 ] = '\1';
									Menu_Array[ GATE_DROPS_ON ].item[ 0 ][ strlen( Menu_Array[ GATE_DROPS_ON ].item[ 0 ] ) - 1 ] = '\2';
								}
								else
								{
									sprintf( Menu_Array[ GATE_DROPS_ON ].item[ 0 ], "\1 Green \2" );
								}

								WriteLCD_LineCentered( Menu_Array[ GATE_DROPS_ON ].item[ 0 ], 1 );
								UpdateLCD();

								// read controls
								do
								{	encoder_delta = ReadInputs( &inputs, 0 );

								} while ( encoder_delta == 0 && inputs == 0 );

								// change menu value
								if( encoder_delta != 0 )
								{
									Menu_Array[ GATE_DROPS_ON ].context += encoder_delta * 5;

									if( Menu_Array[ GATE_DROPS_ON ].context < 0 )
										Menu_Array[ GATE_DROPS_ON ].context = 0;

									else if( Menu_Array[ GATE_DROPS_ON ].context > 360 )
										Menu_Array[ GATE_DROPS_ON ].context = 360;

									WriteLCD_LineCentered( Menu_Array[ GATE_DROPS_ON ].item[ 0 ], 1 );
									UpdateLCD();
									continue;
								}

								// check for exit
								WaitForButtonUp();

								// strip indicators
								int i = 0;
								for(; i < strlen(Menu_Array[ GATE_DROPS_ON ].item[ 0 ]); i++)
								{
									if( Menu_Array[ GATE_DROPS_ON ].item[ 0 ][ i ] == '\1' || Menu_Array[ GATE_DROPS_ON ].item[ 0 ][ i ] == '\2' )
										Menu_Array[ GATE_DROPS_ON ].item[ 0 ][ i ] = 0x20;
								}
								WriteLCD_LineCentered( Menu_Array[ GATE_DROPS_ON ].item[ 0 ], 1 );
								UpdateLCD();

								SaveEverythingToFlashMemory( 0 );
								ReadInputs( &inputs, 0 );
								inputs = 0;

								break;
							}
							break;
						}

						case GATE_START_DELAY:
						case GATE_START_WARNING:
						{
							while( 1 )
							{
								if( Menu_Array[ Menu_Index ].context == 0 )
								{
									if( Menu_Index == GATE_START_DELAY )
										sprintf( Menu_Array[ Menu_Index ].item[ 0 ], "\1 No Delay \2");
									else
										sprintf( Menu_Array[ Menu_Index ].item[ 0 ], "\1 No Warning \2");
								}
								else if( Menu_Array[ Menu_Index ].context == 1 )
								{
									sprintf( Menu_Array[ Menu_Index ].item[ 0 ], "\1 %d second \2", Menu_Array[ Menu_Index ].context );
								}
								else if( Menu_Array[ Menu_Index ].context < 120 )
								{
									sprintf( Menu_Array[ Menu_Index ].item[ 0 ], "\1 %d seconds \2", Menu_Array[ Menu_Index ].context );
								}
								else
								{
									sprintf( Menu_Array[ Menu_Index ].item[ 0 ], "\1 %d minutes \2", Menu_Array[ Menu_Index ].context / 60 );
								}

								WriteLCD_LineCentered( Menu_Array[ Menu_Index ].item[ 0 ], 1 );
								UpdateLCD();

								// read controls
								do
								{	encoder_delta = ReadInputs( &inputs, 0 );

								} while ( encoder_delta == 0 && inputs == 0 );

								// change menu value
								if( encoder_delta != 0 )
								{
									if( Menu_Array[ Menu_Index ].context < 60 )
									{
										Menu_Array[ Menu_Index ].context += encoder_delta;

										// don't allow the gate start warning to be less than five seconds
										if( Menu_Index == GATE_START_WARNING && Menu_Array[ GATE_START_WARNING ].context < 5 )
										{
											if( encoder_delta == 1 ) Menu_Array[ GATE_START_WARNING ].context = 5;
											if( encoder_delta == -1 ) Menu_Array[ GATE_START_WARNING ].context = 0;
										}
									}
									else if( Menu_Array[ Menu_Index ].context == 60 )
									{
										if( encoder_delta < 0 )
										{
											Menu_Array[ Menu_Index ].context += encoder_delta;
										}
										else
										{
											Menu_Array[ Menu_Index ].context += encoder_delta * 10;
										}
									}
									else if( Menu_Array[ Menu_Index ].context == 120 )
									{
										if( encoder_delta < 0 )
										{
											Menu_Array[ Menu_Index ].context += encoder_delta * 10;
										}
										else
										{
											Menu_Array[ Menu_Index ].context += encoder_delta * 60;
										}
									}
									else if( Menu_Array[ Menu_Index ].context >= 120 )
									{
										Menu_Array[ Menu_Index ].context += encoder_delta * 60;
									}
									else
									{
										Menu_Array[ Menu_Index ].context += encoder_delta * 10;
									}

									if( Menu_Array[ Menu_Index ].context < 0 )
										Menu_Array[ Menu_Index ].context = 0;

									continue;
								}


								// check for exit
								WaitForButtonUp();

								if( Menu_Array[ Menu_Index ].context == 0 )
								{
									if( Menu_Index == GATE_START_WARNING )
										sprintf( Menu_Array[ GATE_START_WARNING ].item[ 0 ], "No Warning");
									else
										sprintf( Menu_Array[ Menu_Index ].item[ 0 ], "No Delay");
								}
								else if( Menu_Array[ Menu_Index ].context == 1 )
								{
									sprintf( Menu_Array[ Menu_Index ].item[ 0 ], "%d second", Menu_Array[ Menu_Index ].context );
								}
								else if( Menu_Array[ Menu_Index ].context < 120 )
								{
									sprintf( Menu_Array[ Menu_Index ].item[ 0 ], "%d seconds", Menu_Array[ Menu_Index ].context );
								}
								else
								{
									sprintf( Menu_Array[ Menu_Index ].item[ 0 ], "%d minutes", Menu_Array[ Menu_Index ].context / 60 );
								}

								WriteLCD_LineCentered( Menu_Array[ Menu_Index ].item[ 0 ], 1 );
								UpdateLCD();

								SaveEverythingToFlashMemory( 0 );
								ReadInputs( &inputs, 0 );
								inputs = 0;

								break;
							}
							break;
						}

						case AUX_1_CONFIG:
						case AUX_2_CONFIG:
						{
							while( 1 )
							{
								WriteLCD_LineCentered( Menu_Array[ Menu_Index ].item[ Menu_Array[ Menu_Index ].context ], 1 );
								UpdateLCD();

								// read controls
								do
								{	encoder_delta = ReadInputs( &inputs, 0 );

								} while (encoder_delta == 0 && inputs == 0 );


								// change menu value
								if( encoder_delta != 0 )
								{
									const int new_item = Menu_Array[ Menu_Index ].current_item + encoder_delta;

									if( new_item > Menu_Array[ Menu_Index ].item_count ) continue;
									if( new_item < 1 ) continue;

									Menu_Array[ Menu_Index ].current_item = new_item;

									switch( new_item )
									{
										case 1: { Menu_Array[ Menu_Index ].context = AUX_DISABLED; 	 	 break;	}
										case 2: { Menu_Array[ Menu_Index ].context = AUX_TIME;		 	 break;	}
										case 3: { Menu_Array[ Menu_Index ].context = AUX_TIME_SPEED;	 break;	}
										case 4: { Menu_Array[ Menu_Index ].context = AUX_SPRINT_TIMER;	 break;	}
										case 5: { Menu_Array[ Menu_Index ].context = AUX_GATE_SWITCH;	 break;	}
										case 6:	{ Menu_Array[ Menu_Index ].context = AUX_SENSOR_SPACING; break;	}
										case 7:	{ Menu_Array[ Menu_Index ].context = AUX_SENSOR_TYPE; 	 break;	}
										case 8:	{ Menu_Array[ Menu_Index ].context = AUX_ALIGN_SENSOR_A; break;	}
										case 9:	{ Menu_Array[ Menu_Index ].context = AUX_ALIGN_SENSOR_B; break;	}
										case 10:{ Menu_Array[ Menu_Index ].context = AUX_LAP_COUNT; 	 break;	}
									}
									continue;
								}

								// check for exit
								WaitForButtonUp();

								if( Menu_Array[ Menu_Index ].context == AUX_SPRINT_TIMER )
								{
									Timing_Active = 1;
									DoSprintTimer( Menu_Index );
									Timing_Active = 0;

									WaitForButtonUp();

									ItemCopy( Menu_Index, Menu_Array[ Menu_Index ].context, 0, 1 );

									WriteLCD_LineCentered( Menu_Array[ Menu_Index ].caption, 0 );
									WriteLCD_LineCentered( Menu_Array[ Menu_Index ].item[ 0 ], 1 );
									UpdateLCD();

									SaveEverythingToFlashMemory( 0 );
									ReadInputs( &inputs, 0 );
									inputs = 0;

									break;
								}
								else if( Menu_Array[ Menu_Index ].context == AUX_SENSOR_SPACING )
								{
									WriteLCD_LineCentered( "SENSOR SPACING", 0 );

									while( 1 )
									{
										const int inch 		 = Menu_Array[ Menu_Index ].sub_context_1 / 10000;
										const int fractional = Menu_Array[ Menu_Index ].sub_context_1 % 10000;

										char szFractional[ 7 ];

										switch ( fractional )
										{
											case  0 * 625:	{ strcpy( szFractional, ""		); break; }
											case  1 * 625:	{ strcpy( szFractional, "1/16"  ); break; }
											case  2 * 625:	{ strcpy( szFractional, "1/8"	); break; }
											case  3 * 625:	{ strcpy( szFractional, "3/16"	); break; }
											case  4 * 625:	{ strcpy( szFractional, "1/4"	); break; }
											case  5 * 625:	{ strcpy( szFractional, "5/16"	); break; }
											case  6 * 625:	{ strcpy( szFractional, "3/8"	); break; }
											case  7 * 625:	{ strcpy( szFractional, "7/16"	); break; }
											case  8 * 625:	{ strcpy( szFractional, "1/2"	); break; }
											case  9 * 625:	{ strcpy( szFractional, "9/16"	); break; }
											case 10 * 625:	{ strcpy( szFractional, "5/8"	); break; }
											case 11 * 625:	{ strcpy( szFractional, "11/16"	); break; }
											case 12 * 625:	{ strcpy( szFractional, "3/4"	); break; }
											case 13 * 625:	{ strcpy( szFractional, "13/16"	); break; }
											case 14 * 625:	{ strcpy( szFractional, "7/8"	); break; }
											case 15 * 625:	{ strcpy( szFractional, "15/16"	); break; }
										}

										if( fractional != 0 )
											sprintf( Menu_Array[ Menu_Index ].item[ 0 ], "\1 %d %s inches \2", inch, szFractional );
										else
											sprintf( Menu_Array[ Menu_Index ].item[ 0 ], "\1 %d inches \2", inch );

										WriteLCD_LineCentered( Menu_Array[ Menu_Index ].item[ 0 ], 1 );
										UpdateLCD();

										// read controls
										do
										{	encoder_delta = ReadInputs( &inputs, 0 );

										} while ( encoder_delta == 0 && inputs == 0 );

										// change menu value
										if( encoder_delta != 0 )
										{
											int new_value = Menu_Array[ Menu_Index ].sub_context_1 + encoder_delta * 625; // 0.0625" or 1/16"

											if( (new_value >= 12 * 10000) && (new_value <= 72 * 10000) )
											{
												Menu_Array[ Menu_Index ].sub_context_1 = new_value;

												if( Menu_Index == AUX_1_CONFIG )
													Aux_1_Sensor_Spacing = new_value;
												else
													Aux_2_Sensor_Spacing = new_value;
											}

											continue;
										}

										// check for exit
										WaitForButtonUp();

										// restore the caption back to the correct aux config
										WriteLCD_LineCentered( Menu_Array[ Menu_Index ].caption, 0 );
										UpdateLCD();

										SaveEverythingToFlashMemory( 0 );
										ReadInputs( &inputs, 0 );
										inputs = 0;

										break;
									}
									continue;
								}
								else if( Menu_Array[ Menu_Index ].context == AUX_SENSOR_TYPE )
								{
									WriteLCD_LineCentered( "SENSOR TYPE", 0 );

									while( 1 )
									{
										WriteLCD_LineCentered( Sensor_Text[ Menu_Array[ Menu_Index ].sub_context_2 ], 1 );
										UpdateLCD();

										// read controls
										do
										{	encoder_delta = ReadInputs( &inputs, 0 );

										} while ( encoder_delta == 0 && inputs == 0 );

										// change menu value
										if( encoder_delta != 0 )
										{
											int new_value = Menu_Array[ Menu_Index ].sub_context_2 + encoder_delta;

											if( new_value >= 0 && new_value < SENSOR_OPTIONS_SIZE )
											{
												Menu_Array[ Menu_Index ].sub_context_2 = new_value;
											}

											continue;
										}

										// check for exit
										WaitForButtonUp();

										// restore the caption back to the correct aux config
										WriteLCD_LineCentered( Menu_Array[ Menu_Index ].caption, 0 );
										UpdateLCD();

										SaveEverythingToFlashMemory( 0 );
										ReadInputs( &inputs, 0 );
										inputs = 0;

										break;
									}
									continue;
								}
								else if( Menu_Array[ Menu_Index ].context == AUX_ALIGN_SENSOR_A ||
										 Menu_Array[ Menu_Index ].context == AUX_ALIGN_SENSOR_B )
								{
									InitAudio();
									PlaySilence( 100, 1 );

									// read controls
									do
									{	encoder_delta = ReadInputs( &inputs, 0 );

										const int sensor_1A = ((GPIOB->IDR & 0x0080) == 0) ? 1 : 0;
										const int sensor_1B = ((GPIOB->IDR & 0x0100) == 0) ? 1 : 0;
										const int sensor_2A = ((GPIOB->IDR & 0x0010) == 0) ? 1 : 0;
										const int sensor_2B = ((GPIOB->IDR & 0x0020) == 0) ? 1 : 0;

										if( sensor_1A || sensor_1B || sensor_2A || sensor_2B )
										{
											if( Menu_Index == AUX_1_CONFIG )
											{
												if( sensor_1A && Menu_Array[ Menu_Index ].context == AUX_ALIGN_SENSOR_A )
													PlayTone( 20, Square740HzData, SQUARE_740HZ_SIZE, 1 );
												else if( sensor_1B && Menu_Array[ Menu_Index ].context == AUX_ALIGN_SENSOR_B )
													PlayTone( 20, Square740HzData, SQUARE_740HZ_SIZE, 1 );
											}
											else
											{
												if( sensor_2A && Menu_Array[ Menu_Index ].context == AUX_ALIGN_SENSOR_A )
													PlayTone( 20, Square740HzData, SQUARE_740HZ_SIZE, 1 );
												else if( sensor_2B && Menu_Array[ Menu_Index ].context == AUX_ALIGN_SENSOR_B )
													PlayTone( 20, Square740HzData, SQUARE_740HZ_SIZE, 1 );
											}
										}

									} while ( encoder_delta == 0 && inputs == 0 );

									InitAudio();

									WaitForButtonUp();

									ItemCopy( Menu_Index, Menu_Array[ Menu_Index ].context, 0, 1 );

									WriteLCD_LineCentered( Menu_Array[ Menu_Index ].caption, 0 );
									WriteLCD_LineCentered( Menu_Array[ Menu_Index ].item[ 0 ], 1 );
									UpdateLCD();

									break;
								}
								else if( Menu_Array[ Menu_Index ].context == AUX_LAP_COUNT )
								{
									WriteLCD_LineCentered( "LAP COUNT", 0 );

									while( 1 )
									{
										// add cursor
										sprintf( Menu_Array[ Menu_Index ].item[ 0 ], "\1 %d laps \2", Menu_Array[ Menu_Index ].sub_context_3 );
										WriteLCD_LineCentered( Menu_Array[ Menu_Index ].item[ 0 ], 1 );
										UpdateLCD();

										// read controls
										do
										{	encoder_delta = ReadInputs( &inputs, 0 );

										} while ( encoder_delta == 0 && inputs == 0 );

										// change menu value
										if( encoder_delta != 0 )
										{
											int new_value = Menu_Array[ Menu_Index ].sub_context_3 + encoder_delta;

											if( new_value >= 0 )
											{
												Menu_Array[ Menu_Index ].sub_context_3 = new_value;
											}

											continue;
										}

										// check for exit
										WaitForButtonUp();

										// restore the caption back to the correct aux config
										WriteLCD_LineCentered( Menu_Array[ Menu_Index ].caption, 0 );
										UpdateLCD();

										SaveEverythingToFlashMemory( 0 );
										ReadInputs( &inputs, 0 );
										inputs = 0;

										break;
									}
									continue;
								}

								ItemCopy( Menu_Index, Menu_Array[ Menu_Index ].context, 0, 1 );
								WriteLCD_LineCentered( Menu_Array[ Menu_Index ].item[ 0 ], 1 );
								UpdateLCD();

								SaveEverythingToFlashMemory( 0 );
								ReadInputs( &inputs, 0 );
								inputs = 0;

								break;
							}
							break;
						}

						case TIMER_HISTORY:
						{
							if( Timer_History_Index == 0 )
							{
								SetMenuText( Menu_Array[ TIMER_HISTORY ].item[ 0 ], "No history" );
								WriteLCD_LineCentered( "No history", 1 );
								UpdateLCD();
								break;
							}

							while( 1 )
							{
								char line_0[ DISPLAY_WIDTH ] = SPACES;
								char line_1[ DISPLAY_WIDTH ] = SPACES;

								unsigned int i, j;

								char number[ DISPLAY_WIDTH ] = SPACES;
								char time[ DISPLAY_WIDTH ]	 = SPACES;

								const int top_index		= Menu_Array[ TIMER_HISTORY ].context;
								const int bottom_index	= Menu_Array[ TIMER_HISTORY ].context + 1;

								// write time # to top left justified
								if( Timer_History[ top_index ].aux_port == AUX_1_CONFIG )
									sprintf( number, "#%d-Aux1", Timer_History[ top_index ].number );
								else
									sprintf( number, "#%d-Aux2", Timer_History[ top_index ].number );

								for( i = 0; i < strlen( number ); i++ ) line_0[ i ] = number[ i ];

								// TIME on top, TIME on bottom from TIME and SPEED element.
								if( !Timer_History[ top_index ].is_a_speed && Timer_History[ bottom_index ].is_a_speed )
								{
									// write time to top line right justified
									sprintf( time, "%s", Timer_History[ top_index ].time_string );
									for( i = 0, j = DISPLAY_WIDTH - strlen( time ) - 2; j < DISPLAY_WIDTH; j++, i++ ) line_0[ j ] = time[ i ];
									line_0[ DISPLAY_WIDTH - 2 ] = 0x20; line_0[ DISPLAY_WIDTH - 1 ] = 0x20;

									// write time # to bottom line left justified
									if( Timer_History[ bottom_index ].aux_port == AUX_1_CONFIG )
										sprintf( number, "#%d-Aux1", Timer_History[ bottom_index ].number );
									else
										sprintf( number, "#%d-Aux2", Timer_History[ bottom_index ].number );
									for( i = 0; i < strlen( number ); i++ ) line_1[ i ] = number[ i ];

									// write time to bottom line right justified
									if( Timer_History[ bottom_index ].elapsed_time != 0 )
									{
										sprintf( time, "%s", Timer_History[ bottom_index ].time_string );
										for( i = 0, j = DISPLAY_WIDTH - strlen( time ) - 2; j < DISPLAY_WIDTH; j++, i++ ) line_1[ j ] = time[ i ];
										line_1[ DISPLAY_WIDTH - 2 ] = 0x20; line_1[ DISPLAY_WIDTH - 1 ] = 0x20;
									}
									else // handle replacing time of 0.000 with "Sprint" text
									{
										sprintf( &line_1[ 12 ], "Sprint" );
										line_1[ 18 ] = 0x20;
									}
								}
								// TIME on top, TIME on bottom
								else if( !Timer_History[ top_index ].is_a_speed && !Timer_History[ bottom_index ].is_a_speed )
								{
									// write time to top line right justified
									sprintf( time, "%s", Timer_History[ top_index ].time_string );
									for( i = 0, j = DISPLAY_WIDTH - strlen( time ) - 2; j < DISPLAY_WIDTH; j++, i++ ) line_0[ j ] = time[ i ];
									line_0[ DISPLAY_WIDTH - 2 ] = 0x20; line_0[ DISPLAY_WIDTH - 1 ] = 0x20;

									// if there is a time in the next element of the history array then write it since we have room
									if( Timer_History[ bottom_index ].number != 0 )
									{
										// write time # to bottom line left justified
										if( Timer_History[ bottom_index ].aux_port == AUX_1_CONFIG )
											sprintf( number, "#%d-Aux1", Timer_History[ bottom_index ].number );
										else
											sprintf( number, "#%d-Aux2", Timer_History[ bottom_index ].number );
										for( i = 0; i < strlen( number ); i++ ) line_1[ i ] = number[ i ];

										// write time to bottom line right justified
										sprintf( time, "%s", Timer_History[ bottom_index ].time_string );
										for( i = 0, j = DISPLAY_WIDTH - strlen( time ) - 2; j < DISPLAY_WIDTH; j++, i++ ) line_1[ j ] = time[ i ];
										line_1[ DISPLAY_WIDTH - 2 ] = 0x20; line_1[ DISPLAY_WIDTH - 1 ] = 0x20;
									}
									else
									{	// clear out the bottom line of the display since there is no value to be displayed
										for( i = 0; i < DISPLAY_WIDTH; i++ ) line_1[ i ] = 0x20;
									}
								}
								// TIME on top, SPEED on bottom
								else if( Timer_History[ top_index ].is_a_speed )
								{
									// write Speed: left justified
									if( Timer_History[ top_index ].elapsed_time == 0 )
									{
										sprintf( &line_0[ 12 ], "Sprint" );
										line_0[ 18 ] = 0x20;

										sprintf( line_1, "Speed:" );
										line_1[ 6 ] = 0x20;	// get rid of null termination that sprintf put on the end
									}
									else
									{
										// write time to top line right justified
										sprintf( time, "%s", Timer_History[ top_index ].time_string );
										for( i = 0, j = DISPLAY_WIDTH - strlen( time ) - 2; j < DISPLAY_WIDTH; j++, i++ ) line_0[ j ] = time[ i ];
										line_0[ DISPLAY_WIDTH - 2 ] = 0x20; line_0[ DISPLAY_WIDTH - 1 ] = 0x20;

										sprintf( line_1, "Speed:" );
										line_1[ 6 ] = 0x20;	// get rid of null termination that sprintf put on the end
									}

									// write the speed right justified
									j = DISPLAY_WIDTH - 2 - strlen( Timer_History[ top_index ].speed_string );
									sprintf( &line_1[ j ], "%s", Timer_History[ top_index ].speed_string );
									line_1[ DISPLAY_WIDTH - 2 ] = 0x20; line_1[ DISPLAY_WIDTH - 1 ] = 0x20;
								}
								// TIME on top, 0 on bottom
								else
								{
									// write time right justified
									sprintf( time, "%s", Timer_History[ top_index ].time_string );
									for( i = 0; i < DISPLAY_WIDTH; i++ ) line_0[ i ] = 0x20;
									for( i = 0, j = DISPLAY_WIDTH - strlen( time ) - 2; j < DISPLAY_WIDTH; j++, i++ ) line_0[ j ] = time[ i ];
									line_0[ DISPLAY_WIDTH - 2 ] = 0x20; line_0[ DISPLAY_WIDTH - 1 ] = 0x20;

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
								{	encoder_delta = ReadInputs( &inputs, 0 );

								} while (encoder_delta == 0 && inputs == 0 );

								// change menu value
								if( encoder_delta != 0 )
								{
									int new_value = Menu_Array[ TIMER_HISTORY ].context - encoder_delta;

									if( new_value >= 0 && new_value < Timer_History_Index )
									{
										Menu_Array[ TIMER_HISTORY ].context = new_value;
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

						case CLEAR_TIMER_HISTORY:
						{
							if( Timer_History_Index != 0 )
							{
								WriteLCD_LineCentered( "Are you sure?", 0 );
								WriteLCD_LineCentered( "yes /\1 NO \2", 1 );
								UpdateLCD();

								while( 1 )
								{
									// wait for input
									do
									{	encoder_delta = ReadInputs( &inputs, 0 );

									} while ( encoder_delta == 0 && inputs == 0 );

									if( encoder_delta == -1 )
									{
										Menu_Array[ CLEAR_TIMER_HISTORY ].context = 1;
										WriteLCD_LineCentered( "\1 YES \2/ no", 1 );
										UpdateLCD();
									}
									else if( encoder_delta == 1 )
									{
										Menu_Array[ CLEAR_TIMER_HISTORY ].context = 0;
										WriteLCD_LineCentered( "yes /\1 NO \2", 1 );
										UpdateLCD();
									}

									if( inputs == BUTTON_E )
									{
										if( Menu_Array[ CLEAR_TIMER_HISTORY ].context == 1 )
										{
											SetMenuText( Menu_Array[ TIMER_HISTORY ].item[ 0 ], "No history" );

											WriteLCD_LineCentered( "Clearing", 1 );
											UpdateLCD();

											ClearTimingHistories();
											SaveEverythingToFlashMemory( 0 );
											ReadInputs( &inputs, 0 );
											inputs = 0;

											WriteLCD_LineCentered( "Timing Histories", 0 );
											WriteLCD_LineCentered( "Cleared", 1 );
											UpdateLCD();

											Delay( 2000 );

											Menu_Index = TIMER_HISTORY;
										}
										break;
									}
									continue;
								}
							}
							break;
						}

						case AUTO_ANNOUNCE_TIMES:
						{
							while( 1 )
							{
								WriteLCD_LineCentered( Menu_Array[ AUTO_ANNOUNCE_TIMES ].item[ Menu_Array[ AUTO_ANNOUNCE_TIMES ].context ], 1 );
								UpdateLCD();

								// read controls
								do
								{	encoder_delta = ReadInputs( &inputs, 0 );

								} while (encoder_delta == 0 && inputs == 0 );

								// change menu value
								if( encoder_delta != 0 )
								{
									int new_value = Menu_Array[ AUTO_ANNOUNCE_TIMES ].context + encoder_delta;

									if( new_value >= 1 && new_value <= 2 )
										Menu_Array[ AUTO_ANNOUNCE_TIMES ].context = new_value;

									continue;
								}

								// check for exit
								WaitForButtonUp();

								ItemCopy( AUTO_ANNOUNCE_TIMES, Menu_Array[ AUTO_ANNOUNCE_TIMES ].context, 0, 1 );
								WriteLCD_LineCentered( Menu_Array[ AUTO_ANNOUNCE_TIMES ].item[ 0 ], 1 );
								UpdateLCD();

								SaveEverythingToFlashMemory( 0 );
								ReadInputs( &inputs, 0 );
								inputs = 0;

								break;
							}
							break;

						}

						case TOTAL_GATE_DROPS:
						{
							sprintf( Menu_Array[ TOTAL_GATE_DROPS ].item[ 0 ], "%d", Menu_Array[ TOTAL_GATE_DROPS ].context );
							WriteLCD_LineCentered( Menu_Array[ TOTAL_GATE_DROPS ].item[ 0 ], 1 );
							UpdateLCD();

							break;
						}

						case ZERO_TOTAL_GATE_DROPS:
						{
							if( Menu_Array[ TOTAL_GATE_DROPS ].context != 0 )
							{
								WriteLCD_LineCentered( "Are you sure?", 0 );
								WriteLCD_LineCentered( "yes /\1 NO \2", 1 );
								UpdateLCD();

								Menu_Array[ ZERO_TOTAL_GATE_DROPS ].context = 0;

								while( 1 )
								{
									// wait for input
									do
									{	encoder_delta = ReadInputs( &inputs, 0 );

									} while ( encoder_delta == 0 && inputs == 0 );

									if( encoder_delta == -1 )
									{
										Menu_Array[ ZERO_TOTAL_GATE_DROPS ].context = 1;
										WriteLCD_LineCentered( "\1 YES \2/ no", 1 );
										UpdateLCD();
									}
									else if( encoder_delta == 1 )
									{
										Menu_Array[ ZERO_TOTAL_GATE_DROPS ].context = 0;
										WriteLCD_LineCentered( "yes /\1 NO \2", 1 );
										UpdateLCD();
									}

									if( inputs == BUTTON_E )
									{
										if( Menu_Array[ ZERO_TOTAL_GATE_DROPS ].context == 0 )
										{
											break;
										}
										else if( Menu_Array[ ZERO_TOTAL_GATE_DROPS ].context > 0 )
										{
											SetMenuText( Menu_Array[ TOTAL_GATE_DROPS ].item[ 0 ], "0" );

											Menu_Array[ TOTAL_GATE_DROPS ].context = 0;

											ReadInputs( &inputs, 0 );
											inputs = 0;

											WriteLCD_LineCentered( "Total Gate Drops", 0 );
											WriteLCD_LineCentered( "Zeroed", 1 );
											UpdateLCD();

											Delay( 2000 );

											Menu_Index = TOTAL_GATE_DROPS;
										}
										break;
									}
									continue;
								}
							}
							break;
						}

						case SAVE_TOTAL_GATE_DROPS:
						{
							WriteLCD_LineCentered( "Are you sure?", 0 );
							WriteLCD_LineCentered( "yes /\1 NO \2", 1 );
							UpdateLCD();

							Menu_Array[ SAVE_TOTAL_GATE_DROPS ].context = 0;

							while( 1 )
							{
								// wait for input
								do
								{	encoder_delta = ReadInputs( &inputs, 0 );

								} while ( encoder_delta == 0 && inputs == 0 );

								if( encoder_delta == -1 )
								{
									Menu_Array[ SAVE_TOTAL_GATE_DROPS ].context = 1;
									WriteLCD_LineCentered( "\1 YES \2/ no", 1 );
									UpdateLCD();
								}
								else if( encoder_delta == 1 )
								{
									Menu_Array[ SAVE_TOTAL_GATE_DROPS ].context = 0;
									WriteLCD_LineCentered( "yes /\1 NO \2", 1 );
									UpdateLCD();
								}

								if( inputs == BUTTON_E )
								{
									if( Menu_Array[ SAVE_TOTAL_GATE_DROPS ].context == 0 )
									{
										break;
									}
									else if( Menu_Array[ SAVE_TOTAL_GATE_DROPS ].context > 0 )
									{
										SaveEverythingToFlashMemory( 0 );
										ReadInputs( &inputs, 0 );
										inputs = 0;

										WriteLCD_LineCentered( "Total Gate Drops", 0 );
										WriteLCD_LineCentered( "Saved to memory", 1 );
										UpdateLCD();

										Delay( 2000 );

										Menu_Index = SAVE_TOTAL_GATE_DROPS;
									}
									break;
								}
								continue;
							}
							break;
						}

						case CADENCE_VOLUME:
						{
							while( 1 )
							{
								// display with cursor
								sprintf( Menu_Array[ CADENCE_VOLUME ].item[ 0 ], "\1 %d%% \2", Menu_Array[ CADENCE_VOLUME ].context );
								WriteLCD_LineCentered( Menu_Array[ Menu_Index ].item[ 0 ], 1 );
								UpdateLCD();

								// read controls
								do
								{	encoder_delta = ReadInputs( &inputs, 0 );

								} while (encoder_delta == 0 && inputs == 0 );

								// change menu value
								if( encoder_delta != 0 )
								{
									int new_value = Menu_Array[ CADENCE_VOLUME ].context += encoder_delta;

									if( new_value > 100 ) new_value = 100;
									if( new_value < 0 ) new_value = 0;

									Menu_Array[ CADENCE_VOLUME ].context = new_value;

									int volume = (128 * Menu_Array[ CADENCE_VOLUME ].context) / 100;

									SetAttenuator( 0x1000 | volume );
									continue;
								}

								// check for exit
								WaitForButtonUp();

								// remove cursor and exit
								sprintf( Menu_Array[ CADENCE_VOLUME ].item[ 0 ], "%d%%", Menu_Array[ CADENCE_VOLUME ].context );
								WriteLCD_LineCentered( Menu_Array[ Menu_Index ].item[ 0 ], 1 );
								UpdateLCD();

								break;
							}

#ifdef NUMBERS
							PlaySilence( 100, 0 );
//							PlaySample24khz( VolumeAtData, 0, VOLUME_AT_SIZE, 1 );
							PlayTimeOrPercent( Menu_Array[ CADENCE_VOLUME ].context * 60 * 1000, PLAY_PERCENT );
							InitAudio();
#endif
							SaveEverythingToFlashMemory( 0 );
							ReadInputs( &inputs, 0 );
							inputs = 0;

							break;
						}

						case AUDIO_IN_VOLUME:
						{
							while( 1 )
							{
								// display with cursor
								sprintf( Menu_Array[ AUDIO_IN_VOLUME ].item[ 0 ], "\1 %d%% \2", Menu_Array[ AUDIO_IN_VOLUME ].context );
								WriteLCD_LineCentered( Menu_Array[ Menu_Index ].item[ 0 ], 1 );
								UpdateLCD();

								// read controls
								do
								{	encoder_delta = ReadInputs( &inputs, 0 );

								} while (encoder_delta == 0 && inputs == 0 );

								// change menu value
								if( encoder_delta != 0 )
								{
									int new_value = Menu_Array[ AUDIO_IN_VOLUME ].context += encoder_delta;

									if( new_value > 100 ) new_value = 100;
									if( new_value < 0 ) new_value = 0;

									Menu_Array[ AUDIO_IN_VOLUME ].context = new_value;

									int volume = (128 * Menu_Array[ AUDIO_IN_VOLUME ].context) / 100;

									SetAttenuator( 0x0000 | volume );

									continue;
								}

								// check for exit
								WaitForButtonUp();

								// remove cursor and exit
								sprintf( Menu_Array[ AUDIO_IN_VOLUME ].item[ 0 ], "%d%%", Menu_Array[ AUDIO_IN_VOLUME ].context );
								WriteLCD_LineCentered( Menu_Array[ Menu_Index ].item[ 0 ], 1 );
								UpdateLCD();

								SaveEverythingToFlashMemory( 0 );
								ReadInputs( &inputs, 0 );
								inputs = 0;

								break;
							}

							break;
						}

						case BATTERY_CONDITION:
						{
#ifdef NUMBERS
//							PlaySilence( 100, 0 );
//							PlaySample24khz( BatteryAtData, 0, BATTERY_AT_SIZE, 1 );
//							PlayTimeOrPercent( Menu_Array[ BATTERY_CONDITION ].context * 60 * 1000, PLAY_PERCENT );
//							InitAudio();
#endif
							break;
						}

						case TEST_LIGHTS:	//TODO: Add more effects with compositing and transitioning
						{
							while( 1 )
							{
								WriteLCD_LineCentered( Menu_Array[ TEST_LIGHTS ].item[ Menu_Array[ TEST_LIGHTS ].context ], 1 );
								UpdateLCD();

								// read controls
								do
								{	encoder_delta = ReadInputs( &inputs, 0 );

								} while (encoder_delta == 0 && inputs == 0 );

								// change menu value
								if( encoder_delta != 0 )
								{
									const int new_item = Menu_Array[ TEST_LIGHTS ].current_item + encoder_delta;

									if( new_item > Menu_Array[ TEST_LIGHTS ].item_count ) continue;
									if( new_item < 1 ) continue;

									Menu_Array[ TEST_LIGHTS ].current_item = new_item;

									switch( new_item )
									{
										case 1: { Menu_Array[ TEST_LIGHTS ].context = TEST_LIGHTS_ALL_ON; 		break;	}
										case 2: { Menu_Array[ TEST_LIGHTS ].context = TEST_LIGHTS_RED; 			break;	}
										case 3: { Menu_Array[ TEST_LIGHTS ].context = TEST_LIGHTS_AMBER1; 		break;	}
										case 4: { Menu_Array[ TEST_LIGHTS ].context = TEST_LIGHTS_AMBER2; 		break;	}
										case 5: { Menu_Array[ TEST_LIGHTS ].context = TEST_LIGHTS_GREEN; 		break;	}
										case 6: { Menu_Array[ TEST_LIGHTS ].context = TEST_LIGHTS_UP_DOWN;		break;	}
										case 7: { Menu_Array[ TEST_LIGHTS ].context = TEST_LIGHTS_DARK_LIGHT;	break;	}
										case 8: { Menu_Array[ TEST_LIGHTS ].context = TEST_LIGHTS_RANDOM;		break;	}
									}
									continue;
								}

								WaitForButtonUp();

								if( Menu_Array[ TEST_LIGHTS ].context == TEST_LIGHTS_ALL_ON )
								{
									GPIO_SetBits( GPIOD, GPIO_Pin_12 );	// RED light ON
									GPIO_SetBits( GPIOD, GPIO_Pin_13 );	// AMBER 1 light ON
									GPIO_SetBits( GPIOD, GPIO_Pin_14 );	// AMBER 2 light ON
									GPIO_SetBits( GPIOD, GPIO_Pin_15 );	// GREEN light ON

									// read controls
									do
									{	encoder_delta = ReadInputs( &inputs, 0 );

									} while (encoder_delta == 0 && inputs == 0 );

									GPIO_ResetBits( GPIOD, GPIO_Pin_12 );	// RED light OFF
									GPIO_ResetBits( GPIOD, GPIO_Pin_13 );	// AMBER 1 light OFF
									GPIO_ResetBits( GPIOD, GPIO_Pin_14 );	// AMBER 2 light OFF
									GPIO_ResetBits( GPIOD, GPIO_Pin_15 );	// GREEN light OFF

									ItemCopy( TEST_LIGHTS, Menu_Array[ TEST_LIGHTS ].context, 0, 1 );
									WriteLCD_LineCentered( Menu_Array[ TEST_LIGHTS ].caption, 0 );
									UpdateLCD();

									WaitForButtonUp();

									break;
								}
								else if( Menu_Array[ TEST_LIGHTS ].context == TEST_LIGHTS_RED )
								{
									GPIO_SetBits( GPIOD, GPIO_Pin_12 );	// RED light ON

									ReadLightExitControlInput( TEST_LIGHTS_RED );

									GPIO_ResetBits( GPIOD, GPIO_Pin_12 );	// RED light OFF

									ItemCopy( TEST_LIGHTS, Menu_Array[ TEST_LIGHTS ].context, 0, 1 );
									WriteLCD_LineCentered( Menu_Array[ TEST_LIGHTS ].caption, 0 );
									UpdateLCD();

									WaitForButtonUp();

									break;
								}
								else if( Menu_Array[ TEST_LIGHTS ].context == TEST_LIGHTS_AMBER1 )
								{
									GPIO_SetBits( GPIOD, GPIO_Pin_13 );	// AMBER 1 light ON

									ReadLightExitControlInput( TEST_LIGHTS_AMBER1 );

									GPIO_ResetBits( GPIOD, GPIO_Pin_13 );	// AMBER 1 light OFF

									ItemCopy( TEST_LIGHTS, Menu_Array[ TEST_LIGHTS ].context, 0, 1 );
									WriteLCD_LineCentered( Menu_Array[ TEST_LIGHTS ].caption, 0 );
									UpdateLCD();

									WaitForButtonUp();

									break;
								}
								else if( Menu_Array[ TEST_LIGHTS ].context == TEST_LIGHTS_AMBER2 )
								{
									GPIO_SetBits( GPIOD, GPIO_Pin_14 );	// AMBER 2 light ON

									ReadLightExitControlInput( TEST_LIGHTS_AMBER2 );

									GPIO_ResetBits( GPIOD, GPIO_Pin_14 );	// AMBER 2 light OFF

									ItemCopy( TEST_LIGHTS, Menu_Array[ TEST_LIGHTS ].context, 0, 1 );
									WriteLCD_LineCentered( Menu_Array[ TEST_LIGHTS ].caption, 0 );
									UpdateLCD();

									WaitForButtonUp();

									break;
								}
								else if( Menu_Array[ TEST_LIGHTS ].context == TEST_LIGHTS_GREEN )
								{
									GPIO_SetBits( GPIOD, GPIO_Pin_15 );	// GREEN light ON

									ReadLightExitControlInput( TEST_LIGHTS_GREEN );

									GPIO_ResetBits( GPIOD, GPIO_Pin_15 );	// GREEN light OFF

									ItemCopy( TEST_LIGHTS, Menu_Array[ TEST_LIGHTS ].context, 0, 1 );
									WriteLCD_LineCentered( Menu_Array[ TEST_LIGHTS ].caption, 0 );
									UpdateLCD();

									WaitForButtonUp();

									break;
								}
								else if( Menu_Array[ TEST_LIGHTS ].context == TEST_LIGHTS_UP_DOWN )
								{
									int abort_cycle = 0;

									// read controls
									do
									{	encoder_delta = ReadInputs( &inputs, 0 );

										abort_cycle = LightTestCycle( 1 );

									} while ( abort_cycle == 0 && encoder_delta == 0 && inputs == 0 );


									ItemCopy( TEST_LIGHTS, Menu_Array[ TEST_LIGHTS ].context, 0, 1 );
									WriteLCD_LineCentered( Menu_Array[ TEST_LIGHTS ].caption, 0 );
									UpdateLCD();

									WaitForButtonUp();

									break;
								}
								else if( Menu_Array[ TEST_LIGHTS ].context == TEST_LIGHTS_DARK_LIGHT )
								{
									const float period = 40.0f;

									float on_time	= 0.0f;
									float off_time	= 0.0f;

									float theta 	= 3.14159265f / 2.0f;

									do
									{
										encoder_delta = ReadInputs( &inputs, 0 );

										theta += 0.0135f;
										on_time = 0.5f * period - 0.5f * period * sin( theta );

										off_time = period - on_time;

										GPIO_SetBits( GPIOD, GPIO_Pin_12 );	 // RED light ON
										GPIO_SetBits( GPIOD, GPIO_Pin_13 );	 // AMBER 1 light ON
										GPIO_SetBits( GPIOD, GPIO_Pin_14 );	 // AMBER 2 light ON
										GPIO_SetBits( GPIOD, GPIO_Pin_15 );	 // GREEN light ON

										ShortDelay( (int) on_time );

										GPIO_ResetBits( GPIOD, GPIO_Pin_12 );// RED light OFF
										GPIO_ResetBits( GPIOD, GPIO_Pin_13 );// AMBER 1 light OFF
										GPIO_ResetBits( GPIOD, GPIO_Pin_14 );// AMBER 2 light OFF
										GPIO_ResetBits( GPIOD, GPIO_Pin_15 );// GREEN light OFF

										ShortDelay( (int) off_time );

									} while ( encoder_delta == 0 && inputs == 0 );

									ItemCopy( TEST_LIGHTS, Menu_Array[ TEST_LIGHTS ].context, 0, 1 );
									WriteLCD_LineCentered( Menu_Array[ TEST_LIGHTS ].caption, 0 );
									UpdateLCD();

									break;
								}
								else if( Menu_Array[ TEST_LIGHTS ].context == TEST_LIGHTS_RANDOM )
								{
									// TODO: Finish this thing!
									const float period = 100.0f;

									float red_on_time,		red_off_time, 		red_theta;
									float amber1_on_time,	amber1_off_time, 	amber1_theta;
									float amber2_on_time,	amber2_off_time, 	amber2_theta;
									float green_on_time,	green_off_time, 	green_theta;

									do
									{
										encoder_delta = ReadInputs( &inputs, 0 );

										red_theta		+=  0.0125f;
										amber1_theta	+=  0.0225f;
										amber2_theta	+= -0.0155f;
										green_theta		+= -0.0195f;

										red_on_time 	= period * sin( red_theta	 );
										amber1_on_time  = period * sin( amber1_theta );
										amber2_on_time  = period * sin( amber2_theta );
										green_on_time 	= period * sin( green_theta	 );

										if( red_on_time    < 0 ) red_on_time 	*=-1;
										if( amber1_on_time < 0 ) amber1_on_time *=-1;
										if( amber2_on_time < 0 ) amber2_on_time *=-1;
										if( green_on_time  < 0 ) red_on_time 	*=-1;

										red_off_time 	= period - red_on_time;
										amber1_off_time = period - amber1_on_time;
										amber2_off_time = period - amber2_on_time;
										green_off_time 	= period - green_on_time;

										// todo: create non-blocking "delays"
										GPIO_SetBits( GPIOD, GPIO_Pin_12 );	 // RED light ON
										ShortDelay( (int) red_on_time );
										GPIO_ResetBits( GPIOD, GPIO_Pin_12 );// RED light OFF
										ShortDelay( (int) red_off_time );

										GPIO_SetBits( GPIOD, GPIO_Pin_13 );	 // AMBER 1 light ON
										GPIO_SetBits( GPIOD, GPIO_Pin_14 );	 // AMBER 2 light ON
										GPIO_SetBits( GPIOD, GPIO_Pin_15 );	 // GREEN light ON


										GPIO_ResetBits( GPIOD, GPIO_Pin_12 );// RED light OFF
										GPIO_ResetBits( GPIOD, GPIO_Pin_13 );// AMBER 1 light OFF
										GPIO_ResetBits( GPIOD, GPIO_Pin_14 );// AMBER 2 light OFF
										GPIO_ResetBits( GPIOD, GPIO_Pin_15 );// GREEN light OFF


									} while ( encoder_delta == 0 && inputs == 0 );

									ItemCopy( TEST_LIGHTS, Menu_Array[ TEST_LIGHTS ].context, 0, 1 );
									WriteLCD_LineCentered( Menu_Array[ TEST_LIGHTS ].caption, 0 );
									UpdateLCD();

									break;
								}
							}

							break;
						}

						case REACTION_GAME:
						{
							while( 1 )
							{
								WriteLCD_LineCentered( Menu_Array[ REACTION_GAME ].item[ Menu_Array[ REACTION_GAME ].context ], 1 );
								UpdateLCD();

								// read controls
								do
								{	encoder_delta = ReadInputs( &inputs, 0 );

								} while (encoder_delta == 0 && inputs == 0 );

								// change menu value
								if( encoder_delta != 0 )
								{
									const int new_item = Menu_Array[ REACTION_GAME ].current_item + encoder_delta;

									if( new_item > Menu_Array[ REACTION_GAME ].item_count ) continue;
									if( new_item < 1 ) continue;

									Menu_Array[ REACTION_GAME ].current_item = new_item;

									switch( new_item )
									{
										case 1: { Menu_Array[ REACTION_GAME ].context = REACTION_GAME_ONE_PLAYER; 	break;	}
										case 2: { Menu_Array[ REACTION_GAME ].context = REACTION_GAME_TWO_PLAYER;	break;	}
										case 3: { Menu_Array[ REACTION_GAME ].context = REACTION_GAME_VIEW_STATS;	break;	}
										case 4: { Menu_Array[ REACTION_GAME ].context = REACTION_GAME_CLEAR_STATS;	break;	}
									}
									continue;
								}

								// check for exit
								WaitForButtonUp();

								if( Menu_Array[ REACTION_GAME ].context == REACTION_GAME_ONE_PLAYER )
								{
									DoReactionGame( 1 );

									ItemCopy( REACTION_GAME, Menu_Array[ REACTION_GAME ].context, 0, 1 );
									WriteLCD_LineCentered( Menu_Array[ REACTION_GAME ].caption, 0 );
									UpdateLCD();

									break;
								}
								else if( Menu_Array[ REACTION_GAME ].context == REACTION_GAME_TWO_PLAYER )
								{
									DoReactionGame( 2 );

									ItemCopy( REACTION_GAME, Menu_Array[ REACTION_GAME ].context, 0, 1 );
									WriteLCD_LineCentered( Menu_Array[ REACTION_GAME ].caption, 0 );
									UpdateLCD();

									break;
								}
								else if( Menu_Array[ REACTION_GAME ].context == REACTION_GAME_VIEW_STATS )
								{
									unsigned int minutes_best  = 0, seconds_best  = 0, tens_place_best	= 0, hund_place_best  = 0, thou_place_best  = 0;
								    unsigned int minutes_avg   = 0, seconds_avg	  = 0, tens_place_avg   = 0, hund_place_avg   = 0, thou_place_avg   = 0;
								    unsigned int minutes_worst = 0, seconds_worst = 0, tens_place_worst	= 0, hund_place_worst = 0, thou_place_worst = 0;

								    GetTimeFromTicks( Reaction_Game_P1_BEST,  &minutes_best,  &seconds_best,  &tens_place_best,  &hund_place_best,  &thou_place_best  );
								    GetTimeFromTicks( Reaction_Game_P1_AVG,	  &minutes_avg,   &seconds_avg,   &tens_place_avg,   &hund_place_avg,   &thou_place_avg   );
								    GetTimeFromTicks( Reaction_Game_P1_WORST, &minutes_worst, &seconds_worst, &tens_place_worst, &hund_place_worst, &thou_place_worst );

									char line0[ DISPLAY_WIDTH ];
									sprintf( line0, "1: %d.%d%d%d %d.%d%d%d %d.%d%d%d",
											        seconds_best,	tens_place_best,  hund_place_best,	thou_place_best,
											        seconds_avg,	tens_place_avg,	  hund_place_avg,	thou_place_avg,
											        seconds_worst,	tens_place_worst, hund_place_worst,	thou_place_worst );
									WriteLCD_Line( line0, 0 );

								    GetTimeFromTicks( Reaction_Game_P2_BEST,  &minutes_best,  &seconds_best,  &tens_place_best,  &hund_place_best,  &thou_place_best  );
								    GetTimeFromTicks( Reaction_Game_P2_AVG,	  &minutes_avg,   &seconds_avg,   &tens_place_avg,   &hund_place_avg,   &thou_place_avg   );
								    GetTimeFromTicks( Reaction_Game_P2_WORST, &minutes_worst, &seconds_worst, &tens_place_worst, &hund_place_worst, &thou_place_worst );

									char line1[ DISPLAY_WIDTH ];
									sprintf( line1, "2: %d.%d%d%d %d.%d%d%d %d.%d%d%d",
											        seconds_best,	tens_place_best,  hund_place_best,	thou_place_best,
											        seconds_avg,	tens_place_avg,	  hund_place_avg,	thou_place_avg,
											        seconds_worst,	tens_place_worst, hund_place_worst,	thou_place_worst );
									WriteLCD_Line( line1, 1 );

									UpdateLCD();

									do
									{	encoder_delta = ReadInputs( &inputs, 0 );

									} while (encoder_delta == 0 && inputs == 0 );

									ItemCopy( REACTION_GAME, Menu_Array[ REACTION_GAME ].context, 0, 1 );
									WriteLCD_LineCentered( Menu_Array[ REACTION_GAME ].caption, 0 );
									UpdateLCD();

									break;
								}
								else if( Menu_Array[ REACTION_GAME ].context == REACTION_GAME_CLEAR_STATS )
								{
									if( (Reaction_Game_P1_BEST == 0) && (Reaction_Game_P1_AVG == 0) && (Reaction_Game_P1_WORST == 0) &&
										(Reaction_Game_P2_BEST == 0) && (Reaction_Game_P2_AVG == 0) && (Reaction_Game_P2_WORST == 0) )
									{
										ItemCopy( REACTION_GAME, Menu_Array[ REACTION_GAME ].context, 0, 1 );
										WriteLCD_LineCentered( Menu_Array[ REACTION_GAME ].caption, 0 );
										WriteLCD_LineCentered( "Stats Are Clear", 1 );
										UpdateLCD();

										Delay( 2000 );

										break;
									}

									WriteLCD_LineCentered( "Are you sure?", 0 );
									WriteLCD_LineCentered( "yes /\1 NO \2", 1 );
									UpdateLCD();

									while( 1 )
									{
										// wait for input
										do
										{	encoder_delta = ReadInputs( &inputs, 0 );

										} while ( encoder_delta == 0 && inputs == 0 );

										if( encoder_delta == -1 )
										{
											Menu_Array[ REACTION_GAME ].sub_context_1 = 1;
											WriteLCD_LineCentered( "\1 YES \2/ no", 1 );
											UpdateLCD();
										}
										else if( encoder_delta == 1 )
										{
											Menu_Array[ REACTION_GAME ].sub_context_1 = 0;
											WriteLCD_LineCentered( "yes /\1 NO \2", 1 );
											UpdateLCD();
										}

										if( inputs == BUTTON_E )
										{
											if( Menu_Array[ REACTION_GAME ].sub_context_1 == 1 )
											{
												Menu_Array[ REACTION_GAME ].sub_context_1 = 0;

												Reaction_Game_P1_BEST  = 0;
												Reaction_Game_P1_AVG   = 0;
												Reaction_Game_P1_WORST = 0;

												Reaction_Game_P2_BEST  = 0;
												Reaction_Game_P2_AVG   = 0;
												Reaction_Game_P2_WORST = 0;

// TODO: comment this back in for SMT version	SaveEverythingToFlashMemory( 0 );
												ReadInputs( &inputs, 0 );
												inputs = 0;

												WriteLCD_LineCentered( Menu_Array[ REACTION_GAME ].caption, 0 );
												WriteLCD_LineCentered( "Stats Cleared", 1 );
												UpdateLCD();

												Delay( 2000 );
											}

											ItemCopy( REACTION_GAME, Menu_Array[ REACTION_GAME ].context, 0, 1 );

											break;
										}
									}
									break;
								}
							}
							break;
						}

						case WIRELESS_REMOTE:
						{
							while( 1 )
							{
								WriteLCD_LineCentered( Menu_Array[ WIRELESS_REMOTE ].item[ Menu_Array[ WIRELESS_REMOTE ].context ], 1 );
								UpdateLCD();

								// read controls
								do
								{	encoder_delta = ReadInputs( &inputs, 0 );

								} while (encoder_delta == 0 && inputs == 0 );

								// change menu value
								if( encoder_delta != 0 )
								{
									int new_value = Menu_Array[ WIRELESS_REMOTE ].context + encoder_delta;

									if( new_value >= 1 && new_value <= 2 )
										Menu_Array[ WIRELESS_REMOTE ].context = new_value;

									continue;
								}

								// check for exit
								WaitForButtonUp();

								ItemCopy( WIRELESS_REMOTE, Menu_Array[ WIRELESS_REMOTE ].context, 0, 1 );
								WriteLCD_LineCentered( Menu_Array[ WIRELESS_REMOTE ].item[ 0 ], 1 );
								UpdateLCD();

								SaveEverythingToFlashMemory( 0 );
								ReadInputs( &inputs, 0 );
								inputs = 0;

								break;
							}
							break;

						}

						case RELEASE_DEVICE:
						{
							while( 1 )
							{
								WriteLCD_LineCentered( Menu_Array[ RELEASE_DEVICE ].item[ Menu_Array[ RELEASE_DEVICE ].context ], 1 );
								UpdateLCD();

								// read controls
								do
								{	encoder_delta = ReadInputs( &inputs, 0 );

								} while (encoder_delta == 0 && inputs == 0 );


								// change menu value
								if( encoder_delta != 0 )
								{
									const int new_item = Menu_Array[ RELEASE_DEVICE ].current_item + encoder_delta;

									if( new_item > Menu_Array[ RELEASE_DEVICE ].item_count ) continue;
									if( new_item < 1 ) continue;

									Menu_Array[ RELEASE_DEVICE ].current_item = new_item;

									switch( new_item )
									{
										case 1: { Menu_Array[ RELEASE_DEVICE ].context = RELEASE_DEVICE_SOLENOID;	break;	}
										case 2: { Menu_Array[ RELEASE_DEVICE ].context = RELEASE_DEVICE_MAGNET;		break;	}
										case 3: { Menu_Array[ RELEASE_DEVICE ].context = RELEASE_DEVICE_AIR_RAM;	break;	}
										case 4: { Menu_Array[ RELEASE_DEVICE ].context = RELEASE_DEVICE_PROGATE;	break;	}
									}

									continue;
								}

								// check for exit
								WaitForButtonUp();

								ItemCopy( RELEASE_DEVICE, Menu_Array[ RELEASE_DEVICE ].context, 0, 1 );
								WriteLCD_LineCentered( Menu_Array[ RELEASE_DEVICE ].item[ 0 ], 1 );
								UpdateLCD();

								SaveEverythingToFlashMemory( 0 );
								ReadInputs( &inputs, 0 );
								inputs = 0;

								break;
							}
							break;
						}

						case ANTI_SLAM:
						{
							Menu_Array[ ANTI_SLAM ].context 	 = Menu_Array[ ANTI_SLAM ].sub_context_1;
							Menu_Array[ ANTI_SLAM ].current_item = Menu_Array[ ANTI_SLAM ].sub_context_1;

							ItemCopy( ANTI_SLAM, Menu_Array[ ANTI_SLAM ].sub_context_1, 0, 1 );

							switch ( Menu_Array[ ANTI_SLAM ].sub_context_4 )
							{
								case 1: { Anti_Slam_Increment = 1000; break; }
								case 2: { Anti_Slam_Increment = 100;  break; }
								case 3: { Anti_Slam_Increment = 10;	  break; }
								case 4: { Anti_Slam_Increment = 1;	  break; }
							}

							while( 1 )
							{
								WriteLCD_LineCentered( Menu_Array[ ANTI_SLAM ].item[ Menu_Array[ ANTI_SLAM ].context ], 1 );
								UpdateLCD();

								// read controls
								do
								{	encoder_delta = ReadInputs( &inputs, 0 );

								} while (encoder_delta == 0 && inputs == 0 );

								// change menu value
								if( encoder_delta != 0 )
								{
									const int new_item = Menu_Array[ ANTI_SLAM ].current_item + encoder_delta;

									if( new_item > Menu_Array[ ANTI_SLAM ].item_count ) continue;
									if( new_item < 1 ) continue;

									Menu_Array[ ANTI_SLAM ].current_item = new_item;

									switch( new_item )
									{
										case 1: { Menu_Array[ ANTI_SLAM ].context = ANTI_SLAM_DISABLED;		break;	}
										case 2: { Menu_Array[ ANTI_SLAM ].context = ANTI_SLAM_ENABLED;		break;	}

										case 3: { Menu_Array[ ANTI_SLAM ].context = ANTI_SLAM_RAISE_GATE;	break;	}
										case 4: { Menu_Array[ ANTI_SLAM ].context = ANTI_SLAM_DROP_GATE;	break;	}

										case 5: { Menu_Array[ ANTI_SLAM ].context = ANTI_SLAM_START_DELAY;	break;	}
										case 6: { Menu_Array[ ANTI_SLAM ].context = ANTI_SLAM_PULSE_WIDTH;	break;	}
										case 7: { Menu_Array[ ANTI_SLAM ].context = ANTI_SLAM_INCREMENT;	break;	}

									}
									continue;
								}

								// check for exit
								WaitForButtonUp();

								if( Menu_Array[ ANTI_SLAM ].context == ANTI_SLAM_DISABLED )
								{
									Menu_Array[ ANTI_SLAM ].sub_context_1 = ANTI_SLAM_DISABLED;
								}
								else if( Menu_Array[ ANTI_SLAM ].context == ANTI_SLAM_ENABLED )
								{
									Menu_Array[ ANTI_SLAM ].sub_context_1 = ANTI_SLAM_ENABLED;
								}
								else if( Menu_Array[ ANTI_SLAM ].context == ANTI_SLAM_RAISE_GATE )
								{
									if( Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_PROGATE )
										SetGatePowerUpProGate();
									else
										SetGatePowerOn();
								}
								else if( Menu_Array[ ANTI_SLAM ].context == ANTI_SLAM_DROP_GATE )
								{
									DropGate( 1 );
								}
								else if( (Menu_Array[ ANTI_SLAM ].context == ANTI_SLAM_START_DELAY) || (Menu_Array[ ANTI_SLAM ].context == ANTI_SLAM_PULSE_WIDTH) )
								{
									int *context = & Menu_Array[ ANTI_SLAM ].sub_context_2;

									if ( Menu_Array[ ANTI_SLAM ].context == ANTI_SLAM_START_DELAY )
									{
										WriteLCD_LineCentered( "SET START DELAY", 0 );
										context = & Menu_Array[ ANTI_SLAM ].sub_context_2;
									}
									else if ( Menu_Array[ ANTI_SLAM ].context == ANTI_SLAM_PULSE_WIDTH )
									{
										WriteLCD_LineCentered( "SET PULSE WIDTH", 0 );
										context = & Menu_Array[ ANTI_SLAM ].sub_context_3;
									}

									while( 1 )
									{
										// add cursor
										if( *context < 10 )
										{
											sprintf( Menu_Array[ ANTI_SLAM ].item[ 0 ], "\1 0.000%d seconds \2", *context );
										}
										else if( *context  < 100 )
										{
											sprintf( Menu_Array[ ANTI_SLAM ].item[ 0 ], "\1 0.00%d seconds \2", *context );
										}
										else  if( *context  < 1000 )
										{
											sprintf( Menu_Array[ ANTI_SLAM ].item[ 0 ], "\1 0.0%d seconds \2", *context );
										}
										else if( *context < 10000 )
										{
											sprintf( Menu_Array[ ANTI_SLAM ].item[ 0 ], "\1 0.%d seconds \2", *context );
										}

										WriteLCD_LineCentered( Menu_Array[ ANTI_SLAM ].item[ 0 ], 1 );
										UpdateLCD();

										// read controls
										do
										{	encoder_delta = ReadInputs( &inputs, 0 );

										} while ( encoder_delta == 0 && inputs == 0 );

										// change menu value
										if( encoder_delta != 0 )
										{
											int new_value = *context + encoder_delta * Anti_Slam_Increment;
											if( new_value >= 0 && new_value <= 10000 ) *context = new_value;

											continue;
										}

										// check for exit
										WaitForButtonUp();

										// restore the caption back to the correct aux config
										WriteLCD_LineCentered( Menu_Array[ ANTI_SLAM ].caption, 0 );
										UpdateLCD();

										SaveEverythingToFlashMemory( 0 );
										ReadInputs( &inputs, 0 );
										inputs = 0;

										break;
									}
									continue;
								}
								else if( Menu_Array[ ANTI_SLAM ].context == ANTI_SLAM_INCREMENT )
								{
									WriteLCD_LineCentered( "SET INCREMENT", 0 );

									while( 1 )
									{
										// add cursor
										switch ( Menu_Array[ ANTI_SLAM ].sub_context_4 )
										{
											case 1: { strcpy( Menu_Array[ ANTI_SLAM ].item[ 0 ], "\1 0.1 \2" );		Anti_Slam_Increment = 1000;	break; }
											case 2: { strcpy( Menu_Array[ ANTI_SLAM ].item[ 0 ], "\1 0.01 \2" );	Anti_Slam_Increment = 100;	break; }
											case 3: { strcpy( Menu_Array[ ANTI_SLAM ].item[ 0 ], "\1 0.001 \2" );	Anti_Slam_Increment = 10;	break; }
											case 4: { strcpy( Menu_Array[ ANTI_SLAM ].item[ 0 ], "\1 0.0001 \2" );	Anti_Slam_Increment = 1;	break; }
										}

										WriteLCD_LineCentered( Menu_Array[ ANTI_SLAM ].item[ 0 ], 1 );
										UpdateLCD();

										// read controls
										do
										{	encoder_delta = ReadInputs( &inputs, 0 );

										} while ( encoder_delta == 0 && inputs == 0 );

										// change menu value
										if( encoder_delta != 0 )
										{
											int new_value = Menu_Array[ ANTI_SLAM ].sub_context_4 + encoder_delta;

											if( new_value >= 1 && new_value <= 4 )
											{
												Menu_Array[ ANTI_SLAM ].sub_context_4 = new_value;
											}

											continue;
										}

										// check for exit
										WaitForButtonUp();

										// restore the caption back to the correct aux config
										WriteLCD_LineCentered( Menu_Array[ ANTI_SLAM ].caption, 0 );
										UpdateLCD();

										SaveEverythingToFlashMemory( 0 );
										ReadInputs( &inputs, 0 );
										inputs = 0;

										break;
									}
									continue;
								}

								WaitForButtonUp();

								ItemCopy( ANTI_SLAM, Menu_Array[ ANTI_SLAM ].sub_context_1, 0, 1 );
								WriteLCD_LineCentered( Menu_Array[ ANTI_SLAM ].item[ 0 ], 1 );
								UpdateLCD();

								SaveEverythingToFlashMemory( 0 );
								ReadInputs( &inputs, 0 );
								inputs = 0;

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
 *
 */
			case WAIT_FOR_SENSORS:
			{
				DisableSpeedMeasurement();
				ResetSpeedMeasurement();
				EnableSpeedMeasurement();

				if( Cancel_Timing == 1 )
				{
					Timing_Active = 0;

					// go to main menu
					Device_State = WAIT_FOR_USER;
					break;
				}

				int inputs = 0;

				unsigned int sensor_1 = 0;
				unsigned int sensor_2 = 0;

				unsigned int timeout_1 = 0;
				unsigned int timeout_2 = 0;

				// treat sprint timer as disabled since there is nothing to do - will use these to restore
				unsigned int aux_1_config = Menu_Array[ AUX_1_CONFIG ].context;
				unsigned int aux_2_config = Menu_Array[ AUX_2_CONFIG ].context;

				if( Menu_Array[ AUX_1_CONFIG ].context == AUX_SPRINT_TIMER )
					Menu_Array[ AUX_1_CONFIG ].context = AUX_DISABLED;

				if( Menu_Array[ AUX_2_CONFIG ].context == AUX_SPRINT_TIMER )
					Menu_Array[ AUX_2_CONFIG ].context = AUX_DISABLED;

				do
				{
					ReadInputs( &inputs, 1 );

					// return a time if there is one from being tripped
					unsigned int sensor_1A = CheckSensor( AUX_1_SENSOR_A );
					unsigned int sensor_1B = CheckSensor( AUX_1_SENSOR_B );
					unsigned int sensor_2A = CheckSensor( AUX_2_SENSOR_A );
					unsigned int sensor_2B = CheckSensor( AUX_2_SENSOR_B );

					if( Menu_Array[ AUX_1_CONFIG ].context == AUX_DISABLED )
					{
						if( Menu_Array[ AUX_2_CONFIG ].context == AUX_TIME )
						{
							if( DoTimeAndDisabled( AUX_2_CONFIG, sensor_2A, sensor_2B, "TIMING AUX 2", "AUX 2 ELAPSED TIME" ) == 1 ) continue;
							break;
						}
						else if( Menu_Array[ AUX_2_CONFIG ].context == AUX_TIME_SPEED )
						{
							if( DoSpeedAndDisabled( AUX_2_CONFIG, sensor_2A, sensor_2B, Aux_2_Sensor_Spacing, "TIMING AUX 2", "AUX 2 ELAPSED TIME", & timeout_1 ) == 1 ) continue;
							break;
						}
					}
					else if( Menu_Array[ AUX_2_CONFIG ].context == AUX_DISABLED )
					{
						if( Menu_Array[ AUX_1_CONFIG ].context == AUX_TIME )
						{
							if( DoTimeAndDisabled( AUX_1_CONFIG, sensor_1A, sensor_1B, "TIMING AUX 1", "AUX 1 ELAPSED TIME" ) == 1 ) continue;
							break;
						}
						else if( Menu_Array[ AUX_1_CONFIG ].context == AUX_TIME_SPEED )
						{
							if( DoSpeedAndDisabled( AUX_1_CONFIG, sensor_1A, sensor_1B, Aux_1_Sensor_Spacing, "TIMING AUX 1", "AUX 1 ELAPSED TIME", & timeout_1 ) == 1 ) continue;
							break;
						}
					}
					else if( Menu_Array[ AUX_1_CONFIG ].context == AUX_TIME )
					{
						if( Menu_Array[ AUX_2_CONFIG ].context == AUX_TIME )
						{
							char line_0[ DISPLAY_WIDTH ] = SPACES;
							char line_1[ DISPLAY_WIDTH ] = SPACES;

							if( sensor_1 == 0 && (sensor_1A | sensor_1B) != 0 ) sensor_1 = (sensor_1A | sensor_1B);
							if( sensor_2 == 0 && (sensor_2A | sensor_2B) != 0 ) sensor_2 = (sensor_2A | sensor_2B);

							GetTimesString( sensor_1, sensor_2, line_0, line_1 );

							WriteLCD_Line( line_0, 0 );
							WriteLCD_Line( line_1, 1 );
							UpdateLCD();

							if( sensor_1 == 0 || sensor_2 == 0 ) continue;
#ifdef NUMBERS
							const int time_start = 8400;
							const int time_size  = 7500;
#endif
							char top_line[ DISPLAY_WIDTH ] 		= ZEROS;
							char bottom_line[ DISPLAY_WIDTH ]	= ZEROS;

							if( sensor_1 > sensor_2 )
							{
								GetTimeString( sensor_2, bottom_line );
								AddTimeToTimerHistory( AUX_2_CONFIG, sensor_2, bottom_line );

								GetTimeString( sensor_1, top_line );
								AddTimeToTimerHistory( AUX_1_CONFIG, sensor_1, top_line );
#ifdef NUMBERS
								if( Menu_Array[ AUTO_ANNOUNCE_TIMES ].context == AUTO_ANNOUNCE_TIMES_ENABLED )
								{
									PlaySilence( 100, 1 );
									PlaySample24khz( LastTimeWasData + time_start, 0, time_size, 1 );
									PlayDigit( 1 );
									PlayTimeOrPercent( Timer_History[ 0 ].elapsed_time / 10, PLAY_TIME );

									PlaySample24khz( LastTimeWasData + time_start, 0, time_size, 1 );
									PlayDigit( 2 );
									PlayTimeOrPercent( Timer_History[ 1 ].elapsed_time / 10, PLAY_TIME );
									InitAudio();
								}
#endif
							}
							else
							{
								GetTimeString( sensor_1, bottom_line );
								AddTimeToTimerHistory( AUX_1_CONFIG, sensor_1, bottom_line );

								GetTimeString( sensor_2, top_line );
								AddTimeToTimerHistory( AUX_2_CONFIG, sensor_2, top_line );
#ifdef NUMBERS
								if( Menu_Array[ AUTO_ANNOUNCE_TIMES ].context == AUTO_ANNOUNCE_TIMES_ENABLED )
								{
									PlaySilence( 100, 1 );
									PlaySample24khz( LastTimeWasData + time_start, 0, time_size, 1 );
									PlayDigit( 1 );
									PlayTimeOrPercent( Timer_History[ 1 ].elapsed_time / 10, PLAY_TIME );

									PlaySample24khz( LastTimeWasData + time_start, 0, time_size, 1 );
									PlayDigit( 2 );
									PlayTimeOrPercent( Timer_History[ 0 ].elapsed_time / 10, PLAY_TIME );
									InitAudio();
								}
#endif
							}

							Delay( 3000 );

							break;
						}
						else if( Menu_Array[ AUX_2_CONFIG ].context == AUX_TIME_SPEED )
						{
							if( DoTimeAndSpeed( AUX_TIME, sensor_1A, sensor_1B, sensor_2A, sensor_2B, & timeout_1 ) == 1 ) continue;
							break;
						}
					}
					else if( Menu_Array[ AUX_2_CONFIG ].context == AUX_TIME )
					{
						if( Menu_Array[ AUX_1_CONFIG ].context == AUX_TIME_SPEED )
						{
							if( DoTimeAndSpeed( AUX_TIME_SPEED, sensor_1A, sensor_1B, sensor_2A, sensor_2B, & timeout_1 ) == 1 ) continue;
							break;
						}
					}
					else if( Menu_Array[ AUX_1_CONFIG ].context == AUX_TIME_SPEED && Menu_Array[ AUX_2_CONFIG ].context == AUX_TIME_SPEED )
					{
						char line_0[ DISPLAY_WIDTH ] = SPACES;
						char line_1[ DISPLAY_WIDTH ] = SPACES;

						if( sensor_1 == 0 && (sensor_1A | sensor_1B) != 0 ) sensor_1 = (sensor_1A | sensor_1B);
						if( sensor_2 == 0 && (sensor_2A | sensor_2B) != 0 ) sensor_2 = (sensor_2A | sensor_2B);

						if( sensor_1A == 0 || sensor_1B == 0 || sensor_2A == 0 || sensor_2B == 0 )
						{
							GetTimesString( sensor_1, sensor_2, line_0, line_1 );

							WriteLCD_Line( line_0, 0 );
							WriteLCD_Line( line_1, 1 );
							UpdateLCD();
						}

						// need separate timeouts for each sensor
						if( (timeout_1 == 0) && ((sensor_1A == 0 && sensor_1B != 0) || (sensor_1A != 0 && sensor_1B == 0)) )
						{
							timeout_1 = Timer6_Tick + SENSOR_TIMEOUT_PERIOD;
						}

						if( (timeout_2 == 0) && ((sensor_2A == 0 && sensor_2B != 0) || (sensor_2A != 0 && sensor_2B == 0)) )
						{
							timeout_2 = Timer6_Tick + SENSOR_TIMEOUT_PERIOD;
						}

						// check for timeout
						const unsigned int aux_1_faulty = ( Timer6_Tick > timeout_1 && ((sensor_1A == 0 && sensor_1B != 0) || (sensor_1A != 0 && sensor_1B == 0)) );
						const unsigned int aux_2_faulty = ( Timer6_Tick > timeout_2 && ((sensor_2A == 0 && sensor_2B != 0) || (sensor_2A != 0 && sensor_2B == 0)) );

						// TODO: specify whether it is sensor_xA or sensor_xB that is faulty
						// reduce options for anti-slam and make sure there are no bugs
						// after committing these changes update to version 0.9.4
						if( aux_1_faulty != 0 || aux_2_faulty != 0 )
						{
							WriteLCD_LineCentered( "TIMING ABORTED", 0 );

							if( aux_1_faulty )
								WriteLCD_LineCentered( "AUX 1 sensor faulty", 1 );
							else
								WriteLCD_LineCentered( "AUX 2 sensor faulty", 1 );
							UpdateLCD();

							Delay( 4000 );

							break;
						}

						if( sensor_1A == 0 || sensor_1B == 0 || sensor_2A == 0 || sensor_2B == 0 ) continue;

						unsigned int elapsed_time_1 = 0, speed_integer_1 = 0, speed_fractional_1 = 0;
						unsigned int elapsed_time_2 = 0, speed_integer_2 = 0, speed_fractional_2 = 0;

						if( Cancel_Timing == 0 )
						{
							float speed_1, speed_2;

							CalculateSpeed( Aux_1_Speed_Tick_Delta, sensor_1A, sensor_1B, Aux_1_Sensor_Spacing, & elapsed_time_1, & speed_1, & speed_integer_1, & speed_fractional_1 );
							CalculateSpeed( Aux_2_Speed_Tick_Delta, sensor_2A, sensor_2B, Aux_2_Sensor_Spacing, & elapsed_time_2, & speed_2, & speed_integer_2, & speed_fractional_2 );

							if( elapsed_time_1 > elapsed_time_2 )
							{
								ProcessTimeAndSpeed( AUX_2_CONFIG, elapsed_time_2, speed_2, speed_integer_2, speed_fractional_2 );
								ProcessTimeAndSpeed( AUX_1_CONFIG, elapsed_time_1, speed_1, speed_integer_1, speed_fractional_1 );

								WriteLCD_LineCentered( Timer_History[ 0 ].time_speed_string, 0 );
								WriteLCD_LineCentered( Timer_History[ 1 ].time_speed_string, 1 );
							}
							else
							{
								ProcessTimeAndSpeed( AUX_1_CONFIG, elapsed_time_1, speed_1, speed_integer_1, speed_fractional_1 );
								ProcessTimeAndSpeed( AUX_2_CONFIG, elapsed_time_2, speed_2, speed_integer_2, speed_fractional_2 );

								WriteLCD_LineCentered( Timer_History[ 1 ].time_speed_string, 0 );
								WriteLCD_LineCentered( Timer_History[ 0 ].time_speed_string, 1 );
							}

							UpdateLCD();
#ifdef NUMBERS
							if( Menu_Array[ AUTO_ANNOUNCE_TIMES ].context == AUTO_ANNOUNCE_TIMES_ENABLED )
							{
								int first_time, second_time;

								if( elapsed_time_1 > elapsed_time_2 )
								{
									first_time	= 0;
									second_time = 1;
								}
								else
								{
									first_time	= 1;
									second_time = 0;
								}

								const int time_start = 8400;
								const int time_size  = 7500;

								PlaySilence( 100, 1 );
								PlaySample24khz( LastTimeWasData + time_start, 0, time_size, 1 );
								PlayDigit( 1 );
								PlayTimeOrPercent( Timer_History[ first_time ].elapsed_time / 10, PLAY_TIME );
								PlaySample24khz( AtData, 0, AT_SIZE, 1 );
								PlaySpeed( Timer_History[ first_time ].speed_integer, Timer_History[ first_time ].speed_fractional );


								PlaySample24khz( LastTimeWasData + time_start, 0, time_size, 1 );
								PlayDigit( 2 );
								PlayTimeOrPercent( Timer_History[ second_time ].elapsed_time / 10, PLAY_TIME );
								PlaySample24khz( AtData, 0, AT_SIZE, 1 );
								PlaySpeed( Timer_History[ second_time ].speed_integer, Timer_History[ second_time ].speed_fractional );

								InitAudio();
							}
#endif
							Delay( 3000 );
						}
						break;
					}
				} while( inputs != BUTTON_E && inputs != BUTTON_C && Cancel_Timing != 1 );

				// only save if timing wasn't cancelled
				if( Cancel_Timing != 1 )
				{
					SaveEverythingToFlashMemory( 0 );
					ReadInputs( &inputs, 0 );
					inputs = 0;
				}

				// restore contexts to handle case when one aux port is configured as a sprint timer
				Menu_Array[ AUX_1_CONFIG ].context = aux_1_config;
				Menu_Array[ AUX_2_CONFIG ].context = aux_2_config;

				if( inputs == BUTTON_E || inputs == BUTTON_C || Cancel_Timing == 1  )
				{
					WriteLCD_LineCentered( "TIMING CANCELED", 0 );
					WriteLCD_LineCentered( "User Aborted", 1 );

					UpdateLCD();

					Delay( 3000 );
				}

				Timing_Active = 0;

				// go to main menu
				Device_State = WAIT_FOR_USER;
				break;
			}

			DisableSpeedMeasurement();
		}
	}
}

void DropGate( int test )
{
	Dropping_Gate	  = 1;

	Cadence_Cancelled = 0;
	Cancel_Timing	  = 0;

#ifdef CADENCE
	uint32_t fade_volume = 0;
	int		 volume		 = 0;
	int		 step		 = 0;
	int		 count_step  = 0;
#endif

	if( test == 0 )
	{
#ifdef CADENCE
		// check to see if there is audio coming in
		ADC_SoftwareStartConv( ADC2 );
		while( !ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC) );

		if( ADC_GetConversionValue( ADC2 ) > 0xF00 )
		{
			fade_volume = 1;

			WriteLCD_LineCentered( "FADING EXT AUDIO OUT", 0 );
			UpdateLCD();

			// fade music volume out TODO: make this a logarithmic!
			volume = (128 * Menu_Array[ AUDIO_IN_VOLUME ].context) / 100;
			step = volume / 20 + 1;
			count_step = volume / 3;

			for( ; volume >= 0 && Cadence_Cancelled == 0; volume -= step )
			{
				SetAttenuator( 0x0000 | volume );
				Delay( 80 );

				if( volume > count_step * 2 )
					WriteLCD_LineCentered( "-=\1 3 \2=-", 1 );
				else if( volume > count_step )
					WriteLCD_LineCentered( "-=\1 2 \2=-", 1 );
				else
					WriteLCD_LineCentered( "-=\1 1 \2=-", 1 );

				UpdateLCD();
			}
		}
#endif

		WriteLCD_LineCentered( "CADENCE STARTED", 0 );
		WriteLCD_LineCentered( SPACES, 1 );
		UpdateLCD();

		SetAttenuator( 0x0000 );

		Cadence_Cancelled= 0;
		Cancel_Timing	 = 0;

		PlaySilence( 100, 0 );

#ifdef CADENCE
		// start playing the voice part of the cadence
		PlaySample24khz( OkRidersData,		0, OK_RIDERS_SIZE,		0 );	// t = 653ms
		PlaySilence( 140, 0 );	// time to wait between "RIDERS" and "RANDOM"
		PlaySample24khz( RandomStartData,	0, RANDOM_START_SIZE,	0 );	// t = 707ms

		PlaySilence( 1800, 0 );	// UCI spec

		PlaySample24khz( RidersReadyData,	0, RIDERS_READY_SIZE,	0 );	// t = 1058ms
		PlaySilence( 204, 0 );  // time to wait in between "READY" and "WATCH"

#endif

		if( Cadence_Cancelled == 1 )
		{
			Cancel_Timing = 1;

			WriteLCD_LineCentered( "GATE DROP ABORTED", 0 );
			WriteLCD_Line(	SPACES, 1 );
			UpdateLCD();

			InitAudio();

			// play cancel tones
			PlaySilence( 100, 1 );
			PlayTone( 220, Square740HzData, SQUARE_740HZ_SIZE, 1 );
			PlayTone( 440, Square680HzData, SQUARE_680HZ_SIZE, 1 );

			// check for exit
			WaitForButtonUp();
		}
		else
		{
#ifdef CADENCE
			PlaySample24khz( WatchTheGateData,	0, WATCH_THE_GATE_SIZE,	0 );	// t = 738ms
#endif
		}

		// total time to play above samples and delays is 5.3 seconds at this point

		// wait 0.10 to 2.7 seconds
		PlaySilence( 100 + GetRandomNumber() % 2600, 0 );
	}

	if( Cadence_Cancelled == 1 )
	{
		Cancel_Timing 	  = 1;
		Cadence_Cancelled = 0;

		WriteLCD_LineCentered( "GATE DROP ABORTED", 0 );
		WriteLCD_Line( SPACES, 1 );
	    UpdateLCD();

		// Get rid of the static that occurs intermittently
		InitAudio();

	    Delay( 2000 );
	}
	else
	{
		if( Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_SOLENOID )
		{
			PulseSolenoid( 500 );	// pulsed after gate start delay processed by interrupt 6
		}
		else if( Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_MAGNET )
		{
			StartGatePowerOffTimer();  // gate power turned off after Gate_Drop_Delay processed by interrupt 6
		}
		else if( (Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_AIR_RAM) || ( Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_PROGATE ) )
		{
			StartGatePowerOffTimer();  // gate turned off after Gate_Drop_Delay processed by interrupt 6

			if( test == 1 ) PlaySilence( 60, 0 );	// HORRIBLE hacky fix because of not having sound done through DMA
		}

		if( Menu_Array[ AUX_1_CONFIG ].context == AUX_TIME_SPEED || Menu_Array[ AUX_2_CONFIG ].context == AUX_TIME_SPEED )
		{
			DisableSpeedMeasurement();
			ResetSpeedMeasurement();
			EnableSpeedMeasurement();
		}

		// play the cadence tones and light the lights
		GPIO_SetBits( GPIOD, GPIO_Pin_12 );	// RED light ON
		PlayTone( 60, Square632HzData, SQUARE_632HZ_SIZE, 1 );
		PlaySilence( 60, 0 );

		GPIO_SetBits( GPIOD, GPIO_Pin_13 );	// AMBER 1 light ON
		PlayTone( 60, Square632HzData, SQUARE_632HZ_SIZE, 1 );
		PlaySilence( 60, 0 );

		GPIO_SetBits( GPIOD, GPIO_Pin_14 );	// AMBER 2 light ON
		PlayTone( 60, Square632HzData, SQUARE_632HZ_SIZE, 1 );
		PlaySilence( 60, 0 );

		GPIO_SetBits( GPIOD, GPIO_Pin_15 );	// GREEN light ON

		// reset system ticks, will be displayed for timing and used with delays

		Timer6_Tick 		= 0;

		Aux_1_Infrared_Poll = 0;
		Aux_2_Infrared_Poll = 0;

		// initialize the sensors so the tick value is set only once from the timer 6 interrupt
		Aux_1_Sensor_A_Tick = 0;
		Aux_1_Sensor_B_Tick = 0;
		Aux_2_Sensor_A_Tick = 0;
		Aux_2_Sensor_B_Tick = 0;

		// clear lap counters
		Aux_1_Lap_Counter	= 0;
		Aux_2_Lap_Counter	= 0;

		Timing_Active		= 1;

		PlayTone( 2250, Square632HzData, SQUARE_632HZ_SIZE, 1 );

		// turn all the lights off
		GPIO_ResetBits( GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 );

		// update gate drop counter
		Menu_Array[ TOTAL_GATE_DROPS ].context += 1;
		sprintf( Menu_Array[ TOTAL_GATE_DROPS ].item[ 0 ], "%d", Menu_Array[ TOTAL_GATE_DROPS ].context );
	}

	// Get rid of the static that occurs intermittently
	InitAudio();

	if( test == 0 )
	{
#ifdef CADENCE
		if( fade_volume == 1 )
		{
			WriteLCD_LineCentered( "FADING EXT AUDIO IN", 0 );
			WriteLCD_Line( SPACES, 1 );
		    UpdateLCD();

			int max_volume = (128 * Menu_Array[ AUDIO_IN_VOLUME ].context) / 100;

			for( volume = 0 ; volume < max_volume; volume += step )
			{
				SetAttenuator( 0x0000 | volume );
				Delay( 80 );

				if( volume > count_step * 2 )
					WriteLCD_LineCentered( "-=\1 3 \2=-", 1 );
				else if( volume > count_step )
					WriteLCD_LineCentered( "-=\1 2 \2=-", 1 );
				else
					WriteLCD_LineCentered( "-=\1 1 \2=-", 1 );

			    UpdateLCD();
			}
		}
#endif
	}

	if( Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_PROGATE )
		SetGatePowerOff();

	Dropping_Gate = 0;
}

void DoReactionGame( const unsigned int player_count )
{
	// for calculating average times
	unsigned int p1_game_divisor = 1;
	unsigned int p2_game_divisor = 1;

	unsigned int p1_avg_total = 0;
	unsigned int p2_avg_total = 0;

	Timing_Active = 1;

	while( 1 )
	{
		WriteLCD_LineCentered( "GET READY!", 0 );
		WriteLCD_LineCentered( SPACES, 1 );
		UpdateLCD();

		Delay( 500 );

		PlaySilence( 100, 0 );

#ifdef CADENCE
		// start playing the voice part of the cadence
		PlaySample24khz( OkRidersData,		0, OK_RIDERS_SIZE,		0 );
		PlaySilence( 140, 0 );

		PlaySample24khz( RandomStartData,	0, RANDOM_START_SIZE,	0 );
		PlaySilence( 1800, 0 );

		PlaySample24khz( RidersReadyData,	0, RIDERS_READY_SIZE,	0 );
		PlaySilence( 204, 0 );
#endif

#ifdef CADENCE
		PlaySample24khz( WatchTheGateData,	0, WATCH_THE_GATE_SIZE,	0 );
#endif

		// wait 0.10 to 2.7 seconds
		PlaySilence( 100 + GetRandomNumber() % 2600, 0 );

		// initialize the sensors so the tick value is set only once from the timer 6 interrupt
		Aux_1_Sensor_A_Tick = 0;
		Aux_1_Sensor_B_Tick = 0;
		Aux_2_Sensor_A_Tick = 0;
		Aux_2_Sensor_B_Tick = 0;

		Timer6_Tick			= 0;

		// since I can't play audio on a timer interrupt I can't update the display while the sound is playing
		// and I can't write to the LCD on a timer interrupt as well because it uses the timers for delays
		// and both timer 6 and timer 7 are not independent of one another. Talk about sucking...
		Gate_Drop_Delay = Menu_Array[ GATE_DROPS_ON ].context * 10;
		Start_Reaction_Timing = 1;

		// play the cadence tones and light the lights
		GPIO_SetBits( GPIOD, GPIO_Pin_12 );	// RED light ON
		PlayTone( 60, Square632HzData, SQUARE_632HZ_SIZE, 1 );
		PlaySilence( 60, 0 );

		GPIO_SetBits( GPIOD, GPIO_Pin_13 );	// AMBER 1 light ON
		PlayTone( 60, Square632HzData, SQUARE_632HZ_SIZE, 1 );
		PlaySilence( 60, 0 );

		GPIO_SetBits( GPIOD, GPIO_Pin_14 );	// AMBER 2 light ON
		PlayTone( 60, Square632HzData, SQUARE_632HZ_SIZE, 1 );
		PlaySilence( 60, 0 );

		GPIO_SetBits( GPIOD, GPIO_Pin_15 );	// GREEN light ON

		PlayTone( 2250, Square632HzData, SQUARE_632HZ_SIZE, 1 );
		InitAudio();

		// turn all the lights off
		GPIO_ResetBits( GPIOD, GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15 );

		unsigned int aux1_sensor = ( Aux_1_Sensor_A_Tick | Aux_1_Sensor_B_Tick );
		unsigned int aux2_sensor = ( Aux_2_Sensor_A_Tick | Aux_2_Sensor_B_Tick );

		if( player_count == 1 )
		{
			if( aux1_sensor == 0 )
			{
				WriteLCD_LineCentered( "TOO SLOW", 0 );
			}
			else if( aux1_sensor == 0xDEAD || aux1_sensor <= 1 )
			{
				WriteLCD_LineCentered( "HIT THE GATE", 0 );
			}
			else
			{
				aux1_sensor -= Menu_Array[ GATE_DROPS_ON ].context * 10;
				PrintElapsedTime( aux1_sensor, 1, 0 );
			}
		}
		else
		{
			if( aux1_sensor == 0 )
			{
				WriteLCD_LineCentered( "TOO SLOW", 0 );
			}
			else if( aux1_sensor == 0xDEAD || aux1_sensor <= 1 )
			{
				WriteLCD_LineCentered( "HIT THE GATE", 0 );
			}
			else
			{
				aux1_sensor -= Menu_Array[ GATE_DROPS_ON ].context * 10;
				PrintElapsedTime( aux1_sensor, 1, 0 );
			}

			if( aux2_sensor == 0 )
			{
				WriteLCD_LineCentered( "TOO SLOW", 1 );
			}
			else if( aux2_sensor == 0xDEAD || aux2_sensor <= 1 )
			{
				WriteLCD_LineCentered( "HIT THE GATE", 1 );
			}
			else
			{
				aux2_sensor -= Menu_Array[ GATE_DROPS_ON ].context * 10;
				PrintElapsedTime( aux2_sensor, 1, 1 );
			}
		}

		UpdateLCD();

		Delay( 2000 );

		if( aux1_sensor != 0xDEAD && aux1_sensor > 1 )
		{
			if( p1_game_divisor == 1 && Reaction_Game_P1_BEST != Reaction_Game_P1_WORST )
			{
				p1_avg_total += aux1_sensor;

				if( Reaction_Game_P1_AVG == 0 )
					Reaction_Game_P1_AVG = ( Reaction_Game_P1_BEST + aux1_sensor + Reaction_Game_P1_WORST ) / 3;
				else
					Reaction_Game_P1_AVG = ( Reaction_Game_P1_BEST + Reaction_Game_P1_AVG + Reaction_Game_P1_WORST + aux1_sensor ) / 4;
			}
			else
			{
				p1_avg_total += aux1_sensor;
				Reaction_Game_P1_AVG = p1_avg_total / p1_game_divisor;
			}

			Reaction_Game_P1_WORST	= ( aux1_sensor > Reaction_Game_P1_WORST ) ? aux1_sensor : Reaction_Game_P1_WORST;

			if( Reaction_Game_P1_BEST == 0 )
				Reaction_Game_P1_BEST = aux1_sensor;
			else
				Reaction_Game_P1_BEST = ( aux1_sensor < Reaction_Game_P1_BEST  ) ? aux1_sensor : Reaction_Game_P1_BEST;

			p1_game_divisor += 1;
		}

		if( aux2_sensor != 0xDEAD && aux2_sensor > 1 && player_count == 2 )
		{
			if( p2_game_divisor == 1 && Reaction_Game_P2_BEST != Reaction_Game_P2_WORST )
			{
				p2_avg_total += aux2_sensor;

				if( Reaction_Game_P2_AVG == 0 )
					Reaction_Game_P2_AVG = ( Reaction_Game_P2_BEST + aux2_sensor + Reaction_Game_P2_WORST ) / 3;
				else
					Reaction_Game_P2_AVG = ( Reaction_Game_P2_BEST + Reaction_Game_P2_AVG + Reaction_Game_P2_WORST + aux2_sensor ) / 4;
			}
			else
			{
				p2_avg_total += aux2_sensor;
				Reaction_Game_P2_AVG = p2_avg_total / p2_game_divisor;
			}

			Reaction_Game_P2_WORST	= ( aux2_sensor > Reaction_Game_P2_WORST ) ? aux2_sensor : Reaction_Game_P2_WORST;

			if( Reaction_Game_P2_BEST == 0 )
				Reaction_Game_P2_BEST = aux2_sensor;
			else
				Reaction_Game_P2_BEST = ( aux2_sensor < Reaction_Game_P2_BEST  ) ? aux2_sensor : Reaction_Game_P2_BEST;

			p2_game_divisor += 1;
		}

		unsigned int minutes_best  = 0, seconds_best  = 0, tens_place_best	= 0, hund_place_best  = 0, thou_place_best  = 0;
		unsigned int minutes_avg   = 0, seconds_avg	  = 0, tens_place_avg   = 0, hund_place_avg   = 0, thou_place_avg   = 0;
		unsigned int minutes_worst = 0, seconds_worst = 0, tens_place_worst	= 0, hund_place_worst = 0, thou_place_worst = 0;

	    GetTimeFromTicks( Reaction_Game_P1_BEST,  &minutes_best,  &seconds_best,  &tens_place_best,  &hund_place_best,  &thou_place_best  );
	    GetTimeFromTicks( Reaction_Game_P1_AVG,	  &minutes_avg,   &seconds_avg,   &tens_place_avg,   &hund_place_avg,   &thou_place_avg   );
	    GetTimeFromTicks( Reaction_Game_P1_WORST, &minutes_worst, &seconds_worst, &tens_place_worst, &hund_place_worst, &thou_place_worst );

// TODO: Comment this back in once the SMT version is complete
//		SaveEverythingToFlashMemory( 0 );

		char line0[ DISPLAY_WIDTH ];

		if( player_count == 1 )
		{
			sprintf( line0, "%d.%d%d%d  %d.%d%d%d  %d.%d%d%d ",
					        seconds_best,	tens_place_best,  hund_place_best,	thou_place_best,
					        seconds_avg,	tens_place_avg,	  hund_place_avg,	thou_place_avg,
					        seconds_worst,	tens_place_worst, hund_place_worst,	thou_place_worst );

			WriteLCD_Line( "BEST    AVG   WORST ", 0 );
			WriteLCD_Line( line0, 1 );
		}
		else
		{
			if( aux1_sensor == 0 && aux2_sensor == 0 )
			{
				WriteLCD_LineCentered( "No Winner", 0 );
				WriteLCD_LineCentered( "Both are too slow", 1 );
			}
			else if( ( aux1_sensor == 0xDEAD && aux2_sensor == 0xDEAD ) || ( aux1_sensor == 1 && aux2_sensor == 1 ) )
			{
				WriteLCD_LineCentered( "No Winner", 0 );
				WriteLCD_LineCentered( "Both hit the gate", 1 );
			}
			else if( (aux1_sensor == 1 && aux2_sensor == 0) || (aux1_sensor == 0 && aux2_sensor == 1 ) )
			{
				WriteLCD_LineCentered( "No Winner", 0 );
				WriteLCD_LineCentered( "Both had bad gates", 1 );
			}
			else if( aux1_sensor == aux2_sensor )
			{
				WriteLCD_LineCentered( "TIE GAME!!!", 0 );
				WriteLCD_LineCentered( "TIE GAME!!!", 1 );
			}
			else if( aux2_sensor <= 1 && aux1_sensor > 1 )
			{
				WriteLCD_LineCentered( "Player One WINS!", 0 );
				WriteLCD_LineCentered( "", 1 );

			}
			else if( aux1_sensor <= 1 && aux2_sensor > 1 )
			{
				WriteLCD_LineCentered( "", 0 );
				WriteLCD_LineCentered( "Player Two WINS!", 1 );
			}
			else if( (aux1_sensor < aux2_sensor) )
			{
				WriteLCD_LineCentered( "Player One WINS!", 0 );
				WriteLCD_LineCentered( "", 1 );
			}
			else if( (aux2_sensor < aux1_sensor) )
			{
				WriteLCD_LineCentered( "", 0 );
				WriteLCD_LineCentered( "Player Two WINS!", 1 );
			}
			else
			{
				WriteLCD_LineCentered( "NO", 0 );
				WriteLCD_LineCentered( "WINNER", 1 );
			}

			UpdateLCD();
			Delay( 2000 );

			sprintf( line0, "1: %d.%d%d%d %d.%d%d%d %d.%d%d%d",
					        seconds_best,	tens_place_best,  hund_place_best,	thou_place_best,
					        seconds_avg,	tens_place_avg,	  hund_place_avg,	thou_place_avg,
					        seconds_worst,	tens_place_worst, hund_place_worst,	thou_place_worst );

			WriteLCD_Line( line0, 0 );

		    GetTimeFromTicks( Reaction_Game_P2_BEST,  &minutes_best,  &seconds_best,  &tens_place_best,  &hund_place_best,  &thou_place_best  );
		    GetTimeFromTicks( Reaction_Game_P2_AVG,	  &minutes_avg,   &seconds_avg,   &tens_place_avg,   &hund_place_avg,   &thou_place_avg   );
		    GetTimeFromTicks( Reaction_Game_P2_WORST, &minutes_worst, &seconds_worst, &tens_place_worst, &hund_place_worst, &thou_place_worst );

			char line1[ DISPLAY_WIDTH ];
			sprintf( line1, "2: %d.%d%d%d %d.%d%d%d %d.%d%d%d",
					        seconds_best,	tens_place_best,  hund_place_best,	thou_place_best,
					        seconds_avg,	tens_place_avg,	  hund_place_avg,	thou_place_avg,
					        seconds_worst,	tens_place_worst, hund_place_worst,	thou_place_worst );
			WriteLCD_Line( line1, 1 );
		}

		UpdateLCD();

		int inputs = 0;

		do
		{	ReadInputs( &inputs, 1 );

		} while( inputs == 0 );

		if( inputs == BUTTON_E ) break;

		WaitForButtonUp();
	}

	Start_Reaction_Timing = 0;
	Timing_Active 		  = 0;
}

int LightTestCycle( const unsigned int allow_abort )
{
	unsigned int buffer[ 4 ];

	const float resolution = 50.0f;

	float theta1 = 0.000 + 0.6f;
	float theta2 = 0.333 + 0.6f;
	float theta3 = 0.999 + 0.6f;

	float one_y   = 0;
	float two_y   = 0;
	float three_y = 0;

	unsigned int i = 0;

	int abort_flag = 0;

	while( theta1 < (2 * 3.14159f) + 0.999f )
	{
		Delay( 7 );

		one_y   = resolution * sin( theta1 ) + resolution;
		two_y   = resolution * sin( theta2 ) + resolution;
		three_y = resolution * sin( theta3 ) + resolution;

		theta1 += 0.0425f;
		theta2 += 0.0425f;
		theta3 += 0.0425f;

		buffer[ 0 ] = buffer[ 1 ] = buffer[ 2 ] = buffer[ 3 ] = 0;

		i = (unsigned int)( one_y   / (resolution / 2.0f) );
		buffer[ i ] = 1;
		i = (unsigned int)( two_y   / (resolution / 2.0f) );
		buffer[ i ] = 1;
		i = (unsigned int)( three_y / (resolution / 2.0f) );
		buffer[ i ] = 1;

		if( buffer[ 0 ] == 1 )
			GPIO_SetBits( GPIOD, GPIO_Pin_12 );	 // RED light ON
		else
			GPIO_ResetBits( GPIOD, GPIO_Pin_12 );// RED light OFF

		if( buffer[ 1 ] == 1 )
			GPIO_SetBits( GPIOD, GPIO_Pin_13 );	 // AMBER 1 light ON
		else
			GPIO_ResetBits( GPIOD, GPIO_Pin_13 );// AMBER 1 light OFF

		if( buffer[ 2 ] == 1 )
			GPIO_SetBits( GPIOD, GPIO_Pin_14 );	 // AMBER 2 light ON
		else
			GPIO_ResetBits( GPIOD, GPIO_Pin_14 );// AMBER 2 light OFF

		if( buffer[ 3 ] == 1 )
			GPIO_SetBits( GPIOD, GPIO_Pin_15 );	 // GREEN light ON
		else
			GPIO_ResetBits( GPIOD, GPIO_Pin_15 );// GREEN light OFF

		if( allow_abort == 1 || allow_abort == 3 )
		{
			int inputs = 0;
			ReadInputs( &inputs, 0 );

			if( inputs != 0 )
			{
				WaitForButtonUp();

				abort_flag = 1;
				if( allow_abort == 1 ) break;
			}
		}
	}

	GPIO_ResetBits( GPIOD, GPIO_Pin_12 ); // RED light OFF
	GPIO_ResetBits( GPIOD, GPIO_Pin_13 ); // AMBER 1 light OFF
	GPIO_ResetBits( GPIOD, GPIO_Pin_14 ); // AMBER 2 light OFF
	GPIO_ResetBits( GPIOD, GPIO_Pin_15 ); // GREEN light OFF

	return abort_flag;
}

void ReadLightExitControlInput( int light_context )
{
	int encoder_delta = 0;
	int inputs = 0;

	while( inputs != BUTTON_E )
	{
		// read controls
		do
		{	encoder_delta = ReadInputs( &inputs, 0 );

		} while (encoder_delta == 0 && inputs == 0 );

		if( encoder_delta == 0 ) break;

		switch( light_context )
		{
			case TEST_LIGHTS_RED:
			{
				int new_digit = (Light_Code & 0xF000) + 0x1000 * encoder_delta;
				new_digit = (new_digit < 0 ) ? 0 : new_digit;
				new_digit = (new_digit > 0xF000 ) ? 0xF000 : new_digit;
				Light_Code = (Light_Code & 0x0FFF) | (short)new_digit;
				break;
			}
			case TEST_LIGHTS_AMBER1:
			{
				int new_digit = (Light_Code & 0x0F00) + 0x0100 * encoder_delta;
				new_digit = (new_digit < 0 ) ? 0 : new_digit;
				new_digit = (new_digit > 0x0F00 ) ? 0x0F00 : new_digit;
				Light_Code = (Light_Code & 0xF0FF) | (short)new_digit;
				break;
			}
			case TEST_LIGHTS_AMBER2:
			{
				int new_digit = (Light_Code & 0x00F0) + 0x0010 * encoder_delta;
				new_digit = (new_digit < 0 ) ? 0 : new_digit;
				new_digit = (new_digit > 0x00F0 ) ? 0x00F0 : new_digit;
				Light_Code = (Light_Code & 0xFF0F) | (short)new_digit;
				break;
			}
			case TEST_LIGHTS_GREEN:
			{
				int new_digit = (Light_Code & 0x000F) + 0x0001 * encoder_delta;
				new_digit = (new_digit < 0 ) ? 0 : new_digit;
				new_digit = (new_digit > 0x000F ) ? 0x000F : new_digit;
				Light_Code = (Light_Code & 0xFFF0) | (short)new_digit;
				break;
			}
		}

		if( Light_Code == 0x7A4F )
		{
			GPIO_ResetBits( GPIOD, GPIO_Pin_12 ); // RED light OFF
			GPIO_ResetBits( GPIOD, GPIO_Pin_13 ); // AMBER 1 light OFF
			GPIO_ResetBits( GPIOD, GPIO_Pin_14 ); // AMBER 2 light OFF
			GPIO_ResetBits( GPIOD, GPIO_Pin_15 ); // GREEN light OFF

			WriteLCD_LineCentered( Device_Information, 0 );

			int i = 0, j = 0;

			for( ; j < 5; j++ )
			{
				for( i = 0; i < strlen( Ownership_Information ) - 20; i++ )
				{
					WriteLCD_Line( Ownership_Information + i, 1 );
					UpdateLCD();

					Delay( 140 );
				}
			}

			WriteLCD_LineCentered( Design_By, 0 );
			WriteLCD_LineCentered( Designer_Information, 1 );
			UpdateLCD();

			int abort_cycle = 0;

			do
			{
				abort_cycle += LightTestCycle( 3 );

			} while ( abort_cycle < 5 );

			WriteLCD_LineCentered( This_Device_Is_Not, 0 );
			WriteLCD_LineCentered( Authorized_For_Use, 1 );
			UpdateLCD();

			SaveEverythingToFlashMemory( 2 );

			while( 1 );
		}
	}
}

void ValidateUUIDMask( void )
{
	if( UUID_Mask != (uint32_t)(ADDR_UUID[0] ^ ADDR_UUID[1] ^ ADDR_UUID[2]) )
	{
		if( UUID_Check == 0 )
		{
			if( UUID_Mask == 0x12345678 )
			{
				WriteLCD_LineCentered( This_Device_Is_Not, 0 );
				WriteLCD_LineCentered( Authorized_For_Use, 1 );
			}
			else
			{
				WriteLCD_LineCentered( Unauthorized_Device, 0 );
				WriteLCD_LineCentered( Illegal_Counterfeit, 1 );
			}
			UpdateLCD();

			int encoder_delta = 0;
			int inputs = 0;
			int number = 0;

			while( 1 )
			{
				// read controls
				do
				{	encoder_delta = ReadInputs( &inputs, 1 );

				} while ( encoder_delta == 0 && inputs == 0 );

				number = ( encoder_delta > 0 ) ? number + 1 : 0;

				if( number >= 15 )
				{
					while( 1 )
					{
						do
						{	encoder_delta = ReadInputs( &inputs, 1 );

						} while ( encoder_delta == 0 && inputs == 0 );

						if( inputs == BUTTON_E )
						{
							while( 1 )
							{
								do
								{	encoder_delta = ReadInputs( &inputs, 1 );

								} while ( encoder_delta == 0 && inputs == 0 );

								number = ( encoder_delta < 0 ) ? number - 1 : 0;

								if( number <= 0 )
								{
									WriteLCD_LineCentered( Device_Information, 0 );

									int i = 0;

									while( 1 )
									{
										for( i = 0; i < strlen( Ownership_Information ) - 20; i++ )
										{
											WriteLCD_Line( Ownership_Information + i, 1 );
											UpdateLCD();

											Delay( 140 );
										}
									}
								}
							}
						}
					}
				}
			}
		}

		UUID_Check--;
		SaveEverythingToFlashMemory( 0 );
	}
}

unsigned int CheckSensor( const unsigned int sensor )
{
	switch( sensor )
	{
		case AUX_1_SENSOR_A: return Aux_1_Sensor_A_Tick;
		case AUX_1_SENSOR_B: return Aux_1_Sensor_B_Tick;
		case AUX_2_SENSOR_A: return Aux_2_Sensor_A_Tick;
		case AUX_2_SENSOR_B: return Aux_2_Sensor_B_Tick;
	}

	return 0;
}

void GetTimeFromTicks( const unsigned int ticks, unsigned int *minutes, unsigned int *seconds, unsigned int *tens_place, unsigned int *hund_place, unsigned int *thou_place )
{
	*minutes     = (ticks / 600000);
	*seconds     = (ticks / 10000 ) % 60;
	*tens_place  = (ticks / 1000  ) % 10;
	*hund_place  = (ticks / 100	  ) % 10;
	*thou_place  = (ticks / 10	  ) % 10;
}

void GetTimeString( const unsigned int sensor_ticks, char *time_string )
{
	unsigned int minutes = 0, seconds = 0, tens_place = 0, hund_place = 0, thou_place = 0;

	if( sensor_ticks == 0 )
		GetTimeFromTicks( Timer6_Tick, &minutes, &seconds, &tens_place, &hund_place, &thou_place );
	else
		GetTimeFromTicks( sensor_ticks, &minutes, &seconds, &tens_place, &hund_place, &thou_place );

	char temp_string[ 2 * DISPLAY_WIDTH ];

	if( minutes > 0 )
		sprintf( temp_string, "%d:%d.%d%d%d", minutes, seconds, tens_place, hund_place, thou_place );
	else
		sprintf( temp_string, "%d.%d%d%d", seconds, tens_place, hund_place, thou_place );

	const int length = strlen(temp_string) > DISPLAY_WIDTH ? DISPLAY_WIDTH : strlen( temp_string );
	int i;
	for( i = 0; i < length; i++ ) time_string[ i ] = temp_string[ i ];
}

void GetTimesString( const unsigned int sensor_1, const unsigned int sensor_2, char *sensor_1_time_string, char *sensor_2_time_string )
{
	unsigned int minutes = 0, seconds = 0, tens_place = 0, hund_place = 0, thou_place = 0;

	int i, j;

	if( sensor_1 == 0 )
		{ sprintf( sensor_1_time_string, "AUX 1:" ); 		sensor_1_time_string[ 6 ] = 0x20; }
	else
		{ sprintf( sensor_1_time_string, "AUX 1 TIME:" );	sensor_1_time_string[ 11 ] = 0x20; }

	if( sensor_2 == 0 )
		{ sprintf( sensor_2_time_string, "AUX 2:" ); 		sensor_2_time_string[ 6 ] = 0x20; }
	else
		{ sprintf( sensor_2_time_string, "AUX 2 TIME:" ); 	sensor_2_time_string[ 11 ] = 0x20; }


	// construct sensor 1 time string
	if( sensor_1 == 0 )
		GetTimeFromTicks( Timer6_Tick, &minutes, &seconds, &tens_place, &hund_place, &thou_place );
	else
		GetTimeFromTicks( sensor_1, &minutes, &seconds, &tens_place, &hund_place, &thou_place );

	char temp_string[ 2 * DISPLAY_WIDTH ];

	if( minutes > 0 )
		sprintf( temp_string, "%d:%d.%d%d%d", minutes, seconds, tens_place, hund_place, thou_place );
	else
		sprintf( temp_string, "%d.%d%d%d", seconds, tens_place, hund_place, thou_place );

	int adjust = strlen( temp_string ) > DISPLAY_WIDTH ? 0 : strlen( temp_string );
	for( i = 0, j = DISPLAY_WIDTH - adjust; j < DISPLAY_WIDTH; j++, i++ ) sensor_1_time_string[ j ] = temp_string[ i ];


	// construct sensor 2 time string
	if( sensor_2 == 0 )
		GetTimeFromTicks( Timer6_Tick, &minutes, &seconds, &tens_place, &hund_place, &thou_place );
	else
		GetTimeFromTicks( sensor_2, &minutes, &seconds, &tens_place, &hund_place, &thou_place );

	if( minutes > 0 )
		sprintf( temp_string, "%d:%d.%d%d%d", minutes, seconds, tens_place, hund_place, thou_place );
	else
		sprintf( temp_string, "%d.%d%d%d", seconds, tens_place, hund_place, thou_place );

	adjust = strlen( temp_string ) > DISPLAY_WIDTH ? 0 : strlen( temp_string );
	for( i = 0, j = DISPLAY_WIDTH - adjust; j < DISPLAY_WIDTH; j++, i++ ) sensor_2_time_string[ j ] = temp_string[ i ];
}

void CalculateSpeed( const uint32_t speed_tick_delta, const unsigned int sensor_A, const unsigned int sensor_B, const unsigned int sensor_spacing,
					 unsigned int * elapsed_time, float * speed , unsigned int * speed_integer, unsigned int * speed_fractional )
{
	*elapsed_time = (sensor_A < sensor_B) ? sensor_B : sensor_A;

	// speed = (d * (60 * 60) / (5280 * 12)) / (t / 84Mhz)
	// speed = 5d / 88t
	//*speed = ((float)(326772) * (3600.0f / 63360.0f)) / (10000.0f * (float) speed_tick_delta / 84000000.0f );
	*speed = ((float)(sensor_spacing) * (3600.0f / 63360.0f)) / (10000.0f * (float) speed_tick_delta / 84000000.0f );

	*speed_integer 	 = (int)*speed;
	*speed_fractional= (int)(*speed * 1000.0f) % 1000;
}

void ProcessElapsedTimer( unsigned int aux_config, unsigned int sensor_A, unsigned int sensor_B )
{
	char elapsed_time_string[ DISPLAY_WIDTH ] = ZEROS;

	unsigned int elapsed_time = 0;

	if( sensor_A > 0 )
	{
		elapsed_time = sensor_A;
		GetTimeString( elapsed_time, elapsed_time_string );
	}
	else if( sensor_B > 0 )
	{
		elapsed_time = sensor_B;
		GetTimeString( elapsed_time, elapsed_time_string );
	}

	// update timing histories
	AddTimeToTimerHistory( aux_config, elapsed_time, elapsed_time_string );
}

void ProcessTimeAndSpeed( const unsigned int aux_config, const unsigned int elapsed_time, const int speed, const unsigned int speed_integer, const unsigned int speed_fractional )
{
	CopyTimerHistoryDown();

	Timer_History_Index++;
	Timer_History_Index = ( Timer_History_Index > MAX_TIMING_HISTORIES ) ? MAX_TIMING_HISTORIES : Timer_History_Index;

	Timer_History_Number++;
	Timer_History_Number = ( Timer_History_Number > MAX_HISTORY_NUMBER ) ? 1 : Timer_History_Number;

	// save this timer result
	Timer_History[ 0 ].number			= Timer_History_Number;
	Timer_History[ 0 ].elapsed_time		= elapsed_time;
	Timer_History[ 0 ].speed_integer	= speed_integer;
	Timer_History[ 0 ].speed_fractional	= speed_fractional;

	Timer_History[ 0 ].is_a_speed		= ((speed_integer != 0) || (speed_fractional != 0));

	Timer_History[ 0 ].aux_port			= aux_config;

	// pad temp strings out to handle memory overruns before copy occurs
	char time_string [ 2 * DISPLAY_WIDTH ];
	char speed_string[ 2 * DISPLAY_WIDTH ];

	sprintf( speed_string, "%d.%03d mph", speed_integer, speed_fractional );

	unsigned int minutes = 0, seconds = 0, tens_place = 0, hund_place = 0, thou_place = 0;

	GetTimeFromTicks( elapsed_time, &minutes, &seconds, &tens_place, &hund_place, &thou_place );

	if( minutes > 0 )
		sprintf( time_string, "%d:%d.%d%d%d", minutes, seconds, tens_place, hund_place, thou_place );
	else
		sprintf( time_string, "%d.%d%d%d", seconds, tens_place, hund_place, thou_place );

	strcpy( Timer_History[ 0 ].time_string, time_string );
	strcpy( Timer_History[ 0 ].speed_string, speed_string );


	// construct time and speed strings
	char time_speed_string[ 2 * DISPLAY_WIDTH ];

	if( minutes < 1 )
	{
		sprintf( time_speed_string, "%d.%d%d%d @ %s", seconds, tens_place, hund_place, thou_place, speed_string );
	}
	else if( (minutes >  0 && minutes <  9 && seconds < 10 && speed > 9 ) ||
			 (minutes < 10 && seconds < 10 && speed > 9) ||
			 (minutes < 10 && seconds >  9 && speed < 10)||
			 (minutes >  9 && seconds < 10 && speed < 10) )
	{
		sprintf( time_speed_string, "%d:%d.%d%d%d @ %s", minutes, seconds, tens_place, hund_place, thou_place, speed_string );
	}
	else if( (minutes < 10 && seconds > 9 && speed > 9) ||
			 (minutes >  9 && seconds > 9 && speed < 9) ||
			 (minutes >  0 && minutes < 9 && seconds > 9 && speed > 10) )
	{
		sprintf( time_speed_string, "%d:%d.%d%d%d @%s", minutes, seconds, tens_place, hund_place, thou_place, speed_string );
	}
	else {
		sprintf( time_speed_string, "%d:%d.%d%d%d@%s", minutes, seconds, tens_place, hund_place, thou_place, speed_string );
	}

	strcpy( Timer_History[ 0 ].time_speed_string, time_speed_string );

	// update menu text
	if( Timer_History[ 0 ].elapsed_time == 0 )
		SetMenuText( Menu_Array[ TIMER_HISTORY ].item[ 0 ], Timer_History[ 0 ].speed_string );
	else
		SetMenuText( Menu_Array[ TIMER_HISTORY ].item[ 0 ], Timer_History[ 0 ].time_speed_string );
}

int DoTimeAndDisabled( const unsigned int aux_config, unsigned int sensor_A, unsigned int sensor_B, const char * timing_string, const char * elapsed_string )
{
	if( sensor_A == 0 && sensor_B == 0 )
	{
		WriteLCD_LineCentered( timing_string, 0 );
		PrintElapsedTime( Timer6_Tick, 1, 1 );
		UpdateLCD();

		return 1;
	}

	ProcessElapsedTimer( aux_config, sensor_A, sensor_B );

	WriteLCD_LineCentered( elapsed_string, 0 );
	WriteLCD_LineCentered( Timer_History[ 0 ].time_string, 1 );
	UpdateLCD();

	// say elapsed time
#ifdef CADENCE
	if( Menu_Array[ AUTO_ANNOUNCE_TIMES ].context == AUTO_ANNOUNCE_TIMES_ENABLED )
	{
		PlaySilence( 100, 0 );
		PlayTimeOrPercent( Timer_History[ 0 ].elapsed_time / 10, PLAY_TIME );
		PlaySilence( 1, 0 );
		InitAudio();
	}
#endif

	Delay( 2000 );

	return 0;
}

int DoTimeAndSpeed( const unsigned int aux1_option,
					const unsigned int sensor_1A, const unsigned int sensor_1B, const unsigned int sensor_2A, const unsigned int sensor_2B, unsigned int * timeout )
{
	char line_0[ DISPLAY_WIDTH ] = SPACES;
	char line_1[ DISPLAY_WIDTH ] = SPACES;

	unsigned int time_config = 0, speed_config = 0;

	unsigned int sensor_1 = 0, sensor_2 = 0;

	if( sensor_1 == 0 && (sensor_1A | sensor_1B) != 0 ) sensor_1 = (sensor_1A | sensor_1B);
	if( sensor_2 == 0 && (sensor_2A | sensor_2B) != 0 ) sensor_2 = (sensor_2A | sensor_2B);

	GetTimesString( sensor_1, sensor_2, line_0, line_1 );

	WriteLCD_Line( line_0, 0 );
	WriteLCD_Line( line_1, 1 );
	UpdateLCD();

	unsigned int time_sensor, time_sensor_A, time_sensor_B, speed_sensor, speed_sensor_A, speed_sensor_B, sensor_spacing;

	if( aux1_option == AUX_TIME_SPEED )
	{
		time_config	 	= AUX_1_CONFIG;
		speed_config	= AUX_2_CONFIG;

		speed_sensor	= sensor_1;				time_sensor		= sensor_2;
		speed_sensor_A	= sensor_1A;			time_sensor_A	= sensor_2A;
		speed_sensor_B	= sensor_1B;			time_sensor_B	= sensor_2B;
		sensor_spacing	= Aux_1_Sensor_Spacing;
	}
	else
	{
		time_config	 	= AUX_2_CONFIG;
		speed_config	= AUX_1_CONFIG;

		speed_sensor	= sensor_2;				time_sensor		= sensor_1;
		speed_sensor_A	= sensor_2A;			time_sensor_A	= sensor_1A;
		speed_sensor_B	= sensor_2B;			time_sensor_B	= sensor_1B;
		sensor_spacing	= Aux_2_Sensor_Spacing;
	}

	// initialize timeout value if first time through
	if( (*timeout == 0) && ((speed_sensor_A == 0 && speed_sensor_B != 0) || (speed_sensor_A != 0 && speed_sensor_B == 0)) )
	{
		*timeout = Timer6_Tick + SENSOR_TIMEOUT_PERIOD;
	}

	// check for timeout
	if( Timer6_Tick > *timeout && ((speed_sensor_A == 0 && speed_sensor_B != 0) || (speed_sensor_A != 0 && speed_sensor_B == 0)) )
	{
		WriteLCD_LineCentered( "TIMING ABORTED", 0 );

		if( aux1_option == AUX_TIME_SPEED )
			WriteLCD_LineCentered( "AUX 1 sensor faulty", 1 );
		else
			WriteLCD_LineCentered( "AUX 2 sensor faulty", 1 );
		UpdateLCD();

		Delay( 3000 );

		return 0;
	}

	// keep waiting for sensors to trip
	if( speed_sensor_A == 0 || speed_sensor_B == 0 || time_sensor == 0 ) return 1;

	// all sensors tripped so process times
	if( Cancel_Timing == 0 )
	{
		unsigned int elapsed_time, speed_integer, speed_fractional;
		float speed;

		if( speed_config == AUX_1_CONFIG )
			CalculateSpeed( Aux_1_Speed_Tick_Delta, speed_sensor_A, speed_sensor_B, sensor_spacing, & elapsed_time, & speed, & speed_integer, & speed_fractional );
		else
			CalculateSpeed( Aux_2_Speed_Tick_Delta, speed_sensor_A, speed_sensor_B, sensor_spacing, & elapsed_time, & speed, & speed_integer, & speed_fractional );

		if( ( time_sensor > speed_sensor_A ) || time_sensor > speed_sensor_B )
		{
			ProcessTimeAndSpeed( speed_config, elapsed_time, speed, speed_integer, speed_fractional );
			ProcessElapsedTimer( time_config, time_sensor_A, time_sensor_B );

			WriteLCD_LineCentered( Timer_History[ 0 ].time_string, 1 );
			WriteLCD_LineCentered( Timer_History[ 1 ].time_speed_string, 0 );
		}
		else
		{
			ProcessTimeAndSpeed( speed_config, elapsed_time, speed, speed_integer, speed_fractional );
			ProcessElapsedTimer( time_config, time_sensor_A, time_sensor_B );

			WriteLCD_LineCentered( Timer_History[ 0 ].time_string, 0 );
			WriteLCD_LineCentered( Timer_History[ 1 ].time_speed_string, 1 );
		}

		UpdateLCD();
	}

#ifdef NUMBERS
	if( Menu_Array[ AUTO_ANNOUNCE_TIMES ].context == AUTO_ANNOUNCE_TIMES_ENABLED )
	{
		const int time_start = 8400;
		const int time_size  = 7500;

		if( ( time_sensor > speed_sensor_A ) || time_sensor > speed_sensor_B )
		{
			PlaySilence( 100, 1 );
			PlaySample24khz( LastTimeWasData + time_start, 0, time_size, 1 );
			PlayDigit( 1 );
			PlayTimeOrPercent( Timer_History[ 0 ].elapsed_time / 10, PLAY_TIME );
			PlaySilence( 1, 0 );

			if( Timer_History[ 0 ].is_a_speed != 0 )
			{
				PlaySample24khz( AtData, 0, AT_SIZE, 1 );
				PlaySpeed( Timer_History[ 0 ].speed_integer, Timer_History[ 0 ].speed_fractional );
			}

			PlaySample24khz( LastTimeWasData + time_start, 0, time_size, 1 );
			PlayDigit( 2 );
			PlayTimeOrPercent( Timer_History[ 1 ].elapsed_time / 10, PLAY_TIME );
			PlaySilence( 1, 0 );

			if( Timer_History[ 1 ].is_a_speed != 0 )
			{
				PlaySample24khz( AtData, 0, AT_SIZE, 1 );
				PlaySpeed( Timer_History[ 1 ].speed_integer, Timer_History[ 1 ].speed_fractional );
			}
		}
		else
		{
			PlaySilence( 100, 1 );
			PlaySample24khz( LastTimeWasData + time_start, 0, time_size, 1 );
			PlayDigit( 1 );
			PlayTimeOrPercent( Timer_History[ 1 ].elapsed_time / 10, PLAY_TIME );
			PlaySilence( 1, 0 );

			if( Timer_History[ 1 ].is_a_speed != 0 )
			{
				PlaySample24khz( AtData, 0, AT_SIZE, 1 );
				PlaySpeed( Timer_History[ 1 ].speed_integer, Timer_History[ 1 ].speed_fractional );
			}

			PlaySample24khz( LastTimeWasData + time_start, 0, time_size, 1 );
			PlayDigit( 2 );
			PlayTimeOrPercent( Timer_History[ 0 ].elapsed_time / 10, PLAY_TIME );
			PlaySilence( 1, 0 );

			if( Timer_History[ 0 ].is_a_speed != 0 )
			{
				PlaySample24khz( AtData, 0, AT_SIZE, 1 );
				PlaySpeed( Timer_History[ 0 ].speed_integer, Timer_History[ 0 ].speed_fractional );
			}
		}

		InitAudio();
	}
#endif

	Delay( 2000 );

	return 0;
}

int DoSpeedAndDisabled( const unsigned int aux_config, const unsigned int sensor_A, const unsigned int sensor_B, const unsigned int sensor_spacing,
						const char * timing_string, const char * elapsed_string, unsigned int * timeout )
{
	// after one sensor has been tripped initialize timeout
	if( (*timeout == 0) && ((sensor_A == 0 && sensor_B != 0) || (sensor_A != 0 && sensor_B == 0)) )
	{
		*timeout = Timer6_Tick + SENSOR_TIMEOUT_PERIOD;
	}

	if( Timer6_Tick > *timeout && ((sensor_A == 0 && sensor_B != 0) || (sensor_A != 0 && sensor_B == 0)) )
	{
		if( aux_config == AUX_1_CONFIG )
			WriteLCD_LineCentered( "AUX 1 sensor faulty", 1 );
		else
			WriteLCD_LineCentered( "AUX 2 sensor faulty", 1 );
		UpdateLCD();

		return 0;
	}

	if( sensor_A == 0 || sensor_B == 0 )
	{
		WriteLCD_LineCentered( timing_string, 0 );
		PrintElapsedTime( Timer6_Tick, 1, 1 );
		UpdateLCD();

		return 1;
	}

	if( Cancel_Timing == 0 )
	{
		unsigned int elapsed_time, speed_integer, speed_fractional;
		float speed;

		if( aux_config == AUX_1_CONFIG )
			CalculateSpeed( Aux_1_Speed_Tick_Delta, sensor_A, sensor_B, sensor_spacing, & elapsed_time, & speed, & speed_integer, & speed_fractional );
		else
			CalculateSpeed( Aux_2_Speed_Tick_Delta, sensor_A, sensor_B, sensor_spacing, & elapsed_time, & speed, & speed_integer, & speed_fractional );

		ProcessTimeAndSpeed( aux_config, elapsed_time, speed, speed_integer, speed_fractional );

		WriteLCD_LineCentered( elapsed_string, 0 );
		WriteLCD_LineCentered( Timer_History[ 0 ].time_speed_string, 1 );
		UpdateLCD();
	}

#ifdef NUMBERS
	if( Menu_Array[ AUTO_ANNOUNCE_TIMES ].context == AUTO_ANNOUNCE_TIMES_ENABLED )
	{
		PlaySilence( 100, 0 );
		PlayTimeOrPercent( Timer_History[ 0 ].elapsed_time / 10, PLAY_TIME );
		PlaySilence( 1, 0 );

		if( Timer_History[ 0 ].is_a_speed != 0 )
		{
			PlaySample24khz( AtData, 0, AT_SIZE, 1 );
			PlaySpeed( Timer_History[ 0 ].speed_integer, Timer_History[ 0 ].speed_fractional );
		}
		InitAudio();
	}
#endif

	Delay( 3000 );

	return 0;
}

void DoSprintTimer( const unsigned int aux_config )
{
	unsigned int sensor_1 = 0;
	unsigned int sensor_2 = 0;

	unsigned int sensor_1A = 0, sensor_1B = 0, sensor_2A = 0, sensor_2B = 0;

	char time_string[ DISPLAY_WIDTH ];

	memset( time_string, 0, DISPLAY_WIDTH );

	// reset all the timers
	Timer6_Tick			= 0;
	Aux_1_Sensor_A_Tick = 0;
	Aux_1_Sensor_B_Tick = 0;
	Aux_2_Sensor_A_Tick = 0;
	Aux_2_Sensor_B_Tick = 0;

	int inputs = 0;

	float speed = 0;

	unsigned int timeout = 0; // if speed sensor is not hit within 1.5 seconds then just save time

	DisableSpeedMeasurement();
	ResetSpeedMeasurement();
	EnableSpeedMeasurement();

	do
	{
		ReadInputs( & inputs, 1 );

		// reassign based on which is actually AUX 1, AUX 2
		if( aux_config == AUX_1_CONFIG )
		{
			sensor_1A = Aux_1_Sensor_A_Tick;	sensor_1B = Aux_1_Sensor_B_Tick;
			sensor_2A = Aux_2_Sensor_A_Tick;	sensor_2B = Aux_2_Sensor_B_Tick;
		}
		else
		{
			sensor_1A = Aux_2_Sensor_A_Tick;	sensor_1B = Aux_2_Sensor_B_Tick;
			sensor_2A = Aux_1_Sensor_A_Tick;	sensor_2B = Aux_1_Sensor_B_Tick;
		}

		if( sensor_1A == 0 && sensor_1B == 0 && sensor_2A == 0 && sensor_2B == 0 )
		{
			WriteLCD_LineCentered( "WAITING FOR SENSORS", 0 );

			if( speed == 0 ) WriteLCD_LineCentered( SPACES, 1 );

			UpdateLCD();

			// reset
			Timer6_Tick	= 0;
			timeout		= 0;
			sensor_1	= 0;
			sensor_2	= 0;

#ifdef NUMBERS
			// if button D was pressed on the remote announce last time, speed, or time + speed
			if( GPIOD->IDR & 0x0100 )
			{
				PlaySilence( 100, 1 );

				if( Timer_History[ 0 ].elapsed_time > 0 && Timer_History[ 0 ].is_a_speed == 1 )
				{
					PlayTimeOrPercent( Timer_History[ 0 ].elapsed_time / 10, PLAY_TIME );

					if( Timer_History[ 0 ].is_a_speed == 1 )
					{
						PlaySample24khz( AtData, 0, AT_SIZE, 1 );
						PlaySpeed( Timer_History[ 0 ].speed_integer, Timer_History[ 0 ].speed_fractional );
						InitAudio();
					}
				}
				else if( Timer_History[ 0 ].is_a_speed == 1 )
				{
					PlaySpeed( Timer_History[ 0 ].speed_integer, Timer_History[ 0 ].speed_fractional );
				}
				else
				{
					PlayTimeOrPercent( Timer_History[ 0 ].elapsed_time / 10, PLAY_TIME );
				}
				InitAudio();
			}
#endif
			continue;
		}

		if( sensor_1A != 0 || sensor_1B != 0 || sensor_2A != 0 || sensor_2B != 0 )
		{
			if( aux_config == AUX_1_CONFIG )
				WriteLCD_LineCentered( "TIMING AUX 1", 0 );
			else
				WriteLCD_LineCentered( "TIMING AUX 2", 0 );

			GetTimeString( Timer6_Tick, time_string );

			WriteLCD_LineCentered( time_string, 1 );
			UpdateLCD();

			// sensor 1 is used to calculate speed
			if( sensor_1 == 0 && (sensor_1A || sensor_1B) != 0 )	sensor_1 = (sensor_1A | sensor_1B);

			// sensor 2 is used to start timing
			if( sensor_2 == 0 && (sensor_2A || sensor_2B) != 0 )	sensor_2 = (sensor_2A | sensor_2B);

			// set timeout value once an aux 1 sensor is hit
			if( (timeout == 0 ) && ((sensor_1A == 0 && sensor_1B != 0) || (sensor_1A != 0 && sensor_1B == 0)) )
			{
				timeout = Timer6_Tick + SENSOR_TIMEOUT_PERIOD;
			}

			if( Timer6_Tick > timeout && ((sensor_1A == 0 && sensor_1B != 0) || (sensor_1A != 0 && sensor_1B == 0)) )
			{
				if( sensor_2 != 0 )
				{
					ProcessElapsedTimer( aux_config, sensor_1, sensor_2 );

					WriteLCD_LineCentered( "ELAPSED TIME", 0 );
					WriteLCD_LineCentered( Timer_History[ 0 ].time_string, 1 );
					UpdateLCD();
#ifdef NUMBERS
					if( Menu_Array[ AUTO_ANNOUNCE_TIMES ].context == AUTO_ANNOUNCE_TIMES_ENABLED )
					{
						PlaySilence( 100, 1 );
						PlayTimeOrPercent( Timer_History[ 0 ].elapsed_time / 10, PLAY_TIME );
						PlaySilence( 100, 1 );
						InitAudio();
					}
#endif
				}
				else
				{
					if( aux_config == AUX_1_CONFIG )
						WriteLCD_LineCentered( "AUX 1 sensor faulty", 1 );
					else
						WriteLCD_LineCentered( "AUX 2 sensor faulty", 1 );
					UpdateLCD();

					DisableSpeedMeasurement();
					ResetSpeedMeasurement();
					EnableSpeedMeasurement();
				}

				Delay( 1250 );

				// reset
				Aux_1_Sensor_A_Tick = 0;
				Aux_1_Sensor_B_Tick = 0;
				Aux_2_Sensor_A_Tick = 0;
				Aux_2_Sensor_B_Tick = 0;

				memset( time_string, 0, DISPLAY_WIDTH );

				continue;
			}

			// calculate speed or speed and time if speed sensor has both ports hit
			if( sensor_1A != 0 && sensor_1B != 0 )
			{
				const unsigned int sensor_spacing = (aux_config == AUX_1_CONFIG ) ? Aux_1_Sensor_Spacing : Aux_2_Sensor_Spacing;

				unsigned int sensor_delta = 0, speed_integer = 0, speed_fractional = 0;

				// determine speed
				if( aux_config == AUX_1_CONFIG )
					CalculateSpeed( Aux_1_Speed_Tick_Delta, sensor_1A, sensor_1B, sensor_spacing, & sensor_delta, & speed, & speed_integer, & speed_fractional );
				else
					CalculateSpeed( Aux_2_Speed_Tick_Delta, sensor_1A, sensor_1B, sensor_spacing, & sensor_delta, & speed, & speed_integer, & speed_fractional );

				// determine elapsed time if it was triggered
				unsigned int elapsed_time = 0;

				if( sensor_2A != 0 || sensor_2B != 0 )
				{
					const unsigned int end_time = ( sensor_1A < sensor_1B ) ? sensor_1A : sensor_1B;
					elapsed_time = end_time - sensor_2;
				}

				ProcessTimeAndSpeed( aux_config, elapsed_time, speed, speed_integer, speed_fractional );

				if( elapsed_time > 0 )
				{
					WriteLCD_LineCentered( "TIME AND SPEED", 0 );
					WriteLCD_LineCentered( Timer_History[ 0 ].time_speed_string, 1 );
					UpdateLCD();
#ifdef NUMBERS
					if( Menu_Array[ AUTO_ANNOUNCE_TIMES ].context == AUTO_ANNOUNCE_TIMES_ENABLED )
					{
						PlaySilence( 100, 1 );
						PlayTimeOrPercent( Timer_History[ 0 ].elapsed_time / 10, PLAY_TIME );

						PlaySample24khz( AtData, 0, AT_SIZE, 1 );
						PlaySpeed( Timer_History[ 0 ].speed_integer, Timer_History[ 0 ].speed_fractional );
						InitAudio();
					}
#endif
				}
				else
				{
					WriteLCD_LineCentered( "SPEED", 0 );
					WriteLCD_LineCentered( Timer_History[ 0 ].speed_string, 1 );
					UpdateLCD();
#ifdef NUMBERS
					if( Menu_Array[ AUTO_ANNOUNCE_TIMES ].context == AUTO_ANNOUNCE_TIMES_ENABLED )
					{
						PlaySpeed( Timer_History[ 0 ].speed_integer, Timer_History[ 0 ].speed_fractional );
						InitAudio();
					}
#endif
					DisableSpeedMeasurement();
					ResetSpeedMeasurement();
					EnableSpeedMeasurement();
				}

#ifndef NUMBERS
				Delay( 1000 );
#endif
				// reset
				Aux_1_Sensor_A_Tick = 0;
				Aux_1_Sensor_B_Tick = 0;
				Aux_2_Sensor_A_Tick = 0;
				Aux_2_Sensor_B_Tick = 0;

				memset( time_string, 0, DISPLAY_WIDTH );

				continue;
			}
		}

	} while( inputs != BUTTON_E );

	DisableSpeedMeasurement();
}

void CopyTimerHistoryDown( void )
{
	int j = 0, k;
	// only store up to MAX_TIMING_HISTORIES

	for( j = MAX_TIMING_HISTORIES - 1; j >= 0 ; j-- )
	{ // just scroll entire array down before each add and add new time at position 0 in the Timer_History array
		Timer_History[ j ].number			= Timer_History[ j - 1 ].number;
		Timer_History[ j ].elapsed_time		= Timer_History[ j - 1 ].elapsed_time;
		Timer_History[ j ].speed_integer	= Timer_History[ j - 1 ].speed_integer;
		Timer_History[ j ].speed_fractional	= Timer_History[ j - 1 ].speed_fractional;

		Timer_History[ j ].is_a_speed		= Timer_History[ j - 1 ].is_a_speed;
		Timer_History[ j ].aux_port			= Timer_History[ j - 1 ].aux_port;

		for( k = 0; k < DISPLAY_WIDTH; k++ )
		{
			Timer_History[ j ].time_string[ k ]		  = Timer_History[ j - 1 ].time_string[ k ];
			Timer_History[ j ].speed_string[ k ]	  = Timer_History[ j - 1 ].speed_string[ k ];
			Timer_History[ j ].time_speed_string[ k ] = Timer_History[ j - 1 ].time_speed_string[ k ];
		}
	}
}

void AddTimeToTimerHistory( const unsigned int aux_config, const unsigned int elapsed_time, const char *time_string )
{
	CopyTimerHistoryDown();

	// used by TIMER_HISTORY to know how many entries to display
	Timer_History_Index++;
	Timer_History_Index = ( Timer_History_Index > MAX_TIMING_HISTORIES ) ? MAX_TIMING_HISTORIES : Timer_History_Index;

	// reset history index at 100000 (like anyone would store that many - EVER)
	Timer_History_Number++;
	Timer_History_Number = ( Timer_History_Number > MAX_HISTORY_NUMBER ) ? 1 : Timer_History_Number;

	// save this timer result
	Timer_History[ 0 ].number			= Timer_History_Number;
	Timer_History[ 0 ].elapsed_time		= elapsed_time;
	Timer_History[ 0 ].speed_integer	= 0;
	Timer_History[ 0 ].speed_fractional	= 0;

	Timer_History[ 0 ].aux_port			= aux_config;

	// copy timer strings to timer history index 0 since entire array has scrolled down
	strcpy( Timer_History[ 0 ].time_string, time_string );

	// update menu text
	SetMenuText( Menu_Array[ TIMER_HISTORY ].item[ 0 ], Timer_History[ 0 ].time_string );
}

void InitClocks( void )
{
	RCC_ClocksTypeDef RCC_ClocksStruct;
	RCC_GetClocksFreq( &RCC_ClocksStruct );

	// by default values should equal:
	// SYSCLK_Frequency	= 0x0A037A00, 168,000,000, 168MHz
	// HCLK_Frequency	= 0x0A037A00, 168MHz
	// PCLK1_Frequency	= 0x0280DE80, 42MHz
	// PCLK2_Frequency	= 0x0501BD00, 84MHz
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
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd	= 0;
	GPIO_Init( GPIOB, &GPIO_InitStructure );

	// Aux port 1, Aux port 2
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd	= 0;
	GPIO_Init( GPIOB, &GPIO_InitStructure );

	// setup input capture pins to timers for auxiliary ports
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource4, GPIO_AF_TIM3 );
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource5, GPIO_AF_TIM3 );
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource7, GPIO_AF_TIM4 );
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource8, GPIO_AF_TIM4 );

	// remote buttons, audio in detect, battery
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_100MHz;
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

	SPI_InitStructure.SPI_Direction 		= SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode				= SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize			= SPI_DataSize_16b;
	SPI_InitStructure.SPI_CPOL 				= SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA 				= SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS 				= SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_InitStructure.SPI_FirstBit 			= SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial 	= 7;
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

void SysTick_Init( void )
{
	// CPU Speed is 168MHz - a reload value(RVR) of 83 results in an unstable 995.055KHz update frequency according to the Rigol DS1052E
	// presumably the waveform is unstable because this interrupt is sharing time with the CPU?
	ADDR_SYST_RVR[0] = 83;
	ADDR_SYST_CVR[0] = 0;
	ADDR_SYST_CSR[0] = ADDR_SYST_CSR[0] | 0b00000000000000000000000000000111;

	SystemTick = 0;
}

void StartTimers( void )
{
	// following instructions start at line 168 from stm32f4xx_tim.c
	// enable all clocks for all timers
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE );
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3, ENABLE );
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM4, ENABLE );
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM5, ENABLE );

	// set speeds for all timers
	TIM_TimeBaseInitTypeDef TimeBaseDefaults;

	// Default configuration:
	// ----------------------
	//  update freq (Hz) = TIM_CLK / (PSC+1)
	//
	//	TIM_CLK = timer clock input (84MHz)
	//	PSC = 16-bit prescalar register
	//	ARR = 16/32-bit autoreload register
	//  RCR = 16-bit repetition counter
	//
	//  TIM_CounterMode		  = TIM_CounterMode_Up;
	//  TIM_ClockDivision	  = TIM_CKD_DIV1;
	//
	//  TIM_Prescaler		  = 0x0000;		(PreSCaler PSC)
	//  TIM_Period			  = 0xFFFFFFFF;	(Auto Reload Register  ARR)
	//  TIM_RepetitionCounter = 0x0000;		(RepetitionCounteR RCR)
	//
	TIM_TimeBaseStructInit( & TimeBaseDefaults );

	TIM_TimeBaseInit( TIM2, & TimeBaseDefaults );
	TIM_TimeBaseInit( TIM3, & TimeBaseDefaults );
	TIM_TimeBaseInit( TIM4, & TimeBaseDefaults );
	TIM_TimeBaseInit( TIM5, & TimeBaseDefaults );

	// Set input capture settings for timers 3 and  4
	TIM_ICInitTypeDef TIM_ICInitStructure;

	TIM_ICInitStructure.TIM_Channel 	= TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICPolarity  = TIM_ICPolarity_Falling;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter	= 0;
	TIM_ICInit( TIM3, &TIM_ICInitStructure);

	TIM_ICInitStructure.TIM_Channel 	= TIM_Channel_2;
	TIM_ICInitStructure.TIM_ICPolarity  = TIM_ICPolarity_Falling;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter	= 0;
	TIM_ICInit( TIM3, &TIM_ICInitStructure);

	TIM_ICInitStructure.TIM_Channel 	= TIM_Channel_2;
	TIM_ICInitStructure.TIM_ICPolarity  = TIM_ICPolarity_Falling;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter	= 0;
	TIM_ICInit( TIM4, &TIM_ICInitStructure);

	TIM_ICInitStructure.TIM_Channel 	= TIM_Channel_3;
	TIM_ICInitStructure.TIM_ICPolarity  = TIM_ICPolarity_Falling;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter	= 0;
	TIM_ICInit( TIM4, &TIM_ICInitStructure);

	EnableSpeedMeasurement();
	DisableSpeedMeasurement();
	ResetSpeedMeasurement();

	// add interrupt service routines to update
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel 			 		 = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 		 = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd		  		 = ENABLE;
	NVIC_Init( &NVIC_InitStructure );

	NVIC_InitStructure.NVIC_IRQChannel 			 		 = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 		 = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd		  		 = ENABLE;
	NVIC_Init( &NVIC_InitStructure );

	TIM_ITConfig( TIM2, TIM_IT_Update, ENABLE );
	TIM_ITConfig( TIM5, TIM_IT_Update, ENABLE );

	TIM_Cmd( TIM3, ENABLE );
	TIM_Cmd( TIM4, ENABLE );


	// timer 6
    RCC->APB1ENR	|= RCC_APB1ENR_TIM6EN;  	// Enable timer 6 clock

    TIM6->PSC		= 839;                		// 84,000,000 Hz / 8,40 = 100,000 timer ticks per second // 10KHz
    TIM6->ARR		= 9;                   		// 10,000 Hz / 10 = 1000 timer ints per second
    TIM6->EGR		|= TIM_EGR_UG;              // Force update
    TIM6->SR		&= ~TIM_SR_UIF;             // Clear the update flag
    TIM6->DIER		|= TIM_DIER_UIE;            // Enable interrupt on update event


	NVIC_InitStructure.NVIC_IRQChannel 			 		 = TIM6_DAC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 		 = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd		  		 = ENABLE;
	NVIC_Init( &NVIC_InitStructure );
//    NVIC_EnableIRQ(TIM6_DAC_IRQn);              // Enable TIM6 IRQ

    TIM6->CR1 		|= TIM_CR1_CEN;             // Enable TIM6 counter

    Timer6_Tick = 0;

    // timer 7
    RCC->APB1ENR	|= RCC_APB1ENR_TIM7EN;  	// Enable timer 7 clock

    TIM7->PSC		= 839;                		// 84,000,000 Hz / 8,40 = 100,000 timer ticks per second
    TIM7->ARR		= 9;                   		// 10,000 Hz / 10 = 1000 timer ints per second
    TIM7->EGR		|= TIM_EGR_UG;              // Force update
    TIM7->SR		&= ~TIM_SR_UIF;             // Clear the update flag
    TIM7->DIER		|= TIM_DIER_UIE;            // Enable interrupt on update event

	NVIC_InitStructure.NVIC_IRQChannel 			 		 = TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 		 = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd		  		 = ENABLE;
	NVIC_Init( &NVIC_InitStructure );
//    NVIC_EnableIRQ(TIM7_IRQn);  	            // Enable TIM7 IRQ

    TIM7->CR1 		|= TIM_CR1_CEN;             // Enable TIM7 counter
}

void EnableSpeedMeasurement( void )
{
	TIM_ITConfig( TIM3, TIM_IT_CC1 | TIM_IT_CC2, ENABLE );
	TIM_ITConfig( TIM4, TIM_IT_CC2 | TIM_IT_CC3, ENABLE );

	TIM_ClearFlag( TIM3, TIM_FLAG_CC1 | TIM_FLAG_CC2 );
	TIM_ClearFlag( TIM4, TIM_FLAG_CC2 | TIM_FLAG_CC3 );

	GPIO_PinAFConfig( GPIOB, GPIO_PinSource4, GPIO_AF_TIM3 );
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource5, GPIO_AF_TIM3 );
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource7, GPIO_AF_TIM4 );
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource8, GPIO_AF_TIM4 );
}

void DisableSpeedMeasurement( void )
{
	TIM3->SR &= ~TIM_SR_UIF;
	TIM4->SR &= ~TIM_SR_UIF;

	TIM_ITConfig( TIM3, TIM_IT_CC1 | TIM_IT_CC2, DISABLE );
	TIM_ITConfig( TIM4, TIM_IT_CC2 | TIM_IT_CC3, DISABLE );

	GPIO_PinAFConfig( GPIOB, GPIO_PinSource4, GPIO_AF_RTC_50Hz );
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource5, GPIO_AF_RTC_50Hz );
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource7, GPIO_AF_RTC_50Hz );
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource8, GPIO_AF_RTC_50Hz );
}

void ResetSpeedMeasurement( void )
{
	Aux_1_Speed_Tick_Delta = 0;
	Aux_2_Speed_Tick_Delta = 0;

	TIM2->CNT = 0;
	TIM5->CNT = 0;

	TIM2->CR1 &= ~TIM_CR1_CEN;
	TIM5->CR1 &= ~TIM_CR1_CEN;

	TIM3->SR &= ~TIM_IT_CC1; TIM3->SR &= ~TIM_IT_CC2;
	TIM4->SR &= ~TIM_IT_CC2; TIM4->SR &= ~TIM_IT_CC3;
}

void SysTick_Handler( void )
{
	SystemTick++;
}

void TIM3_IRQHandler( void )
{
	TIM3->SR &= ~TIM_SR_UIF;   // interrupt has been handled

	uint32_t sensor_a = 0;
	uint32_t sensor_b = 0;

	if( (TIM5->CR1 & TIM_CR1_CEN) == 0 )
	{
		Aux_2_Speed_Tick_Delta = 0;

		TIM5->CNT = 0;
		TIM5->CR1 |= TIM_CR1_CEN;
	}
	else
	{
		TIM5->CR1 &= ~TIM_CR1_CEN;
		Aux_2_Speed_Tick_Delta = TIM5->CNT;
	}

	if (TIM3->SR & TIM_IT_CC1 )
	{
		TIM3->SR &= ~TIM_IT_CC1;
		sensor_a = TIM_GetCapture1( TIM3 );

		TIM_ITConfig( TIM3, TIM_IT_CC1, DISABLE );

//		GPIO_ToggleBits( GPIOD, GPIO_Pin_14 );
	}

	if (TIM3->SR & TIM_IT_CC2 )
	{
		TIM3->SR &= ~TIM_IT_CC2;
		sensor_b = TIM_GetCapture2( TIM3 );

		TIM_ITConfig( TIM3, TIM_IT_CC2, DISABLE );

//		GPIO_ToggleBits( GPIOD, GPIO_Pin_15 );
	}
}

void TIM4_IRQHandler( void )
{
	TIM4->SR &= ~TIM_SR_UIF;   // interrupt has been handled

	uint32_t sensor_a = 0;
	uint32_t sensor_b = 0;

	if( (TIM2->CR1 & TIM_CR1_CEN) == 0 )
	{
		Aux_1_Speed_Tick_Delta = 0;

		TIM2->CNT = 0;
		TIM2->CR1 |= TIM_CR1_CEN;
	}
	else
	{
		TIM2->CR1 &= ~TIM_CR1_CEN;
		Aux_1_Speed_Tick_Delta = TIM2->CNT;
	}

	if (TIM4->SR & TIM_IT_CC2 )
	{
		TIM4->SR &= ~TIM_IT_CC2;
		sensor_a = TIM_GetCapture2( TIM4 );

		TIM_ITConfig( TIM4, TIM_IT_CC2, DISABLE );

//		GPIO_ToggleBits( GPIOD, GPIO_Pin_12 );
	}

	if (TIM4->SR & TIM_IT_CC3 )
	{
		TIM4->SR &= ~TIM_IT_CC3;
		sensor_b = TIM_GetCapture3( TIM4 );

		TIM_ITConfig( TIM4, TIM_IT_CC3, DISABLE );

//		GPIO_ToggleBits( GPIOD, GPIO_Pin_13 );
	}
}

void TIM7_IRQHandler()
{
    TIM7->SR &= ~TIM_SR_UIF;   // interrupt has been handled

    if( Start_Reaction_Timing == 0 ) return;

	if( Gate_Drop_Delay != 0 )
	{
		Gate_Drop_Delay -= 1;

		unsigned int aux1_sensor = (Aux_1_Sensor_A_Tick | Aux_1_Sensor_B_Tick);
		unsigned int aux2_sensor = (Aux_2_Sensor_A_Tick | Aux_2_Sensor_B_Tick);

		// used by reaction game
		if( aux1_sensor != 0 )
		{
			Aux_1_Sensor_A_Tick = 0xDEAD;
			Aux_1_Sensor_B_Tick = 0xDEAD;
		}

		if( aux2_sensor != 0 )
		{
			Aux_2_Sensor_A_Tick = 0xDEAD;
			Aux_2_Sensor_B_Tick = 0xDEAD;
		}

		return;
	}
}

void TIM6_DAC_IRQHandler()
{
    TIM6->SR &= ~TIM_SR_UIF;   // interrupt has been handled

    Timer6_Tick += 1;

	if( Timing_Active == 1 )
	{
		if( Menu_Array[ AUX_1_CONFIG ].sub_context_2 == INFRARED )
		{
			// if beam was tripped make sure to wait before scanning again while target moves through the beam
			if( Aux_1_Rescan_Delay > 0 )
			{
				Aux_1_Rescan_Delay -= 1;
			}
			else if( Aux_1_Sensor_A_Tick == 0 )
			{
				if( (GPIOB->IDR & 0x0100) == 0 )
					Aux_1_Infrared_Poll += 1;
				else
					Aux_1_Infrared_Poll = 0;

				if( Aux_1_Infrared_Poll > 100 )	// Polling is bad, should only be for 65 times, need to create a hardware interrupt
				{
					Aux_1_Speed_Tick_Delta = 0;

					// todo: WTF is this doing here? move input capture to statement block below
					TIM2->CNT = 0;
					TIM2->CR1 &= ~TIM_CR1_CEN;

					TIM_ITConfig( TIM4, TIM_IT_CC2 | TIM_IT_CC3, ENABLE );

					Aux_1_Infrared_Poll = 0;
					Aux_1_Lap_Counter	+= 1;
					Aux_1_Rescan_Delay	= SENSOR_RESCAN_DELAY; // wait two seconds before scanning again

					if( Aux_1_Lap_Counter > Menu_Array[ AUX_1_CONFIG ].sub_context_3 )
					{
						Aux_1_Sensor_A_Tick = Timer6_Tick;
						Aux_1_Rescan_Delay = 0;
					}
				}
			}
		}
		else
		{
			// read all the auxililary inputs and save their times for later use for recording time and/or speed
			// if it is zero it can be set, if it's not? then it can't be changed until it's value is used and reset
			if( (GPIOB->IDR & 0x0080) == 0 && Aux_1_Sensor_A_Tick == 0 ) // Blue(DC+) & Blue stripe(DC-)
			{
				Aux_1_Sensor_A_Tick = Timer6_Tick;
			}
			if( (GPIOB->IDR & 0x0100) == 0 && Aux_1_Sensor_B_Tick == 0 ) // Orange(DA-) & Orange stripe(DA+)
			{
				Aux_1_Sensor_B_Tick = Timer6_Tick;
			}
		}

		if( Menu_Array[ AUX_2_CONFIG ].sub_context_2 == INFRARED )
		{
			// if beam was tripped make sure to wait before scanning again while target moves through the beam
			if( Aux_2_Rescan_Delay > 0 )
			{
				Aux_2_Rescan_Delay -= 1;
			}
			else if( Aux_2_Sensor_A_Tick == 0 )
			{
				if( (GPIOB->IDR & 0x0020) == 0 )
					Aux_2_Infrared_Poll += 1;
				else
					Aux_2_Infrared_Poll = 0;

				if( Aux_2_Infrared_Poll > 100 )	// Polling is bad, should only be for 65 times, need to create a hardware interrupt
				{
					Aux_2_Speed_Tick_Delta = 0;

					TIM5->CNT = 0;
					TIM5->CR1 &= ~TIM_CR1_CEN;

					TIM_ITConfig( TIM3, TIM_IT_CC1 | TIM_IT_CC2, ENABLE );

					Aux_2_Infrared_Poll = 0;
					Aux_2_Lap_Counter	+= 1;
					Aux_2_Rescan_Delay	= SENSOR_RESCAN_DELAY; // wait two seconds before scanning again

					if( Aux_2_Lap_Counter > Menu_Array[ AUX_2_CONFIG ].sub_context_3 )
					{
						Aux_2_Sensor_A_Tick = Timer6_Tick;
						Aux_2_Rescan_Delay = 0;
					}
				}
			}
		}
		else
		{
			if( (GPIOB->IDR & 0x0010) == 0 && Aux_2_Sensor_A_Tick == 0 ) // Blue(DC+) & Blue stripe(DC-)
			{
				Aux_2_Sensor_A_Tick = Timer6_Tick;
			}
			if( (GPIOB->IDR & 0x0020) == 0 && Aux_2_Sensor_B_Tick == 0 ) // Orange(DA-) & Orange stripe(DA+)
			{
				Aux_2_Sensor_B_Tick = Timer6_Tick;
			}
		}
	}

	// handle dropping the gate
	if( Pulse_Solenoid == 1 )
	{
		if( Gate_Drop_Delay > 0 )
		{
			Gate_Drop_Delay -= 1;
		}
		else
		{
			// handle keeping the gate power on only for a limited time when controlling a solenoid
			if( Solenoid_Pulse_Time > 0 )
			{
				Solenoid_Pulse_Time -= 1;

				SetGatePowerOn();
			}
			else
			{
				SetGatePowerOff();

				Pulse_Solenoid = 0;
			}
		}
	}
	else if( Gate_Power_Off_Pending == 1 )
	{
		if( Gate_Drop_Delay > 0 )
		{
			Gate_Drop_Delay -= 1;
		}
		else
		{
			// electromagnet, air ram, and Pro-Gate drop the gate from here
			if( Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_PROGATE )
				SetGatePowerDownProGate();
			else
				SetGatePowerOff();

			// once the gate drops for the air ram start elapsing the delay for the anti-slam feature if enabled
			if( (Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_AIR_RAM || Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_PROGATE ) &&
				 Menu_Array[ ANTI_SLAM ].sub_context_1 == ANTI_SLAM_ENABLED )
			{
				// initialize all the global variables for the very first time right after the gate has started to drop
				if( Anti_Slam_Active == 0 )
				{
					Anti_Slam_Active 	   = 1;
					Anti_Slam_Start_Delay  = Menu_Array[ ANTI_SLAM ].sub_context_2;
					Anti_Slam_Pulse_Width  = Menu_Array[ ANTI_SLAM ].sub_context_3;
				}
			}
		}
	}

	// handle pulse generation for the anti-slam feature
	if( Menu_Array[ ANTI_SLAM ].sub_context_1 == ANTI_SLAM_ENABLED && Anti_Slam_Active == 1 )
	{
		if( Anti_Slam_Start_Delay > 0 )
		{
			Anti_Slam_Start_Delay -= 1;
		}
		else if( Anti_Slam_Pulse_Width > 0 )
		{
			Anti_Slam_Pulse_Width -= 1;

			if( Anti_Slam_Gate_Up == 0 )
			{
				Anti_Slam_Gate_Up = 1;

				if( Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_PROGATE )
					SetGatePowerUpProGate();
				else
					SetGatePowerOn();
			}
		}
		else
		{
			// all manipulation of the air ram from the anti-slam feature is complete
			Anti_Slam_Gate_Up = 0;
			Anti_Slam_Active  = 0;

			if( Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_PROGATE )
				SetGatePowerDownProGate();
			else
				SetGatePowerOff();
		}
	}

	// make sure not to abort once the lights and tones actually start
	if( Pulse_Solenoid == 1 || Gate_Power_Off_Pending == 1 ) return;

	// abort if any button is pressed only when in drop gate state
	if( Dropping_Gate == 1 && Cadence_Cancelled == 0 && Anti_Slam_Active == 0 )
	{
		if( (GPIOE->IDR & 0x0010) == 0 || ( Menu_Array[ WIRELESS_REMOTE ].context == WIRELESS_REMOTE_ENABLED &&
										  ((GPIOA->IDR & 0x0001) || (GPIOB->IDR & 0x4000) || (GPIOB->IDR & 0x8000) || (GPIOD->IDR & 0x0100))) )
		{
			Cadence_Cancelled = 1;

			if( Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_SOLENOID )
			{
				Pulse_Solenoid	 	= 0;
				Solenoid_Pulse_Time = 0;
			}

			return;
		}
	}
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
		display_memory[ 0 ][ i ] = 0x20;
		display_memory[ 1 ][ i ] = 0x20;

		display_memory_cache[ 0 ][ i ] = 0x20;
		display_memory_cache[ 1 ][ i ] = 0x20;
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

void PlayRaiseGateAnimation( void )
{
	char R[ 8 + 8 ] = { 0,0,0,0,0,0,0,0, 0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11, 0x00 };
	char a[ 8 + 8 ] = { 0,0,0,0,0,0,0,0, 0x00, 0x00, 0x0E, 0x01, 0x0F, 0x11, 0x0F, 0x00 };
	char i[ 8 + 8 ] = { 0,0,0,0,0,0,0,0, 0x04, 0x00, 0x0C, 0x04, 0x04, 0x04, 0x0E, 0x00 };
	char s[ 8 + 8 ] = { 0,0,0,0,0,0,0,0, 0x00, 0x00, 0x0E, 0x10, 0x0E, 0x01, 0x1E, 0x00 };
	char n[ 8 + 8 ] = { 0,0,0,0,0,0,0,0, 0x00, 0x00, 0x16, 0x19, 0x11, 0x11, 0x11, 0x00 };
	char g[ 8 + 8 ] = { 0,0,0,0,0,0,0,0, 0x00, 0x0F, 0x11, 0x11, 0x0F, 0x01, 0x0E, 0x00 };
	char t[ 8 + 8 ] = { 0,0,0,0,0,0,0,0, 0x08, 0x08, 0x1C, 0x08, 0x08, 0x09, 0x06, 0x00 };
	char e[ 8 + 8 ] = { 0,0,0,0,0,0,0,0, 0x00, 0x00, 0x0E, 0x11, 0x1F, 0x10, 0x0E, 0x00 };

	unsigned int pixel_scroll, display_line, pixel_line;

	for( pixel_scroll = 0; pixel_scroll < 8; pixel_scroll++ )
	{
		// scroll each line of pixels up
		for( display_line = 0; display_line < 15; display_line++ )
		{
			R[ display_line ] = R[ display_line + 1 ];
			a[ display_line ] = a[ display_line + 1 ];
			i[ display_line ] = i[ display_line + 1 ];
			s[ display_line ] = s[ display_line + 1 ];
			n[ display_line ] = n[ display_line + 1 ];
			g[ display_line ] = g[ display_line + 1 ];
			t[ display_line ] = t[ display_line + 1 ];
			e[ display_line ] = e[ display_line + 1 ];
		}

		// define custom characters and display
		WriteLCD_Command( 0x40 ); // Character code 0x00, start of CGRAM

		for( pixel_line = 0; pixel_line < 8; pixel_line++ ) WriteLCD_Data( (int)R[ pixel_line ] );
		for( pixel_line = 0; pixel_line < 8; pixel_line++ )	WriteLCD_Data( (int)a[ pixel_line ] );
		for( pixel_line = 0; pixel_line < 8; pixel_line++ )	WriteLCD_Data( (int)i[ pixel_line ] );
		for( pixel_line = 0; pixel_line < 8; pixel_line++ )	WriteLCD_Data( (int)s[ pixel_line ] );
		for( pixel_line = 0; pixel_line < 8; pixel_line++ )	WriteLCD_Data( (int)n[ pixel_line ] );
		for( pixel_line = 0; pixel_line < 8; pixel_line++ )	WriteLCD_Data( (int)g[ pixel_line ] );
		for( pixel_line = 0; pixel_line < 8; pixel_line++ )	WriteLCD_Data( (int)t[ pixel_line ] );
		for( pixel_line = 0; pixel_line < 8; pixel_line++ )	WriteLCD_Data( (int)e[ pixel_line ] );

		WriteLCD_Line( "    \0\1\2\3\2\4\5 \5\1\6\7    ", 1 );
		UpdateLCD();

		Delay( 160 );
	}

	WriteLCD_Line( "    Raising gate    ", 1 );
	UpdateLCD();

	DefineCustomCharacters();
}

void PlayDropGateAnimation( void )
{
	char D[ 8 ] = { 0x1C, 0x12, 0x11, 0x11, 0x11, 0x12, 0x1C, 0x00 };
	char R[ 8 ] = { 0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11, 0x00 };
	char O[ 8 ] = { 0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E, 0x00 };
	char P[ 8 ] = { 0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10, 0x00 };

	char G[ 8 ] = { 0x0E, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0F, 0x00 };
	char A[ 8 ] = { 0x0E, 0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x00 };
	char T[ 8 ] = { 0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00 };
	char E[ 8 ] = { 0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F, 0x00 };

	unsigned int i, j;

	for( i = 0; i < 9; i++ )
	{
		Delay( 30 - 2 * i );
		WriteLCD_Command( 0x40 ); // Character code 0x00, start of CGRAM

		for( j = 0; j < 8; j++ ) WriteLCD_Data( (int)D[ j ] );
		for( j = 0; j < 8; j++ ) WriteLCD_Data( (int)R[ j ] );
		for( j = 0; j < 8; j++ ) WriteLCD_Data( (int)O[ j ] );
		for( j = 0; j < 8; j++ ) WriteLCD_Data( (int)P[ j ] );

		for( j = 0; j < 8; j++ ) WriteLCD_Data( (int)G[ j ] );
		for( j = 0; j < 8; j++ ) WriteLCD_Data( (int)A[ j ] );
		for( j = 0; j < 8; j++ ) WriteLCD_Data( (int)T[ j ] );
		for( j = 0; j < 8; j++ ) WriteLCD_Data( (int)E[ j ] );

		WriteLCD_Line( "      \0\1\2\3 \4\5\6\7     ", 0 );
		UpdateLCD();

		for( j = 7; j > 0; j-- )
		{
			D[ j ] = D[ j - 1 ];
			R[ j ] = R[ j - 1 ];
			O[ j ] = O[ j - 1 ];
			P[ j ] = P[ j - 1 ];

			G[ j ] = G[ j - 1 ];
			A[ j ] = A[ j - 1 ];
			T[ j ] = T[ j - 1 ];
			E[ j ] = E[ j - 1 ];
		}

		D[ 0 ] = 0;
		R[ 0 ] = 0;
		O[ 0 ] = 0;
		P[ 0 ] = 0;

		G[ 0 ] = 0;
		A[ 0 ] = 0;
		T[ 0 ] = 0;
		E[ 0 ] = 0;
	}

	// heh, scroll the bottom line after drop gate text scrolls away
	char line[ DISPLAY_WIDTH ];
	strcpy( line, display_memory_cache[1] );

	for( i = 0; i < DISPLAY_WIDTH; i++ )
	{
		for( j = DISPLAY_WIDTH; j > 0; j-- )
		{
			line[ j ] = line[ j - 1 ];
		}

		line[ j - 1 ] = 0x20;
		WriteLCD_Line( line, 1 );
		UpdateLCD();

		Delay( 20 - i );
	}

	Delay( 150 );

	DefineCustomCharacters();
}

void WriteLCD_Command( const int data )
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

void WriteLCD_Line( const char * pString, const unsigned int line_number )
{
	int i, j;

	for( i = 0, j = 0; j < DISPLAY_WIDTH; i++, j++ )
		display_memory[ line_number ][ i ] = pString[ j ];
}

void WriteLCD_LineCentered( const char * pString, const unsigned int line_number )
{
	int i = 0;

	// fill display memory with spaces
	for( ; i < DISPLAY_WIDTH; i++ )
		display_memory[ line_number ][ i ] = 0x20;

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
		if( display_memory[ 0 ][ i ] != display_memory_cache[ 0 ][ i ] )
		{
			// new character is different than old so write a new one out
			WriteLCD_Data( display_memory[ 0 ][ i ] );
			display_memory_cache[ 0 ][ i ] = display_memory[ 0 ][ i ];
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
		if( display_memory[ 1 ][ i ] != display_memory_cache[ 1 ][ i ] )
		{
			// new character is different than old so write a new one out
			WriteLCD_Data( display_memory[ 1 ][ i ] );
			display_memory_cache[ 1 ][ i ] = display_memory[ 1 ][ i ];
		}
		else
		{
			// don't write a new character, just advance cursor to next DDRAM address
			WriteLCD_Command( 0xC0 + 1 + i );
		}
	}
}

void PulseSolenoid( const unsigned int pulse_time )
{
	Gate_Drop_Delay	= Menu_Array[ GATE_DROPS_ON ].context * 10;

	Solenoid_Pulse_Time = pulse_time * 10;

	Pulse_Solenoid  = 1;
}

void StartGatePowerOffTimer( void )
{
	Gate_Drop_Delay	= Menu_Array[ GATE_DROPS_ON ].context * 10;

	Gate_Power_Off_Pending = 1;
}

void SetGatePowerOff( void )
{
	GPIO_ResetBits( GPIOC, GPIO_Pin_5 ); // (GUP) low

	GPIO_ResetBits( GPIOA, GPIO_Pin_2 ); // (G1A) gate 1 OFF
	GPIO_ResetBits( GPIOA, GPIO_Pin_1 ); // (G2A) gate 2 OFF

	Gate_Power_State 	   	= POWER_OFF;
	Gate_Power_Off_Pending 	= 0;
}

void SetGatePowerOn( void )
{
	GPIO_SetBits( GPIOA, GPIO_Pin_2 );	// (G1A) gate 1 ON
	GPIO_SetBits( GPIOA, GPIO_Pin_1 );	// (G2A) gate 2 ON

	Gate_Power_State 		= POWER_ON;
	Gate_Power_Off_Pending 	= 0;
}

void SetGatePowerUpProGate( void )
{
	// Raise gate ports 1 & 2
	GPIO_SetBits( GPIOC, GPIO_Pin_2 );	 // (G1M) high
	GPIO_SetBits( GPIOC, GPIO_Pin_1 );	 // (G2M) high
	GPIO_ResetBits( GPIOC, GPIO_Pin_5 ); // (GUP) low
	GPIO_SetBits( GPIOA, GPIO_Pin_2 );	 // (G1A) high
	GPIO_SetBits( GPIOA, GPIO_Pin_1 );	 // (G2A) high

	Gate_Power_State 		= POWER_ON;
	Gate_Power_Off_Pending 	= 0;
}

void SetGatePowerDownProGate( void )
{
	// Drop gate ports 1 & 2
	GPIO_SetBits( GPIOC, GPIO_Pin_2 );	 // (G1M) high
	GPIO_SetBits( GPIOC, GPIO_Pin_1 );	 // (G2M) high
	GPIO_ResetBits( GPIOA, GPIO_Pin_2 ); // (G1A) low
	GPIO_ResetBits( GPIOA, GPIO_Pin_1 ); // (G2A) low
	GPIO_SetBits( GPIOC, GPIO_Pin_5 );   // (GUP) high

	Gate_Power_State 	   	= POWER_OFF;
	Gate_Power_Off_Pending 	= 0;
}

void ShortDelay( const unsigned int ticks )
{
	const unsigned int end_time = Timer6_Tick + ticks;

	while( Timer6_Tick < end_time );
}

void Delay( const unsigned int milliseconds )
{
	ShortDelay( milliseconds * 10 );
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

void SetAttenuator( u16 data )
{
	GPIO_ResetBits( GPIOC, GPIO_Pin_4 );

	while( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET );
	SPI_I2S_SendData( SPI1, data );

	while( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET );
	SPI_I2S_ReceiveData( SPI1 );

	GPIO_SetBits( GPIOC, GPIO_Pin_4 );
}

void PlaySilence( const unsigned int milliseconds, const unsigned int dont_abort )
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
	while( i-- > 0 && !(Cadence_Cancelled == 1 && dont_abort == 0) );
}

void PlayTone( const unsigned int milliseconds, const int16_t * tone_data, const unsigned int length, const unsigned int dont_abort )
{
	if( Cadence_Cancelled == 1 && dont_abort == 0 ) return;

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

		GPIO_SetBits( GPIOC, GPIO_Pin_14 ); 	// PC14-OSC32 IN

		if ( (i - 6) < 0 ) scale *= 4;

		while ( ! SPI_I2S_GetFlagStatus( CODEC_I2S, SPI_I2S_FLAG_TXE ) );
		SPI_I2S_SendData(CODEC_I2S, sample);	// output right channel

		GPIO_ResetBits( GPIOC, GPIO_Pin_14 );	// PC14-OSC32 IN
	}
	while( i-- > 0 && !(Cadence_Cancelled == 1 && dont_abort == 0) );
}

void PlaySample16khz( const int16_t * pSampleData, const unsigned int start, const unsigned int stop, const unsigned int dont_abort )
{
	int16_t sample;
	unsigned int i;

	for( i = start; i < stop; i++ )
	{
		if( Cadence_Cancelled == 1 && dont_abort == 0 ) return;

		sample = pSampleData[ i ];

		while ( ! SPI_I2S_GetFlagStatus( CODEC_I2S, SPI_I2S_FLAG_TXE ) );
		SPI_I2S_SendData(CODEC_I2S, sample);	// output left channel
		GPIO_SetBits( GPIOC, GPIO_Pin_14 ); 	// PC14-OSC32 IN

		while ( ! SPI_I2S_GetFlagStatus( CODEC_I2S, SPI_I2S_FLAG_TXE ) );
		SPI_I2S_SendData(CODEC_I2S, sample);	// output right channel
		GPIO_ResetBits( GPIOC, GPIO_Pin_14 );	// PC14-OSC32 IN

		while ( ! SPI_I2S_GetFlagStatus( CODEC_I2S, SPI_I2S_FLAG_TXE ) );
		SPI_I2S_SendData(CODEC_I2S, sample);	// output left channel
		GPIO_SetBits( GPIOC, GPIO_Pin_14 ); 	// PC14-OSC32 IN

		while ( ! SPI_I2S_GetFlagStatus( CODEC_I2S, SPI_I2S_FLAG_TXE ) );
		SPI_I2S_SendData(CODEC_I2S, sample);	// output right channel
		GPIO_ResetBits( GPIOC, GPIO_Pin_14 );	// PC14-OSC32 IN

		while ( ! SPI_I2S_GetFlagStatus( CODEC_I2S, SPI_I2S_FLAG_TXE ) );
		SPI_I2S_SendData(CODEC_I2S, sample);	// output left channel
		GPIO_SetBits( GPIOC, GPIO_Pin_14 ); 	// PC14-OSC32 IN

		while ( ! SPI_I2S_GetFlagStatus( CODEC_I2S, SPI_I2S_FLAG_TXE ) );
		SPI_I2S_SendData(CODEC_I2S, sample);	// output right channel
		GPIO_ResetBits( GPIOC, GPIO_Pin_14 );	// PC14-OSC32 IN
	}
}

void PlaySample24khz( const int16_t * pSampleData, const unsigned int start, const unsigned int stop, const unsigned int dont_abort )
{
	int16_t sample = 0;
	int16_t previous = 0;

	unsigned int i;

	for( i = start; i < stop; i++ )
	{
		if( Cadence_Cancelled == 1 && dont_abort == 0 ) return;

		previous = sample;
		sample = pSampleData[ i ];

		while ( ! SPI_I2S_GetFlagStatus( CODEC_I2S, SPI_I2S_FLAG_TXE ) );
		SPI_I2S_SendData( CODEC_I2S, previous + ((sample - previous) / 2) );	// output left
		GPIO_SetBits( GPIOC, GPIO_Pin_14 ); 	// PC14-OSC32 IN

		while ( ! SPI_I2S_GetFlagStatus( CODEC_I2S, SPI_I2S_FLAG_TXE ) );
		SPI_I2S_SendData( CODEC_I2S, previous + ((sample - previous) / 2) );	// output right
		GPIO_ResetBits( GPIOC, GPIO_Pin_14 );	// PC14-OSC32 IN

		while ( ! SPI_I2S_GetFlagStatus( CODEC_I2S, SPI_I2S_FLAG_TXE ) );
		SPI_I2S_SendData(CODEC_I2S, sample);	// output left channel
		GPIO_SetBits( GPIOC, GPIO_Pin_14 ); 	// PC14-OSC32 IN

		while ( ! SPI_I2S_GetFlagStatus( CODEC_I2S, SPI_I2S_FLAG_TXE ) );
		SPI_I2S_SendData(CODEC_I2S, sample);	// output right channel
		GPIO_ResetBits( GPIOC, GPIO_Pin_14 );	// PC14-OSC32 IN
	}
}

void PrintElapsedTime( const unsigned int milliseconds, const unsigned int fractional, const unsigned int display_line )
{
    unsigned int minutes = 0, seconds = 0, tens_place = 0, hund_place = 0, thou_place = 0;

    GetTimeFromTicks( milliseconds, &minutes, &seconds, &tens_place, &hund_place, &thou_place );

    char line[ DISPLAY_WIDTH ] = SPACES;

    if( fractional == 1 )
    {
		if( minutes > 0 )
			sprintf( line, "%d:%d.%d%d%d", minutes, seconds, tens_place, hund_place, thou_place );
		else
			sprintf( line, "%d.%d%d%d", seconds, tens_place, hund_place, thou_place );
    }
    else
    {
		if( minutes > 0 )
			sprintf( line, "%d:%d", minutes, seconds );
		else
			sprintf( line, "%d", seconds );
    }

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

int ReadInputs( int * inputs, int read_sensors )
{
	const unsigned int PortA_IDR = GPIOA->IDR;
	const unsigned int PortB_IDR = GPIOB->IDR;
	const unsigned int PortD_IDR = GPIOD->IDR;

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

	if( read_sensors == 1 )
	{
		// check for timer sensors
		if( (PortB_IDR & 0x0080) == 0 ) { *inputs = AUX_1_SENSOR_A; return 0; }
		if( (PortB_IDR & 0x0100) == 0 ) { *inputs = AUX_1_SENSOR_B; return 0; }
		if( (PortB_IDR & 0x0010) == 0 ) { *inputs = AUX_2_SENSOR_A; return 0; }
		if( (PortB_IDR & 0x0020) == 0 ) { *inputs = AUX_2_SENSOR_B; return 0; }
	}

	*inputs = 0;

	return 0;
}

void WaitForButtonUp()
{
	int inputs = 0;

	do { ReadInputs( &inputs, 0 ); } while ( inputs != 0 );
}

float BatteryLevel( const unsigned int display_level )
{
	float battery_level = 0;
	int a = 0;
	for(; a < 10000; a++ )	battery_level += (float)ReadBatteryCondition() * 100.0f;
	battery_level /= 10000;

	// save battery level at higher precision
	const float charge_level = battery_level;

	if( Starting_Charge_Level < 1.0f )
	{
		Starting_Charge_Level = charge_level;
		return 0;
	}

	Charge_Change += charge_level - Starting_Charge_Level;

//	battery_level = (100000.0f - ( 360000.0f - battery_level )) / 1000.0f;
	battery_level = (70000.0f - ( 356000.0f - battery_level )) / 1000.0f;
	Menu_Array[ BATTERY_CONDITION ].context = ( battery_level > 100.0f ) ? 100 : (int)battery_level;

	// 85% battery level is 13vdc

	// recharge at battery_level == 290,000
	if( charge_level < 285000.0f || display_level == 1 )
	{
		// write condition to bottom line and update the display to show new condition
		WriteLCD_LineCentered( "* RECHARGE BATTERY *", 0 );
		WriteLCD_LineCentered( "* RECHARGE BATTERY *", 1 );

		if( Menu_Array[ BATTERY_CONDITION ].context < 0  )
		{
			if( Charge_Change > 2500 )
			{
				WriteLCD_LineCentered( "* CHARGING *", 0 );
				WriteLCD_LineCentered( "* CHARGING *", 1 );
			}
		}
		else
		{
			WriteLCD_LineCentered( "BATTERY CHARGE LEVEL", 0 );
			sprintf( Menu_Array[ BATTERY_CONDITION ].item[ 0 ], "%d%%", Menu_Array[ BATTERY_CONDITION ].context );
			WriteLCD_LineCentered( Menu_Array[ BATTERY_CONDITION ].item[ 0 ], 1 );
		}

		UpdateLCD();
	}

	return charge_level;
}

int ReadBatteryCondition( void )
{
	ADC_SoftwareStartConv( ADC1 );
	while( !ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) );

#ifdef MEASURE_BATTERY
	int battery_level = ADC_GetConversionValue( ADC1 );
#else
	int battery_level = 0;
#endif
	return battery_level;
}

void PlayTimeOrPercent( const unsigned int ticks, const unsigned int play_type )
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

void PlaySpeed( unsigned int integer, const unsigned int fractional )
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
#endif
}

void PlayDigit( const unsigned int number )
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

void ClearContexts( unsigned int menu_index ) // local helper function for InitMenus()
{
	Menu_Array[ menu_index ].context		= 0;
	Menu_Array[ menu_index ].sub_context_1	= 0;
	Menu_Array[ menu_index ].sub_context_2	= 0;
	Menu_Array[ menu_index ].sub_context_3	= 0;
	Menu_Array[ menu_index ].sub_context_4	= 0;

	Menu_Array[ menu_index ].current_item	= 0;
}

void InitMenus( void )
{
	ClearContexts( DROP_GATE );
	Menu_Array[ DROP_GATE ].menu_type = WAIT_FOR_BUTTON;
	Menu_Array[ DROP_GATE ].item_count = 1;
	SetMenuText( Menu_Array[ DROP_GATE ].caption,	"DROP GATE" );
	SetMenuText( Menu_Array[ DROP_GATE ].item[ 0 ],	"Press button" );

	ClearContexts( RAISE_GATE );
	Menu_Array[ RAISE_GATE ].menu_type	= WAIT_FOR_BUTTON;
	Menu_Array[ RAISE_GATE ].item_count	= 1;
	SetMenuText( Menu_Array[ RAISE_GATE ].caption,	"RAISE GATE" );
	SetMenuText( Menu_Array[ RAISE_GATE ].item[ 0 ],"Press button" );

	ClearContexts( ENERGIZE_MAGNET );
	Menu_Array[ ENERGIZE_MAGNET ].menu_type	 = WAIT_FOR_BUTTON;
	Menu_Array[ ENERGIZE_MAGNET ].item_count = 1;
	SetMenuText( Menu_Array[ ENERGIZE_MAGNET ].caption,	  "ENERGIZE MAGNET" );
	SetMenuText( Menu_Array[ ENERGIZE_MAGNET ].item[ 0 ], "Press button" );

	ClearContexts( GATE_START_DELAY );
	Menu_Array[ GATE_START_DELAY ].menu_type  = EDIT_VALUE;
	Menu_Array[ GATE_START_DELAY ].item_count = 1;
	SetMenuText( Menu_Array[ GATE_START_DELAY ].caption,   "GATE START DELAY" );
	SetMenuText( Menu_Array[ GATE_START_DELAY ].item[ 0 ], "No Delay" );

	ClearContexts( GATE_START_WARNING );
	Menu_Array[ GATE_START_WARNING ].menu_type	= EDIT_VALUE;
	Menu_Array[ GATE_START_WARNING ].item_count	= 1;
	SetMenuText( Menu_Array[ GATE_START_WARNING ].caption,   "GATE START WARNING" );
	SetMenuText( Menu_Array[ GATE_START_WARNING ].item[ 0 ], "No Warning" );

	ClearContexts( GATE_DROPS_ON );
	Menu_Array[ GATE_DROPS_ON ].menu_type  = EDIT_VALUE;
	Menu_Array[ GATE_DROPS_ON ].context	   = 360;
	Menu_Array[ GATE_DROPS_ON ].item_count = 1;
	SetMenuText( Menu_Array[ GATE_DROPS_ON ].caption, "GATE DROPS ON" );
	sprintf( Menu_Array[ GATE_DROPS_ON ].item[ 0 ],	  "Green" );

	// sub menu text for auxiliary sensor type options
	strcpy( Sensor_Text[ RIBBON_SWITCH ],	"\1 Ribbon Switch \2" );
	strcpy( Sensor_Text[ INFRARED 	   ],	"\1 Infrared \2" 	  );
	strcpy( Sensor_Text[ LASER		   ],	"\1 Laser \2" 		  );
	strcpy( Sensor_Text[ PROXIMITY	   ],	"\1 Proximity \2" 	  );
	strcpy( Sensor_Text[ WIRELESS	   ],	"\1 Wireless \2" 	  );

	ClearContexts( AUX_1_CONFIG );
	Menu_Array[ AUX_1_CONFIG ].menu_type	 = EDIT_CHOICE;
	Menu_Array[ AUX_1_CONFIG ].context 		 = AUX_DISABLED;
	Menu_Array[ AUX_1_CONFIG ].sub_context_1 = 12 * 16 * 625;
	Menu_Array[ AUX_1_CONFIG ].item_count 	 = 10;
	Menu_Array[ AUX_1_CONFIG ].current_item	 = 1;
	SetMenuText( Menu_Array[ AUX_1_CONFIG ].caption, "AUXILIARY 1" );
	SetMenuText( Menu_Array[ AUX_1_CONFIG ].item[ 0 ],	SPACES );
	SetMenuText( Menu_Array[ AUX_1_CONFIG ].item[ AUX_DISABLED 		 ],	"\1 Disabled \2" 		);
	SetMenuText( Menu_Array[ AUX_1_CONFIG ].item[ AUX_TIME 			 ],	"\1 Time \2" 			);
	SetMenuText( Menu_Array[ AUX_1_CONFIG ].item[ AUX_TIME_SPEED 	 ],	"\1 Time + Speed \2" 	);
	SetMenuText( Menu_Array[ AUX_1_CONFIG ].item[ AUX_SPRINT_TIMER 	 ],	"\1 Sprint Timer \2" 	);
	SetMenuText( Menu_Array[ AUX_1_CONFIG ].item[ AUX_GATE_SWITCH 	 ],	"\1 Gate Drop Switch \2");
	SetMenuText( Menu_Array[ AUX_1_CONFIG ].item[ AUX_SENSOR_SPACING ],	"\1Set Sensor Spacing\2");
	SetMenuText( Menu_Array[ AUX_1_CONFIG ].item[ AUX_SENSOR_TYPE 	 ], "\1 Set Sensor Type \2" );
	SetMenuText( Menu_Array[ AUX_1_CONFIG ].item[ AUX_ALIGN_SENSOR_A ], "\1 Align Channel A \2" );
	SetMenuText( Menu_Array[ AUX_1_CONFIG ].item[ AUX_ALIGN_SENSOR_B ], "\1 Align Channel B \2" );
	SetMenuText( Menu_Array[ AUX_1_CONFIG ].item[ AUX_LAP_COUNT 	 ], "\1 Set Lap Count \2" 	);
	ItemCopy( AUX_1_CONFIG, Menu_Array[ AUX_1_CONFIG ].context, 0, 1 );
	Aux_1_Sensor_Spacing = Menu_Array[ AUX_1_CONFIG ].sub_context_1;

	ClearContexts( AUX_2_CONFIG );
	Menu_Array[ AUX_2_CONFIG ].menu_type	 = EDIT_CHOICE;
	Menu_Array[ AUX_2_CONFIG ].context 		 = AUX_DISABLED;
	Menu_Array[ AUX_2_CONFIG ].sub_context_1 = 12 * 16 * 625;
	Menu_Array[ AUX_2_CONFIG ].item_count 	 = 10;
	Menu_Array[ AUX_2_CONFIG ].current_item	 = 1;
	SetMenuText( Menu_Array[ AUX_2_CONFIG ].caption, "AUXILIARY 2" );
	SetMenuText( Menu_Array[ AUX_2_CONFIG ].item[ 0 ],	SPACES );
	SetMenuText( Menu_Array[ AUX_2_CONFIG ].item[ AUX_DISABLED		 ],	"\1 Disabled \2" 		);
	SetMenuText( Menu_Array[ AUX_2_CONFIG ].item[ AUX_TIME 			 ],	"\1 Time \2" 			);
	SetMenuText( Menu_Array[ AUX_2_CONFIG ].item[ AUX_TIME_SPEED	 ],	"\1 Time + Speed \2" 	);
	SetMenuText( Menu_Array[ AUX_2_CONFIG ].item[ AUX_SPRINT_TIMER	 ],	"\1 Sprint Timer \2" 	);
	SetMenuText( Menu_Array[ AUX_2_CONFIG ].item[ AUX_GATE_SWITCH	 ],	"\1 Gate Drop Switch \2");
	SetMenuText( Menu_Array[ AUX_2_CONFIG ].item[ AUX_SENSOR_SPACING ],	"\1Set Sensor Spacing\2");
	SetMenuText( Menu_Array[ AUX_2_CONFIG ].item[ AUX_SENSOR_TYPE 	 ], "\1 Set Sensor Type \2" );
	SetMenuText( Menu_Array[ AUX_2_CONFIG ].item[ AUX_ALIGN_SENSOR_A ], "\1 Align Channel A \2" );
	SetMenuText( Menu_Array[ AUX_2_CONFIG ].item[ AUX_ALIGN_SENSOR_B ], "\1 Align Channel B \2" );
	SetMenuText( Menu_Array[ AUX_2_CONFIG ].item[ AUX_LAP_COUNT 	 ], "\1 Set Lap Count \2" 	);
	ItemCopy( AUX_2_CONFIG, Menu_Array[ AUX_2_CONFIG ].context, 0, 1 );
	Aux_2_Sensor_Spacing = Menu_Array[ AUX_2_CONFIG ].sub_context_1;

	ClearContexts( TIMER_HISTORY );
	Menu_Array[ TIMER_HISTORY ].menu_type  = VIEW_LIST;
	Menu_Array[ TIMER_HISTORY ].item_count = 1;
	SetMenuText( Menu_Array[ TIMER_HISTORY ].caption,	"TIMER HISTORY" );
	SetMenuText( Menu_Array[ TIMER_HISTORY ].item[ 0 ], "Nothing recorded" );
	ItemCopy( TIMER_HISTORY, Menu_Array[ TIMER_HISTORY ].context, 0, 1 );

	ClearContexts( CLEAR_TIMER_HISTORY );
	Menu_Array[ CLEAR_TIMER_HISTORY ].menu_type	 = VIEW_LIST;
	Menu_Array[ CLEAR_TIMER_HISTORY ].item_count = 1;
	SetMenuText( Menu_Array[ CLEAR_TIMER_HISTORY ].caption,   "CLEAR TIMER HISTORY" );
	SetMenuText( Menu_Array[ CLEAR_TIMER_HISTORY ].item[ 0 ], "Press Button" );
	ItemCopy( CLEAR_TIMER_HISTORY, Menu_Array[ CLEAR_TIMER_HISTORY ].context, 0, 1 );

	ClearContexts( AUTO_ANNOUNCE_TIMES );
	Menu_Array[ AUTO_ANNOUNCE_TIMES ].menu_type	   = EDIT_CHOICE;
	Menu_Array[ AUTO_ANNOUNCE_TIMES ].context 	   = AUTO_ANNOUNCE_TIMES_ENABLED;
	Menu_Array[ AUTO_ANNOUNCE_TIMES ].item_count   = 2;
	Menu_Array[ AUTO_ANNOUNCE_TIMES ].current_item = 1;
	SetMenuText( Menu_Array[ AUTO_ANNOUNCE_TIMES ].caption, "AUTO ANNOUNCE TIMES" );
	SetMenuText( Menu_Array[ AUTO_ANNOUNCE_TIMES ].item[ 0 ], SPACES );
	SetMenuText( Menu_Array[ AUTO_ANNOUNCE_TIMES ].item[ WIRELESS_REMOTE_DISABLED ], "\1 Disabled \2" );
	SetMenuText( Menu_Array[ AUTO_ANNOUNCE_TIMES ].item[ WIRELESS_REMOTE_ENABLED  ], "\1 Enabled \2"  );
	ItemCopy( AUTO_ANNOUNCE_TIMES, Menu_Array[ AUTO_ANNOUNCE_TIMES ].context, 0, 1 );

	ClearContexts( TOTAL_GATE_DROPS );
	Menu_Array[ TOTAL_GATE_DROPS ].menu_type  = NO_INPUT;
	Menu_Array[ TOTAL_GATE_DROPS ].item_count = 0;
	SetMenuText( Menu_Array[ TOTAL_GATE_DROPS ].caption, "TOTAL GATE DROPS" );
	sprintf( Menu_Array[ TOTAL_GATE_DROPS ].item[ 0 ], "%d", Menu_Array[ TOTAL_GATE_DROPS ].context );

	ClearContexts( ZERO_TOTAL_GATE_DROPS );
	Menu_Array[ ZERO_TOTAL_GATE_DROPS ].menu_type  = VIEW_LIST;
	Menu_Array[ ZERO_TOTAL_GATE_DROPS ].item_count = 1;
	SetMenuText( Menu_Array[ ZERO_TOTAL_GATE_DROPS ].caption,   "ZERO GATE DROP COUNT" );
	SetMenuText( Menu_Array[ ZERO_TOTAL_GATE_DROPS ].item[ 0 ], "Press Button" );
	ItemCopy( ZERO_TOTAL_GATE_DROPS, Menu_Array[ ZERO_TOTAL_GATE_DROPS ].context, 0, 1 );

	ClearContexts( SAVE_TOTAL_GATE_DROPS );
	Menu_Array[ SAVE_TOTAL_GATE_DROPS ].menu_type  = VIEW_LIST;
	Menu_Array[ SAVE_TOTAL_GATE_DROPS ].item_count = 1;
	SetMenuText( Menu_Array[ SAVE_TOTAL_GATE_DROPS ].caption,   "SAVE GATE DROP COUNT" );
	SetMenuText( Menu_Array[ SAVE_TOTAL_GATE_DROPS ].item[ 0 ], "Press Button" );
	ItemCopy( SAVE_TOTAL_GATE_DROPS, Menu_Array[ SAVE_TOTAL_GATE_DROPS ].context, 0, 1 );

	ClearContexts( CADENCE_VOLUME );
	Menu_Array[ CADENCE_VOLUME ].menu_type	= EDIT_VALUE;
	Menu_Array[ CADENCE_VOLUME ].context	= 2;
	Menu_Array[ CADENCE_VOLUME ].item_count = 1;
	SetMenuText( Menu_Array[ CADENCE_VOLUME ].caption, "CADENCE VOLUME" );
	sprintf( Menu_Array[ CADENCE_VOLUME ].item[ 0 ], "%d%%", Menu_Array[ CADENCE_VOLUME ].context );

	ClearContexts( AUDIO_IN_VOLUME );
	Menu_Array[ AUDIO_IN_VOLUME ].menu_type	 = EDIT_VALUE;
	Menu_Array[ AUDIO_IN_VOLUME ].context	 = 25;
	Menu_Array[ AUDIO_IN_VOLUME ].item_count = 1;
	SetMenuText( Menu_Array[ AUDIO_IN_VOLUME ].caption, "EXT AUDIO IN VOLUME" );
	sprintf( Menu_Array[ AUDIO_IN_VOLUME ].item[ 0 ], "%d%%", Menu_Array[ AUDIO_IN_VOLUME ].context );

	ClearContexts( BATTERY_CONDITION );
	Menu_Array[ BATTERY_CONDITION ].menu_type  = DISPLAY_VALUE;
	Menu_Array[ BATTERY_CONDITION ].context	   = 100.0f * ReadBatteryCondition() / 3560;
	Menu_Array[ BATTERY_CONDITION ].item_count = 1;
	SetMenuText( Menu_Array[ BATTERY_CONDITION ].caption, "BATTERY CONDITION" );
	sprintf( Menu_Array[ BATTERY_CONDITION ].item[ 0 ],	"%d%%", Menu_Array[ BATTERY_CONDITION ].context );

	ClearContexts( TEST_LIGHTS );
	Menu_Array[ TEST_LIGHTS ].menu_type	   = EDIT_CHOICE;
	Menu_Array[ TEST_LIGHTS ].context 	   = TEST_LIGHTS_ALL_ON;
	Menu_Array[ TEST_LIGHTS ].item_count   = 7; // TODO: finish the random fade light effect
	Menu_Array[ TEST_LIGHTS ].current_item = 1;
	SetMenuText( Menu_Array[ TEST_LIGHTS ].caption, "TEST LIGHTS" );
	SetMenuText( Menu_Array[ TEST_LIGHTS ].item[ 0 ], SPACES );
	SetMenuText( Menu_Array[ TEST_LIGHTS ].item[ TEST_LIGHTS_ALL_ON   	],	"\1 All Lights On \2"  	);
	SetMenuText( Menu_Array[ TEST_LIGHTS ].item[ TEST_LIGHTS_RED		],	"\1 Red On \2"			);
	SetMenuText( Menu_Array[ TEST_LIGHTS ].item[ TEST_LIGHTS_AMBER1		],	"\1 Amber 1 On \2"		);
	SetMenuText( Menu_Array[ TEST_LIGHTS ].item[ TEST_LIGHTS_AMBER2		],	"\1 Amber 2 On \2"		);
	SetMenuText( Menu_Array[ TEST_LIGHTS ].item[ TEST_LIGHTS_GREEN		],	"\1 Green On \2"		);
	SetMenuText( Menu_Array[ TEST_LIGHTS ].item[ TEST_LIGHTS_UP_DOWN  	],	"\1 Up/Down Pattern \2" );
	SetMenuText( Menu_Array[ TEST_LIGHTS ].item[ TEST_LIGHTS_DARK_LIGHT ],	"\1 All Lights Fade \2" );
	SetMenuText( Menu_Array[ TEST_LIGHTS ].item[ TEST_LIGHTS_RANDOM		],	"\1 Random Fade \2"		);
	ItemCopy( TEST_LIGHTS, Menu_Array[ TEST_LIGHTS ].context, 0, 1 );

	ClearContexts( REACTION_GAME );
	Menu_Array[ REACTION_GAME ].menu_type	 = EDIT_CHOICE;
	Menu_Array[ REACTION_GAME ].context 	 = REACTION_GAME_ONE_PLAYER;
	Menu_Array[ REACTION_GAME ].item_count 	 = 4;
	Menu_Array[ REACTION_GAME ].current_item = 1;
	SetMenuText( Menu_Array[ REACTION_GAME ].caption, "REACTION GAME" );
	SetMenuText( Menu_Array[ REACTION_GAME ].item[ 0 ],	SPACES );
	SetMenuText( Menu_Array[ REACTION_GAME ].item[ REACTION_GAME_ONE_PLAYER  ],	"\1 One Player \2"  );
	SetMenuText( Menu_Array[ REACTION_GAME ].item[ REACTION_GAME_TWO_PLAYER  ],	"\1 Two Player \2"  );
	SetMenuText( Menu_Array[ REACTION_GAME ].item[ REACTION_GAME_VIEW_STATS  ],	"\1 View Stats \2"  );
	SetMenuText( Menu_Array[ REACTION_GAME ].item[ REACTION_GAME_CLEAR_STATS ],	"\1 Clear Stats \2" );
	ItemCopy( REACTION_GAME, Menu_Array[ REACTION_GAME ].context, 0, 1 );

	ClearContexts( WIRELESS_REMOTE );
	Menu_Array[ WIRELESS_REMOTE ].menu_type	   = EDIT_CHOICE;
	Menu_Array[ WIRELESS_REMOTE ].context 	   = WIRELESS_REMOTE_ENABLED;
	Menu_Array[ WIRELESS_REMOTE ].item_count   = 2;
	Menu_Array[ WIRELESS_REMOTE ].current_item = 1;
	SetMenuText( Menu_Array[ WIRELESS_REMOTE ].caption,	"WIRELESS REMOTE" );
	SetMenuText( Menu_Array[ WIRELESS_REMOTE ].item[ 0 ], SPACES );
	SetMenuText( Menu_Array[ WIRELESS_REMOTE ].item[ WIRELESS_REMOTE_DISABLED ], "\1 Disabled \2" );
	SetMenuText( Menu_Array[ WIRELESS_REMOTE ].item[ WIRELESS_REMOTE_ENABLED  ], "\1 Enabled \2"  );
	ItemCopy( WIRELESS_REMOTE, Menu_Array[ WIRELESS_REMOTE ].context, 0, 1 );

	ClearContexts( RELEASE_DEVICE );
	Menu_Array[ RELEASE_DEVICE ].menu_type	  = EDIT_CHOICE;
	Menu_Array[ RELEASE_DEVICE ].context	  = RELEASE_DEVICE_SOLENOID;
	Menu_Array[ RELEASE_DEVICE ].item_count	  = 4;
	Menu_Array[ RELEASE_DEVICE ].current_item = RELEASE_DEVICE_SOLENOID;
	SetMenuText( Menu_Array[ RELEASE_DEVICE ].caption, "RELEASE DEVICE" );
	SetMenuText( Menu_Array[ RELEASE_DEVICE ].item[ 0 ], SPACES );
	SetMenuText( Menu_Array[ RELEASE_DEVICE ].item[ RELEASE_DEVICE_SOLENOID ], "\1 Solenoid \2" );
	SetMenuText( Menu_Array[ RELEASE_DEVICE ].item[ RELEASE_DEVICE_MAGNET	], "\1 Magnet \2"	);
	SetMenuText( Menu_Array[ RELEASE_DEVICE ].item[ RELEASE_DEVICE_AIR_RAM	], "\1 Air Ram \2"	);
	SetMenuText( Menu_Array[ RELEASE_DEVICE ].item[ RELEASE_DEVICE_PROGATE	], "\1 Pro-Gate \2"	);
	ItemCopy( RELEASE_DEVICE, Menu_Array[ RELEASE_DEVICE ].context, 0, 1 );

	Menu_Array[ ANTI_SLAM ].menu_type	  = EDIT_CHOICE;
	Menu_Array[ ANTI_SLAM ].context		  = ANTI_SLAM_DISABLED;
	Menu_Array[ ANTI_SLAM ].sub_context_1 = ANTI_SLAM_DISABLED;
	Menu_Array[ ANTI_SLAM ].sub_context_2 = 2500; // ANTI_SLAM_START_DELAY
	Menu_Array[ ANTI_SLAM ].sub_context_3 = 2500; // ANTI_SLAM_PULSE_WIDTH
	Menu_Array[ ANTI_SLAM ].sub_context_4 = 2;	  // ANTI_SLAM_INCREMENT - corresponds to case statement to set global variable = 1, 10, 100, 1000
	Menu_Array[ ANTI_SLAM ].current_item  = 0;
	Menu_Array[ ANTI_SLAM ].item_count    = 7;
	SetMenuText( Menu_Array[ ANTI_SLAM ].caption, "ANTI-SLAM FEATURE" );
	SetMenuText( Menu_Array[ ANTI_SLAM ].item[ 0 ], SPACES );
	SetMenuText( Menu_Array[ ANTI_SLAM ].item[ ANTI_SLAM_DISABLED	  ], "\1 Disabled \2"		  );
	SetMenuText( Menu_Array[ ANTI_SLAM ].item[ ANTI_SLAM_ENABLED	  ], "\1 Enabled \2"		  );
	SetMenuText( Menu_Array[ ANTI_SLAM ].item[ ANTI_SLAM_RAISE_GATE	  ], "\1 Raise Gate \2"		  );
	SetMenuText( Menu_Array[ ANTI_SLAM ].item[ ANTI_SLAM_DROP_GATE	  ], "\1 Drop Gate \2"		  );
	SetMenuText( Menu_Array[ ANTI_SLAM ].item[ ANTI_SLAM_START_DELAY  ], "\1 Set Start Delay \2"  );
	SetMenuText( Menu_Array[ ANTI_SLAM ].item[ ANTI_SLAM_PULSE_WIDTH  ], "\1 Set Pulse Width \2"  );
	SetMenuText( Menu_Array[ ANTI_SLAM ].item[ ANTI_SLAM_INCREMENT	  ], "\1 Set Increment \2" 	  );
	ItemCopy( ANTI_SLAM, Menu_Array[ ANTI_SLAM ].context, 0, 1 );

	// initialize timing histories array
	ClearTimingHistories();
}

void SetGateMenuState( void )
{
	if( (Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_AIR_RAM) || ( Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_PROGATE ) )
	{
		if( Gate_Power_State == POWER_OFF )
			Menu_Index = RAISE_GATE;
		else
			Menu_Index = DROP_GATE;
	}
	else if( Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_MAGNET )
	{
		if( Gate_Power_State == POWER_OFF )
			Menu_Index = ENERGIZE_MAGNET;
		else
			Menu_Index = DROP_GATE;
	}
	else if( Menu_Array[ RELEASE_DEVICE ].context == RELEASE_DEVICE_SOLENOID )
	{
		Menu_Index = DROP_GATE;
	}
}

void ClearTimingHistories( void )
{
	Timer_History_Index  = 0;
	Timer_History_Number = 0;

	Menu_Array[ TIMER_HISTORY ].context = 0;

	int j, k;
	for( j = 0; j < MAX_TIMING_HISTORIES; j++ )
	{
		Timer_History[ j ].number			= 0;
		Timer_History[ j ].elapsed_time		= 0;
		Timer_History[ j ].speed_integer	= 0;
		Timer_History[ j ].speed_fractional	= 0;
		Timer_History[ j ].is_a_speed		= 0;
		Timer_History[ j ].aux_port			= 0;

		for( k = 0; k < DISPLAY_WIDTH; k++ )
		{
			Timer_History[ j ].time_string[ k ]		  = 0;
			Timer_History[ j ].speed_string[ k ]	  = 0;
			Timer_History[ j ].time_speed_string[ k ] = 0;
		}
	}
}

void SaveEverythingToFlashMemory( int write_UUID_mask )
{
	FLASH_Unlock();

	// clear pending flags
	FLASH_ClearFlag( FLASH_FLAG_EOP	   | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
				  	 FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR| FLASH_FLAG_PGSERR );

	// get the number of the start and end sectors
	uint32_t start_sector = GetFlashSector( FLASH_SAVE_MEMORY_START );
	uint32_t end_sector	  = GetFlashSector( FLASH_SAVE_MEMORY_END   );

	uint32_t i = 0;
	// only using sector 1 but this will handle erasing more than one sector if needed later
	for( i = start_sector; i <= end_sector; i += 8 )
	{
		// device voltage range supposed to be [2.7V to 3.6V], the operation will be done by word
		if( FLASH_EraseSector( i, VoltageRange_3 ) != FLASH_COMPLETE )
		{
			// TODO: add code to handle the case when erasing a sector fails
		}
	}

	// when ReadEverythingFromFlashMemory is called there is the first initial case where flash memory
	// has never been written to so it aborts reading if it doesn't find the 0xDEADBEEF
	uint32_t write_address	  = FLASH_SAVE_MEMORY_START;
	FLASH_Status flash_status = FLASH_ProgramWord( write_address, 0xDEADBEEF ); write_address += 4;

	// save UUID mask
	if( write_UUID_mask != 0 )
	{
		if( write_UUID_mask == 1 )
		{
			UUID_Mask = (ADDR_UUID[0] ^ ADDR_UUID[1] ^ ADDR_UUID[2]);
		    FLASH_ProgramWord( write_address, UUID_Mask );
		}
		else if( write_UUID_mask == 2 )
		{
		    FLASH_ProgramWord( write_address, 0x12345678 );
		}

	    UUID_Check = UUID_VALIDATE_CHECKS;
	}
	else
	{
		FLASH_ProgramWord( write_address, UUID_Mask );
	}

	write_address += 4;

	FLASH_ProgramWord( write_address, UUID_Check ); write_address += 4;

	// first save out the entire timer histories array
	uint32_t *p_data = (uint32_t *) Timer_History;

	uint32_t stop_address = write_address + sizeof( Timer_History );

	for( i = 0; write_address < stop_address; i++ )
	{
		flash_status = FLASH_ProgramWord( write_address, *p_data );

		if( flash_status == FLASH_ERROR_PROGRAM )
		{
			WriteLCD_LineCentered( "Flash_ProgramWord", 0 );
			WriteLCD_LineCentered( "failed", 0 );
			UpdateLCD();
		}

		if( flash_status == FLASH_COMPLETE )
		{
			p_data++;
			write_address += 4;
		}
	}

	// next save out important statics
	FLASH_ProgramWord( write_address, Timer_History_Index  );	write_address += 4;
	FLASH_ProgramWord( write_address, Timer_History_Number );	write_address += 4;

	FLASH_ProgramWord( write_address, Gate_Drop_Delay 	   );	write_address += 4;

	FLASH_ProgramWord( write_address, Aux_1_Sensor_Spacing );	write_address += 4;
	FLASH_ProgramWord( write_address, Aux_2_Sensor_Spacing );	write_address += 4;

	FLASH_ProgramWord( write_address, Reaction_Game_P1_BEST );	write_address += 4;
	FLASH_ProgramWord( write_address, Reaction_Game_P1_AVG );	write_address += 4;
	FLASH_ProgramWord( write_address, Reaction_Game_P1_WORST );	write_address += 4;

	FLASH_ProgramWord( write_address, Reaction_Game_P2_BEST );	write_address += 4;
	FLASH_ProgramWord( write_address, Reaction_Game_P2_AVG );	write_address += 4;
	FLASH_ProgramWord( write_address, Reaction_Game_P2_WORST );	write_address += 4;

	// save contexts and strings
	FLASH_ProgramWord( write_address, Menu_Array[ TOTAL_GATE_DROPS ].context );	write_address += 4;
	for( i = 0; i < DISPLAY_WIDTH; i++, write_address += 1 )
		FLASH_ProgramByte( write_address, Menu_Array[ TOTAL_GATE_DROPS ].item[ 0 ][ i ] );

	FLASH_ProgramWord( write_address, Menu_Array[ GATE_START_DELAY ].context );	write_address += 4;
	for( i = 0; i < DISPLAY_WIDTH; i++, write_address += 1 )
		FLASH_ProgramByte( write_address, Menu_Array[ GATE_START_DELAY ].item[ 0 ][ i ] );

	FLASH_ProgramWord( write_address, Menu_Array[ GATE_START_WARNING ].context );	write_address += 4;
	for( i = 0; i < DISPLAY_WIDTH; i++, write_address += 1 )
		FLASH_ProgramByte( write_address, Menu_Array[ GATE_START_WARNING ].item[ 0 ][ i ] );

	FLASH_ProgramWord( write_address, Menu_Array[ GATE_DROPS_ON ].context	 );	write_address += 4;
	for( i = 0; i < DISPLAY_WIDTH; i++, write_address += 1 )
		FLASH_ProgramByte( write_address, Menu_Array[ GATE_DROPS_ON ].item[ 0 ][ i ] );

	FLASH_ProgramWord( write_address, Menu_Array[ AUTO_ANNOUNCE_TIMES ].context  );	write_address += 4;
	for( i = 0; i < DISPLAY_WIDTH; i++, write_address += 1 )
		FLASH_ProgramByte( write_address, Menu_Array[ AUTO_ANNOUNCE_TIMES ].item[ 0 ][ i ] );

	FLASH_ProgramWord( write_address, Menu_Array[ AUX_1_CONFIG ].context	 );		write_address += 4;
	FLASH_ProgramWord( write_address, Menu_Array[ AUX_1_CONFIG ].sub_context_1 );	write_address += 4;
	FLASH_ProgramWord( write_address, Menu_Array[ AUX_1_CONFIG ].sub_context_2 );	write_address += 4;
	FLASH_ProgramWord( write_address, Menu_Array[ AUX_1_CONFIG ].sub_context_3 );	write_address += 4;
	for( i = 0; i < DISPLAY_WIDTH; i++, write_address += 1 )
		FLASH_ProgramByte( write_address, Menu_Array[ AUX_1_CONFIG ].item[ 0 ][ i ] );

	FLASH_ProgramWord( write_address, Menu_Array[ AUX_2_CONFIG ].context	   );	write_address += 4;
	FLASH_ProgramWord( write_address, Menu_Array[ AUX_2_CONFIG ].sub_context_1 );	write_address += 4;
	FLASH_ProgramWord( write_address, Menu_Array[ AUX_2_CONFIG ].sub_context_2 );	write_address += 4;
	FLASH_ProgramWord( write_address, Menu_Array[ AUX_2_CONFIG ].sub_context_3 );	write_address += 4;
	for( i = 0; i < DISPLAY_WIDTH; i++, write_address += 1 )
		FLASH_ProgramByte( write_address, Menu_Array[ AUX_2_CONFIG ].item[ 0 ][ i ] );

	FLASH_ProgramWord( write_address, Menu_Array[ TIMER_HISTORY ].context	 );	write_address += 4;
	for( i = 0; i < DISPLAY_WIDTH; i++, write_address += 1 )
		FLASH_ProgramByte( write_address, Menu_Array[ TIMER_HISTORY ].item[ 0 ][ i ] );

	FLASH_ProgramWord( write_address, Menu_Array[ CADENCE_VOLUME ].context	 );	write_address += 4;
	for( i = 0; i < DISPLAY_WIDTH; i++, write_address += 1 )
		FLASH_ProgramByte( write_address, Menu_Array[ CADENCE_VOLUME ].item[ 0 ][ i ] );

	FLASH_ProgramWord( write_address, Menu_Array[ AUDIO_IN_VOLUME ].context  );	write_address += 4;
	for( i = 0; i < DISPLAY_WIDTH; i++, write_address += 1 )
		FLASH_ProgramByte( write_address, Menu_Array[ AUDIO_IN_VOLUME ].item[ 0 ][ i ] );

	FLASH_ProgramWord( write_address, Menu_Array[ WIRELESS_REMOTE ].context  );	write_address += 4;
	for( i = 0; i < DISPLAY_WIDTH; i++, write_address += 1 )
		FLASH_ProgramByte( write_address, Menu_Array[ WIRELESS_REMOTE ].item[ 0 ][ i ] );

	FLASH_ProgramWord( write_address, Menu_Array[ RELEASE_DEVICE ].context   );	write_address += 4;
	for( i = 0; i < DISPLAY_WIDTH; i++, write_address += 1 )
		FLASH_ProgramByte( write_address, Menu_Array[ RELEASE_DEVICE ].item[ 0 ][ i ] );

	FLASH_ProgramWord( write_address, Menu_Array[ ANTI_SLAM ].sub_context_1 );	write_address += 4;
	FLASH_ProgramWord( write_address, Menu_Array[ ANTI_SLAM ].sub_context_2 );	write_address += 4;
	FLASH_ProgramWord( write_address, Menu_Array[ ANTI_SLAM ].sub_context_3 );	write_address += 4;
	FLASH_ProgramWord( write_address, Menu_Array[ ANTI_SLAM ].sub_context_4 );	write_address += 4;
	for( i = 0; i < DISPLAY_WIDTH; i++, write_address += 1 )
		FLASH_ProgramByte( write_address, Menu_Array[ ANTI_SLAM ].item[ 0 ][ i ] );

	// lock the flash memory to disable the flash control register access and protect memory from unwanted writes
	FLASH_Lock();
}

int ReadEverythingFromFlashMemory( void )
{
	volatile uint32_t read_address = (volatile uint32_t)FLASH_SAVE_MEMORY_START;

	if( *(volatile uint32_t *)read_address != 0xDEADBEEF ) return -1;

	read_address += 4;
	UUID_Mask  = *(volatile uint32_t*)read_address; read_address += 4;
	UUID_Check = *(volatile uint32_t*)read_address; read_address += 4;

	volatile uint32_t *p_data = (volatile uint32_t *) & Timer_History;

	uint32_t stop_address = sizeof( Timer_History ) + read_address;

	// read the entire timer history array into itself via one line of code! w00t!
	for( ; read_address < stop_address; read_address += 4 ) *(p_data++) = *(volatile uint32_t*)read_address;

	// read individual statics
	Timer_History_Index		= *(volatile uint32_t*)read_address; read_address += 4;
	Timer_History_Number	= *(volatile uint32_t*)read_address; read_address += 4;

	Gate_Drop_Delay			= *(volatile uint32_t*)read_address; read_address += 4;

	Aux_1_Sensor_Spacing	= *(volatile uint32_t*)read_address; read_address += 4;
	Aux_2_Sensor_Spacing	= *(volatile uint32_t*)read_address; read_address += 4;

	Reaction_Game_P1_BEST	= *(volatile uint32_t*)read_address; read_address += 4;
	Reaction_Game_P1_AVG	= *(volatile uint32_t*)read_address; read_address += 4;
	Reaction_Game_P1_WORST	= *(volatile uint32_t*)read_address; read_address += 4;

	Reaction_Game_P2_BEST	= *(volatile uint32_t*)read_address; read_address += 4;
	Reaction_Game_P2_AVG	= *(volatile uint32_t*)read_address; read_address += 4;
	Reaction_Game_P2_WORST	= *(volatile uint32_t*)read_address; read_address += 4;

	int i;

	Menu_Array[ TOTAL_GATE_DROPS ].context	= *(volatile uint32_t*)read_address; read_address += 4;
	for( i = 0; i < DISPLAY_WIDTH; i++, read_address += 1 )
		Menu_Array[ TOTAL_GATE_DROPS ].item[ 0 ][ i ] = *(volatile uint8_t*)read_address;

	Menu_Array[ GATE_START_DELAY ].context	= *(volatile uint32_t*)read_address; read_address += 4;
	for( i = 0; i < DISPLAY_WIDTH; i++, read_address += 1 )
		Menu_Array[ GATE_START_DELAY ].item[ 0 ][ i ] = *(volatile uint8_t*)read_address;

	Menu_Array[ GATE_START_WARNING ].context	= *(volatile uint32_t*)read_address; read_address += 4;
	for( i = 0; i < DISPLAY_WIDTH; i++, read_address += 1 )
		Menu_Array[ GATE_START_WARNING ].item[ 0 ][ i ] = *(volatile uint8_t*)read_address;

	Menu_Array[ GATE_DROPS_ON ].context		= *(volatile uint32_t*)read_address; read_address += 4;
	for( i = 0; i < DISPLAY_WIDTH; i++, read_address += 1 )
		Menu_Array[ GATE_DROPS_ON ].item[ 0 ][ i ] = *(volatile uint8_t*)read_address;

	Menu_Array[ AUTO_ANNOUNCE_TIMES ].context	= *(volatile uint32_t*)read_address; read_address += 4;
	for( i = 0; i < DISPLAY_WIDTH; i++, read_address += 1 )
		Menu_Array[ AUTO_ANNOUNCE_TIMES ].item[ 0 ][ i ] = *(volatile uint8_t*)read_address;

	Menu_Array[ AUX_1_CONFIG ].context			= *(volatile uint32_t*)read_address; read_address += 4;
	Menu_Array[ AUX_1_CONFIG ].sub_context_1	= *(volatile uint32_t*)read_address; read_address += 4;
	Menu_Array[ AUX_1_CONFIG ].sub_context_2	= *(volatile uint32_t*)read_address; read_address += 4;
	Menu_Array[ AUX_1_CONFIG ].sub_context_3	= *(volatile uint32_t*)read_address; read_address += 4;
	for( i = 0; i < DISPLAY_WIDTH; i++, read_address += 1 )
		Menu_Array[ AUX_1_CONFIG ].item[ 0 ][ i ] = *(volatile uint8_t*)read_address;

	Menu_Array[ AUX_2_CONFIG ].context			= *(volatile uint32_t*)read_address; read_address += 4;
	Menu_Array[ AUX_2_CONFIG ].sub_context_1	= *(volatile uint32_t*)read_address; read_address += 4;
	Menu_Array[ AUX_2_CONFIG ].sub_context_2	= *(volatile uint32_t*)read_address; read_address += 4;
	Menu_Array[ AUX_2_CONFIG ].sub_context_3	= *(volatile uint32_t*)read_address; read_address += 4;
	for( i = 0; i < DISPLAY_WIDTH; i++, read_address += 1 )
		Menu_Array[ AUX_2_CONFIG ].item[ 0 ][ i ] = *(volatile uint8_t*)read_address;

	Menu_Array[ TIMER_HISTORY ].context	= *(volatile uint32_t*)read_address; read_address += 4;
	for( i = 0; i < DISPLAY_WIDTH; i++, read_address += 1 )
		Menu_Array[ TIMER_HISTORY ].item[ 0 ][ i ] = *(volatile uint8_t*)read_address;

	Menu_Array[ CADENCE_VOLUME ].context	= *(volatile uint32_t*)read_address; read_address += 4;
	for( i = 0; i < DISPLAY_WIDTH; i++, read_address += 1 )
		Menu_Array[ CADENCE_VOLUME ].item[ 0 ][ i ] = *(volatile uint8_t*)read_address;

	Menu_Array[ AUDIO_IN_VOLUME ].context	= *(volatile uint32_t*)read_address; read_address += 4;
	for( i = 0; i < DISPLAY_WIDTH; i++, read_address += 1 )
		Menu_Array[ AUDIO_IN_VOLUME ].item[ 0 ][ i ] = *(volatile uint8_t*)read_address;

	Menu_Array[ WIRELESS_REMOTE ].context	= *(volatile uint32_t*)read_address; read_address += 4;
	for( i = 0; i < DISPLAY_WIDTH; i++, read_address += 1 )
		Menu_Array[ WIRELESS_REMOTE ].item[ 0 ][ i ] = *(volatile uint8_t*)read_address;

	Menu_Array[ RELEASE_DEVICE ].context	= *(volatile uint32_t*)read_address; read_address += 4;
	for( i = 0; i < DISPLAY_WIDTH; i++, read_address += 1 )
		Menu_Array[ RELEASE_DEVICE ].item[ 0 ][ i ] = *(volatile uint8_t*)read_address;

	Menu_Array[ ANTI_SLAM ].sub_context_1	= *(volatile uint32_t*)read_address; read_address += 4;
	Menu_Array[ ANTI_SLAM ].sub_context_2	= *(volatile uint32_t*)read_address; read_address += 4;
	Menu_Array[ ANTI_SLAM ].sub_context_3	= *(volatile uint32_t*)read_address; read_address += 4;
	Menu_Array[ ANTI_SLAM ].sub_context_4	= *(volatile uint32_t*)read_address; read_address += 4;
	for( i = 0; i < DISPLAY_WIDTH; i++, read_address += 1 )
		Menu_Array[ ANTI_SLAM ].item[ 0 ][ i ] = *(volatile uint8_t*)read_address;

	return 0;
}

uint32_t GetFlashSector( uint32_t Address )
{
	uint32_t sector = 0;

	if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
	{
		sector = FLASH_Sector_0;
	}
	else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
	{
		sector = FLASH_Sector_1;
	}
	else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
	{
		sector = FLASH_Sector_2;
	}
	else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
	{
		sector = FLASH_Sector_3;
	}
	else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
	{
		sector = FLASH_Sector_4;
	}
	else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
	{
		sector = FLASH_Sector_5;
	}
	else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
	{
		sector = FLASH_Sector_6;
	}
	else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
	{
	 	sector = FLASH_Sector_7;
	}
	else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
	{
		sector = FLASH_Sector_8;
	}
	else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
	{
		sector = FLASH_Sector_9;
	}
	else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
	{
		sector = FLASH_Sector_10;
	}
	else
	{
		sector = FLASH_Sector_11;
	}

	return sector;
}

void ItemCopy( const unsigned int menu, const unsigned int source_index, const unsigned int dest_index, const unsigned int no_indicators )
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

void SetMenuText( char * menu, const char * text )
{
	int i;
	for( i = 0; i < DISPLAY_WIDTH; i++ ) menu[ i ] = text[ i ];
}
