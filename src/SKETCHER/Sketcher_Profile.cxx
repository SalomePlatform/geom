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

//  GEOM SKETCHER : basic sketcher
//  File   : Sketcher_Profile.cxx
//  Author : Damien COQUERET
//  Module : GEOM

#include "Sketcher_Profile.hxx"

#include <TopoDS_Vertex.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

#include <GeomAPI.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Circle.hxx>

#include <Precision.hxx>
#include <gp_Pln.hxx>
#include <gp_Ax2.hxx>

#include "utilities.h"

/*!
  \class Sketcher_Profile::Functor
  \brief Generic functor class to process sketcher command
  \internal
*/

class Sketcher_Profile::Functor
{
public:
  Functor();
  virtual ~Functor();

  virtual void init( const TCollection_AsciiString& );

  virtual void initCommand() = 0;
  virtual void addPoint( const TCollection_AsciiString& x,
                         const TCollection_AsciiString& y ) = 0;
  virtual void addAngle( const TCollection_AsciiString& angle ) = 0;
  virtual void addSegmentParalX( const TCollection_AsciiString& x ) = 0;
  virtual void addSegmentParalXToZero() = 0;
  virtual void addSegmentParalY( const TCollection_AsciiString& y ) = 0;
  virtual void addSegmentParalYToZero() = 0;
  virtual void addSegmentAbsolute( const TCollection_AsciiString& x,
                                   const TCollection_AsciiString& y ) = 0;
  virtual void addSegmentRelative( const TCollection_AsciiString& dx,
                                   const TCollection_AsciiString& dy ) = 0;
  virtual void addSegmentLength( const TCollection_AsciiString& length ) = 0;
  virtual void addSegmentX( const TCollection_AsciiString& x,
                            int CurrentIndex ) = 0;
  virtual void addSegmentY( const TCollection_AsciiString& y,
                            int CurrentIndex ) = 0;
  virtual void addSegmentAngleLength( const TCollection_AsciiString& angle,
                                      const TCollection_AsciiString& length,
                                      int& CurrentIndex ) = 0;
  virtual void addSegmentAngleX( const TCollection_AsciiString& angle,
                                 const TCollection_AsciiString& x,
                                 int& CurrentIndex ) = 0;
  virtual void addSegmentAngleY( const TCollection_AsciiString& angle,
                                 const TCollection_AsciiString& y,
                                 int& CurrentIndex ) = 0;
  virtual void addSegmentDirectionLength( const TCollection_AsciiString& dx,
                                          const TCollection_AsciiString& dy,
                                          const TCollection_AsciiString& length,
                                          int& CurrentIndex ) = 0;
  virtual void addSegmentDirectionX( const TCollection_AsciiString& dx,
                                     const TCollection_AsciiString& dy,
                                     const TCollection_AsciiString& x,
                                     int& CurrentIndex ) = 0;
  virtual void addSegmentDirectionY( const TCollection_AsciiString& dx,
                                     const TCollection_AsciiString& dy,
                                     const TCollection_AsciiString& y,
                                     int& CurrentIndex ) = 0;
  virtual void addArcAbsolute( const TCollection_AsciiString& x,
                               const TCollection_AsciiString& y ) = 0;
  virtual void addArcRelative( const TCollection_AsciiString& dx,
                               const TCollection_AsciiString& dy ) = 0;
  virtual void addArcRadiusAbsolute( const TCollection_AsciiString& x,
                                     const TCollection_AsciiString& y,
                                     const TCollection_AsciiString& radius,
                                     const TCollection_AsciiString& flag ) = 0;
  virtual void addArcRadiusRelative( const TCollection_AsciiString& dx,
                                     const TCollection_AsciiString& dy,
                                     const TCollection_AsciiString& radius,
                                     const TCollection_AsciiString& flag ) = 0;
  virtual void addArcCenterAbsolute( const TCollection_AsciiString& x,
                                     const TCollection_AsciiString& y,
                                     const TCollection_AsciiString& xc,
                                     const TCollection_AsciiString& yc,
                                     const TCollection_AsciiString& flag1,
                                     const TCollection_AsciiString& flag2 ) = 0;
  virtual void addArcCenterRelative( const TCollection_AsciiString& dx,
                                     const TCollection_AsciiString& dy,
                                     const TCollection_AsciiString& xc,
                                     const TCollection_AsciiString& yc,
                                     const TCollection_AsciiString& flag1,
                                     const TCollection_AsciiString& flag2 ) = 0;
  virtual void addArcRadiusLength( const TCollection_AsciiString& radius,
                                   const TCollection_AsciiString& length ) = 0;
  virtual void addArcAngleRadiusLength( const TCollection_AsciiString& angle,
                                        const TCollection_AsciiString& radius,
                                        const TCollection_AsciiString& length ,
                                        int& CurrentIndex ) = 0;
  virtual void addArcDirectionRadiusLength( const TCollection_AsciiString& dx,
                                            const TCollection_AsciiString& dy,
                                            const TCollection_AsciiString& radius,
                                            const TCollection_AsciiString& length ,
                                            int& CurrentIndex ) = 0;
  virtual void closeWire() = 0;
  virtual void closeWireAndBuildFace() = 0;

  virtual void nextCommand( int& CurrentIndex ) = 0;
  virtual void makeResult() = 0;

  void setNumberOfCommand( int n );
  double error();
  bool isOk();

protected:
  int    myNumberOfCommand;
  double myError;
  bool   myOk;
};

/*!
  \class Sketcher_Profile::ShapeFunctor
  \brief Functor that creates a shape from sketcher command
  \internal
*/

class Sketcher_Profile::ShapeFunctor : public Functor
{
public:
  ShapeFunctor();

  virtual void initCommand();
  virtual void addPoint( const TCollection_AsciiString& x,
                         const TCollection_AsciiString& y );
  virtual void addAngle( const TCollection_AsciiString& angle );
  virtual void addSegmentParalX( const TCollection_AsciiString& x );
  virtual void addSegmentParalXToZero();
  virtual void addSegmentParalY( const TCollection_AsciiString& y );
  virtual void addSegmentParalYToZero();
  virtual void setAngle( const TCollection_AsciiString& angle );
  virtual void setDirection( const TCollection_AsciiString& dx,
                             const TCollection_AsciiString& dy );
  virtual void addSegmentAbsolute( const TCollection_AsciiString& x,
                                   const TCollection_AsciiString& y );
  virtual void addSegmentRelative( const TCollection_AsciiString& dx,
                                   const TCollection_AsciiString& dy );
  virtual void addSegmentLength( const TCollection_AsciiString& length );
  virtual void addSegmentX( const TCollection_AsciiString& x,
                            int CurrentIndex );
  virtual void addSegmentY( const TCollection_AsciiString& y,
                            int CurrentIndex );
  virtual void addSegmentAngleLength( const TCollection_AsciiString& angle,
                                      const TCollection_AsciiString& length,
                                      int& CurrentIndex );
  virtual void addSegmentAngleX( const TCollection_AsciiString& angle,
                                 const TCollection_AsciiString& x,
                                 int& CurrentIndex );
  virtual void addSegmentAngleY( const TCollection_AsciiString& angle,
                                 const TCollection_AsciiString& y,
                                 int& CurrentIndex );
  virtual void addSegmentDirectionLength( const TCollection_AsciiString& dx,
                                          const TCollection_AsciiString& dy,
                                          const TCollection_AsciiString& length,
                                          int& CurrentIndex );
  virtual void addSegmentDirectionX( const TCollection_AsciiString& dx,
                                     const TCollection_AsciiString& dy,
                                     const TCollection_AsciiString& x,
                                     int& CurrentIndex );
  virtual void addSegmentDirectionY( const TCollection_AsciiString& dx,
                                     const TCollection_AsciiString& dy,
                                     const TCollection_AsciiString& y,
                                     int& CurrentIndex );
  virtual void addArcAbsolute( const TCollection_AsciiString& x,
                               const TCollection_AsciiString& y );
  virtual void addArcRelative( const TCollection_AsciiString& dx,
                               const TCollection_AsciiString& dy );
  virtual void addArcRadiusAbsolute( const TCollection_AsciiString& x,
                                     const TCollection_AsciiString& y,
                                     const TCollection_AsciiString& radius,
                                     const TCollection_AsciiString& flag );
  virtual void addArcRadiusRelative( const TCollection_AsciiString& dx,
                                     const TCollection_AsciiString& dy,
                                     const TCollection_AsciiString& radius,
                                     const TCollection_AsciiString& flag );
  virtual void addArcCenterAbsolute( const TCollection_AsciiString& x,
                                     const TCollection_AsciiString& y,
                                     const TCollection_AsciiString& xc,
                                     const TCollection_AsciiString& yc,
                                     const TCollection_AsciiString& flag1,
                                     const TCollection_AsciiString& flag2 );
  virtual void addArcCenterRelative( const TCollection_AsciiString& dx,
                                     const TCollection_AsciiString& dy,
                                     const TCollection_AsciiString& xc,
                                     const TCollection_AsciiString& yc,
                                     const TCollection_AsciiString& flag1,
                                     const TCollection_AsciiString& flag2 );
  virtual void addArcRadiusLength( const TCollection_AsciiString& radius,
                                   const TCollection_AsciiString& length );
  virtual void addArcAngleRadiusLength( const TCollection_AsciiString& angle,
                                        const TCollection_AsciiString& radius,
                                        const TCollection_AsciiString& length ,
                                        int& CurrentIndex );
  virtual void addArcDirectionRadiusLength( const TCollection_AsciiString& dx,
                                            const TCollection_AsciiString& dy,
                                            const TCollection_AsciiString& radius,
                                            const TCollection_AsciiString& length ,
                                            int& CurrentIndex );
  virtual void closeWire();
  virtual void closeWireAndBuildFace();

