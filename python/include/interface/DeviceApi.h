//Confidential and proprietary to Innoviz Technologies Ltd//

#ifndef __DEVICE_API_H__
#define __DEVICE_API_H__
// Copyright 2019 Innoviz Technologies
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
/*=========================================================================

Program:   Device Interface
Module:    DeviceApi.h

Authors: Chen Vivnate

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

/**
* @file DeviceApi.h
* @authors Chen Vivnate
* @date 17-03-2019
* @brief File containing File Reader Interface decleration.
*/


/*! \mainpage Home
*
* \section intro_sec Introduction
*
*/

#include "interface/LidarInterface.h"
#include <string>
#ifndef INVZ_DRIVER_VERSION
#define INVZ_DRIVER_VERSION  "x.x.x"
#endif
namespace invz {

# 

	/**
	* @brief Initialize FileReader API
	*
	* @param f_name .invz file path
	* @param indexed weather to support file indexing or not. if file is indexed, accessing specific frame using frame_num argument in FileReaderInterface::GetFrame is supported
	*/
	INVZ_API IDevice *DeviceInit(std::string config_filename, std::string api_log_filename = "", uint32_t api_log_severity_level = 3);

	/**
	* @brief Close FileReader API
	*
	* @param reader_interface Pointer to the FileReader
	*/
	INVZ_API void DeviceClose(IDevice* device_interface);

	INVZ_API std::string GetVersion();
	
}
#endif /*__DEVICE_API_H__*/