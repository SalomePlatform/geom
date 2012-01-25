
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
General presentation of the GEOM python package
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

The GEOM python package essentially contains:

* Utility functions to handle GEOM items in Salome study.
* Wrapping functions to help the usage of existing tools:

  - a sketcher module to create sketches from the python API   
* The visualization of structural elements: a function to create
  geometrical 3D representations of mechanical models called
  "structural elements".

The functions are distributed in the python package
``salome.geom``. For example, the usage of the visualization of
structural elements can be appreciated with this set of instructions:

.. code-block:: python

   from salome.geom.structelem import TEST_StructuralElement
   TEST_StructuralElement()

This creates the geometrical objects displayed in the study below:

.. image:: /images/salome-geom-structuralelements.png
   :align: center

The specification of the programming interface of this package is
detailled in the part :doc:`Documentation of the programming interface
(API)</docapi>` of this documentation.

.. note::
   The main package ``salome`` contains other sub-packages that are
   distributed with the other SALOME modules. For example, the KERNEL
   module provides the python package ``salome.kernel`` and SMESH the
   package ``salome.smesh``.
