//Confidential and proprietary to Innoviz Technologies Ltd//

#ifndef __IUDP_RECEIVER_H__
#define __IUDP_RECEIVER_H__

/**
* @file IUdpReceiver.h
* @brief File containing classes and structures required to
* recept and parse the UDP datapacket sent by the target sensor
* The udp communication is responsible of sending the point clouds
* data
*/

#include "common_includes/PacketContainer.h"
#include "common_includes/invz_types.h"

namespace invz {

	class INVZ_API IUdpReceiver
	{
	public:
		virtual bool CloseConnection() = 0;
		virtual bool StartListening() = 0;
		virtual void RegisterCallback(std::function<bool(uint16_t, PacketContainer*)> callback) = 0;
	};

	INVZ_API IUdpReceiver* UdpReceiverInit(uint32_t marker, const std::string ipAddress, const int lidarPort, uint32_t api_log_severity_level = 3);

}
#endif /*__IUDP_RECEIVER_H__*/