// All encrypted strings are stored in this file and encrypted by the encrypt.py script
//
#include <string.h>
#include "math.h"
#include "TextStrings.h"


void InitTextStrings( void )
{
	DecryptString( Splash_Text_0, splash_text_0 );
	DecryptString( Splash_Text_1, splash_text_1 );
}

void DecryptString( char *pDeCoded, const short *pCoded )
{
	int i = 0;
	int mask  = 0;
	int angle = *pCoded++;
	int theta = *pCoded++;

	memset( pDeCoded, 0, 20 );

	for( i = 0; i < 20 && *pCoded != 0; i++, pCoded++, pDeCoded++ )
	{
		angle += theta;

		mask = 32767.0f * sin( (float)angle );

		*pDeCoded = *pCoded ^ mask;
	}
}
