// Copyright (C) 2013-2024  CEA, EDF, OPEN CASCADE
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
// Author : Frederic Pons (OpenCascade)

#ifndef __XAO_DOUBLESTEP_HXX__
#define __XAO_DOUBLESTEP_HXX__

#include <vector>

#include "XAO.hxx"
#include "XAO_XaoUtils.hxx"
#include "XAO_Step.hxx"

#ifdef WIN32
#pragma warning(disable:4290) // Warning Exception ...
#endif


namespace XAO
{
    /**
     * @class DoubleStep
     * Step with double values.
     */
    class XAO_EXPORT DoubleStep : public Step
    {
    public:
        /**
         * Constructor.
         * @param step the step number.
         * @param stamp the stamp of the step.
         * @param nbElements the number elements of the geometry.
         * @param nbComponents the number of components of the field.
         */
        DoubleStep(int step, int stamp, int nbElements, int nbComponents);

        virtual XAO::Type getType() { return XAO::DOUBLE; }

        /**
         * Gets all the values of the step as a list.
         * @return a vector containing all the values of the step.
         */
        std::vector<double> getValues();

        /**
         * Gets all the values for a given element.
         * @param element the index of the element.
         * @return a vector containing all the values for the given element.
         */
        std::vector<double> getElement(int element) ;

        /**
         * Gets all the values for a given component.
         * @param component the index of the component.
         * @return a vector containing all the values for the given component.
         */
        std::vector<double> getComponent(int component) ;

        /**
         * Gets the value for an element and a component.
         * @param element the index of the element.
         * @param component the index of the component.
         * @return the value for the given element and component.
         */
        double getValue(int element, int component) ;

        /**
         * Sets all the values from a list.
         * @param values the list of values to set.
         */
        void setValues(const std::vector<double>& values) ;

        /**
         * Sets the values for an element.
         * @param element the index of the element to set.
         * @param elements the values to set.
         */
        void setElement(int element, const std::vector<double>& elements) ;

        /**
         * Sets the values for a component.
         * @param component the index of the component to set.
         * @param components the values to set.
         */
        void setComponent(int component, const std::vector<double>& components) ;

        /**
         * Sets the value for an element and a component.
         * @param element the index of the element.
         * @param component the index of the component.
         * @param value the value.
         */
        void setValue(int element, int component, double value) ;

        virtual const std::string getStringValue(int element, int component) ;
        virtual void setStringValue(int element, int component, const std::string& value) ;

    private:
        std::vector< std::vector<double> > m_values;
    };
}


#endif /* __XAO_DOUBLESTEP_HXX__ */
