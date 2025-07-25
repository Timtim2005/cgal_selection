// Copyright (c) 2022 GeometryFactory Sarl (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
//
// $URL$
// $Id$
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-Commercial
//
// Author(s): Guillaume Damiand <guillaume.damiand@liris.cnrs.fr>
//            Mostafa Ashraf <mostaphaashraf1996@gmail.com>

#ifndef CGAL_GRAPHICS_SCENE_H
#define CGAL_GRAPHICS_SCENE_H

// TODO #include <CGAL/license/GraphicsView.h>

#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <tuple>
#include <vector>

#include <CGAL/Cartesian_converter.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Constrained_triangulation_plus_2.h>
#include <CGAL/IO/Color.h>
#include <CGAL/Projection_traits_3.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/assertions.h>
#include <CGAL/Random.h>

#include <cstdlib>

#include <CGAL/Buffer_for_vao.h>

#include <CGAL/Graphics_scene_selector.h> // ADDED

namespace CGAL {

//------------------------------------------------------------------------------
inline CGAL::IO::Color get_random_color(CGAL::Random& random)
{
  CGAL::IO::Color res;
  do
  {
    res=CGAL::IO::Color(random.get_int(0,256),
                        random.get_int(0,256),
                        random.get_int(0,256));
  }
  while(res.red()==255 && res.green()==255 && res.blue()==255);
  return res;
}
//------------------------------------------------------------------------------
// This class is responsible for dealing with available CGAL data structures and
// handling buffers.
class Graphics_scene
{
public:
  using BufferType=float;

  typedef CGAL::Exact_predicates_inexact_constructions_kernel Local_kernel;
  typedef Local_kernel::Point_3 Local_point;
  typedef Local_kernel::Vector_3 Local_vector;

  Graphics_scene()
      : m_buffer_for_points(&arrays[POS_POINTS], nullptr,
                            &m_bounding_box, &arrays[COLOR_POINTS]),
        m_buffer_for_segments(&arrays[POS_SEGMENTS], nullptr,
                              &m_bounding_box, &arrays[COLOR_SEGMENTS]),
        m_buffer_for_rays(&arrays[POS_RAYS], nullptr, &m_bounding_box,
                          &arrays[COLOR_RAYS]),
        m_buffer_for_lines(&arrays[POS_RAYS], nullptr,
                           &m_bounding_box, &arrays[COLOR_LINES]),
        m_buffer_for_faces(&arrays[POS_FACES], nullptr, &m_bounding_box, &arrays[COLOR_FACES],
                           &arrays[FLAT_NORMAL_FACES], &arrays[SMOOTH_NORMAL_FACES]),
        m_default_color_face(60, 60, 200),
        m_default_color_point(200, 60, 60),
        m_default_color_segment(0, 0, 0),
        m_default_color_ray(0, 0, 0),
        m_default_color_line(0, 0, 0)
  {}

  inline
  const CGAL::IO::Color &get_default_color_face() const
  { return m_default_color_face; }

  inline
  const CGAL::IO::Color &get_default_color_point() const
  { return m_default_color_point; }

  inline
  const CGAL::IO::Color &get_default_color_segment() const
  { return m_default_color_segment; }

  inline
  const CGAL::IO::Color &get_default_color_ray() const
  { return m_default_color_ray; }

  inline
  const CGAL::IO::Color &get_default_color_line() const
  { return m_default_color_line; }

  inline
  const Buffer_for_vao &get_buffer_for_points() const
  { return m_buffer_for_points; }

  inline
  void set_default_color_face(const CGAL::IO::Color& c)
  { m_default_color_face = c; }

  inline
  void set_default_color_point(const CGAL::IO::Color& c)
  { m_default_color_point = c; }

  inline
  void set_default_color_segment(const CGAL::IO::Color& c)
  { m_default_color_segment = c; }

