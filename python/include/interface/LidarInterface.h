//Confidential and proprietary to Innoviz Technologies Ltd//

#ifndef __LIDAR_INTERFACE_H__
#define __LIDAR_INTERFACE_H__


/**
* @file LiDARInterface.h
* @brief File containing LiDAR Interface decleration.
*/



/*! \mainpage Home
*
* \section intro_sec Introduction
*
* The Innoviz API is the application program interface used for communicating with the software running on Innoviz products.
*
* The API is used by InnovizPlayer&trade;, a visualization utility developed by Innoviz Technologies Ltd. for viewing point clouds and perception outputs in real-time.
* is used by third-party visualization utilities to enable them to view Innoviz point clouds and perception output
*
* \section win_files_sec Windows Files
*
* The Windows folder includes to following files:
*	- innovizApi.chm - the API Help file to run on the relevant browser
*	- innovizApi.dll -  Runs the API on third-party (and visualization) utilities and to run the demo program
*	- demoProgram.cpp -  Source code of the demo application for developers toint port,new understand how the demo app uses the API
*	- demoProgram.exe - Executable file of the demo application
*

* \section help_sec This help file includes
*
* This Help file includes the following sections:
*	- Classes - This section describes the Device Interface, File Reader Interface, Measurement, and Vector 3 - Commands, Parameters, and Indicators
*	- Files - This section documents the source code file (Apiinterface.h)
*/

#include "ReaderInterface.h"
#include "CncInterface.h"

/*! \mainpage Home
*
* \section intro_sec Introduction
*
*/


namespace invz {


	class INVZ_API IDevice : public IReader, public ICnc 
	{
	public:
		virtual Result GetConnectionStatus(ChannelStatistics* channel_statistics, size_t& statistics_count) = 0;
	};
}
#endif /*__LIDAR_INTERFACE_H__*/