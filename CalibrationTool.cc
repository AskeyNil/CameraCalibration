
#include "Calibrate.h"
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char **argv) {
    int type{}, videoIndex{}, videoCount{}, spacing{};
    String filePath, saveFileName;
    Size patternSize;
    bool isFile;
    FileStorage fs("./_config_dedistort.yml", FileStorage::READ);
    fs["isFile"] >> isFile;
    fs["type"] >> type;
    fs["patternSize"]["width"] >> patternSize.width;
    fs["patternSize"]["height"] >> patternSize.height;
    fs["spacing"] >> spacing;
    fs["imgPath"] >> filePath;
    fs["saveFileName"] >> saveFileName;
    fs["videoIndex"] >> videoIndex;
    fs["videoCount"] >> videoCount;
    fs.release();

    if (isFile) {
        switch (type) {
        case 1:
            fileCalibrationWithChessboard(filePath, patternSize, spacing,
                                          saveFileName);
            break;
        case 2:
            fileCalibrationWithCircles(filePath, patternSize, spacing,
                                       saveFileName);
            break;
        case 3:
            fileCalibrationWithACircles(filePath, patternSize, spacing,
                                        saveFileName);
            break;
        default:
            std::cout << " type 参数输入有误" << std::endl;
            return -1;
        }
    } else {
        switch (type) {
        case 1:
            videoCalibrationWithChessboard(patternSize, spacing, saveFileName,
                                           videoCount, videoIndex);
            break;
        case 2:
            videoCalibrationWithCircles(patternSize, spacing, saveFileName,
                                        videoCount, videoIndex);

            break;
        case 3:
            videoCalibrationWithACircles(patternSize, spacing, saveFileName,
                                         videoCount, videoIndex);
            break;
        default:
            std::cout << " type 参数输入有误" << std::endl;
            return -1;
        }
    }
    return 0;
}