  inline
  void set_default_color_ray(const CGAL::IO::Color& c)
  { m_default_color_ray = c; }

  inline
  void set_default_color_line(const CGAL::IO::Color& c)
  { m_default_color_line = c; }

  inline
  const Buffer_for_vao &get_buffer_for_segments() const
  { return m_buffer_for_segments; }

  inline
  const Buffer_for_vao &get_buffer_for_rays() const
  { return m_buffer_for_rays; }

  inline
  const Buffer_for_vao &get_buffer_for_lines() const
  { return m_buffer_for_lines; }

  inline
  const Buffer_for_vao &get_buffer_for_faces() const
  { return m_buffer_for_faces; }

  const CGAL::Bbox_3 &bounding_box() const { return m_bounding_box; }

  const std::vector<BufferType> &get_array_of_index(int index) const
  { assert(index<LAST_INDEX); return arrays[index]; }

  int get_size_of_index(int index) const
  { return static_cast<int>(arrays[index].size()*sizeof(BufferType)); }

  unsigned int number_of_elements(int index) const
  { return static_cast<unsigned int>(arrays[index].size()/3); }

  const std::vector<std::size_t> &get_full_faces_index() const // ADDED
  { return m_full_faces_index; } // ADDED

  void initiate_bounding_box(const CGAL::Bbox_3& new_bounding_box)
  { m_bounding_box = new_bounding_box; }

  void update_bounding_box(const CGAL::Bbox_3 &box) { m_bounding_box+=box; }

  template <typename KPoint, typename KVector>
  void update_bounding_box_for_ray(const KPoint &p, const KVector &v)
  {
    Local_point lp = get_local_point(p);
    Local_vector lv = get_local_vector(v);
    update_bounding_box((lp + lv).bbox());
  }

  template <typename KPoint, typename KVector>
  void update_bounding_box_for_line(const KPoint &p, const KVector &v,
                                    const KVector &pv)
  {
    Local_point lp = get_local_point(p);
    Local_vector lv = get_local_vector(v);
    Local_vector lpv = get_local_vector(pv);
    update_bounding_box(lp.bbox() + (lp + lv).bbox() + (lp + lpv).bbox());
  }

  void reverse_all_normals() const
  {
    m_buffer_for_faces.negate_normals();
  }

  template <typename KPoint, class GSSelector, typename vertex_descriptor>
  void add_point(const KPoint &p, GSSelector *gss = nullptr,
                 vertex_descriptor vd = {})
  {
    m_buffer_for_points.add_point(p, m_default_color_point);

    GSS_push<GSSelector, vertex_descriptor>::run(gss, vd);
  }

  template <typename KPoint, class GSSelector, typename vertex_descriptor>
  void add_point(const KPoint &p, const CGAL::IO::Color &acolor,
                 GSSelector *gss = nullptr,
                 vertex_descriptor vd = {})
  {
    m_buffer_for_points.add_point(p, acolor);

    GSS_push<GSSelector, vertex_descriptor>::run(gss, vd);
  }

  template <typename KPoint, class GSSelector, typename edge_descriptor>
  void add_segment(const KPoint &p1, const KPoint &p2, GSSelector *gss = nullptr, 
                   edge_descriptor ed = {})
  {
    m_buffer_for_segments.add_segment(p1, p2, m_default_color_segment);

    GSS_push<GSSelector, edge_descriptor>::run(gss, ed);
  }

  template <typename KPoint, class GSSelector, typename edge_descriptor>
  void add_segment(const KPoint &p1, const KPoint &p2,
                   const CGAL::IO::Color &acolor, GSSelector *gss = nullptr,
                   edge_descriptor ed = {})
  {
    m_buffer_for_segments.add_segment(p1, p2, acolor);

    GSS_push<GSSelector, edge_descriptor>::run(gss, ed);
  }

