# Import/Export

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

import tempfile, os

# create a sphere
sphere = geompy.MakeSphereR(100)

tmpdir = tempfile.mkdtemp()

# export sphere to the BREP file
f_brep = os.path.join(tmpdir, "sphere.brep")
geompy.ExportBREP(sphere, f_brep)

# export sphere to the IGES v5.3 file
f_iges = os.path.join(tmpdir, "sphere.iges")
geompy.ExportIGES(sphere, f_iges, "5.3")

# export sphere to the STEP file, using millimeters as length units
f_step = os.path.join(tmpdir, "sphere.step")
geompy.ExportSTEP(sphere, f_step, GEOM.LU_MILLIMETER)

# export sphere to the binary STL file, with default deflection coefficient
f_stl1 = os.path.join(tmpdir, "sphere1.stl")
geompy.ExportSTL(sphere, f_stl1, False)

# export sphere to the ASCII STL file, with custom deflection coefficient
f_stl2 = os.path.join(tmpdir, "sphere2.stl")
geompy.ExportSTL(sphere, f_stl2, True, 0.1)

# export sphere to the VTK file, with default deflection coefficient
f_vtk1 = os.path.join(tmpdir, "sphere1.vtk")
geompy.ExportVTK(sphere, f_vtk1)

# export sphere to the VTK file, with custom deflection coefficient
f_vtk2 = os.path.join(tmpdir, "sphere2.vtk")
geompy.ExportVTK(sphere, f_vtk2, 0.1)

# export sphere to the XAO file
f_xao = os.path.join(tmpdir, "sphere.xao")
geompy.ExportXAO(sphere, [], [], "author", f_xao)

# import BREP file
sphere_brep = geompy.ImportBREP(f_brep)

# import IGES file
sphere_iges = geompy.ImportIGES(f_iges)

# import STEP file, taking units into account
sphere_step1 = geompy.ImportSTEP(f_step)

# import STEP file, ignoring units (result is scaled)
sphere_step2 = geompy.ImportSTEP(f_step, True)

# import STL files
sphere_stl1 = geompy.ImportSTL(f_stl1)
sphere_stl2 = geompy.ImportSTL(f_stl2)

# import XAO file
ok, sphere_xao, sub_shapes, groups, fields = geompy.ImportXAO(f_xao)

# clean up
for f in f_brep, f_iges, f_step, f_stl1, f_stl2, f_vtk1, f_vtk2, f_xao:
  os.remove(f)
os.rmdir(tmpdir)