  virtual void nextCommand( int& CurrentIndex );
  virtual void makeResult();

  TopoDS_Shape getShape();

private:
  void setMove( int& CurrentIndex );

private:
  Standard_Real myX0, myY0, myX, myY, myDx, myDy;
  Standard_Boolean myFirst, myStayFirst, myFace, myClose;
  Standard_Real myLength, myRadius, myAngle;

  enum {line, circle, point, none} myMove;

  TopLoc_Location TheLocation; // ?????????
  gp_Pln myPlane;
  TopoDS_Vertex myVertex;
  BRepBuilderAPI_MakeWire myMakeWire;

  TopoDS_Shape myShape;
};

/*!
  \class Sketcher_Profile::ShapeFunctor
  \brief Functor that generates a Python script from sketcher command
  \internal
*/

class Sketcher_Profile::DumpFunctor : public Functor
{
public:
  DumpFunctor();

  virtual void init( const TCollection_AsciiString& );

  virtual void initCommand();
  virtual void addPoint( const TCollection_AsciiString& x,
                         const TCollection_AsciiString& y );
  virtual void addAngle( const TCollection_AsciiString& angle );
  virtual void addSegmentParalX( const TCollection_AsciiString& x );
  virtual void addSegmentParalXToZero();
  virtual void addSegmentParalY( const TCollection_AsciiString& y );
  virtual void addSegmentParalYToZero();
  virtual void addSegmentAbsolute( const TCollection_AsciiString& x,
                                   const TCollection_AsciiString& y );
  virtual void addSegmentRelative( const TCollection_AsciiString& dx,
                                   const TCollection_AsciiString& dy );
  virtual void addSegmentLength( const TCollection_AsciiString& length );
  virtual void addSegmentX( const TCollection_AsciiString& x,
                            int CurrentIndex );
  virtual void addSegmentY( const TCollection_AsciiString& y,
                            int CurrentIndex );
  virtual void addSegmentAngleLength( const TCollection_AsciiString& angle,
                                      const TCollection_AsciiString& length,
                                      int& CurrentIndex );
  virtual void addSegmentAngleX( const TCollection_AsciiString& angle,
                                 const TCollection_AsciiString& x,
                                 int& CurrentIndex );
  virtual void addSegmentAngleY( const TCollection_AsciiString& angle,
                                 const TCollection_AsciiString& y,
                                 int& CurrentIndex );
  virtual void addSegmentDirectionLength( const TCollection_AsciiString& dx,
                                          const TCollection_AsciiString& dy,
                                          const TCollection_AsciiString& length,
                                          int& CurrentIndex );
  virtual void addSegmentDirectionX( const TCollection_AsciiString& dx,
                                     const TCollection_AsciiString& dy,
                                     const TCollection_AsciiString& x,
                                     int& CurrentIndex );
  virtual void addSegmentDirectionY( const TCollection_AsciiString& dx,
                                     const TCollection_AsciiString& dy,
                                     const TCollection_AsciiString& y,
                                     int& CurrentIndex );
  virtual void addArcAbsolute( const TCollection_AsciiString& x,
                               const TCollection_AsciiString& y );
  virtual void addArcRelative( const TCollection_AsciiString& dx,
                               const TCollection_AsciiString& dy );
  virtual void addArcRadiusAbsolute( const TCollection_AsciiString& x,
                                     const TCollection_AsciiString& y,
                                     const TCollection_AsciiString& radius,
                                     const TCollection_AsciiString& flag );
  virtual void addArcRadiusRelative( const TCollection_AsciiString& dx,
                                     const TCollection_AsciiString& dy,
                                     const TCollection_AsciiString& radius,
                                     const TCollection_AsciiString& flag );
  virtual void addArcCenterAbsolute( const TCollection_AsciiString& x,
                                     const TCollection_AsciiString& y,
                                     const TCollection_AsciiString& xc,
                                     const TCollection_AsciiString& yc,
                                     const TCollection_AsciiString& flag1,
                                     const TCollection_AsciiString& flag2 );
  virtual void addArcCenterRelative( const TCollection_AsciiString& dx,
                                     const TCollection_AsciiString& dy,
                                     const TCollection_AsciiString& xc,
                                     const TCollection_AsciiString& yc,
                                     const TCollection_AsciiString& flag1,
                                     const TCollection_AsciiString& flag2 );
  virtual void addArcRadiusLength( const TCollection_AsciiString& radius,
                                   const TCollection_AsciiString& length );
  virtual void addArcAngleRadiusLength( const TCollection_AsciiString& angle,
                                        const TCollection_AsciiString& radius,
                                        const TCollection_AsciiString& length ,
                                        int& CurrentIndex );
  virtual void addArcDirectionRadiusLength( const TCollection_AsciiString& dx,
                                            const TCollection_AsciiString& dy,
                                            const TCollection_AsciiString& radius,
                                            const TCollection_AsciiString& length ,
                                            int& CurrentIndex );
  virtual void closeWire();
  virtual void closeWireAndBuildFace();

  virtual void nextCommand( int& CurrentIndex );
  virtual void makeResult();

  TCollection_AsciiString getDescription();

private:
  TCollection_AsciiString myDescr;
  TCollection_AsciiString mySketcherEntry;
  TCollection_AsciiString myWPEntry;
  TCollection_AsciiString myTail;
  Standard_Boolean        myFace;
};

//===========================================================================
// Sketcher_Profile::Functor
//===========================================================================

/*!
  \brief Constructor
  \internal
*/
Sketcher_Profile::Functor::Functor() : myError( 0 ), myNumberOfCommand( 0 ), myOk( true )
{
}

/*!
  \brief Destructor
  \internal
*/
Sketcher_Profile::Functor::~Functor()
{
}

/*!
  \brief Initialize functor from the script
  \param command sketcher command being parsed
  \internal
*/
void Sketcher_Profile::Functor::init( const TCollection_AsciiString& /*command*/ )
{
}

/*!
  \brief Set total number of sketcher operators
  \param n total number of sketcher operators
  \internal
*/
void Sketcher_Profile::Functor::setNumberOfCommand( int n )
{
  myNumberOfCommand = n;
}

/*!
  \brief Get error (numerical value that describes, e.g. a deviation of point from the specified arc)
  \return numerical error
  \internal
*/
double Sketcher_Profile::Functor::error()
{
  return myError;
}

/*!
  \brief Get result of parsing
  \return \c true if parsing is successful or \c false otherwise
  \internal
*/
bool Sketcher_Profile::Functor::isOk()
{
  return myOk;
}

//===========================================================================
// Sketcher_Profile::ShapeFunctor
//===========================================================================

/*!
  \brief Constructor
  \internal
*/
Sketcher_Profile::ShapeFunctor::ShapeFunctor() : Functor()
{
  myX0 = myY0 = 0;
  myX  = myY  = 0;
  myDy = 0;
  myDx = 1;

  myFirst = Standard_True;
  myStayFirst = myFace = myClose = Standard_False;

  myLength = myRadius = myAngle = 0;

  myMove = point;

  myPlane = gp_Pln( gp_Ax3( gp::XOY() ) );
}

/*!
  \brief Prepare functor for processing of new sketcher operator
  \internal
*/
void Sketcher_Profile::ShapeFunctor::initCommand()
{
  myLength = myRadius = myAngle = 0;
  myMove = point;
}

/*!
  \brief Add point with absolute coordinates (\a x, \a y)
  \param x X coordinate
  \param y Y coordinate
  \internal
*/
void Sketcher_Profile::ShapeFunctor::addPoint( const TCollection_AsciiString& x,
                                               const TCollection_AsciiString& y )
{
  if ( !myFirst ) {
    MESSAGE("profile : The addPoint instruction must precede all moves");
    return;
  }
  myX0 = myX = x.RealValue();
  myY0 = myY = y.RealValue();
  myStayFirst = Standard_True;
}

/*!
  \brief Add angle
  \param angle angle
  \internal
*/
void Sketcher_Profile::ShapeFunctor::addAngle( const TCollection_AsciiString& angle )
{
  myAngle = angle.RealValue() * M_PI / 180.;
  myDx = Cos( myAngle );
  myDy = Sin( myAngle );
}

