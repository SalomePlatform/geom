# Copyright (C) 2013-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

# Python API for field on geometry manipulations:

# field          = geompy.CreateField(shape, name, type, dimension, componentNames)
# geompy.RemoveField(field)
# shape          = field.getShape()
# name           = field.getName()
# field.setName(name)
# typ            = field.getType()
# dim            = field.getDimension()
# componentNames = field.getComponents()
# n              = geompy.CountFields(shape)
# fields         = geompy.GetFields(shape)
# field          = geompy.GetField(shape, name)
#
# field.addStep(step, stamp, values)
# field.removeStep(step)
# n              = field.countSteps()
# steps          = field.getSteps() # donne la liste des numeros de pas du champ
# stamp          = field.getStamp(step)
# field.setStamp(step, stamp)
# values         = field.getValues(step)
# field.setValues(step, values)

MustFail = -1
geompy = None

def CheckFieldCreation(shape, name, ftype, dimension, componentNames, nbFiOrMustFail=-1):
    # WARNING: assure name uniquness to check geompy.GetField( shape, name )
    try:
        field = geompy.CreateField(shape, name, ftype, dimension, componentNames)
    except Exception, e:
        if nbFiOrMustFail == MustFail:
            print "Ok, expected exception caught: %s"%e
            return
        raise e
    if nbFiOrMustFail == MustFail:
        raise RuntimeError, "Expected exception NOT thrown"
    assert field.getShape()
    assert field.getShape().IsSame( shape )
    assert field.getName() == name
    if isinstance( ftype, int ):
        assert field.getType() == ftype
    else:
        assert field.getTypeEnum() == ftype
    assert field.getDimension() == dimension
    assert field.getComponents() == componentNames
    assert geompy.CountFields( shape ) == nbFiOrMustFail
    assert geompy.GetField( shape, name )
    assert geompy.GetField( shape, name ).IsSame( field )

def CheckStepManips(field, step, stamp, values, nbStepsOrMustFail, toRemove=False):
    try:
        stp = field.addStep(step, stamp, values)
    except Exception, e:
        if nbStepsOrMustFail == MustFail:
            print "Ok, expected exception caught: %s"%e
            return
        raise e
    if nbStepsOrMustFail == MustFail:
        raise RuntimeError, "Expected exception NOT thrown"
    assert field.countSteps() == nbStepsOrMustFail
    assert len( field.getSteps() ) == nbStepsOrMustFail
    assert step in field.getSteps()
    assert field.getStamp(step) == stamp
    field.setStamp(step, stamp*2)
    assert field.getStamp(step) == stamp*2
    assert field.getValues(step) == values
    values[-1] = values[0]
    field.setValues(step, values)
    assert field.getValues(step) == values
    if toRemove:
        field.removeStep(step)
        assert field.countSteps() == nbStepsOrMustFail-1
        assert len( field.getSteps() ) == nbStepsOrMustFail-1
        assert step not in field.getSteps()
        pass
    return

def TestField (geomBuilder, math):

    global geompy
    geompy = geomBuilder

    oldAutoPublish = geompy.myMaxNbSubShapesAllowed
    geompy.addToStudyAuto()

    shape = geompy.MakeBoxDXDYDZ(1,1,1)

    # Field operations

    import GEOM
    CheckFieldCreation(shape,"fBool1",GEOM.FDT_Bool, 0, ['b1'], 1)
    CheckFieldCreation(shape,"fInt1", GEOM.FDT_Int,  1, ['i1','i2'], 2)
    CheckFieldCreation(shape,"fDbl1", 2,  3, ['d3'], 3)
    CheckFieldCreation(shape,"fStr1", 3,  -1, ['s4'], 4)

    # assert exception in case of invalid parameters
    CheckFieldCreation(None,"bad",GEOM.FDT_Bool, 0, ['b1'], MustFail)   # no shape
    CheckFieldCreation(shape,"",GEOM.FDT_Bool, 0, ['b1'], MustFail)     # no name
    CheckFieldCreation(shape,"bad",-1, ['b1'], MustFail)                # too low data type
    CheckFieldCreation(shape,"bad",4, ['b1'], MustFail)                 # too high data type
    CheckFieldCreation(shape,"bad",GEOM.FDT_Bool, -2, ['b1'], MustFail) # too low dim
    CheckFieldCreation(shape,"bad",GEOM.FDT_Bool, 4, ['b1'], MustFail)  # too high dim
    CheckFieldCreation(shape,"bad",GEOM.FDT_Bool, 0, [], MustFail)      # no components

    # Field step operations


    toRemove = True

    # string field on the whole shape
    sfield = geompy.CreateField(shape, "strWhole", GEOM.FDT_String, -1, ["day","year"])
    CheckStepManips( sfield, -1, -10, ["25 Sep","2013"], 1 )
    CheckStepManips( sfield,  2,  10, ["26 Sep","2014"], 2, toRemove )
    CheckStepManips( sfield, 10,1000, ["27 Sep","2015"], 2 )
    # bool field on 8 vertices
    bfield = geompy.CreateField(shape, "boolV", GEOM.FDT_Bool, 0, ["ok"])
    CheckStepManips( bfield, 1, -1, [0,1]*4, 1, toRemove )
    CheckStepManips( bfield, 2, -2, [1,0]*4, 1 )
    # int field on 6 faces
    ifield = geompy.CreateField(shape, "intF", GEOM.FDT_Int, 2, ["id","domain"])
    CheckStepManips( ifield, -1, -10, range(12),  1 )
    CheckStepManips( ifield, -2, -20, range(6)*2, 2 )
    # double field on a solid
    dfield = geompy.CreateField(shape, "dblS", GEOM.FDT_Double, 3, ["a","b","c"])
    CheckStepManips( dfield, -1, -10, [-1.1, 2.3, 4000], 1 )
    CheckStepManips( dfield, -2, -20, range(3), 2 )

    # assert exception in case of invalid parameters
    CheckStepManips( sfield, -1, -10, ["25 Sep","2013"], MustFail ) # step already exists
    CheckStepManips( sfield, 20, -10, ["25 Sep"], MustFail ) # wrong data size
    CheckStepManips( sfield, 30, -10, [1, 2], MustFail ) # wrong data type

    geompy.addToStudyAuto(oldAutoPublish)

    # Check Python dump
    import tempfile, os
    dumpFile = tempfile.NamedTemporaryFile().name
    pyFile = dumpFile+".py"
    # dump the study
    import salome
    assert( salome.myStudy.DumpStudy(os.path.dirname(dumpFile), os.path.basename(dumpFile), 1, 0))
    execfile( pyFile )
    os.remove( pyFile )

    print "Field manipulations work OK"
