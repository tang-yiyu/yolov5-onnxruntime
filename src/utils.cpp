#include "utils.h"

/**
 * @brief Calculate the product of a vector
 * 
 * @param vector Vector of int64_t
 * @return size_t Product of the vector
*/
size_t utils::vectorProduct(const std::vector<int64_t>& vector)
{
    if (vector.empty())
        return 0;

    size_t product = 1;
    for (const auto& element : vector)
        product *= element;

    return product;
}

/**
 * @brief Convert char* to wstring
 * 
 * @param str Char*
 * @return std::wstring 
*/
std::wstring utils::charToWstring(const char* str)
{
    typedef std::codecvt_utf8<wchar_t> convert_type;
    std::wstring_convert<convert_type, wchar_t> converter;

    return converter.from_bytes(str);
}

/**
 * @brief Load class names from file
 * 
 * @param path Path to class names file
 * @return std::vector<std::string> Vector of class names
*/
std::vector<std::string> utils::loadNames(const std::string& path)
{
    // load class names
    std::vector<std::string> classNames;
    std::ifstream infile(path);
    if (infile.good())
    {
        std::string line;
        while (getline (infile, line))
        {
            if (line.back() == '\r')
                line.pop_back();
            classNames.emplace_back(line);
        }
        infile.close();
    }
    else
    {
        std::cerr << "ERROR: Failed to access class name path: " << path << std::endl;
    }

    return classNames;
}

/**
 * @brief Visualize detection result
 * 
 * @param image Image to draw on
 * @param detections Vector of detections
 * @param classNames Vector of class names
*/
void utils::visualizeDetection(cv::Mat& image, std::vector<Detection>& detections,
                               const std::vector<std::string>& classNames)
{
    for (const Detection& detection : detections)
    {
        cv::rectangle(image, detection.box, cv::Scalar(229, 160, 21), 2); // draw bounding box

        int x = detection.box.x;
        int y = detection.box.y;

        int conf = (int)std::round(detection.conf * 100);
        int classId = detection.classId;
        std::string label = classNames[classId] + " 0." + std::to_string(conf);

        // draw label
        int baseline = 0;
        cv::Size size = cv::getTextSize(label, cv::FONT_ITALIC, 0.8, 2, &baseline); // get text size
        cv::rectangle(image,
                      cv::Point(x, y - 25), cv::Point(x + size.width, y),
                      cv::Scalar(229, 160, 21), -1);

        cv::putText(image, label,
                    cv::Point(x, y - 3), cv::FONT_ITALIC,
                    0.8, cv::Scalar(255, 255, 255), 2);
    }
}

/**
 * @brief Visualize detection result
 * 
 * @param image Image to draw on
 * @param detection Detection
 * @param classNames Vector of class names
*/
void utils::visualizeDetection(cv::Mat& image, Detection& detection,
                            const std::vector<std::string>& classNames)
{
    cv::rectangle(image, detection.box, cv::Scalar(229, 160, 21), 2); // draw bounding box

    int x = detection.box.x;
    int y = detection.box.y;

    int conf = (int)std::round(detection.conf * 100);
    int classId = detection.classId;
    std::string label = classNames[classId] + " 0." + std::to_string(conf);

    // draw label
    int baseline = 0;
    cv::Size size = cv::getTextSize(label, cv::FONT_ITALIC, 0.8, 2, &baseline); // get text size
    cv::rectangle(image,
                    cv::Point(x, y - 25), cv::Point(x + size.width, y),
                    cv::Scalar(229, 160, 21), -1);

    cv::putText(image, label,
                cv::Point(x, y - 3), cv::FONT_ITALIC,
                0.8, cv::Scalar(255, 255, 255), 2);
}

