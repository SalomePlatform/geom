# Copyright (C) 2012-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

# ===============================================================
# Files to be installed
# ===============================================================

# These files are data, module or lib files
SET( _res_files
  AdvancedGEOM.xml
  STLPlugin.xml
  BREPPlugin.xml
  STEPPlugin.xml
  IGESPlugin.xml
  XAOPlugin.xml
  GEOM_en.xml
  GEOM_fr.xml
  GEOM.config
  GEOMDS_Resources
  ShHealing
  Y14.5M-2009.ttf
  3dsketch.png
  isoline.png
  isoline_v.png
  angle.png
  arc.png
  arccenter.png
  arcofellipse.png
  archimede.png
  axisinertia.png
  basicproperties.png
  bounding.png
  box.png
  box2points.png
  boxdxyz.png
  build_compound.png
  build_edge.png
  build_edge_wire.png
  build_edge_curve.png
  build_face.png
  build_face_surface.png
  build_face_constraints.png
  build_shell.png
  build_solid.png
  build_wire.png
  centergravity.png
  chamfer.png
  chamferall.png
  chamferedge.png
  chamferface.png
  chamferedgefromface.png
  change_orientation.png
  change_direction.png
  check.png
  circle.png
  circlepointvector.png
  circle3points.png
  circlecenter2pnts.png
  common.png
  cone.png
  conedxyz.png
  conepointvector.png
  cut.png
  cylinder.png
  cylinderdxyz.png
  cylinderpointvector.png
  delete.png
  divided_disk.png
  disk.png
  disk3points.png
  disk_pntvecr.png
  disk_r.png
  display.png
  displayonly.png
  displayall.png
  draft.png
  edgeextension.png
  erase.png
  eraseall.png
  extract.png
  extruded_boss.png
  extruded_cut.png
  facetosurface.png
  faceextension.png
  face_hw.png
  face_vechw.png
  feature_detect.png
  fillet.png
  fillet1d.png
  fillet2d.png
  filletall.png
  filletedge.png
  filletwire.png
  filletface.png
  filling.png
  folder.png
  fuse.png
  fuse_collinear_edges.png
  geometry.png
  import_picture.png
  import.png
  limit_tolerance.png
  line.png
  line2points.png
  line2faces.png
  managedimensions.png
  mindist.png
  mirrorPoint.png
  mirrorAxe.png
  mirrorPlane.png
  ModuleGeom.png
  multirotation.png
  multirotationdouble.png
  multirotationsimple.png
  multitranslation.png
  multitranslationdouble.png
  multitranslationsimple.png
  normale.png
  offset.png
  projection.png
  projection_on_edge.png
  projection_on_wire.png
  proj_on_cyl.png
  origin_and_vectors.png
  partition.png
  partitionkeep.png
  partitionplane.png
  pipe.png
  pipebinormal.png
  pipesections.png
  plane.png
  planeWorking.png
  plane3points.png
  planeface.png
  planepointvector.png
  planeworkingface.png
  planeworkingvector.png
  planeworkingorigin.png
  point2.png
  pointonedge.png
  pointonedgebypoint.png
  pointonface.png
  point_line_intersection.png
  polyline.png
  prism.png
  prism2.png
  prism3.png
  rectangle.png
  revol.png
  rotate.png
  rotatepnt.png
  scale.png
  scale_along_axes.png
  section.png
  select1.png
  sewing.png
  shading.png
  shading_with_edges.png
  shapesonshape.png
  shared_shapes.png
  sketch.png
  solid_from_faces.png
  sphere.png
  spheredxyz.png
  spherepoint.png
  subshape.png
  supressHolesOnFaceShell.png
  supressface.png
  supresshole.png
  thickness.png
  thickness2.png
  tolerance.png
  torus.png
  torusdxyz.png
  toruspointvector.png
  transfer_data.png
  translationDxyz.png
  translationPoints.png
  translationVector.png
  tree_compound.png
  tree_compsolid.png
  tree_edge.png
  tree_face.png
  tree_shape.png
  tree_shell.png
  tree_solid.png
  tree_vertex.png
  tree_wire.png
  tree_group_vertex.png
  tree_group_edge.png
  tree_group_face.png
  tree_group_solid.png
  tree_field_vertex.png
  tree_field_edge.png
  tree_field_face.png
  tree_field_solid.png
  tree_lcs.png
  vector.png
  vector_mode.png
  vector2points.png
  vectordxyz.png
  wireframe.png
  whatis.png
  ellipse.png
  ellipsepointvector.png
  bezier.png
  interpol.png
  spline.png
  closecontour.png
  suppressintwires.png
  marker.png
  marker2.png
  marker3.png
  marker_1.png
  marker_2.png
  marker_3.png
  marker_4.png
  marker_5.png
  marker_6.png
  marker_7.png
  marker_8.png
  marker_9.png
  marker_10.png
  marker_11.png
  marker_12.png
  marker_13.png
  position.png
  position2.png
  position3.png
  free_bound.png
  point_coord.png
  point3.png
  shapeprocess.png
  block_2f.png
  block_6f.png
  block_face_2e.png
  block_face_4e.png
  block_face_4v.png
  block_multitrsf_double.png
  block_multitrsf_simple.png
  tree_block.png
  subblock.png
  group_new.png
  group_edit.png
  field_new.png
  field_edit.png
  glue.png
  check_blocks_compound.png
  get_non_blocks.png
  check_self_intersections.png
  fast_intersect.png
  free_faces.png
  propagate.png
  redo.png
  undo.png
  glue2.png
  remove_webs.png
  remove_extra_edges.png
  union_faces.png
  coordsys.png
  translation.png
  arrow_down.png
  arrow_up.png
  closedpolyline.png
  closedspline.png
  edit_points.png
  join2.png
  bringTogether.png
  new_point.png
  new_section.png
  # ADVANCED_RESOURCES:
  pipetshape.png tree_pipetshape.png pipetshape_import_icon.png pipetshape_section.png
  dlg_pipetshape.png dlg_pipetshapechamfer.png dlg_pipetshapefillet.png
  dlg_pipetshapel1.png dlg_pipetshaper1.png dlg_pipetshapew1.png
  dlg_pipetshapel2.png dlg_pipetshaper2.png dlg_pipetshapew2.png
  dlg_pipetshapechamferl1.png dlg_pipetshapechamferr1.png dlg_pipetshapechamferw1.png
  dlg_pipetshapechamferl2.png dlg_pipetshapechamferr2.png dlg_pipetshapechamferw2.png
  dlg_pipetshapechamferh.png dlg_pipetshapechamferw.png
  dlg_pipetshapefilletl1.png dlg_pipetshapefilletr1.png dlg_pipetshapefilletw1.png
  dlg_pipetshapefilletl2.png dlg_pipetshapefilletr2.png dlg_pipetshapefilletw2.png
  dlg_pipetshapefilletrf.png
  dividedcylinder.png dividedcylinder_r_h.png
  smoothingsurface.png smoothingsurface_lpoints.png tree_smoothingsurface.png
  exportxao.png importxao.png
  ##@@ insert new functions before this line @@ do not remove this line @@##
  )
IF(SALOME_GEOM_USE_VTK)
  SET(_res_files
    VTKPlugin.xml
    ${_res_files}
  )
ENDIF(SALOME_GEOM_USE_VTK)

INSTALL(FILES ${_res_files} DESTINATION ${SALOME_GEOM_INSTALL_RES_DATA})

SET( _texture_files
  texture1.dat
  texture2.dat
  texture3.dat
)
INSTALL(FILES ${_texture_files} DESTINATION ${SALOME_GEOM_INSTALL_RES_DATA}/textures)

SALOME_CONFIGURE_FILE(GEOMCatalog.xml.in GEOMCatalog.xml INSTALL ${SALOME_GEOM_INSTALL_RES_DATA})
SALOME_CONFIGURE_FILE(SalomeApp.xml.in SalomeApp.xml INSTALL ${SALOME_GEOM_INSTALL_RES_DATA})
SALOME_CONFIGURE_FILE(Plugin.in Plugin INSTALL ${SALOME_GEOM_INSTALL_RES_DATA})




