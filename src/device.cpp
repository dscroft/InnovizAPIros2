#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include <sensor_msgs/msg/point_cloud2.hpp>

#include <pcl_conversions/pcl_conversions.h>

// Innoviz
#include "interface/DeviceApi.h"
#include "common_includes/invz_types.h"
#include "common.h"
using namespace invz;

using namespace invz;
std::mutex monitorMutex;
std::condition_variable cv;

std::mutex interrupt_mutex;

bool interrupt_recieved=false;
bool isInterruptRecieved()
{
    interrupt_mutex.lock();
    bool val= interrupt_recieved;
    interrupt_mutex.unlock();
    return val;
}

void setInterruptRecieved(bool val)
{
    interrupt_mutex.lock();
    interrupt_recieved=val;
    interrupt_mutex.unlock();
}

size_t buffersCount = 100; // 100 is max size it will be changed to relevant number after GetFrameDataAttributes
static size_t EAGLE_PIXELS_PER_FRAME = 192000;
static size_t EAGLE_SUM_PIXELS_PER_FRAME = 48000;
static size_t EAGLE_FPS = 15;
static size_t FALCON_PIXELS_PER_FRAME = 307200;
static size_t FALCON_SUM_PIXELS_PER_FRAME = 76800;
static size_t FALCON_FPS = 10;

void frame_callback(uint32_t* id)
{	
	// Release the lock and let the GrabFrame function continue
	cv.notify_one();
    //  ROS_DEBUG_STREAM("frame_callback received");
    RCLCPP_DEBUG_STREAM( rclcpp::get_logger("rclcpp"), "frame_callback received");
}

using namespace std::chrono_literals;

/* This example creates a subclass of Node and uses std::bind() to register a
* member function as a callback from the timer. */

class DeviceHandler
{   
public:
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

        RCLCPP_INFO_STREAM( rclcpp::get_logger("rclcpp"), "DeviceHandler constructor" << path ); 	
        //                          config path, log file name, api log level
        device_interface = DeviceInit(path,            "",          3);

		
		// Maybe sleep will solve this instead of connecting
		//ros::Duration(1.0).sleep();
        rclcpp::sleep_for(std::chrono::seconds(1));
    }


	Result StartRecording(std::string path)
	{	
        //ROS_INFO_STREAM("Srart recording "); 	
        RCLCPP_INFO_STREAM( rclcpp::get_logger("rclcpp"), "Start recording" );
		return device_interface->StartRecording(path);
	}

	Result StopRecording()
	{
        //ROS_INFO_STREAM("Stop recording"); 
        RCLCPP_INFO_STREAM( rclcpp::get_logger("rclcpp"), "Stop recoding");
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
		//ROS_INFO_STREAM("device_interface finished GetFrameDataAttributes ");       
        RCLCPP_INFO_STREAM( rclcpp::get_logger("rclcpp"), "device_interface finished GetFrameDataAttributes ");
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
        //ROS_DEBUG_STREAM("Registered Frame callback");
        RCLCPP_DEBUG_STREAM( rclcpp::get_logger("rclcpp"), "Registered Frame callback");
    }

    Result GrabFrame(uint32_t& frameNumber, uint64_t &pc_timestamp)
    {
        //ROS_DEBUG_STREAM("Waiting for the grabbed frame");
        RCLCPP_DEBUG_STREAM( rclcpp::get_logger("rclcpp"), "Waiting for the grabbed frame");

        // wait_until a frame callback arrives
        std::unique_lock<std::mutex> lk(monitorMutex);
        cv.wait(lk);
        
        return device_interface->GrabFrame(reqBuffers, 5, frameNumber, pc_timestamp);
    }

    void Close()
    {
        //ROS_DEBUG_STREAM("Disconnecting from device");
        RCLCPP_DEBUG_STREAM(rclcpp::get_logger("rclcpp"), "Disconnecting from device");
        device_interface->Disconnect();

        if(reqBuffers != nullptr){
            delete reqBuffers[0].dataBuffer;
            delete reqBuffers[1].dataBuffer;
			delete reqBuffers[2].dataBuffer;
			delete reqBuffers[3].dataBuffer;
			delete reqBuffers[4].dataBuffer;
        }
        //ROS_INFO_STREAM("Removed the reqBuffers");
        RCLCPP_INFO_STREAM( rclcpp::get_logger("rclcpp"), "Removed the reqBuffers");

        if(attributes != nullptr){
            delete attributes;            
        }
        //ROS_INFO_STREAM("Removed the attributes");
        RCLCPP_INFO_STREAM( rclcpp::get_logger("rclcpp"), "Removed the attributes");

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
        //ROS_INFO_STREAM("Removed the measurements");
        RCLCPP_INFO_STREAM( rclcpp::get_logger("rclcpp"), "Removed the measurements");
    }
	
};

