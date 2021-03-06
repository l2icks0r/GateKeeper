#ifndef _TEXT_STRINGS
#define _TEXT_STRINGS

// "** RRP BMX GATES **"
static const short splash_text_0[ 22 ] = { -666, 24800, 9262, 17183, 23998, 28943, 32089, 32642, 31071, 27333, 21428, 14122, 5848, -2917, -11318, -19145, -25394, -30053, -32387, -32545, -30286, 0 };
char Splash_Text_0[ 21 ];

// "Epicenter v0.9.5"
static const short splash_text_1[ 19 ] = { 128, 227, 69, -23611, -32651, -21646, 2647, 25470, 32608, 19820, -5153, -26900, -32004, -17559, 7805, 28308, 31525, 15328, 0 };
char Splash_Text_1[ 21 ];

// "-Device Information-"
static const short device_information[ 23 ] = { 6502, 32000, -32009, -32663, -31198, -27446, -21785, -14755, -6784, 1910, 10254, 17946, 24493, 29409, 32006, 32720, 31008, 27142, 21564, 14338, 6347, -2304, 0 };
char Device_Information[ 21 ];

// "                   This device is NOT an RRP BMX Gates product. This device is an unfinished proprietary prototype designed by Richard E. Saenz Jr.                    "
static const short ownership_information[ 170 ] = { 4130, 7075, 28568, 26188, 23407, 20146, 16465, 12555, 8485, 4201, -156, -4511, -8785, -12966, -16790, -20379, -23670, -26477, -28750, -30641, -31924, -32524, -32642, -32328, -31303, -29745, -27577, -25052, -21910, -18668, -14780, -10820, -6550, -2215, 2126, 6406, 10706, 14618, 18526, 21847, 24894, 27616, 29683, 31320, 32324, 32693, 32591, 31991, 30600, 28908, 26499, 23804, 20559, 16960, 13160, 8844, 4721, 319, -4037, -8219, -12486, -16262, -20095, -23187, -26173, -28496, -30341, -31871, -32578, -32652, -32419, -31402, -29840, -27782, -25445, -22279, -19030, -15243, -11390, -7110, -2811, 1616, 5964, 10122, 14091, 17976, 21387, 24673, 27136, 29337, 30996, 32223, 32681, 32760, 32007, 30730, 29147, 26811, 24146, 20918, 17306, 13448, 9346, 5174, 814, -3542, -7770, -11980, -15963, -19477, -22791, -25789, -28190, -30321, -31714, -32398, -32734, -32426, -31545, -30153, -28267, -25650, -22669, -19389, -15645, -11744, -7640, -3229, 1029, 5452, 9695, 13767, 17653, 21054, 24280, 26915, 29221, 30968, 32127, 32651, 32659, 32240, 31042, 29433, 27015, 24482, 21279, 17887, 14055, 9931, 5757, 1352, -2947, -7259, -11506, -15484, -19189, -22489, -25517, -28029, -29980, -31526, -32385, -32732, -32561, -31685, -30374, -28460, -26042, 0 };
char Ownership_Information[ 168 ];

// "Epicenter design by:"
static const short design_by[ 23 ] = { -1, 1, 69, 27588, 29707, 4723, -24761, -31443, -9143, 21618, 32464, 13471, -17861, -32665, -17632, 13742, 32428, 21333, -9465, -31600, -24680, 4885, 0 };
char Design_By[ 21 ];

// "Richard E. Saenz Jr."
static const short designer_information[ 23 ] = { -6969, 42, -6838, 32026, -18706, -17113, 32428, -8912, -25446, 29139, 1951, -30741, 22644, 12737, -32668, 13493, 21970, -31008, 2881, 28673, -25928, -8061, 0 };
char Designer_Information[ 21 ];

// "Unauthorized Device"
static const short unauthorized_device[ 22 ] = { -12, 12, 85, -17603, -29578, -32390, -25121, -10091, 8210, 23978, 32140, 30427, 18997, 1711, -16121, -28825, -32709, -26365, -11525, 6589, 22898, 0 };
char Unauthorized_Device[ 21 ];

// "Illegal Counterfeit"
static const short illegal_counterfeit[ 22 ] = { -128, 128, 73, 23590, -32649, 21655, 2634, -25451, 32633, -19760, -5131, 26945, -32003, 17631, 7730, -28353, 31613, -15240, -10358, 29443, -30635, 0 };
char Illegal_Counterfeit[ 21 ];

// "This device is not"
static const short this_device_is_not[ 21 ] = { -255, 512, -18743, 20758, -22910, 24687, -26279, 27695, -28987, 30152, -31027, 31829, -32289, 32681, -32661, 32720, -32346, 31981, -31151, 30280, 0 };
char This_Device_Is_Not[ 21 ];

// "authorized for use."
static const short authorized_for_use[ 22 ] = { -1234, 5432, 24261, -27780, 30704, -32135, 32657, -32237, 30351, -27755, 23835, -19402, 14176, -8335, 2305, 3602, -9615, 15301, -20436, 24757, -28299, 0 };
char Authorized_For_Use[ 21 ];


void InitTextStrings( void );
void DecryptString( char *pDeCoded, const short *pCoded );

#endif
