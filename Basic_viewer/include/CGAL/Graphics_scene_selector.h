// Copyright (c) 2022 GeometryFactory Sarl (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
//
// $URL$
// $Id$
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-Commercial
//
// Author(s):   Guillaume Damiand <guillaume.damiand@liris.cnrs.fr>
//              Timothée Lafon de La Laurencie <timotheelafondll@gmail.com>

#ifndef CGAL_GRAPHICS_SCENE_SELECTOR_H
#define CGAL_GRAPHICS_SCENE_SELECTOR_H

#include <CGAL/license/GraphicsView.h>
#include <CGAL/IO/Color.h>
#include <functional>

namespace CGAL {

template <typename DS,
          typename VertexDescriptor,
          typename EdgeDescriptor,
          typename FaceDescriptor,
          typename VolumeDescriptor=void>
struct Graphics_scene_selector;

// Drawing functor for a 2D combinatorial data structure
// (with vertices, edges and faces)
template <typename DS,
          typename VertexDescriptor,
          typename EdgeDescriptor,
          typename FaceDescriptor>
struct Graphics_scene_selector<DS, VertexDescriptor, EdgeDescriptor, FaceDescriptor, void>
{
  typedef DS data_structure;
  typedef VertexDescriptor vertex_descriptor;
  typedef EdgeDescriptor edge_descriptor;
  typedef FaceDescriptor face_descriptor;

  Graphics_scene_selector() {}

  template<class GSS, class descriptor>
  friend struct GSS_push;

  vertex_descriptor get_vertex_descriptor(std::size_t index) const
  {
    if (index >= m_vertices_index.size())
    {
      return vertex_descriptor(); // Return an invalid descriptor
    }
    return m_vertices_index.at(index);
  }

  edge_descriptor get_edge_descriptor(std::size_t index) const
  {
    if (index >= m_edges_index.size())
    {
      return edge_descriptor(); // Return an invalid descriptor
    }
    return m_edges_index.at(index);
  }

  face_descriptor get_face_descriptor(std::size_t index) const
  {
    if (index >= m_faces_index.size())
    {
      return face_descriptor(); // Return an invalid descriptor
    }
    return m_faces_index.at(index);
  }

  void clear()
  {
    m_vertices_index.clear();
    m_edges_index.clear();
    m_faces_index.clear();
  }

  protected:
  void add_vertex(vertex_descriptor vd)
  {
    m_vertices_index.push_back(vd);
  }

  void add_edge(edge_descriptor ed)
  {
    m_edges_index.push_back(ed);
  }

  void add_face(face_descriptor fd)
  {
    m_faces_index.push_back(fd);
  }

protected:
  std::vector<vertex_descriptor> m_vertices_index;
  std::vector<edge_descriptor> m_edges_index;
  std::vector<face_descriptor> m_faces_index;
};

template<class GSSelector, class descriptor>
struct GSS_push;

template<class descriptor>
struct GSS_push<void, descriptor>
{
  static void run(void*, const descriptor&) {}
};

template<class GSSelector, class descriptor>
struct GSS_push
{
  static void run(GSSelector* gss, const descriptor& desc)
  {
    if(gss != nullptr && std::is_same<descriptor, typename GSSelector::face_descriptor>::value)
    {
      gss->add_face(desc);
    }
    else if(gss != nullptr && std::is_same<descriptor, typename GSSelector::edge_descriptor>::value)
    {
      gss->add_edge(desc);
    }
    else if(gss != nullptr && std::is_same<descriptor, typename GSSelector::vertex_descriptor>::value)
    {
      gss->add_vertex(desc);
    }
  }
};

} // End namespace CGAL

#endif // CGAL_GRAPHICS_SCENE_SELECTOR_H
