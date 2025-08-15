#ifndef DRAW_FACEWIDTH_H
#define DRAW_FACEWIDTH_H

#include <CGAL/draw_linear_cell_complex.h>
#include <CGAL/Graphics_scene_options.h>

template<typename ALCC>
struct Facewidth_graphics_scene_options:
  public CGAL::Graphics_scene_options<ALCC,
                                      typename ALCC::Dart_const_handle,
                                      typename ALCC::Dart_const_handle,
                                      typename ALCC::Dart_const_handle,
                                      typename ALCC::Dart_const_handle>
{
  Facewidth_graphics_scene_options(typename ALCC::size_type amark1, typename ALCC::size_type amark2) :
    m_vertex_mark(amark1), m_face_mark(amark2)
  {
    this->colored_vertex=[this](const ALCC& alcc, typename ALCC::Dart_const_handle dh)->bool
    { return alcc.is_marked(dh, m_vertex_mark); };
    this->vertex_color=[](const ALCC&, typename ALCC::Dart_const_handle)->CGAL::IO::Color
    { return CGAL::IO::Color(0, 255, 0); };

    this->colored_face=[](const ALCC&, typename ALCC::Dart_const_handle)->bool
    { return true; };
    this->face_color=[this](const ALCC& alcc, typename ALCC::Dart_const_handle dh)->CGAL::IO::Color
    { return alcc.is_marked(dh, m_face_mark)?CGAL::IO::Color(255, 0, 0)
        :CGAL::IO::Color(211, 211, 211); };
  }

  typename ALCC::size_type m_vertex_mark, m_face_mark;
};

template<typename LCC>
void draw_facewidth(const LCC& lcc,
                    const std::vector<typename LCC::Dart_const_descriptor>& cycle)
{
  typename LCC::size_type vertex_mark = lcc.get_new_mark();
  typename LCC::size_type face_mark = lcc.get_new_mark();
  for (std::size_t i=0; i<cycle.size(); ++i)
  {
    // Color the vertices
    if (!lcc.is_marked(cycle[i], vertex_mark))
    { lcc.template mark_cell<0>(cycle[i], vertex_mark); }
    // Color the faces
    if (!lcc.is_marked(cycle[i], face_mark))
    { lcc.template mark_cell<2>(cycle[i], face_mark); }
  }

  Facewidth_graphics_scene_options<LCC> df(vertex_mark, face_mark);

  CGAL::Graphics_scene gs;

  CGAL::Graphics_scene_selector<LCC,
                                typename LCC::Dart_const_descriptor,
                                typename LCC::Dart_const_descriptor,
                                typename LCC::Dart_const_descriptor> gss;

  CGAL::add_to_graphics_scene(lcc, gs, df, &gss);

  CGAL::Qt::QApplication_and_basic_viewer app(gs, "Small faces");
  if(app)
  {
    app.basic_viewer().on_mouse_pressed = [&gss, &lcc] (QMouseEvent* e, CGAL::Qt::Basic_viewer* basic_viewer) -> bool
    {
      if(e->button() == Qt::LeftButton)
      {
        bool selected = false;
        typename LCC::Dart_const_handle dh = basic_viewer->select_face(e, gss, selected);
        if(!selected)
          return false;
        typename LCC::Dart_const_handle cur = dh;
        do
        {
          std::cout << lcc.point(cur) << std::endl;
          cur = lcc.next(cur);
        } while (cur != dh);

        return true;
        /*LCC::Dart_const_handle dh = basic_viewer->select_edge(e, gss);
        if(dh == LCC::null_descriptor)
          return false;
        LCC::Dart_const_handle cur = dh;

        std::cout << lcc.point(cur) << std::endl;
        std::cout << lcc.point(lcc.other_extremity(cur)) << std::endl;

        return true;*/
      }
      return false;
    };

    app.run();
  }

  lcc.free_mark(vertex_mark);
  lcc.free_mark(face_mark);
}

#endif // DRAW_FACEWIDTH_H
