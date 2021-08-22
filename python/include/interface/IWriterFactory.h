//Confidential and proprietary to Innoviz Technologies Ltd//

/**
* @file IWriterFactory.h
* @authors Julia Sher
* @date 02-05-2019
* @brief
*/

#ifndef __IWRITER_FACTORY_H__
#define __IWRITER_FACTORY_H__

#include "IWriter.h"

namespace invz {

	/**
	* @brief Initialize FileWriter Interface
	*
	* @param f_name .invz file path
	*/

	INVZ_API IWriter* FileWriterInit(std::string filename, std::string device_ip, uint32_t file_format,
		std::vector<std::pair<uint32_t, uint16_t>> virtual_channels_ports, std::vector<invz::FrameDataAttributes> frame_data_attr,
		std::string api_log_filename = "", uint32_t api_log_severity_level = 3);

	/**
	* @brief Close FileWriter Interface
	*
	* @param writer_interface Pointer to the FileWriter
	*/
	INVZ_API void FileWriterClose(IWriter* writer_interface);

}
#endif /*__IWRITER_FACTORY_H__*/