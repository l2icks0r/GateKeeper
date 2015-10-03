#ifndef _TEXT_STRINGS
#define _TEXT_STRINGS

// "** RRP BMX GATES **"
static const short splash_text_0[ 22 ] = { -666, 24800, 9262, 17183, 23998, 28943, 32089, 32642, 31071, 27333, 21428, 14122, 5848, -2917, -11318, -19145, -25394, -30053, -32387, -32545, -30286, 0 };
// "Epicenter v0.9.4"
static const short splash_text_1[ 19 ] = { 128, 227, 69, -23611, -32651, -21646, 2647, 25470, 32608, 19820, -5153, -26900, -32004, -17559, 7805, 28308, 31525, 15329, 0 };


char Splash_Text_0[ 20 ];
char Splash_Text_1[ 20 ];

void InitTextStrings( void );
void DecryptString( char *pDeCoded, const short *pCoded );

#endif
