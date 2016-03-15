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

// File   : GEOMGUI_DimensionProperty.h
// Author : Anton POLETAEV, Open CASCADE S.A.S.
//

#ifndef GEOMGUI_DIMENSIONPROPERTY_H
#define GEOMGUI_DIMENSIONPROPERTY_H

// OCCT includes
#include <AIS_DiameterDimension.hxx>
#include <AIS_LengthDimension.hxx>
#include <AIS_AngleDimension.hxx>
#include <gp_Ax3.hxx>
#include <QVariant>
#include <QVector>
#include <QSharedPointer>

#include <string>
#include <vector>
#include <list>

class SalomeApp_Study;

/*!
 * \brief Utility class to unpack/pack dimension presentations as object property of study.
 *
 * Dimension presentations are store in relative coordinate system (LCS).
 * To ensure that dimension is bound to the equal shape irrespectively of its location
 * transformation.
 *
 * The record is a list of qvariant, containing packed dimension properties and its attributes:
 * (name);(is_visible);(dimension type);(dimension property list);
 * 
 * The following packing scheme is used to store dimension data:
 * Length: (plane)[0-3] (flyout)[4] (text flags)[5-6] (arrow flag)[7] (p1)[8-10] (pnt2)[11-13]
 * Diam:   (plane)[0-3] (flyout)[4] (text flags)[5-6] (arrow flag)[7] (circle loc, xdir, ydir, rad)[8-17]
 * Angle:               (flyout)[0] (text flags)[1-2] (arrow flag)[3] (p1)[4-6] (p2)[7-9] (center)[10-12]
 */
class Standard_EXPORT GEOMGUI_DimensionProperty
{
public:

  /*!
   * \brief Type of packed presentation.
   */
  enum DimensionType
  {
    DimensionType_Length,
    DimensionType_Diameter,
    DimensionType_Angle
  };

  /*!
   * \brief Base class for pointer-optimized records storing.
   */
  struct Length;
  struct Diameter;
  struct Angle;
  struct Record
  {
  public:
    Record( const DimensionType theType ) 
      : myType( theType )
      {}

    DimensionType Type() const
    {
      return myType;
    }

    Length*   AsLength()   { return static_cast<Length*>( this ); }
    Diameter* AsDiameter() { return static_cast<Diameter*>( this ); }
    Angle*    AsAngle()    { return static_cast<Angle*>( this ); }

    virtual void ToValues(std::vector<double>& theValues) const = 0;
    virtual void FromValues(int& theIt, const std::vector<double>& theValues) = 0;

  private:
    DimensionType myType;
  };

  /*!
   * \brief Declaration of properties for length dimensions.
   */
  struct Standard_EXPORT Length : public Record
  {
    Length() :
      Record( DimensionType_Length ),
      FirstPoint( gp::Origin() ),
      SecondPoint( gp::Origin() ),
      Plane( gp::XOY() ),
      Flyout( 0.0 ),
      TextHPos( Prs3d_DTHP_Fit ),
      TextVPos( Prs3d_DTVP_Center ),
      ArrowPos( Prs3d_DAO_Fit )
      {}

    Length( const Length& theOther ) :
      Record( DimensionType_Length ),
      FirstPoint( theOther.FirstPoint ),
      SecondPoint( theOther.SecondPoint ),
      Plane( theOther.Plane ),
      Flyout( theOther.Flyout ),
      TextHPos( theOther.TextHPos ),
      TextVPos( theOther.TextVPos ),
      ArrowPos( theOther.ArrowPos )
      {}

    ~Length() {}

    /*!
     * \brief Inits property fields from the passed length object.
     * \param theIO [in] the interactive presentation.
     * \param theLCS [in] the local coordiante system of parent object.
     */
    void Init( const Handle(AIS_LengthDimension)& theIO, const gp_Ax3& theLCS );

    /*!
     * \brief Updates length object properties from the fields.
     * \param theIO [in/out] the interactive presentation.
     * \param theLCS [in] the local coordiante system of parent object.
     */
    void Update( Handle(AIS_LengthDimension)& theIO, const gp_Ax3& theLCS );

    /*!
     * \brief Packs properties to array of doubles.
     * \param theValues [out] the values vector to populate.
     */
    void ToValues(std::vector<double>& theValues) const;

    /*!
     * \brief Unpacks properties from array of doubles.
     * \param theIt [in/out] the array index iterator.
     * \param theValues [in] the vector of values.
     */
    void FromValues(int& theIt, const std::vector<double>& theValues);

    /*!
     * \brief Overload comparsion.
     */
    bool operator == (const Length &theOther) const;
    bool operator != (const Length &theOther) const { return !(operator == (theOther)); }

    gp_Pnt FirstPoint;
    gp_Pnt SecondPoint;
    gp_Pln Plane;
    double Flyout;
    Prs3d_DimensionTextHorizontalPosition TextHPos;
    Prs3d_DimensionTextVerticalPosition   TextVPos;
    Prs3d_DimensionArrowOrientation       ArrowPos;
  };