//void readAndSendFramePCL(ros::Publisher &publisher, INVZ2MeasurementXYZType* measurements, int pixels_per_frame, float fpa, std::string frame_id)
void readAndSendFramePCL(rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr publisher, 
        INVZ2MeasurementXYZType* measurements, int pixels_per_frame, float fpa, std::string frame_id)
{
    // Set ros pcl message
    pcl::PointCloud<pcl::PointXYZI>::Ptr pcl_msg(new pcl::PointCloud<pcl::PointXYZI>());
    pcl_msg->header.stamp = pcl_conversions::toPCL(rclcpp::Clock().now());
    pcl_msg->header.frame_id = frame_id;
    pcl_msg->points.resize(pixels_per_frame);

    //ROS_DEBUG_STREAM("Preparing msg to publish");
    RCLCPP_DEBUG_STREAM(rclcpp::get_logger("rclcpp"), "Preparing msg to publish");
    
	static constexpr float hundredth = 1.0/100.0f;
	
    // For each pixel - update its details within the message info, in both types
    for(int i = 0; i < pixels_per_frame; i++)
    {
		// PCL Msg
        pcl::PointXYZI current_point;	
		if (measurements[i].pfa<=fpa)
		{
            current_point.x = measurements[i].x*hundredth; 
            current_point.y = measurements[i].y*hundredth;
            current_point.z = measurements[i].z*hundredth; 

            current_point.intensity = measurements[i].reflectivity;
		}
		else
		{     
            current_point.x = 0; 
            current_point.y = 0;
            current_point.z = 0; 
            current_point.intensity =0;	
		}
        
        // Add to pointcloud msg
        pcl_msg->points[i] = current_point;	
    }

    // Publish message:
    sensor_msgs::msg::PointCloud2 ros_msg;
    pcl::toROSMsg( *pcl_msg, ros_msg );
    publisher->publish( ros_msg );

    //publisher->publish(pcl_msg);
    //ROS_DEBUG_STREAM("Published pcl_msg");	
    RCLCPP_DEBUG_STREAM(rclcpp::get_logger("rclcpp"), "Published pcl_msg");	
}

//void readAndSendFrameSumPCL(ros::Publisher &publisher, INVZ2SumMeasurementXYZType* measurements, int sum_pixels_per_frame, float fpa, std::string frame_id)
void readAndSendFrameSumPCL(rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr publisher, 
        INVZ2SumMeasurementXYZType* measurements, int sum_pixels_per_frame, float fpa, std::string frame_id)
{
    // Set ros pcl message
    pcl::PointCloud<pcl::PointXYZI>::Ptr pcl_msg(new pcl::PointCloud<pcl::PointXYZI>());
    pcl_msg->header.stamp = pcl_conversions::toPCL(rclcpp::Clock().now());
    pcl_msg->header.frame_id = frame_id;
    pcl_msg->points.resize(sum_pixels_per_frame);

    //ROS_DEBUG_STREAM("Preparing msg to publish");
    RCLCPP_DEBUG_STREAM(rclcpp::get_logger("rclcpp"), "Preparing msg to publish");

	static constexpr float hundredth = 1.0/100.0f;
	
    // For each pixel - update its details within the message info, in both types
    for(int i = 0; i < sum_pixels_per_frame; i++)
    {
		// PCL Msg
        pcl::PointXYZI current_point;	
		if (measurements[i].pfa<=fpa)
		{
            current_point.x = measurements[i].x*hundredth; 
            current_point.y = measurements[i].y*hundredth;
            current_point.z = measurements[i].z*hundredth; 

            current_point.intensity = measurements[i].reflectivity;
		}
		else
		{		       
            current_point.x =0; 
            current_point.y = 0;
            current_point.z = 0; 
            current_point.intensity =0;	
		}
        
        // Add to pointcloud msg
        pcl_msg->points[i] = current_point;	
    }

    // Publish message:
    //publisher.publish(pcl_msg);
    sensor_msgs::msg::PointCloud2 ros_msg;
    pcl::toROSMsg( *pcl_msg, ros_msg );
    publisher->publish( ros_msg );
    
    //ROS_DEBUG_STREAM("Published pcl_msg");	
    RCLCPP_DEBUG_STREAM(rclcpp::get_logger("rclcpp"), "Published pcl_msg");
}

