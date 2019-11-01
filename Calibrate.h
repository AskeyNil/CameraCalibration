#if !defined(CALIBRATE_H)
#define CALIBRATE_H

#include <opencv2/opencv.hpp>
using namespace cv;

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
                                    const String &saveFileName,
                                    int videoCount = 15, int videoIndex = 0);

/*
 * 从摄像头中标定圆网格数据
 * @param patternSize       标定图案尺寸
 * @param squareSize        大小
 * @param saveFileName      保存的文件名称
 * @param videoCount        检测的图片个数
 * @param videoIndex        摄像头设备的索引号
 */
void videoCalibrationWithCircles(const Size &patternSize, const int squareSize,
                                 const String &saveFileName,
                                 int videoCount = 15, int videoIndex = 0);

/*
 * 从摄像头中标定非对称圆网格数据
 * @param patternSize       标定图案尺寸
 * @param squareSize        大小
 * @param saveFileName      保存的文件名称
 * @param videoCount        检测的图片个数
 * @param videoIndex        摄像头设备的索引号
 */
void videoCalibrationWithACircles(const Size &patternSize, const int squareSize,
                                  const String &saveFileName,
                                  int videoCount = 15, int videoIndex = 0);

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
                                   const String &saveFileName);

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
                                const String &saveFileName);

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
                                 const String &saveFileName);

/*
 * 从图像中矫正畸变
 * @param filePath          畸变参数文件路径
 * @param img               源图
 * @param dst               修改后的图
 * @param isLarge           是否有大量数据需要修改
 */
void fileCorrectDistortion(const String &filePath, const Mat &img, Mat &dst,
                           bool isLarge = false);

/*
 * 从视频中矫正畸变
 * @param filePath          畸变参数文件路径
 * @param videoIndex        摄像头设备索引
 */
void videoCorrectDistortion(const String &filePath, int videoIndex = 0);

#endif // CALIBRATE_H