  /*!
   * \brief Declaration of properties for diameter dimensions.
   */
  struct Standard_EXPORT Diameter : public Record
  {
    Diameter() :
      Record( DimensionType_Diameter ),
      Plane( gp::XOY() ),
      Flyout( 0.0 ),
      TextHPos( Prs3d_DTHP_Fit ),
      TextVPos( Prs3d_DTVP_Center ),
      ArrowPos( Prs3d_DAO_Fit )
      {}

     Diameter( const Diameter& theOther ) :
       Record( DimensionType_Diameter ),
       Circle( theOther.Circle ),
       Plane( theOther.Plane ),
       Flyout( theOther.Flyout ),
       TextHPos( theOther.TextHPos ),
       TextVPos( theOther.TextVPos ),
       ArrowPos( theOther.ArrowPos )
       {}

    ~Diameter() {}

    /*!
     * \brief Inits property fields from the passed length object.
     * \param theIO [in] the interactive presentation.
     * \param theLCS [in] the local coordiante system of parent object.
     */
    void Init( const Handle(AIS_DiameterDimension)& theIO, const gp_Ax3& theLCS );

    /*!
     * \brief Updates length object properties from the fields.
     * \param theIO [in/out] the interactive presentation.
     * \param theLCS [in] the local coordiante system of parent object.
     */
    void Update( Handle(AIS_DiameterDimension)& theIO, const gp_Ax3& theLCS );

    /*!
     * \brief Packs properties to array of doubles.
     * \param theValues [out] the values vector to populate.
     */
    void ToValues(std::vector<double>& theValues) const;

    /*!
     * \brief Unpacks properties from array of doubles.
     * \param theIt [in/out] the array index iterator.
     * \param theValues [in] the vector of values.
     */
    void FromValues(int& theIt, const std::vector<double>& theValues);

    /*!
     * \brief Overload comparsion.
     */
    bool operator == (const Diameter &theOther) const;
    bool operator != (const Diameter &theOther) const { return !(operator == (theOther)); }

    gp_Circ Circle;
    gp_Pln  Plane;
    double Flyout;
    Prs3d_DimensionTextHorizontalPosition TextHPos;
    Prs3d_DimensionTextVerticalPosition   TextVPos;
    Prs3d_DimensionArrowOrientation       ArrowPos;
  };

  /*!
   * \brief Declaration of properties for angle dimensions.
   */
  struct Standard_EXPORT Angle : public Record
  {
    Angle() :
      Record( DimensionType_Angle ),
      FirstPoint( gp::Origin() ),
      SecondPoint( gp::Origin() ),
      CenterPoint( gp::Origin() ),
      Flyout( 0.0 ),
      TextHPos( Prs3d_DTHP_Fit ),
      TextVPos( Prs3d_DTVP_Center ),
      ArrowPos( Prs3d_DAO_Fit )
      {}

    Angle( const Angle& theOther ) :
      Record( DimensionType_Angle ),
      FirstPoint( theOther.FirstPoint ),
      SecondPoint( theOther.SecondPoint ),
      CenterPoint( theOther.CenterPoint ),
      Flyout( theOther.Flyout ),
      TextHPos( theOther.TextHPos ),
      TextVPos( theOther.TextVPos ),
      ArrowPos( theOther.ArrowPos )
      {}

    ~Angle() {}

    /*!
     * \brief Inits property fields from the passed length object.
     * \param theIO [in] the interactive presentation.
     * \param theLCS [in] the local coordiante system of parent object.
     */
    void Init( const Handle(AIS_AngleDimension)& theIO, const gp_Ax3& theLCS );

    /*!
     * \brief Updates length object properties from the fields.
     * \param theIO [in/out] the interactive presentation.
     * \param theLCS [in] the local coordiante system of parent object.
     */
    void Update( Handle(AIS_AngleDimension)& theIO, const gp_Ax3& theLCS );

    /*!
     * \brief Packs properties to array of doubles.
     * \param theValues [out] the values vector to populate.
     */
    void ToValues(std::vector<double>& theValues) const;

    /*!
     * \brief Unpacks properties from array of doubles.
     * \param theIt [in/out] the array index iterator.
     * \param theValues [in] the vector of values.
     */
    void FromValues(int& theIt, const std::vector<double>& theValues);

    /*!
     * \brief Overload comparsion.
     */
    bool operator == (const Angle &theOther) const;
    bool operator != (const Angle &theOther) const { return !(operator == (theOther)); }

    gp_Pnt FirstPoint;
    gp_Pnt SecondPoint;
    gp_Pnt CenterPoint;
    double Flyout;
    Prs3d_DimensionTextHorizontalPosition TextHPos;
    Prs3d_DimensionTextVerticalPosition   TextVPos;
    Prs3d_DimensionArrowOrientation       ArrowPos;
  };