void handle_sigint(int singal){
    //ROS_INFO_STREAM("handle_sigint Caught signal:" << signal);
    RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"), "handle_sigint Caught signal:" << signal);
    setInterruptRecieved(true);
    rclcpp::shutdown();
}

class DevicePublisher : public rclcpp::Node
{
public:
    std::string file_path = "";
	std::string frame_id = "base_link";
    std::string ref0_name = "invz_reflection_0";
	std::string ref1_name = "invz_reflection_1";
	std::string ref2_name = "invz_reflection_2";
	std::string sum_ref_name = "invz_summation_reflection";
    size_t number_of_frames = 0;
	int log_level = 3;
    std::string file_path_rec = "";

    // Loop helpers
    uint32_t frameNumber=0;
    uint64_t pc_timestamp=0;
    int counter = 0;
	float fpa = 0;

    std::unique_ptr<DeviceHandler> dh;

     // Initialize publisher for publisher_node
    // Messages are of String type, Queue is limited to size of QUEUE_BUFFER (message buffer)
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pub0, pub1, pub2, pub3;

    rclcpp::TimerBase::SharedPtr timer_;

    DevicePublisher()
        : Node("device")
    {
        struct sigaction sigIntHandler;

        sigIntHandler.sa_handler = handle_sigint;
        sigemptyset(&sigIntHandler.sa_mask);
        sigIntHandler.sa_flags = 0;

        sigaction(SIGINT, &sigIntHandler, NULL);

        // Initialize ROS
        //ros::init(argc, argv, "invz_device_publisher", ros::init_options::NoSigintHandler);

    
        this->file_path = 
            this->declare_parameter<std::string>("file_path", "");
        /*ros::NodeHandle private_node("~");
        private_node.param("reflection_0", ref0_name, std::string("base_link"));
	    private_node.param("reflection_1", ref1_name, std::string("base_link"));
	    private_node.param("reflection_2", ref2_name, std::string("base_link"));
	    private_node.param("summation_reflection", sum_ref_name, std::string("base_link"));
        private_node.param("file_path", file_path, std::string(""));
	    private_node.param("frame_id", frame_id, std::string("base_link"));	
        private_node.param<float>("false_positive_alarm", fpa, 0.0);
	    private_node.param<int>("log_level", log_level, 3);
        private_node.param("file_path_rec", file_path_rec, std::string(""));
        */

        // ROS Log level
        /*bool res = false;
        switch (log_level)
        {
        case 0:
            res = ros::console::set_logger_level(ROSCONSOLE_DEFAULT_NAME, ros::console::levels::Debug);
            break;
        case 1: res = ros::console::set_logger_level(ROSCONSOLE_DEFAULT_NAME, ros::console::levels::Info);
            break;
        case 2: res = ros::console::set_logger_level(ROSCONSOLE_DEFAULT_NAME, ros::console::levels::Warn);
            break;
        case 3: res = ros::console::set_logger_level(ROSCONSOLE_DEFAULT_NAME, ros::console::levels::Error);
            break;
        case 4: res = ros::console::set_logger_level(ROSCONSOLE_DEFAULT_NAME, ros::console::levels::Fatal);
            break;
        default: break;
        }
	    if (res)
		    ros::console::notifyLoggerLevelsChanged();*/

        RCLCPP_INFO_STREAM(this->get_logger(), "RVIZ topic 0: " << ref0_name);
        RCLCPP_INFO_STREAM(this->get_logger(), "RVIZ topic 1: " << ref1_name);
        RCLCPP_INFO_STREAM(this->get_logger(), "RVIZ topic 2: " << ref2_name);
        RCLCPP_INFO_STREAM(this->get_logger(), "RVIZ topic 3: " << sum_ref_name);
        RCLCPP_INFO_STREAM(this->get_logger(), "configuration path: " << file_path);
        RCLCPP_INFO_STREAM(this->get_logger(), "false_positive_alarm: " << fpa);
        RCLCPP_INFO_STREAM(this->get_logger(), "log_level: " << log_level);

        RCLCPP_INFO_STREAM(this->get_logger(), "HERE1" );

        // Initialize FileHandler Interface
        //DeviceHandler dh = DeviceHandler(file_path);
        this->dh = std::make_unique<DeviceHandler>(file_path);

        RCLCPP_INFO_STREAM(this->get_logger(), "HERE2" );

        this->dh->AllocateBuffers();
        //ROS_INFO_STREAM("allocate done: ");
        this->dh->register_frame_callback();
        //ROS_INFO_STREAM("register_frame_callback done: ");

        RCLCPP_INFO_STREAM(this->get_logger(), "Allocated buffers");

        this->pub0 = this->create_publisher<sensor_msgs::msg::PointCloud2>(ref0_name, 1);
	    this->pub1 = this->create_publisher<sensor_msgs::msg::PointCloud2>(ref1_name, 1);
	    this->pub2 = this->create_publisher<sensor_msgs::msg::PointCloud2>(ref2_name, 1);
	    this->pub3 = this->create_publisher<sensor_msgs::msg::PointCloud2>(sum_ref_name, 1);
   
        this->timer_ = this->create_wall_timer(
        std::chrono::seconds(1)/this->dh->fps, std::bind(&DevicePublisher::timer_callback, this));

        RCLCPP_INFO_STREAM(this->get_logger(), "Start getting frames from device");
        //this->begin = clock();
        
        try 
        {
            if(!file_path_rec.empty())  
                this->dh->StartRecording(file_path_rec);	        
                // Read each frame and send it as PCL
        }
        catch (const std::runtime_error& e) 
        {
                // All errors are currently handled as runtime errors
                std::cout << "Error has occured: " << e.what() << std::endl;
        }
    }