  template <typename KPoint, typename KVector>
  void add_ray(const KPoint &p, const KVector &v)
  {
    double bigNumber = 1e30;
    m_buffer_for_rays.add_ray_segment(p, (p + (bigNumber)*v), m_default_color_ray);
  }

  template <typename KPoint, typename KVector>
  void add_ray(const KPoint &p, const KVector &v,
               const CGAL::IO::Color &acolor)
  {
    double bigNumber = 1e30;
    m_buffer_for_rays.add_ray_segment(p, (p + (bigNumber)*v), acolor);
  }

  template <typename KPoint, typename KVector>
  void add_line(const KPoint &p, const KVector &v)
  {
    double bigNumber = 1e30;
    m_buffer_for_lines.add_line_segment((p - (bigNumber)*v),
                                             (p + (bigNumber)*v), m_default_color_line);
  }

  template <typename KPoint, typename KVector>
  void add_line(const KPoint &p, const KVector &v,
                const CGAL::IO::Color &acolor)
  {
    double bigNumber = 1e30;
    m_buffer_for_lines.add_line_segment((p - (bigNumber)*v),
                                                (p + (bigNumber)*v), acolor);
  }

  template <typename KPoint> bool add_point_in_face(const KPoint &kp)
  {
    if (m_buffer_for_faces.is_a_face_started())
    { return m_buffer_for_faces.add_point_in_face(kp); }
    return false;
  }

  template <typename KPoint, typename KVector>
  bool add_point_in_face(const KPoint &kp, const KVector &p_normal)
  {
    if (m_buffer_for_faces.is_a_face_started())
    { return m_buffer_for_faces.add_point_in_face(kp, p_normal); }
    return false;
  }

  bool a_face_started() const
  {
    return m_buffer_for_faces.is_a_face_started();
  }

  void face_begin()
  {
    if (a_face_started())
    {
      std::cerr
          << "You cannot start a new face before to finish the previous one."
          << std::endl;
    }
    else
    { m_buffer_for_faces.face_begin(m_default_color_face); }
  }

  void face_begin(const CGAL::IO::Color &acolor)
  {
    if (a_face_started())
    {
      std::cerr
          << "You cannot start a new face before to finish the previous one."
          << std::endl;
    }
    else
    { m_buffer_for_faces.face_begin(acolor); }
  }

  template<typename GSSelector, typename face_descriptor>
  void face_end(GSSelector* gss = nullptr, face_descriptor fd = {})
  {
    if (m_buffer_for_faces.is_a_face_started())
    {
      std::size_t nb_triangles_before = arrays[POS_FACES].size() / 9; // ADDED
      m_buffer_for_faces.face_end(); 
      std::size_t nb_triangles_after = arrays[POS_FACES].size() / 9; // ADDED
      std::size_t nb_triangles_added = nb_triangles_after - nb_triangles_before; // ADDED
      
      for(std::size_t i = 0; i < nb_triangles_added; ++i) // ADDED
      {
        m_full_faces_index.push_back(m_full_faces_number); // ADDED
      }
      ++m_full_faces_number; // ADDED

      GSS_push<GSSelector, face_descriptor>::run(gss, fd);
    }
  }

  template <typename KPoint>
  void add_text(const KPoint &kp, const std::string &txt)
  {
    Local_point p = get_local_point(kp);
    m_texts.push_back(std::make_tuple(p, txt));
  }

  template <typename KPoint>
  void add_text(const KPoint &kp, const char *txt)
  { add_text(kp, std::string(txt)); }

  bool empty() const
  {
    return (m_buffer_for_points.is_empty() &&
            m_buffer_for_segments.is_empty() &&
            m_buffer_for_rays.is_empty() &&
            m_buffer_for_lines.is_empty() &&
            m_buffer_for_faces.is_empty());
  }

  bool has_zero_x() const
  {
    return m_buffer_for_points.has_zero_x() &&
           m_buffer_for_segments.has_zero_x() &&
           m_buffer_for_faces.has_zero_x() &&
           m_buffer_for_rays.has_zero_x() &&
           m_buffer_for_lines.has_zero_x();
  }

