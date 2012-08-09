
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
General presentation of the GEOM python package
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

The GEOM python package essentially contains:

* Utility functions to handle GEOM items in Salome study (see ``geomtools.py``).

  - add a shape to a study, remove from the study
  - display a shape in a viewer, erase the shape from the viewers
  - completly delete a shape (undisplay, unpublish, and destroy the shape)
  - helper functions to manage the selection in the object browser
* Wrapping functions to help the usage of GEOM tools:

  - a sketcher module to create sketches from the python API (see
    ``sketcher.py``)
* The visualization of structural elements: a function to create
  geometrical 3D representations of mechanical models called
  "structural elements" (see package ``structelem``)

The functions are distributed in the python package
``salome.geom``. The specification of the programming interface of
this package is detailled in the part :doc:`Documentation of the
programming interface (API)</docapi>` of this documentation.

.. note::
   The main package ``salome`` contains other sub-packages that are
   distributed with the other SALOME modules. For example, the KERNEL
   module provides the python package ``salome.kernel`` and SMESH the
   package ``salome.smesh``.

Visualization of structural elements (package ``structelem``)
-------------------------------------------------------------

The usage of the visualization of structural elements can be
appreciated with this set of instructions:

.. code-block:: python

   from salome.geom.structelem import TEST_StructuralElement
   TEST_StructuralElement()

This creates the geometrical objects displayed in the study below:

.. image:: /images/salome-geom-structuralelements.png
   :align: center

Manipulate GEOM object in the study (module ``geomtools``)
----------------------------------------------------------

The usage of the ``geomtools`` module can be appreciated with this set
of instructions:

.. code-block:: python

   from salome.geom.geomtools import TEST_createAndDeleteShape
   TEST_createAndDeleteShape()

This test executes the following procedure:

* Create, publish, and display a cylinder
* Create, publish, and display a sphere
* Create a box, publish it in a folder "boxset", and display it with a
  "pink" color.
* Erase the sphere from the viewer (the sphere still exists in the study)
* Delete the cylinder (the cylinder is no longer displayed and does
  not exist any more, neither in the study nor the GEOM componet.

At the end of the execution of this test, you should have in the
SALOME session:

* the box, in a dedicated folder of the study, and displayed in the
  viewer
* the sphere, in the standard place of the study, and not displayed