/*!
  \brief Add new segment of \a x length along X axis
  \param x length of segment
  \internal
*/
void Sketcher_Profile::ShapeFunctor::addSegmentParalX( const TCollection_AsciiString& x )
{
  myLength = x.RealValue();
  myDx = 1;
  myDy = 0;
  myMove = line;
}

/*!
  \brief Add new segment along X axis with X coordinate of end set to 0
  \internal
*/
void Sketcher_Profile::ShapeFunctor::addSegmentParalXToZero()
{
  myLength -= myX;
  myDx = 1;
  myDy = 0;
  myMove = line;
}

/*!
  \brief Add new segment of \a y length along Y axis
  \param y length of segment
  \internal
*/
void Sketcher_Profile::ShapeFunctor::addSegmentParalY( const TCollection_AsciiString& y )
{
  myLength = y.RealValue();
  myDx = 0;
  myDy = 1;
  myMove = line;
}

/*!
  \brief Add new segment along Y axis with Y coordinate of end set to 0
  \internal
*/
void Sketcher_Profile::ShapeFunctor::addSegmentParalYToZero()
{
  myLength -= myY;
  myDx = 0;
  myDy = 1;
  myMove = line;
}

/*!
  \brief Set current angle
  \param angle current working angle
  \internal
*/
void Sketcher_Profile::ShapeFunctor::setAngle( const TCollection_AsciiString& angle )
{
  myAngle = angle.RealValue();
  Standard_Real alpha = myAngle * M_PI / 180.;
  Standard_Real c = Cos( alpha );
  Standard_Real s = Sin( alpha );
  Standard_Real t = c * myDx - s * myDy;
  myDy = s * myDx + c * myDy;
  myDx = t;
}

/*!
  \brief Set current direction
  \param dx X component of direction vector
  \param dy Y component of direction vector
  \internal
*/
void Sketcher_Profile::ShapeFunctor::setDirection( const TCollection_AsciiString& dx,
                                                   const TCollection_AsciiString& dy )
{
  Standard_Real vx = dx.RealValue();
  Standard_Real vy = dy.RealValue();
  myLength = Sqrt( vx * vx + vy * vy );
  if ( myLength > Precision::Confusion() ) {
    myDx = vx / myLength;
    myDy = vy / myLength;
  }
  else
    myMove = none;
}

/*!
  \brief Add segment by absolute coordinates
  \param x X coordinate of segment end
  \param y Y coordinate of segment end
  \internal
*/
void Sketcher_Profile::ShapeFunctor::addSegmentAbsolute( const TCollection_AsciiString& x,
                                                         const TCollection_AsciiString& y )
{
  Standard_Real vx = x.RealValue() - myX;
  Standard_Real vy = y.RealValue() - myY;
  myLength = Sqrt( vx * vx + vy * vy );
  if ( myLength > Precision::Confusion() ) {
    myMove = line;
    myDx = vx / myLength;
    myDy = vy / myLength;
  }
  else
    myMove = none;
}

/*!
  \brief Add segment by relativ coordinates
  \param dx dX value specifing segment end
  \param dy dY value specifing segment end
  \internal
*/
void Sketcher_Profile::ShapeFunctor::addSegmentRelative( const TCollection_AsciiString& dx,
                                                         const TCollection_AsciiString& dy )
{
  Standard_Real vx = dx.RealValue();
  Standard_Real vy = dy.RealValue();
  myLength = Sqrt( vx * vx + vy * vy );
  if ( myLength > Precision::Confusion() ) {
    myMove = line;
    myDx = vx / myLength;
    myDy = vy / myLength;
  }
  else
    myMove = none;
}

/*!
  \brief Add segment with specified length along current direction
  \param length segment length
  \internal
*/
void Sketcher_Profile::ShapeFunctor::addSegmentLength( const TCollection_AsciiString& length )
{
  myLength = length.RealValue();
  if ( Abs( myLength ) > Precision::Confusion() )
    myMove = line;
  else
    myMove = none;
}

/*!
  \brief Add segment along X axis to reach specified X coordinate
  \param x X coordinate of segment end
  \param CurrentIndex index of current operator
  \internal
*/
void Sketcher_Profile::ShapeFunctor::addSegmentX( const TCollection_AsciiString& x,
                                                  int CurrentIndex )
{
  myMove = none;
  myLength = x.RealValue();
  if ( Abs( myDx ) < Precision::Confusion() ) {
    MESSAGE("profile : cannot intersect, arg "<<CurrentIndex-1);
    return;
  }
  myLength = ( myLength - myX ) / myDx;
  if ( Abs( myLength ) > Precision::Confusion() )
    myMove = line;
}

/*!
  \brief Add segment along Y axis to reach specified Y coordinate
  \param y Y coordinate of segment end
  \param CurrentIndex index of current operator
  \internal
*/
void Sketcher_Profile::ShapeFunctor::addSegmentY( const TCollection_AsciiString& y,
                                                  int CurrentIndex )
{
  myMove = none;
  myLength = y.RealValue();
  if ( Abs( myDy ) < Precision::Confusion() ) {
    MESSAGE("profile : cannot intersect, arg "<<CurrentIndex-1);
    return;
  }
  myLength = ( myLength - myY ) / myDy;
  if ( Abs( myLength ) > Precision::Confusion() )
    myMove = line;
}

/*!
  \brief Add segment by specified angle and length
  \param angle angle that specifies segment direction
  \param length segment length
  \param CurrentIndex index of current operator
  \internal
*/
void Sketcher_Profile::ShapeFunctor::addSegmentAngleLength( const TCollection_AsciiString& angle,
                                                            const TCollection_AsciiString& length,
                                                            int& CurrentIndex )
{
  setAngle( angle );
  setMove( CurrentIndex );
  addSegmentLength( length );
}

/*!
  \brief Add segment that crosses Y axis by specified angle and X coordinate
  \param angle angle that specifies segment direction
  \param x X coordinate of segment end
  \param CurrentIndex index of current operator
  \internal
*/
void Sketcher_Profile::ShapeFunctor::addSegmentAngleX( const TCollection_AsciiString& angle,
                                                       const TCollection_AsciiString& x,
                                                       int& CurrentIndex )
{
  setAngle( angle );
  setMove( CurrentIndex );
  addSegmentX( x, CurrentIndex );
}

/*!
  \brief Add segment that crosses X axis by specified angle and Y coordinate
  \param angle angle that specifies segment direction
  \param y Y coordinate of segment end
  \param CurrentIndex index of current operator
  \internal
*/
void Sketcher_Profile::ShapeFunctor::addSegmentAngleY( const TCollection_AsciiString& angle,
                                                       const TCollection_AsciiString& y,
                                                       int& CurrentIndex )
{
  setAngle( angle );
  setMove( CurrentIndex );
  addSegmentY( y, CurrentIndex );
}

/*!
  \brief Add segment by specified direction and length
  \param dx X component of direction vector
  \param dx Y component of direction vector
  \param length segment length
  \param CurrentIndex index of current operator
  \internal
*/
void Sketcher_Profile::ShapeFunctor::addSegmentDirectionLength( const TCollection_AsciiString& dx,
                                                                const TCollection_AsciiString& dy,
                                                                const TCollection_AsciiString& length,
                                                                int& CurrentIndex )
{
  setDirection( dx, dy );
  setMove( CurrentIndex );
  addSegmentLength( length );
}

/*!
  \brief Add segment by specified direction and X coordinate
  \param dx X component of direction vector
  \param dx Y component of direction vector
  \param x X coordinate of segment end
  \param CurrentIndex index of current operator
  \internal
*/
void Sketcher_Profile::ShapeFunctor::addSegmentDirectionX( const TCollection_AsciiString& dx,
                                                           const TCollection_AsciiString& dy,
                                                           const TCollection_AsciiString& x,
                                                           int& CurrentIndex )
{
  setDirection( dx, dy );
  setMove( CurrentIndex );
  addSegmentX( x, CurrentIndex );
}

/*!
  \brief Add segment by specified direction and Y coordinate
  \param dx X component of direction vector
  \param dx Y component of direction vector
  \param y Y coordinate of segment end
  \param CurrentIndex index of current operator
  \internal
*/
void Sketcher_Profile::ShapeFunctor::addSegmentDirectionY( const TCollection_AsciiString& dx,
                                                           const TCollection_AsciiString& dy,
                                                           const TCollection_AsciiString& y,
                                                           int& CurrentIndex )
{
  setDirection( dx, dy );
  setMove( CurrentIndex );
  addSegmentY( y, CurrentIndex );
}

