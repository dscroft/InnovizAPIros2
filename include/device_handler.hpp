#ifndef DEVICE_HANDLER_HPP
#define DEVICE_HANDLER_HPP

class DeviceHandler
{   
    invz::FrameDataUserBuffer reqBuffers[5];
    invz::DeviceMeta meta;
    invz::FrameDataAttributes* attributes = new invz::FrameDataAttributes[buffersCount];

    public:
	int pixels_per_frame = 0;
    int sum_pixels_per_frame = 0;
    int fps = 0;
    invz::INVZ2MeasurementXYZType* measurement_reflection_0 = nullptr;
	invz::INVZ2MeasurementXYZType* measurement_reflection_1 = nullptr;
	invz::INVZ2MeasurementXYZType* measurement_reflection_2 = nullptr;
	invz::INVZ2SumMeasurementXYZType* summation_measurement = nullptr;
	
	IDevice* device_interface = nullptr;

    DeviceHandler(std::string path)
    {
		uint8_t accual_level;		
        //                          config path, log file name, api log level
        device_interface = DeviceInit(path,            "",          3);

		
		// Maybe sleep will solve this instead of connecting
		ros::Duration(1.0).sleep();
    }


	Result StartRecording(std::string path)
	{	
        ROS_INFO_STREAM("Srart recording "); 	
		return device_interface->StartRecording(path);
	}

	Result StopRecording()
	{
        ROS_INFO_STREAM("Stop recording"); 
		return device_interface->StopRecording();
	}

    void GetFrameDataAttributes()
    {
		Result result;
		result.error_code = ERROR_CODE_GENERAL;

		// Waiting until we get the frame data attributes
		while (result.error_code!= ERROR_CODE_OK)
		{
			result = device_interface->GetFrameDataAttributes(attributes, buffersCount);
			for (int i=0; i<buffersCount; i++)
		{
		if (attributes[i].known_type==GRAB_TYPE_MEASURMENTS_REFLECTION0)
                     {
                        if (attributes[i].length == EAGLE_PIXELS_PER_FRAME)
			{
				pixels_per_frame =EAGLE_PIXELS_PER_FRAME;
				sum_pixels_per_frame = EAGLE_SUM_PIXELS_PER_FRAME;
                                fps = EAGLE_FPS;
			}
                        else
			{
				pixels_per_frame =FALCON_PIXELS_PER_FRAME;
				sum_pixels_per_frame = FALCON_SUM_PIXELS_PER_FRAME;
                                fps = FALCON_FPS;
			}
                      break;  
                     }
		}
		} 
		ROS_INFO_STREAM("device_interface finished GetFrameDataAttributes ");       
    }
    
    void AllocateBuffers()
    {
        // Check if API is able to grab this data in the first frame
        // We get this data each frame in the frame meta		
        meta.lrf_count = 4;	
        for (int i = 0; i < meta.lrf_count; i++)
        {
            meta.lrf_height[i] = 20;
            meta.lrf_width[i] = 300;
        }
GetFrameDataAttributes();
        measurement_reflection_0 = new INVZ2MeasurementXYZType[pixels_per_frame];
		measurement_reflection_1 = new INVZ2MeasurementXYZType[pixels_per_frame];
		measurement_reflection_2 = new INVZ2MeasurementXYZType[pixels_per_frame];
		summation_measurement = new INVZ2SumMeasurementXYZType[sum_pixels_per_frame];
        

        // Activating the relevant buffers
        for (size_t i = 0; i < buffersCount; i++)
        {
			if (attributes[i].known_type== GRAB_TYPE_METADATA)
            {
                reqBuffers[0].dataAttrs=attributes[i];	
                reqBuffers[0].dataBuffer = new uint8_t[attributes[i].itemSize * attributes[i].length];        
                device_interface->ActivateBuffer(attributes[i], true);        
            }
            if (attributes[i].known_type== GRAB_TYPE_MEASURMENTS_REFLECTION0)
            {
                reqBuffers[1].dataAttrs=attributes[i];		
                reqBuffers[1].dataBuffer = reinterpret_cast<uint8_t*>(measurement_reflection_0);
                device_interface->ActivateBuffer(attributes[i], true);                
            }
			 if (attributes[i].known_type== GRAB_TYPE_MEASURMENTS_REFLECTION1)
            {
                reqBuffers[2].dataAttrs=attributes[i];		
                reqBuffers[2].dataBuffer = reinterpret_cast<uint8_t*>(measurement_reflection_1);
                device_interface->ActivateBuffer(attributes[i], true);                
            }
			 if (attributes[i].known_type== GRAB_TYPE_MEASURMENTS_REFLECTION2)
            {
                reqBuffers[3].dataAttrs=attributes[i];		
                reqBuffers[3].dataBuffer = reinterpret_cast<uint8_t*>(measurement_reflection_2);
                device_interface->ActivateBuffer(attributes[i], true);                
            }
			 if (attributes[i].known_type== GRAB_TYPE_SUMMATION_REFLECTION0)
            {
                reqBuffers[4].dataAttrs=attributes[i];		
                reqBuffers[4].dataBuffer = reinterpret_cast<uint8_t*>(summation_measurement);
                device_interface->ActivateBuffer(attributes[i], true);                
            }
        }
    }
    
    void register_frame_callback()
    {
		// Register Frame callback
		device_interface->RegisterFrameCallback(frame_callback);			
        ROS_DEBUG_STREAM("Registered Frame callback");
    }

    Result GrabFrame(uint32_t& frameNumber, uint64_t &pc_timestamp)
    {
        ROS_DEBUG_STREAM("Waiting for the grabbed frame");

        // wait_until a frame callback arrives
        std::unique_lock<std::mutex> lk(monitorMutex);
        cv.wait(lk);
        
        return device_interface->GrabFrame(reqBuffers, 5, frameNumber, pc_timestamp);
    }

    void Close()
    {
        ROS_DEBUG_STREAM("Disconnecting from device");
        device_interface->Disconnect();

        if(reqBuffers != nullptr){
            delete reqBuffers[0].dataBuffer;
            delete reqBuffers[1].dataBuffer;
			delete reqBuffers[2].dataBuffer;
			delete reqBuffers[3].dataBuffer;
			delete reqBuffers[4].dataBuffer;
        }
        ROS_INFO_STREAM("Removed the reqBuffers");

        if(attributes != nullptr){
            delete attributes;            
        }
        ROS_INFO_STREAM("Removed the attributes");

		if (measurement_reflection_0 != nullptr){
            delete measurement_reflection_0;            
        }
		
		if (measurement_reflection_1 != nullptr){
            delete measurement_reflection_1;            
        }
		
		if (measurement_reflection_2 != nullptr){
            delete measurement_reflection_2;            
        }
		
		if (summation_measurement != nullptr){
            delete summation_measurement;            
        }
        ROS_INFO_STREAM("Removed the measurements");
    }
	
};

#endif // DEVICE_HANDLER_HPP
