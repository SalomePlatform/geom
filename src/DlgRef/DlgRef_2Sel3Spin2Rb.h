//  File   : DlgRef_2Sel3Spin2Rb.h
//  Author : Dmitry MATVEITCHEV
//  Module : GEOM
//  $Header: 

#ifndef DLGREF_2SEL3SPIN2RB_H
#define DLGREF_2SEL3SPIN2RB_H

#include "GEOM_DlgRef.hxx"

#include "DlgRef_2Sel3Spin2Rb_QTD.h"
#include "DlgRef_SpinBox.h"

class GEOM_DLGREF_EXPORT DlgRef_2Sel3Spin2Rb : public DlgRef_2Sel3Spin2Rb_QTD
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