/*!
  \brief Add arc along current direction vector by specified absolute coordinates
  \param x X coordinate of arc end
  \param x Y coordinate of arc end
  \internal
*/
void Sketcher_Profile::ShapeFunctor::addArcAbsolute( const TCollection_AsciiString& x,
                                                     const TCollection_AsciiString& y )
{
  Standard_Real vx = x.RealValue() - myX;
  Standard_Real vy = y.RealValue() - myY;
  Standard_Real det = myDx * vy - myDy * vx;
  Standard_Real c = Sqrt( ( myDx * myDx + myDy * myDy ) * ( vx * vx + vy * vy ) );
  if ( Abs( det ) > Precision::Confusion() && Abs( c ) > Precision::Confusion() ) {
    // Cosine of alpha = arc of angle / 2 , alpha in [0,Pi]
    c = ( myDx * vx + myDy * vy ) / c;
    // radius = distance between start and end point / 2 * sin(alpha)
    // radius is > 0 or < 0
    myRadius = ( vx * vx + vy * vy )* Sqrt( myDx * myDx + myDy * myDy ) / ( 2.0 * det );
    if ( Abs( myRadius ) > Precision::Confusion() ) {
      myAngle = 2.0 * acos( c ); // angle in [0,2Pi]
      myMove = circle;
    }
    else
      myMove = none;
  }
  else
    myMove = none;
}

/*!
  \brief Add arc along current direction vector by specified relative coordinates
  \param dx dX value specifing arc end
  \param dy dY value specifing arc end
  \internal
*/
void Sketcher_Profile::ShapeFunctor::addArcRelative( const TCollection_AsciiString& dx,
                                                     const TCollection_AsciiString& dy )
{
  Standard_Real vx = dx.RealValue();
  Standard_Real vy = dy.RealValue();
  Standard_Real det = myDx * vy - myDy * vx;
  Standard_Real c = Sqrt( ( myDx * myDx + myDy * myDy ) * ( vx * vx + vy * vy ) );
  if ( Abs( det ) > Precision::Confusion() && Abs( c ) > Precision::Confusion() ) {
    // Cosine of alpha = arc of angle / 2 , alpha in [0,Pi]
    c = ( myDx * vx + myDy * vy ) / c;
    // radius = distance between start and end point / 2 * sin(alpha)
    // radius is > 0 or < 0
    myRadius = ( vx * vx + vy * vy )* Sqrt( myDx * myDx + myDy * myDy ) / ( 2.0 * det );
    if ( Abs( myRadius ) > Precision::Confusion() ) {
      myAngle = 2.0 * acos(c); // angle in [0,2Pi]
      myMove = circle;
    }
    else
      myMove = none;
  }
  else
    myMove = none;
}

/*!
  \brief Add arc with given radius by specified absolute coordinates
  \param x X coordinate of arc end
  \param x Y coordinate of arc end
  \param radius arc radius
  \param flag reverse direction flag
  \internal
*/
void Sketcher_Profile::ShapeFunctor::addArcRadiusAbsolute( const TCollection_AsciiString& x,
                                                           const TCollection_AsciiString& y,
                                                           const TCollection_AsciiString& radius,
                                                           const TCollection_AsciiString& flag )
{
  Standard_Real vx = x.RealValue() - myX;
  Standard_Real vy = y.RealValue() - myY;
  myRadius  = radius.RealValue();
  int reversed = flag.IntegerValue();
  Standard_Real length = Sqrt( vx * vx + vy * vy );
  if ( Abs( myRadius ) > Precision::Confusion() &&
       ( 4.0 - ( vx * vx + vy * vy ) / ( myRadius * myRadius ) >= 0.0 ) && ( length > Precision::Confusion() ) ) {
    // Cosine of alpha = arc angle / 2 , alpha in [0,Pi/2]
    Standard_Real c = 0.5 * Sqrt( 4.0 - ( vx * vx + vy * vy ) / ( myRadius * myRadius ) );
    myAngle = 2.0 * acos( c ); // angle in [0,Pi]
    if ( reversed == 2 )
      myAngle = myAngle - 2 * M_PI;
    myDx =  0.5 * ( vy * 1.0 / myRadius
                    + vx * Sqrt( 4.0 / ( vx * vx + vy * vy ) - 1.0 / ( myRadius * myRadius ) ) );
    myDy = -0.5 * ( vx * 1.0 / myRadius
                    - vy * Sqrt( 4.0 / ( vx * vx + vy * vy ) - 1.0 / ( myRadius * myRadius ) ) );
    myMove = circle;
  }
  else {
    myMove = none;
  }
}

/*!
  \brief Add arc with given radius by specified relative coordinates
  \param dx dX value specifing arc end
  \param dy dY value specifing arc end
  \param radius arc radius
  \param flag reverse direction flag
  \internal
*/
void Sketcher_Profile::ShapeFunctor::addArcRadiusRelative( const TCollection_AsciiString& dx,
                                                           const TCollection_AsciiString& dy,
                                                           const TCollection_AsciiString& radius,
                                                           const TCollection_AsciiString& flag )
{
  Standard_Real vx = dx.RealValue();
  Standard_Real vy = dy.RealValue();
  myRadius  = radius.RealValue();
  int reversed = flag.IntegerValue();
  Standard_Real length = Sqrt( vx * vx + vy * vy );
  if ( Abs( myRadius ) > Precision::Confusion() &&
       ( 4.0 - ( vx * vx + vy * vy ) / ( myRadius * myRadius ) >= 0.0 ) && ( length > Precision::Confusion() ) ) {
    // Cosine of alpha = arc angle / 2 , alpha in [0,Pi/2]
    Standard_Real c = 0.5 * Sqrt( 4.0 - ( vx * vx + vy * vy ) / ( myRadius * myRadius ) );
    myAngle = 2.0 * acos( c ); // angle in [0,Pi]
    if ( reversed == 2 )
      myAngle = myAngle - 2 * M_PI;
    myDx =  0.5 * ( vy * 1.0 / myRadius
                    + vx * Sqrt( 4.0  / ( vx * vx + vy * vy ) - 1.0 / ( myRadius * myRadius ) ) );
    myDy = -0.5 * ( vx * 1.0 / myRadius
                    - vy * Sqrt( 4.0  / ( vx * vx + vy * vy ) - 1.0 / ( myRadius * myRadius ) ) );
    myMove = circle;
  }
  else {
    myMove = none;
  }
}

/*!
  \brief Add arc with given center by specified absolute coordinates
  \param x X coordinate of arc end
  \param x Y coordinate of arc end
  \param xc X coordinate of arc center
  \param yc Y coordinate of arc center
  \param flag1 reverse direction flag
  \param flag2 tolerance
  \internal
*/
void Sketcher_Profile::ShapeFunctor::addArcCenterAbsolute( const TCollection_AsciiString& x,
                                                           const TCollection_AsciiString& y,
                                                           const TCollection_AsciiString& xc,
                                                           const TCollection_AsciiString& yc,
                                                           const TCollection_AsciiString& flag1,
                                                           const TCollection_AsciiString& flag2 )
{
  Standard_Real vx = x.RealValue() - myX;
  Standard_Real vy = y.RealValue() - myY;
  Standard_Real vxc  = xc.RealValue() - myX;
  Standard_Real vyc  = yc.RealValue() - myY;
  int reversed = flag1.IntegerValue();
  int control_Tolerance = flag2.IntegerValue();

  myRadius = Sqrt( vxc * vxc + vyc * vyc );
  Standard_Real det = vx * vyc - vy * vxc;
  Standard_Real length = Sqrt( vx * vx + vy * vy );
  Standard_Real length2 = Sqrt( ( vx - vxc ) * ( vx - vxc ) + ( vy - vyc ) * ( vy - vyc ) );
  Standard_Real length3 = Sqrt( vxc * vxc + vyc * vyc );
  Standard_Real error = Abs( length2 - myRadius );
  myError = error;
  if ( error > Precision::Confusion() ) {
    MESSAGE("Warning : The specified end point is not on the Arc, distance = "<<error);
  }
  if ( error > Precision::Confusion() && control_Tolerance == 1 ) // Don't create the arc if the end point
    myMove = none;                                                // is too far from it
  else if ( ( length  > Precision::Confusion() ) &&
            ( length2 > Precision::Confusion() ) &&
            ( length3 > Precision::Confusion() ) ) {
    Standard_Real c = ( myRadius * myRadius - ( vx * vxc + vy * vyc ) )
      / ( myRadius * Sqrt( ( vx - vxc ) * ( vx - vxc ) + ( vy - vyc ) * ( vy - vyc ) ) ) ;  // Cosine of arc angle
    myAngle = acos(c);                                                                      // angle in [0,Pi]
    if ( reversed == 2 )
      myAngle = myAngle - 2 * M_PI;
    if ( det < 0 )
      myAngle = -myAngle;
    myDx =  vyc / myRadius;
    myDy = -vxc / myRadius;
    myMove = circle;
  }
  else {
    myMove = none;
  }
}

