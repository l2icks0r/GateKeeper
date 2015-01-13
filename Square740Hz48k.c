/**********************************************************************
* Written by WAVToCode
* Date:				Sun Nov 16 04:21:05 PM
* FileName:			Square740Hz48k.c
* Interleaved:		Yes
* Signed:			Yes
* No. of channels:	1
* No. of samples:	3828
* Bits/Sample:		16
**********************************************************************/

#include "stm32f4xx.h"

#define SQUARE_740HZ_SIZE 3828

static const int16_t Square740HzData[SQUARE_740HZ_SIZE] = {
  0, 21420, 26610, 26178, 26317, 26256, 26287, 26269, // 0-7
26281, 26272, 26279, 26274, 26278, 26274, 26277, 26276, // 8-15
26276, 26276, 26277, 26273, 26279, 26274, 26278, 26275, // 16-23
26276, 26275, 26278, 26272, 26286, 26247, 26384, 25359, // 24-31
10918, -13931, -25962, -26300, -26273, -26277, -26275, -26277, // 32-39
-26275, -26277, -26274, -26279, -26273, -26279, -26274, -26276, // 40-47
-26277, -26275, -26277, -26275, -26277, -26275, -26277, -26275, // 48-55
-26276, -26278, -26270, -26287, -26257, -26313, -26190, -26560, // 56-63
-19468, 3541, 23012, 26581, 26185, 26314, 26257, 26286, // 64-71
26271, 26279, 26274, 26277, 26275, 26277, 26275, 26276, // 72-79
26278, 26273, 26278, 26275, 26275, 26279, 26273, 26278, // 80-87
26274, 26278, 26273, 26283, 26262, 26304, 26212, 26482, // 88-95
24481, 7665, -16646, -26342, -26234, -26297, -26262, -26287, // 96-103
-26268, -26282, -26272, -26278, -26274, -26278, -26274, -26279, // 104-111
-26273, -26278, -26274, -26277, -26276, -26276, -26277, -26274, // 112-119
-26278, -26274, -26278, -26274, -26281, -26265, -26299, -26224, // 120-127
-26395, -17149, 6988, 24274, 26495, 26212, 26303, 26261, // 128-135
26285, 26271, 26279, 26274, 26277, 26274, 26280, 26271, // 136-143
26280, 26273, 26278, 26275, 26277, 26273, 26281, 26270, // 144-151
26283, 26268, 26284, 26267, 26289, 26255, 26315, 26188, // 152-159
26565, 23294, 4237, -19029, -26542, -26192, -26313, -26257, // 160-167
-26286, -26270, -26281, -26272, -26279, -26273, -26279, -26273, // 168-175
-26279, -26274, -26277, -26275, -26277, -26275, -26277, -26276, // 176-183
-26274, -26280, -26271, -26281, -26272, -26279, -26274, -26277, // 184-191
-26286, -26055, -14497, 10282, 25209, 26401, 26242, 26288, // 192-199
26272, 26277, 26275, 26278, 26273, 26279, 26274, 26277, // 200-207
26275, 26277, 26275, 26277, 26275, 26277, 26274, 26279, // 208-215
26273, 26279, 26272, 26282, 26266, 26292, 26250, 26323, // 216-223
26172, 26615, 21762, 714, -21059, -26609, -26178, -26317, // 224-231
-26255, -26288, -26269, -26280, -26274, -26276, -26277, -26275, // 232-239
-26277, -26275, -26277, -26275, -26276, -26277, -26275, -26277, // 240-247
-26276, -26274, -26279, -26273, -26279, -26274, -26275, -26283, // 248-255
-26253, -26366, -25499, -11543, 13350, 25862, 26315, 26270, // 256-263
26275, 26278, 26274, 26277, 26277, 26274, 26278, 26275, // 264-271
26275, 26279, 26272, 26280, 26273, 26278, 26274, 26278, // 272-279
26275, 26276, 26277, 26273, 26281, 26269, 26287, 26257, // 280-287
26313, 26187, 26579, 19886, -2835, -22725, -26588, -26184, // 288-295
-26315, -26255, -26290, -26265, -26285, -26269, -26281, -26273, // 296-303
-26277, -26276, -26276, -26276, -26276, -26275, -26278, -26274, // 304-311
-26278, -26273, -26279, -26274, -26279, -26270, -26285, -26262, // 312-319
-26301, -26221, -26459, -24683, -8330, 16128, 26281, 26246, // 320-327
26291, 26269, 26279, 26274, 26279, 26272, 26280, 26272, // 328-335
26279, 26274, 26278, 26274, 26278, 26274, 26277, 26276, // 336-343
26276, 26277, 26274, 26278, 26273, 26281, 26269, 26286, // 344-351
26260, 26305, 26214, 26439, 17643, -6312, -24045, -26516, // 352-359
-26205, -26305, -26262, -26284, -26270, -26281, -26272, -26279, // 360-367
-26274, -26277, -26275, -26278, -26274, -26277, -26276, -26275, // 368-375
-26277, -26276, -26275, -26277, -26274, -26280, -26270, -26285, // 376-383
-26260, -26308, -26198, -26547, -23558, -4935, 18585, 26508, // 384-391
26203, 26307, 26260, 26286, 26269, 26280, 26274, 26277, // 392-399
26275, 26277, 26275, 26277, 26275, 26277, 26275, 26278, // 400-407
26273, 26278, 26276, 26275, 26278, 26273, 26279, 26273, // 408-415
26280, 26270, 26284, 26271, 26137, 15058, -9647, -25038, // 416-423
-26427, -26230, -26295, -26266, -26283, -26271, -26279, -26274, // 424-431
-26277, -26276, -26277, -26273, -26280, -26272, -26280, -26273, // 432-439
-26277, -26276, -26277, -26274, -26279, -26271, -26282, -26269, // 440-447
-26287, -26257, -26314, -26182, -26603, -22102, -1419, 20680, // 448-455
26606, 26176, 26320, 26252, 26291, 26266, 26284, 26269, // 456-463
26282, 26271, 26280, 26272, 26279, 26274, 26278, 26274, // 464-471
26277, 26275, 26277, 26276, 26275, 26277, 26275, 26277, // 472-479
26277, 26271, 26286, 26253, 26353, 25628, 12156, -12759, // 480-487
-25749, -26334, -26261, -26281, -26275, -26275, -26277, -26275, // 488-495
-26277, -26275, -26277, -26275, -26277, -26274, -26279, -26273, // 496-503
-26278, -26275, -26276, -26277, -26274, -26279, -26272, -26281, // 504-511
-26269, -26286, -26259, -26313, -26183, -26594, -20289, 2126, // 512-519
22421, 26598, 26180, 26317, 26256, 26285, 26272, 26279, // 520-527
26273, 26279, 26272, 26280, 26272, 26281, 26271, 26280, // 528-535
26273, 26278, 26274, 26279, 26273, 26279, 26272, 26281, // 536-543
26269, 26286, 26262, 26299, 26225, 26443, 24866, 8994, // 544-551
-15601, -26212, -26258, -26289, -26267, -26282, -26273, -26277, // 552-559
-26276, -26275, -26276, -26278, -26274, -26277, -26275, -26276, // 560-567
-26278, -26273, -26279, -26272, -26281, -26272, -26279, -26273, // 568-575
-26279, -26272, -26283, -26262, -26305, -26208, -26478, -18119, // 576-583
5626, 23806, 26536, 26197, 26310, 26259, 26284, 26273, // 584-591
26277, 26276, 26275, 26276, 26278, 26273, 26279, 26274, // 592-599
26276, 26278, 26273, 26278, 26276, 26275, 26277, 26275, // 600-607
26278, 26272, 26283, 26262, 26306, 26203, 26529, 23811, // 608-615
5624, -18119, -26478, -26207, -26308, -26257, -26290, -26266, // 616-623
-26282, -26272, -26278, -26276, -26276, -26276, -26275, -26277, // 624-631
-26275, -26277, -26276, -26275, -26277, -26275, -26277, -26275, // 632-639
-26276, -26277, -26273, -26282, -26267, -26289, -26257, -26214, // 640-647
-15599, 8993, 24865, 26446, 26222, 26301, 26263, 26283, // 648-655
26272, 26279, 26273, 26279, 26273, 26279, 26272, 26281, // 656-663
26270, 26282, 26271, 26279, 26275, 26276, 26275, 26278, // 664-671
26273, 26281, 26268, 26288, 26256, 26315, 26183, 26596, // 672-679
22420, 2130, -20294, -26589, -26187, -26312, -26258, -26286, // 680-687
-26270, -26280, -26273, -26278, -26275, -26277, -26275, -26277, // 688-695
-26274, -26279, -26274, -26277, -26276, -26275, -26277, -26275, // 696-703
-26277, -26275, -26277, -26275, -26276, -26279, -26264, -26331, // 704-711
-25750, -12760, 12158, 25627, 26352, 26255, 26284, 26273, // 712-719
26276, 26277, 26275, 26276, 26277, 26274, 26278, 26275, // 720-727
26276, 26277, 26275, 26276, 26277, 26275, 26277, 26275, // 728-735
26277, 26274, 26281, 26267, 26289, 26255, 26316, 26181, // 736-743
26602, 20682, -1420, -22101, -26605, -26178, -26319, -26252, // 744-751
-26292, -26264, -26285, -26269, -26281, -26273, -26278, -26274, // 752-759
-26278, -26274, -26279, -26272, -26280, -26272, -26279, -26275, // 760-767
-26276, -26276, -26277, -26272, -26283, -26265, -26297, -26229, // 768-775
-26427, -25038, -9647, 15059, 26134, 26275, 26279, 26275, // 776-783
26277, 26275, 26276, 26276, 26276, 26276, 26276, 26276, // 784-791
26276, 26276, 26276, 26276, 26276, 26275, 26278, 26274, // 792-799
26277, 26277, 26272, 26282, 26268, 26288, 26257, 26310, // 800-807
26199, 26514, 18579, -4930, -23562, -26546, -26196, -26311, // 808-815
-26256, -26289, -26267, -26283, -26270, -26281, -26271, -26281, // 816-823
-26271, -26281, -26271, -26281, -26271, -26281, -26272, -26278, // 824-831
-26276, -26274, -26279, -26274, -26276, -26280, -26263, -26306, // 832-839
-26204, -26516, -24047, -6308, 17638, 26442, 26214, 26304, // 840-847
26262, 26283, 26273, 26277, 26275, 26278, 26273, 26279, // 848-855
26275, 26275, 26277, 26276, 26274, 26280, 26271, 26281, // 856-863
26271, 26280, 26274, 26277, 26276, 26274, 26280, 26268, // 864-871
26293, 26245, 26281, 16129, -8332, -24680, -26461, -26221, // 872-879
-26300, -26263, -26284, -26270, -26281, -26272, -26279, -26274, // 880-887
-26278, -26274, -26277, -26276, -26275, -26278, -26274, -26277, // 888-895
-26276, -26275, -26278, -26272, -26283, -26266, -26290, -26254, // 896-903
-26316, -26184, -26588, -22725, -2834, 19884, 26582, 26182, // 904-911
26319, 26252, 26291, 26267, 26281, 26273, 26277, 26277, // 912-919
26274, 26278, 26275, 26275, 26279, 26272, 26280, 26272, // 920-927
26279, 26274, 26277, 26276, 26276, 26275, 26278, 26274, // 928-935
26278, 26269, 26315, 25861, 13353, -11546, -25497, -26367, // 936-943
-26253, -26283, -26273, -26277, -26276, -26276, -26277, -26274, // 944-951
-26278, -26274, -26278, -26275, -26276, -26277, -26275, -26276, // 952-959
-26276, -26277, -26274, -26279, -26272, -26280, -26271, -26285, // 960-967
-26257, -26317, -26177, -26611, -21054, 706, 21772, 26605, // 968-975
26180, 26318, 26252, 26292, 26266, 26281, 26274, 26276, // 976-983
26277, 26275, 26277, 26274, 26278, 26274, 26278, 26276, // 984-991
26273, 26280, 26272, 26280, 26273, 26278, 26273, 26281, // 992-999
26269, 26289, 26242, 26401, 25207, 10286, -14501, -26052, // 1000-1007
-26287, -26277, -26275, -26277, -26274, -26279, -26272, -26280, // 1008-1015
-26273, -26278, -26275, -26276, -26276, -26276, -26277, -26274, // 1016-1023
-26279, -26272, -26280, -26272, -26280, -26273, -26278, -26274, // 1024-1031
-26281, -26263, -26307, -26197, -26539, -19031, 4238, 23295, // 1032-1039
26562, 26192, 26311, 26259, 26285, 26271, 26279, 26274, // 1040-1047
26277, 26276, 26275, 26278, 26273, 26279, 26273, 26279, // 1048-1055
26274, 26277, 26275, 26277, 26276, 26274, 26280, 26269, // 1056-1063
26287, 26259, 26306, 26208, 26499, 24270, 6990, -17149, // 1064-1071
-26395, -26223, -26301, -26263, -26283, -26271, -26279, -26275, // 1072-1079
-26277, -26274, -26278, -26273, -26280, -26273, -26278, -26275, // 1080-1087
-26275, -26278, -26274, -26278, -26275, -26276, -26276, -26277, // 1088-1095
-26273, -26282, -26265, -26297, -26234, -26341, -16646, 7665, // 1096-1103
24481, 26481, 26213, 26304, 26261, 26285, 26271, 26279, // 1104-1111
26274, 26277, 26276, 26275, 26277, 26276, 26276, 26276, // 1112-1119
26276, 26275, 26277, 26277, 26274, 26278, 26273, 26280, // 1120-1127
26270, 26287, 26256, 26314, 26187, 26577, 23016, 3538, // 1128-1135
-19466, -26561, -26191, -26311, -26259, -26286, -26270, -26279, // 1136-1143
-26275, -26276, -26277, -26275, -26276, -26277, -26274, -26279, // 1144-1151
-26273, -26278, -26276, -26274, -26279, -26273, -26278, -26276, // 1152-1159
-26274, -26280, -26271, -26281, -26269, -26303, -25961, -13930, // 1160-1167
10918, 25358, 26385, 26246, 26287, 26273, 26275, 26278, // 1168-1175
26274, 26277, 26276, 26276, 26277, 26274, 26278, 26273, // 1176-1183
26280, 26273, 26278, 26275, 26276, 26276, 26277, 26273, // 1184-1191
26281, 26269, 26287, 26256, 26316, 26180, 26608, 21422, // 1192-1199
 -2, -21419, -26609, -26180, -26315, -26257, -26287, -26268, // 1200-1207
-26283, -26269, -26282, -26272, -26278, -26275, -26277, -26274, // 1208-1215
-26279, -26273, -26278, -26275, -26276, -26276, -26276, -26276, // 1216-1223
-26277, -26273, -26281, -26268, -26290, -26244, -26385, -25360, // 1224-1231
-10916, 13927, 25967, 26296, 26276, 26274, 26277, 26276, // 1232-1239
26275, 26278, 26273, 26279, 26274, 26277, 26276, 26275, // 1240-1247
26277, 26275, 26278, 26274, 26277, 26276, 26274, 26281, // 1248-1255
26269, 26284, 26266, 26289, 26256, 26314, 26188, 26564, // 1256-1263
19464, -3538, -23015, -26579, -26185, -26315, -26256, -26287, // 1264-1271
-26270, -26279, -26275, -26276, -26276, -26277, -26273, -26280, // 1272-1279
-26272, -26279, -26275, -26275, -26278, -26274, -26277, -26276, // 1280-1287
-26275, -26279, -26271, -26283, -26264, -26300, -26217, -26479, // 1288-1295
-24481, -7667, 16650, 26338, 26235, 26297, 26264, 26284, // 1296-1303
26270, 26281, 26270, 26284, 26268, 26282, 26272, 26278, // 1304-1311
26276, 26275, 26277, 26275, 26278, 26273, 26279, 26272, // 1312-1319
26281, 26272, 26280, 26270, 26284, 26263, 26301, 26223, // 1320-1327
26394, 17151, -6992, -24268, -26500, -26210, -26301, -26267, // 1328-1335
-26279, -26274, -26278, -26273, -26280, -26272, -26281, -26270, // 1336-1343
-26282, -26271, -26279, -26274, -26278, -26274, -26279, -26272, // 1344-1351
-26280, -26272, -26281, -26268, -26289, -26255, -26315, -26187, // 1352-1359
-26568, -23289, -4242, 19033, 26540, 26192, 26314, 26255, // 1360-1367
26289, 26268, 26281, 26272, 26279, 26274, 26278, 26273, // 1368-1375
26280, 26272, 26280, 26272, 26280, 26272, 26279, 26274, // 1376-1383
26278, 26274, 26277, 26275, 26276, 26279, 26272, 26278, // 1384-1391
26286, 26054, 14500, -10286, -25206, -26404, -26238, -26292, // 1392-1399
-26268, -26281, -26272, -26280, -26272, -26279, -26274, -26277, // 1400-1407
-26276, -26276, -26275, -26277, -26275, -26278, -26273, -26279, // 1408-1415
-26273, -26278, -26276, -26276, -26274, -26283, -26259, -26314, // 1416-1423
-26181, -26607, -21769, -710, 21059, 26607, 26180, 26316, // 1424-1431
26256, 26287, 26269, 26281, 26272, 26280, 26272, 26279, // 1432-1439
26273, 26279, 26273, 26279, 26273, 26278, 26275, 26276, // 1440-1447
26277, 26275, 26276, 26276, 26277, 26275, 26275, 26282, // 1448-1455
26254, 26365, 25501, 11540, -13348, -25862, -26316, -26267, // 1456-1463
-26279, -26275, -26277, -26274, -26277, -26277, -26274, -26279, // 1464-1471
-26272, -26280, -26273, -26278, -26275, -26276, -26277, -26275, // 1472-1479
-26277, -26275, -26277, -26274, -26281, -26267, -26291, -26251, // 1480-1487
-26320, -26180, -26586, -19880, 2831, 22728, 26586, 26185, // 1488-1495
26314, 26257, 26287, 26268, 26282, 26271, 26281, 26271, // 1496-1503
26280, 26272, 26280, 26272, 26280, 26273, 26278, 26275, // 1504-1511
26276, 26276, 26277, 26274, 26279, 26272, 26282, 26266, // 1512-1519
26297, 26223, 26459, 24682, 8331, -16127, -26283, -26245, // 1520-1527
-26293, -26265, -26284, -26270, -26280, -26274, -26276, -26277, // 1528-1535
-26275, -26276, -26277, -26275, -26276, -26277, -26274, -26278, // 1536-1543
-26274, -26279, -26273, -26278, -26273, -26280, -26271, -26285, // 1544-1551
-26259, -26307, -26212, -26441, -17642, 6313, 24043, 26517, // 1552-1559
26206, 26302, 26266, 26281, 26271, 26281, 26272, 26278, // 1560-1567
26276, 26275, 26277, 26275, 26277, 26276, 26275, 26277, // 1568-1575
26275, 26276, 26277, 26275, 26276, 26278, 26271, 26285, // 1576-1583
26259, 26311, 26194, 26550, 23556, 4937, -18585, -26509, // 1584-1591
-26203, -26307, -26259, -26288, -26266, -26284, -26270, -26280, // 1592-1599
-26275, -26275, -26277, -26275, -26278, -26274, -26277, -26276, // 1600-1607
-26276, -26276, -26276, -26275, -26277, -26276, -26276, -26275, // 1608-1615
-26278, -26272, -26282, -26273, -26136, -15057, 9644, 25042, // 1616-1623
26423, 26233, 26294, 26267, 26281, 26273, 26278, 26274, // 1624-1631
26278, 26275, 26276, 26277, 26274, 26278, 26274, 26277, // 1632-1639
26276, 26276, 26276, 26275, 26278, 26273, 26280, 26269, // 1640-1647
26289, 26253, 26320, 26175, 26609, 22096, 1425, -20686, // 1648-1655
-26599, -26182, -26316, -26256, -26287, -26269, -26280, -26273, // 1656-1663
-26279, -26274, -26276, -26277, -26275, -26276, -26278, -26272, // 1664-1671
-26280, -26273, -26278, -26275, -26277, -26274, -26278, -26274, // 1672-1679
-26279, -26272, -26283, -26257, -26349, -25630, -12156, 12759, // 1680-1687
25750, 26332, 26263, 26280, 26275, 26276, 26276, 26276, // 1688-1695
26276, 26276, 26276, 26276, 26276, 26276, 26277, 26274, // 1696-1703
26278, 26274, 26278, 26275, 26275, 26279, 26272, 26282, // 1704-1711
26267, 26288, 26257, 26314, 26185, 26591, 20292, -2130, // 1712-1719
-22418, -26599, -26181, -26315, -26258, -26285, -26270, -26282, // 1720-1727
-26270, -26281, -26273, -26277, -26276, -26275, -26277, -26275, // 1728-1735
-26278, -26273, -26279, -26273, -26278, -26276, -26275, -26278, // 1736-1743
-26272, -26283, -26264, -26298, -26226, -26443, -24867, -8991, // 1744-1751
15599, 26212, 26259, 26287, 26269, 26282, 26271, 26279, // 1752-1759
26274, 26277, 26276, 26276, 26275, 26277, 26275, 26278, // 1760-1767
26274, 26277, 26276, 26275, 26278, 26273, 26280, 26271, // 1768-1775
26282, 26269, 26285, 26261, 26306, 26206, 26481, 18117, // 1776-1783
-5624, -23809, -26533, -26199, -26309, -26258, -26288, -26267, // 1784-1791
-26283, -26271, -26279, -26275, -26276, -26277, -26274, -26278, // 1792-1799
-26275, -26277, -26276, -26275, -26277, -26275, -26277, -26275, // 1800-1807
-26278, -26271, -26285, -26260, -26307, -26201, -26532, -23808, // 1808-1815
-5626, 18120, 26478, 26207, 26307, 26259, 26287, 26268, // 1816-1823
26281, 26273, 26278, 26275, 26276, 26276, 26275, 26278, // 1824-1831
26274, 26278, 26275, 26275, 26278, 26274, 26277, 26276, // 1832-1839
26276, 26276, 26275, 26278, 26271, 26287, 26259, 26212, // 1840-1847
15600, -8993, -24867, -26441, -26228, -26295, -26267, -26282, // 1848-1855
-26271, -26280, -26272, -26280, -26273, -26277, -26276, -26276, // 1856-1863
-26275, -26279, -26271, -26281, -26272, -26279, -26274, -26277, // 1864-1871
-26274, -26281, -26267, -26291, -26251, -26320, -26180, -26596, // 1872-1879
-22424, -2123, 20286, 26596, 26181, 26317, 26254, 26291, // 1880-1887
26265, 26283, 26272, 26278, 26276, 26275, 26277, 26275, // 1888-1895
26277, 26275, 26276, 26277, 26275, 26277, 26274, 26278, // 1896-1903
26275, 26277, 26275, 26276, 26275, 26280, 26263, 26332, // 1904-1911
25751, 12756, -12152, -25634, -26345, -26260, -26282, -26273, // 1912-1919
-26278, -26273, -26280, -26272, -26280, -26273, -26277, -26277, // 1920-1927
-26273, -26280, -26272, -26279, -26275, -26275, -26278, -26273, // 1928-1935
-26280, -26271, -26282, -26268, -26288, -26256, -26316, -26179, // 1936-1943
-26605, -20679, 1418, 22102, 26604, 26179, 26318, 26254, // 1944-1951
26289, 26267, 26282, 26272, 26279, 26274, 26276, 26277, // 1952-1959
26274, 26278, 26275, 26276, 26276, 26276, 26276, 26276, // 1960-1967
26277, 26273, 26280, 26271, 26284, 26264, 26295, 26232, // 1968-1975
26424, 25042, 9643, -15056, -26138, -26270, -26284, -26271, // 1976-1983
-26279, -26275, -26275, -26278, -26274, -26278, -26275, -26275, // 1984-1991
-26278, -26274, -26278, -26275, -26275, -26278, -26274, -26277, // 1992-1999
-26276, -26276, -26275, -26280, -26267, -26290, -26256, -26310, // 2000-2007
-26201, -26510, -18584, 4936, 23555, 26552, 26192, 26313, // 2008-2015
26257, 26286, 26270, 26280, 26274, 26278, 26273, 26279, // 2016-2023
26273, 26279, 26274, 26277, 26275, 26277, 26275, 26278, // 2024-2031
26273, 26279, 26273, 26279, 26273, 26281, 26265, 26302, // 2032-2039
26207, 26515, 24046, 6312, -17643, -26440, -26213, -26306, // 2040-2047
-26258, -26288, -26268, -26281, -26273, -26277, -26277, -26274, // 2048-2055
-26278, -26274, -26278, -26274, -26278, -26273, -26280, -26273, // 2056-2063
-26277, -26276, -26276, -26275, -26279, -26271, -26282, -26269, // 2064-2071
-26288, -26250, -26277, -16132, 8332, 24682, 26460, 26222, // 2072-2079
26298, 26264, 26284, 26271, 26279, 26273, 26279, 26274, // 2080-2087
26278, 26273, 26279, 26273, 26280, 26271, 26281, 26271, // 2088-2095
26280, 26273, 26279, 26272, 26282, 26267, 26290, 26253, // 2096-2103
26318, 26182, 26589, 22725, 2834, -19885, -26580, -26185, // 2104-2111
-26316, -26255, -26288, -26269, -26280, -26274, -26277, -26276, // 2112-2119
-26275, -26278, -26273, -26279, -26273, -26279, -26273, -26279, // 2120-2127
-26273, -26278, -26275, -26276, -26277, -26275, -26277, -26275, // 2128-2135
-26278, -26268, -26317, -25860, -13351, 11544, 25496, 26371, // 2136-2143
26246, 26290, 26269, 26279, 26276, 26274, 26278, 26274, // 2144-2151
26278, 26275, 26276, 26276, 26276, 26276, 26276, 26277, // 2152-2159
26273, 26281, 26270, 26282, 26270, 26282, 26268, 26289, // 2160-2167
26255, 26315, 26181, 26606, 21060, -711, -21769, -26606, // 2168-2175
-26182, -26313, -26259, -26285, -26270, -26281, -26271, -26281, // 2176-2183
-26271, -26281, -26272, -26279, -26273, -26279, -26274, -26278, // 2184-2191
-26274, -26277, -26276, -26275, -26277, -26276, -26274, -26280, // 2192-2199
-26268, -26291, -26240, -26403, -25206, -10285, 14498, 26056, // 2200-2207
26283, 26281, 26271, 26280, 26273, 26277, 26277, 26274, // 2208-2215
26278, 26275, 26275, 26278, 26274, 26277, 26277, 26273, // 2216-2223
26280, 26271, 26281, 26272, 26280, 26272, 26279, 26271, // 2224-2231
26286, 26258, 26312, 26192, 26542, 19030, -4238, -23294, // 2232-2239
-26564, -26190, -26312, -26258, -26286, -26271, -26279, -26273, // 2240-2247
-26279, -26273, -26279, -26274, -26277, -26275, -26277, -26275, // 2248-2255
-26277, -26275, -26278, -26272, -26281, -26271, -26280, -26273, // 2256-2263
-26281, -26264, -26303, -26209, -26500, -24268, -6993, 17152, // 2264-2271
26393, 26224, 26302, 26260, 26287, 26267, 26283, 26272, // 2272-2279
26278, 26275, 26276, 26277, 26274, 26279, 26272, 26280, // 2280-2287
26273, 26278, 26275, 26276, 26276, 26276, 26277, 26275, // 2288-2295
26276, 26277, 26271, 26291, 26239, 26337, 16649, -7666, // 2296-2303
-24481, -26481, -26214, -26303, -26261, -26285, -26271, -26279, // 2304-2311
-26274, -26277, -26275, -26278, -26273, -26279, -26273, -26279, // 2312-2319
-26273, -26279, -26273, -26278, -26275, -26277, -26274, -26280, // 2320-2327
-26269, -26288, -26255, -26315, -26186, -26578, -23016, -3538, // 2328-2335
19466, 26561, 26190, 26313, 26257, 26286, 26271, 26278, // 2336-2343
26276, 26275, 26277, 26275, 26277, 26275, 26277, 26275, // 2344-2351
26277, 26276, 26274, 26278, 26275, 26276, 26278, 26273, // 2352-2359
26278, 26275, 26276, 26277, 26272, 26300, 25964, 13928, // 2360-2367
-10917, -25358, -26388, -26240, -26294, -26266, -26281, -26274, // 2368-2375
-26276, -26276, -26277, -26274, -26278, -26275, -26276, -26277, // 2376-2383
-26274, -26278, -26275, -26276, -26277, -26274, -26279, -26272, // 2384-2391
-26282, -26267, -26289, -26254, -26318, -26178, -26610, -21420, // 2392-2399
  0, 21421, 26608, 26181, 26314, 26257, 26287, 26270, // 2400-2407
26279, 26275, 26275, 26277, 26276, 26276, 26276, 26275, // 2408-2415
26277, 26276, 26275, 26277, 26275, 26277, 26276, 26275, // 2416-2423
26277, 26275, 26278, 26272, 26286, 26247, 26384, 25358, // 2424-2431
10920, -13932, -25961, -26302, -26272, -26276, -26276, -26276, // 2432-2439
-26277, -26274, -26278, -26274, -26278, -26275, -26275, -26277, // 2440-2447
-26276, -26276, -26276, -26276, -26275, -26279, -26271, -26281, // 2448-2455
-26272, -26280, -26271, -26284, -26259, -26312, -26190, -26562, // 2456-2463
-19465, 3538, 23016, 26576, 26189, 26311, 26260, 26284, // 2464-2471
26272, 26277, 26276, 26276, 26276, 26277, 26274, 26278, // 2472-2479
26275, 26276, 26277, 26274, 26279, 26273, 26277, 26277, // 2480-2487
26273, 26281, 26270, 26284, 26262, 26302, 26215, 26481, // 2488-2495
24481, 7665, -16648, -26339, -26236, -26295, -26266, -26282, // 2496-2503
-26272, -26279, -26274, -26278, -26274, -26277, -26276, -26275, // 2504-2511
-26277, -26276, -26276, -26276, -26275, -26277, -26275, -26277, // 2512-2519
-26276, -26275, -26278, -26272, -26283, -26263, -26301, -26223, // 2520-2527
-26394, -17151, 6991, 24271, 26496, 26212, 26303, 26262, // 2528-2535
26285, 26269, 26280, 26275, 26275, 26278, 26275, 26275, // 2536-2543
26278, 26274, 26277, 26276, 26275, 26277, 26276, 26276, // 2544-2551
26276, 26274, 26281, 26268, 26289, 26255, 26314, 26190, // 2552-2559
26563, 23296, 4235, -19028, -26542, -26193, -26312, -26258, // 2560-2567
-26286, -26269, -26282, -26271, -26281, -26271, -26280, -26273, // 2568-2575
-26278, -26275, -26276, -26277, -26274, -26279, -26272, -26281, // 2576-2583
-26271, -26280, -26273, -26278, -26274, -26278, -26274, -26277, // 2584-2591
-26286, -26054, -14499, 10285, 25206, 26404, 26240, 26289, // 2592-2599
26271, 26277, 26277, 26275, 26277, 26275, 26276, 26276, // 2600-2607
26277, 26275, 26276, 26277, 26274, 26279, 26273, 26279, // 2608-2615
26273, 26280, 26271, 26282, 26268, 26288, 26256, 26316, // 2616-2623
26179, 26608, 21768, 711, -21059, -26607, -26180, -26316, // 2624-2631
-26255, -26289, -26267, -26282, -26273, -26277, -26275, -26278, // 2632-2639
-26273, -26280, -26272, -26278, -26276, -26275, -26277, -26275, // 2640-2647
-26276, -26277, -26274, -26279, -26272, -26280, -26271, -26285, // 2648-2655
-26253, -26365, -25501, -11540, 13348, 25862, 26316, 26267, // 2656-2663
26279, 26275, 26276, 26276, 26276, 26276, 26276, 26276, // 2664-2671
26276, 26276, 26276, 26276, 26276, 26275, 26279, 26272, // 2672-2679
26280, 26272, 26279, 26273, 26281, 26268, 26288, 26256, // 2680-2687
26314, 26186, 26581, 19883, -2832, -22728, -26586, -26185, // 2688-2695
-26313, -26259, -26285, -26271, -26279, -26274, -26276, -26277, // 2696-2703
-26275, -26277, -26275, -26276, -26277, -26275, -26277, -26274, // 2704-2711
-26279, -26273, -26279, -26273, -26279, -26271, -26284, -26263, // 2712-2719
-26300, -26221, -26460, -24680, -8334, 16131, 26279, 26247, // 2720-2727
26292, 26266, 26284, 26269, 26281, 26273, 26277, 26277, // 2728-2735
26275, 26276, 26277, 26274, 26278, 26276, 26274, 26279, // 2736-2743
26272, 26280, 26273, 26278, 26274, 26279, 26271, 26285, // 2744-2751
26261, 26304, 26213, 26443, 17637, -6305, -24052, -26511, // 2752-2759
-26207, -26306, -26260, -26285, -26271, -26279, -26273, -26279, // 2760-2767
-26273, -26279, -26274, -26276, -26277, -26274, -26280, -26272, // 2768-2775
-26278, -26275, -26276, -26277, -26276, -26276, -26273, -26284, // 2776-2783
-26259, -26311, -26195, -26548, -23559, -4932, 18580, 26514, // 2784-2791
26198, 26311, 26257, 26288, 26268, 26282, 26271, 26279, // 2792-2799
26274, 26278, 26275, 26276, 26275, 26277, 26276, 26275, // 2800-2807
26279, 26271, 26282, 26270, 26281, 26272, 26280, 26272, // 2808-2815
26281, 26269, 26285, 26269, 26140, 15054, -9642, -25043, // 2816-2823
-26423, -26232, -26296, -26264, -26284, -26270, -26279, -26275, // 2824-2831
-26277, -26275, -26277, -26274, -26278, -26274, -26278, -26275, // 2832-2839
-26276, -26276, -26276, -26275, -26279, -26271, -26283, -26266, // 2840-2847
-26290, -26255, -26315, -26183, -26600, -22104, -1419, 20682, // 2848-2855
26601, 26182, 26315, 26257, 26287, 26268, 26282, 26272, // 2856-2863
26279, 26274, 26277, 26276, 26276, 26276, 26276, 26275, // 2864-2871
26278, 26274, 26277, 26276, 26275, 26277, 26276, 26275, // 2872-2879
26278, 26273, 26282, 26258, 26349, 25630, 12156, -12760, // 2880-2887
-25748, -26335, -26259, -26284, -26272, -26277, -26277, -26274, // 2888-2895
-26278, -26274, -26278, -26274, -26278, -26274, -26277, -26277, // 2896-2903
-26274, -26278, -26273, -26279, -26274, -26278, -26273, -26281, // 2904-2911
-26268, -26288, -26256, -26314, -26185, -26591, -20292, 2128, // 2912-2919
22422, 26595, 26183, 26315, 26256, 26288, 26267, 26284, // 2920-2927
26268, 26283, 26271, 26278, 26276, 26276, 26276, 26276, // 2928-2935
26275, 26277, 26276, 26276, 26275, 26278, 26273, 26281, // 2936-2943
26268, 26286, 26263, 26298, 26227, 26441, 24868, 8992, // 2944-2951
-15600, -26212, -26259, -26287, -26269, -26280, -26275, -26276, // 2952-2959
-26277, -26274, -26277, -26276, -26276, -26276, -26277, -26273, // 2960-2967
-26280, -26272, -26279, -26275, -26276, -26275, -26278, -26273, // 2968-2975
-26281, -26269, -26285, -26262, -26303, -26212, -26473, -18123, // 2976-2983
5626, 23809, 26532, 26201, 26307, 26261, 26283, 26272, // 2984-2991
26279, 26273, 26280, 26272, 26278, 26275, 26276, 26278, // 2992-2999
26274, 26276, 26277, 26274, 26279, 26273, 26279, 26272, // 3000-3007
26281, 26269, 26287, 26258, 26309, 26200, 26531, 23812, // 3008-3015
5621, -18116, -26480, -26207, -26305, -26262, -26284, -26271, // 3016-3023
-26279, -26274, -26278, -26274, -26277, -26276, -26275, -26278, // 3024-3031
-26274, -26278, -26273, -26280, -26270, -26283, -26270, -26281, // 3032-3039
-26272, -26278, -26274, -26280, -26269, -26288, -26258, -26212, // 3040-3047
-15602, 8996, 24863, 26445, 26226, 26296, 26266, 26282, // 3048-3055
26271, 26281, 26272, 26279, 26274, 26276, 26278, 26273, // 3056-3063
26279, 26274, 26276, 26278, 26272, 26281, 26271, 26281, // 3064-3071
26271, 26282, 26267, 26289, 26256, 26314, 26185, 26593, // 3072-3079
22424, 2125, -20288, -26596, -26180, -26318, -26253, -26291, // 3080-3087
-26266, -26282, -26272, -26279, -26274, -26278, -26274, -26277, // 3088-3095
-26276, -26275, -26278, -26274, -26277, -26276, -26275, -26278, // 3096-3103
-26274, -26277, -26275, -26277, -26274, -26282, -26260, -26335, // 3104-3111
-25748, -12760, 12156, 25630, 26350, 26255, 26285, 26271, // 3112-3119
26278, 26277, 26274, 26277, 26275, 26277, 26276, 26275, // 3120-3127
26278, 26273, 26279, 26273, 26279, 26273, 26281, 26270, // 3128-3135
26280, 26272, 26282, 26268, 26289, 26253, 26318, 26179, // 3136-3143
26605, 20678, -1417, -22103, -26604, -26178, -26319, -26253, // 3144-3151
-26290, -26267, -26283, -26269, -26283, -26269, -26282, -26272, // 3152-3159
-26278, -26275, -26276, -26276, -26277, -26274, -26278, -26274, // 3160-3167
-26277, -26276, -26277, -26273, -26281, -26267, -26294, -26233, // 3168-3175
-26423, -25042, -9644, 15057, 26136, 26273, 26282, 26272, // 3176-3183
26278, 26275, 26277, 26275, 26277, 26275, 26276, 26277, // 3184-3191
26275, 26277, 26275, 26276, 26276, 26277, 26275, 26277, // 3192-3199
26275, 26276, 26276, 26278, 26271, 26285, 26259, 26310, // 3200-3207
26198, 26516, 18577, -4930, -23560, -26547, -26197, -26308, // 3208-3215
-26261, -26283, -26273, -26278, -26274, -26278, -26273, -26280, // 3216-3223
-26273, -26277, -26276, -26276, -26276, -26276, -26276, -26276, // 3224-3231
-26275, -26279, -26271, -26283, -26267, -26288, -26258, -26308, // 3232-3239
-26203, -26517, -24045, -6313, 17645, 26436, 26219, 26300, // 3240-3247
26264, 26282, 26272, 26280, 26272, 26280, 26272, 26279, // 3248-3255
26275, 26275, 26278, 26274, 26278, 26275, 26276, 26276, // 3256-3263
26276, 26276, 26276, 26276, 26276, 26275, 26279, 26269, // 3264-3271
26291, 26246, 26281, 16129, -8332, -24680, -26462, -26220, // 3272-3279
-26300, -26264, -26282, -26274, -26276, -26276, -26277, -26274, // 3280-3287
-26279, -26273, -26278, -26275, -26276, -26277, -26275, -26276, // 3288-3295
-26277, -26273, -26281, -26270, -26283, -26267, -26289, -26254, // 3296-3303
-26317, -26183, -26587, -22728, -2832, 19884, 26580, 26187, // 3304-3311
26312, 26258, 26287, 26268, 26283, 26269, 26282, 26272, // 3312-3319
26278, 26276, 26275, 26277, 26275, 26276, 26277, 26276, // 3320-3327
26275, 26276, 26277, 26274, 26280, 26271, 26280, 26273, // 3328-3335
26279, 26269, 26314, 25862, 13350, -11543, -25497, -26369, // 3336-3343
-26250, -26287, -26270, -26280, -26273, -26278, -26275, -26277, // 3344-3351
-26274, -26280, -26271, -26281, -26271, -26280, -26273, -26279, // 3352-3359
-26274, -26276, -26277, -26275, -26276, -26279, -26268, -26291, // 3360-3367
-26251, -26321, -26175, -26611, -21056, 709, 21769, 26606, // 3368-3375
26182, 26313, 26259, 26285, 26270, 26280, 26273, 26278, // 3376-3383
26275, 26277, 26274, 26278, 26275, 26277, 26275, 26276, // 3384-3391
26276, 26277, 26274, 26279, 26272, 26281, 26269, 26285, // 3392-3399
26264, 26295, 26236, 26406, 25203, 10289, -14502, -26053, // 3400-3407
-26285, -26280, -26271, -26280, -26273, -26277, -26276, -26276, // 3408-3415
-26276, -26276, -26276, -26275, -26278, -26274, -26278, -26275, // 3416-3423
-26276, -26276, -26276, -26276, -26276, -26276, -26277, -26273, // 3424-3431
-26282, -26262, -26309, -26194, -26542, -19027, 4234, 23296, // 3432-3439
26563, 26191, 26312, 26257, 26288, 26267, 26284, 26269, // 3440-3447
26281, 26272, 26280, 26272, 26280, 26272, 26279, 26275, // 3448-3455
26276, 26275, 26278, 26274, 26279, 26271, 26283, 26266, // 3456-3463
26290, 26257, 26306, 26210, 26496, 24272, 6991, -17153, // 3464-3471
-26390, -26228, -26298, -26263, -26285, -26270, -26279, -26275, // 3472-3479
-26277, -26274, -26279, -26273, -26278, -26275, -26276, -26277, // 3480-3487
-26274, -26278, -26275, -26276, -26276, -26276, -26274, -26281, // 3488-3495
-26269, -26285, -26263, -26297, -26235, -26340, -16648, 7667, // 3496-3503
24479, 26483, 26212, 26304, 26262, 26283, 26272, 26279, // 3504-3511
26274, 26278, 26273, 26279, 26273, 26279, 26274, 26276, // 3512-3519
26278, 26273, 26278, 26275, 26276, 26277, 26275, 26277, // 3520-3527
26273, 26284, 26258, 26314, 26186, 26577, 23018, 3535, // 3528-3535
-19462, -26566, -26186, -26316, -26254, -26289, -26268, -26281, // 3536-3543
-26274, -26276, -26276, -26276, -26276, -26277, -26274, -26278, // 3544-3551
-26274, -26278, -26274, -26278, -26274, -26278, -26274, -26277, // 3552-3559
-26277, -26273, -26280, -26272, -26276, -26299, -25963, -13930, // 3560-3567
10919, 25357, 26386, 26245, 26287, 26273, 26276, 26275, // 3568-3575
26279, 26272, 26281, 26272, 26277, 26277, 26275, 26277, // 3576-3583
26276, 26274, 26279, 26272, 26282, 26268, 26285, 26267, // 3584-3591
26285, 26266, 26289, 26254, 26319, 26177, 26611, 21418, // 3592-3599
  2, -21422, -26607, -26181, -26316, -26255, -26289, -26267, // 3600-3607
-26282, -26273, -26277, -26276, -26276, -26276, -26276, -26276, // 3608-3615
-26276, -26276, -26276, -26275, -26278, -26273, -26280, -26271, // 3616-3623
-26282, -26269, -26284, -26266, -26291, -26244, -26385, -25359, // 3624-3631
-10917, 13929, 25963, 26300, 26273, 26277, 26275, 26276, // 3632-3639
26277, 26274, 26279, 26273, 26278, 26275, 26276, 26277, // 3640-3647
26274, 26279, 26273, 26279, 26272, 26280, 26272, 26281, // 3648-3655
26270, 26284, 26264, 26292, 26253, 26317, 26186, 26565, // 3656-3663
19462, -3534, -23019, -26575, -26188, -26315, -26254, -26290, // 3664-3671
-26265, -26284, -26271, -26280, -26273, -26277, -26276, -26275, // 3672-3679
-26278, -26275, -26276, -26276, -26275, -26279, -26273, -26279, // 3680-3687
-26272, -26280, -26272, -26283, -26262, -26303, -26213, -26483, // 3688-3695
-24479, -7666, 16646, 26344, 26230, 26299, 26264, 26282, // 3696-3703
26274, 26276, 26276, 26275, 26278, 26273, 26280, 26272, // 3704-3711
26278, 26276, 26274, 26279, 26274, 26276, 26277, 26275, // 3712-3719
26277, 26275, 26277, 26273, 26283, 26263, 26301, 26223, // 3720-3727
26394, 17151, -6992, -24269, -26498, -26212, -26301, -26264, // 3728-3735
-26283, -26271, -26281, -26271, -26280, -26272, -26280, -26273, // 3736-3743
-26278, -26275, -26276, -26276, -26276, -26276, -26277, -26274, // 3744-3751
-26279, -26271, -26282, -26269, -26287, -26257, -26312, -26190, // 3752-3759
-26566, -23291, -4241, 19034, 26538, 26194, 26313, 26255, // 3760-3767
26289, 26268, 26281, 26273, 26278, 26274, 26278, 26274, // 3768-3775
26277, 26275, 26277, 26275, 26277, 26274, 26278, 26275, // 3776-3783
26276, 26276, 26275, 26279, 26273, 26278, 26274, 26277, // 3784-3791
26288, 26051, 14502, -10288, -25203, -26407, -26236, -26295, // 3792-3799
-26264, -26284, -26270, -26281, -26273, -26278, -26274, -26278, // 3800-3807
-26273, -26280, -26272, -26279, -26275, -26275, -26278, -26273, // 3808-3815
-26278, -26277, -26272, -26283, -26265, -26292, -26252, -26319, // 3816-3823
-26178, -26608, -21769, -708};// 3824-3827
