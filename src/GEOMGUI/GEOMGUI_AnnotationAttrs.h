// Copyright (C) 2007-2020  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File   : GEOMGUI_AnnotationAttrs.h
// Author : Anton POLETAEV, Open CASCADE S.A.S.
//

#ifndef GEOMGUI_ANNOTATIONATTRS_H
#define GEOMGUI_ANNOTATIONATTRS_H

// SALOME GUI includes
#include <GEOMGUI.h>
#include <SalomeApp_Study.h>

// OCCT includes
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Transient.hxx>

// Qt, STL includes
#include <QString>

class GEOM_Annotation;
class gp_Ax3;

/*!
 * \brief Attribute-style helper tool to manage shape annotation data of an object.
 */
class GEOMGUI_AnnotationAttrs : public Standard_Transient
{
public:

  DEFINE_STANDARD_RTTIEXT( GEOMGUI_AnnotationAttrs, Standard_Transient )

  //! Find annotation data defined for an object.
  GEOMGUI_EXPORT static Handle(GEOMGUI_AnnotationAttrs) FindAttributes( const _PTR(SObject)& theObject );

  //! Find or create annotation data fields for an object.
  GEOMGUI_EXPORT static Handle(GEOMGUI_AnnotationAttrs) FindOrCreateAttributes( const _PTR(SObject)& theObject,
                                                                                SalomeApp_Study* theStudy );

  //! Remove annotation data fields for an object.
  GEOMGUI_EXPORT static void Remove( const _PTR(SObject)& theObject );

// Import / Export
public:

  //! Exports annotation records as a property string.
  GEOMGUI_EXPORT QString ExportAsPropertyString() const;

  //! Imports annotation records from a property string.
  GEOMGUI_EXPORT void ImportFromPropertyString( const QString& theString );

public:

  /*!
   * \ brief Structure representing visual properties of the shape annotation.
   */
  struct Properties
  {
    QString Text;          //!< Displayed annotation text.
    bool    IsVisible;     //!< Application visibility flag of annotation.
    bool    IsScreenFixed; //!< Fixed screen mode flag.
    gp_Pnt  Position;      //!< Position of the annotation.
    gp_Pnt  Attach;        //!< Attachment point of the annotation.
    int     ShapeIndex;    //!< Index of the annotated subshape.
    int     ShapeType;     //!< Type of the annotated subshape.
  };

  //! Setup parameters of the annotation presentation with the properties given.
  //! @param thePresentation [in] the presentation to setup.
  //! @param theProps [in] the set of properties.
  //! @param theShapeLCS [in] the local coordinate system of the shape.
  GEOMGUI_EXPORT static void SetupPresentation( const Handle(GEOM_Annotation)& thePresentation,
                                                const Properties& theProps,
                                                const gp_Ax3& theShapeLCS );

  //! Setup parameters of the annotation presentation with the properties of a definition.
  //! @param thePresentation [in] the presentation to setup.
  //! @param theIndex [in] the index of the annotation definition.
  //! @param theShapeLCS [in] the local coordinate system of the shape.
  GEOMGUI_EXPORT void SetupPresentation( const Handle(GEOM_Annotation)& thePresentation,
                                         const int theIndex,
                                         const gp_Ax3& theShapeLCS );

public:

  //! Changes count of annotation definitions stored on the object.
  //! If the count is decreased the extra annotation definitions are
  //! cleared out from the attribute.
  //! @param theNumber [in] the new number of annotation definitions.
  GEOMGUI_EXPORT void SetNbAnnotation( const int theCount ) const;

  //! Returns number of annotation definitions stored on the object.
  GEOMGUI_EXPORT int GetNbAnnotation() const;

  //! Sets application visibility state of an annotation definition.
  //! @param theIndex [in] the index of the annotation definition.
  //! @param theIsVisible [in] the visibility state.
  GEOMGUI_EXPORT void SetIsVisible( const int theIndex, const bool theIsVisible );

  //! Returns application visibility state of an annotation definition.
  GEOMGUI_EXPORT bool GetIsVisible( const int theIndex ) const;

  //! Sets annotation label's text.
  //! @param theIndex [in] the index of the annotation definition.
  //! @param theText [in] the text string.
  GEOMGUI_EXPORT void SetText( const int theIndex, const QString& theText );

  //! Returns annotation label's text.
  GEOMGUI_EXPORT QString GetText( const int theIndex ) const;