/*!
  \brief Add arc with given center by specified relative coordinates
  \param dx dX value specifing arc end
  \param dy dY value specifing arc end
  \param xc X coordinate of arc center
  \param yc Y coordinate of arc center
  \param flag1 reverse direction flag
  \param flag2 tolerance
  \internal
*/
void Sketcher_Profile::ShapeFunctor::addArcCenterRelative( const TCollection_AsciiString& dx,
                                                           const TCollection_AsciiString& dy,
                                                           const TCollection_AsciiString& xc,
                                                           const TCollection_AsciiString& yc,
                                                           const TCollection_AsciiString& flag1,
                                                           const TCollection_AsciiString& flag2 )
{
  Standard_Real vx = dx.RealValue();
  Standard_Real vy = dy.RealValue();
  Standard_Real vxc  = xc.RealValue();
  Standard_Real vyc  = yc.RealValue();
  int reversed = flag1.IntegerValue();
  int control_Tolerance = flag2.IntegerValue();
  myRadius = Sqrt( vxc * vxc + vyc * vyc );
  Standard_Real det = vx * vyc - vy * vxc;
  Standard_Real length = Sqrt( vx * vx + vy * vy );
  Standard_Real length2 = Sqrt( ( vx - vxc ) * ( vx - vxc ) + ( vy - vyc ) * ( vy - vyc ) );
  Standard_Real length3 = Sqrt( vxc * vxc + vyc * vyc );
  Standard_Real error = Abs( length2 - myRadius );
  myError = error;
  if ( error > Precision::Confusion() ) {
    MESSAGE("Warning : The specified end point is not on the Arc, distance = "<<error);
  }
  if ( error > Precision::Confusion() && control_Tolerance == 1 ) // Don't create the arc if the end point
    myMove = none;                                                // is too far from it
  else if ( ( length  > Precision::Confusion() ) &&
            ( length2 > Precision::Confusion() ) &&
            ( length3 > Precision::Confusion() ) ) {
    Standard_Real c = ( myRadius * myRadius - ( vx * vxc + vy * vyc ) )
      / ( myRadius * Sqrt( ( vx - vxc ) * ( vx - vxc ) + ( vy - vyc ) * ( vy - vyc ) ) ) ;  // Cosine of arc angle
    myAngle = acos( c );                                                                    // angle in [0,Pi]
    if ( reversed == 2 )
      myAngle = myAngle - 2 * M_PI;
    if ( det < 0 )
      myAngle = -myAngle;
    myDx =  vyc / myRadius;
    myDy = -vxc / myRadius;
    myMove = circle;
  }
  else {
    myMove = none;
  }
}

/*!
  \brief Add arc with given radius by specified length
  \param radius arc radius
  \param length arc length
  \internal
*/
void Sketcher_Profile::ShapeFunctor::addArcRadiusLength( const TCollection_AsciiString& radius,
                                                         const TCollection_AsciiString& length )
{
  myRadius = radius.RealValue();
  if ( Abs( myRadius ) > Precision::Confusion() ) {
    myAngle = length.RealValue() * M_PI / 180.;
    myMove = circle;
  }
  else
    myMove = none;
}

/*!
  \brief Add arc with given radius by specified angle and length
  \param angle angle between arc start tangent and current direction
  \param radius arc radius
  \param length arc length
  \param CurrentIndex index of current operator
  \internal
*/
void Sketcher_Profile::ShapeFunctor::addArcAngleRadiusLength( const TCollection_AsciiString& angle,
                                                              const TCollection_AsciiString& radius,
                                                              const TCollection_AsciiString& length ,
                                                              int& CurrentIndex )
{
  setAngle( angle );
  setMove( CurrentIndex );
  addArcRadiusLength( radius, length );
}

/*!
  \brief Add arc with given radius by specified direction and length
  \param dx X component of direction vector
  \param dx Y component of direction vector
  \param radius arc radius
  \param length arc length
  \param CurrentIndex index of current operator
  \internal
*/
void Sketcher_Profile::ShapeFunctor::addArcDirectionRadiusLength( const TCollection_AsciiString& dx,
                                                                  const TCollection_AsciiString& dy,
                                                                  const TCollection_AsciiString& radius,
                                                                  const TCollection_AsciiString& length ,
                                                                  int& CurrentIndex )
{
  setDirection( dx, dy );
  setMove( CurrentIndex );
  addArcRadiusLength( radius, length );
}

/*!
  \brief Close wire
  \internal
*/
void Sketcher_Profile::ShapeFunctor::closeWire()
{
  myClose = Standard_True;
}

/*!
  \brief Close wire and build face
  \internal
*/
void Sketcher_Profile::ShapeFunctor::closeWireAndBuildFace()
{
  myClose = Standard_True;
  myFace = Standard_True;
}

/*!
  \brief Set internal parameters according to the current operator type
  \param CurrentIndex index of current operator
  \internal
*/
void Sketcher_Profile::ShapeFunctor::setMove( int& CurrentIndex )
{
  switch ( myMove )
  {
  case line :
    {
      if ( myLength < 0 ) {
        myLength = -myLength;
        myDx = -myDx;
        myDy = -myDy;
      }
      Handle(Geom2d_Line) l = new Geom2d_Line( gp_Pnt2d( myX, myY ),gp_Dir2d( myDx, myDy ) );
      BRepBuilderAPI_MakeEdge ME( GeomAPI::To3d( l, myPlane ), 0, myLength );
      if ( !ME.IsDone() )
        return;
      myMakeWire.Add( ME );
      myX += myLength * myDx;
      myY += myLength * myDy;
      break;
    }
  case circle :
    {
      Standard_Boolean sense = Standard_True;
      if ( myRadius < 0 ) {
        myRadius = -myRadius;
        sense = !sense;
        myDx = -myDx;
        myDy = -myDy;
      }
      gp_Ax2d ax( gp_Pnt2d( myX - myRadius * myDy, myY + myRadius * myDx ), gp_Dir2d( myDy, -myDx ) );
      if ( myAngle < 0 ) {
        myAngle = -myAngle;
        sense = !sense;
      }
      Handle(Geom2d_Circle) c = new Geom2d_Circle( ax, myRadius, sense );
      BRepBuilderAPI_MakeEdge ME( GeomAPI::To3d( c, myPlane ), 0, myAngle );
      if ( !ME.IsDone() )
        return;
      myMakeWire.Add( ME );
      gp_Pnt2d p;
      gp_Vec2d v;
      c->D1( myAngle, p, v );
      myX = p.X();
      myY = p.Y();
      myDx = v.X() / myRadius;
      myDy = v.Y() / myRadius;
      break;
    }
  case point:
    {
      myVertex = BRepBuilderAPI_MakeVertex( gp_Pnt( myX, myY, 0.0 ) );
      break;
    }
  case none:
    {
      CurrentIndex = myNumberOfCommand - 1;
      break;
    }
  }
}

/*!
  \brief Complete parsing of current operator
  \param CurrentIndex index of current operator
  \internal
*/
void Sketcher_Profile::ShapeFunctor::nextCommand( int& CurrentIndex )
{
  setMove( CurrentIndex );
  CurrentIndex ++;
  // update first
  myFirst = myStayFirst;
  myStayFirst = Standard_False;

  // next segment....
  if ( ( CurrentIndex == myNumberOfCommand ) && myClose ) {
    // the closing segment
    myDx = myX0 - myX;
    myDy = myY0 - myY;
    myLength = Sqrt( myDx * myDx + myDy * myDy );
    myMove = line;
    if ( myLength > Precision::Confusion() ) {
      myDx = myDx / myLength;
      myDy = myDy / myLength;
      setMove( CurrentIndex );
    }
  }
}

/*!
  \brief Finish parsing and create result
  \internal
*/
void Sketcher_Profile::ShapeFunctor::makeResult()
{
  // get the result, face or wire
  if ( myMove == none ) {
    myOk = false;
    return;
  }
  else if ( myMove == point ) {
    myShape = myVertex;
  }
  else if ( myFace ) {
    if ( !myMakeWire.IsDone() ) {
      myOk = false;
      return;
    }
    BRepBuilderAPI_MakeFace MF ( myPlane, myMakeWire.Wire() );
    if ( !MF.IsDone() ) {
      myOk = false;
      return;
    }
    myShape = MF;
  }
  else {
    if ( !myMakeWire.IsDone() ) {
      myOk = false;
      return;
    }
    myShape = myMakeWire.Shape();
  }

  if ( !TheLocation.IsIdentity() )
    myShape.Move( TheLocation );
}

/*!
  \brief Get resulting shape
  \return shape resulting from parsing of sketcher command
  \internal
*/
TopoDS_Shape Sketcher_Profile::ShapeFunctor::getShape()
{
  return myShape;
}

//===========================================================================
// Sketcher_Profile::DumpFunctor
//===========================================================================

/*!
  \brief Constructor
  \internal
*/
Sketcher_Profile::DumpFunctor::DumpFunctor()
{
  myFace = Standard_False;
}

