
#include "Calibrate.h"
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char const *argv[]) {
    int videoIndex{};
    String filePath, imgPath;
    Size patternSize;
    bool isFile;
    FileStorage fs("./_config_dedistort.yml", FileStorage::READ);
    fs["isFile"] >> isFile;
    fs["filePath"] >> filePath;
    fs["imgPath"] >> imgPath;
    fs["videoIndex"] >> videoIndex;
    fs.release();
    if (isFile) {
        vector<String> result;
        glob(imgPath, result);
        bool isLarge = result.size() > 10 ? true : false;
        for (auto &imgFile : result) {
            Mat dst, img = imread(imgFile);
            fileCorrectDistortion(filePath, img, dst, isLarge);
        }
    } else {
        videoCorrectDistortion(filePath, videoIndex);
    }

    return 0;
}
