#include <astra/astra.hpp>
#include <map>

class MultiFrameListener : public astra::FrameListener
{
public:
    MultiFrameListener();
    MultiFrameListener(void* rgbData, void* depthData, std::map<astra::JointType, astra::Vector2i>* jointData, int* numInQueue, int queueMax);
    ~MultiFrameListener();
private:
    astra::RgbPixel* rgbDataOutput;
    astra::int16_t* depthDataOutput;
    std::map<astra::JointType, astra::Vector2i>* jointDataOutput;
    int* numInQueue;
    int queueMax;

    void get_joint_data(const astra::BodyFrame& bodyFrame);
    void on_frame_ready(astra::StreamReader& reader, astra::Frame& frame);
};