/*!
  \brief Initialize functor from the script
  \param command sketcher command being parsed
  \internal
*/
void Sketcher_Profile::DumpFunctor::init( const TCollection_AsciiString& command )
{
  // parse only first line of the script
  TCollection_AsciiString aSubStr = command.Token( "\n\t" );
  if ( aSubStr.Length() < command.Length() )
    myTail = command.SubString( aSubStr.Length()+1, command.Length() );
  // get sketch entry
  mySketcherEntry = aSubStr.Token( " =" );
  // Using this Working Plane for Sketcher
  myWPEntry = myWPEntry + "[" + aSubStr.Token( "[]", 2 ) + "]";
  // Using this Working Plane for SketcherOnPlane
  if ( aSubStr.Search( "MakeSketcherOnPlane" ) != -1 ) {
    myWPEntry = aSubStr.Token( ",)", 2 );
    myWPEntry.RemoveAll( ' ' );
  }
  myDescr += "sk = geompy.Sketcher2D()";
}

/*!
  \brief Prepare functor for processing of new sketcher operator
  \internal
*/
void Sketcher_Profile::DumpFunctor::initCommand()
{
  myDescr += "\n\t";
}

/*!
  \brief Add point with absolute coordinates (\a x, \a y)
  \param x X coordinate
  \param y Y coordinate
  \internal
*/
void Sketcher_Profile::DumpFunctor::addPoint( const TCollection_AsciiString& x,
                                              const TCollection_AsciiString& y )
{
  myDescr += "sk.addPoint(";
  myDescr += x + ", " + y + ")";
}

/*!
  \brief Add angle
  \param angle angle
  \internal
*/
void Sketcher_Profile::DumpFunctor::addAngle( const TCollection_AsciiString& angle )
{
  myDescr += "sk.addAngle(";
  myDescr += angle + ")";
}

/*!
  \brief Add new segment of \a x length along X axis
  \param x length of segment
  \internal
*/
void Sketcher_Profile::DumpFunctor::addSegmentParalX( const TCollection_AsciiString& x )
{
  myDescr += "sk.addSegmentParalX(";
  myDescr += x + ")";
}

/*!
  \brief Add new segment along X axis with X coordinate of end set to 0
  \internal
*/
void Sketcher_Profile::DumpFunctor::addSegmentParalXToZero()
{
  myDescr += "sk.addSegmentParalXToZero()";
}

/*!
  \brief Add new segment of \a y length along Y axis
  \param y length of segment
  \internal
*/
void Sketcher_Profile::DumpFunctor::addSegmentParalY( const TCollection_AsciiString& y )
{
  myDescr += "sk.addSegmentParalY(";
  myDescr += y + ")";
}

/*!
  \brief Add new segment along Y axis with Y coordinate of end set to 0
  \internal
*/
void Sketcher_Profile::DumpFunctor::addSegmentParalYToZero()
{
  myDescr += "sk.addSegmentParalYToZero()";
}

/*!
  \brief Add segment by absolute coordinates
  \param x X coordinate of segment end
  \param y Y coordinate of segment end
  \internal
*/
void Sketcher_Profile::DumpFunctor::addSegmentAbsolute( const TCollection_AsciiString& x,
                                                        const TCollection_AsciiString& y )
{
  myDescr += "sk.addSegmentAbsolute(";
  myDescr += x + ", " + y + ")";
}

/*!
  \brief Add segment by relativ coordinates
  \param dx dX value specifing segment end
  \param dy dY value specifing segment end
  \internal
*/
void Sketcher_Profile::DumpFunctor::addSegmentRelative( const TCollection_AsciiString& dx,
                                                        const TCollection_AsciiString& dy )
{
  myDescr += "sk.addSegmentRelative(";
  myDescr += dx + ", " + dy + ")";
}

/*!
  \brief Add segment with specified length along current direction
  \param length segment length
  \internal
*/
void Sketcher_Profile::DumpFunctor::addSegmentLength( const TCollection_AsciiString& length )
{
  myDescr += "sk.addSegmentLength(";
  myDescr += length + ")";
}

/*!
  \brief Add segment along X axis to reach specified X coordinate
  \param x X coordinate of segment end
  \param CurrentIndex index of current operator
  \internal
*/
void Sketcher_Profile::DumpFunctor::addSegmentX( const TCollection_AsciiString& x,
                                                 int CurrentIndex )
{
  myDescr += "sk.addSegmentX(";
  myDescr += x + ")";
}

/*!
  \brief Add segment along Y axis to reach specified Y coordinate
  \param y Y coordinate of segment end
  \param CurrentIndex index of current operator
  \internal
*/
void Sketcher_Profile::DumpFunctor::addSegmentY( const TCollection_AsciiString& y,
                                                 int CurrentIndex  )
{
  myDescr += "sk.addSegmentY(";
  myDescr += y + ")";
}

/*!
  \brief Add segment by specified angle and length
  \param angle angle that specifies segment direction
  \param length segment length
  \param CurrentIndex index of current operator
  \internal
*/
void Sketcher_Profile::DumpFunctor::addSegmentAngleLength( const TCollection_AsciiString& angle,
                                                           const TCollection_AsciiString& length,
                                                           int& CurrentIndex )
{
  double aAngle = angle.RealValue();
  if ( aAngle == 90 ) {
    myDescr += "sk.addSegmentPerpLength(";
    myDescr += length + ")";
  }
  else {
    myDescr += "sk.addSegmentAngleLength(";
    myDescr += angle + ", " + length + ")";
  }
}

/*!
  \brief Add segment that crosses Y axis by specified angle and X coordinate
  \param angle angle that specifies segment direction
  \param x X coordinate of segment end
  \param CurrentIndex index of current operator
  \internal
*/
void Sketcher_Profile::DumpFunctor::addSegmentAngleX( const TCollection_AsciiString& angle,
                                                      const TCollection_AsciiString& x,
                                                      int& CurrentIndex )
{
  double aAngle = angle.RealValue();
  if ( aAngle == 90 ) {
    myDescr += "sk.addSegmentPerpX(";
    myDescr += x + ")";
  }
  else {
    myDescr += "sk.addSegmentAngleX(";
    myDescr += angle + ", " + x + ")";
  }
}

/*!
  \brief Add segment that crosses X axis by specified angle and Y coordinate
  \param angle angle that specifies segment direction
  \param y Y coordinate of segment end
  \param CurrentIndex index of current operator
  \internal
*/
void Sketcher_Profile::DumpFunctor::addSegmentAngleY( const TCollection_AsciiString& angle,
                                                      const TCollection_AsciiString& y,
                                                      int& CurrentIndex )
{
  double aAngle = angle.RealValue();
  if ( aAngle == 90 ) {
    myDescr += "sk.addSegmentPerpY(";
    myDescr += y + ")";
  }
  else {
    myDescr += "sk.addSegmentAngleY(";
    myDescr += angle + ", " + y + ")";
  }
}

/*!
  \brief Add segment by specified direction and length
  \param dx X component of direction vector
  \param dx Y component of direction vector
  \param length segment length
  \param CurrentIndex index of current operator
  \internal
*/
void Sketcher_Profile::DumpFunctor::addSegmentDirectionLength( const TCollection_AsciiString& dx,
                                                               const TCollection_AsciiString& dy,
                                                               const TCollection_AsciiString& length,
                                                               int& CurrentIndex )
{
  myDescr += "sk.addSegmentDirectionLength(";
  myDescr += dx + ", " + dy + ", " + length + ")";
}

/*!
  \brief Add segment by specified direction and X coordinate
  \param dx X component of direction vector
  \param dx Y component of direction vector
  \param x X coordinate of segment end
  \param CurrentIndex index of current operator
  \internal
*/
void Sketcher_Profile::DumpFunctor::addSegmentDirectionX( const TCollection_AsciiString& dx,
                                                          const TCollection_AsciiString& dy,
                                                          const TCollection_AsciiString& x,
                                                          int& CurrentIndex )
{
  myDescr += "sk.addSegmentDirectionX(";
  myDescr += dx + ", " + dy + ", " + x + ")";
}

/*!
  \brief Add segment by specified direction and Y coordinate
  \param dx X component of direction vector
  \param dx Y component of direction vector
  \param y Y coordinate of segment end
  \param CurrentIndex index of current operator
  \internal
*/
void Sketcher_Profile::DumpFunctor::addSegmentDirectionY( const TCollection_AsciiString& dx,
                                                          const TCollection_AsciiString& dy,
                                                          const TCollection_AsciiString& y,
                                                          int& CurrentIndex )
{
  myDescr += "sk.addSegmentDirectionY(";
  myDescr += dx + ", " + dy + ", " + y + ")";
}

/*!
  \brief Add arc along current direction vector by specified absolute coordinates
  \param x X coordinate of arc end
  \param x Y coordinate of arc end
  \internal
*/
void Sketcher_Profile::DumpFunctor::addArcAbsolute( const TCollection_AsciiString& x,
                                                    const TCollection_AsciiString& y )
{
  myDescr += "sk.addArcAbsolute(";
  myDescr += x + ", " + y + ")";
}

