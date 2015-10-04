// All encrypted strings are stored in this file and encrypted by the encrypt.py script
//
#include <string.h>
#include "math.h"
#include "TextStrings.h"

void InitTextStrings( void )
{
	DecryptString( Splash_Text_0,	 		splash_text_0 			); 	// ** RRP BMX GATES **
	DecryptString( Splash_Text_1,	 		splash_text_1 			); 	// Epicenter v0.9.4

	DecryptString( Device_Information,		device_information 		);	// -Device Information-
	DecryptString( Ownership_Information, 	ownership_information 	); 	// ...

	DecryptString( Design_By,				design_by 				);	// Epicenter design by:
	DecryptString( Designer_Information, 	designer_information 	); 	// Richard E. Saenz Jr.

	DecryptString( Unauthorized_Device, 	unauthorized_device 	);	// Unauthorized Device
	DecryptString( Illegal_Counterfeit, 	illegal_counterfeit 	);	// Illegal counterfeit

	DecryptString( This_Device_Is_Not, 		this_device_is_not 		);	// "This device is not"
	DecryptString( Authorized_For_Use, 		authorized_for_use 		);	// "authorized for use."
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
