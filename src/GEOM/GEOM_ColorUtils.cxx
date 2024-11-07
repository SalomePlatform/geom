// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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

#include "GEOM_ColorUtils.hxx"

#include <QRandomGenerator>
#include <QColor>


SALOMEDS::Color GEOM_ColorUtils::getPredefinedUniqueColor()
{
    static QList<QColor> colors = []() {
        QList<QColor> tempColors;
        for (int s = 0; s < 2; s++)
        {
            for (int v = 100; v >= 40; v = v - 20)
            {
                for (int h = 0; h < 359; h = h + 60)
                {
                    tempColors.append(QColor::fromHsv(h, 255 - s * 127, v * 255 / 100));
                }
            }
        }
        return tempColors;
    }();

    static int currentColor = randomize(colors.size());

    SALOMEDS::Color color;
    color.R = (double)colors[currentColor].red()   / 255.0;
    color.G = (double)colors[currentColor].green() / 255.0;
    color.B = (double)colors[currentColor].blue()  / 255.0;

    currentColor = (currentColor+1) % colors.count();

    return color;
}

int GEOM_ColorUtils::randomize(int size)
{
    return QRandomGenerator::global()->bounded(size);
}