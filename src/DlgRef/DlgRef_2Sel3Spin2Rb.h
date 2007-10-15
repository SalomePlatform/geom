//  File   : DlgRef_2Sel3Spin2Rb.h
//  Author : Dmitry MATVEITCHEV
//  Module : GEOM
//  $Header: 

#ifndef DLGREF_2SEL3SPIN2RB_H
#define DLGREF_2SEL3SPIN2RB_H

#include "DlgRef_2Sel3Spin2Rb_QTD.h"
#include "DlgRef_SpinBox.h"
//#if defined WNT 
//#include <SALOME_WNT.hxx>
//#else
//#define SALOME_WNT_EXPORT
//#endif
#if defined WNT && defined WIN32 && defined SALOME_WNT_EXPORTS
#define DLGREF_WNT_EXPORT __declspec( dllexport )
#else
#define DLGREF_WNT_EXPORT
#endif
class DLGREF_WNT_EXPORT DlgRef_2Sel3Spin2Rb : public DlgRef_2Sel3Spin2Rb_QTD
{ 
    Q_OBJECT

public:
    DlgRef_2Sel3Spin2Rb(QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
    ~DlgRef_2Sel3Spin2Rb();

    DlgRef_SpinBox* SpinBox_DX;
    DlgRef_SpinBox* SpinBox_DY;
    DlgRef_SpinBox* SpinBox_DZ;
};

#endif // DLGREF_2SEL3SPIN2RB_H
