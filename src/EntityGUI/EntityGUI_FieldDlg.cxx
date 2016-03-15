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

//  File   : EntityGUI_FieldDlg.cxx

#include "EntityGUI_FieldDlg.h"

#include <DlgRef.h>
#include <GEOMBase.h>
#include <GEOMBase_Skeleton.h>
#include <GEOM_Displayer.h>
#include <GeometryGUI.h>

#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_Tools.h>
#include <SalomeApp_IntSpinBox.h>
#include <SalomeApp_DoubleSpinBox.h>

#include <LightApp_SelectionMgr.h>

#include <OCCViewer_ViewModel.h>
#include <OCCViewer_ViewManager.h>
#include <SVTK_ViewModel.h>
#include <SALOME_Prs.h>
#include <SALOME_ListIO.hxx>

#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>
#include <SUIT_OverrideCursor.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>

#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_DataMapIteratorOfDataMapOfIntegerInteger.hxx>

#include <limits>

#include <SALOMEDS_wrap.hxx>
#include <GEOMImpl_Types.hxx>

#ifdef max
  #undef max
#endif


const int theIntLimit = std::numeric_limits<int>::max() - 10;

/*
  Class       : EntityGUI_FieldDlg::CheckItem
  Description : Bool item.
*/
class EntityGUI_FieldDlg::CheckItem : public QTableWidgetItem
{
public:
  static int     Type();

  CheckItem( bool = false );
  ~CheckItem();

  void  setValue( bool );
  bool  value() const;
};

int EntityGUI_FieldDlg::CheckItem::Type()
{
  return QTableWidgetItem::UserType + 2;
}

EntityGUI_FieldDlg::CheckItem::CheckItem( bool value )
 : QTableWidgetItem( Type() )
{
  Qt::ItemFlags f = flags();
  f = f | Qt::ItemIsUserCheckable;
  f = f & ~Qt::ItemIsTristate;
  f = f & ~Qt::ItemIsEditable;
  setFlags( f );
  setValue(value);
}

EntityGUI_FieldDlg::CheckItem::~CheckItem()
{
}

void EntityGUI_FieldDlg::CheckItem::setValue( bool value )
{
  setCheckState( value ? Qt::Checked : Qt::Unchecked );
}

bool EntityGUI_FieldDlg::CheckItem::value() const
{
  return checkState() == Qt::Checked;
}

/*
  Class       : EntityGUI_FieldDlg::IntSpinItem
  Description : Integer spin table item.
*/

class EntityGUI_FieldDlg::IntSpinItem : public QTableWidgetItem
{
public:
  static int     Type();

  IntSpinItem( const int theValue );

  int            value() const;
  void           setValue( const int theValue );

  void           clear();
};

int EntityGUI_FieldDlg::IntSpinItem::Type()
{
  return QTableWidgetItem::UserType + 3;
}

EntityGUI_FieldDlg::IntSpinItem::IntSpinItem( const int theValue )
 : QTableWidgetItem( Type() )
{
  setValue( theValue );
}

int EntityGUI_FieldDlg::IntSpinItem::value() const
{
  bool ok = false;
  int value = data( Qt::UserRole ).toInt( &ok );
  return ok ? value : 0; 
}

void EntityGUI_FieldDlg::IntSpinItem::setValue( const int theValue )
{
  setData( Qt::UserRole, theValue );
  setText( QString::number( theValue ) ); 
}

void EntityGUI_FieldDlg::IntSpinItem::clear()
{
  setText( "" );
}

/*
  Class       : EntityGUI_FieldDlg::DoubleSpinItem
  Description : Double spin table item.
*/

class EntityGUI_FieldDlg::DoubleSpinItem : public QTableWidgetItem
{
public:
  static int     Type();

  DoubleSpinItem( const double theValue, const int thePrecision=6 );

  double         value() const;
  void           setValue( const double theValue );

  int            precision() const;
  void           setPrecision( const int thePrecision );

  void           clear();
};

int EntityGUI_FieldDlg::DoubleSpinItem::Type()
{
  return QTableWidgetItem::UserType + 4;
}

EntityGUI_FieldDlg::DoubleSpinItem::DoubleSpinItem( const double theValue,
                                                    const int    thePrecision)
 : QTableWidgetItem( Type() )
{
  setValue( theValue );
  setPrecision( thePrecision );
}

double EntityGUI_FieldDlg::DoubleSpinItem::value() const
{
  bool ok = false;
  double value = data( Qt::UserRole ).toDouble( &ok );
  return ok ? value : 0; 
}

void EntityGUI_FieldDlg::DoubleSpinItem::setValue( const double theValue )
{
  setData( Qt::UserRole, theValue );
  setText( QString::number( theValue ) ); 
}

int EntityGUI_FieldDlg::DoubleSpinItem::precision() const
{
  bool ok = false;
  int precision = data( Qt::UserRole + 1 ).toInt( &ok );
  return ok ? precision : 0; 
}

void EntityGUI_FieldDlg::DoubleSpinItem::setPrecision( const int thePrecision )
{
  setData( Qt::UserRole + 1, thePrecision );
}

void EntityGUI_FieldDlg::DoubleSpinItem::clear()
{
  setText( "" );
}

/*
  Class       : EntityGUI_FieldDlg::ComboDelegate
  Description : Table used by this widget
*/

class EntityGUI_FieldDlg::Delegate : public QItemDelegate
{
public:
  Delegate( QObject* = 0 );
  ~Delegate();
  
  QWidget*      createEditor( QWidget*, const QStyleOptionViewItem&,
                              const QModelIndex& ) const;
  
  void          setEditorData( QWidget*, const QModelIndex& ) const;
  void          setModelData( QWidget*, QAbstractItemModel*, const QModelIndex& ) const;
  
  void          updateEditorGeometry( QWidget*, const QStyleOptionViewItem&, 
                                      const QModelIndex& ) const;
private:
  StepTable* myTable;
};

EntityGUI_FieldDlg::Delegate::Delegate( QObject* parent )
  : QItemDelegate( parent ), 
    myTable( dynamic_cast<EntityGUI_FieldDlg::StepTable*>( parent ) )
{
}
  
EntityGUI_FieldDlg::Delegate::~Delegate()
{
}

QWidget* EntityGUI_FieldDlg::Delegate::createEditor( QWidget* parent,
                                                     const QStyleOptionViewItem& option,
                                                     const QModelIndex& index ) const
{
  QVariant aData = index.data( Qt::UserRole );
  QVariant::Type aDataType = aData.type();
  if( aDataType == QVariant::Int ) {
    bool ok = false;
    int aValue = aData.toInt( &ok );
    if ( ok ) {
      SalomeApp_IntSpinBox* intSpin = new SalomeApp_IntSpinBox( -theIntLimit, theIntLimit, 1, parent, false, true );
      intSpin->setFrame( false );
      intSpin->setValue( aValue );
      return intSpin;
    }
  }
  else if( aDataType == QVariant::Double ) {
    bool ok = false;
    double aValue = aData.toDouble( &ok );
    if ( ok ) {
      int aPrecision = index.data( Qt::UserRole + 1 ).toInt( &ok );
      if ( !ok )
        aPrecision = 0;

      SalomeApp_DoubleSpinBox* dblSpin = new SalomeApp_DoubleSpinBox( -1.e20, 1.e20, 1, aPrecision, 20, parent, false, true );
      dblSpin->setFrame( false );
      dblSpin->setValue( aValue );
      return dblSpin;
    }
  }
  return QItemDelegate::createEditor( parent, option, index );
}

void EntityGUI_FieldDlg::Delegate::setEditorData( QWidget* editor, 
                                                  const QModelIndex& index ) const
{
  QVariant data = index.model()->data( index, Qt::DisplayRole );
  QString value = data.toString();
  bool bOk = false;
  if ( SalomeApp_DoubleSpinBox* dblSpin = dynamic_cast<SalomeApp_DoubleSpinBox*>( editor ) ) {
    if( data.type() == QVariant::Double ) {
      double valueDouble = data.toDouble( &bOk );
      if( bOk )
        dblSpin->setValue( valueDouble );
    }
  }
  if ( !bOk ) QItemDelegate::setEditorData( editor, index );
}

