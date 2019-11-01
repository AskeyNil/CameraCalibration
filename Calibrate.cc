#include "Calibrate.h"
#include <iostream>

using namespace std;

enum CalibMode {
    // 读取图片的模式

    // 从文件中读取
    FILES = 0,
    // 从摄像头中读取
    CAMERA = 1,

    // 标定板样式

    // 棋盘格
    CHESSBOARD = 0b010,
    // 圆网格
    CIRCLES = 0b100,
    // 非对称圆网格
    ACIRCLES = 0b110
};

/*
 * 查找角点
 * @param   img             灰度图
 * @param   corners         角点
 * @param   patternSize     匹配尺寸
 * @return  bool            是否找到角点
 */
bool findCorners(Mat &img, vector<Point2f> &corners, const Size &patternSize,
                 const CalibMode &mode) {
    // img 必须是灰度图
    if (img.channels() != 1) {
        cerr << "'img' 必须是灰度图！" << endl;
        throw Exception();
    }
    // eps 准确度
    // count 迭代次数
    static TermCriteria criteria(TermCriteria::EPS + TermCriteria::COUNT, 30,
                                 0.1);
    if ((mode & 0b100) == 0b100) {
        // 圆网格
        int flags = CALIB_CB_SYMMETRIC_GRID;
        if ((mode & 0b010) == 0b010) {
            //  非对称圆网格
            flags = CALIB_CB_ASYMMETRIC_GRID;
        }
        return findCirclesGrid(img, patternSize, corners, flags);
    } else {
        // 棋盘格
        bool patternFound =
            findChessboardCorners(img, patternSize, corners, 11);
        if (patternFound) {
            cornerSubPix(img, corners, Size(5, 5), Size(-1, -1), criteria);
            return true;
        }
    }
    return false;
}

/*
 * 查找角点
 * @param   imgPath         图片路径
 * @param   corners         角点
 * @param   patternSize     匹配尺寸
 * @return  bool            是否找到角点
 */
bool findCorners(String &imgPath, vector<Point2f> &corners,
                 const Size &patternSize, const CalibMode &mode) {
    Mat img = imread(imgPath, IMREAD_GRAYSCALE);
    if (img.empty()) {
        cerr << "文件路径有误" << endl;
        throw Exception();
    }
    return findCorners(img, corners, patternSize, mode);
}

/*
 * 标定摄像头
 * @param objectPoints
 * @param cornerPoints
 * @param imgSize
 * @param saveFileName
 */
void calibrateCamera(vector<vector<Point3f>> &objectPoints,
                     vector<vector<Point2f>> &cornerPoints, const Size &imgSize,
                     const String &saveFileName) {
    Mat cameraMatrix; // 相机参数矩阵
    Mat distCoeffs;   // 失真系数 distortion coefficients
    Mat rvecs;        // 图片旋转向量
    Mat tvecs;        // 图片平移向量
    double rms = calibrateCamera(objectPoints, cornerPoints, imgSize,
                                 cameraMatrix, distCoeffs, rvecs, tvecs);

    // 检查标定结果误差
    bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);
    if (ok) {
        cout << "正在保存到指定目录..." << endl;
        // 保存到指定目录
        FileStorage fs(saveFileName, FileStorage::WRITE);

        // 获取当前时间
        time_t tm;
        time(&tm);
        struct tm *t2 = localtime(&tm);
        char buf[1024];
        strftime(buf, sizeof(buf), "%c", t2);
        fs << "calibration_time" << buf;
        // 保存参数
        std::cout << rms << std::endl;
        fs << "rms" << rms;
        fs << "cameraMatrix" << cameraMatrix;
        fs << "distCoeffs" << distCoeffs;
        fs << "imgWidth" << imgSize.width << "imgHeight" << imgSize.height;
        fs.release();
    } else {
        std::cout << "误差过大" << std::endl;
    }
}
/*
 * 获取 Points
 * @param objectPoints
 * @param cornerPoints
 * @param corners
 * @param patternSize
 * @param squareSize
 * @param mode
 */
