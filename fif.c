/**********************************************************************
* Written by WAVToCode
* Date:				Fri Aug 16 06:41:27 PM
* FileName:			fif.C
* Interleaved:		Yes
* Signed:			Yes
* No. of channels:	1
* No. of samples:	5056
* Bits/Sample:		16
**********************************************************************/
#include "stm32f4xx.h"

#define FIF_SIZE 5056

static const int16_t FifData[FIF_SIZE] = {
  9, -52, -80, -42, -10, -53, -86, -51, // 0-7
-93, -250, -248,  21, 167,  -5, -134, -29, // 8-15
 43,  -5,  54, 165,  80, -94, -89,  44, // 16-23
 83,  50,  65,  70, -12, -69, -13,  26, // 24-31
 20,  83, 120, -19, -140, -102, -171, -330, // 32-39
-177, 181, 159, -184, -288, -147, -78, -56, // 40-47
 23,   0, -119, -119, -65, -143, -180,   6, // 48-55
 89, -136, -265, -54,  46, -187, -283, -109, // 56-63
-27, -61,  38, 108, -47, -166, -78,  47, // 64-71
 87,  61, -39, -186, -326, -404, -318, -103, // 72-79
  8, -33, -153, -369, -494, -237,  71, -14, // 80-87
-111, 121, 199, -95, -172, -12, -137, -331, // 88-95
-111, 150,  21, -87,  66,  94, -137, -229, // 96-103
-43, 119,  78, -91, -205, -123,  24, -37, // 104-111
-203, -150,  36,  11, -150, -180, -181, -226, // 112-119
-145,  -8, -50, -119, -54, -69, -204, -209, // 120-127
-101, -53,  16, 117, 135,  93, -37, -298, // 128-135
-311,  40, 137, -200, -230, 165, 225,  -9, // 136-143
 15, 131, -67, -313, -314, -194, -79, -77, // 144-151
-170, -46, 247, 220,  -2,  14,  14, -207, // 152-159
-63, 340, 172, -300, -135, 305, 170, -126, // 160-167
-74,   0, -91, -81, -68, -219, -258, -81, // 168-175
 38, -17, -113, -144, -84, -15,  -2,  27, // 176-183
 35, -100, -128,  99, 115, -319, -520, -132, // 184-191
161,  -2, -163, -40,  42, -25,  -1, 109, // 192-199
106,  33,  77, 157, 130, 145, 273, 242, // 200-207
-17, -113,  60, 154,  72,  41,  72,  15, // 208-215
-18,  76, 119, -28, -158, -81,   9, -23, // 216-223
 22, 180, 105, -177, -169, 121, 256, 193, // 224-231
148,  51, -82, -23, 115, 111,  91, 120, // 232-239
 -7, -189, -124, 112, 211, 126,  34,  32, // 240-247
 50,  28,  68, 175,  73, -177, -134, 111, // 248-255
 76, -52, 142, 232, -69, -199,   8, -50, // 256-263
-302, -118, 184,  94,   9, 252, 293,  89, // 264-271
222, 392, -61, -635, -475, -48, -105, -246, // 272-279
 61, 351,  96, -149, 193, 518, 210, -89, // 280-287
181, 119, -536, -384, 620, 632, -304, -127, // 288-295
689, 250, -639, -396,   2, -325, -91, 636, // 296-303
357, -469, -301, 269, 177, 102, 384, 262, // 304-311
-258, -326,  -3,  92,  -5,  72, 309, 299, // 312-319
-68, -197, 208, 434,  18, -202, 230, 378, // 320-327
-22, -67, 329, 217, -247, -61, 409, 173, // 328-335
-384, -200, 356, 298,  -6, 105, 121, -389, // 336-343
-525,  54, 274, -21, 106, 426,  10, -346, // 344-351
240, 649,  20, -480, -39, 379, 235,  21, // 352-359
-49, -72,  42, 189, 165,  58, -98, -257, // 360-367
-79, 371, 483, 278, 208, 167, -29,  -3, // 368-375
203,  86, -143,   5, 187,  -6, -114, 106, // 376-383
257, 265, 409, 430,  60, -123, 219, 392, // 384-391
-94, -466, -90, 341, 222,  15, 198, 361, // 392-399
197, 137, 294, 202, -214, -364,   0, 432, // 400-407
518, 186, -329, -513, -58, 520, 508, -20, // 408-415
-343, -91, 215, -77, -404, 520, 2421, 3152, // 416-423
1911, 458, -143, -539, -781, -198, 563, 468, // 424-431
115, 197, 195, -241, -563, -636, -728, -595, // 432-439
-165, -58, -463, -613, -115, 368, 186, -387, // 440-447
-671, -655, -767, -890, -669, -308, -176, -92, // 448-455
 34, -138, -485, -411,  -7,  40, -220, -187, // 456-463
110, 189,  81, 204, 460, 424, 192, 185, // 464-471
343, 247, -76, -266, -186, 124, 500, 550, // 472-479
181, -74,   0, -42, -412, -649, -494, -262, // 480-487
-190, -143, 119, 512, 546, 108, -162,  69, // 488-495
256,  17, -194, -84, 120, 299, 371, 133, // 496-503
-200, -215, -167, -381, -509, -354, -475, -917, // 504-511
-888, -359, -183, -539, -704, -498, -395, -443, // 512-519
-394, -353, -472, -481, -267, -170, -277, -185, // 520-527
176, 284,  10, -149, -47, -58, -179,  17, // 528-535
381, 400, 200, 268, 511, 520, 390, 407, // 536-543
440, 227, -100, -220, -143, -122, -149, -56, // 544-551
 32, -117, -315, -321, -310, -375, -249,  35, // 552-559
 59, -97, -78, -66, -394, -694, -496, -165, // 560-567
-169, -265, -87,  78, -107, -305, -212, -111, // 568-575
-201, -153, 109, 106, -226, -431, -367, -284, // 576-583
-158,  98, 175, -87, -336, -329, -330, -463, // 584-591
-463, -293, -236, -347, -360, -213, -122, -140, // 592-599
-135, -129, -305, -623, -792, -725, -618, -562, // 600-607
-420, -224, -226, -416, -533, -513, -498, -432, // 608-615
-161, 201, 414, 397, 232,  85, 135, 339, // 616-623
461, 443, 470, 571, 564, 454, 429, 515, // 624-631
551, 480, 407, 409, 475, 542, 533, 430, // 632-639
338, 364, 443, 411, 248, 161, 297, 499, // 640-647
525, 376, 256, 259, 285, 275, 293, 354, // 648-655
348, 210,  50,  22, 112, 163,  79, -44, // 656-663
-55,  17,   2, -129, -251, -349, -522, -670, // 664-671
-589, -378, -293, -353, -416, -496, -650, -819, // 672-679
-944, -987, -899, -734, -625, -557, -484, -519, // 680-687
-730, -918, -918, -847, -857, -912, -906, -830, // 688-695
-751, -736, -823, -1038, -1370, -1716, -1896, -1825, // 696-703
-1603, -1406, -1345, -1435, -1610, -1722, -1677, -1587, // 704-711
-1641, -1809, -1872, -1778, -1661, -1553, -1323, -929, // 712-719
-554, -422, -546, -705, -663, -374,  82, 647, // 720-727
1223, 1659, 1904, 2024, 2059, 2113, 2401, 2934, // 728-735
3407, 3638, 3799, 3974, 4053, 4122, 4341, 4552, // 736-743
4561, 4522, 4508, 4330, 4074, 4090, 4285, 4236, // 744-751
3971, 3750, 3471, 2937, 2378, 2122, 2118, 2159, // 752-759
2136, 1946, 1525, 1045, 756, 633, 483, 258, // 760-767
 54, -110, -274, -421, -568, -771, -980, -1059, // 768-775
-1085, -1362, -1991, -2666, -3084, -3347, -3619, -3753, // 776-783
-3687, -3741, -4171, -4751, -5178, -5502, -5896, -6406, // 784-791
-6975, -7503, -7898, -8213, -8620, -9195, -9891, -10726, // 792-799
-11721, -12541, -12438, -10686, -7290, -3427, -908, -675, // 800-807
-1967, -3332, -3895, -3336, -1391, 1649, 4698, 6862, // 808-815
8101, 8655, 8530, 7934, 7540, 8000, 9402, 11085, // 816-823
12057, 11963, 11525, 11587, 12110, 12396, 12063, 11189, // 824-831
9831, 8071, 6307, 5122, 4944, 5713, 6609, 6510, // 832-839
4990, 2687, 463, -1319, -2533, -3014, -2834, -2457, // 840-847
-2383, -2692, -3109, -3483, -3861, -4130, -3972, -3371, // 848-855
-2793, -2572, -2463, -1889, -618, 875, 1836, 1973, // 856-863
1578, 1128, 999, 1445, 2520, 3957, 5298, 6122, // 864-871
6178, 5475, 4389, 3453, 2878, 2524, 2279, 2055, // 872-879
1474, 306, -1006, -1993, -2875, -4056, -5458, -6985, // 880-887
-8798, -10805, -12647, -14200, -15515, -16559, -17524, -18585, // 888-895
-18946, -17105, -13011, -9096, -7831, -9165, -11075, -11937, // 896-903
-11382, -9326, -5768, -1464, 2222, 4365, 5147, 5471, // 904-911
5982, 6694, 7638, 9356, 12072, 14700, 15789, 15432, // 912-919
15156, 16031, 17429, 18046, 17422, 15868, 13744, 11433, // 920-927
9616, 8906, 9178, 9565, 9108, 7439, 4948, 2280, // 928-935
-145, -2091, -3385, -4104, -4648, -5386, -6324, -7231, // 936-943
-7952, -8459, -8690, -8526, -8102, -7923, -8271, -8619, // 944-951
-8041, -6364, -4356, -2691, -1500, -857, -799, -812, // 952-959
-94, 1482, 3342, 4999, 6432, 7681, 8507, 8683, // 960-967
8381, 8030, 7945, 8139, 8302, 7957, 6944, 5588, // 968-975
4260, 2975, 1625, 230, -1209, -2819, -4763, -7073, // 976-983
-9462, -11508, -13191, -14833, -16574, -18340, -20204, -21955, // 984-991
-22341, -19952, -15225, -11053, -10272, -12610, -15166, -15786, // 992-999
-14406, -11845, -8337, -3907, 552, 3684, 5014, 5419, // 1000-1007
5932, 6752, 7994, 10247, 13490, 16354, 17435, 17152, // 1008-1015
17322, 18770, 20410, 20655, 19201, 16936, 14645, 12605, // 1016-1023
11083, 10429, 10528, 10461, 9219, 6829, 4106, 1503, // 1024-1031
-1081, -3377, -4739, -5305, -5922, -6963, -8019, -8818, // 1032-1039
-9559, -10151, -10188, -9804, -9761, -10355, -10893, -10553, // 1040-1047
-9149, -6979, -4638, -2866, -2031, -1766, -1452, -789, // 1048-1055
261, 1751, 3658, 5741, 7723, 9349, 10222, 10060, // 1056-1063
9312, 8983, 9462, 10099, 10020, 9062, 7698, 6363, // 1064-1071
5173, 4080, 2899, 1320, -787, -3137, -5361, -7428, // 1072-1079
-9508, -11636, -13607, -15229, -16735, -18716, -21311, -23382, // 1080-1087
-22919, -18961, -13380, -9861, -10583, -14009, -16590, -16454, // 1088-1095
-14320, -11206, -7148, -2253, 2176, 4676, 5479, 6074, // 1096-1103
7153, 8316, 9582, 11833, 15080, 17680, 18313, 17776, // 1104-1111
18145, 20021, 21651, 21296, 19207, 16655, 14234, 11945, // 1112-1119
10305, 10000, 10517, 10279, 8333, 5441, 2903, 903, // 1120-1127
-1112, -3160, -4690, -5557, -6282, -7306, -8510, -9514, // 1128-1135
-10095, -10212, -9902, -9474, -9497, -10149, -10762, -10366, // 1136-1143
-8667, -6271, -4067, -2612, -1951, -1719, -1411, -666, // 1144-1151
554, 2071, 3777, 5688, 7600, 8964, 9372, 9029, // 1152-1159
8577, 8551, 8993, 9501, 9546, 8825, 7435, 5839, // 1160-1167
4509, 3514, 2487, 1065, -796, -2899, -5081, -7288, // 1168-1175
-9443, -11424, -13191, -14843, -16559, -18526, -20725, -22414, // 1176-1183
-22059, -18587, -13301, -9706, -10237, -13502, -16059, -15959, // 1184-1191
-13853, -10798, -6710, -1752, 2581, 4936, 5772, 6491, // 1192-1199
7465, 8290, 9378, 11692, 15082, 17843, 18594, 18135, // 1200-1207
18439, 20154, 21725, 21325, 18992, 16136, 13628, 11558, // 1208-1215
10213, 10148, 10865, 10509, 7970, 4324, 1484, -121, // 1216-1223
-1574, -3487, -5281, -6415, -7226, -8320, -9583, -10400, // 1224-1231
-10572, -10314, -9776, -9213, -9342, -10548, -11756, -11236, // 1232-1239
-8476, -4910, -2448, -1645, -1687, -1716, -1343, -394, // 1240-1247
1202, 3202, 5175, 6902, 8325, 9272, 9476, 8999, // 1248-1255
8518, 8772, 9653, 10097, 9210, 7330, 5578, 4522, // 1256-1263
3843, 2980, 1654, -241, -2751, -5535, -7897, -9466, // 1264-1271
-10691, -12299, -14443, -16783, -19110, -21288, -22584, -21590, // 1272-1279
-17630, -12482, -9459, -10391, -13637, -15879, -15529, -13305, // 1280-1287
-9990, -5522, -314, 3846, 5521, 5621, 6119, 7540, // 1288-1295
9034, 10535, 13090, 16525, 18805, 18687, 17659, 18217, // 1296-1303
20505, 22118, 21354, 18849, 16073, 13566, 11320, 9839, // 1304-1311
9568, 9787, 9054, 6752, 3712, 1046, -1054, -2937, // 1312-1319
-4622, -5762, -6296, -6702, -7529, -8861, -10304, -11269, // 1320-1327
-11278, -10220, -8707, -7915, -8545, -9801, -9862, -7750, // 1328-1335
-4458, -1842, -795, -684, -379, 533, 1765, 3067, // 1336-1343
4438, 5899, 7375, 8649, 9350, 9240, 8618, 8223, // 1344-1351
8571, 9338, 9558, 8582, 6693, 4663, 2972, 1605, // 1352-1359
378, -926, -2541, -4557, -6813, -9101, -11433, -13860, // 1360-1367
-16134, -17972, -19635, -21538, -22855, -21562, -17080, -11922, // 1368-1375
-9780, -11677, -14922, -16346, -15462, -13391, -10337, -5562, // 1376-1383
203, 4376, 5526, 5034, 5170, 6520, 8312, 10560, // 1384-1391
13911, 17524, 19224, 18370, 17293, 18428, 20960, 22140, // 1392-1399
20787, 18320, 16272, 14332, 11854, 9744, 9248, 9747, // 1400-1407
9150, 6536, 2985, -286, -2999, -5106, -6166, -6133, // 1408-1415
-6003, -6689, -7943, -8976, -9709, -10498, -10957, -10394, // 1416-1423
-9210, -8685, -9197, -9435, -8051, -5431, -3002, -1505, // 1424-1431
-586,  88, 380, 553, 1413, 3260, 5373, 6963, // 1432-1439
8140, 9327, 10225, 10179, 9315, 8623, 8625, 8707, // 1440-1447
8203, 7265, 6369, 5397, 3859, 1877, 108, -1378, // 1448-1455
-3427, -6554, -9801, -11754, -12453, -13222, -15019, -17609, // 1456-1463
-20345, -22697, -23610, -21674, -16912, -11966, -10133, -11938, // 1464-1471
-14760, -16037, -15296, -13247, -9927, -5045, 337, 4242, // 1472-1479
5822, 6188, 6746, 7670, 8754, 10573, 13592, 16806, // 1480-1487
18391, 18006, 17515, 18676, 21057, 22568, 21785, 19081, // 1488-1495
15649, 12454, 10212, 9308, 9345, 9153, 7743, 5228, // 1496-1503
2454, -17, -2272, -4342, -5778, -6353, -6591, -7210, // 1504-1511
-8302, -9349, -9952, -10139, -10010, -9566, -9040, -8958, // 1512-1519
-9428, -9650, -8695, -6459, -3715, -1308, 385, 1257, // 1520-1527
1294, 899, 1029, 2461, 4896, 7242, 8799, 9598, // 1528-1535
9838, 9483, 8640, 7934, 7968, 8583, 8982, 8617, // 1536-1543
7613, 6224, 4427, 2267,  95, -1780, -3453, -5209, // 1544-1551
-7069, -8842, -10540, -12465, -14719, -17018, -19226, -21398, // 1552-1559
-22939, -22397, -18975, -14185, -11094, -11348, -13782, -15767, // 1560-1567
-15744, -13816, -10411, -5775, -710, 3375, 5569, 6208, // 1568-1575
6272, 6463, 7296, 9450, 13143, 17046, 18978, 18448, // 1576-1583
17394, 17989, 20090, 21581, 21048, 19090, 16833, 14480, // 1584-1591
11798, 9418, 8417, 8611, 8480, 6787, 3753, 458, // 1592-1599
-2426, -4790, -6376, -6836, -6517, -6532, -7593, -9188, // 1600-1607
-10370, -10768, -10624, -10132, -9256, -8254, -7754, -7935, // 1608-1615
-7939, -6572, -3724, -740, 894, 965, 590, 946, // 1616-1623
2092, 3385, 4709, 6506, 8727, 10344, 10318, 8877, // 1624-1631
7366, 6837, 7260, 7984, 8540, 8567, 7519, 5247, // 1632-1639
2521, 431, -864, -2323, -4673, -7420, -9617, -11158, // 1640-1647
-12770, -14824, -17060, -19413, -21939, -23797, -23231, -19460, // 1648-1655
-14391, -11270, -11632, -14009, -15747, -15537, -13614, -10394, // 1656-1663
-6134, -1508, 2382, 4816, 5940, 6326, 6518, 7273, // 1664-1671
9566, 13523, 17560, 19641, 19465, 18645, 18885, 20202, // 1672-1679
21225, 20882, 19253, 16971, 14576, 12483, 11046, 10224, // 1680-1687
9517, 8376, 6525, 3929, 720, -2632, -5350, -6845, // 1688-1695
-7250, -7242, -7342, -7743, -8661, -10121, -11428, -11521, // 1696-1703
-10252, -8836, -8453, -8839, -8661, -7126, -4790, -2603, // 1704-1711
-914, 220, 591, 232, -39, 869, 3199, 6110, // 1712-1719
8531, 9963, 10393, 9867, 8712, 7768, 7776, 8583, // 1720-1727
9315, 9305, 8516, 7172, 5431, 3439, 1410, -603, // 1728-1735
-2842, -5395, -7886, -9920, -11623, -13365, -15198, -17091, // 1736-1743
-19269, -21650, -22999, -21529, -17137, -12509, -10783, -12472, // 1744-1751
-15108, -16142, -15199, -13109, -10082, -5883, -1094, 2759, // 1752-1759
4713, 5347, 5782, 6616, 8101, 10649, 14220, 17506, // 1760-1767
18927, 18388, 17623, 18314, 20112, 21108, 20139, 17726, // 1768-1775
15137, 13078, 11534, 10404, 9736, 9378, 8703, 6932, // 1776-1783
3802, -108, -3730, -6138, -7031, -6815, -6255, -6111, // 1784-1791
-6916, -8644, -10492, -11346, -10771, -9425, -8350, -7938, // 1792-1799
-7807, -7385, -6435, -4884, -2682, -277, 1270, 1195, // 1800-1807
 65, -517, 630, 3315, 6416, 8832, 10001, 9816, // 1808-1815
8626, 7289, 6670, 6944, 7596, 8087, 8164, 7602, // 1816-1823
6147, 3895, 1523, -278, -1675, -3526, -6214, -9127, // 1824-1831
-11406, -12979, -14547, -16678, -19121, -20867, -20812, -18624, // 1832-1839
-15263, -12654, -12231, -13696, -15205, -15206, -13752, -11701, // 1840-1847
-9233, -5877, -1640, 2359, 4784, 5451, 5397, 5836, // 1848-1855
7443, 10251, 13625, 16340, 17440, 17194, 16982, 17906, // 1856-1863
19507, 20232, 19224, 17053, 14789, 12879, 11186, 9769, // 1864-1871
8964, 8662, 8004, 6102, 3058, -91, -2520, -4231, // 1872-1879
-5528, -6403, -6668, -6583, -6913, -8061, -9501, -10268, // 1880-1887
-10048, -9447, -9077, -8751, -7822, -6044, -3941, -2193, // 1888-1895
-928,  70, 777, 1004, 989, 1495, 3070, 5371, // 1896-1903
7459, 8632, 8903, 8647, 8092, 7433, 7080, 7404, // 1904-1911
8232, 8766, 8150, 6273, 3918, 2033, 809, -411, // 1912-1919
-2290, -4753, -7256, -9518, -11779, -14251, -16673, -18714, // 1920-1927
-20321, -21171, -20333, -17241, -13112, -10547, -11031, -13415, // 1928-1935
-15058, -14529, -12566, -10275, -7527, -3799, 370, 3468, // 1936-1943
4634, 4636, 4886, 6050, 8013, 10568, 13410, 15636, // 1944-1951
16408, 16206, 16519, 18033, 19561, 19453, 17677, 15516, // 1952-1959
13725, 11897, 9806, 8324, 8149, 8454, 7689, 5296, // 1960-1967
2172, -543, -2465, -3636, -4115, -4273, -4913, -6439, // 1968-1975
-8156, -9039, -8935, -8615, -8707, -8933, -8678, -7924, // 1976-1983
-7089, -6199, -4742, -2528, -344, 715, 385, -511, // 1984-1991
-789, 186, 2270, 4808, 6969, 8086, 8089, 7490, // 1992-1999
6825, 6338, 6228, 6716, 7598, 8035, 7232, 5326, // 2000-2007
3164, 1316, -335, -2046, -3754, -5510, -7667, -10235, // 2008-2015
-12588, -14283, -15838, -17873, -19606, -19203, -15908, -11532, // 2016-2023
-9046, -9655, -11890, -13530, -13508, -12141, -9982, -7130, // 2024-2031
-3736, -498, 1807, 3050, 3656, 4034, 4611, 6068, // 2032-2039
8890, 12473, 15148, 15707, 14904, 14673, 16002, 17889, // 2040-2047
18566, 17330, 14930, 12549, 10956, 10266, 10116, 9892, // 2048-2055
8992, 7213, 4863, 2446, 340, -1186, -1974, -2243, // 2056-2063
-2599, -3560, -5084, -6680, -7949, -8769, -8945, -8182, // 2064-2071
-6743, -5730, -5974, -6727, -6414, -4514, -2261, -1081, // 2072-2079
-883, -611,   0, 397, 536, 1341, 3314, 5515, // 2080-2087
6719, 6763, 6328, 5769, 4965, 4271, 4505, 5691, // 2088-2095
6522, 5630, 3135, 361, -1752, -3280, -4699, -6172, // 2096-2103
-7589, -9094, -11133, -13893, -16880, -18990, -18989, -16286, // 2104-2111
-11807, -7846, -6578, -8327, -11426, -13804, -14348, -12792, // 2112-2119
-9180, -4191, 412, 2768, 2781, 2073, 2073, 2895, // 2120-2127
4263, 6515, 9679, 12428, 13308, 12756, 12857, 14659, // 2128-2135
16815, 17545, 16584, 14780, 12682, 10449, 8737, 8392, // 2136-2143
9201, 9781, 8935, 6748, 4087, 1601, -362, -1519, // 2144-2151
-1823, -1730, -1981, -3076, -4922, -6840, -7904, -7566, // 2152-2159
-6148, -4708, -4154, -4582, -5308, -5576, -5057, -3835, // 2160-2167
-2245, -800,  31, 166,  38, 306, 1271, 2680, // 2168-2175
4098, 5357, 6365, 6679, 5927, 4627, 4030, 4775, // 2176-2183
5974, 6137, 4839, 2784, 642, -1511, -3544, -4966, // 2184-2191
-5809, -6962, -9158, -12064, -14818, -17033, -18512, -18417, // 2192-2199
-15818, -11310, -7401, -6522, -8809, -12284, -14623, -14579, // 2200-2207
-12202, -8262, -3792, 113, 2544, 3226, 2719, 2074, // 2208-2215
2242, 3641, 6111, 9044, 11534, 12712, 12509, 12075, // 2216-2223
12790, 14731, 16353, 16065, 13916, 11395, 9795, 9272, // 2224-2231
9271, 9400, 9378, 8573, 6480, 3543, 1025, -216, // 2232-2239
-540, -853, -1509, -2414, -3714, -5688, -7720, -8293, // 2240-2247
-6744, -4263, -2818, -3136, -4270, -5014, -5011, -4389, // 2248-2255
-3262, -1899, -921, -671, -636, -38, 1225, 2521, // 2256-2263
3381, 4045, 4917, 5822, 6111, 5506, 4654, 4573, // 2264-2271
5466, 6367, 6019, 4097, 1443, -898, -2545, -3620, // 2272-2279
-4405, -5383, -7163, -9905, -12990, -15592, -17219, -17595, // 2280-2287
-16295, -13154, -9274, -6831, -7387, -10390, -13563, -14776, // 2288-2295
-13394, -10108, -5993, -1940, 1317, 3092, 3208, 2479, // 2296-2303
2154, 2956, 4768, 7184, 9807, 11907, 12658, 12188, // 2304-2311
11960, 13185, 15001, 15349, 13618, 11186, 9560, 8780, // 2312-2319
8249, 8105, 8625, 9066, 8130, 5627, 2803, 817, // 2320-2327
-274, -885, -1236, -1533, -2313, -3955, -5956, -7150, // 2328-2335
-6716, -5004, -3307, -2756, -3389, -4320, -4671, -4313, // 2336-2343
-3616, -2733, -1633, -584, -20,  54,  79, 348, // 2344-2351
890, 1874, 3488, 5274, 6128, 5442, 3988, 3093, // 2352-2359
3308, 4069, 4484, 3954, 2235, -363, -2877, -4370, // 2360-2367
-4927, -5568, -7039, -9126, -11339, -13702, -16368, -18439, // 2368-2375
-18027, -14307, -8925, -5053, -4804, -7646, -11312, -13811, // 2376-2383
-14174, -12109, -7954, -3057, 741, 2646, 3347, 3792, // 2384-2391
4037, 3822, 3865, 5407, 8390, 11194, 12419, 12456, // 2392-2399
12708, 13731, 14858, 15200, 14436, 12760, 10583, 8606, // 2400-2407
7699, 8225, 9444, 9928, 8741, 6218, 3586, 1903, // 2408-2415
1225, 689, -499, -2245, -3757, -4430, -4421, -4397, // 2416-2423
-4637, -4659, -3992, -3011, -2685, -3515, -4881, -5578, // 2424-2431
-4894, -3037, -832, 769, 1174, 557, -188, -276, // 2432-2439
336, 1158, 1941, 2878, 4052, 5124, 5620, 5279, // 2440-2447
4176, 2721, 1465, 549, -533, -2251, -4101, -5024, // 2448-2455
-4998, -5320, -6968, -9535, -12140, -14682, -17114, -18129, // 2456-2463
-16129, -11414, -6617, -4372, -5129, -7601, -10364, -12256, // 2464-2471
-12216, -9765, -5742, -1825, 909, 2532, 3606, 4274, // 2472-2479
4404, 4396, 5103, 6881, 9015, 10432, 10962, 11506, // 2480-2487
12753, 14289, 15111, 14782, 13560, 11675, 9348, 7345, // 2488-2495
6615, 7175, 7984, 8208, 7962, 7438, 6157, 3643, // 2496-2503
482, -2114, -3611, -4210, -4103, -3385, -2570, -2346, // 2504-2511
-2652, -2817, -2646, -2819, -3878, -5228, -5682, -4723, // 2512-2519
-2787, -750, 678, 1244, 1148, 741, 218, -290, // 2520-2527
-457,  80, 1255, 2667, 4043, 5208, 5654, 4746, // 2528-2535
2710, 708, -502, -1405, -2751, -4170, -4760, -4753, // 2536-2543
-5328, -7034, -9237, -11402, -13919, -16801, -18510, -17152, // 2544-2551
-12864, -8170, -5560, -5426, -6747, -8634, -10612, -11812, // 2552-2559
-10919, -7546, -2997, 887, 3324, 4645, 5123, 4774, // 2560-2567
4239, 4706, 6584, 8759, 9892, 10189, 10979, 12791, // 2568-2575
14682, 15509, 15155, 14078, 12413, 10197, 8004, 6785, // 2576-2583
6949, 7915, 8735, 8805, 7922, 6084, 3511, 762, // 2584-2591
-1500, -2881, -3383, -3279, -2893, -2506, -2362, -2527, // 2592-2599
-2793, -2945, -3175, -3887, -4930, -5434, -4646, -2814, // 2600-2607
-977, 154, 731, 1081, 1159, 849, 443, 420, // 2608-2615
925, 1788, 2859, 4022, 4949, 5090, 4139, 2480, // 2616-2623
908, -126, -926, -2007, -3265, -4111, -4479, -5165, // 2624-2631
-6872, -9196, -11316, -13363, -15777, -17615, -16792, -12753, // 2632-2639
-7890, -5241, -5444, -7064, -8760, -10199, -10885, -9761, // 2640-2647
-6738, -3137, -184, 2176, 4263, 5445, 5122, 4089, // 2648-2655
3983, 5415, 7360, 8675, 9479, 10597, 12342, 14126, // 2656-2663
15068, 14708, 13222, 11144, 9029, 7378, 6673, 7097, // 2664-2671
8152, 8948, 8852, 7767, 5802, 3080, 135, -2055, // 2672-2679
-2854, -2607, -2095, -1641, -1285, -1349, -2117, -3261, // 2680-2687
-4177, -4747, -5216, -5439, -4924, -3597, -2073, -920, // 2688-2695
-92, 527, 650,  65, -753, -1036, -480, 756, // 2696-2703
2394, 3987, 4911, 4798, 3887, 2696, 1466, 155, // 2704-2711
-1148, -2107, -2558, -2756, -3069, -3824, -5283, -7344, // 2712-2719
-9431, -11226, -13092, -15206, -16673, -16117, -13236, -9410, // 2720-2727
-6526, -5521, -5893, -6486, -6524, -6018, -5569, -5655, // 2728-2735
-5814, -4751, -1710, 2470, 6013, 7688, 7649, 7004, // 2736-2743
6541, 6189, 5797, 5781, 6772, 9009, 12064, 14795, // 2744-2751
15742, 14218, 11041, 8065, 6706, 6927, 7637, 7925, // 2752-2759
7628, 7036, 6301, 5390, 4387, 3364, 2137, 558, // 2760-2767
-994, -1938, -2280, -2447, -2566, -2398, -1900, -1446, // 2768-2775
-1480, -2142, -3150, -3937, -3958, -3017, -1513, -264, // 2776-2783
168, -25, -221, -62, 347, 721, 872, 902, // 2784-2791
1156, 1827, 2639, 3036, 2693, 1798, 905, 341, // 2792-2799
-112, -860, -2022, -3382, -4522, -5126, -5450, -6252, // 2800-2807
-7877, -9918, -11850, -13545, -14753, -14753, -13249, -10963, // 2808-2815
-8939, -7569, -6795, -6721, -7110, -6884, -5233, -3114, // 2816-2823
-2410, -3286, -3787, -2350, 649, 3808, 6205, 7516, // 2824-2831
7712, 7333, 7346, 8110, 8830, 8686, 8228, 8686, // 2832-2839
10280, 11795, 11979, 10809, 9291, 8422, 8393, 8353, // 2840-2847
7260, 5161, 3319, 2845, 3701, 4839, 5130, 4124, // 2848-2855
2138, -75, -1683, -2278, -2339, -2658, -3112, -2838, // 2856-2863
-1715, -664, -432, -794, -1207, -1477, -1611, -1607, // 2864-2871
-1500, -1369, -1173, -650, 355, 1487, 2064, 1838, // 2872-2879
1282, 1037, 1319, 1878, 2223, 2022, 1338, 560, // 2880-2887
 65, -158, -618, -1742, -3263, -4559, -5376, -5819, // 2888-2895
-6159, -6807, -7995, -9321, -10270, -10995, -11968, -12926, // 2896-2903
-12872, -11420, -9429, -7890, -7018, -6654, -6800, -7113, // 2904-2911
-6725, -5279, -3409, -1996, -1228, -540, 627, 1993, // 2912-2919
2734, 2770, 3109, 4326, 5698, 6531, 7058, 7688, // 2920-2927
8193, 8267, 8217, 8499, 9092, 9541, 9436, 8822, // 2928-2935
8246, 8095, 7992, 7310, 6247, 5582, 5402, 5134, // 2936-2943
4590, 4021, 3486, 2814, 1905, 766, -326, -869, // 2944-2951
-838, -879, -1279, -1514, -1274, -969, -1035, -1271, // 2952-2959
-1216, -880, -727, -980, -1225, -1033, -656, -487, // 2960-2967
-296, 331, 1231, 1774, 1639, 1193, 1053, 1388, // 2968-2975
1755, 1676, 1168, 527, -24, -372, -604, -1020, // 2976-2983
-1790, -2769, -3783, -4732, -5462, -5978, -6668, -7741, // 2984-2991
-8695, -9099, -9412, -10301, -11342, -11425, -10376, -9011, // 2992-2999
-7902, -6974, -6268, -6141, -6337, -5890, -4426, -2914, // 3000-3007
-2394, -2373, -1593,  30, 1579, 2613, 3510, 4406, // 3008-3015
4981, 5286, 5714, 6302, 6698, 6865, 7217, 7968, // 3016-3023
8858, 9453, 9435, 8741, 7769, 7220, 7316, 7452, // 3024-3031
6963, 6042, 5403, 5263, 5233, 4913, 4211, 3165, // 3032-3039
1800, 359, -659, -978, -889, -765, -506, -39, // 3040-3047
214, -25, -439, -741, -1063, -1277, -957, -188, // 3048-3055
407, 500, 358, 379, 657, 965, 1104, 1195, // 3056-3063
1331, 1217, 802, 602, 859, 1057, 747, 159, // 3064-3071
-409, -1004, -1718, -2516, -3334, -4077, -4594, -4844, // 3072-3079
-5027, -5421, -6013, -6497, -6833, -7427, -8548, -9875, // 3080-3087
-10762, -10683, -9522, -7814, -6453, -5899, -5893, -6074, // 3088-3095
-6328, -6426, -6027, -5108, -3924, -2543, -959, 459, // 3096-3103
1258, 1471, 1577, 1964, 2669, 3447, 3984, 4280, // 3104-3111
4747, 5654, 6674, 7310, 7511, 7576, 7637, 7480, // 3112-3119
7002, 6504, 6235, 6101, 6021, 6134, 6383, 6401, // 3120-3127
5968, 5099, 3992, 3074, 2638, 2418, 2113, 1842, // 3128-3135
1686, 1455, 1215, 1221, 1309, 1203, 995, 856, // 3136-3143
757, 745, 861, 911, 811, 803, 1007, 1197, // 3144-3151
1199, 1039, 814, 661, 645, 624, 495, 355, // 3152-3159
212, -108, -589, -958, -1154, -1359, -1590, -1679, // 3160-3167
-1658, -1813, -2289, -2898, -3359, -3603, -3811, -4138, // 3168-3175
-4494, -4705, -4781, -4929, -5262, -5607, -5839, -6225, // 3176-3183
-6987, -7742, -7851, -7242, -6340, -5423, -4616, -4122, // 3184-3191
-4009, -4013, -3841, -3522, -3305, -3260, -2999, -2036, // 3192-3199
-499, 890, 1632, 1723, 1412, 1043, 1029, 1456, // 3200-3207
1969, 2356, 2870, 3705, 4571, 5076, 5054, 4500, // 3208-3215
3751, 3477, 3893, 4422, 4654, 4806, 5099, 5424, // 3216-3223
5716, 5896, 5616, 4697, 3567, 2948, 3146, 3703, // 3224-3231
3915, 3716, 3562, 3535, 3236, 2637, 2074, 1605, // 3232-3239
1213, 1114, 1395, 1803, 2034, 1838, 1246, 846, // 3240-3247
1055, 1289, 905, 285, -55, -263, -441, -273, // 3248-3255
101, 189,  36, -45, -208, -626, -1005, -1304, // 3256-3263
-1800, -2212, -1914, -1242, -1200, -1813, -2384, -2783, // 3264-3271
-3263, -3535, -3366, -3273, -3526, -3699, -3544, -3337, // 3272-3279
-3223, -3256, -3595, -4039, -4166, -4139, -4374, -4552, // 3280-3287
-4182, -3542, -3215, -3155, -2955, -2694, -2699, -2834, // 3288-3295
-2649, -2100, -1555, -1210, -1081, -1136, -1098, -681, // 3296-3303
-158,  84, 262, 597, 797, 703, 789, 1386, // 3304-3311
2068, 2264, 2000, 1898, 2348, 2956, 3166, 3013, // 3312-3319
2883, 3026, 3418, 3762, 3788, 3695, 3765, 3784, // 3320-3327
3516, 3310, 3402, 3467, 3301, 3060, 2823, 2591, // 3328-3335
2443, 2305, 2122, 2104, 2258, 2170, 1759, 1413, // 3336-3343
1261, 1174, 1197, 1323, 1297, 1122, 1042, 1025, // 3344-3351
873, 723, 799, 907, 719, 229, -263, -532, // 3352-3359
-607, -617, -623, -708, -927, -1106, -1064, -1079, // 3360-3367
-1485, -1972, -1982, -1699, -1771, -2156, -2353, -2401, // 3368-3375
-2677, -2990, -2845, -2433, -2381, -2763, -3063, -2954, // 3376-3383
-2669, -2510, -2450, -2444, -2633, -2948, -3002, -2802, // 3384-3391
-2745, -2737, -2406, -1986, -1903, -1851, -1510, -1256, // 3392-3399
-1344, -1430, -1224, -826, -431, -266, -395, -481, // 3400-3407
-242, 121, 346, 438, 510, 657, 807, 792, // 3408-3415
792, 1109, 1487, 1528, 1500, 1781, 2058, 2044, // 3416-3423
1957, 1979, 2097, 2417, 2853, 3015, 2873, 2763, // 3424-3431
2732, 2583, 2452, 2540, 2711, 2841, 2957, 2870, // 3432-3439
2446, 2141, 2218, 2223, 1967, 2021, 2352, 2124, // 3440-3447
1414, 1067, 1162, 1119, 937, 883, 826, 643, // 3448-3455
370,  16, -312, -492, -632, -737, -721, -823, // 3456-3463
-1184, -1417, -1441, -1577, -1692, -1500, -1380, -1569, // 3464-3471
-1599, -1446, -1588, -1882, -1800, -1538, -1536, -1668, // 3472-3479
-1676, -1584, -1502, -1488, -1550, -1560, -1438, -1313, // 3480-3487
-1298, -1295, -1166, -994, -1006, -1227, -1367, -1380, // 3488-3495
-1630, -1995, -1811, -1073, -615, -624, -430, -69, // 3496-3503
-176, -628, -748, -620, -688, -673, -331, -148, // 3504-3511
-425, -688, -693, -701, -625, -328, -294, -733, // 3512-3519
-820, -207, 232, 102, 181, 740, 947, 638, // 3520-3527
524, 718, 828, 1042, 1547, 1742, 1464, 1380, // 3528-3535
1615, 1542, 1257, 1345, 1535, 1389, 1229, 1328, // 3536-3543
1291, 997, 808, 815, 864, 917, 933, 886, // 3544-3551
897, 974, 989, 871, 548,  62, -210, -142, // 3552-3559
-83, -177, -238, -323, -372, -74, 245, -34, // 3560-3567
-504, -476, -383, -605, -706, -605, -667, -587, // 3568-3575
-220, -299, -750, -580, -72, -403, -991, -693, // 3576-3583
-204, -458, -671, -303, -344, -824, -721, -328, // 3584-3591
-552, -661,   8, 307, -153, -338, -171, -392, // 3592-3599
-474, -91, -113, -408, -95, 101, -511, -622, // 3600-3607
-19, -210, -990, -747, -121, -386, -558,  -1, // 3608-3615
113, -421, -346, 119, -138, -848, -1131, -927, // 3616-3623
-503, -134, -243, -563, -350,  93, -243, -980, // 3624-3631
-852, -58, 204, -65, 161, 762, 697,  72, // 3632-3639
-148, 140, 219,  22,  34, 200, 191,  53, // 3640-3647
-39, -99,  36, 471, 731, 398, -45, -31, // 3648-3655
 97, -37, -206, -269, -348, -173, 399, 750, // 3656-3663
536, 224,  85, -123, -300, -200, -29,  66, // 3664-3671
278, 530, 559, 506, 532, 467, 284, 233, // 3672-3679
272, 184, 123, 290, 542, 713, 817, 760, // 3680-3687
606, 665, 751, 297, -253,  61, 750, 734, // 3688-3695
281, 131, -81, -464, -250, 377, 467, 160, // 3696-3703
131, 110, -24, 174, 443, 135, -314, -241, // 3704-3711
-43, -104, -143, -66, -154, -246, -22, 239, // 3712-3719
 86, -355, -600, -523, -357, -251, -206, -168, // 3720-3727
-95, -133, -391, -498, -193, -25, -416, -698, // 3728-3735
-468, -369, -574, -495, -204, -252, -358, -127, // 3736-3743
113,  57, -178, -516, -778, -663, -317, -38, // 3744-3751
218, 364,  69, -340, -199, 378, 572,  36, // 3752-3759
-579, -497,  95, 403, 245, 164, 263, 117, // 3760-3767
-233, -275, 169, 491,  37, -532, -25, 986, // 3768-3775
831, -60, 142, 818, 350, -341, 180, 761, // 3776-3783
125, -430,  75, 678, 730, 717, 586, -22, // 3784-3791
-419, -63, 385, 357, 355, 583, 526, 400, // 3792-3799
734, 928, 301, -245, -47,  19, -355, -271, // 3800-3807
257, 338, 226, 467, 677, 545, 280, -144, // 3808-3815
-449, -112, 233, -214, -473, 385, 945, 208, // 3816-3823
-422,  73, 457, -135, -558, -219, -111, -610, // 3824-3831
-691, -71, 150, -305, -619, -510, -381, -380, // 3832-3839
-544, -778, -555,  -1, -93, -670, -499,  79, // 3840-3847
 -1, -268, -38, -51, -504, -443, -134, -346, // 3848-3855
-440, 164, 470,  92, -41, 206, 220,  65, // 3856-3863
 30, -14,  51, 201,   0, -240, 241, 784, // 3864-3871
429, -72, 139, 320, -120, -310,  95, 295, // 3872-3879
 53,  30, 422, 647, 407,  85,  57, 176, // 3880-3887
150,  46, -13,   4, 253, 568, 399, -54, // 3888-3895
 -4, 279, 154, 103, 589, 681,  -4, -331, // 3896-3903
103, 410, 235, 139, 294, 381, 216, -71, // 3904-3911
-99, 189, 222, -137, -225, 178, 495, 445, // 3912-3919
232, -81, -293, -97, 221, 133, -173, -283, // 3920-3927
-281, -223,  25, 125, -336, -733, -361, 187, // 3928-3935
 35, -377, -293, -83, -118, -47,  92, -214, // 3936-3943
-683, -656, -354, -208, -139, -35,  54, 176, // 3944-3951
212,  50, -105, -231, -490, -471, -24,  39, // 3952-3959
-413, -301, 466, 708, 382, 230, 157, -56, // 3960-3967
-30, 100, -115, -221, 206, 652, 627, 330, // 3968-3975
 59,  12, 197, 295, 246, 326, 322,  21, // 3976-3983
-55, 311, 415, -18, -297, -99, 171, 189, // 3984-3991
120, 250, 336,   1, -230, 198, 436, -250, // 3992-3999
-621,   9, 301, -173, -161, 333, 134, -359, // 4000-4007
-289,  -6,  63,  68, -119, -349, -94, 276, // 4008-4015
111, -152, -69, -79, -164,  72, 380, 449, // 4016-4023
456, 300,  -4,   8, 144, -144, -242, 351, // 4024-4031
552,  19, -24, 512, 216, -694, -699,  -2, // 4032-4039
182, -127, -150, 107, 244, 219, 189, 232, // 4040-4047
171, -190, -453, -213,  -3, -351, -479, 223, // 4048-4055
589, -111, -626, -29, 506,   2, -633, -681, // 4056-4063
-470, -67, 607, 766, 131, -73, 492, 490, // 4064-4071
-406, -616, 275, 763, 306, -163, -80, 190, // 4072-4079
270,  53, -168,  53, 424, 224, -190, -52, // 4080-4087
204,  45,  21, 369, 223, -461, -610, -48, // 4088-4095
288,  67, -36, 233, 258, -179, -285, 264, // 4096-4103
305, -565, -786,  88, 449, -89, -53, 345, // 4104-4111
-59, -355, 287, 427, -465, -564, 263, 399, // 4112-4119
-85, -87,  -8, -289, -122, 421, 344, -275, // 4120-4127
-448, -24, 330, 233, -33,  51, 326, 154, // 4128-4135
-255, -254,  18, 110,  32, -179, -373, -123, // 4136-4143
204, -76, -287, 370, 694, -111, -666, -270, // 4144-4151
-175, -586, -209, 588, 519, -32, -72, -156, // 4152-4159
-825, -1016, -320,  70, -351, -501, -133, -212, // 4160-4167
-514, -235,  46, -327, -386, 178, 196, -252, // 4168-4175
-63, 183, -448, -663, 313, 898, 212, -354, // 4176-4183
-31, 232,  81,  79, 284, 156, -271, -220, // 4184-4191
395, 504, -286, -701, -75, 368, 155, 163, // 4192-4199
303, -165, -534, -54, 237, -189, -425, -172, // 4200-4207
-63,  66, 454, 357, -365, -673, -188, 257, // 4208-4215
297, 276, 186, -153, -343,   0, 338, 159, // 4216-4223
-30, 208, 271, -116, -107, 388, 428, -38, // 4224-4231
-141,  63, -85, -308, -74, 349, 316, -219, // 4232-4239
-479,  40, 535, 229, -114, 107, -178, -1079, // 4240-4247
-1057, -151,  85, -155, 184, 325, -228, -395, // 4248-4255
 38,  -6, -462, -509, -313, -159,  70, 253, // 4256-4263
267, 243,  32, -269, -192, -26, -412, -775, // 4264-4271
-435, 110, 385, 438,  31, -676, -743, -224, // 4272-4279
 29, 113, 380,  91, -755, -820,  87, 595, // 4280-4287
249, -101, -198, -249, -69, 260, 114, -392, // 4288-4295
-361, 139, 229, -89, -29, 239, -14, -404, // 4296-4303
-266,  50,  92, 240, 490, 204, -376, -375, // 4304-4311
192, 568, 519, 236, -119, -239,  34, 259, // 4312-4319
 60, -105, 235, 670, 466, -204, -464, -153, // 4320-4327
 44, -147, -214, 107, 344, 238, 156, 217, // 4328-4335
 -8, -488, -402, 310, 564,  40, -337, -215, // 4336-4343
-143,  27, 522, 571, -17, -191, 135, -14, // 4344-4351
-286, -21,  62, -396, -351, 239, 322, 121, // 4352-4359
240, 219, -44, 154, 389,  31, -165, 230, // 4360-4367
183, -395, -311, 224, 251, 139, 421, 272, // 4368-4375
-424, -468,  56, -136, -798, -595,  74, 124, // 4376-4383
  4, 381, 479, -275, -796, -293, 434, 494, // 4384-4391
171,  45,   0, -147, -123, 108, 144,  -8, // 4392-4399
 33,  57, -130,  10, 550, 458, -401, -693, // 4400-4407
-139, 176, -116, -224, 129, 385, 250,  23, // 4408-4415
122, 459, 433, -126, -588, -505, -146,  90, // 4416-4423
120,  16,   4, 299, 623, 402, -219, -371, // 4424-4431
 25,  80, -367, -352, 266, 489, 297, 381, // 4432-4439
465, 106, -81,  67, -183, -522, -207, 201, // 4440-4447
 37, -29, 263, 274,  60,  37, -103, -422, // 4448-4455
-343, -70, -135, -200, 135, 389, 217, -68, // 4456-4463
-254, -343, -279, -93,  70, 139,  90, -16, // 4464-4471
-19,  90, 127,   5, -184, -253, -68, 225, // 4472-4479
320, 146, -128, -287, -209, -40,  29, 140, // 4480-4487
272,  51, -362, -235, 301, 380,  23, -162, // 4488-4495
-87,  28, 107,  14, -98, 102, 323, 104, // 4496-4503
-55, 255, 324, -125, -328, -42,  83,  11, // 4504-4511
187, 405, 227, -209, -391, -46, 453, 438, // 4512-4519
-55, -253, -56, -135, -347, -94, 283, 134, // 4520-4527
-213, -212, -78, -101, -200, -244, -156,  44, // 4528-4535
157, 141,  86, -89, -245,  25, 359,  81, // 4536-4543
-319, -86, 218, -74, -292, -19, 161,  89, // 4544-4551
167, 177, -193, -471, -331, -59, 111, 180, // 4552-4559
101,  17, 102, 203, 131, -41, -271, -448, // 4560-4567
-274, 144, 254, -64, -338, -309, -82, 139, // 4568-4575
 94, -229, -398, -215,   0,  57,  21, -115, // 4576-4583
-213, -58,  27, -218, -288, 134, 340, -41, // 4584-4591
-297, -129, -72, -292, -293,   0,  91, -185, // 4592-4599
-407, -206, 113,  57, -145, -42, 150,  28, // 4600-4607
-153, -80, -67, -293, -432, -295, -63, 161, // 4608-4615
312, 158, -277, -549, -407, -159, -41,  85, // 4616-4623
165, -43, -330, -277, -97, -142, -183, -23, // 4624-4631
 28, -120, -118,  19, -33, -302, -521, -488, // 4632-4639
-185, 148, 213,   0, -257, -344, -196, -11, // 4640-4647
-51, -201, -150,  39,  72, -110, -321, -293, // 4648-4655
 -9,  87, -167, -246, -109, -296, -542, -153, // 4656-4663
265, -91, -435, -108, 153, -118, -292, -169, // 4664-4671
-38, 167, 276, -48, -253, 183, 481, 161, // 4672-4679
-89, 108, 285, 344, 395, 179, -133,  -2, // 4680-4687
356, 402, 377, 481, 350,  23,  44, 309, // 4688-4695
267,   7, -98, -85, -98, -31, 116, 173, // 4696-4703
112, -15, -210, -328, -231, -159, -277, -268, // 4704-4711
 21,  97, -234, -448, -352, -256, -220, -101, // 4712-4719
-39, -93, -137, -171, -134,  64,  74, -248, // 4720-4727
-314,  55, 233, 101, 154, 241,  48, -132, // 4728-4735
-73,  99, 368, 463,  43, -383, -234,  25, // 4736-4743
 -9,  96, 253,  42, -115, 169, 257, -125, // 4744-4751
-362, -273, -61, 285, 511, 263,  39, 213, // 4752-4759
223, -206, -390, -108,  77,  44,  14, -12, // 4760-4767
 30, 244, 209, -260, -529, -234,   8, -103, // 4768-4775
-24, 260, 185, -156, -169,  46,  22, -175, // 4776-4783
-310, -309, -129,  74, -18, -264, -306, -219, // 4784-4791
-214, -191, -122, -101, -63, -86, -328, -473, // 4792-4799
-206,  74,  38, -121, -244, -261,  38, 337, // 4800-4807
 74, -416, -400, -82,  -2,  -7, -15, -229, // 4808-4815
-401, -218, -90, -262, -253,   2,   0, -166, // 4816-4823
 19, 201, -113, -370, -53, 209, -13, -18, // 4824-4831
356, 362,  11,  -2, 119, -64,  -4, 445, // 4832-4839
545, 274, 342, 424, -52, -411, -105, 221, // 4840-4847
180, 256, 415, 271, 103, 221, 262,  66, // 4848-4855
 14, 104,  90, 115, 297, 333, 229, 325, // 4856-4863
471, 304,  43,  45, 104,  31,  -3,  59, // 4864-4871
129, 263, 405, 378, 288, 244,  88, -75, // 4872-4879
123, 459, 460, 260, 230, 288, 262, 245, // 4880-4887
228, 136,  61,  83,  76, -27, -72,  48, // 4888-4895
140, -30, -254, -211, -127, -256, -296, -59, // 4896-4903
 61, -26,   1,  64, -31, -32, 133, 120, // 4904-4911
 23,  88,  46, -221, -211, 153, 295, 131, // 4912-4919
 42,  75,  70, 127, 219, 128, -52, -18, // 4920-4927
149, 172, 192, 414, 556, 296, -60, -84, // 4928-4935
 46,  47,  37, 137, 210, 214, 190,  65, // 4936-4943
-78, -41,  46, -52, -144, -43,  33, -24, // 4944-4951
-39, -17, -61, -40,  23, -69, -154,  64, // 4952-4959
286, 149, -60, -58, -43, -93,  25, 217, // 4960-4967
224, 170, 241, 272, 175, 153, 208, 183, // 4968-4975
157, 245, 253,  95,  61, 244, 333, 218, // 4976-4983
206, 342, 286,  44,   8, 218, 272, 140, // 4984-4991
204, 458, 528, 401, 393, 449, 304, 110, // 4992-4999
144, 268, 235, 166, 196, 197, 139, 158, // 5000-5007
212, 166, 119, 145,  99, -48, -81,  45, // 5008-5015
141, 168, 202, 197, 110,  47,  59,  62, // 5016-5023
 43,  48,  62,  73, 139, 217, 162, -30, // 5024-5031
-185, -186, -97, -18,  27,  58,  77,  70, // 5032-5039
 57,  73,  94,  70,  24,  38, 106, 135, // 5040-5047
105,  93, 109,  90,  39,  46,  94,  97};// 5048-5055