/*!
  \brief Add arc along current direction vector by specified relative coordinates
  \param dx dX value specifing arc end
  \param dy dY value specifing arc end
  \internal
*/
void Sketcher_Profile::DumpFunctor::addArcRelative( const TCollection_AsciiString& dx,
                                                    const TCollection_AsciiString& dy )
{
  myDescr += "sk.addArcRelative(";
  myDescr += dx + ", " + dy + ")";
}

/*!
  \brief Add arc with given radius by specified absolute coordinates
  \param x X coordinate of arc end
  \param x Y coordinate of arc end
  \param radius arc radius
  \param flag reverse direction flag
  \internal
*/
void Sketcher_Profile::DumpFunctor::addArcRadiusAbsolute( const TCollection_AsciiString& x,
                                                          const TCollection_AsciiString& y,
                                                          const TCollection_AsciiString& radius,
                                                          const TCollection_AsciiString& flag )
{
  myDescr += "sk.addArcRadiusAbsolute(";
  myDescr += x + ", " + y + ", " + radius + ", " + flag + ")";
}

/*!
  \brief Add arc with given radius by specified relative coordinates
  \param dx dX value specifing arc end
  \param dy dY value specifing arc end
  \param radius arc radius
  \param flag reverse direction flag
  \internal
*/
void Sketcher_Profile::DumpFunctor::addArcRadiusRelative( const TCollection_AsciiString& dx,
                                                          const TCollection_AsciiString& dy,
                                                          const TCollection_AsciiString& radius,
                                                          const TCollection_AsciiString& flag )
{
  myDescr += "sk.addArcRadiusRelative(";
  myDescr += dx + ", " + dy + ", " + radius + ", " + flag + ")";
}

/*!
  \brief Add arc with given center by specified absolute coordinates
  \param x X coordinate of arc end
  \param x Y coordinate of arc end
  \param xc X coordinate of arc center
  \param yc Y coordinate of arc center
  \param flag1 reverse direction flag
  \param flag2 tolerance
  \internal
*/
void Sketcher_Profile::DumpFunctor::addArcCenterAbsolute( const TCollection_AsciiString& x,
                                                          const TCollection_AsciiString& y,
                                                          const TCollection_AsciiString& xc,
                                                          const TCollection_AsciiString& yc,
                                                          const TCollection_AsciiString& flag1,
                                                          const TCollection_AsciiString& flag2 )
{
  myDescr += "sk.addArcCenterAbsolute(";
  myDescr += xc + ", " + yc + ", " + x + ", " + y + ", " + flag1 + ", " + flag2 + ")";
}

/*!
  \brief Add arc with given center by specified relative coordinates
  \param dx dX value specifing arc end
  \param dy dY value specifing arc end
  \param xc X coordinate of arc center
  \param yc Y coordinate of arc center
  \param flag1 reverse direction flag
  \param flag2 tolerance
  \internal
*/
void Sketcher_Profile::DumpFunctor::addArcCenterRelative( const TCollection_AsciiString& dx,
                                                          const TCollection_AsciiString& dy,
                                                          const TCollection_AsciiString& xc,
                                                          const TCollection_AsciiString& yc,
                                                          const TCollection_AsciiString& flag1,
                                                          const TCollection_AsciiString& flag2 )
{
  myDescr += "sk.addArcCenterRelative(";
  myDescr += xc + ", " + yc + ", " + dx + ", " + dy + ", " + flag1 + ", " + flag2 + ")";
}

/*!
  \brief Add arc with given radius by specified length
  \param radius arc radius
  \param length arc length
  \internal
*/
void Sketcher_Profile::DumpFunctor::addArcRadiusLength( const TCollection_AsciiString& radius,
                                                        const TCollection_AsciiString& length )
{
  myDescr += "sk.addArcRadiusLength(";
  myDescr += radius + ", " + length + ")";
}

/*!
  \brief Add arc with given radius by specified angle and length
  \param angle angle between arc start tangent and current direction
  \param radius arc radius
  \param length arc length
  \param CurrentIndex index of current operator
  \internal
*/
void Sketcher_Profile::DumpFunctor::addArcAngleRadiusLength( const TCollection_AsciiString& angle,
                                                             const TCollection_AsciiString& radius,
                                                             const TCollection_AsciiString& length ,
                                                             int& CurrentIndex )
{
  double aAngle = angle.RealValue();
  if ( aAngle == 90 ) {
    myDescr += "sk.addArcPerpRadiusLength(";
    myDescr += radius + ", " + length + ")";
  }
  else {
    myDescr += "sk.addArcAngleRadiusLength(";
    myDescr += angle + ", " + radius + ", " + length + ")";
  }
}

/*!
  \brief Add arc with given radius by specified direction and length
  \param dx X component of direction vector
  \param dx Y component of direction vector
  \param radius arc radius
  \param length arc length
  \param CurrentIndex index of current operator
  \internal
*/
void Sketcher_Profile::DumpFunctor::addArcDirectionRadiusLength( const TCollection_AsciiString& dx,
                                                                 const TCollection_AsciiString& dy,
                                                                 const TCollection_AsciiString& radius,
                                                                 const TCollection_AsciiString& length ,
                                                                 int& CurrentIndex )
{
  myDescr += "sk.addArcDirectionRadiusLength(";
  myDescr += dx + ", " + dy + ", " + radius + ", " + length + ")";
}

/*!
  \brief Close wire
  \internal
*/
void Sketcher_Profile::DumpFunctor::closeWire()
{
  myDescr += "sk.close()";
}

/*!
  \brief Close wire and build face
  \internal
*/
void Sketcher_Profile::DumpFunctor::closeWireAndBuildFace()
{
  myDescr += "sk.close()";
  myFace = Standard_True;
}

/*!
  \brief Complete parsing of current operator
  \param CurrentIndex index of current operator
  \internal
*/
void Sketcher_Profile::DumpFunctor::nextCommand( int& CurrentIndex )
{
  CurrentIndex++;
}

/*!
  \brief Finish parsing and create result
  \internal
*/
void Sketcher_Profile::DumpFunctor::makeResult()
{
  if ( mySketcherEntry == "" ) {
    myOk = false;
    return;
  }
  myDescr += "\n\t";
  if ( myFace )
    myDescr += mySketcherEntry + " = sk.face(" + myWPEntry + ")";
  else
    myDescr += mySketcherEntry + " = sk.wire(" + myWPEntry + ")";
  myDescr += myTail;
}

/*!
  \brief Get python script
  \return string representing Python dump resulting from parsing of sketcher command
  \internal
*/
TCollection_AsciiString Sketcher_Profile::DumpFunctor::getDescription()
{
  return myDescr;
}

//=======================================================================
// Sketcher_Profile
//=======================================================================


/*!
  \brief Default constructor
*/
Sketcher_Profile::Sketcher_Profile()
{
}

/*!
  \brief Constructor
  \param command sketcher script to parse
*/
Sketcher_Profile::Sketcher_Profile( const char* command )
{
  SetCommand( command );
}

/*!
  \brief Set sketcher script to parse
  \param command sketcher script to parse
*/
void Sketcher_Profile::SetCommand( const char* command )
{
  myCommand = command;
}

/*!
  \brief Parse sketcher command and get resulting shape
  \param isDone if specified (non-zero), result of parsing is returned via this parameter
  \param error if specified (non-zero), numerical error is returned via this parameter
  \return shape resulting from parsing of sketcher command
*/
TopoDS_Shape Sketcher_Profile::GetShape( bool* isDone, double* error )
{
  ShapeFunctor functor;
  parse( myCommand, &functor );
  TopoDS_Shape s = functor.getShape();

  if ( isDone ) *isDone = functor.isOk();
  if ( error )  *error  = functor.error();

  return s;
}

/*!
  \brief Parse sketcher command and get resulting Python script
  \param isDone if specified (non-zero), result of parsing is returned via this parameter
  \return string representing Python dump resulting from parsing of sketcher command
*/
TCollection_AsciiString Sketcher_Profile::GetDump( bool* isDone )
{
  DumpFunctor functor;
  parse( myCommand, &functor );
  TCollection_AsciiString d = functor.getDescription();

  if ( isDone ) *isDone = functor.isOk();

  return d;
}

