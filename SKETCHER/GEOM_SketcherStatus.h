//  GEOM SKETCHER : basic sketcher
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : GEOM_SketcherStatus.h
//  Author : Nicolas REJNERI
//  Module : GEOM
//  $Header$

enum TransitionStatus {
	NOCONSTRAINT, // no constraint between consecutive edges
	TANGENT,      // arc and segment are tangent
	PERPENDICULAR,// arc is tangent to the perpendicular to the segment
	ANGLE,        // Angular constraint between 2 segments
	LENGTH_FIXED, // Length of segment has been fixed 
	X_FIXED,      // X coordinate for segment has been fixed
	Y_FIXED       // Y coordinate for segment has been fixed
	};

enum TypeOfParameter {
	ANGLE_PARAMETER,
	LENGTH_PARAMETER,
	RADIUS_PARAMETER,
	XVALUE_PARAMETER,
	YVALUE_PARAMETER
	};

enum SketchStatus {
	BEGIN_SKETCH, // Begin sketch; no edges created yet
	SEGMENT,      // Current mode for creation is segment
	ARC_CHORD,    // Current mode for creation is arc by chord
	ARC_CHORD_END,// Chord validated, waiting for radius or center
	END_SKETCH    // End sketch
};
