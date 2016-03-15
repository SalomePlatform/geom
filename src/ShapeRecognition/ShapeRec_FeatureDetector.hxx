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

// File   : ShapeRec_FeatureDetector.h
// Author : Renaud NEDELEC, Open CASCADE S.A.S.

// OpenCV includes
#include <cv.h>
#include <highgui.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

// Qt
#include <QRect>

#ifdef WIN32
  #if defined GEOM_SHAPEREC_EXPORTS || defined GEOMShapeRec_EXPORTS
    #define GEOM_SHAPEREC_EXPORT __declspec( dllexport )
  #else
    #define GEOM_SHAPEREC_EXPORT __declspec( dllimport )
  #endif
#else
   #define GEOM_SHAPEREC_EXPORT
#endif

class GEOM_SHAPEREC_EXPORT ShapeRec_Parameters
{
public:
  ShapeRec_Parameters();
  virtual ~ShapeRec_Parameters();

  int kernelSize;
  int findContoursMethod;
};

class GEOM_SHAPEREC_EXPORT ShapeRec_CornersParameters : public ShapeRec_Parameters
{
public:
  ShapeRec_CornersParameters();
  virtual ~ShapeRec_CornersParameters();

  double qualityLevel;
  double minDistance;
  int typeCriteria;
  int maxIter;
  double epsilon;
};

class GEOM_SHAPEREC_EXPORT ShapeRec_CannyParameters : public ShapeRec_Parameters
{
public:
  ShapeRec_CannyParameters();
  virtual ~ShapeRec_CannyParameters();
  
  int lowThreshold;
  int ratio;
  bool L2gradient;
};

class GEOM_SHAPEREC_EXPORT ShapeRec_ColorFilterParameters : public ShapeRec_Parameters
{
public:
  ShapeRec_ColorFilterParameters();
  virtual ~ShapeRec_ColorFilterParameters();

  int smoothSize;
  int* histSize;
  int histType;
  double threshold;
  double maxThreshold;
};

class GEOM_SHAPEREC_EXPORT ShapeRec_FeatureDetector
{
public:
  
  typedef std::vector<cv::Point>               CvContour;
  typedef std::vector<std::vector<cv::Point> > CvContoursArray;
  
  ShapeRec_FeatureDetector();                                            // Constructor
  
  void                    SetPath( const std::string& );                 // Sets the image path
  void                    SetROI( const QRect& );                        // Sets a Region Of Interest in the image
  CvPoint2D32f*           GetCorners()           { return corners;     };
  CvContoursArray         GetContours()          { return contours;    };
  std::vector<cv::Vec4i>  GetLines()             { return lines;       };
  std::vector<cv::Vec4i>  GetContoursHierarchy() { return hierarchy;   };
  int                     GetCornerCount()       { return cornerCount; };
  int                     GetImgHeight()         { return imgHeight;   };
  int                     GetImgWidth()          { return imgWidth;    };
  
  std::string             CroppImage();
  void                    ComputeCorners( bool useROI = false, ShapeRec_Parameters* parameters = 0 );  // Detects the corners from the image located at imagePath
  bool                    ComputeLines();                                                              // Detects the lines from the image located at imagePath
  bool                    ComputeContours( bool useROI = false, ShapeRec_Parameters* parameters = 0 ); // Detects the contours from the image located at imagePath
  
  
private:
  std::string             imagePath;
  
  CvPoint2D32f*           corners;
  int                     cornerCount;
  
  CvContoursArray         contours;
  std::vector<cv::Vec4i>  hierarchy;
  std::vector<cv::Vec4i>  lines;
  int                     imgHeight;
  int                     imgWidth; 
  CvRect                  rect;
};