void EntityGUI_FieldDlg::Delegate::setModelData( QWidget* editor,
                                                 QAbstractItemModel* model,
                                                 const QModelIndex& index) const
{
  QString oldData = myTable->text( index.row(), index.column() );

  if( SalomeApp_IntSpinBox* intSpin = dynamic_cast<SalomeApp_IntSpinBox*>( editor ) )
    model->setData( index, intSpin->value(), Qt::DisplayRole );
  else if( SalomeApp_DoubleSpinBox* dblSpin = dynamic_cast<SalomeApp_DoubleSpinBox*>( editor ) )
    model->setData( index, dblSpin->value(), Qt::DisplayRole );
  else
    QItemDelegate::setModelData( editor, model, index );

  QString newData = myTable->text( index.row(), index.column() );
  if ( newData != oldData )
    myTable->setIsChanged();
}

void EntityGUI_FieldDlg::Delegate::updateEditorGeometry( QWidget* editor,
                                                         const QStyleOptionViewItem& option, 
                                                         const QModelIndex& index ) const
{
  editor->setGeometry( option.rect );
}

//=======================================================================
// name    : EntityGUI_FieldDlg::StepTable::Table
// Purpose : Constructor
//=======================================================================
EntityGUI_FieldDlg::StepTable::StepTable (int stepID, int dataType,
                                          int nbRows, int nbComps,
                                          QString shapeName, QStringList headers,
                                          GEOM::GEOM_FieldStep_ptr stepVar,
                                          QWidget* parent)
  : QTableWidget(0, nbComps+1, parent),
    myDataType( dataType ),
    myStepID( stepID ),
    myStamp( 0 ),
    myStep( GEOM::GEOM_FieldStep::_duplicate( stepVar )),
    myIsChanged( false )
{
  setDim( nbRows, shapeName, stepVar->_is_nil() );

  setHorizontalHeaderLabels( headers );
  verticalHeader()->hide();

  // set custom item delegate
  setItemDelegate( new Delegate(this) );
  // set edit triggers by default
  setReadOnly( false );

  if ( stepVar->_is_nil() )
  {
    connect( horizontalHeader(), SIGNAL( sectionDoubleClicked( int ) ), this, SLOT( headerDblClicked( int ) ) );

    return;
  }
  myStamp = stepVar->GetStamp();

  const int nbColumns = nbComps + 1;

  switch ( dataType )
  {
  case 0:
  {
    GEOM::GEOM_BoolFieldStep_var bs = GEOM::GEOM_BoolFieldStep::_narrow( stepVar );
    if ( !bs->_is_nil() )
    {
      GEOM::short_array_var vals = bs->GetValues();
      if ( vals->length() == nbRows * nbComps )
        for ( int iV = 0, iR = 0; iR < nbRows; ++iR )
          for ( int iC = 1; iC < nbColumns; ++iC )
            setItem( iR, iC, new CheckItem( vals[ iV++ ]));
    }
    connect( this, SIGNAL( itemClicked(QTableWidgetItem *)), this, SLOT( setIsChanged() ));
    break;
  }
  case 1:
  {
    GEOM::GEOM_IntFieldStep_var is = GEOM::GEOM_IntFieldStep::_narrow( stepVar );
    if ( !is->_is_nil() )
    {
      GEOM::ListOfLong_var vals = is->GetValues();
      if ( vals->length() == nbRows * nbComps )
        for ( int iV = 0, iR = 0; iR < nbRows; ++iR )
          for ( int iC = 1; iC < nbColumns; ++iC )
            setItem( iR, iC, new IntSpinItem( vals[ iV++ ]));
    }
    break;
  }
  case 2:
  {
    GEOM::GEOM_DoubleFieldStep_var ds = GEOM::GEOM_DoubleFieldStep::_narrow( stepVar );
    if ( !ds->_is_nil() )
    {
      GEOM::ListOfDouble_var vals = ds->GetValues();
      if ( vals->length() == nbRows * nbComps )
        for ( int iV = 0, iR = 0; iR < nbRows; ++iR )
          for ( int iC = 1; iC < nbColumns; ++iC )
            setItem( iR, iC, new DoubleSpinItem( vals[ iV++ ]));
    }
    break;
  }
  default:
    GEOM::GEOM_StringFieldStep_var ss = GEOM::GEOM_StringFieldStep::_narrow( stepVar );
    if ( !ss->_is_nil() )
    {
      GEOM::string_array_var vals = ss->GetValues();
      if ( vals->length() == nbRows * nbComps )
        for ( int iV = 0, iR = 0; iR < nbRows; ++iR )
          for ( int iC = 1; iC < nbColumns; ++iC )
            setItem( iR, iC, new QTableWidgetItem( vals[ iV++ ].in() ));
    }
    break;
  }
}

//=======================================================================
// name    : EntityGUI_FieldDlg::StepTable::~StepTable
// Purpose : Destructor
//=======================================================================
EntityGUI_FieldDlg::StepTable::~StepTable()
{
}

//=======================================================================
// name    : EntityGUI_FieldDlg::StepTable::minimumSizeHint
// Purpose : Get minimum size for the table
//=======================================================================
QSize EntityGUI_FieldDlg::StepTable::minimumSizeHint() const
{
  QSize s = QTableWidget::minimumSizeHint();
  QHeaderView* hv = horizontalHeader();
  if ( hv )
    s.setWidth( qMax( s.width(), hv->length() ) );
  return s;
}

//=======================================================================
// name    : EntityGUI_FieldDlg::StepTable::setEditable
// Purpose : Set editable of specified cell
//=======================================================================
void EntityGUI_FieldDlg::StepTable::setEditable (bool isEditable,
                                                 int row, int col)
{
  QTableWidgetItem* anItem = item( row, col );
  if ( anItem ) {
    bool isSignalsBlocked = signalsBlocked();
    blockSignals( true );

    Qt::ItemFlags f = anItem->flags();
    if ( !isEditable ) f = f & ~Qt::ItemIsEditable;
    else f = f | Qt::ItemIsEditable;
    anItem->setFlags( f );
    
    blockSignals( isSignalsBlocked );
  }
}

//=======================================================================
// name    : EntityGUI_FieldDlg::StepTable::isEditable
// Purpose : Verify wheter cell is editable
//=======================================================================
bool EntityGUI_FieldDlg::StepTable::isEditable (int row, int col) const
{
  QTableWidgetItem* anItem = item( row, col );
  return anItem == 0 || anItem->flags() & Qt::ItemIsEditable;
}

void EntityGUI_FieldDlg::StepTable::setReadOnly( bool on )
{
  setEditTriggers( on ? 
                   QAbstractItemView::NoEditTriggers  :
                   QAbstractItemView::AllEditTriggers );
}

bool EntityGUI_FieldDlg::StepTable::isReadOnly() const
{
  return editTriggers() == QAbstractItemView::NoEditTriggers;
}

//=======================================================================
// name    : EntityGUI_FieldDlg::StepTable::insertRows
// Purpose : Insert rows (virtual redefined)
//=======================================================================
void EntityGUI_FieldDlg::StepTable::insertRows (int row, int count)
{
  while ( count-- ) insertRow( row );
}

//=======================================================================
// name    : EntityGUI_FieldDlg::StepTable::text
// Purpose : Get text from cell (virtual redefined)
//=======================================================================
QString EntityGUI_FieldDlg::StepTable::text (int row, int col)
{
  closePersistentEditor( currentItem() );
  QTableWidgetItem* anItem = item( row, col );
  return anItem ? anItem->text() : QString();
}

QList<int> EntityGUI_FieldDlg::StepTable::selectedRows()
{
  QList<QTableWidgetItem*> selItems = selectedItems();
  QTableWidgetItem* anItem;
  QList<int> rows;

  foreach( anItem, selItems ) {
    int r = row( anItem );
    if ( !rows.contains( r ) ) rows.append( r );
  }

  qSort( rows );
  return rows;
}

//=======================================================================
//function : selectRows
//purpose  : 
//=======================================================================

void EntityGUI_FieldDlg::StepTable::selectRows(const QList<int>& rows)
{
  closePersistentEditor( currentItem() );

  QList<QTableWidgetSelectionRange> ranges = selectedRanges();
  for ( int i = 0; i < ranges.count(); ++i )
    setRangeSelected( ranges[i], false );

  QList<int>::const_iterator row = rows.begin();
  for ( ; row != rows.end(); ++row )
    // QTableWidgetSelectionRange ( int top, int left, int bottom, int right )
    setRangeSelected ( QTableWidgetSelectionRange(*row,0,*row,0), true );
}

