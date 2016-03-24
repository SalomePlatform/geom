// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File   : DlgRef.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#ifndef GEOM_DLGREF_H
#define GEOM_DLGREF_H

#if defined WIN32
#  if defined DLGREF_EXPORTS || defined DlgRef_EXPORTS
#    define DLGREF_EXPORT __declspec( dllexport )
#  else
#    define DLGREF_EXPORT __declspec( dllimport )
#  endif
#else
#  define DLGREF_EXPORT
#endif

//////////////////////////////////////////
// DlgRef_1Check1Spin1Check
//////////////////////////////////////////

#include "ui_DlgRef_1Check1Spin1Check_QTD.h"

class DLGREF_EXPORT DlgRef_1Check1Spin1Check : public QWidget, 
                                            public Ui::DlgRef_1Check1Spin1Check_QTD
{
  Q_OBJECT

public:
  DlgRef_1Check1Spin1Check( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_1Check1Spin1Check();
};


//////////////////////////////////////////
// DlgRef_1List1Spin1Btn
//////////////////////////////////////////

#include "ui_DlgRef_1List1Spin1Btn_QTD.h"

class DLGREF_EXPORT DlgRef_1List1Spin1Btn : public QWidget, 
                                            public Ui::DlgRef_1List1Spin1Btn_QTD
{
  Q_OBJECT

public:
  DlgRef_1List1Spin1Btn( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_1List1Spin1Btn();
};

//////////////////////////////////////////
// DlgRef_1Sel1Check1List
//////////////////////////////////////////

#include "ui_DlgRef_1Sel1Check1List_QTD.h"

class DLGREF_EXPORT DlgRef_1Sel1Check1List : public QWidget, 
                                             public Ui::DlgRef_1Sel1Check1List_QTD
{
  Q_OBJECT

public:
  DlgRef_1Sel1Check1List( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_1Sel1Check1List();
};

//////////////////////////////////////////
// DlgRef_1Sel1Check1Sel
//////////////////////////////////////////

#include "ui_DlgRef_1Sel1Check1Sel_QTD.h"

class DLGREF_EXPORT DlgRef_1Sel1Check1Sel : public QWidget, 
                                            public Ui::DlgRef_1Sel1Check1Sel_QTD
{
  Q_OBJECT

public:
  DlgRef_1Sel1Check1Sel( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_1Sel1Check1Sel();
};

//////////////////////////////////////////
// DlgRef_1Sel1Check
//////////////////////////////////////////

#include "ui_DlgRef_1Sel1Check_QTD.h"

class DLGREF_EXPORT DlgRef_1Sel1Check : public QWidget, 
                                        public Ui::DlgRef_1Sel1Check_QTD
{
  Q_OBJECT

public:
  DlgRef_1Sel1Check( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_1Sel1Check();
};

//////////////////////////////////////////
// DlgRef_1Sel1Frame
//////////////////////////////////////////

#include "ui_DlgRef_1Sel1Frame_QTD.h"

class DLGREF_EXPORT DlgRef_1Sel1Frame : public QWidget, 
                                        public Ui::DlgRef_1Sel1Frame_QTD
{
  Q_OBJECT

public:
  DlgRef_1Sel1Frame( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_1Sel1Frame();
};

//////////////////////////////////////////
// DlgRef_1Sel1List1Check3Btn
//////////////////////////////////////////

#include "ui_DlgRef_1Sel1List1Check3Btn_QTD.h"

class DLGREF_EXPORT DlgRef_1Sel1List1Check3Btn : public QWidget, 
                                                 public Ui::DlgRef_1Sel1List1Check3Btn_QTD
{
  Q_OBJECT

public:
  DlgRef_1Sel1List1Check3Btn( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_1Sel1List1Check3Btn();
};

//////////////////////////////////////////
// DlgRef_1Sel1Spin1Check
//////////////////////////////////////////

#include "ui_DlgRef_1Sel1Spin1Check_QTD.h"

class DLGREF_EXPORT DlgRef_1Sel1Spin1Check : public QWidget, 
                                             public Ui::DlgRef_1Sel1Spin1Check_QTD
{
  Q_OBJECT

public:
  DlgRef_1Sel1Spin1Check( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_1Sel1Spin1Check();
};

//////////////////////////////////////////
// DlgRef_1Sel1Spin
//////////////////////////////////////////

#include "ui_DlgRef_1Sel1Spin_QTD.h"

class DLGREF_EXPORT DlgRef_1Sel1Spin : public QWidget, 
                                       public Ui::DlgRef_1Sel1Spin_QTD
{
  Q_OBJECT

public:
  DlgRef_1Sel1Spin( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_1Sel1Spin();
};

//////////////////////////////////////////
// DlgRef_1Sel2Spin1View1Check
//////////////////////////////////////////

#include "ui_DlgRef_1Sel2Spin1View1Check_QTD.h"

class DLGREF_EXPORT DlgRef_1Sel2Spin1View1Check : public QWidget, 
                                                  public Ui::DlgRef_1Sel2Spin1View1Check_QTD
{
  Q_OBJECT

public:
  DlgRef_1Sel2Spin1View1Check( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_1Sel2Spin1View1Check();
};

//////////////////////////////////////////
// DlgRef_1Sel2Spin
//////////////////////////////////////////

#include "ui_DlgRef_1Sel2Spin_QTD.h"

class DLGREF_EXPORT DlgRef_1Sel2Spin : public QWidget, 
                                       public Ui::DlgRef_1Sel2Spin_QTD
{
  Q_OBJECT

public:
  DlgRef_1Sel2Spin( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_1Sel2Spin();
};

//////////////////////////////////////////
// DlgRef_1Sel3Check
//////////////////////////////////////////

#include "ui_DlgRef_1Sel3Check_QTD.h"

class DLGREF_EXPORT DlgRef_1Sel3Check : public QWidget, 
                                        public Ui::DlgRef_1Sel3Check_QTD
{
  Q_OBJECT

public:
  DlgRef_1Sel3Check( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_1Sel3Check();
};



//////////////////////////////////////////
// DlgRef_1Sel3Spin
//////////////////////////////////////////

#include "ui_DlgRef_1Sel3Spin_QTD.h"

class DLGREF_EXPORT DlgRef_1Sel3Spin : public QWidget, 
                                       public Ui::DlgRef_1Sel3Spin_QTD
{
  Q_OBJECT

public:
  DlgRef_1Sel3Spin( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_1Sel3Spin();
};

//////////////////////////////////////////
// DlgRef_1Sel3Spin2Check1Spin
//////////////////////////////////////////

#include "ui_DlgRef_1Sel3Spin2Check1Spin_QTD.h"

class DLGREF_EXPORT DlgRef_1Sel3Spin2Check1Spin : public QWidget, 
                                                  public Ui::DlgRef_1Sel3Spin2Check1Spin_QTD
{
  Q_OBJECT

public:
  DlgRef_1Sel3Spin2Check1Spin( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_1Sel3Spin2Check1Spin();
};

//////////////////////////////////////////
// DlgRef_1Sel3Spin1Check
//////////////////////////////////////////

#include "ui_DlgRef_1Sel3Spin1Check_QTD.h"

class DLGREF_EXPORT DlgRef_1Sel3Spin1Check : public QWidget, 
                                             public Ui::DlgRef_1Sel3Spin1Check_QTD
{
  Q_OBJECT

public:
  DlgRef_1Sel3Spin1Check( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_1Sel3Spin1Check();
};

//////////////////////////////////////////
// DlgRef_1Sel5Spin1Check
//////////////////////////////////////////

#include "ui_DlgRef_1Sel5Spin1Check_QTD.h"

class DLGREF_EXPORT DlgRef_1Sel5Spin1Check : public QWidget, 
                                             public Ui::DlgRef_1Sel5Spin1Check_QTD
{
  Q_OBJECT

public:
  DlgRef_1Sel5Spin1Check( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_1Sel5Spin1Check();
};

//////////////////////////////////////////
// DlgRef_1Spin
//////////////////////////////////////////

#include "ui_DlgRef_1Spin_QTD.h"

class DLGREF_EXPORT DlgRef_1Spin : public QWidget, 
                                   public Ui::DlgRef_1Spin_QTD
{
  Q_OBJECT

public:
  DlgRef_1Spin( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_1Spin();
};

//////////////////////////////////////////
// DlgRef_1Sel
//////////////////////////////////////////

#include "ui_DlgRef_1Sel_QTD.h"

class DLGREF_EXPORT DlgRef_1Sel : public QWidget, 
                                   public Ui::DlgRef_1Sel_QTD
{
  Q_OBJECT

public:
  DlgRef_1Sel( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_1Sel();
};

//////////////////////////////////////////
// DlgRef_1SelExt
//////////////////////////////////////////

#include "ui_DlgRef_1SelExt_QTD.h"

class DLGREF_EXPORT DlgRef_1SelExt : public QWidget, 
                                     public Ui::DlgRef_1SelExt_QTD
{
  Q_OBJECT

public:
  DlgRef_1SelExt( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_1SelExt();
};

//////////////////////////////////////////
// DlgRef_2Sel1List1Check
//////////////////////////////////////////

#include "ui_DlgRef_2Sel1List1Check_QTD.h"

class DLGREF_EXPORT DlgRef_2Sel1List1Check : public QWidget, 
                                             public Ui::DlgRef_2Sel1List1Check_QTD
{
  Q_OBJECT

public:
  DlgRef_2Sel1List1Check( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_2Sel1List1Check();
};

//////////////////////////////////////////
// DlgRef_2Sel1List2Check
//////////////////////////////////////////

#include "ui_DlgRef_2Sel1List2Check_QTD.h"

class DLGREF_EXPORT DlgRef_2Sel1List2Check : public QWidget, 
                                             public Ui::DlgRef_2Sel1List2Check_QTD
{
  Q_OBJECT

public:
  DlgRef_2Sel1List2Check( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_2Sel1List2Check();
};

//////////////////////////////////////////
// DlgRef_2Sel1List
//////////////////////////////////////////

#include "ui_DlgRef_2Sel1List_QTD.h"

class DLGREF_EXPORT DlgRef_2Sel1List : public QWidget, 
                                       public Ui::DlgRef_2Sel1List_QTD
{
  Q_OBJECT

public:
  DlgRef_2Sel1List( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_2Sel1List();
};

//////////////////////////////////////////
// DlgRef_2Sel1Spin2Check
//////////////////////////////////////////

#include "ui_DlgRef_2Sel1Spin2Check_QTD.h"

class DLGREF_EXPORT DlgRef_2Sel1Spin2Check : public QWidget, 
                                             public Ui::DlgRef_2Sel1Spin2Check_QTD
{
  Q_OBJECT

public:
  DlgRef_2Sel1Spin2Check( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_2Sel1Spin2Check();
};

//////////////////////////////////////////
// DlgRef_2Sel1Spin3Check1Spin
//////////////////////////////////////////

#include "ui_DlgRef_2Sel1Spin3Check1Spin_QTD.h"

class DLGREF_EXPORT DlgRef_2Sel1Spin3Check1Spin : public QWidget, 
                                                  public Ui::DlgRef_2Sel1Spin3Check1Spin_QTD
{
  Q_OBJECT

public:
  DlgRef_2Sel1Spin3Check1Spin( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_2Sel1Spin3Check1Spin();
};

//////////////////////////////////////////
// DlgRef_2Sel1Spin
//////////////////////////////////////////

#include "ui_DlgRef_2Sel1Spin_QTD.h"

class DLGREF_EXPORT DlgRef_2Sel1Spin : public QWidget, 
                                       public Ui::DlgRef_2Sel1Spin_QTD
{
  Q_OBJECT

public:
  DlgRef_2Sel1Spin( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_2Sel1Spin();
};

//////////////////////////////////////////
// DlgRef_2Sel1SpinInt
//////////////////////////////////////////

#include "ui_DlgRef_2Sel1SpinInt_QTD.h"

class DLGREF_EXPORT DlgRef_2Sel1SpinInt : public QWidget, 
                                          public Ui::DlgRef_2Sel1SpinInt_QTD
{
  Q_OBJECT

public:
  DlgRef_2Sel1SpinInt( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_2Sel1SpinInt();
};

//////////////////////////////////////////
// DlgRef_2Sel2List
//////////////////////////////////////////

#include "ui_DlgRef_2Sel2List_QTD.h"

class DLGREF_EXPORT DlgRef_2Sel2List : public QWidget, 
                                       public Ui::DlgRef_2Sel2List_QTD
{
  Q_OBJECT

public:
  DlgRef_2Sel2List( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_2Sel2List();
};

//////////////////////////////////////////
// DlgRef_2Sel2Spin1Check
//////////////////////////////////////////

#include "ui_DlgRef_2Sel2Spin1Check_QTD.h"

class DLGREF_EXPORT DlgRef_2Sel2Spin1Check : public QWidget, 
                                             public Ui::DlgRef_2Sel2Spin1Check_QTD
{
  Q_OBJECT

public:
  DlgRef_2Sel2Spin1Check( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_2Sel2Spin1Check();
};

//////////////////////////////////////////
// DlgRef_2Sel2Spin3Check
//////////////////////////////////////////

#include "ui_DlgRef_2Sel2Spin3Check_QTD.h"

class DLGREF_EXPORT DlgRef_2Sel2Spin3Check : public QWidget, 
                                             public Ui::DlgRef_2Sel2Spin3Check_QTD
{
  Q_OBJECT

public:
  DlgRef_2Sel2Spin3Check( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_2Sel2Spin3Check();
};

//////////////////////////////////////////
// DlgRef_2Sel2Spin
//////////////////////////////////////////

#include "ui_DlgRef_2Sel2Spin_QTD.h"

class DLGREF_EXPORT DlgRef_2Sel2Spin : public QWidget, 
                                       public Ui::DlgRef_2Sel2Spin_QTD
{
  Q_OBJECT

public:
  DlgRef_2Sel2Spin( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_2Sel2Spin();
};

//////////////////////////////////////////
// DlgRef_2Sel2Spin1Push
//////////////////////////////////////////

#include "ui_DlgRef_2Sel2Spin1Push_QTD.h"

class DLGREF_EXPORT DlgRef_2Sel2Spin1Push : public QWidget, 
                                       public Ui::DlgRef_2Sel2Spin1Push_QTD
{
  Q_OBJECT

public:
  DlgRef_2Sel2Spin1Push( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_2Sel2Spin1Push();
};

//////////////////////////////////////////
// DlgRef_2Sel2Spin2Push
//////////////////////////////////////////

#include "ui_DlgRef_2Sel2Spin2Push_QTD.h"

class DLGREF_EXPORT DlgRef_2Sel2Spin2Push : public QWidget, 
                                       public Ui::DlgRef_2Sel2Spin2Push_QTD
{
  Q_OBJECT

public:
  DlgRef_2Sel2Spin2Push( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_2Sel2Spin2Push();
};

//////////////////////////////////////////
// DlgRef_2Sel3Spin1Check
//////////////////////////////////////////

#include "ui_DlgRef_2Sel3Spin1Check_QTD.h"

class DLGREF_EXPORT DlgRef_2Sel3Spin1Check : public QWidget, 
					public Ui::DlgRef_2Sel3Spin1Check_QTD
{
  Q_OBJECT

public:
  DlgRef_2Sel3Spin1Check( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_2Sel3Spin1Check();
};

//////////////////////////////////////////
// DlgRef_2Sel3Spin2Rb
//////////////////////////////////////////

#include "ui_DlgRef_2Sel3Spin2Rb_QTD.h"

class DLGREF_EXPORT DlgRef_2Sel3Spin2Rb : public QWidget, 
                                          public Ui::DlgRef_2Sel3Spin2Rb_QTD
{
  Q_OBJECT

public:
  DlgRef_2Sel3Spin2Rb( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_2Sel3Spin2Rb();
};

//////////////////////////////////////////
// DlgRef_2Sel3Spin
//////////////////////////////////////////

#include "ui_DlgRef_2Sel3Spin_QTD.h"

class DLGREF_EXPORT DlgRef_2Sel3Spin : public QWidget, 
                                       public Ui::DlgRef_2Sel3Spin_QTD
{
  Q_OBJECT

public:
  DlgRef_2Sel3Spin( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_2Sel3Spin();
};

//////////////////////////////////////////
// DlgRef_2Sel
//////////////////////////////////////////

#include "ui_DlgRef_2Sel_QTD.h"

class DLGREF_EXPORT DlgRef_2Sel : public QWidget, 
                                  public Ui::DlgRef_2Sel_QTD
{
  Q_OBJECT

public:
  DlgRef_2Sel( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_2Sel();
};

//////////////////////////////////////////
// DlgRef_2SelExt
//////////////////////////////////////////

#include "ui_DlgRef_2SelExt_QTD.h"

class DLGREF_EXPORT DlgRef_2SelExt : public QWidget, 
                                     public Ui::DlgRef_2SelExt_QTD
{
  Q_OBJECT

public:
  DlgRef_2SelExt( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_2SelExt();
};

//////////////////////////////////////////
// DlgRef_2Spin
//////////////////////////////////////////

#include "ui_DlgRef_2Spin_QTD.h"

class DLGREF_EXPORT DlgRef_2Spin : public QWidget, 
                                   public Ui::DlgRef_2Spin_QTD
{
  Q_OBJECT

public:
  DlgRef_2Spin( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_2Spin();
};

//////////////////////////////////////////
// DlgRef_3Check
//////////////////////////////////////////

#include "ui_DlgRef_3Check_QTD.h"

class DLGREF_EXPORT DlgRef_3Check : public QWidget, 
                                    public Ui::DlgRef_3Check_QTD
{
  Q_OBJECT

public:
  DlgRef_3Check( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_3Check();
};

//////////////////////////////////////////
// DlgRef_1Sel3Radio
//////////////////////////////////////////

#include "ui_DlgRef_3Radio_QTD.h"

class DLGREF_EXPORT DlgRef_3Radio : public QWidget, 
                                    public Ui::DlgRef_3Radio_QTD
{
  Q_OBJECT

public:
  DlgRef_3Radio( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_3Radio();
};

//////////////////////////////////////////
// DlgRef_1Sel3Radio1Sel1Spin
//////////////////////////////////////////

#include "ui_DlgRef_3Radio1Sel1Spin_QTD.h"

class DLGREF_EXPORT DlgRef_3Radio1Sel1Spin : public QWidget, 
                                             public Ui::DlgRef_3Radio1Sel1Spin_QTD
{
  Q_OBJECT

public:
  DlgRef_3Radio1Sel1Spin( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_3Radio1Sel1Spin();
};

//////////////////////////////////////////
// DlgRef_3Sel1Check
//////////////////////////////////////////

#include "ui_DlgRef_3Sel1Check_QTD.h"

class DLGREF_EXPORT DlgRef_3Sel1Check : public QWidget, 
                                        public Ui::DlgRef_3Sel1Check_QTD
{
  Q_OBJECT

public:
  DlgRef_3Sel1Check( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_3Sel1Check();
};

//////////////////////////////////////////
// DlgRef_3Sel1Spin
//////////////////////////////////////////

#include "ui_DlgRef_3Sel1Spin_QTD.h"

class DLGREF_EXPORT DlgRef_3Sel1Spin : public QWidget, 
                                       public Ui::DlgRef_3Sel1Spin_QTD
{
  Q_OBJECT

public:
  DlgRef_3Sel1Spin( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_3Sel1Spin();
};

//////////////////////////////////////////
// DlgRef_3Sel2Spin
//////////////////////////////////////////

#include "ui_DlgRef_3Sel2Spin_QTD.h"

class DLGREF_EXPORT DlgRef_3Sel2Spin : public QWidget, 
                                       public Ui::DlgRef_3Sel2Spin_QTD
{
  Q_OBJECT

public:
  DlgRef_3Sel2Spin( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_3Sel2Spin();
};

//////////////////////////////////////////
// DlgRef_3Sel3Spin1Check
//////////////////////////////////////////

#include "ui_DlgRef_3Sel3Spin1Check_QTD.h"

class DLGREF_EXPORT DlgRef_3Sel3Spin1Check : public QWidget, 
                                             public Ui::DlgRef_3Sel3Spin1Check_QTD
{
  Q_OBJECT

public:
  DlgRef_3Sel3Spin1Check( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_3Sel3Spin1Check();

  void ShowRows( int, int, bool = true );
};

//////////////////////////////////////////
// DlgRef_3Sel2Check3Spin
//////////////////////////////////////////

#include "ui_DlgRef_3Sel2Check3Spin_QTD.h"

class DLGREF_EXPORT DlgRef_3Sel2Check3Spin : public QWidget, 
                                             public Ui::DlgRef_3Sel2Check3Spin_QTD
{
  Q_OBJECT

public:
  DlgRef_3Sel2Check3Spin( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_3Sel2Check3Spin();

  void ShowRows( int, int, bool = true );
};

//////////////////////////////////////////
// DlgRef_3Sel3Spin2Check
//////////////////////////////////////////

#include "ui_DlgRef_3Sel3Spin2Check_QTD.h"

class DLGREF_EXPORT DlgRef_3Sel3Spin2Check : public QWidget, 
                                             public Ui::DlgRef_3Sel3Spin2Check_QTD
{
  Q_OBJECT

public:
  DlgRef_3Sel3Spin2Check( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_3Sel3Spin2Check();

  void ShowRows( int, int, bool = true );
};

//////////////////////////////////////////
// DlgRef_3Sel4Spin2Check
//////////////////////////////////////////

#include "ui_DlgRef_3Sel4Spin2Check_QTD.h"

class DLGREF_EXPORT DlgRef_3Sel4Spin2Check : public QWidget, 
                                             public Ui::DlgRef_3Sel4Spin2Check_QTD
{
  Q_OBJECT

public:
  DlgRef_3Sel4Spin2Check( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_3Sel4Spin2Check();
};

//////////////////////////////////////////
// DlgRef_3Sel
//////////////////////////////////////////

#include "ui_DlgRef_3Sel_QTD.h"

class DLGREF_EXPORT DlgRef_3Sel : public QWidget, 
                                  public Ui::DlgRef_3Sel_QTD
{
  Q_OBJECT

public:
  DlgRef_3Sel( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_3Sel();
};

//////////////////////////////////////////
// DlgRef_3Spin1Check
//////////////////////////////////////////

#include "ui_DlgRef_3Spin1Check_QTD.h"

class DLGREF_EXPORT DlgRef_3Spin1Check : public QWidget, 
                                         public Ui::DlgRef_3Spin1Check_QTD
{
  Q_OBJECT

public:
  DlgRef_3Spin1Check( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_3Spin1Check();
};

//////////////////////////////////////////
// DlgRef_3Spin1CheckCyl
//////////////////////////////////////////

#include "ui_DlgRef_3Spin1CheckCyl_QTD.h"

class DLGREF_EXPORT DlgRef_3Spin1CheckCyl : public QWidget, 
                                         public Ui::DlgRef_3Spin1CheckCyl_QTD
{
  Q_OBJECT

public:
  DlgRef_3Spin1CheckCyl( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_3Spin1CheckCyl();
};

//////////////////////////////////////////
// DlgRef_3Spin
//////////////////////////////////////////

#include "ui_DlgRef_3Spin_QTD.h"

class DLGREF_EXPORT DlgRef_3Spin : public QWidget, 
                                   public Ui::DlgRef_3Spin_QTD
{
  Q_OBJECT

public:
  DlgRef_3Spin( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_3Spin();
};

//////////////////////////////////////////
// DlgRef_4Spin
//////////////////////////////////////////

#include "ui_DlgRef_4Spin_QTD.h"

class DLGREF_EXPORT DlgRef_4Spin : public QWidget, 
                                   public Ui::DlgRef_4Spin_QTD
{
  Q_OBJECT

public:
  DlgRef_4Spin( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_4Spin();
};

//////////////////////////////////////////
// DlgRef_4Sel1List1Check
//////////////////////////////////////////

#include "ui_DlgRef_4Sel1List1Check_QTD.h"

class DLGREF_EXPORT DlgRef_4Sel1List1Check : public QWidget, 
                                             public Ui::DlgRef_4Sel1List1Check_QTD
{
  Q_OBJECT

public:
  DlgRef_4Sel1List1Check( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_4Sel1List1Check();
};

//////////////////////////////////////////
// DlgRef_4Sel1List
//////////////////////////////////////////

#include "ui_DlgRef_4Sel1List_QTD.h"

class DLGREF_EXPORT DlgRef_4Sel1List : public QWidget, 
                                       public Ui::DlgRef_4Sel1List_QTD
{
  Q_OBJECT

public:
  DlgRef_4Sel1List( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_4Sel1List();
};

//////////////////////////////////////////
// DlgRef_4Sel1Spin2Check
//////////////////////////////////////////

#include "ui_DlgRef_4Sel1Spin2Check_QTD.h"

class DLGREF_EXPORT DlgRef_4Sel1Spin2Check : public QWidget, 
                                             public Ui::DlgRef_4Sel1Spin2Check_QTD
{
  Q_OBJECT

public:
  DlgRef_4Sel1Spin2Check( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_4Sel1Spin2Check();

  void ShowRows( int, int, bool = true );
};

//////////////////////////////////////////
// DlgRef_4Sel1Spin2Check
//////////////////////////////////////////

#include "ui_DlgRef_4Sel1Spin3Check_QTD.h"

class DLGREF_EXPORT DlgRef_4Sel1Spin3Check : public QWidget, 
                                             public Ui::DlgRef_4Sel1Spin3Check_QTD
{
  Q_OBJECT

public:
  DlgRef_4Sel1Spin3Check( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_4Sel1Spin3Check();
};

//////////////////////////////////////////
// DlgRef_6Sel
//////////////////////////////////////////

#include "ui_DlgRef_6Sel_QTD.h"

class DLGREF_EXPORT DlgRef_6Sel : public QWidget, 
                                  public Ui::DlgRef_6Sel_QTD
{
  Q_OBJECT

public:
  DlgRef_6Sel( QWidget* = 0, Qt::WindowFlags = 0 );
  ~DlgRef_6Sel();
};

//////////////////////////////////////////
// Utility functions
//////////////////////////////////////////

namespace DlgRef
{
  DLGREF_EXPORT QString PrintDoubleValue( double, int = 16 );
};

#endif // GEOM_DLGREF_H
