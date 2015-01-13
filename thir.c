/**********************************************************************
* Written by WAVToCode
* Date:				Fri Aug 16 06:36:17 PM
* FileName:			thir.c
* Interleaved:		Yes
* Signed:			Yes
* No. of channels:	1
* No. of samples:	4889
* Bits/Sample:		16
**********************************************************************/
#include "stm32f4xx.h"

#define THIR_SIZE 4889

static const int16_t ThirData[THIR_SIZE] = {
 43, -18, -67, -24,  16,  -7, -45, -74, // 0-7
-87, -62, -38, -37,  -8,  13, -61, -137, // 8-15
-86, -27, -89, -119,   0, 131, 114, -31, // 16-23
-189, -212, -77,  32,   3,   6, 142, 203, // 24-31
100,  44, 114, 151, 128, 157, 158,  38, // 32-39
-49,  52, 211, 243, 182, 112,  26, -80, // 40-47
-141, -123, -83, -54,   0,  70,  79,  25, // 48-55
-16, -38, -80, -141, -167, -71, 147, 275, // 56-63
191,  94, 119, 142, 403, 1229, 1932, 1466, // 64-71
173, -645, -760, -775, -719, -429, -187, -156, // 72-79
-180, -317, -522, -496, -316, -237, -15, 307, // 80-87
217,  19, 392, 733, 537, 1147, 2836, 3473, // 88-95
2205, 917, 225, -1165, -2487, -2038, -733, -511, // 96-103
-874, -679, -542, -986, -1316, -1039, -584, -389, // 104-111
-381, -173, 269, 306, -175, -187, 325, 178, // 112-119
-643, -949, -813, -1006, -954, -391, -239, -433, // 120-127
127, 924, 898, 830, 1284, 1246, 560, 381, // 128-135
425, -81, -355, 107, 250, -52, 270, 755, // 136-143
391, -224, -270, -274, -445, -264,  -7, -139, // 144-151
-257, -131, -155, -106, 309, 516, 188,   2, // 152-159
192, 255, 233, 350, 270, -127, -335, -288, // 160-167
-312, -319, -184, -165, -244, -109,  47, -75, // 168-175
-250, -201, -80,  13, 128, 147,  45,  35, // 176-183
102,  48,  21, 197, 322, 151, -23,  94, // 184-191
272, 278, 366, 832, 1370, 1442, 1113, 829, // 192-199
645, 244, -301, -459, -140, 101, -40, -178, // 200-207
-60, -157, -662, -906, -565, -368, -827, -1297, // 208-215
-1261, -1142, -1183, -1004, -569, -412, -625, -800, // 216-223
-831, -916, -1048, -981, -608, -191, -107, -288, // 224-231
-201, 216, 347,  91,  53, 316, 268, -135, // 232-239
-269, -91, -54, -116, 116, 520, 634, 414, // 240-247
156,  31,   7, -35, -156, -185,  62, 372, // 248-255
416, 342, 400, 275, -180, -372, -33, 109, // 256-263
-353, -725, -586, -366, -379, -446, -396, -172, // 264-271
108,  53, -365, -554, -339, -236, -249, -35, // 272-279
110, -121, -173, 209, 295, -51, -102, 305, // 280-287
527, 348, 131, 109, 284, 477, 555, 711, // 288-295
917, 658,  72,  -8, 270,  -8, -384,  62, // 296-303
584, 204, -270,  25, 253, -129, -334,  51, // 304-311
407, 350, 187, 245, 464, 416, -29, -165, // 312-319
332, 487, -23, -156, 321, 232, -365, -179, // 320-327
420, 188, -456, -358,   2, -215, -462, -146, // 328-335
265, 224, -44, -129, -11,  -6, -269, -412, // 336-343
-152,  -4, -292, -436, -88, 153, -55, -198, // 344-351
-40,  75,   6, -56, -38, -19, -79, -196, // 352-359
-163,  80, 162, -65, -119, 129, 116, -364, // 360-367
-699, -541, -316, -315, -355, -298, -309, -485, // 368-375
-609, -472, -247, -254, -388, -298, -123, -211, // 376-383
-365, -208,  86, 177, 200, 335, 417, 305, // 384-391
117,  -1,   9, 111, 193, 272, 452, 608, // 392-399
588, 593, 731, 749, 568, 479, 516, 493, // 400-407
516, 677, 776, 775, 862, 946, 925, 951, // 408-415
937, 627, 376, 595, 798, 604, 559, 933, // 416-423
997, 563, 409, 672, 717, 484, 486, 650, // 424-431
565, 329, 281, 424, 522, 465, 369, 395, // 432-439
490, 434, 227,  88,  68, 103, 210, 284, // 440-447
 87, -236, -236,   3, -45, -404, -505, -272, // 448-455
-311, -746, -1066, -1104, -1226, -1442, -1431, -1398, // 456-463
-1769, -2444, -3057, -3622, -4251, -4719, -4824, -4747, // 464-471
-4882, -5384, -5979, -6397, -6791, -7346, -7836, -8011, // 472-479
-7850, -6992, -5111, -2869, -1353, -620, -38, 438, // 480-487
605, 656, 868, 1385, 2401, 3778, 5013, 5981, // 488-495
6923, 7801, 8484, 9090, 9382, 8957, 8181, 7784, // 496-503
7665, 7392, 7183, 7279, 7479, 7591, 7345, 6115, // 504-511
3993, 2084, 999, 198, -679, -1234, -1352, -1345, // 512-519
-1350, -1428, -1825, -2542, -3141, -3338, -3318, -3382, // 520-527
-3353, -2728, -1683, -1047, -928, -503, 412, 1010, // 528-535
1171, 1507, 1848, 1583, 1174, 1611, 2670, 3393, // 536-543
3677, 4140, 4856, 5107, 4588, 3996, 3920, 4014, // 544-551
3795, 3577, 3612, 3294, 2412, 1802, 1938, 1951, // 552-559
1196, 428, 198, -179, -1353, -2689, -3382, -3723, // 560-567
-4236, -4832, -5398, -6275, -7454, -8290, -8640, -9081, // 568-575
-9787, -10172, -10116, -10398, -11455, -12902, -14287, -15499, // 576-583
-16371, -16355, -14482, -9996, -3780, 1373, 3305, 2748, // 584-591
1753, 1105, 553, 245, 1054, 3723, 7884, 11862, // 592-599
13895, 13906, 13503, 13931, 14791, 15000, 13999, 12055, // 600-607
9968, 8591, 8105, 7888, 7564, 7653, 8455, 8754, // 608-615
6901, 2780, -2246, -6464, -8746, -9179, -8852, -8380, // 616-623
-7398, -5897, -5090, -6124, -8569, -10844, -11742, -11275, // 624-631
-10381, -9721, -8963, -7394, -4821, -1765, 824, 2294, // 632-639
2934, 3509, 4061, 3916, 2834, 1816, 2375, 4938, // 640-647
8252, 10587, 11332, 11187, 10931, 10493, 9277, 7403, // 648-655
6417, 7571, 9282, 8854, 6793, 5550, 5157, 3792, // 656-663
2111, 1932, 1864, -223, -3053, -4147, -4252, -5287, // 664-671
-6275, -5425, -3503, -2856, -4180, -6176, -7592, -8160, // 672-679
-8070, -7408, -6131, -4464, -3029, -2261, -2080, -2270, // 680-687
-2795, -3448, -3782, -3693, -3451, -3326, -3499, -3952, // 688-695
-4160, -3803, -3600, -4442, -6017, -7472, -8926, -10793, // 696-703
-12174, -11122, -6716, -713, 3511, 4340, 3260, 2299, // 704-711
1277, -910, -3234, -3203, -102, 4291, 7572, 8797, // 712-719
8830, 8899, 9122, 9026, 8739, 8565, 7888, 6272, // 720-727
4780, 4499, 4900, 5021, 5329, 6611, 7862, 7015, // 728-735
3325, -1621, -5544, -7495, -7847, -7102, -5555, -3677, // 736-743
-2202, -1811, -2828, -4935, -7161, -8467, -8432, -7379, // 744-751
-6020, -4831, -3623, -1871, 365, 2272, 3237, 3509, // 752-759
3667, 3687, 2946, 1454, 461, 1127, 3340, 6055, // 760-767
8322, 9594, 9641, 8604, 6860, 4862, 3188, 2517, // 768-775
3146, 4462, 5357, 5377, 5046, 4640, 3742, 2470, // 776-783
1431, 529, -725, -2010, -2736, -2906, -2623, -2010, // 784-791
-1415, -898, -539, -1263, -3374, -5425, -6032, -5454, // 792-799
-4334, -2990, -1880, -1057, -293, -132, -961, -1852, // 800-807
-2300, -2761, -3147, -3128, -3326, -3807, -3620, -2873, // 808-815
-2525, -2618, -3087, -4616, -6860, -8712, -10192, -11758, // 816-823
-12803, -12952, -12452, -10258, -4951, 1863, 5804, 4726, // 824-831
1323, -777, -1154, -1775, -2825, -2095, 1418, 6079, // 832-839
9628, 11118, 11086, 10488, 9732, 8879, 8197, 7675, // 840-847
6644, 5056, 4240, 5042, 6568, 7329, 6978, 6277, // 848-855
5689, 4490, 1566, -2897, -6923, -8629, -8157, -6849, // 856-863
-5386, -3766, -2410, -2076, -2858, -4317, -6142, -7837, // 864-871
-8530, -7784, -6079, -4080, -2098, -203, 1645, 3318, // 872-879
4340, 4492, 4214, 3899, 3440, 2826, 2536, 2985, // 880-887
4193, 5920, 7648, 8755, 9019, 8617, 7628, 6016, // 888-895
4037, 2399, 1865, 2614, 3938, 4879, 5125, 4991, // 896-903
4510, 3345, 1736, 387, -593, -1685, -2763, -3113, // 904-911
-2758, -2374, -2132, -1671, -1136, -1233, -2241, -3735, // 912-919
-5079, -5719, -5353, -4298, -3291, -2470, -1427, -392, // 920-927
-318, -1262, -2198, -2694, -3016, -3247, -3513, -4159, // 928-935
-4745, -4537, -3937, -3779, -4004, -4502, -5794, -7741, // 936-943
-9604, -11255, -12816, -13857, -14189, -13734, -11251, -5728, // 944-951
835, 4458, 3682, 1019, -614, -826, -1077, -1733, // 952-959
-1122, 2140, 7019, 10964, 12400, 12077, 11650, 11539, // 960-967
11158, 10403, 9609, 8546, 7018, 6081, 6811, 8513, // 968-975
9388, 8726, 7370, 6021, 4183, 1064, -3020, -6418, // 976-983
-7672, -7092, -6050, -5120, -4040, -3105, -3264, -4620, // 984-991
-6271, -7578, -8398, -8565, -7932, -6573, -4534, -1965, // 992-999
572, 2480, 3657, 4309, 4554, 4392, 3813, 3041, // 1000-1007
2736, 3534, 5308, 7316, 8890, 9778, 10002, 9593, // 1008-1015
8520, 6891, 5063, 3482, 2474, 2207, 2589, 3192, // 1016-1023
3595, 3722, 3614, 2966, 1358, -962, -3060, -4219, // 1024-1031
-4608, -4710, -4442, -3452, -1982, -843, -669, -1420, // 1032-1039
-2582, -3684, -4576, -5148, -5054, -4104, -2839, -1955, // 1040-1047
-1303, -387, 521, 647,   2, -680, -1191, -1843, // 1048-1055
-2659, -3300, -3598, -3539, -3143, -2767, -2952, -3799, // 1056-1063
-5046, -6717, -8877, -11043, -12566, -13356, -13737, -13838, // 1064-1071
-13169, -10487, -4819, 2303, 6819, 6315, 3152, 1036, // 1072-1079
660,  98, -1224, -1222, 2074, 7847, 13045, 15099, // 1080-1087
14261, 12720, 11688, 10781, 9812, 9197, 8644, 7340, // 1088-1095
5927, 6020, 7557, 8482, 7450, 5434, 3920, 2807, // 1096-1103
789, -2992, -7616, -10811, -11230, -9803, -8065, -6448, // 1104-1111
-4909, -4161, -4810, -6243, -7602, -8667, -9356, -9205, // 1112-1119
-7774, -4962, -1130, 2528, 4523, 4722, 4681, 5490, // 1120-1127
6339, 5944, 4404, 3169, 3488, 5276, 7370, 8689, // 1128-1135
9194, 9528, 9735, 9138, 7579, 5642, 3749, 1919, // 1136-1143
551, 415, 1513, 2818, 3230, 2512, 1275, 163, // 1144-1151
-886, -2365, -4257, -5676, -5730, -4543, -3116, -2263, // 1152-1159
-2050, -2061, -2122, -2503, -3342, -4236, -4611, -4261, // 1160-1167
-3372, -2144, -715, 566, 1221, 1185, 1046, 1326, // 1168-1175
1643, 1212,  -1, -1130, -1528, -1571, -1847, -2071, // 1176-1183
-1708, -1190, -1657, -3369, -5392, -7078, -8735, -10708, // 1184-1191
-12583, -13628, -13726, -13265, -11922, -8364, -2159, 4117, // 1192-1199
6626, 4745, 1573, -142, -778, -1884, -2975, -1939, // 1200-1207
2042, 7448, 11738, 13541, 13385, 12361, 10801, 8901, // 1208-1215
7557, 7413, 7546, 6759, 5680, 5746, 6869, 7382, // 1216-1223
6213, 3922, 1705, 116, -1434, -3985, -7388, -9988, // 1224-1231
-10616, -9727, -8171, -6217, -4311, -3498, -4176, -5619, // 1232-1239
-6989, -7962, -8384, -8016, -6581, -3740, 362, 4310, // 1240-1247
6249, 6022, 5354, 5411, 5742, 5484, 4663, 4041, // 1248-1255
4416, 5965, 7862, 8965, 9130, 9083, 8999, 8428, // 1256-1263
7325, 6033, 4583, 2863, 1239, 352, 513, 1432, // 1264-1271
2202, 1891, 585, -630, -1218, -1847, -3250, -4928, // 1272-1279
-5765, -5422, -4380, -3217, -2361, -2113, -2444, -2993, // 1280-1287
-3429, -3558, -3319, -2882, -2437, -1726, -425, 1073, // 1288-1295
1945, 1976, 1699, 1491, 1272, 939, 511, -63, // 1296-1303
-911, -1943, -2693, -2714, -2255, -2287, -3444, -5280, // 1304-1311
-7090, -8839, -10803, -12846, -14533, -15637, -15957, -14804, // 1312-1319
-11173, -4993, 1668, 5493, 4989, 1998, -573, -1786, // 1320-1327
-2720, -3778, -3421, -142, 5523, 11149, 14442, 14933, // 1328-1335
13693, 11764, 9626, 7834, 7120, 7337, 7367, 6868, // 1336-1343
6818, 7820, 8787, 8186, 5922, 3241, 1245, -110, // 1344-1351
-1933, -4978, -8303, -10161, -10072, -8817, -6998, -4809, // 1352-1359
-3095, -2924, -4169, -5829, -7256, -8346, -8866, -8378, // 1360-1367
-6476, -2985, 1379, 4873, 6215, 5940, 5318, 4849, // 1368-1375
4421, 4075, 3939, 4125, 4997, 6672, 8429, 9416, // 1376-1383
9642, 9449, 8738, 7540, 6374, 5469, 4372, 2840, // 1384-1391
1360, 554, 702, 1579, 2336, 2028, 765, -475, // 1392-1399
-1271, -2192, -3601, -4975, -5733, -5702, -4799, -3242, // 1400-1407
-1903, -1580, -2090, -2757, -3203, -3382, -3381, -3358, // 1408-1415
-3175, -2357, -877, 536, 1307, 1632, 1769, 1532, // 1416-1423
875, 190, -194, -415, -824, -1574, -2404, -2800, // 1424-1431
-2650, -2640, -3504, -5061, -6690, -8308, -10285, -12509, // 1432-1439
-14317, -15410, -15970, -15674, -13272, -7852, -734, 4619, // 1440-1447
5689, 3296, 594, -623, -1438, -3045, -4115, -2231, // 1448-1455
3080, 9417, 13809, 15350, 15087, 13929, 11897, 9337, // 1456-1463
7609, 7493, 8002, 7921, 7431, 7546, 8286, 8361, // 1464-1471
6744, 3958, 1518, 144, -1184, -3748, -7077, -9530, // 1472-1479
-10199, -9449, -7804, -5524, -3382, -2559, -3589, -5814, // 1480-1487
-7940, -8997, -8993, -8482, -7318, -4417, 441, 5215, // 1488-1495
7385, 6766, 5211, 4359, 4266, 4070, 3427, 3168, // 1496-1503
4350, 6732, 8761, 9490, 9521, 9468, 8967, 7699, // 1504-1511
6170, 4850, 3506, 1952, 560, -136, 157, 1257, // 1512-1519
2319, 2451, 1731, 840, -97, -1538, -3396, -4962, // 1520-1527
-5799, -5776, -4751, -2948, -1239, -397, -493, -1280, // 1528-1535
-2432, -3432, -3881, -3874, -3479, -2376, -538, 1270, // 1536-1543
2281, 2445, 1969, 949, -289, -1154, -1350, -1149, // 1544-1551
-1072, -1486, -2256, -2730, -2583, -2586, -3698, -5660, // 1552-1559
-7484, -8932, -10609, -12591, -14148, -14955, -15271, -14795, // 1560-1567
-12134, -6347, 952, 6160, 6810, 4003, 1015, -511, // 1568-1575
-1666, -3284, -3922, -1442, 4071, 10056, 14083, 15752, // 1576-1583
15710, 14192, 11231, 7901, 6023, 6184, 7136, 7422, // 1584-1591
7132, 7419, 8405, 8554, 6594, 3131,  53, -1640, // 1592-1599
-2776, -4570, -6960, -8878, -9503, -8861, -7381, -5530, // 1600-1607
-3904, -3206, -3883, -5645, -7406, -8169, -8067, -7815, // 1608-1615
-7209, -4985, -687, 4118, 7247, 7821, 6689, 5387, // 1616-1623
4696, 4216, 3464, 2989, 3831, 6050, 8481, 10079, // 1624-1631
10718, 10617, 9865, 8523, 6750, 4822, 3204, 2193, // 1632-1639
1568, 1051, 985, 1698, 2446, 2217, 1069, -129, // 1640-1647
-1034, -2032, -3342, -4673, -5506, -5431, -4546, -3486, // 1648-1655
-2694, -2042, -1557, -1751, -2736, -3709, -3980, -3752, // 1656-1663
-3359, -2658, -1445,  84, 1513, 2355, 2237, 1350, // 1664-1671
422, -105, -475, -1004, -1668, -2296, -2642, -2513, // 1672-1679
-2254, -2666, -3940, -5469, -6890, -8597, -10685, -12407, // 1680-1687
-13360, -14019, -14613, -14015, -10597, -4342, 2305, 6080, // 1688-1695
5670, 2881, 416, -1057, -2561, -4202, -4332, -1330, // 1696-1703
4359, 10406, 14571, 16098, 15471, 13385, 10318, 7122, // 1704-1711
5116, 4977, 6077, 7258, 8097, 8864, 9450, 9050, // 1712-1719
6940, 3432, -83, -2353, -3493, -4546, -6053, -7447, // 1720-1727
-7866, -7150, -5873, -4685, -3930, -3742, -4297, -5606, // 1728-1735
-7098, -8000, -8111, -7722, -6777, -4676, -1227, 2667, // 1736-1743
5606, 6709, 6191, 4989, 3840, 2817, 1916, 1697, // 1744-1751
2834, 5078, 7337, 8911, 9885, 10398, 10135, 8713, // 1752-1759
6343, 4003, 2681, 2444, 2426, 2122, 2088, 2786, // 1760-1767
3554, 3430, 2302, 809, -519, -1621, -2726, -3975, // 1768-1775
-5000, -5114, -4257, -3265, -2704, -2322, -1879, -1820, // 1776-1783
-2584, -3846, -4875, -5127, -4447, -3197, -1974, -907, // 1784-1791
281, 1367, 1633, 885, -231, -1203, -2118, -3053, // 1792-1799
-3746, -3879, -3383, -2612, -2339, -3047, -4403, -5865, // 1800-1807
-7586, -9865, -12112, -13527, -14090, -13953, -12357, -8288, // 1808-1815
-2199, 3737, 7059, 6714, 3851, 667, -1748, -3783, // 1816-1823
-5369, -5021, -1435, 4508, 10327, 14168, 15672, 15230, // 1824-1831
13161, 9726, 5827, 3161, 2892, 4463, 6251, 7521, // 1832-1839
8751, 9933, 9945, 7830, 3970, -165, -3213, -4865, // 1840-1847
-5756, -6452, -6742, -6135, -4808, -3500, -2715, -2604, // 1848-1855
-3265, -4669, -6354, -7608, -8032, -7684, -6797, -5425, // 1856-1863
-3370, -590, 2307, 4344, 4980, 4514, 3583, 2593, // 1864-1871
1712, 1101, 1065, 1987, 3929, 6312, 8345, 9664, // 1872-1879
10178, 9707, 8192, 6112, 4231, 3010, 2544, 2736, // 1880-1887
3379, 4175, 4801, 4948, 4416, 3238, 1631, -166, // 1888-1895
-1829, -2926, -3297, -3271, -3200, -3014, -2624, -2312, // 1896-1903
-2312, -2508, -2798, -3359, -4229, -5035, -5308, -4782, // 1904-1911
-3574, -2246, -1355, -830, -314,   0, -491, -1778, // 1912-1919
-3104, -3946, -4352, -4391, -3996, -3301, -2693, -2562, // 1920-1927
-3157, -4454, -6130, -7946, -9966, -12071, -13693, -14250, // 1928-1935
-13299, -10265, -4946, 1304, 6019, 7637, 6494, 3898, // 1936-1943
800, -2338, -4988, -6216, -4855, -459, 5719, 11342, // 1944-1951
14861, 16050, 15058, 12075, 7972, 4281, 2167, 1891, // 1952-1959
3121, 5250, 7654, 9790, 10917, 10116, 7109, 2904, // 1960-1967
-965, -3747, -5538, -6471, -6519, -5755, -4412, -2874, // 1968-1975
-1785, -1689, -2525, -3841, -5344, -6796, -7662, -7578, // 1976-1983
-6757, -5493, -3705, -1394, 929, 2692, 3667, 3852, // 1984-1991
3408, 2713, 2097, 1598, 1297, 1567, 2686, 4399, // 1992-1999
6196, 7771, 8969, 9562, 9336, 8236, 6438, 4444, // 2000-2007
2898, 2144, 2088, 2610, 3663, 4828, 5377, 4951, // 2008-2015
3724, 1951, -170, -2173, -3475, -3866, -3532, -2760, // 2016-2023
-1895, -1380, -1452, -2011, -2887, -3975, -5007, -5569, // 2024-2031
-5518, -5008, -4168, -3084, -2034, -1347, -1057, -1025, // 2032-2039
-1203, -1614, -2241, -3063, -3948, -4514, -4424, -3807, // 2040-2047
-3116, -2641, -2516, -2955, -4095, -5805, -7828, -9943, // 2048-2055
-11885, -13330, -13801, -12431, -8409, -2296, 3588, 6993, // 2056-2063
7453, 5954, 3483, 393, -2957, -5630, -6324, -4092, // 2064-2071
826, 6866, 12110, 15388, 16293, 14750, 11189, 6842, // 2072-2079
3238, 1357, 1360, 2850, 5200, 7868, 10275, 11398, // 2080-2087
10221, 6790, 2442, -1351, -4027, -5677, -6382, -6125, // 2088-2095
-4981, -3281, -1686, -947, -1334, -2513, -4025, -5596, // 2096-2103
-6898, -7497, -7295, -6508, -5190, -3237, -908, 1148, // 2104-2111
2481, 3048, 3019, 2664, 2249, 1856, 1478, 1313, // 2112-2119
1704, 2802, 4424, 6176, 7609, 8437, 8650, 8318, // 2120-2127
7353, 5762, 4039, 2818, 2322, 2422, 3035, 4049, // 2128-2135
4968, 5209, 4575, 3249, 1523, -274, -1816, -2929, // 2136-2143
-3536, -3505, -2813, -1830, -1179, -1244, -1984, -3091, // 2144-2151
-4188, -4991, -5415, -5477, -5125, -4316, -3219, -2181, // 2152-2159
-1466, -1147, -1182, -1449, -1833, -2346, -3055, -3818, // 2160-2167
-4299, -4241, -3694, -2987, -2540, -2618, -3210, -4186, // 2168-2175
-5553, -7409, -9576, -11569, -13000, -13500, -12279, -8467, // 2176-2183
-2443, 3624, 7329, 7882, 6220, 3659, 723, -2532, // 2184-2191
-5379, -6282, -4017, 1057, 7088, 12116, 15163, 16001, // 2192-2199
14565, 11129, 6775, 3105, 1257, 1345, 2813, 5052, // 2200-2207
7652, 10010, 11018, 9790, 6552, 2543, -1082, -3913, // 2208-2215
-5896, -6864, -6648, -5348, -3452, -1758, -1034, -1494, // 2216-2223
-2750, -4288, -5827, -7105, -7724, -7502, -6573, -5061, // 2224-2231
-3003, -711, 1224, 2406, 2812, 2636, 2212, 1844, // 2232-2239
1558, 1262, 1190, 1792, 3178, 4942, 6562, 7720, // 2240-2247
8300, 8308, 7774, 6681, 5201, 3930, 3398, 3504, // 2248-2255
3795, 4173, 4792, 5373, 5348, 4499, 3037, 1319, // 2256-2263
-220, -1235, -1825, -2283, -2536, -2298, -1777, -1613, // 2264-2271
-2049, -2783, -3510, -4182, -4802, -5258, -5326, -4809, // 2272-2279
-3828, -2907, -2418, -2140, -1807, -1679, -2070, -2790, // 2280-2287
-3437, -3930, -4312, -4453, -4261, -3835, -3342, -3045, // 2288-2295
-3334, -4331, -5747, -7329, -9143, -11099, -12646, -13159, // 2296-2303
-12045, -8668, -3129, 2826, 6760, 7706, 6478, 4218, // 2304-2311
1358, -1953, -4948, -6126, -4253, 500, 6388, 11420, // 2312-2319
14640, 15856, 14876, 11784, 7545, 3694, 1407, 1078, // 2320-2327
2427, 4734, 7238, 9409, 10610, 9930, 7114, 3129, // 2328-2335
-654, -3584, -5601, -6567, -6332, -5051, -3191, -1499, // 2336-2343
-825, -1464, -2844, -4295, -5672, -6987, -7827, -7694, // 2344-2351
-6581, -4811, -2725, -705, 835, 1715, 2009, 1873, // 2352-2359
1558, 1347, 1262, 1151, 1237, 1997, 3479, 5174, // 2360-2367
6589, 7529, 7933, 7908, 7664, 7162, 6154, 4797, // 2368-2375
3744, 3376, 3538, 3995, 4645, 5196, 5202, 4556, // 2376-2383
3472, 2106, 530, -1025, -2235, -2879, -2849, -2253, // 2384-2391
-1567, -1339, -1706, -2471, -3469, -4532, -5360, -5701, // 2392-2399
-5523, -4900, -3997, -3139, -2607, -2362, -2265, -2404, // 2400-2407
-2869, -3492, -4056, -4533, -4864, -4841, -4444, -3921, // 2408-2415
-3508, -3377, -3797, -4922, -6464, -8006, -9491, -11117, // 2416-2423
-12661, -13130, -11294, -6723, -523, 4973, 7788, 7709, // 2424-2431
5908, 3365, 296, -3075, -5642, -5810, -2865, 2292, // 2432-2439
7816, 12222, 14924, 15612, 13925, 10209, 5891, 2651, // 2440-2447
1267, 1507, 2866, 5013, 7568, 9786, 10617, 9315, // 2448-2455
6142, 2284, -1076, -3512, -5155, -5972, -5658, -4239, // 2456-2463
-2365, -974, -684, -1460, -2816, -4303, -5687, -6753, // 2464-2471
-7202, -6831, -5685, -3994, -2090, -366, 838, 1395, // 2472-2479
1435, 1262, 1117, 1040, 959, 980, 1454, 2624, // 2480-2487
4251, 5778, 6801, 7304, 7436, 7278, 6863, 6241, // 2488-2495
5481, 4715, 4179, 4082, 4400, 4866, 5139, 5001, // 2496-2503
4445, 3600, 2609, 1538, 375, -844, -1864, -2330, // 2504-2511
-2212, -1880, -1756, -1998, -2556, -3339, -4194, -4883, // 2512-2519
-5232, -5261, -5070, -4663, -4045, -3437, -3160, -3288, // 2520-2527
-3537, -3653, -3799, -4282, -5005, -5446, -5282, -4781, // 2528-2535
-4382, -4172, -4050, -4154, -4848, -6238, -7906, -9365, // 2536-2543
-10552, -11566, -11862, -10167, -5818, -99, 4553, 6546, // 2544-2551
6215, 4789, 2832, 224, -2712, -4727, -4369, -1237, // 2552-2559
3580, 8383, 12034, 14062, 14246, 12478, 9164, 5450, // 2560-2567
2653, 1546, 2011, 3436, 5360, 7544, 9446, 10097, // 2568-2575
8799, 5887, 2467, -561, -2938, -4624, -5463, -5269, // 2576-2583
-4114, -2505, -1252, -932, -1512, -2587, -3889, -5302, // 2584-2591
-6528, -7145, -6884, -5758, -3999, -2012, -275, 868, // 2592-2599
1340, 1329, 1140, 993, 913, 860, 947, 1433, // 2600-2607
2440, 3800, 5200, 6340, 6976, 7069, 6857, 6565, // 2608-2615
6145, 5498, 4758, 4209, 4076, 4413, 5018, 5460, // 2616-2623
5380, 4763, 3843, 2797, 1696, 620, -345, -1165, // 2624-2631
-1766, -2010, -1912, -1761, -1861, -2292, -2950, -3692, // 2632-2639
-4383, -4891, -5113, -5008, -4658, -4238, -3850, -3478, // 2640-2647
-3181, -3132, -3377, -3747, -4109, -4501, -4873, -5010, // 2648-2655
-4818, -4509, -4326, -4287, -4381, -4761, -5589, -6823, // 2656-2663
-8258, -9696, -10936, -11525, -10615, -7453, -2404, 2740, // 2664-2671
5994, 6699, 5589, 3678, 1320, -1406, -3760, -4413, // 2672-2679
-2485, 1602, 6411, 10553, 13289, 14283, 13304, 10505, // 2680-2687
6784, 3473, 1554, 1266, 2325, 4236, 6492, 8525, // 2688-2695
9585, 9008, 6738, 3498, 289, -2285, -4119, -5184, // 2696-2703
-5296, -4378, -2809, -1371, -762, -1190, -2389, -3899, // 2704-2711
-5342, -6491, -7157, -7129, -6274, -4664, -2665, -836, // 2712-2719
361, 827, 794, 622, 588, 714, 818, 835, // 2720-2727
1010, 1661, 2833, 4220, 5411, 6187, 6602, 6805, // 2728-2735
6836, 6592, 5990, 5185, 4575, 4537, 5112, 5920, // 2736-2743
6414, 6245, 5419, 4184, 2824, 1543, 459, -352, // 2744-2751
-827, -945, -817, -691, -813, -1308, -2191, -3324, // 2752-2759
-4355, -4896, -4872, -4573, -4307, -4124, -3956, -3831, // 2760-2767
-3841, -3958, -4017, -3940, -3858, -3996, -4403, -4861, // 2768-2775
-5115, -5129, -5012, -4868, -4799, -4938, -5358, -6018, // 2776-2783
-6877, -7973, -9322, -10637, -11097, -9668, -6014, -1114, // 2784-2791
3162, 5388, 5464, 4196, 2271, -13, -2322, -3847, // 2792-2799
-3567, -1021, 3182, 7688, 11220, 13131, 13250, 11632, // 2800-2807
8703, 5390, 2835, 1833, 2458, 4156, 6222, 8091, // 2808-2815
9287, 9330, 7950, 5400, 2372, -409, -2489, -3724, // 2816-2823
-4124, -3746, -2731, -1436, -469, -415, -1366, -2857, // 2824-2831
-4351, -5603, -6532, -7016, -6874, -5962, -4341, -2427, // 2832-2839
-855, -59,  43, -104, -113,  84, 318, 480, // 2840-2847
741, 1413, 2576, 3929, 5030, 5647, 5891, 6055, // 2848-2855
6340, 6695, 6858, 6612, 6040, 5445, 5057, 4908, // 2856-2863
4904, 4868, 4636, 4192, 3624, 2945, 2121, 1196, // 2864-2871
278, -561, -1280, -1787, -2018, -2036, -2024, -2177, // 2872-2879
-2609, -3296, -4047, -4617, -4898, -4974, -4945, -4808, // 2880-2887
-4532, -4170, -3874, -3808, -4031, -4461, -4982, -5483, // 2888-2895
-5791, -5724, -5320, -4894, -4750, -4935, -5319, -5839, // 2896-2903
-6586, -7663, -9010, -10265, -10702, -9519, -6447, -2170, // 2904-2911
1921, 4537, 5141, 4137, 2331, 285, -1684, -3060, // 2912-2919
-3032, -1037, 2634, 6859, 10341, 12296, 12509, 11155, // 2920-2927
8668, 5758, 3357, 2249, 2691, 4300, 6350, 8165, // 2928-2935
9255, 9300, 8163, 5985, 3234, 586, -1430, -2668, // 2936-2943
-3173, -2937, -1986, -683, 258, 228, -850, -2513, // 2944-2951
-4148, -5428, -6358, -6957, -7016, -6205, -4488, -2378, // 2952-2959
-657, 195, 252, -91, -432, -581, -538, -311, // 2960-2967
173, 978, 2072, 3326, 4550, 5554, 6230, 6564, // 2968-2975
6596, 6385, 6004, 5533, 5074, 4729, 4570, 4627, // 2976-2983
4864, 5158, 5330, 5212, 4676, 3688, 2406, 1120, // 2984-2991
 67, -667, -1098, -1275, -1254, -1113, -1015, -1200, // 2992-2999
-1858, -2967, -4237, -5274, -5811, -5833, -5497, -5017, // 3000-3007
-4579, -4307, -4286, -4566, -5083, -5626, -5984, -6115, // 3008-3015
-6124, -6099, -6040, -5922, -5844, -6011, -6526, -7259, // 3016-3023
-8045, -8875, -9767, -10376, -9892, -7643, -3854, 282, // 3024-3031
3315, 4442, 3835, 2221, 236, -1693, -3048, -3065, // 3032-3039
-1180, 2365, 6527, 10025, 12019, 12361, 11295, 9127, // 3040-3047
6310, 3679, 2268, 2700, 4687, 7207, 9259, 10326, // 3048-3055
10308, 9218, 7110, 4289, 1409, -803, -1994, -2318, // 3056-3063
-2078, -1379, -291, 802, 1214, 418, -1446, -3605, // 3064-3071
-5308, -6362, -6963, -7182, -6723, -5294, -3151, -1077, // 3072-3079
147, 295, -269, -1006, -1588, -1926, -1950, -1506, // 3080-3087
-489, 993, 2673, 4270, 5553, 6364, 6631, 6388, // 3088-3095
5793, 5103, 4572, 4336, 4421, 4784, 5324, 5892, // 3096-3103
6317, 6427, 6077, 5195, 3841, 2267, 885,  69, // 3104-3111
-69, 246, 624, 741, 484, -113, -997, -2092, // 3112-3119
-3215, -4136, -4747, -5121, -5406, -5632, -5654, -5376, // 3120-3127
-5002, -4949, -5469, -6336, -6984, -7021, -6585, -6141, // 3128-3135
-6044, -6290, -6644, -6929, -7155, -7410, -7744, -8214, // 3136-3143
-8897, -9647, -9899, -8918, -6330, -2593, 1063, 3343, // 3144-3151
3731, 2620, 823, -958, -2296, -2796, -1975, 404, // 3152-3159
3951, 7657, 10436, 11663, 11329, 9770, 7420, 4842, // 3160-3167
2871, 2381, 3676, 6176, 8749, 10356, 10591, 9673, // 3168-3175
7991, 5807, 3374, 1109, -508, -1222, -1155, -630, // 3176-3183
115, 939, 1567, 1523, 465, -1425, -3505, -5109, // 3184-3191
-6010, -6387, -6354, -5693, -4210, -2172, -333, 563, // 3192-3199
362, -455, -1313, -1955, -2404, -2605, -2252, -1056, // 3200-3207
870, 2997, 4746, 5843, 6307, 6217, 5632, 4699, // 3208-3215
3742, 3158, 3158, 3640, 4376, 5199, 5978, 6502, // 3216-3223
6493, 5778, 4472, 2942, 1588, 626,  82, -88, // 3224-3231
 49, 361, 605, 496, -104, -1099, -2255, -3350, // 3232-3239
-4229, -4827, -5152, -5241, -5140, -4913, -4657, -4536, // 3240-3247
-4762, -5427, -6314, -6995, -7157, -6847, -6419, -6207, // 3248-3255
-6252, -6387, -6505, -6673, -7044, -7687, -8505, -9268, // 3256-3263
-9625, -9089, -7210, -4009, -265, 2794, 4210, 3782, // 3264-3271
2092,  26, -1717, -2698, -2546, -987, 1869, 5336, // 3272-3279
8405, 10308, 10835, 10148, 8518, 6305, 4125, 2814, // 3280-3287
2976, 4542, 6729, 8578, 9549, 9592, 8820, 7305, // 3288-3295
5176, 2818, 810, -427, -893, -802, -307, 499, // 3296-3303
1337, 1650, 971, -637, -2579, -4196, -5246, -5873, // 3304-3311
-6169, -5923, -4858, -3074, -1183, 120, 567, 327, // 3312-3319
-329, -1253, -2319, -3190, -3332, -2416, -606, 1530, // 3320-3327
3422, 4770, 5495, 5562, 4960, 3865, 2727, 2070, // 3328-3335
2153, 2871, 3928, 5055, 6071, 6761, 6830, 6103, // 3336-3343
4728, 3130, 1768, 890, 483, 428, 654, 1070, // 3344-3351
1440, 1431, 853, -189, -1404, -2524, -3465, -4254, // 3352-3359
-4848, -5122, -5027, -4725, -4543, -4742, -5301, -5963, // 3360-3367
-6488, -6810, -7025, -7258, -7510, -7675, -7700, -7644, // 3368-3375
-7596, -7609, -7720, -7980, -8408, -8862, -8939, -8107, // 3376-3383
-6106, -3251, -340, 1760, 2513, 1928, 470, -1168, // 3384-3391
-2283, -2362, -1220, 934, 3574, 6052, 7865, 8800, // 3392-3399
8846, 8117, 6855, 5476, 4505, 4364, 5159, 6591, // 3400-3407
8113, 9199, 9511, 8957, 7670, 5980, 4344, 3168, // 3408-3415
2565, 2329, 2179, 2016, 1976, 2192, 2488, 2396, // 3416-3423
1545,   8, -1721, -3085, -3801, -3921, -3607, -2948, // 3424-3431
-2004, -996, -292, -160, -581, -1303, -2046, -2665, // 3432-3439
-3101, -3216, -2808, -1790, -346, 1122, 2222, 2767, // 3440-3447
2785, 2402, 1800, 1244, 1016, 1240, 1794, 2417, // 3448-3455
2943, 3411, 3915, 4342, 4401, 3911, 3017, 2122, // 3456-3463
1555, 1318, 1202, 1102, 1106, 1291, 1493, 1358, // 3464-3471
665, -438, -1554, -2382, -2936, -3404, -3870, -4241, // 3472-3479
-4404, -4400, -4404, -4532, -4735, -4907, -5024, -5178, // 3480-3487
-5466, -5875, -6279, -6561, -6660, -6547, -6238, -5850, // 3488-3495
-5586, -5591, -5781, -5795, -5205, -3887, -2183, -669, // 3496-3503
213, 351, -81, -792, -1478, -1825, -1556, -566, // 3504-3511
938, 2484, 3580, 3998, 3834, 3358, 2859, 2565, // 3512-3519
2619, 3055, 3780, 4577, 5195, 5487, 5471, 5238, // 3520-3527
4852, 4349, 3797, 3331, 3106, 3174, 3441, 3724, // 3528-3535
3852, 3745, 3425, 2961, 2415, 1830, 1236, 657, // 3536-3543
132, -262, -459, -449, -319, -220, -288, -547, // 3544-3551
-914, -1267, -1492, -1528, -1386, -1138, -875, -679, // 3552-3559
-599, -630, -728, -825, -836, -674, -297, 237, // 3560-3567
777, 1166, 1350, 1380, 1321, 1187, 981, 768, // 3568-3575
692, 861, 1241, 1673, 2018, 2254, 2414, 2466, // 3576-3583
2307, 1869, 1249, 678, 346, 258, 315, 447, // 3584-3591
651, 877, 945, 642, -66, -974, -1784, -2344, // 3592-3599
-2693, -2927, -3062, -3064, -2974, -2942, -3088, -3366, // 3600-3607
-3632, -3791, -3893, -4051, -4276, -4457, -4514, -4495, // 3608-3615
-4499, -4540, -4522, -4374, -4160, -4000, -3862, -3502, // 3616-3623
-2670, -1414, -140, 656, 707, 101, -834, -1713, // 3624-3631
-2180, -1973, -1003, 533, 2172, 3388, 3896, 3750, // 3632-3639
3195, 2498, 1863, 1463, 1456, 1908, 2715, 3618, // 3640-3647
4344, 4733, 4747, 4411, 3784, 2989, 2258, 1854, // 3648-3655
1885, 2236, 2679, 3017, 3175, 3171, 2999, 2589, // 3656-3663
1902, 1015, 106, -635, -1076, -1155, -873, -300, // 3664-3671
382, 896, 984, 564, -224, -1106, -1816, -2188, // 3672-3679
-2154, -1716, -959, -74, 684, 1104, 1130, 889, // 3680-3687
557, 249,  14, -97, -23, 262, 674, 1041, // 3688-3695
1218, 1200, 1090, 951, 750, 441,  71, -213, // 3696-3703
-296, -202, -65,  14,  34,  27, -29, -222, // 3704-3711
-582, -1015, -1362, -1523, -1518, -1420, -1272, -1103, // 3712-3719
-982, -1013, -1231, -1515, -1668, -1605, -1427, -1310, // 3720-3727
-1328, -1403, -1430, -1420, -1468, -1615, -1770, -1799, // 3728-3735
-1677, -1516, -1434, -1426, -1388, -1251, -1064, -934, // 3736-3743
-924, -1029, -1221, -1492, -1813, -2069, -2101, -1824, // 3744-3751
-1320, -783, -391, -206, -200, -325, -529, -722, // 3752-3759
-779, -621, -291,  80, 377, 570, 700, 793, // 3760-3767
817, 736, 582, 457, 456, 608, 871, 1178, // 3768-3775
1472, 1700, 1814, 1791, 1661, 1494, 1345, 1227, // 3776-3783
1130, 1077, 1119, 1291, 1558, 1825, 1993, 2012, // 3784-3791
1895, 1697, 1491, 1348, 1320, 1422, 1618, 1846, // 3792-3799
2051, 2203, 2288, 2286, 2180, 1989, 1776, 1605, // 3800-3807
1493, 1391, 1248, 1076, 947, 919, 974, 1038, // 3808-3815
1043, 964, 805, 575, 288,  -9, -244, -362, // 3816-3823
-376, -353, -357, -393, -428, -453, -503, -610, // 3824-3831
-755, -867, -889, -833, -773, -772, -841, -952, // 3832-3839
-1081, -1222, -1365, -1479, -1531, -1519, -1473, -1419, // 3840-3847
-1355, -1276, -1203, -1176, -1214, -1294, -1366, -1399, // 3848-3855
-1405, -1403, -1388, -1330, -1229, -1126, -1085, -1129, // 3856-3863
-1224, -1313, -1370, -1406, -1438, -1462, -1448, -1369, // 3864-3871
-1243, -1127, -1078, -1107, -1181, -1242, -1241, -1179, // 3872-3879
-1099, -1043, -1014, -978, -896, -761, -603, -470, // 3880-3887
-376, -288, -159,  24, 241, 450, 612, 703, // 3888-3895
727, 716, 722, 785, 914, 1074, 1212, 1301, // 3896-3903
1358, 1416, 1489, 1567, 1621, 1638, 1627, 1615, // 3904-3911
1621, 1642, 1669, 1688, 1688, 1664, 1628, 1602, // 3912-3919
1599, 1620, 1641, 1635, 1586, 1498, 1383, 1243, // 3920-3927
1086, 937, 836, 816, 870, 949, 994, 968, // 3928-3935
873, 733, 569, 395, 232, 105,  32,   7, // 3936-3943
 11,   9, -31, -119, -237, -360, -456, -493, // 3944-3951
-469, -416, -388, -434, -568, -736, -847, -845, // 3952-3959
-744, -615, -534, -530, -569, -598, -602, -607, // 3960-3967
-640, -695, -731, -713, -634, -525, -422, -344, // 3968-3975
-290, -253, -230, -224, -233, -243, -241, -239, // 3976-3983
-255, -304, -384, -483, -573, -622, -617, -575, // 3984-3991
-535, -529, -552, -560, -518, -449, -423, -494, // 3992-3999
-646, -797, -875, -871, -845, -851, -882, -882, // 4000-4007
-811, -681, -549, -458, -415, -398, -391, -385, // 4008-4015
-376, -351, -301, -231, -161, -94, -14,  88, // 4016-4023
195, 260, 264, 236, 233, 298, 425, 573, // 4024-4031
702, 793, 838, 843, 828, 816, 832, 884, // 4032-4039
954, 1000, 996, 955, 898, 832, 759, 698, // 4040-4047
675, 707, 781, 850, 873, 845, 799, 761, // 4048-4055
721, 657, 569, 486, 434, 414, 393, 343, // 4056-4063
263, 185, 137, 125, 133, 139, 136, 129, // 4064-4071
115,  84,  26, -48, -118, -160, -175, -184, // 4072-4079
-208, -246, -280, -289, -261, -215, -182, -174, // 4080-4087
-177, -170, -146, -116, -98, -104, -123, -136, // 4088-4095
-137, -138, -160, -213, -279, -323, -317, -259, // 4096-4103
-180, -129, -137, -189, -239, -256, -247, -246, // 4104-4111
-277, -328, -363, -361, -343, -350, -388, -428, // 4112-4119
-442, -437, -446, -486, -527, -519, -440, -327, // 4120-4127
-245, -230, -258, -286, -292, -284, -278, -282, // 4128-4135
-283, -266, -229, -172, -95, -17,  24,  -2, // 4136-4143
-81, -161, -186, -145, -74, -12,  36,  93, // 4144-4151
164, 224, 237, 198, 136,  92,  79,  91, // 4152-4159
122, 173, 230, 263, 238, 157,  64,  13, // 4160-4167
 14,  41,  70, 107, 172, 256, 307, 276, // 4168-4175
170,  50, -18, -25, -11, -17, -43, -57, // 4176-4183
-33,  15,  69, 110, 127, 109,  50, -34, // 4184-4191
-108, -143, -138, -116, -89, -50,  17, 107, // 4192-4199
186, 220, 200, 145,  76,  -4, -87, -140, // 4200-4207
-120, -23,  85, 126,  73, -19, -75, -58, // 4208-4215
 -2,  31,  18, -24, -69, -106, -143, -183, // 4216-4223
-215, -225, -206, -165, -115, -66, -42, -71, // 4224-4231
-160, -275, -362, -384, -344, -276, -211, -161, // 4232-4239
-134, -147, -209, -297, -355, -339, -259, -176, // 4240-4247
-137, -139, -143, -122, -86, -59, -49, -35, // 4248-4255
  1,  42,  52,  19, -27, -47, -15,  57, // 4256-4263
145, 227, 294, 340, 354, 341, 326, 333, // 4264-4271
360, 376, 354, 311, 286, 307, 356, 395, // 4272-4279
396, 365, 339, 353, 425, 555, 724, 899, // 4280-4287
1030, 1076, 1031, 925, 796, 650, 474, 262, // 4288-4295
 44, -130, -234, -274, -283, -288, -288, -275, // 4296-4303
-254, -244, -249, -248, -208, -126, -28,  44, // 4304-4311
 73,  76,  84, 106, 126, 124,  95,  51, // 4312-4319
 15,  -1,  -8, -23, -52, -94, -138, -173, // 4320-4327
-193, -200, -203, -207, -213, -213, -203, -196, // 4328-4335
-209, -251, -310, -357, -369, -354, -342, -359, // 4336-4343
-402, -446, -468, -464, -455, -456, -466, -473, // 4344-4351
-470, -459, -443, -420, -378, -314, -246, -204, // 4352-4359
-200, -222, -246, -259, -272, -302, -353, -402, // 4360-4367
-427, -413, -364, -301, -244, -204, -190, -199, // 4368-4375
-201, -167, -95,  -7,  68, 111, 118, 110, // 4376-4383
110, 121, 138, 156, 180, 209, 241, 264, // 4384-4391
278, 290, 298, 285, 247, 200, 166, 157, // 4392-4399
163, 168, 162, 158, 168, 186, 194, 185, // 4400-4407
163, 147, 153, 177, 191, 181, 155, 127, // 4408-4415
105,  91,  90, 105, 138, 175, 196, 199, // 4416-4423
196, 195, 199, 204, 202, 181, 151, 120, // 4424-4431
 91,  68,  66,  82, 102, 115, 116, 108, // 4432-4439
103, 110, 125, 136, 133, 127, 136, 152, // 4440-4447
141,  96,  44,   2, -33, -68, -99, -117, // 4448-4455
-109, -77, -40, -11,  14,  39,  52,  48, // 4456-4463
 35,  22,   8, -14, -41, -60, -78, -105, // 4464-4471
-124, -128, -136, -150, -141, -110, -90, -85, // 4472-4479
-73, -51, -37, -45, -64, -74, -78, -101, // 4480-4487
-140, -159, -145, -137, -157, -175, -160, -129, // 4488-4495
-123, -156, -193, -193, -172, -164, -164, -150, // 4496-4503
-127, -115, -111, -102, -87, -75, -73, -75, // 4504-4511
-71, -68, -62, -36,  -5,  -6, -31, -38, // 4512-4519
-16,  14,  32,  17, -28, -54, -26,   8, // 4520-4527
 -9, -56, -77, -62, -42, -31, -11,  24, // 4528-4535
 47,  45,  50,  83, 122, 152, 192, 235, // 4536-4543
256, 244, 214, 187, 160, 123, 101, 127, // 4544-4551
165, 145,  85,  68, 101, 129, 130, 133, // 4552-4559
158, 185, 174, 132, 134, 202, 232, 171, // 4560-4567
116, 141, 180, 163, 137, 148, 162, 147, // 4568-4575
122,  91,  37,   6,  36,  59,  15, -14, // 4576-4583
 32,  50, -21, -75, -57, -53, -59,  17, // 4584-4591
108,  75, -32, -74, -47, -16,  23,  39, // 4592-4599
-19, -82, -42,  33,  25, -12,   2,  10, // 4600-4607
-52, -93, -72, -94, -156, -128, -53, -109, // 4608-4615
-204, -133,  -2, -24, -93, -29,  34, -26, // 4616-4623
-63,   8,  41,  -4,   3,  35, -33, -146, // 4624-4631
-151, -88, -97, -168, -213, -206, -129,  26, // 4632-4639
129,  47, -94, -105, -59, -92, -91,  12, // 4640-4647
 41, -44, -31,  97, 113,  20,  13, 100, // 4648-4655
154, 159, 159, 147, 138, 152, 147,  79, // 4656-4663
 23,  74, 170, 148,   8, -67,  -8,  52, // 4664-4671
 47,  45,  94, 168, 197, 124,  46, 104, // 4672-4679
205, 134, -44, -99, -40,  27, 129, 247, // 4680-4687
242, 122,  44,  33,   0, -68, -103, -65, // 4688-4695
  0,  30,   3, -46, -93, -150, -219, -225, // 4696-4703
-98,  65,  40, -171, -255, -79,  92,  56, // 4704-4711
-45, -79, -125, -165, -75,  57,  18, -136, // 4712-4719
-164, -69, -19, -34, -20,  18,  31,  49, // 4720-4727
 70,  19, -81, -96, -51, -100, -163, -40, // 4728-4735
139,  57, -215, -323, -221, -114, -128, -250, // 4736-4743
-343, -225,  -7,  -2, -183, -207, -44,  -1, // 4744-4751
-115, -160, -72, -15, -73, -121, -51,  38, // 4752-4759
 20, -60, -124, -171, -140,  -2,  98,  74, // 4760-4767
 48,  70,  81, 138, 246, 225,  32, -119, // 4768-4775
-99,  38, 204, 268, 133, -63, -62, 150, // 4776-4783
304, 188, -56, -161, -94,  47, 205, 250, // 4784-4791
122,  27,   9, -183, -385, -183, 157,  69, // 4792-4799
-181, -119,   8, -35,   2, 142,  84, -26, // 4800-4807
 48, 116,   0, -101, -75, -42, -20,  31, // 4808-4815
 41, -40, -110, -61,  66, 152, 168, 165, // 4816-4823
116,  11,   0, 104, 128,  33,   0, -23, // 4824-4831
-159, -212, -19, 187, 223, 208, 129, -68, // 4832-4839
-139,  79, 255, 137, -38, -108, -142, -93, // 4840-4847
 63,  56, -157, -185,  64, 188,  28, -127, // 4848-4855
-137, -57,  89, 158, -13, -189, -57, 155, // 4856-4863
119, -53, -143, -216, -281, -186, -23, -17, // 4864-4871
-93,  -4, 180, 225, 133, 113, 198, 186, // 4872-4879
-19, -160, -62, 101, 106,  98, 213, 282, // 4880-4887
144};// 4888-4888
