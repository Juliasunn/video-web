#include "mediaProcessor.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <opencv2/core.hpp>
#include <opencv2/core/types_c.h> //for CV_StsError (not necessary)
#include "opencv2/video.hpp"
#include <opencv2/videoio.hpp> //for VideoCapture
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <boost/bind.hpp>

using namespace cv;
/*
This functions opens a video file and extracts the frames and put them into a vector of Mat(its the class for representing an img)
*/
void extractFrame(const Url &mpegUrl, std::shared_ptr<DiskStorage> mpegStorage, Mat& frame){
	try {
		//open the video file
  	VideoCapture cap; // open the video file

    //cap.open(mpegUrl);
   // (int(A::*)(int,int))&A::operator()
    auto openVcOverload = static_cast<bool(VideoCapture::*)(const String&, int)>(&VideoCapture::open);
    auto openFunction = boost::bind(openVcOverload, &cap, _1, CAP_ANY);
    mpegStorage->openFileWith(mpegUrl, openFunction);
    //cap.open(videoFilePath);
  	if(!cap.isOpened())  // check if we succeeded
  		CV_Error(CV_StsError, "Can not open mpeg url: " + mpegUrl);

    auto setected_frame = cap.get(CAP_PROP_FRAME_COUNT) / 2;
    std::cout << "Frame for preview index: " << setected_frame << std::endl;
    cap.set(CAP_PROP_POS_FRAMES, setected_frame);
    cap >> frame;

	
  	//cap.get(CV_CAP_PROP_FRAME_COUNT) contains the number of frames in the video;
 // 	for(auto frameNum = 0; frameNum < cap.get(CAP_PROP_FRAME_COUNT); frameNum++)
 // 	{
  //		Mat frame;
  //		cap >> frame; // get the next frame from video
  //		frames.push_back(frame);
 // 	}
  }
  catch( cv::Exception& e ){
    std::cerr << e.msg << std::endl;
    //cap.close();
    exit(1);
  }
 // cap.close();
	
}

/*
It saves a vector of frames into jpg images into the outputDir as 1.jpg,2.jpg etc where 1,2 etc represents the frame number
*/


std::string VideoProcessor::process(const Url &mpegUrl, std::shared_ptr<DiskStorage> mpegStorage) const {
    Mat frame;
    extractFrame(mpegUrl, mpegStorage, frame);

    auto previewUrl = m_diskStorage->createFile(DiskStorage::Extensions::Jpg);
    std::cout << "Mpeg file url: " << previewUrl << std::endl;

   // auto previewImgName = srcVideoPath.filename().stem().string() + ".jpg";
    //auto destImgPath = m_dstDir + previewImgName;

 //   auto isBracesOdd = std::count( destImgPath.begin(), destImgPath.end(), '\'') % 2;
  //  if ( isBracesOdd ) {
   //      destImgPath.append("'");
   // }

   // std::cout << "Preview file path: " << destImgPath << std::endl;

    std::vector<int> compression_params{IMWRITE_JPEG_QUALITY, 100};

    auto writer = boost::bind(imwrite, _1, frame, compression_params);
    m_diskStorage->writeToFile(previewUrl, writer);

	  //imwrite(destImgPath, frame, compression_params);
    return previewUrl;
}


