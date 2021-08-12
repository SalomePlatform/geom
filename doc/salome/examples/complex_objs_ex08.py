# Creation of a PipeBiNormalAlongVector
import salome
salome.salome_init_without_session()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New()

def MakeSpring(radius, height, rotation, direction, thread_radius, base_rotation, geom_builder):

    def MakeHelix(radius, height, rotation, direction, geom_builder):
        #  - create a helix -
        radius = 1.0 * radius
        height = 1.0 * height
        rotation = 1.0 * rotation
        if direction > 0:
            direction = +1
        else:
            direction = -1
            pass
        from math import sqrt
        length_z  = height
        length_xy = radius*rotation
        length = sqrt(length_z*length_z + length_xy*length_xy)
        nb_steps = 1
        epsilon = 1.0e-6
        while 1:
            z_step = height / nb_steps
            angle_step = rotation / nb_steps
            z = 0.0
            angle = 0.0
            helix_points = []
            for n in range(nb_steps+1):
                from math import cos, sin
                x = radius * cos(angle)
                y = radius * sin(angle)
                p = geom_builder.MakeVertex(x, y, z)
                helix_points.append( p )
                z += z_step
                angle += direction * angle_step
                pass
            helix = geom_builder.MakeInterpol(helix_points)
            length_test = geom_builder.BasicProperties(helix)[0]
            prec = abs(length-length_test)/length
            # print nb_steps, length_test, prec
            if prec < epsilon:
                break
            nb_steps *= 2
            pass
        return helix

    #  - create a pipe -
    thread_radius = 1.0 * thread_radius
    # create a helix
    helix = MakeHelix(radius, height, rotation, direction, geom_builder=geom_builder)
    # base in the (Ox, Oz) plane
    p0 = geom_builder.MakeVertex(radius-3*thread_radius, 0.0, -thread_radius)
    p1 = geom_builder.MakeVertex(radius+3*thread_radius, 0.0, -thread_radius)
    p2 = geom_builder.MakeVertex(radius+3*thread_radius, 0.0, +thread_radius)
    p3 = geom_builder.MakeVertex(radius-3*thread_radius, 0.0, +thread_radius)
    e0 = geom_builder.MakeEdge(p0, p1)
    e1 = geom_builder.MakeEdge(p1, p2)
    e2 = geom_builder.MakeEdge(p2, p3)
    e3 = geom_builder.MakeEdge(p3, p0)
    w = geom_builder.MakeWire([e0, e1, e2, e3])
    # create a base face
    base = geom_builder.MakeFace(w, True)
    # create a binormal vector
    binormal = geom_builder.MakeVectorDXDYDZ(0.0, 0.0, 10.0)
    # create a pipe
    spring = geom_builder.MakePipeBiNormalAlongVector(base, helix, binormal)
    # Publish in the study
    geom_builder.addToStudy(base, "base")
    geom_builder.addToStudy(helix, "helix")
    geom_builder.addToStudy(binormal, "binormal")
    geom_builder.addToStudy(spring, "spring")
    return spring

from math import pi
spring = MakeSpring(50, 100, 2*pi, 1, 5, pi/2, geom_builder=geompy)
