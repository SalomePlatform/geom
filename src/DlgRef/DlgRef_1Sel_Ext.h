/****************************************************************************
** Form interface generated from reading ui file 'DlgRef_1Sel_QTD.ui'
**
** Created: lun sep 22 17:38:05 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef DlgRef_1Sel_Ext_H
#define DlgRef_1Sel_Ext_H

#include "DlgRef_1Sel_QTD.h"

class QGridLayout;

class DlgRef_1Sel_Ext : public DlgRef_1Sel_QTD
{ 
    Q_OBJECT

public:
    DlgRef_1Sel_Ext( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 )
    	: DlgRef_1Sel_QTD( parent, name, fl ) {};
    ~DlgRef_1Sel_Ext() {};

    QGridLayout* getGroupBoxLayout() { return GroupBox1Layout; }
};

#endif // DlgRef_1Sel_Ext_H
