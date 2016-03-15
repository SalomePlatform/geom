// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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

// TODO : All the following methods but ComputeContours use the C API of OpenCV while ComputContours
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
    cvReleaseImage(&src);
  }
}

/*!
  Computes the corners of the image located at imagePath
*/
void ShapeRec_FeatureDetector::ComputeCorners( bool useROI, ShapeRec_Parameters* parameters )
{
  ShapeRec_CornersParameters* aCornersParameters = dynamic_cast<ShapeRec_CornersParameters*>( parameters );
  if ( !aCornersParameters ) aCornersParameters = new  ShapeRec_CornersParameters();

  // Images to be used for detection
  IplImage *eig_img, *temp_img, *src_img_gray;
  
  // Load image
  src_img_gray = cvLoadImage (imagePath.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
  
  if ( useROI )
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
  cvGoodFeaturesToTrack (src_img_gray, eig_img, temp_img, corners, &cornerCount, aCornersParameters->qualityLevel, aCornersParameters->minDistance);
  cvFindCornerSubPix (src_img_gray, corners, cornerCount, cvSize (aCornersParameters->kernelSize, aCornersParameters->kernelSize), cvSize (-1, -1),
		      cvTermCriteria (aCornersParameters->typeCriteria, aCornersParameters->maxIter, aCornersParameters->epsilon));

  cvReleaseImage (&eig_img);
  cvReleaseImage (&temp_img);
  cvReleaseImage (&src_img_gray);

}

/*!
  Computes the contours of the image located at imagePath
*/
bool ShapeRec_FeatureDetector::ComputeContours( bool useROI, ShapeRec_Parameters* parameters )
{ 
  // Initialising images
  cv::Mat src, src_gray;
  cv::Mat detected_edges;
  
  // Read image
  src = cv::imread( imagePath.c_str() );
  if( !src.data )
    return false; 
  
  if ( !useROI )   // CANNY: The problem is that with that filter the detector detects double contours
  {   
    // Convert the image to grayscale
    if (src.channels() == 3)
      cv::cvtColor( src, src_gray, CV_BGR2GRAY );
    else if (src.channels() == 1)
      src_gray = src;

    ShapeRec_CannyParameters* aCannyParameters = dynamic_cast<ShapeRec_CannyParameters*>( parameters );
    if ( !aCannyParameters ) aCannyParameters = new ShapeRec_CannyParameters();

    // Reduce noise              
    blur( src_gray, detected_edges, cv::Size( aCannyParameters->kernelSize, aCannyParameters->kernelSize ) );
    // Canny detector
    Canny( detected_edges, detected_edges, aCannyParameters->lowThreshold, aCannyParameters->lowThreshold * aCannyParameters->ratio,
	   aCannyParameters->kernelSize, aCannyParameters->L2gradient );
  }
  else //COLORFILTER
  {
    // Load the input image where we want to detect contours
    IplImage* input_image = cvLoadImage(imagePath.c_str(),CV_LOAD_IMAGE_COLOR);

    ShapeRec_ColorFilterParameters* aColorFilterParameters = dynamic_cast<ShapeRec_ColorFilterParameters*>( parameters );
    if ( !aColorFilterParameters ) aColorFilterParameters = new ShapeRec_ColorFilterParameters();

    // Reduce noise
    cvSmooth( input_image, input_image, CV_GAUSSIAN, aColorFilterParameters->smoothSize, aColorFilterParameters->smoothSize );
  
    // Crop the image to the selected part only (sample_image)
    cvSetImageROI(input_image, rect);
    IplImage* sample_image = cvCreateImage(cvGetSize(input_image),
                                           input_image->depth,
                                           input_image->nChannels);
    cvCopy(input_image, sample_image, NULL);
    cvResetImageROI(input_image);
  
    IplImage* sample_hsv = cvCreateImage( cvGetSize(sample_image),8,3 );
    IplImage* sample_h_plane  = cvCreateImage( cvGetSize(sample_image), 8, 1 );
    IplImage* sample_s_plane = cvCreateImage( cvGetSize(sample_image), 8, 1 );
    CvHistogram* sample_hist;

    cvCvtColor(sample_image, sample_hsv, CV_BGR2HSV);
  
    cvCvtPixToPlane(sample_hsv, sample_h_plane, sample_s_plane, 0, 0);
    IplImage* sample_planes[] = { sample_h_plane, sample_s_plane };
  
    // Create the hue / saturation histogram of the SAMPLE image.
    // This histogramm will be representative of what is the zone
    // we want to find the frontier of. Indeed, the sample image is meant to 
    // be representative of this zone
    float hranges[] = { 0, 180 };
    float sranges[] = { 0, 256 };
    float* ranges[] = { hranges, sranges };
    sample_hist = cvCreateHist( 2, aColorFilterParameters->histSize, aColorFilterParameters->histType, ranges );
  
    //calculate hue /saturation histogram
    cvCalcHist(sample_planes, sample_hist, 0 ,0);

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
  
  
    // Calculate the back projection of hue and saturation planes of the INPUT image
    // by mean of the histogram of the SAMPLE image.
    //
    // The pixels which (h,s) coordinates correspond to high values in the histogram
    // will have high values in the grey image result. It means that a pixel of the INPUT image 
    // which is more probably in the zone represented by the SAMPLE image, will be whiter 
    // in the back projection.
    IplImage* backproject = cvCreateImage(cvGetSize(input_image), 8, 1);
    IplImage* binary_backproject = cvCreateImage(cvGetSize(input_image), 8, 1);
    IplImage* input_hsv = cvCreateImage(cvGetSize(input_image),8,3);
    IplImage* input_hplane = cvCreateImage(cvGetSize(input_image),8,1);
    IplImage* input_splane = cvCreateImage(cvGetSize(input_image),8,1);
  
    // Get hue and saturation planes of the INPUT image
    cvCvtColor(input_image, input_hsv, CV_BGR2HSV);
    cvCvtPixToPlane(input_hsv, input_hplane, input_splane, 0, 0);
    IplImage* input_planes[] = { input_hplane, input_splane };
    
    // Compute the back projection
    cvCalcBackProject(input_planes, backproject, sample_hist);
  
    // Threshold in order to obtain a binary image
    cvThreshold(backproject, binary_backproject, aColorFilterParameters->threshold, aColorFilterParameters->maxThreshold, CV_THRESH_BINARY);
    cvReleaseImage(&sample_image);
    cvReleaseImage(&sample_hsv);
    cvReleaseImage(&sample_h_plane);
    cvReleaseImage(&sample_s_plane);
    cvReleaseImage(&input_image);
    cvReleaseImage(&input_hsv);
    cvReleaseImage(&input_hplane);
    cvReleaseImage(&input_splane);
    cvReleaseImage(&backproject);
  
    detected_edges = cv::Mat(binary_backproject);
  }
  // else if ( detection_method == RIDGE_DETECTOR )  // Method adapted for engineering drawings (e.g. watershed functionnality could be used here cf.OpenCV documentation and samples)
  // {
  //   // TODO
  //   return false;
  // }

  //  _detectAndRetrieveContours( detected_edges, parameters->findContoursMethod );
  detected_edges = detected_edges > 1;
  findContours( detected_edges, contours, hierarchy, CV_RETR_CCOMP, parameters->findContoursMethod);

  return true;
  
}

/*!
  Computes the lines in the image located at imagePath
*/
bool ShapeRec_FeatureDetector::ComputeLines(){
  MESSAGE("ShapeRec_FeatureDetector::ComputeLines()")
  // Initialising images
  cv::Mat src, src_gray, detected_edges, dst;
  
  src=cv::imread(imagePath.c_str(), 0);
  
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
  
  cvReleaseImage(&src);
  cvReleaseImage(&cropped_image);
  
  return "/tmp/cropped_image.bmp";
}

/*!
  \class ShapeRec_CornersParameters
  \brief Parameters for the corners detection 
*/
ShapeRec_CornersParameters::ShapeRec_CornersParameters()
{
  qualityLevel = 0.2;
  minDistance = 1;
  typeCriteria = CV_TERMCRIT_ITER | CV_TERMCRIT_EPS;
  maxIter = 20;
  epsilon = 0.03;
}
ShapeRec_CornersParameters::~ShapeRec_CornersParameters()
{
}

/*!
  \class ShapeRec_Parameters
  \brief Parameters for the contour/corners detection 
*/
ShapeRec_Parameters::ShapeRec_Parameters()
{
  kernelSize = 3;
  findContoursMethod = CV_CHAIN_APPROX_NONE;
}
ShapeRec_Parameters::~ShapeRec_Parameters()
{
}

/*!
  \class ShapeRec_CannyParameters
  \brief Parameters for the contour detection 
*/
ShapeRec_CannyParameters::ShapeRec_CannyParameters()
{
  lowThreshold = 100; // is used for edge linking.
  ratio = 3;          // lowThreshold*ratio is used to find initial segments of strong edges
  L2gradient = true;  // norm L2 or L1
}

ShapeRec_CannyParameters::~ShapeRec_CannyParameters()
{
}

/*!
  \class ShapeRec_ColorFilterParameters
  \brief Parameters for the contour detection 
*/
ShapeRec_ColorFilterParameters::ShapeRec_ColorFilterParameters()
{
  smoothSize = 3;           // The parameter of the smoothing operation, the aperture width. Must be a positive odd number
  histSize = new int[2];    // array of the histogram dimension sizes
  histSize[0] = 30;         // hbins
  histSize[1] = 32;         // sbins
  histType = CV_HIST_ARRAY; // histogram representation format
  threshold = 128;          // threshold value
  maxThreshold = 255;       // maximum value to use with the THRESH_BINARY thresholding types
}

ShapeRec_ColorFilterParameters::~ShapeRec_ColorFilterParameters()
{
}
