// All encrypted strings are stored in this file and encrypted by the encrypt.py script
//
#include <string.h>
#include "math.h"
#include "TextStrings.h"

void InitTextStrings( void )
{
	DecryptString( Splash_Text_0,	 		splash_text_0 			);
	DecryptString( Splash_Text_1,	 		splash_text_1 			);

	DecryptString( Device_Information,		device_information 		);
	DecryptString( Ownership_Information, 	ownership_information 	);

	DecryptString( More_Info,				more_info 				);
	DecryptString( Email_Address,		 	email_address		 	);

	DecryptString( Unauthorized_Device, 	unauthorized_device 	);
	DecryptString( Illegal_Counterfeit, 	illegal_counterfeit 	);

	DecryptString( This_Device_Is_Not, 		this_device_is_not 		);
	DecryptString( Authorized_For_Use, 		authorized_for_use 		);
}

void DecryptString( char *pDeCoded, const short *pCoded )
{
	int mask  = 0;
	int angle = *pCoded++;
	int theta = *pCoded++;

	int coded_length = 0;
	for( ; *(pCoded + 2 + coded_length) != 0; coded_length++ );

	memset( pDeCoded, 0, coded_length + 1 );

	for( ; *pCoded != 0; pCoded++, pDeCoded++ )
	{
		angle += theta;

		mask = 32767.0f * sin( (float)angle );

		*pDeCoded = *pCoded ^ mask;
	}
}
