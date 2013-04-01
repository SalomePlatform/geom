// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// File   : ShapeRec_FeatureDetector.cxx
// Author : Renaud NEDELEC, Open CASCADE S.A.S.

#include "ShapeRec_FeatureDetector.hxx"
#include <stdio.h>
#include "utilities.h"

using namespace cv;

//TODO : All the following methods but ComputeContours use the C API of OpenCV while ComputContours
// uses the C++ API of the library.
// This should be homogenized and preferably by using the C++ API (which is more recent for all the methods

// The code has to be "cleaned up" too

/*!
  Constructor
  \param theFilename - image to process
*/
ShapeRec_FeatureDetector::ShapeRec_FeatureDetector(): 
  corners()
{
  cornerCount = 2000;
  rect=cvRect(0,0,0,0);
  imagePath = ""; //theFilename;
  // Store the dimensions of the picture
  imgHeight = 0;
  imgWidth  = 0;
}

/*!
  Sets the path of the image file to be processed
  \param thePath - Location of the image file 
*/
void ShapeRec_FeatureDetector::SetPath( const std::string& thePath )
{
  imagePath = thePath; 
  if (imagePath != "")
  {
    IplImage* src = cvLoadImage(imagePath.c_str(),CV_LOAD_IMAGE_COLOR);
    imgHeight = src->height;
    imgWidth = src->width; 
  }
}

/*!
  Computes the corners of the image located at imagePath
*/
void ShapeRec_FeatureDetector::ComputeCorners(){
  
  // Parameters for the corner detection
  double qualityLevel = 0.2;
  double minDistance = 1;
 
  // Images to be used for detection
  IplImage *eig_img, *temp_img, *src_img_gray;
  
  // Load image
  src_img_gray = cvLoadImage (imagePath.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
  
  if ( rect.width > 1 )
  {
    // If a ROI as been set use it for detection
    cvSetImageROI( src_img_gray, rect );
  }
  
  eig_img = cvCreateImage (cvGetSize (src_img_gray), IPL_DEPTH_32F, 1);
  temp_img = cvCreateImage (cvGetSize (src_img_gray), IPL_DEPTH_32F, 1);
  corners = (CvPoint2D32f *) cvAlloc (cornerCount * sizeof (CvPoint2D32f));
  
  // image height and width
  imgHeight = src_img_gray->height;
  imgWidth  = src_img_gray->width;

  // Corner detection using cvCornerMinEigenVal 
  // (one of the methods available inOpenCV, there is also a cvConerHarris method that can be used by setting a flag in cvGoodFeaturesToTrack)
  cvGoodFeaturesToTrack (src_img_gray, eig_img, temp_img, corners, &cornerCount, /*quality-level=*/qualityLevel, /*min-distance=*/minDistance);
  cvFindCornerSubPix (src_img_gray, corners, cornerCount,
                    cvSize (3, 3), cvSize (-1, -1), cvTermCriteria (CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03));

  cvReleaseImage (&eig_img);
  cvReleaseImage (&temp_img);
  cvReleaseImage (&src_img_gray);

}

/*!
  Computes the contours of the image located at imagePath
*/
bool ShapeRec_FeatureDetector::ComputeContours( int detection_method ){
 
  // Initialising images
  Mat src, src_gray;
  Mat detected_edges;
  
  // Read image
  src = imread( imagePath.c_str() );
  if( !src.data )
    return false; 
  
  if ( detection_method == CANNY )   // The problem is that with that filter the detector detects double contours
  {   
    // Thresholds for Canny detector
    int lowThreshold = 100;
    int ratio = 3;
    int kernel_size = 3; // 3,5 or 7
    
    // Convert the image to grayscale
    if (src.channels() == 3)
      cvtColor( src, src_gray, CV_BGR2GRAY );
    else if (src.channels() == 1)
      src_gray = src;
  
    // Reduce noise with a kernel 3x3               
    blur( src_gray, detected_edges, Size(3,3) );
    // Canny detector
    Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size, /*L2gradient =*/true );      
  }
  else if ( detection_method == COLORFILTER )
  {
    if ( !rect.width > 1 )
      return false;
    detected_edges = _colorFiltering();
  }
  else if ( detection_method == RIDGE_DETECTOR )  // Method adapted for engineering drawings (e.g. watershed functionnality could be used here cf.OpenCV documentation and samples)
  {
    // TODO
    return false;
  }
  _detectAndRetrieveContours( detected_edges );
  
  return true;
  
}

/*!
  Computes the lines in the image located at imagePath
*/
bool ShapeRec_FeatureDetector::ComputeLines(){
  MESSAGE("ShapeRec_FeatureDetector::ComputeLines()")
  // Initialising images
  Mat src, src_gray, detected_edges, dst;
  
  src=imread(imagePath.c_str(), 0);
  
  Canny( src, dst, 50, 200, 3 );
  HoughLinesP( dst, lines, 1, CV_PI/180, 80, 30, 10 );
  return true;
  
}

/*!
  Stores a region of interest given by user in rect
  \param theRect - Region Of Interest of the image located at imagePath 
*/
void ShapeRec_FeatureDetector::SetROI( const QRect& theRect )
{
  if (!theRect.isEmpty()){
    rect = cvRect(theRect.x(),theRect.y(),theRect.width(),theRect.height());
  }
}

