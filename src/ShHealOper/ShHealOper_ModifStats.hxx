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

// File      : ShHealOper_ModifStats.hxx
// Created   : Mon Dec  1 15:29:48 2014

#ifndef __ShHealOper_ModifStats_HXX__
#define __ShHealOper_ModifStats_HXX__

#include <string>
#include <set>

/*!
 * \brief Structure describing modifications done in a shape
 */
class Standard_EXPORT ShHealOper_ModifStats
{
 public:

  struct Standard_EXPORT Datum
  {
    std::string myModif; // what is done
    mutable int myCount; // how many times (in how many sub-shapes)

    Datum( const std::string& txt, int cnt=0 ): myModif( txt ), myCount(cnt) {}
    bool operator< ( const Datum& o ) const { return myModif < o.myModif; }
  };

  // record a modification
  void AddModif( const std::string& modifText, int nb=1 )
  {
    std::set< Datum >::iterator d = myData.insert( Datum( modifText )).first;
    d->myCount += nb;
  }

  // add data from another ShHealOper_ModifStats
  void Add( const ShHealOper_ModifStats& stats )
  {
    if ( myData.empty() ) myData = stats.myData;
    else {
      std::set< Datum >::const_iterator d = stats.myData.begin();
      for ( ; d != stats.myData.end(); ++d )
        AddModif( d->myModif, d->myCount );
    }
  }

  // return all recorder modifications
  const std::set< Datum >& GetData() const { return myData; }

  // clear all statistics
  void Clear() { myData.clear(); }

 protected:

  std::set< Datum > myData;

};

#endif
