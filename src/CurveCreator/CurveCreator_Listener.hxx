// Copyright (C) 2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#ifndef CURVE_CREATOR_LISTENER_HXX
#define CURVE_CREATOR_LISTENER_HXX

class CurveCreator_Listener
{
public:
  CurveCreator_Listener(void){};
  virtual ~CurveCreator_Listener(void){};

  virtual void pointChanged( int theSection, int thePoint ){}
  virtual void pointRemoved( int theSection, int theFirstPoint, int thePointCnt ){}
  virtual void pointInserted( int theSection, int theIndx ){}

  virtual void sectionClosed( int theSection, bool isClosed ){}
  virtual void sectionAdded( int theSection ){}
  virtual void sectionRemoved( int theSection ){}
  virtual void sectionTypeChanged( int theSection ){}

  virtual void curveChanged(){}
};

#endif