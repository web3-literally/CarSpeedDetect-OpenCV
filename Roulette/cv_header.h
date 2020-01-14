#pragma once
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//



#include <stdio.h>

// TODO: reference additional headers your program requires here

//////////////////////////////////////////////////////////////////////////
// std Includes and namespaces
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <deque>
#include <tchar.h>
#include <filesystem>

using namespace  std;

//////////////////////////////////////////////////////////////////////////
// leptonica Includes
//#include "leptonica\allheaders.h""

//////////////////////////////////////////////////////////////////////////
// tesseract Includes and namespaces
// #include "tesseract\baseapi.h"
// #include "tesseract\basedir.h"
// #include "tesseract\strngs.h"

//using namespace tesseract;

//////////////////////////////////////////////////////////////////////////
// OpenCV Includes and namespaces
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "opencv2/opencv.hpp"
#include "opencv/ml.h"
#include "opencv\cvaux.h"
#include <opencv2/core/core.hpp> // Basic OpenCV structures (cv::Mat)
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/videoio.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/core.hpp>
#include "opencv2/objdetect.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>

using namespace cv;
//#include <mysql.h>
//#include <atlconv.h>
//#include <atlbase.h>
//
//#define MAX_SIZE 65536
//#pragma comment(lib,"libmysql")
using namespace std;

#ifdef _DEBUG
#pragma comment(lib,"opencv_calib3d310d.lib")
#pragma comment(lib,"opencv_core310d.lib")
#pragma comment(lib,"opencv_features2d310d.lib")
#pragma comment(lib,"opencv_flann310d.lib")
#pragma comment(lib,"opencv_highgui310d.lib")
#pragma comment(lib,"opencv_imgcodecs310d.lib")
#pragma comment(lib,"opencv_imgproc310d.lib")
#pragma comment(lib,"opencv_ml310d.lib")
#pragma comment(lib,"opencv_objdetect310d.lib")
#pragma comment(lib,"opencv_photo310d.lib")
#pragma comment(lib,"opencv_shape310d.lib")
#pragma comment(lib,"opencv_stitching310d.lib")
#pragma comment(lib,"opencv_superres310d.lib")
#pragma comment(lib,"opencv_ts310d.lib")
#pragma comment(lib,"opencv_video310d.lib")
#pragma comment(lib,"opencv_videoio310d.lib")
#pragma comment(lib,"opencv_videostab310d.lib")


#else
#pragma comment(lib,"opencv_calib3d310.lib")
#pragma comment(lib,"opencv_core310.lib")
#pragma comment(lib,"opencv_features2d310.lib")
#pragma comment(lib,"opencv_flann310.lib")
#pragma comment(lib,"opencv_highgui310.lib")
#pragma comment(lib,"opencv_imgcodecs310.lib")
#pragma comment(lib,"opencv_imgproc310.lib")
#pragma comment(lib,"opencv_ml310.lib")
#pragma comment(lib,"opencv_objdetect310.lib")
#pragma comment(lib,"opencv_photo310.lib")
#pragma comment(lib,"opencv_shape310.lib")
#pragma comment(lib,"opencv_stitching310.lib")
#pragma comment(lib,"opencv_superres310.lib")
#pragma comment(lib,"opencv_ts310.lib")
#pragma comment(lib,"opencv_video310.lib")
#pragma comment(lib,"opencv_videoio310.lib")
#pragma comment(lib,"opencv_videostab310.lib")

#endif
//#pragma comment(lib,"liblept168.lib")
//#pragma comment(lib,"libtesseract302.lib")