  //! Sets screen fixed flag of the annotation definition.
  //! @param theIndex [in] the index of the annotation definition.
  //! @param theIsScreenFixed [in] the presentation flag.
  GEOMGUI_EXPORT void SetIsScreenFixed( const int theIndex, const bool theIsScreenFixed );

  //! Returns screen fixed flag of the annotation definition.
  GEOMGUI_EXPORT bool GetIsScreenFixed( const int theIndex ) const;

  //! Sets position of the annotation definition.
  //! @param theIndex [in] the index of the annotation definition.
  //! @param thePosition [in] the position of the annotation label.
  GEOMGUI_EXPORT void SetPosition( const int theIndex, const gp_Pnt& thePosition );

  //! Returns position of the annotation definition.
  GEOMGUI_EXPORT gp_Pnt GetPosition( const int theIndex ) const;

  //! Sets attach point of the annotation definition.
  //! @param theIndex [in] the index of the annotation definition.
  //! @param theAttach [in] the attach point of the annotation.
  GEOMGUI_EXPORT void SetAttach( const int theIndex, const gp_Pnt& theAttach );

  //! Returns attach point of the annotation definition.
  GEOMGUI_EXPORT gp_Pnt GetAttach( const int theIndex ) const;

  //! Sets shape selection arguments.
  //! @param theIndex [in] the index of the annotation definition.
  //! @param theShapeType, theSubIdx [in] the type of the selected shape and the sub-shape index.
  GEOMGUI_EXPORT void SetShapeSel( const int theIndex, const int theShapeType, const int theSubIdx );

  //! Returns shape selection arguments.
  //! @param theIndex [in] the index of the annotation definition.
  //! @param theShapeType, theSubIdx [out] the type of the selected shape and the sub-shape index.
  GEOMGUI_EXPORT void GetShapeSel( const int theIndex, int& theShapeType, int& theSubIdx ) const;

public:

  //! Appends new annotation definition with the given properties.
  GEOMGUI_EXPORT void Append( const Properties& theProps );

  //! Appends new annotation definition with the given properties and converts
  //! position and attachment points to the given shape's local frame of reference.
  //! @param theShapeLCS [in] the position of shape's local frame of reference with respect
  //!        to the world frame of reference.
  GEOMGUI_EXPORT void Append( const Properties& theProps, const gp_Ax3& theShapeLCS );

  //! Removes annotation definition by the given index.
  //! This methods performs iterative reindexing of elements starting
  //! from the given index, so sequential removal should be performed
  //! from the last element, or avoided at all.
  GEOMGUI_EXPORT void Remove( const Standard_Integer theIndex );

  //! Sets complete properties of an annotation definition.
  //! @param theIndex [in] the index of the annotation definition.
  //! @param theProps [in] the structure containing the properties.
  GEOMGUI_EXPORT void SetProperties( const int theIndex, const Properties& theProps );

  //! Sets complete properties of an annotation definition with converting
  //! position and attachment points to the given shape's local frame
  //! of reference.
  //! @param theIndex [in] the index of the annotation definition.
  //! @param theProps [in] the structure containing the properties.
  //! @param theShapeLCS [in] the position of shape's local frame of reference with respect
  //!        to the world frame of reference.
  GEOMGUI_EXPORT void SetProperties( const int theIndex, const Properties& theProps, const gp_Ax3& theShapeLCS );

  //! Returns complete properties of an annotation definition.
  //! @param theIndex [in] the index of the annotation definition.
  //! @param theProps [out] the structure containing the properties.
  GEOMGUI_EXPORT void GetProperties( const int theIndex, Properties& theProps ) const;

private:

  GEOMGUI_AnnotationAttrs( const _PTR(SObject)& theObject, const _PTR(AttributeParameter)& theParameter )
  : myObj( theObject ),
    myParameterMap( theParameter ) {}

  GEOMGUI_AnnotationAttrs( const GEOMGUI_AnnotationAttrs& ); // private copy constructor, must be never called
  void operator=( const GEOMGUI_AnnotationAttrs& ); // private assignment operator, must be never called

private:

  _PTR(SObject) myObj;
  _PTR(AttributeParameter) myParameterMap;
};

DEFINE_STANDARD_HANDLE( GEOMGUI_AnnotationAttrs, Standard_Transient )

#endif