//=======================================================================
//function : setDim
//purpose  : change nb rows in the table
//=======================================================================

void EntityGUI_FieldDlg::StepTable::setDim( int nbRows, QString shapeName, bool setDefault )
{
  closePersistentEditor( currentItem() );
  int curNbRows = rowCount();
  if ( nbRows < curNbRows )
  {
    while ( nbRows < curNbRows )
      removeRow( --curNbRows );
  }
  else if ( nbRows > curNbRows )
  {
    int nbColumns = columnCount();
    for ( ; nbRows > curNbRows; ++curNbRows )
    {
      insertRow( curNbRows );
      if ( setDefault )
        for ( int iC = 1; iC < nbColumns; ++iC )
          setItem( curNbRows, iC, newDefaultItem() );
    }
  }

  int indexWidth = 1, maxNbR = 10;
  while ( nbRows >= maxNbR )
    ++indexWidth, maxNbR *= 10;
  shapeName = shapeName.toLower() + "_%1";
  for ( int iR = 0; iR < nbRows; ++iR )
  {
    setItem( iR, 0, new QTableWidgetItem( shapeName.arg( iR+1, indexWidth, 10, QChar('0') )));
    setEditable( false, iR, 0 );
  }
}

//=======================================================================
//function : setNbComps
//purpose  : 
//=======================================================================

void EntityGUI_FieldDlg::StepTable::setNbComps( int nbComps )
{
  closePersistentEditor( currentItem() );
  nbComps++; // add sub-shape column
  int curNbCols = columnCount();
  if ( nbComps < curNbCols )
  {
    while ( nbComps < curNbCols )
      removeColumn( --curNbCols );
  }
  else if ( nbComps > curNbCols )
  {
    int nbRows = rowCount();
    for ( ; nbComps > curNbCols; ++curNbCols )
    {
      insertColumn( curNbCols );
      setHorizontalHeaderItem( curNbCols,
                               new QTableWidgetItem(QString("Comp %1").arg( curNbCols )));
      for ( int iR = 0; iR < nbRows; ++iR )
        setItem( iR, curNbCols, newDefaultItem() );
    }
  }
}

//=======================================================================
//function : setDataType
//purpose  : change type of data
//=======================================================================

void EntityGUI_FieldDlg::StepTable::setDataType( int dataType )
{
  myDataType = dataType;

  int nbRows = rowCount();
  int nbColumns = columnCount();

  for ( int iR = 0; iR < nbRows; ++iR )
    for ( int iC = 1; iC < nbColumns; ++iC )
      setItem( iR, iC, newDefaultItem() );
}

//=======================================================================
//function : newDefaultItem
//purpose  : creates a table item with a default value
//=======================================================================

QTableWidgetItem * EntityGUI_FieldDlg::StepTable::newDefaultItem()
{
  switch( myDataType ) {
  case 0: return new CheckItem(false);
  case 1: return new IntSpinItem(0);
  case 2: return new DoubleSpinItem(0);
  default:;
  }
  return new QTableWidgetItem(""); // string
}

//=======================================================================
//function : getHeaders
//purpose  : 
//=======================================================================

QStringList EntityGUI_FieldDlg::StepTable::getHeaders()
{
  QStringList headers;
  int nbColumns = columnCount();
  for ( int iC = 0; iC < nbColumns; ++iC )
    headers << horizontalHeaderItem( iC )->text();
  return headers;
}

//=======================================================================
//function : setHeaders
//purpose  : 
//=======================================================================

void EntityGUI_FieldDlg::StepTable::setHeaders(const QStringList& headers)
{
  setHorizontalHeaderLabels( headers );
}

//=======================================================================
//function : setValues
//purpose  : store values from this table to a field step
//=======================================================================

void EntityGUI_FieldDlg::StepTable::setValues(GEOM::GEOM_FieldStep_var& step)
{
  closePersistentEditor( currentItem() );

  if ( step->GetStamp() != myStamp )
    step->SetStamp( myStamp );

  if ( step->_is_equivalent( myStep ) && !myIsChanged )
    return;

  const int nbColumns = columnCount();
  const int nbComps = nbColumns - 1;
  const int nbRows = rowCount();

  switch ( myDataType ) {
  case 0:
  {
    GEOM::GEOM_BoolFieldStep_var bs = GEOM::GEOM_BoolFieldStep::_narrow( step );
    if ( !bs->_is_nil() )
    {
      GEOM::short_array_var vals = new GEOM::short_array();
      vals->length( nbRows * nbComps );
      for ( int iV = 0, iR = 0; iR < nbRows; ++iR )
        for ( int iC = 1; iC < nbColumns; ++iC )
          vals[ iV++ ] = ((CheckItem*)item( iR, iC ))->value();
      bs->SetValues( vals );
    }
    break;
  }
  case 1:
  {
    GEOM::GEOM_IntFieldStep_var is = GEOM::GEOM_IntFieldStep::_narrow( step );
    if ( !is->_is_nil() )
    {
      GEOM::ListOfLong_var vals = new GEOM::ListOfLong();
      vals->length( nbRows * nbComps );
      for ( int iV = 0, iR = 0; iR < nbRows; ++iR )
        for ( int iC = 1; iC < nbColumns; ++iC )
          vals[ iV++ ] = text( iR, iC ).toInt();
      is->SetValues( vals );
    }
    break;
  }
  case 2:
  {
    GEOM::GEOM_DoubleFieldStep_var ds = GEOM::GEOM_DoubleFieldStep::_narrow( step );
    if ( !ds->_is_nil() )
    {
      GEOM::ListOfDouble_var vals = new GEOM::ListOfDouble();
      vals->length( nbRows * nbComps );
      for ( int iV = 0, iR = 0; iR < nbRows; ++iR )
        for ( int iC = 1; iC < nbColumns; ++iC )
          vals[ iV++ ] = text( iR, iC ).toDouble();
      ds->SetValues( vals );
    }
    break;
  }
  default:

    GEOM::GEOM_StringFieldStep_var ss = GEOM::GEOM_StringFieldStep::_narrow( step );
    if ( !ss->_is_nil() )
    {
      GEOM::string_array_var vals = new GEOM::string_array();
      vals->length( nbRows * nbComps );
      for ( int iV = 0, iR = 0; iR < nbRows; ++iR )
        for ( int iC = 1; iC < nbColumns; ++iC )
          vals[ iV++ ] = item( iR, iC )->text().toLatin1().constData();
      ss->SetValues( vals );
    }
  }

  myIsChanged = false;

  return;
}

//=======================================================================
//function : headerDblClicked
//purpose  : rename column, called when used double-clicks on the header
//=======================================================================

void EntityGUI_FieldDlg::StepTable::headerDblClicked( int section )
{
  if ( section > 0 ) {
    bool bOk;
    QString label = QInputDialog::getText( this, EntityGUI_FieldDlg::tr( "RENAME_COMPONENT" ),
                                           EntityGUI_FieldDlg::tr ( "COMPONENT_NAME" ), QLineEdit::Normal,
                                           horizontalHeaderItem( section )->text(), &bOk );
    if ( bOk && !label.isEmpty() )
      horizontalHeaderItem( section )->setText( label );
  }
}

//=======================================================================
//function : EntityGUI_FieldDlg
//purpose  : 
//=======================================================================

