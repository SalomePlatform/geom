//  File      : GEOM_SketcherStatus.h
//  Created   : Wed Jul 5 10:12:09 2000
//  Author    : Martine LANGLOIS

//  Modified  : Tue Dec 11 21:29:57 2001
//  Author    : Nicolas REJNERI
//  Project   : SALOME
//  Module    : SALOMEGUI
//  Copyright : Open CASCADE
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
