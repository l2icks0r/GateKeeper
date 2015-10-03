#ifndef _TEXT_STRINGS
#define _TEXT_STRINGS

// "** RRP BMX GATES **"
static const short splash_text_0[ 22 ] = { -666, 24800, 9262, 17183, 23998, 28943, 32089, 32642, 31071, 27333, 21428, 14122, 5848, -2917, -11318, -19145, -25394, -30053, -32387, -32545, -30286, 0 };
char Splash_Text_0[ 20 ];

// "Epicenter v0.9.4"
static const short splash_text_1[ 19 ] = { 128, 227, 69, -23611, -32651, -21646, 2647, 25470, 32608, 19820, -5153, -26900, -32004, -17559, 7805, 28308, 31525, 15329, 0 };
char Splash_Text_1[ 20 ];

// "Copyright 2015"
static const short copyright_text[ 17 ] = { 1230, 1123, 1671, 32374, -9405, -29998, 16589, 25984, -22883, -20701, 27723, 14029, -31074, -6711, 32694, -1123, 0 };
char Copyright_Text[ 20 ];

// "l2icks0r! Designs"
static const short l2icks0r_designs[ 20 ] = { 911, 911, -4008, -6064, -7985, -9878, -11783, -13766, -15612, -17367, -18972, -20650, -22221, -23640, -25074, -26139, -27289, -28493, -29355, 0 };
char l2icks0r_Designs[ 20 ];

// "Unauthorized Device"
static const short unauthorized_device[ 22 ] = { -12, 12, 85, -17603, -29578, -32390, -25121, -10091, 8210, 23978, 32140, 30427, 18997, 1711, -16121, -28825, -32709, -26365, -11525, 6589, 22898, 0 };
char Unauthorized_Device[ 20 ];

// "Illegal counterfeit"
static const short illegal_counterfeit[ 22 ] = { -128, 128, 73, 23590, -32649, 21655, 2634, -25451, 32633, -19760, -5163, 26945, -32003, 17631, 7730, -28353, 31613, -15240, -10358, 29443, -30635, 0 };
char Illegal_Counterfeit[ 20 ];

// "Counterfeit box!"
static const short counterfeit_box[ 19 ] = { -11111, -22222, -21224, 26177, 18237, -28277, -15229, 29897, 11874, -31150, -8423, 32000, 4837, -32602, -1026, 32664, -2478, -32512, 0 };
char Counterfeit_Box[ 20 ];

// "You SUCK!!!"
static const short you_suck[ 14 ] = { -1111, -2222, -7552, -20107, 32649, -20810, -6573, 29097, -29830, 8193, 19573, -32712, 21440, 0 };
char You_Suck[ 20 ];


void InitTextStrings( void );
void DecryptString( char *pDeCoded, const short *pCoded );

#endif