/**
 * @brief Resize and pad image while meeting stride-multiple constraints
 * @param image Input image
 * @param outImage Output image
 * @param newShape New shape of output image
 * @param color Color of padding
 * @param auto_ Whether to automatically choose stride
 * @param scaleFill Whether to stretch image to new shape
 * @param scaleUp Whether to scale up image
 * @param stride Stride
*/
void utils::letterbox(const cv::Mat& image, cv::Mat& outImage,
                      const cv::Size& newShape = cv::Size(640, 640),
                      const cv::Scalar& color = cv::Scalar(114, 114, 114),
                      bool auto_ = true,
                      bool scaleFill = false,
                      bool scaleUp = true,
                      int stride = 32)
{
    cv::Size shape = image.size();
    float r = std::min((float)newShape.height / (float)shape.height,
                       (float)newShape.width / (float)shape.width);
    if (!scaleUp)
        r = std::min(r, 1.0f);

    float ratio[2] {r, r};
    int newUnpad[2] {(int)std::round((float)shape.width * r),
                     (int)std::round((float)shape.height * r)};

    // Compute padding
    auto dw = (float)(newShape.width - newUnpad[0]);
    auto dh = (float)(newShape.height - newUnpad[1]);

    if (auto_)
    {
        // Check if stride is multiple of padding, satisfy stride-multiple constraints
        dw = (float)((int)dw % stride);
        dh = (float)((int)dh % stride);
    }
    else if (scaleFill)
    {
        // Sterch image to new shape
        dw = 0.0f;
        dh = 0.0f;
        newUnpad[0] = newShape.width;
        newUnpad[1] = newShape.height;
        ratio[0] = (float)newShape.width / (float)shape.width;
        ratio[1] = (float)newShape.height / (float)shape.height;
    }

    dw /= 2.0f;
    dh /= 2.0f;

    if (shape.width != newUnpad[0] && shape.height != newUnpad[1])
    {
        cv::resize(image, outImage, cv::Size(newUnpad[0], newUnpad[1])); // Resize
    }

    int top = int(std::round(dh - 0.1f));
    int bottom = int(std::round(dh + 0.1f));
    int left = int(std::round(dw - 0.1f));
    int right = int(std::round(dw + 0.1f));
    cv::copyMakeBorder(outImage, outImage, top, bottom, left, right, cv::BORDER_CONSTANT, color); // Pad
}

/**
 * @brief transform coordinates from resized image to original image
 * 
 * @param imageShape Shape of resized image
 * @param coords coordinates to transform
 * @param imageOriginalShape Shape of original image
 */
void utils::scaleCoords(const cv::Size& imageShape,
                        cv::Rect& coords,
                        const cv::Size& imageOriginalShape)
{
    float ratio = std::min((float)imageShape.height / (float)imageOriginalShape.height,
                          (float)imageShape.width / (float)imageOriginalShape.width);

    int pad[2] = {(int) (( (float)imageShape.width - (float)imageOriginalShape.width * ratio) / 2.0f),
                  (int) (( (float)imageShape.height - (float)imageOriginalShape.height * ratio) / 2.0f)};

    coords.x = (int) std::round(((float)(coords.x - pad[0]) / ratio));
    coords.y = (int) std::round(((float)(coords.y - pad[1]) / ratio));

    coords.width = (int) std::round(((float)coords.width / ratio));
    coords.height = (int) std::round(((float)coords.height / ratio));

    // // clip coords, should be modified for width and height
    // coords.x = utils::clip(coords.x, 0, imageOriginalShape.width);
    // coords.y = utils::clip(coords.y, 0, imageOriginalShape.height);
    // coords.width = utils::clip(coords.width, 0, imageOriginalShape.width);
    // coords.height = utils::clip(coords.height, 0, imageOriginalShape.height);
}

// void utils::scaleCoords(const cv::Size& imgShape,
//                         cv::Rect& coords,
//                         const cv::Size& oriImgShape)
// {
//     float ratio = std::min((float)imgShape.height / (float)oriImgShape.height,
//                            (float)imgShape.width / (float)oriImgShape.width);

//     int pad[2] = {(int) (((float)imgShape.width - 
//                           (float)oriImgShape.width * ratio) / 2.0f),
//                   (int) (((float)imgShape.height - 
//                           (float)oriImgShape.height * ratio) / 2.0f)};

//     coords.x = (int) std::round(((float)(coords.x - pad[0]) / ratio));
//     coords.y = (int) std::round(((float)(coords.y - pad[1]) / ratio));

//     coords.width = (int) std::round(((float)coords.width / ratio));
//     coords.height = (int) std::round(((float)coords.height / ratio));
// }

template <typename T>
T utils::clip(const T& n, const T& lower, const T& upper)
{
    return std::max(lower, std::min(n, upper));
}
              