/*!
  \brief Parse sketcher script using specified functor
  \param cmd sketcher script to parse
  \internal
*/
void Sketcher_Profile::parse( const TCollection_AsciiString& cmd, Functor* functor )
{
  int CurrentIndex = 1;
  int NumberOfArg = 0;
  int NumberOfCommand = 0;

  functor->init( myCommand );
  TCollection_AsciiString command = extractCommand( myCommand );

  TCollection_AsciiString aToken = command.Token( ":", 1 );
  TColStd_Array1OfAsciiString aTab( 0, command.Length() - 1 );
  if ( command.Length() ) {
    while ( aToken.Length() != 0 ) {
      TCollection_AsciiString aNewToken = command.Token( ":", NumberOfCommand + 1 );
      if ( aNewToken.Length() > 0 )
        aTab( NumberOfCommand ) = aNewToken;
      aToken = command.Token( ":", ++NumberOfCommand );
    }
    --NumberOfCommand;
  }

  functor->setNumberOfCommand( NumberOfCommand );
  if ( aTab.Length() && aTab( 0 ).Length() ) {
    while ( CurrentIndex < NumberOfCommand ) {
      functor->initCommand();
      TColStd_Array1OfAsciiString a( 0, aTab( 0 ).Length() );
      findNextCommand( aTab, a, CurrentIndex, NumberOfArg );
      if ( a( 0 ) == "F" ) {
        if ( NumberOfArg != 3 ) badArgs();
        functor->addPoint( a.Value( 1 ), a.Value( 2 ) );
      }
      else if ( a( 0 ) == "X" ) {
        if ( NumberOfArg != 2 ) badArgs();
        functor->addSegmentParalX( a.Value( 1 ) );
      }
      else if ( a( 0 ) == "XX" ) {
        if ( NumberOfArg != 2 ) badArgs();
        functor->addSegmentParalXToZero();
      }
      else if ( a( 0 ) == "Y" ) {
        if ( NumberOfArg != 2 ) badArgs();
        functor->addSegmentParalY( a.Value( 1 ) );
      }
      else if ( a( 0 ) == "YY" ) {
        if ( NumberOfArg != 2 ) badArgs();
        functor->addSegmentParalYToZero();
      }
      else if ( a( 0 ) == "RR" ) {
        if ( NumberOfArg != 2 ) badArgs();
        functor->addAngle( a.Value( 1 ) );
      }
      else if ( a( 0 ) == "TT" ) {
        if ( NumberOfArg != 3 ) badArgs();
        functor->addSegmentAbsolute( a.Value( 1 ), a.Value( 2 ) );
      }
      else if ( a( 0 ) == "T" ) {
        if ( NumberOfArg != 3 ) badArgs();
        functor->addSegmentRelative( a.Value( 1 ), a.Value( 2 ) );
      }
      else if ( a( 0 ) == "L" ) {
        if ( NumberOfArg != 2 ) badArgs();
        functor->addSegmentLength( a.Value( 1 ) );
      }
      else if ( a( 0 ) == "IX" ) {
        if ( NumberOfArg != 2 ) badArgs();
        functor->addSegmentX( a.Value( 1 ), CurrentIndex );
      }
      else if ( a( 0 ) == "IY" ) {
        if ( NumberOfArg != 2 ) badArgs();
        functor->addSegmentY( a.Value( 1 ), CurrentIndex );
      }
      else if ( a( 0 ) == "R" ) {
        if ( NumberOfArg != 2) badArgs();
        CurrentIndex++;
        TColStd_Array1OfAsciiString aNew( 0, aTab( 0 ).Length() );
        findNextCommand( aTab, aNew, CurrentIndex, NumberOfArg );
        if ( aNew( 0 ) == "L" ) {
          if ( NumberOfArg != 2 ) badArgs();
          functor->addSegmentAngleLength( a.Value( 1 ), aNew.Value( 1 ), CurrentIndex );
        }
        else if ( aNew( 0 ) == "IX" ) {
          if ( NumberOfArg != 2 ) badArgs();
          functor->addSegmentAngleX( a.Value( 1 ), aNew.Value( 1 ), CurrentIndex );
        }
        else if ( aNew( 0 ) == "IY" ) {
          if ( NumberOfArg != 2 ) badArgs();
          functor->addSegmentAngleY( a.Value( 1 ), aNew.Value( 1 ), CurrentIndex );
        }
        else if ( aNew( 0 ) == "C" ) {
          if ( NumberOfArg != 3 ) badArgs();
          functor->addArcAngleRadiusLength( a.Value( 1 ), aNew.Value( 1 ), aNew.Value( 2 ), CurrentIndex );
        }
      }
      else if ( a( 0 ) == "D" ) {
        if ( NumberOfArg != 3 ) badArgs();
        CurrentIndex++;
        TColStd_Array1OfAsciiString aNew ( 0, aTab( 0 ).Length() );
        findNextCommand( aTab, aNew, CurrentIndex, NumberOfArg );
        if ( aNew( 0 ) == "L" ) {
          if ( NumberOfArg != 2 ) badArgs();
          functor->addSegmentDirectionLength( a.Value( 1 ), a.Value( 2 ), aNew.Value( 1 ), CurrentIndex );
        }
        else if ( aNew( 0 ) == "IX" ) {
          if ( NumberOfArg != 2 ) badArgs();
          functor->addSegmentDirectionX( a.Value( 1 ), a.Value( 2 ), aNew.Value( 1 ), CurrentIndex );
        }
        else if ( aNew( 0 ) == "IY" ) {
          if ( NumberOfArg != 2 ) badArgs();
          functor->addSegmentDirectionY( a.Value( 1 ), a.Value( 2 ), aNew.Value( 1 ), CurrentIndex );
        }
        else if ( aNew( 0 ) == "C" ) {
          if ( NumberOfArg != 3 ) badArgs();
          functor->addArcDirectionRadiusLength( a.Value( 1 ), a.Value( 2 ), aNew.Value( 1 ), aNew.Value( 2 ), CurrentIndex );
        }
      }
      else if ( a( 0 ) == "AA" ) {
        if ( NumberOfArg != 3 ) badArgs();
        functor->addArcAbsolute( a.Value( 1 ), a.Value( 2 ) );
      }
      else if ( a( 0 ) == "A" ) {
        if ( NumberOfArg != 3 ) badArgs();
        functor->addArcRelative( a.Value( 1 ), a.Value( 2 ) );
      }
      else if ( a( 0 ) == "UU" ) {
        if ( NumberOfArg != 5 ) badArgs();
        functor->addArcRadiusAbsolute( a.Value( 1 ), a.Value( 2 ), a.Value( 3 ), a.Value( 4 ) );
      }
      else if ( a( 0 ) == "U" ) {
        if ( NumberOfArg != 5 ) badArgs();
        functor->addArcRadiusRelative( a.Value( 1 ), a.Value( 2 ), a.Value( 3 ), a.Value( 4 ) );
      }
      else if ( a( 0 ) == "EE" ) {
        if ( NumberOfArg != 7 ) badArgs();
        functor->addArcCenterAbsolute( a.Value( 1 ), a.Value( 2 ), a.Value( 3 ), a.Value( 4 ), a.Value( 5 ), a.Value( 6 ) );
      }
      else if ( a( 0 ) == "E" ) {
        if ( NumberOfArg != 7 ) badArgs();
        functor->addArcCenterRelative( a.Value( 1 ), a.Value( 2 ), a.Value( 3 ), a.Value( 4 ), a.Value( 5 ), a.Value( 6 ) );
      }
      else if ( a( 0 ) == "C" ) {
        if ( NumberOfArg != 3 ) badArgs();
        functor->addArcRadiusLength( a.Value( 1 ), a.Value( 2 ) );
      }
      else if ( a( 0 ) == "WW" ) {
        functor->closeWire();
        CurrentIndex = NumberOfCommand - 1;
      }
      else if ( a( 0 ) == "WF" ) {
        functor->closeWireAndBuildFace();
        CurrentIndex = NumberOfCommand - 1;
      }
      else {
        MESSAGE("profile : unknown code " << a(CurrentIndex));
        return;
      }
      functor->nextCommand( CurrentIndex );
    }
  }

  functor->makeResult();
}

/*!
  \brief Extract sketcher command from script
  \param cmd sketcher script to parse
  \return sketcher command
  \internal
*/
TCollection_AsciiString Sketcher_Profile::extractCommand( const TCollection_AsciiString& cmd )
{
  // parse only first line of the script
  TCollection_AsciiString aSubStr = cmd.Token( "\n\t", 1 );
  return aSubStr.Token( "\"", aSubStr.Search( "=" ) != -1 ? 2 : 1 );
}

/*!
  \brief Print an error message if the number of arguments of sketcher operator is wrong
  \internal
*/
void Sketcher_Profile::badArgs()
{
  MESSAGE("profile : bad number of arguments");
}

/*!
  \brief Find the next sketcher operator in the input string
  \param aTab all sketcher command data
  \param a sketcher operator with parameters is returned via this parameter
  \param CurrentIndex current operator index
  \param NumberOfArg number of operator arguments is returned via this parameter
  \internal
*/
void Sketcher_Profile::findNextCommand( const TColStd_Array1OfAsciiString& aTab,
                                        TColStd_Array1OfAsciiString& a, int CurrentIndex,
                                        int& NumberOfArg)
{
  int n1 = 0;
  TCollection_AsciiString aToken = aTab(CurrentIndex).Token(" ", 1);
  while (aToken.Length() != 0) {
    if (aTab(CurrentIndex).Token(" ", n1 + 1).Length() > 0)
      a(n1) = aTab(CurrentIndex).Token(" ", n1 + 1);
    aToken = aTab(CurrentIndex).Token(" ", ++n1);
  }
  n1 = n1 - 1;
  NumberOfArg = n1;
}