EntityGUI_FieldDlg::EntityGUI_FieldDlg (GeometryGUI* theGeometryGUI,
                                        GEOM::GEOM_Field_ptr theField, int stepID,
                                        QWidget* parent,
                                        bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl),
    myIsCreation( CORBA::is_nil( theField )),
    myField( GEOM::GEOM_Field::_duplicate( theField )),
    myCurStepID( stepID ),
    myCurStepTable( NULL ),
    myIsHiddenMain( false )
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  QPixmap iconSelect (resMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(myIsCreation ? tr("CREATE_FIELD_TITLE") : tr("EDIT_FIELD_TITLE"));

  // Shape type button group
  mainFrame()->GroupConstructors->hide();

  // Field name
  mainFrame()->GroupBoxName->setTitle(tr("FIELD_NAME"));

  // Field properties

  QGroupBox* propGroup = new QGroupBox(tr("PROPERTIES"), centralWidget());
  QGridLayout* propLayout = new QGridLayout(propGroup);
  propLayout->setMargin(9);
  propLayout->setSpacing(6);

  // shape
  QLabel* shapeLabel = new QLabel(tr("SHAPE"), propGroup);
  myShapeSelBtn = new QPushButton(propGroup);
  myShapeSelBtn->setIcon(iconSelect);
  myShapeSelBtn->setEnabled( myIsCreation );
  myShapeName = new QLineEdit(propGroup);
  myShapeName->setReadOnly(true);
  myShapeName->setEnabled( myIsCreation );

  // data type
  QLabel* typeLabel = new QLabel( tr("DATA_TYPE"), propGroup );
  myTypeCombo = new QComboBox( propGroup );
  myTypeCombo->insertItem( GEOM::FDT_Bool,   tr("BOOL"));
  myTypeCombo->insertItem( GEOM::FDT_Int,    tr("INT"));
  myTypeCombo->insertItem( GEOM::FDT_Double, tr("DOUBLE"));
  myTypeCombo->insertItem( GEOM::FDT_String, tr("STRING"));
  myTypeCombo->setCurrentIndex( GEOM::FDT_Double ); // double

  // dimension
  QLabel* dimLabel = new QLabel( tr("SHAPE_TYPE"), propGroup );
  myDimCombo = new QComboBox( propGroup );

  // nb components
  QLabel* nbCompsLabel = new QLabel( tr("NB_COMPS"), propGroup );
  myNbCompsSpin = new SalomeApp_IntSpinBox( 1, 1000, 1, propGroup, true, true );
  myNbCompsSpin->setValue( 1 );
  propLayout->addWidget(shapeLabel,         0, 0);
  propLayout->addWidget(myShapeSelBtn,      0, 1);
  propLayout->addWidget(myShapeName,        0, 2);
  propLayout->addWidget(typeLabel,          1, 0, 1, 2);
  propLayout->addWidget(myTypeCombo,        1, 2);
  propLayout->addWidget(dimLabel,           2, 0, 1, 2);
  propLayout->addWidget(myDimCombo,         2, 2);
  propLayout->addWidget(nbCompsLabel,       3, 0, 1, 2);
  propLayout->addWidget(myNbCompsSpin,      3, 2);
  // propLayout->addWidget(shapeLabel,         0, 0);
  // propLayout->addWidget(myShapeName,        0, 1);
  // propLayout->addWidget(typeLabel,          1, 0);
  // propLayout->addWidget(myTypeCombo,        1, 1);
  // propLayout->addWidget(dimLabel,           2, 0);
  // propLayout->addWidget(myDimCombo,         2, 1);
  // propLayout->addWidget(nbCompsLabel,       3, 0);
  // propLayout->addWidget(myNbCompsSpin,      3, 1);

  propLayout->setColumnStretch(2, 5);

  // Field values

  QGroupBox* valsGroup = new QGroupBox(tr("VALUES"), centralWidget());
  QGridLayout* valsLayout = new QGridLayout(valsGroup);
  valsLayout->setMargin(9);
  valsLayout->setSpacing(6);

  // value table
  mySwitchTableWdg = new QWidget(valsGroup);
  QVBoxLayout* switchTableGrpLayout = new QVBoxLayout(mySwitchTableWdg);
  switchTableGrpLayout->setMargin(0);
  switchTableGrpLayout->setSpacing(0);

  // step add/rm controls
  QPushButton* addStepBtn = new QPushButton( tr("ADD_STEP"), valsGroup );
  QLabel* curStepLbl = new QLabel(tr("STEP"), valsGroup );
  myStepEdit = new QLineEdit( valsGroup );
  myStepsCombo = new QComboBox(valsGroup);
  myStepsCombo->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  myRmStepBtn = new QPushButton( tr("REMOVE_STEP"), valsGroup );

  // step browse controls
  myPrevStepBtn = new QPushButton( tr("PREV_STEP"), valsGroup );
  QLabel* stampLbl = new QLabel(tr("STAMP"), valsGroup );
  myStampSpin = new SalomeApp_IntSpinBox( -theIntLimit, theIntLimit, 1, valsGroup, true, true);
  myNextStepBtn = new QPushButton( tr("NEXT_STEP"), valsGroup );
  
  valsLayout->addWidget(mySwitchTableWdg,   0, 0, 1, 5);
  valsLayout->addWidget(addStepBtn,         1, 0);
  valsLayout->addWidget(curStepLbl,         1, 1);
  valsLayout->addWidget(myStepEdit,         1, 2);
  valsLayout->addWidget(myStepsCombo,       1, 3);
  valsLayout->addWidget(myRmStepBtn,        1, 4);
  valsLayout->addWidget(myPrevStepBtn,      2, 0);
  valsLayout->addWidget(stampLbl,           2, 1);
  valsLayout->addWidget(myStampSpin,        2, 2, 1, 2);
  valsLayout->addWidget(myNextStepBtn,      2, 4);

  valsLayout->setColumnStretch(3, 5);
  valsLayout->setRowStretch   (0, 5);

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(propGroup);
  layout->addWidget(valsGroup);

  setHelpFileName("geom_field_page.html");

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  connect(aSelMgr,       SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  connect(myPrevStepBtn, SIGNAL(clicked()),                 this, SLOT( onPrevStep() ));
  connect(myNextStepBtn, SIGNAL(clicked()),                 this, SLOT( onNextStep() ));
  connect(addStepBtn,    SIGNAL(clicked()),                 this, SLOT( onAddStep() ));
  connect(myRmStepBtn,   SIGNAL(clicked()),                 this, SLOT( onRmStep() ));
  connect(myStampSpin,   SIGNAL(valueChanged(int)),         this, SLOT( onStampChange() ));
  connect(myStepsCombo,  SIGNAL(currentIndexChanged(int)),  this, SLOT( showCurStep() ));
  connect(myStepsCombo,  SIGNAL(activated(int)),            this, SLOT( showCurStep() ));

  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  Init();
  //updateState();
}

//=======================================================================
//function : ~EntityGUI_FieldDlg
//purpose  : 
//=======================================================================

EntityGUI_FieldDlg::~EntityGUI_FieldDlg()
{
  GEOM_Displayer* aDisplayer = getDisplayer();
  if (myIsHiddenMain) {
    aDisplayer->Display(myShape);
    myIsHiddenMain = false;
  }
  // if ( !myField->_is_nil())
  //   aDisplayer->Display(myField);

  QListIterator<int> anIter( myHiddenFieldStepIds );
  while( anIter.hasNext() )
  {
    const int aStepId = anIter.next();
    GEOM::GEOM_FieldStep_var step = myField->GetStep( aStepId );
    if( !step->_is_nil() )
      aDisplayer->Display( step, false );
  }
  aDisplayer->UpdateViewer();
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void EntityGUI_FieldDlg::Init()
{
  myDmMode = -1;
  myHiddenFieldStepIds.clear();
  myStepsCombo->clear();

  if ( myIsCreation || myField->_is_nil() )
  {
    myIsCreation = true;
    myStepTables.clear();
    if ( myCurStepTable )
      myCurStepTable->hide();
    myCurStepTable = NULL;
    myCurStepID = 0;

    initName(tr("FIELD_PREFIX"));
    myStampSpin->setValue(0);

    SelectionIntoArgument();

    onAddStep();

    connect(myShapeSelBtn, SIGNAL(clicked()),                 this, SLOT(SetEditCurrentArgument()));
    connect(myTypeCombo,   SIGNAL(currentIndexChanged(int)),  this, SLOT( onTypeChange()));
    connect(myDimCombo,    SIGNAL(currentIndexChanged(int)),  this, SLOT( onDimChange()));
    connect(myNbCompsSpin, SIGNAL(valueChanged(int)),         this, SLOT( onNbCompsChange()));

  }
  else // edition
  {
    // 1) get and sort step IDs
    // 2) hide all displayed field steps' presentations
    GEOM_Displayer* aDisplayer = getDisplayer();
    GEOM::ListOfLong_var stepIDs = myField->GetSteps();
    size_t i;
    QList< int > stepsList;
    for ( i = 0; i < stepIDs->length(); ++i )
    {
      int aStepId = stepIDs[i];
      stepsList.push_back( aStepId );
      GEOM::GEOM_FieldStep_var step = myField->GetStep( aStepId );
      if( !step->_is_nil() )
      {
        QString anEntry( step->GetStudyEntry() );
        if( aDisplayer->IsDisplayed( anEntry ) )
        {
          aDisplayer->Erase( step, false, false );
          myHiddenFieldStepIds << aStepId;
        }
      }
    }
    qSort( stepsList.begin(), stepsList.end() );

    myIsCreation = false;

    CORBA::String_var fName = myField->GetName();
    myMainFrame->ResultName->setText( fName.in() );

    myShape = myField->GetShape();
    CORBA::String_var sName;
    if ( !myShape->_is_nil() )
      sName = myShape->GetName();
    myShapeName->setText( sName.in() ? sName.in() : "");

    myTypeCombo->setCurrentIndex( myField->GetDataType() );

    updateDims( myField->GetDimension() );
    updateShapeIDs();

    GEOM::string_array_var compNames = myField->GetComponents();
    myNbCompsSpin->setValue( compNames->length() );

    myShapeSelBtn->setEnabled( false );
    myShapeName->setEnabled( false );
    myTypeCombo->setEnabled( false );
    myDimCombo->setEnabled( false );
    myNbCompsSpin->setEnabled( false );

    myStepsCombo->blockSignals( true );
    for ( i = 0; i < stepIDs->length(); ++i )
    {
      myStepsCombo->insertItem( i, QString::number( stepsList[i] ));
      if ( myCurStepID == stepsList[i] )
        myStepsCombo->setCurrentIndex( i );
    }
    // if ( myStepsCombo->count() == 0 )
    //   myStepsCombo->insertItem( 0, QString::number( myCurStepID ));
    myStepsCombo->blockSignals( false );

    showCurStep();
    activateSelection();
  }
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void EntityGUI_FieldDlg::enterEvent(QEvent* e)
{
  if (!buttonCancel()->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void EntityGUI_FieldDlg::ClickOnOk()
{
  setIsApplyAndClose(true);
  if (ClickOnApply())
    ClickOnCancel();
  setIsApplyAndClose(false);
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool EntityGUI_FieldDlg::ClickOnApply()
{
  if(!isApplyAndClose()) {
    setIsDisableBrowsing( true );
    setIsDisplayResult( false );
  }

  QString msg;
  if ( !isValid( msg ) ) {
    showError( msg );
    return false;
  }
  SUIT_OverrideCursor wc;
  SUIT_Session::session()->activeApplication()->putInfo( "" );

  try {
    if ( openCommand() )
      if (!execute (/*isApplyAndClose()*/))
      {
        abortCommand();
        showError();
        return false;
      }
  }
  catch( const SALOME::SALOME_Exception& e ) {
    SalomeApp_Tools::QtCatchCorbaException( e );
    abortCommand();
    return false;
  }
  commitCommand();

  if(!isApplyAndClose()) {
    setIsDisableBrowsing( false );
    setIsDisplayResult( true );
  }

  if ( myIsCreation )
  {
    myField = GEOM::GEOM_Field::_nil();
    if ( !isApplyAndClose() )
      Init();
  }
  return true;
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void EntityGUI_FieldDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  activateSelection();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void EntityGUI_FieldDlg::SetEditCurrentArgument()
{
  myEditCurrentArgument = myShapeName;
  SelectionIntoArgument();
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed
//=================================================================================
void EntityGUI_FieldDlg::SelectionIntoArgument()
{
  if (myEditCurrentArgument == myShapeName ||  // Selection of a shape is active
      myShape->_is_nil())
  {
    myShapeName->setText("");
    myShape = GEOM::GEOM_Object::_nil();

    LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
    SALOME_ListIO aSelList;
    aSelMgr->selectedObjects(aSelList);
    int nbSel = aSelList.Extent();

    if (nbSel == 1) {
      GEOM::GEOM_Object_var anObj =
        GEOMBase::ConvertIOinGEOMObject(aSelList.First());

      if ( GEOMBase::IsShape(anObj) && anObj->IsMainShape() ) {
        if (myIsHiddenMain) {
          GEOM_Displayer* aDisplayer = getDisplayer();
          aDisplayer->Display(myShape);
          myIsHiddenMain = false;
        }
        myShapeName->setText(GEOMBase::GetName(anObj));

        if ( !anObj->_is_nil() && !anObj->IsSame( myShape ))
        {
          myShape = anObj;
          myShapeMap.Clear();
          myEditCurrentArgument = 0;

          // re-fill myDimCombo
          //const int curDim = getDim();
          updateDims( -100 );

          // update table
          onDimChange();
        }
      }
    }
    else {
      if (myIsHiddenMain) {
        GEOM_Displayer* aDisplayer = getDisplayer();
        aDisplayer->Display(myShape);
        myIsHiddenMain = false;
      }
    }
  }
  else // select rows of a table according to shape selection
  {
    if ( !myCurStepTable )
      return;

    bool isSignalsBlocked = myCurStepTable->signalsBlocked();
    myCurStepTable->blockSignals(true);

    QList< int > rowsToSelect;
    TColStd_IndexedMapOfInteger aMapIndex;
    if ( getSelectedSubshapes(aMapIndex ))
      for (int ii = 1, nn = aMapIndex.Extent(); ii <= nn; ii++) {
        const int shapeID = aMapIndex( ii );
        for (int row = 0, n = myShapeIDs.size(); row < n; row++)
          if ( myShapeIDs[row] == shapeID ) {
            rowsToSelect.push_back( row );
            break;
          }
      }
    myCurStepTable->selectRows( rowsToSelect );

    myCurStepTable->blockSignals(isSignalsBlocked);
  }
}

//=================================================================================
// function : onDimChange()
// purpose  : update dialog at change of dimension
//=================================================================================
void EntityGUI_FieldDlg::onDimChange()
{
  // get order of sub-shapes - myShapeIDs
  updateShapeIDs();

  // update size of tables
  const int nbRows = myShapeIDs.size();
  QString subName = ( getDim() == -1 ) ? tr("WHOLE_SHAPE_VHEADER") : myDimCombo->currentText();
  QMap< int, StepTable* >::iterator tblIt = myStepTables.begin();
  for ( ; tblIt != myStepTables.end(); ++tblIt )
    if ( tblIt.value() )
      tblIt.value()->setDim( nbRows, subName );

  activateSelection();
}

//=======================================================================
//function : onNbCompsChange
//purpose  : 
//=======================================================================

void EntityGUI_FieldDlg::onNbCompsChange()
{
  QMap< int, StepTable* >::iterator tblIt = myStepTables.begin();
  for ( ; tblIt != myStepTables.end(); ++tblIt )
    if ( tblIt.value() )
      tblIt.value()->setNbComps( getNbComps() );
}

//=======================================================================
//function : onTypeChange
//purpose  : 
//=======================================================================

void EntityGUI_FieldDlg::onTypeChange()
{
  QMap< int, StepTable* >::iterator tblIt = myStepTables.begin();
  for ( ; tblIt != myStepTables.end(); ++tblIt )
    if ( tblIt.value() )
      tblIt.value()->setDataType( myTypeCombo->currentIndex() );
}

//=================================================================================
// function : onPrevStep()
// purpose  :
//=================================================================================
void EntityGUI_FieldDlg::onPrevStep()
{
  int i = myStepsCombo->currentIndex();
  if ( i > 0 )
  {
    myStepsCombo->setCurrentIndex( i-1 );
    showCurStep();
  }
}

//=================================================================================
// function : onNextStep()
// purpose  :
//=================================================================================
void EntityGUI_FieldDlg::onNextStep()
{
  int i = myStepsCombo->currentIndex();
  if ( i+1 < myStepsCombo->count() )
  {
    myStepsCombo->setCurrentIndex( i+1 );
    showCurStep();
  }
}

static int findInCombo( QComboBox* where, int what, bool& ok )
{
  int idx = 0;
  ok = false;

  for ( ; idx < where->count() && !ok; idx++ ) {
    int step = where->itemText( idx ).toInt();
    if ( step == what ) {
      ok = true;
    }
    else if ( step > what )
      break;
  }
  
  return idx;
}

//=======================================================================
//function : onAddStep
//purpose  : 
//=======================================================================
void EntityGUI_FieldDlg::onAddStep()
{
  int step = 0;

  if ( !myStepEdit->text().isEmpty() )
    step = myStepEdit->text().toInt();
  bool ok = false;
  int idx = findInCombo( myStepsCombo, step, ok );
  
  if ( ok ) {
    SUIT_MessageBox::critical(this, QObject::tr("ERR_ERROR"),
			      tr("ERR_STEP_EXISTS"));
    return;
  }

  myStepsCombo->insertItem( idx, QString::number( step ));
  myStepsCombo->setCurrentIndex( idx );
  myRemovedSteps.remove( getCurStepID() );
  //showCurStep();
}

//=======================================================================
//function : onRmStep
//purpose  : 
//=======================================================================
void EntityGUI_FieldDlg::onRmStep()
{
  if ( myStepsCombo->count() > 1 )
  {
    myStepTables.remove( getCurStepID() );
    myRemovedSteps.insert( getCurStepID() );
    if ( myCurStepTable )
      myCurStepTable->hide();
    myCurStepTable = NULL;
    myStepsCombo->removeItem( myStepsCombo->currentIndex() );
    //showCurStep();
  }
}

//=======================================================================
//function : onStampChange
//purpose  : 
//=======================================================================
void EntityGUI_FieldDlg::onStampChange()
{
  if ( myCurStepTable )
    myCurStepTable->setStamp( myStampSpin->value() );
}

//=======================================================================
//function : showCurStep
//purpose  : 
//=======================================================================
void EntityGUI_FieldDlg::showCurStep()
{
  myCurStepID = getCurStepID(); 
  myStepEdit->setText(QString::number( myCurStepID ));

  QStringList headers;
  if ( myCurStepTable )
  {
    if ( myCurStepTable->getStepID() == myCurStepID )
    {
      myCurStepTable->show();
      return;
    }
    else
    {
      myCurStepTable->hide();
      headers = myCurStepTable->getHeaders();
      myCurStepTable->getStamp();
    }
  }

  if ( myStepTables.count ( myCurStepID ))
  {
    myCurStepTable = myStepTables[ myCurStepID ];
    myCurStepTable->setHeaders( headers );
  }
  else
  {
    // if ( myStepsCombo->count() == 0 )
    //   return; // edit a field with no steps

    // get step values
    GEOM::GEOM_FieldStep_var stepVar;
    if ( !myField->_is_nil() )
      stepVar = myField->GetStep( myCurStepID );

    int nbComps = getNbComps();
    if ( headers.count() == 0 )
    {
      headers << tr("SUB_SHAPE_HEADER");
      if ( !myIsCreation && !myField->_is_nil() )
      {
        GEOM::string_array_var compNames = myField->GetComponents();
        for ( int iC = 0; iC < compNames->length(); ++iC )
          headers << compNames[ iC ].in();
      }
      else
      {
        for ( int iC = 0; iC < nbComps; ++iC )
          headers << QString("Comp %1").arg( iC + 1 );
      }
    }
    QString subName = ( getDim() == -1 ) ? tr("WHOLE_SHAPE_VHEADER") : myDimCombo->currentText();
    myCurStepTable = new StepTable( myCurStepID, getDataType(), myShapeIDs.size(),
                                    nbComps, subName, headers, stepVar, mySwitchTableWdg );
    myStepTables[ myCurStepID ] = myCurStepTable;
    mySwitchTableWdg->layout()->addWidget( myCurStepTable );
  }
  myCurStepTable->show();
  myStampSpin->setValue( myCurStepTable->getStamp() );

  connect(myCurStepTable, SIGNAL(itemSelectionChanged()), this, SLOT( highlightSubShapes() ));

  myPrevStepBtn->setEnabled( myStepsCombo->currentIndex() > 0 );
  myNextStepBtn->setEnabled( myStepsCombo->currentIndex()+1 < myStepsCombo->count() );
  myRmStepBtn->setEnabled( myStepsCombo->count() > 1 );
}

//=================================================================================
// function : getSelectedSubshapes
// purpose  :
//=================================================================================
int EntityGUI_FieldDlg::getSelectedSubshapes (TColStd_IndexedMapOfInteger& theMapIndex)
{
  theMapIndex.Clear();

  SalomeApp_Application* app = myGeomGUI->getApp();
  if (!app || myShape->_is_nil())
    return 0;

  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  // try to find out and process the global selection
  // (of not published objects and of published sub-shapes)
  {
    SALOME_ListIteratorOfListIO anIter (aSelList);
    for (int i = 0; anIter.More(); anIter.Next(), i++)
    {
      Handle(SALOME_InteractiveObject) anIObj = anIter.Value();
      QString anEntry = anIObj->getEntry();
      QString str = "_";
      int index = anEntry.lastIndexOf(str);
      if (index > 0) // selection among special preview
      {
        anEntry.remove(0, index+1);
        int anIndex = anEntry.toInt();
        if (anIndex)
          theMapIndex.Add(anIndex);
      }
      else // selection among published shapes
      {
        SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>(app->activeStudy());
        if (!appStudy) return 0;
        _PTR(Study) aStudy = appStudy->studyDS();

        _PTR(SObject) aSObj (aStudy->FindObjectID(anEntry.toLatin1().constData()));
        GEOM::GEOM_Object_var aGeomObj =
          GEOM::GEOM_Object::_narrow(GeometryGUI::ClientSObjectToObject(aSObj));
        TopoDS_Shape aShape;
        if (GEOMBase::GetShape(aGeomObj, aShape)) {
          if (aGeomObj->GetType() == GEOM_GROUP || aShape.ShapeType() == getShapeType())
          {
            TopExp_Explorer anExp (aShape, getShapeType());
            for (; anExp.More(); anExp.Next()) {
              TopoDS_Shape aSubShape = anExp.Current();
              int anIndex = myShapeMap.FindIndex(aSubShape);
              if (anIndex == 0) {
                SUIT_MessageBox::warning(app->desktop(), QObject::tr("WRN_WARNING"),
                                         tr("WRN_NOT_SUBSHAPE"));
              }
              theMapIndex.Add(anIndex);
            }
          }
        }
      }
    } // for aSelList
  }

  return theMapIndex.Extent();
}

//=================================================================================
// function : getShapeType()
// purpose  :
//=================================================================================
TopAbs_ShapeEnum EntityGUI_FieldDlg::getShapeType(int* dim) const
{
  
  switch ( dim ? *dim : getDim() )
  {
  case 0:  return TopAbs_VERTEX;
  case 1:  return TopAbs_EDGE;
  case 2:  return TopAbs_FACE;
  case 3:  return TopAbs_SOLID;
  default: return TopAbs_SHAPE;
  }
}

//=======================================================================
//function : getDim
//purpose  : 
//=======================================================================
int EntityGUI_FieldDlg::getDim() const
{
  int i = myDimCombo->currentIndex();
  return ( i < 0 ) ? -1 : myDimCombo->itemData( i ).toInt();
}

//=======================================================================
//function : getDataType
//purpose  : 
//=======================================================================
int EntityGUI_FieldDlg::getDataType() const
{
  return myTypeCombo->currentIndex();
}

//=======================================================================
//function : getCurStepID
//purpose  : 
//=======================================================================
int EntityGUI_FieldDlg::getCurStepID() const
{
  if ( myStepsCombo->count() > 0 )
    return myStepsCombo->currentText().toInt();
  return myCurStepID;
}

//=======================================================================
//function : getNbComps
//purpose  : 
//=======================================================================
int EntityGUI_FieldDlg::getNbComps() const
{
  return myNbCompsSpin->value();
}

//=================================================================================
// function : updateShapeIDs()
// purpose  : update myShapeIDs
//=================================================================================
void EntityGUI_FieldDlg::updateShapeIDs()
{
  myShapeIDs.clear();
  //myShapeMap.Clear();
  if ( !myShape->_is_nil() )
  {
    TopoDS_Shape aShape =
      GEOM_Client::get_client().GetShape(GeometryGUI::GetGeomGen(), myShape);
    if ( myShapeMap.IsEmpty() )
      TopExp::MapShapes(aShape, myShapeMap);
    TopAbs_ShapeEnum shapeType = getShapeType();
    if ( shapeType == TopAbs_SHAPE )
    {
      myShapeIDs.push_back( 1 );
    }
    else
    {
      TopTools_IndexedMapOfShape dimSubShapesMap;
      TopExp::MapShapes(aShape, shapeType, dimSubShapesMap);
      myShapeIDs.resize( dimSubShapesMap.Extent() );
      for ( int i = 1; i <= dimSubShapesMap.Extent(); ++i )
        myShapeIDs[i-1] = myShapeMap.FindIndex( dimSubShapesMap(i) );
    }
  }
}

//=======================================================================
//function : updateDims
//purpose  : update myDimCombo
//=======================================================================
void EntityGUI_FieldDlg::updateDims(int curDim)
{
  myDimCombo->blockSignals( true );
  myDimCombo->clear();
  TopoDS_Shape aShape =
    GEOM_Client::get_client().GetShape(GeometryGUI::GetGeomGen(), myShape);

  if ( !aShape.IsNull() )
  {
    const char* subNames[4] = { "VERTEX","EDGE","FACE","SOLID" };
    TopExp_Explorer exp;
    for ( int dim = 0; dim < 4; ++dim )
    {
      exp.Init( aShape, getShapeType(&dim));
      if ( exp.More() && !aShape.IsSame( exp.Current() ))
        myDimCombo->insertItem( dim, tr( subNames[dim] ), dim );
    }
    myDimCombo->insertItem( 4, tr("WHOLE"), -1 );

    int idx = myDimCombo->findData( curDim );
    if ( idx != -1 )
      myDimCombo->setCurrentIndex( idx );
    else 
      myDimCombo->setCurrentIndex( myDimCombo->count()-1-(myDimCombo->count()>1?1:0) );
  }
  myDimCombo->blockSignals( false );
}

//=================================================================================
// function : activateSelection
// purpose  : Activate local selection
//=================================================================================
void EntityGUI_FieldDlg::activateSelection()
{
  bool isApply = ((QPushButton*)sender() == buttonApply());
  if(!isApplyAndClose())
    erasePreview(false);

  // local selection
  if (!myShape->_is_nil() &&
      !myEditCurrentArgument &&
      myShapeIDs.size() > 1 ) // shape type is already choosen by user
  {
    GEOM_Displayer*   aDisplayer = getDisplayer();
    CORBA::String_var aMainEntry = myShape->GetStudyEntry();

    //display mode for main shape
    if ( myDmMode == -1 ) {
      SALOME_View* view = GEOM_Displayer::GetActiveView();
      if (view) {
        Handle(SALOME_InteractiveObject) io =
          new SALOME_InteractiveObject (aMainEntry.in(), "GEOM", "TEMP_IO");
        if ( view->isVisible( io ) ) {
          Handle(GEOM_AISShape) aSh = GEOMBase::ConvertIOinGEOMAISShape( io, true );
          if(!aSh.IsNull()) {
            myDmMode = aSh->isTopLevel() ? aSh->prevDisplayMode() : aSh->DisplayMode();
          }
          // Hide main shape, if explode on VERTEX
          if(getShapeType() != TopAbs_VERTEX) {
            aDisplayer->Erase(myShape, false, false);
            myIsHiddenMain = true;
          }
        }
        else
          myDmMode = SUIT_Session::session()->resourceMgr()->integerValue( "Geometry", "display_mode" );
      }
    }
    aDisplayer->SetDisplayMode(myDmMode);

    // Mantis issue 0021421: do not hide main shape, if explode on VERTEX
    if (getShapeType() == TopAbs_VERTEX) {
      if (myIsHiddenMain)
        aDisplayer->Display(myShape);
    }
    //aDisplayer->Erase(myField, false, false); ------- NOR YET implemented

    QColor aColor = SUIT_Session::session()->resourceMgr()->colorValue( "Geometry", "editField_color" );
    Quantity_NameOfColor aCol = SalomeApp_Tools::color( aColor ).Name();

    if(!isApplyAndClose()) {
      SUIT_ViewWindow* aViewWindow = 0;
      SUIT_Study* activeStudy = SUIT_Session::session()->activeApplication()->activeStudy();
      if (activeStudy)
        aViewWindow = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();
      if (aViewWindow == 0) return;

      SUIT_ViewManager* aViewManager = aViewWindow->getViewManager();
      if (aViewManager->getType() != OCCViewer_Viewer::Type() &&
          aViewManager->getType() != SVTK_Viewer::Type())
        return;

      SUIT_ViewModel* aViewModel = aViewManager->getViewModel();
      SALOME_View* aView = dynamic_cast<SALOME_View*>(aViewModel);
      if (aView == 0) return;

      TopoDS_Shape aMainShape = myShapeMap(1);
      for ( int i = 0; i < myShapeIDs.size(); ++i )
      {
        int index = myShapeIDs[ i ];
        TopoDS_Shape aSubShape = myShapeMap( index );
        QString anEntry = QString( "TEMP_" ) + aMainEntry.in() + QString("_%1").arg(index);
        Handle(SALOME_InteractiveObject) io =
          new SALOME_InteractiveObject(anEntry.toLatin1(), "GEOM", "TEMP_IO");
        aDisplayer->SetColor( aCol );
        SALOME_Prs* aPrs = aDisplayer->buildSubshapePresentation(aSubShape, anEntry, aView);
        if (aPrs) {
          displayPreview(aPrs, true, false); // append, do not update
          // TODO: map or delete Prs
        }
      }
      aDisplayer->UnsetDisplayMode();
      aDisplayer->UnsetColor();
      aDisplayer->UpdateViewer();
    }
  }

  globalSelection(GEOM_ALLSHAPES);

  SelectionIntoArgument();
}

//=================================================================================
// function : highlightSubShapes
// purpose  : select objects corresponding to selected table rows
//=================================================================================
void EntityGUI_FieldDlg::highlightSubShapes()
{
  if (CORBA::is_nil(myShape) || !myCurStepTable )
    return;

  TColStd_MapOfInteger anIds;
  QList<int> rows = myCurStepTable->selectedRows();
  int ii = 0, nn = rows.count();
  for (; ii < nn; ii++)
  {
    int shapeNum = rows[ ii ];
    if ( shapeNum < myShapeIDs.size() )
      anIds.Add( myShapeIDs[ shapeNum ]);
  }

  SalomeApp_Application* app = myGeomGUI->getApp();
  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  aSelMgr->clearSelected();
  if ( anIds.IsEmpty() )
    return;

  SUIT_ViewWindow* aViewWindow = 0;
  SUIT_Study* activeStudy = app->activeStudy();
  if (activeStudy)
    aViewWindow = app->desktop()->activeWindow();
  if (aViewWindow == 0) return;

  SUIT_ViewManager* aViewManager = aViewWindow->getViewManager();
  if (aViewManager->getType() != OCCViewer_Viewer::Type() &&
      aViewManager->getType() != SVTK_Viewer::Type())
    return;

  SUIT_ViewModel* aViewModel = aViewManager->getViewModel();
  SALOME_View* aView = dynamic_cast<SALOME_View*>(aViewModel);
  if (aView == 0) return;

  // TODO??: use here GEOMBase_Helper::myPreview instead of ic->DisplayedObjects()

  OCCViewer_Viewer* v3d = ((OCCViewer_ViewManager*)aViewManager)->getOCCViewer();
  Handle(AIS_InteractiveContext) ic = v3d->getAISContext();
  AIS_ListOfInteractive List;
  //ic->DisplayedObjects(List);
  ic->ObjectsInside(List); // Mantis issue 0021367

  SALOME_ListIO aSelList;

  // To highlight the selected sub-shape in Object Browser, if it's already published under the main shape
  GEOM::GEOM_ILocalOperations_var aLocOp = getGeomEngine()->GetILocalOperations(getStudyId());
  QMap<int, QString> childsMap;
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>(app->activeStudy());
  if (appStudy) {
    _PTR(Study) aStudy = appStudy->studyDS();
    CORBA::String_var aMainEntry = myShape->GetStudyEntry();
    _PTR(SObject) aSObj = aStudy->FindObjectID( aMainEntry.in() );
    _PTR(ChildIterator) anIt = aStudy->NewChildIterator(aSObj);
    for (anIt->InitEx(true); anIt->More(); anIt->Next()) {
      GEOM::GEOM_Object_var aChild =
        GEOM::GEOM_Object::_narrow(GeometryGUI::ClientSObjectToObject(anIt->Value()));
      if (!CORBA::is_nil(aChild)) {
        int index = aLocOp->GetSubShapeIndex(myShape, aChild);
        if ( anIds.Contains( index )) {
          CORBA::String_var aChildEntry = aChild->GetStudyEntry();
          //childsMap.insert(index, aChildEntry.in());
            Handle(SALOME_InteractiveObject) tmpIO =
              new SALOME_InteractiveObject( aChildEntry.in(), "GEOM", "TEMP_IO");
            aSelList.Append(tmpIO);
        }
      }
    }
  }

  AIS_ListIteratorOfListOfInteractive ite (List);
  for (; ite.More(); ite.Next()) {
    if (ite.Value()->IsInstance(STANDARD_TYPE(GEOM_AISShape))) {
      Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(ite.Value());
      if (aSh->hasIO()) {
        Handle(SALOME_InteractiveObject) anIO = aSh->getIO();
        QString anEntry = anIO->getEntry();
        int index = anEntry.lastIndexOf("_");
        anEntry.remove(0, index+1);
        int anIndex = anEntry.toInt();
        if (anIds.Contains(anIndex)) {
          aSelList.Append(anIO);
          // if (childsMap.contains (anIndex)) {
          //   Handle(SALOME_InteractiveObject) tmpIO = new SALOME_InteractiveObject(childsMap.value(anIndex).toLatin1().constData(), "GEOM", "TEMP_IO");
          //   aSelList.Append(tmpIO);
          // }
        }
      }
    }
  }
  aSelMgr->setSelectedObjects(aSelList);
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr EntityGUI_FieldDlg::createOperation()
{
  return getGeomEngine()->GetIFieldOperations(getStudyId());
}

#define RETURN_WITH_MSG(a, b) \
  if ((a)) { \
    theMessage += (b); \
    return false; \
  }

//=================================================================================
// function : isValid()
// purpose  : Verify validity of input data
//=================================================================================
bool EntityGUI_FieldDlg::isValid(QString& theMessage)
{
  SalomeApp_Study* study = getStudy();
  RETURN_WITH_MSG  (!study, tr("GEOM_NO_STUDY"))
  RETURN_WITH_MSG  (study->studyDS()->GetProperties()->IsLocked(), tr("GEOM_STUDY_LOCKED"))

  if ( myIsCreation ) {
    RETURN_WITH_MSG(CORBA::is_nil(myShape), tr("NO_SHAPE"))
  }
  else {
    RETURN_WITH_MSG(CORBA::is_nil(myShape), tr("NO_FIELD"))
  }

  QString aName = getNewObjectName().trimmed();
  RETURN_WITH_MSG  (aName.isEmpty(), tr("EMPTY_NAME"))

  RETURN_WITH_MSG  ( !myCurStepTable, tr("NO_VALUES"))
  return true;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool EntityGUI_FieldDlg::execute()
{
  SALOMEDS::Study_var aStudyDS = GeometryGUI::ClientStudyToStudy( getStudy()->studyDS() );
  SALOMEDS::StudyBuilder_var aBuilder = aStudyDS->NewBuilder();

  QString aName = getNewObjectName().trimmed();
  QStringList anEntryList;

  if ( myField->_is_nil() ) // create field
  {
    QStringList columnNames = myCurStepTable->getHeaders();
    int nbComps = columnNames.count() - 1;
    GEOM::string_array_var compNames = new GEOM::string_array();
    compNames->length( nbComps );
    for ( int iC = 0; iC < nbComps; ++iC )
      compNames[ iC ] = columnNames[ iC+1 ].toLatin1().constData();

    GEOM::GEOM_IFieldOperations_var anOper = GEOM::GEOM_IFieldOperations::_narrow(getOperation());
    myField = anOper->CreateField( myShape,
                                   aName.toLatin1().constData(),
                                   GEOM::field_data_type( getDataType() ),
                                   CORBA::Short( getDim() ),
                                   compNames );
    if ( myField->_is_nil() )
      return false;
    
    SALOMEDS::SObject_wrap aSO =
      getGeomEngine()->AddInStudy( aStudyDS, myField, aName.toLatin1().constData(), myShape );
    if ( !aSO->_is_nil() ) {
      myField->UnRegister();
      CORBA::String_var entry = aSO->GetID();
      anEntryList << entry.in();
    }
  }
  else // update field name
  {
    myField->SetName( aName.toLatin1().constData() );

    CORBA::String_var entry = myField->GetStudyEntry();
    if ( entry.in() ) {
      SALOMEDS::SObject_wrap SO = aStudyDS->FindObjectID( entry.in() );
      if ( !SO->_is_nil() ) {
        aBuilder->SetName(SO, aName.toLatin1().constData());
      }
    }
  }

  // create / update steps
  QMap< int, StepTable* >::iterator i_tbl = myStepTables.begin();
  for ( ; i_tbl != myStepTables.end(); ++i_tbl )
  {
    StepTable* tbl = i_tbl.value();
    QString stepName = (tr("STEP")+" %1 %2").arg( tbl->getStepID() ).arg( tbl->getStamp() );

    GEOM::GEOM_FieldStep_var step = tbl->getStep();
    if ( step->_is_nil() )
    {
      step = myField->GetStep( tbl->getStepID() );
      if ( step->_is_nil() )
      {
        step = myField->AddStep( tbl->getStepID(), tbl->getStamp() );

        SALOMEDS::SObject_wrap aSO =
          getGeomEngine()->AddInStudy( aStudyDS, step, stepName.toLatin1().constData(), myField );
        if ( /*!myIsCreation &&*/ !aSO->_is_nil() ) {
          step->UnRegister();
          CORBA::String_var entry = aSO->GetID();
          anEntryList << entry.in();
        }
      }
    }
    else if ( step->GetStamp() != tbl->getStamp() )
    {
      // update a stamp in the object browser
      CORBA::String_var entry = step->GetStudyEntry();
      if ( entry.in() ) {
        SALOMEDS::SObject_wrap SO = aStudyDS->FindObjectID( entry.in() );
        if ( !SO->_is_nil() )
          aBuilder->SetName( SO, stepName.toLatin1().constData() );
      }
    }

    tbl->setValues( step );

    // update the presentation if it is displayed
    CORBA::String_var aStepEntry = step->GetStudyEntry();
    Handle(SALOME_InteractiveObject) aStepIO =
      new SALOME_InteractiveObject( aStepEntry.in(), "GEOM", "TEMP_IO" );
    getDisplayer()->Redisplay( aStepIO, false, false );
  }
  getDisplayer()->UpdateViewer();

  // remove steps
  if ( !myIsCreation )
  {
    QSet< int >::iterator stepID = myRemovedSteps.begin();
    for ( ; stepID != myRemovedSteps.end(); ++stepID )
    {
      GEOM::GEOM_FieldStep_var step = myField->GetStep( *stepID );
      if ( !step->_is_nil() )
      {
        CORBA::String_var entry = step->GetStudyEntry();
        myField->RemoveStep( *stepID );

        if ( entry.in() ) {
          SALOMEDS::SObject_wrap SO = aStudyDS->FindObjectID( entry.in() );
          if ( !SO->_is_nil() )
            aBuilder->RemoveObjectWithChildren( SO );
        }
      }
    }
  }
  myRemovedSteps.clear();

  updateObjBrowser();

  if( SUIT_Application* anApp = SUIT_Session::session()->activeApplication() ) {
    LightApp_Application* aLightApp = dynamic_cast<LightApp_Application*>( anApp );
    if( aLightApp && !isDisableBrowsing() && anEntryList.count() )
    {
      aLightApp->browseObjects( anEntryList, isApplyAndClose(), isOptimizedBrowsing() );
      anApp->putInfo( QObject::tr("GEOM_PRP_DONE") );
    }
  }

  return true;
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> EntityGUI_FieldDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  GEOM::GeomObjPtr aGeomObjPtr(myField->GetShape());
  res << aGeomObjPtr;
  return res;
}