  bool has_zero_y() const
  {
    return m_buffer_for_points.has_zero_y() &&
           m_buffer_for_segments.has_zero_y() &&
           m_buffer_for_faces.has_zero_y() &&
           m_buffer_for_rays.has_zero_y() &&
           m_buffer_for_lines.has_zero_y();
  }

  bool has_zero_z() const
  {
    return m_buffer_for_points.has_zero_z() &&
           m_buffer_for_segments.has_zero_z() &&
           m_buffer_for_faces.has_zero_z() &&
           m_buffer_for_rays.has_zero_z() &&
           m_buffer_for_lines.has_zero_z();
  }

  // Returns true if the data structure lies on a XY or XZ or YZ plane
  bool is_two_dimensional() const
  {
    return (!empty() && (has_zero_x() || has_zero_y() || has_zero_z()));
  }

  void clear()
  {
    m_buffer_for_points.clear();
    m_buffer_for_segments.clear();
    m_buffer_for_rays.clear();
    m_buffer_for_lines.clear();
    m_buffer_for_faces.clear();
    m_texts.clear();
    m_full_faces_index.clear(); // ADDED
    m_full_faces_number = 0; // ADDED
    m_bounding_box=CGAL::Bbox_3();
  }

  template <typename KPoint>
  static Local_point get_local_point(const KPoint &p)
  {
    return internal::Geom_utils<typename CGAL::Kernel_traits<KPoint>::Kernel,
                                Local_kernel>::get_local_point(p);
  }

  template <typename KVector>
  static Local_vector get_local_vector(const KVector &v)
  {
    return internal::Geom_utils<typename CGAL::Kernel_traits<KVector>::Kernel,
                                Local_kernel>::get_local_vector(v);
  }

  void m_texts_clear()
  { m_texts.clear(); }

  std::size_t m_texts_size() const
  { return m_texts.size(); }

  const std::vector<std::tuple<Local_point, std::string>>& get_m_texts() const
  { return m_texts; }

public:
  // The following enum gives the indices of different elements of arrays
  // vectors.
  enum Buffers {
    BEGIN_POS = 0,
    POS_POINTS = BEGIN_POS,
    POS_SEGMENTS,
    POS_RAYS,
    POS_LINES,
    POS_FACES,
    END_POS,
    BEGIN_COLOR = END_POS,
    COLOR_POINTS = BEGIN_COLOR,
    COLOR_SEGMENTS,
    COLOR_RAYS,
    COLOR_LINES,
    COLOR_FACES,
    END_COLOR,
    BEGIN_NORMAL = END_COLOR,
    SMOOTH_NORMAL_FACES = BEGIN_NORMAL,
    FLAT_NORMAL_FACES,
    END_NORMAL,
    LAST_INDEX = END_NORMAL
  };

protected:
  Buffer_for_vao m_buffer_for_points;
  Buffer_for_vao m_buffer_for_segments;
  Buffer_for_vao m_buffer_for_rays;
  Buffer_for_vao m_buffer_for_lines;
  Buffer_for_vao m_buffer_for_faces;

  std::vector<std::size_t> m_full_faces_index; // ADDED
  std::size_t m_full_faces_number = 0; // ADDED

  CGAL::IO::Color m_default_color_face;
  CGAL::IO::Color m_default_color_point;
  CGAL::IO::Color m_default_color_segment;
  CGAL::IO::Color m_default_color_ray;
  CGAL::IO::Color m_default_color_line;

  std::vector<std::tuple<Local_point, std::string>> m_texts;

  std::vector<BufferType> arrays[LAST_INDEX];

  CGAL::Bbox_3 m_bounding_box;
};

} // namespace CGAL

#endif // CGAL_GRAPHICS_SCENE_H