void obtainPoints(vector<vector<Point3f>> &objectPoints,
                  vector<vector<Point2f>> &cornerPoints,
                  vector<Point2f> &corners, const Size &patternSize,
                  const int squareSize, const CalibMode &mode) {
    static vector<Point3f> objPoint;
    static Size nowSize;
    if (nowSize != patternSize) {
        objPoint.clear();
        nowSize = patternSize;
        if ((mode & CalibMode::ACIRCLES) == CalibMode::ACIRCLES) {
            //  非对称圆网格
            for (int i = 0; i < patternSize.height; i++) {
                for (int j = 0; j < patternSize.width; j++) {
                    objPoint.emplace_back((2 * j + i % 2) * squareSize,
                                          i * squareSize, 0);
                }
            }
        } else {
            // 其他
            for (int i = 0; i < patternSize.height; i++) {
                for (int j = 0; j < patternSize.width; j++) {
                    objPoint.emplace_back(j * squareSize, i * squareSize, 0);
                }
            }
        }
    }
    objectPoints.push_back(objPoint);
    cornerPoints.push_back(corners);
}

/*
 * 从摄像头中标定数据
 * @param patternSize       标定图案尺寸
 * @param squareSize        大小
 * @param saveFileName      保存的文件名称
 * @param mode              标定模式
 * @param videoCount        检测的图片个数
 * @param videoIndex        摄像头设备的索引号
 */
void videoCalibration(const Size &patternSize, const int squareSize,
                      const String &saveFileName, const CalibMode &mode,
                      int videoCount = 5, int videoIndex = 0) {
    // 打开摄像头
    VideoCapture video(videoIndex);
    if (!video.isOpened()) {
        cerr << "摄像头打开失败" << endl;
        throw Exception();
    }
    vector<vector<Point3f>> objectPoints;
    vector<vector<Point2f>> cornerPoints;
    Size imgSize;
    int index{};
    while (index < videoCount) {
        Mat src, src_copy, gray;
        video >> src;
        imgSize = src.size();
        src.copyTo(src_copy);
        // 转化为灰度图
        cvtColor(src, gray, COLOR_BGR2GRAY);
        vector<Point2f> corners;
        bool found = findCorners(gray, corners, patternSize, mode);
        drawChessboardCorners(src_copy, patternSize, corners, found);
        imshow("video", src_copy);

        int key = waitKey(1000 / 100);

        switch (key) {
        case 32: // 按下 space
            if (found) {
                index += 1;
                cout << "当前共添加 " << index << " 个，需要添加 " << videoCount
                     << " 个!" << endl;
                //  添加到对应的数组中
                obtainPoints(objectPoints, cornerPoints, corners, patternSize,
                             squareSize, mode);
            } else {
                cout << "当前未检测到角点" << endl;
            }
            break;
        case 27: // 按下 ESC
            cout << "用户退出当前程序" << endl;
            return;
        default:
            break;
        }
    }
    // 获取结束，执行相机标定
    calibrateCamera(objectPoints, cornerPoints, imgSize, saveFileName);
}

/*
 * 从摄像头中标定棋盘格数据
 * @param patternSize       标定图案尺寸
 * @param squareSize        大小
 * @param saveFileName      保存的文件名称
 * @param videoCount        检测的图片个数
 * @param videoIndex        摄像头设备的索引号
 */
void videoCalibrationWithChessboard(const Size &patternSize,
                                    const int squareSize,
                                    const String &saveFileName, int videoCount,
                                    int videoIndex) {
    videoCalibration(patternSize, squareSize, saveFileName,
                     CalibMode::CHESSBOARD, videoCount, videoIndex);
}

/*
 * 从摄像头中标定圆网格数据
 * @param patternSize       标定图案尺寸
 * @param squareSize        大小
 * @param saveFileName      保存的文件名称
 * @param videoCount        检测的图片个数
 * @param videoIndex        摄像头设备的索引号
 */
void videoCalibrationWithCircles(const Size &patternSize, const int squareSize,
                                 const String &saveFileName, int videoCount,
                                 int videoIndex) {
    videoCalibration(patternSize, squareSize, saveFileName, CalibMode::CIRCLES,
                     videoCount, videoIndex);
}

/*
 * 从摄像头中标定非对称圆网格数据
 * @param patternSize       标定图案尺寸
 * @param squareSize        大小
 * @param saveFileName      保存的文件名称
 * @param videoCount        检测的图片个数
 * @param videoIndex        摄像头设备的索引号
 */
void videoCalibrationWithACircles(const Size &patternSize, const int squareSize,
                                  const String &saveFileName, int videoCount,
                                  int videoIndex) {
    videoCalibration(patternSize, squareSize, saveFileName, CalibMode::ACIRCLES,
                     videoCount, videoIndex);
}

/*
 * 从文件中标定数据
 * @param filePath          文件路径
 * @param patternSize       标定图案尺寸
 * @param squareSize        大小
 * @param saveFileName      保存的文件名称
 * @param mode              标定模式
 * @param videoCount        检测的图片个数
 * @param videoIndex        摄像头设备的索引号
 */
