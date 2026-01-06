// Copyright (C) 2007-2026  CEA, EDF, OPEN CASCADE
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

#include <vector>
#include <cmath>
#include <random>

namespace
{
    struct ColorRGB
    {
        double r{0.0};
        double g{0.0};
        double b{0.0};
    };

    ColorRGB fromHsv(const int h, const int s, const int v)
    {
        const double hh = h / 60.0;
        const double ss = s / 255.0;
        const double vv = v / 255.0;
    
        const int i = static_cast<int>(std::floor(hh)) % 6;
        const double f = hh - std::floor(hh);
        const double p = vv * (1.0 - ss);
        const double q = vv * (1.0 - f * ss);
        const double t = vv * (1.0 - (1.0 - f) * ss);
    
        switch (i)
        {
            case 0: return { vv, t, p };
            case 1: return { q, vv, p };
            case 2: return { p, vv, t };
            case 3: return { p, q, vv };
            case 4: return { t, p, vv };
            case 5: return { vv, p, q };
            default: return { 0.0, 0.0, 0.0 };
        }
    }
}

SALOMEDS::Color GEOM_ColorUtils::getPredefinedUniqueColor()
{
    static const std::vector<ColorRGB> colors = []()
    {
        std::vector<ColorRGB> tempColors;
        for (int s = 0; s < 2; s++)
        {
            for (int v = 100; v >= 40; v -= 20)
            {
                for (int h = 0; h < 359; h += 60)
                {
                    tempColors.push_back(fromHsv(h, 255 - s * 127, v * 255 / 100));
                }
            }
        }
        return tempColors;
    }();

    static int currentColor = randomize(colors.size());

    const SALOMEDS::Color color
    {
        colors[currentColor].r,
        colors[currentColor].g,
        colors[currentColor].b
    };

    currentColor = (currentColor + 1) % colors.size();

    return color;
}

int GEOM_ColorUtils::randomize(int size)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, size - 1);
    return dis(gen);
}