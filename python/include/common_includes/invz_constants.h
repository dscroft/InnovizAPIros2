//Confidential and proprietary to Innoviz Technologies Ltd//

#ifndef __INVZ_CONSTANTS_H__
#define __INVZ_CONSTANTS_H__

#include <stdint.h>

/**
* @file invz_constants.h
*/

#define INVZ_MARKER 0xCAFEBABE

#define IP_HEADER_SIZE_BYTES 20
#define UDP_HEADER_SIZE_BYTES 8
#define UDP_MAX_PAYLOAD_SIZE_BYTES (UINT16_MAX - UDP_HEADER_SIZE_BYTES - IP_HEADER_SIZE_BYTES)


/* INVZ3 Related constants */
#define INVZ3_RECORD_MAJOR_VERSION 0
#define INVZ3_RECORD_MINOR_VERSION 0
#define INVZ3_META_FILE_NAME	"/RecordMeta"
#define INVZ3_FRAME_DATA_ATTR_FILE_NAME	"/FrameDataAttr.json"
#define INVZ3_INDEX_FILE_NAME	"/RecordIndex"
#define INVZ3_STREAM_FILE_NAME_LEN 12
#define INVZ3_STREAM_FILE_NAME_FORMAT "/%05d_%03d"
#define INVZ3_FILE_MARKER 0xCABABEEF
#define INVZ3_DATA_ATTR_MAJOR 0
#define INVZ3_DATA_ATTR_MINOR 0


/* INVZ4 Related constants */
#define INVZ4_RECORD_MAJOR_VERSION 0
#define INVZ4_RECORD_MINOR_VERSION 2
#define INVZ4_LIDAR_SN_LENGTH 12
#define INVZ4_CSAMPLE_LIDAR_SERIAL_LENGTH 20
#define INVZ4_META_FILE_NAME	"/RecordMeta"
#define INVZ4_FRAME_DATA_ATTR_FILE_NAME	"/FrameDataAttr.json"
#define INVZ4_INDEX_FILE_NAME	"/RecordIndex"
#define INVZ4_STREAM_FILE_NAME_LEN 12
#define INVZ4_STREAM_FILE_NAME_FORMAT "/%05d_%03d"
#define INVZ4_FILE_MARKER 0xBABEBEEF
#define INVZ4_DATA_ATTR_MAJOR 0
#define INVZ4_DATA_ATTR_MINOR 1

#define INVZ4_CSAMPLE_LIDAR_SERIAL 20

#define DEVICE_NUM_OF_LRFS 4
#define LUT_STRUCT_SIZE 300
#define PIXELS_BUFF_SIZE  (300 * 2 * sizeof(MacroPixelFixed) + sizeof(SummationMacroPixelFixed))
#define FIVE_MB (5*1024*1024)

#endif /* __INVZ_CONSTANTS_H__ */