    ~DevicePublisher()
    {
        //ROS_INFO_STREAM("Shutting down the device node");
        RCLCPP_INFO_STREAM(this->get_logger(), "Shutting down the device node");
     
        if(!file_path_rec.empty())
            this->dh->StopRecording();
     
        this->dh->Close();
    }

    void timer_callback()
    {
        try
        {
            invz::Result ret = this->dh->GrabFrame(frameNumber, pc_timestamp);

            // Validating the grab was succesfull
            if (ret.error_code != invz::ERROR_CODE_OK)
            {
                RCLCPP_WARN_STREAM(this->get_logger(), "Frame " << counter << " was not grabbed.");
                return;
            }

            // Publishing the data
            readAndSendFramePCL(pub0, dh->measurement_reflection_0, dh->pixels_per_frame, fpa, frame_id);
            readAndSendFramePCL(pub1, dh->measurement_reflection_1, dh->pixels_per_frame, fpa, frame_id);
            readAndSendFramePCL(pub2, dh->measurement_reflection_2, dh->pixels_per_frame, fpa, frame_id);
            readAndSendFrameSumPCL(pub3, dh->summation_measurement, dh->sum_pixels_per_frame, fpa, frame_id);
        }
        catch (const std::runtime_error& e) 
        {
                // All errors are currently handled as runtime errors
                std::cout << "Error has occured: " << e.what() << std::endl;
        }
    }
};


int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv, rclcpp::InitOptions(), rclcpp::SignalHandlerOptions::None);
    rclcpp::spin(std::make_shared<DevicePublisher>());
    rclcpp::shutdown();
    return 0;
}