  typedef QSharedPointer<Record> RecordPtr;

public:

  /*!
   * \brief Constructor. Inits empty property.
   */
  GEOMGUI_DimensionProperty();

  /*!
   * \brief Copy constructor.
   */
  GEOMGUI_DimensionProperty( const GEOMGUI_DimensionProperty& theOther );

   /*!
   * \brief Constructor. Inits property from attribute.
   */
  GEOMGUI_DimensionProperty( SalomeApp_Study* theStudy, const std::string& theEntry );

   /*!
   * \brief Constructor. Inits property from formatted QString.
   */
  GEOMGUI_DimensionProperty( const QString& theProperty );

  /*!
   * \brief Destructor.
   */
  ~GEOMGUI_DimensionProperty();

  /*!
   * \brief Overload QVariant cast operator.
   */
  operator QVariant() const;

  /*!
   * \brief Overload QString cast operator.
   */
  operator QString() const;

  /*!
   * \brief Overload comparsion.
   */
  bool operator == (const GEOMGUI_DimensionProperty &theOther) const;

  /*!
   * \brief Overload comparsion.
   */
  bool operator != (const GEOMGUI_DimensionProperty &theOther) const
  {
    return !(operator == (theOther));
  }

public:

  /*!
   * \brief Returns number of dimension records.
   */
  int GetNumber() const;

  /*!
   * \brief Adds new record for the passed interactive object. Convenience method.
   * \param theIO [in] the interactive dimension to append as new record.
   * \param theLCS [in] the local coordinate system of parent object.
   */
  void AddRecord( const Handle(AIS_Dimension)& theIO, const gp_Ax3& theLCS );

  /*!
   * \brief Adds new dimension record.
   * \param theRecord [in] the record to add.
   */
  void AddRecord( const RecordPtr& theRecord );

  /*!
   * \brief Update dimension record data.
   * \param theIndex [in] the index of the record.
   * \param theIO [in] the interactive dimension to update properties.
   * \param theLCS [in] the local coordinate system of parent object.
   */
  void SetRecord( const int theIndex,
                  const Handle(AIS_Dimension)& theIO,
                  const gp_Ax3& theLCS );

  /*!
   * \brief Update dimension record data.
   * \param theIndex [in] the index of the dimension record.
   * \param theRecord [in] the record to replace with.
   */
  void SetRecord( const int theIndex, const RecordPtr& theRecord );

  /*!
   * \brief Access record by index.
   * \param theIndex [in] the index of the dimension record.
   */
  const RecordPtr& GetRecord( const int theIndex ) const;

  /*!
   * \brief Removes record by its index.
   * \param theIndex [in] the index of dimension record.
   */
  void RemoveRecord( const int theIndex );

  /*!
   * \brief Clears property data.
   */
  void Clear();

public:

  /*!
   * \brief Returns visibility state of dimension record by its index.
   *
   * \param theIndex [in] the index of the dimension record.
   */
  bool IsVisible( const int theIndex ) const;

  /*!
   * \brief Changes visibility state of the dimension record.
   *
   * \param theIndex [in] the index of the dimension record.
   * \param theIsVisible [in] the new visibility state.
   */
  void SetVisible( const int theIndex, const bool theIsVisible );

  /*!
   * \brief Returns name of dimension record by its index.
   *
   * \param theIndex [in] the index of the dimension record.
   */
  QString GetName( const int theIndex ) const;

  /*!
   * \brief Changes name of dimension record.
   *
   * \param theIndex [in] the index of the dimension record.
   * \param theName [in] the new name.
   */
  void SetName( const int theIndex, const QString& theName );

  /*!
   * \brief Returns dimension type.
   */
  int GetType( const int theIndex ) const;

public:

  /*!
   * \brief Loads properties data from attribute "AttributeTableOfReal".
   * \param theStudy [in] the study.
   * \param theEntry [in] the entry of GEOM object to operate with.
   */
  void LoadFromAttribute( SalomeApp_Study* theStudy, const std::string& theEntry );

  /*!
   * \brief Saves properties data to attribute "AttributeTableOfReal".
   * \param theStudy [in] the study.
   * \param theEntry [in] the entry of GEOM object to operate with.
   */
  void SaveToAttribute( SalomeApp_Study* theStudy, const std::string& theEntry );

private:

  /*!
   * \brief Determines dimension type code.
   */
  int TypeFromIO( const Handle(AIS_Dimension)& theIO ) const;

private:

  typedef QVector<bool>      VectorOfVisibility;
  typedef QVector<QString>   VectorOfNames;
  typedef QVector<RecordPtr> VectorOfRecords;

private:

  VectorOfVisibility         myVisibility;
  VectorOfNames              myNames;
  VectorOfRecords            myRecords;
};

Q_DECLARE_METATYPE(GEOMGUI_DimensionProperty);

#endif
