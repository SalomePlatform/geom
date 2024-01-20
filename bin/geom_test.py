#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2018-2024  CEA, EDF, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

import unittest

class TestGeometry(unittest.TestCase):

    def setUp(self):
        import salome
        salome.salome_init()

    def processGuiEvents(self):
        import salome
        if salome.sg.hasDesktop():
            salome.sg.updateObjBrowser();
            import SalomePyQt
            SalomePyQt.SalomePyQt().processEvents()

    def test_geometry(self):
        """Quick test for Geometry module"""

        print()
        print('Testing Geometry module')

        from salome.geom import geomBuilder
        geompy = geomBuilder.New()

        # ---- create box
        print('... Create box')
        box = geompy.MakeBox(0., 0., 0., 100., 200., 300, theName='box')
        self.assertIsNotNone(box)
        self.assertEqual(box.GetName(), 'box')
        self.processGuiEvents()

        # ---- extract shell from box
        print('... Extract shell from box')
        shells = geompy.SubShapeAll(box, geompy.ShapeType['SHELL'], theName=['shell'])
        self.assertEqual(len(shells), 1)
        shell = shells[0]
        self.assertIsNotNone(shell)
        self.assertEqual(shell.GetName(), 'shell')
        self.processGuiEvents()

        # ---- extract faces from box
        print('... Extract faces from box')
        faces = geompy.SubShapeAll(box, geompy.ShapeType['FACE'], theName='face')
        self.assertEqual(len(faces), 6)
        for i, face in enumerate(faces):
            self.assertIsNotNone(face)
            self.assertEqual(face.GetName(), 'face_{}'.format(i+1))
        self.processGuiEvents()

        # ---- extract edges from 1st face
        print('... Extract edges from 1st face')
        edges = geompy.SubShapeAll(faces[0], geompy.ShapeType['EDGE'], theName='edge')
        self.assertEqual(len(edges), 4)
        for i, edge in enumerate(edges):
            self.assertIsNotNone(edge)
            self.assertEqual(edge.GetName(), 'edge_{}'.format(i+1))
        self.processGuiEvents()

if __name__ == '__main__':
    unittest.main()
