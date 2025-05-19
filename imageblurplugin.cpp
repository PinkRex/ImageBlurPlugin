#include "imageblurplugin.h"

QString ImageBlurPlugin::name() {
    return "Blur";
}

void ImageBlurPlugin::edit(const cv::Mat &input, cv::Mat &output, QWidget* parent) {
    cv::Mat tmp;
    cv::blur(input, tmp, cv::Size(8, 8));
    output = tmp;
}
