#include "joint_by_openpose.h"
// Third-party dependencies
#include <opencv2/opencv.hpp>
// Command-line user interface
#define OPENPOSE_FLAGS_DISABLE_PRODUCER
#define OPENPOSE_FLAGS_DISABLE_DISPLAY

// OpenPose dependencies
#include <openpose/headers.hpp>
#include <openpose/flags.hpp>

void configureWrapper(op::Wrapper& opWrapper)
{
    try
    {
        // Configuring OpenPose

        // Applying user defined configuration - GFlags to program variables
        // outputSize: using input image size
        const auto outputSize = op::flagsToPoint(op::String(FLAGS_output_resolution), "-1x-1");
        // netInputSize
        const auto netInputSize = op::flagsToPoint(op::String(FLAGS_net_resolution), "640x480");
        // poseMode
        const auto poseMode = op::flagsToPoseMode(FLAGS_body);
        // poseModel
        const auto poseModel = op::flagsToPoseModel(op::String("COCO"));
        // JSON saving
        //if (!FLAGS_write_keypoint.empty())
        //    op::opLog(
        //        "Flag `write_keypoint` is deprecated and will eventually be removed. Please, use `write_json`"
        //        " instead.", op::Priority::Max);
        // keypointScaleMode
        const auto keypointScaleMode = op::flagsToScaleMode(FLAGS_keypoint_scale);
        // heatmaps to add
        const auto heatMapTypes = op::flagsToHeatMaps(FLAGS_heatmaps_add_parts, FLAGS_heatmaps_add_bkg,
            FLAGS_heatmaps_add_PAFs);
        const auto heatMapScaleMode = op::flagsToHeatMapScaleMode(FLAGS_heatmaps_scale);
        // >1 camera view?
        const auto multipleView = (FLAGS_3d || FLAGS_3d_views > 1);
        // Enabling Google Logging
        const bool enableGoogleLogging = true;
        // Pose configuration (use WrapperStructPose{} for default and recommended configuration)
        const op::WrapperStructPose wrapperStructPose{
            poseMode, netInputSize, outputSize, keypointScaleMode, FLAGS_num_gpu, FLAGS_num_gpu_start,
            FLAGS_scale_number, (float)FLAGS_scale_gap, op::flagsToRenderMode(FLAGS_render_pose, multipleView),
            poseModel, !FLAGS_disable_blending, (float)FLAGS_alpha_pose, (float)FLAGS_alpha_heatmap,
            FLAGS_part_to_show, op::String(FLAGS_model_folder), heatMapTypes, heatMapScaleMode, FLAGS_part_candidates,
            (float)FLAGS_render_threshold, 1, FLAGS_maximize_positives, FLAGS_fps_max,
            op::String(FLAGS_prototxt_path), op::String(FLAGS_caffemodel_path),
            (float)FLAGS_upsampling_ratio, enableGoogleLogging };
        opWrapper.configure(wrapperStructPose);
        if (FLAGS_disable_multi_thread)
            opWrapper.disableMultiThreading();
    }
    catch (const std::exception& e)
    {
        op::error(e.what(), __LINE__, __FUNCTION__, __FILE__);
    }
}
JointByOpenpose::JointByOpenpose()
{
    //³õÊ¼»¯openpose
    configureWrapper(opWrapper);
    opWrapper.start();
}

bool JointByOpenpose::getJointsFromRgb(cv::Mat frame, cv::Mat& frameOut, float* jointsArrayOutput)
{
    const op::Matrix imageToProcess = OP_CV2OPCONSTMAT(frame);
    auto datumProcessed = opWrapper.emplaceAndPop(imageToProcess);
    if (datumProcessed != nullptr)
    {
        frameOut = OP_OP2CVCONSTMAT(datumProcessed->at(0)->cvOutputData).clone();
        auto jointsData = datumProcessed->at(0)->poseKeypoints;
        if (NULL == jointsArrayOutput||jointsData.empty())
            return false;

        for (int i = 0; i < 18; i++)
        {
            jointsArrayOutput[i * 3] = jointsData[i * 3];
            jointsArrayOutput[i * 3 + 1] = jointsData[i * 3 + 1];
            jointsArrayOutput[i * 3 + 2] = jointsData[i * 3 + 2];
        }
        return true;
    }
    return false;
}