/*!
  Crops the image located at imagePath to the region of interest given by the user via SetROI
  and stores the result in /tmp
  \param theRect - Region Of Interest of the image located at imagePath 
*/
std::string ShapeRec_FeatureDetector::CroppImage()
{
  IplImage* src = cvLoadImage(imagePath.c_str(),CV_LOAD_IMAGE_COLOR);
 
  cvSetImageROI(src, rect);
  IplImage* cropped_image = cvCreateImage(cvGetSize(src),
                                          src->depth,
                                          src->nChannels);
  cvCopy(src, cropped_image, NULL);
  cvResetImageROI(src);
  
  cvSaveImage ("/tmp/cropped_image.bmp", cropped_image);
  
  return "/tmp/cropped_image.bmp";
}


/*!
  Performs contours detection and store them in contours 
  \param binaryImg - src image to find contours of 
*/
void ShapeRec_FeatureDetector::_detectAndRetrieveContours( Mat binaryImg )
{
  binaryImg = binaryImg > 1; 
  int method = CV_CHAIN_APPROX_NONE;
  findContours( binaryImg, contours, hierarchy,CV_RETR_CCOMP, method);
  // Other possible approximations CV_CHAIN_APPROX_TC89_KCOS, CV_CHAIN_APPROX_TC89_L1, CV_CHAIN_APPROX_SIMPLE cf. OpenCV documentation 
  // for precise information
}

/*!
  Performs color filtering from the image sample contained in the ROI rect of the image 
  located at imagePath
  Thresholds the result in order ot obtain a binary image
  \return binary image resulting from filtering and thersholding
*/
Mat ShapeRec_FeatureDetector::_colorFiltering()
{  
  IplImage* find_image = cvLoadImage(imagePath.c_str(),CV_LOAD_IMAGE_COLOR);
  // Reduce noise with a kernel 3x3               
  cvSmooth( find_image, find_image, CV_GAUSSIAN, 3, 3 );
  
  if ( !rect.width > 1 )
    return Mat(find_image);
  
  // Crop the image to build an histogram from the selected part
  cvSetImageROI(find_image, rect);
  IplImage* test_image = cvCreateImage(cvGetSize(find_image),
                                       find_image->depth,
                                       find_image->nChannels);
  cvCopy(find_image, test_image, NULL);
  cvResetImageROI(find_image);
  
  IplImage* test_hsv = cvCreateImage(cvGetSize(test_image),8,3);
  IplImage* h_plane = cvCreateImage( cvGetSize(test_image), 8, 1 );
  IplImage* s_plane = cvCreateImage( cvGetSize(test_image), 8, 1 );
  CvHistogram* hist;

  cvCvtColor(test_image, test_hsv, CV_BGR2HSV);
  
  cvCvtPixToPlane(test_hsv, h_plane, s_plane, 0, 0);
  IplImage* planes[] = { h_plane, s_plane };
  
  //create hist
  int hbins = 30, sbins = 32;                        // TODO think to the best values here
  int   hist_size[] = { hbins, sbins };
  float hranges[] = { 0, 180 };
  float sranges[] = { 0, 255 };
  float* ranges[] = { hranges, sranges };
  hist = cvCreateHist(2, hist_size, CV_HIST_ARRAY, ranges, 1);
  
  //calculate hue /saturation histogram
  cvCalcHist(planes, hist, 0 ,0);

//   // TEST print of the histogram for debugging
//   IplImage* hist_image = cvCreateImage(cvSize(320,300),8,3);
//   
//   //draw hist on hist_test image.
//   cvZero(hist_image);
//   float max_value = 0;
//   cvGetMinMaxHistValue(hist, 0 , &max_value, 0, 0);
//   int bin_w = hist_image->width/size_hist;
//   for(int i = 0; i < size_hist; i++ )
//   {
//     //prevent overflow
//     int val = cvRound( cvGetReal1D(hist->bins,i)*hist_image->
//     height/max_value);
//     CvScalar color = CV_RGB(200,0,0);
//     //hsv2rgb(i*180.f/size_hist);
//     cvRectangle( hist_image, cvPoint(i*bin_w,hist_image->height),
//     cvPoint((i+1)*bin_w,hist_image->height - val),
//     color, -1, 8, 0 );
//   }
//  
//    
//   cvNamedWindow("hist", 1); cvShowImage("hist",hist_image);
  
  
  //calculate back projection of hue and saturation planes of input image
  IplImage* backproject = cvCreateImage(cvGetSize(find_image), 8, 1);
  IplImage* binary_backproject = cvCreateImage(cvGetSize(find_image), 8, 1);
  IplImage* find_hsv = cvCreateImage(cvGetSize(find_image),8,3);
  IplImage* find_hplane = cvCreateImage(cvGetSize(find_image),8,1);
  IplImage* find_splane = cvCreateImage(cvGetSize(find_image),8,1);
  
  cvCvtColor(find_image, find_hsv, CV_BGR2HSV);
  cvCvtPixToPlane(find_hsv, find_hplane, find_splane, 0, 0);
  IplImage* find_planes[] = { find_hplane, find_splane };
  cvCalcBackProject(find_planes, backproject, hist);
  
  // Threshold in order to obtain binary image
  cvThreshold(backproject, binary_backproject, 1, 255, CV_THRESH_BINARY);  // NOTE it would be good to think about the best threshold to use (it's 1 for now)
  cvReleaseImage(&test_image);
  cvReleaseImage(&test_hsv);
  cvReleaseImage(&h_plane);
  cvReleaseImage(&s_plane);
  cvReleaseImage(&find_image);
  cvReleaseImage(&find_hsv);
  cvReleaseImage(&find_hplane);
  cvReleaseImage(&find_splane);
  cvReleaseImage(&backproject);
  
  return Mat(binary_backproject);
}
