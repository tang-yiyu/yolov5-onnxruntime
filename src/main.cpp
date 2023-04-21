#include <iostream>
#include <opencv2/opencv.hpp>
#include "cmdline.h"
#include "utils.h"
#include "detector.h"

#define MUTIPLE 0 // 0: single image, 1: multiple images


int main(int argc, char* argv[])
{
    const float confThreshold = 0.3f;
    const float iouThreshold = 0.4f;

    cmdline::parser cmd;
    // cmd.add<std::string>("model_path", 'm', "Path to onnx model.", true, "../models/cardetect.onnx");
    // cmd.add<std::string>("image", 'i', "Image source to be detected.", true, "../images/car4.png");
    // cmd.add<std::string>("class_names", 'c', "Path to class names file.", true, "../models/carclasses.txt");
    // cmd.add("gpu", '\0', "Inference on cuda device.");

    // cmd.parse_check(argc, argv);

    // bool isGPU = cmd.exist("gpu");
    // const std::string classNamesPath = cmd.get<std::string>("class_names");
    const std::string modelPath = "../models/cardetect.onnx";
    const std::string classNamesPath = "../models/carclass.txt";
    const std::vector<std::string> classNames = utils::loadNames(classNamesPath);
    bool isGPU = 0; // 0: CPU, 1: GPU
    // const std::string imagePath = cmd.get<std::string>("image");
    // const std::string modelPath = cmd.get<std::string>("model_path");

    if (classNames.empty())
    {
        std::cerr << "Error: Empty class names file." << std::endl;
        return -1;
    }

    YOLODetector detector {nullptr};
    cv::Mat image;
    std::vector<Detection> result;

    std::string imagePath;

    #if MUTIPLE == 1
        for(int i = 421; i <= 455; i++)
        {
            imagePath = "../CL01_WVC/" + std::to_string(i) + ".png";
            std::cout << imagePath << std::endl;
            try
            {
                detector = YOLODetector(modelPath, isGPU, cv::Size(640, 640));
                std::cout << "Model was initialized." << std::endl;

                image = cv::imread(imagePath);
                result = detector.detect(image, confThreshold, iouThreshold);
                if(result.empty())
                {
                    std::cerr << "No car exists!" << std::endl;
                    return 0;
                }
            }
            // catch the exception thrown by the constructor
            catch(const std::exception& e)
            {
                std::cerr << e.what() << std::endl;
                return -1;
            }

            // utils::visualizeDetection(image, result, classNames);

            // find the detection result with the highest confidence
            float MaxIndex = 0;
            for(int i = 1; i < result.size(); i++)
            {
                if(result[i].conf > result[MaxIndex].conf)
                {
                    MaxIndex = i;
                }
            }

            utils::visualizeDetection(image, result[MaxIndex], classNames);

            std::cout << "Left up point: " << result[MaxIndex].box.x << ", " << result[MaxIndex].box.y << std::endl;
            std::cout << "Right down point: " << result[MaxIndex].box.x + result[MaxIndex].box.width << ", " << result[MaxIndex].box.y + result[MaxIndex].box.height << std::endl;


            //cv::imshow(imagePath, image);
            cv::imwrite(imagePath, image);
            cv::waitKey(0);
        }

    #else
        try
        {
            detector = YOLODetector(modelPath, isGPU, cv::Size(640, 640));
            std::cout << "Model was initialized." << std::endl;

            imagePath = "../images/car4.png";

            image = cv::imread(imagePath);
            result = detector.detect(image, confThreshold, iouThreshold);
            if(result.empty())
            {
                std::cerr << "No car exists!" << std::endl;
                return 0;
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            return -1;
        }

        // utils::visualizeDetection(image, result, classNames);

        // find the detection result with the highest confidence
        float MaxIndex = 0;
        for(int i = 1; i < result.size(); i++)
        {
            if(result[i].conf > result[MaxIndex].conf)
            {
                MaxIndex = i;
            }
        }

        utils::visualizeDetection(image, result[MaxIndex], classNames);

        std::cout << "Left up point: " << result[MaxIndex].box.x << ", " << result[MaxIndex].box.y << std::endl;
        std::cout << "Right down point: " << result[MaxIndex].box.x + result[MaxIndex].box.width << ", " << result[MaxIndex].box.y + result[MaxIndex].box.height << std::endl;


        cv::imshow("Result of detection", image);
        // cv::imwrite("result.jpg", image);
        cv::waitKey(0);
    #endif

    return 0;
}