void fileCalibration(const String &filePath, const Size &patternSize,
                     const int squareSize, const String &saveFileName,
                     const CalibMode &mode) {
    vector<String> fileNames;
    vector<vector<Point3f>> objectPoints;
    vector<vector<Point2f>> cornerPoints;
    Size imgSize;
    glob(filePath, fileNames);
    for (auto &fileName : fileNames) {
        Mat img = imread(fileName, IMREAD_GRAYSCALE);
        imgSize = img.size();
        vector<Point2f> corners;
        bool found = findCorners(img, corners, patternSize, mode);
        if (found) {
            obtainPoints(objectPoints, cornerPoints, corners, patternSize,
                         squareSize, mode);
        }
    }
    // 获取结束，执行相机标定
    calibrateCamera(objectPoints, cornerPoints, imgSize, saveFileName);
}

/*
 * 从文件中标定棋盘格数据
 * @param filePath          文件路径
 * @param patternSize       标定图案尺寸
 * @param squareSize        大小
 * @param saveFileName      保存的文件名称
 * @param videoCount        检测的图片个数
 * @param videoIndex        摄像头设备的索引号
 */
void fileCalibrationWithChessboard(const String &filePath,
                                   const Size &patternSize,
                                   const int squareSize,
                                   const String &saveFileName) {
    fileCalibration(filePath, patternSize, squareSize, saveFileName,
                    CalibMode::CHESSBOARD);
}

/*
 * 从文件中标定圆网格数据
 * @param filePath          文件路径
 * @param patternSize       标定图案尺寸
 * @param squareSize        大小
 * @param saveFileName      保存的文件名称
 * @param videoCount        检测的图片个数
 * @param videoIndex        摄像头设备的索引号
 */
void fileCalibrationWithCircles(const String &filePath, const Size &patternSize,
                                const int squareSize,
                                const String &saveFileName) {
    fileCalibration(filePath, patternSize, squareSize, saveFileName,
                    CalibMode::CIRCLES);
}

/*
 * 从文件中标定非对称圆网格数据
 * @param filePath          文件路径
 * @param patternSize       标定图案尺寸
 * @param squareSize        大小
 * @param saveFileName      保存的文件名称
 * @param videoCount        检测的图片个数
 * @param videoIndex        摄像头设备的索引号
 */
void fileCalibrationWithACircles(const String &filePath,
                                 const Size &patternSize, const int squareSize,
                                 const String &saveFileName) {
    fileCalibration(filePath, patternSize, squareSize, saveFileName,
                    CalibMode::ACIRCLES);
}

// -------------------------  去畸变 -----------------------------------

void fileCorrectDistortion(const String &filePath, const Mat &img, Mat &dst,
                           bool isLarge) {
    static String files;
    static Mat cameraMatrix, distCoeffs;
    static Size imgSize;
    static bool getMap = false;
    if (files != filePath) {
        files = filePath;
        getMap = true;
        FileStorage fs(filePath, FileStorage::READ);
        if (!fs.isOpened()) {
            cerr << "文件有误.." << endl;
            throw Exception();
        }
        fs["cameraMatrix"] >> cameraMatrix;
        fs["distCoeffs"] >> distCoeffs;
        fs["imgWidth"] >> imgSize.width;
        fs["imgHeight"] >> imgSize.height;
        fs.release();
    }

    if (isLarge) {
        static Mat map1, map2;
        if (getMap) {
            initUndistortRectifyMap(cameraMatrix, distCoeffs, Mat(),
                                    cameraMatrix, imgSize, CV_16SC2, map1,
                                    map2);
        }
        remap(img, dst, map1, map2, INTER_LINEAR);

    } else {
        undistort(img, dst, cameraMatrix, distCoeffs);
    }
}

void videoCorrectDistortion(const String &filePath, int videoIndex) {
    // 打开摄像头
    VideoCapture video(videoIndex);
    if (!video.isOpened()) {
        cerr << "摄像头打开失败" << endl;
        throw Exception();
    }
    while (true) {
        Mat src;
        video >> src;
        imshow("video", src);
        fileCorrectDistortion(filePath, src, src, true);
        imshow("correct_later", src);
        int key = waitKey(1000 / 100);
        switch (key) {
        case 27: // 按下 ESC
            cout << "用户退出当前程序" << endl;
            return;
        default:
            break;
        }
    }
}