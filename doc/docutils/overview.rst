
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
General presentation of the GEOM python package
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

The GEOM python package essentially contains:

* The visualization of structural elements: a function to create
  geometrical 3D representations of mechanical models called
  "structural elements".

Note that these functions either encapsulate the python programming
interface of GEOM core (the CORBA or SWIG interfaces for example) or
extend existing utilities as the ``geompy.py`` module.

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
