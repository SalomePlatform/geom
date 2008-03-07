//  SALOME GEOMToolsGUI : implementation of desktop and GUI kernel
//
//  Copyright (C) 2003  CEA/DEN, EDF R&D
//
//
//
//  File   : GEOMToolsGUI_NameDlg.h
//  Author : Vadim SANDLER
//  Module : SALOME
//  $Header$

#ifndef GEOMToolsGUI_NAMEDLG_H
#define GEOMToolsGUI_NAMEDLG_H

#include "GEOM_ToolsGUI.hxx"

#include <qdialog.h>

class QLineEdit;
class QPushButton;

//=================================================================================
// class    : GEOMToolsGUI_NameDlg
// purpose  : Common <Rename> dialog box class
//=================================================================================
class GEOMTOOLSGUI_EXPORT GEOMToolsGUI_NameDlg : public QDialog
{ 
  Q_OBJECT

public:
  GEOMToolsGUI_NameDlg( QWidget* parent = 0 );
  ~GEOMToolsGUI_NameDlg();
    
  void            setName( const QString& name );
  QString         name();
    
  static QString  getName( QWidget* parent = 0, const QString& oldName = QString::null );
    
protected slots:
  void accept();
  
private:
  QPushButton*    myButtonOk;
  QPushButton*    myButtonCancel;
  QLineEdit*      myLineEdit;
};

#endif // GEOMToolsGUI_NAMEDLG_H
