#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/draw_constrained_triangulation_2.h>

#include <cassert>
#include <iostream>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

typedef CGAL::Exact_predicates_tag                               Itag;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, CGAL::Default, Itag> CDT;
typedef CDT::Point Point;
typedef CDT::Edge  Edge;
int
main( )
{
  CGAL::Graphics_scene gs;

  CGAL::Graphics_scene_selector<CDT,
                  CDT::Vertex_handle,
                  CDT::Finite_edges_iterator,
                  CDT::Finite_faces_iterator> gs_selector;

  CDT cdt;
  std::cout << "Inserting a grid of 5x5 constraints " << std::endl;
  for (int i = 1; i < 6; ++i)
    cdt.insert_constraint( Point(0,i), Point(6,i));
  for (int j = 1; j < 6; ++j)
    cdt.insert_constraint( Point(j,0), Point(j,6));

  assert(cdt.is_valid());
  int count = 0;
  for (const Edge& e : cdt.finite_edges())
    if (cdt.is_constrained(e))
      ++count;
  std::cout << "The number of resulting constrained edges is  ";
  std::cout <<  count << std::endl;

  CGAL::add_to_graphics_scene(cdt, gs, &gs_selector);

  #ifdef CGAL_USE_BASIC_VIEWER

  CGAL::Qt::QApplication_and_basic_viewer app(gs, "Small faces");
  if(app)
  {
    app.basic_viewer().on_mouse_pressed = [&gs_selector, &cdt] (QMouseEvent* e, CGAL::Qt::Basic_viewer* basic_viewer) -> bool
    {
      if(e->button() == Qt::LeftButton)
      {
        bool found = false;
        CDT::Finite_faces_iterator dh = basic_viewer->select_face(e, gs_selector);
        auto invalid_face = CDT::Finite_faces_iterator();
        if(dh != invalid_face)
        {
          std::cout << "Face : ";
          std::cout << dh->vertex(0)->point() << std::endl;
          std::cout << dh->vertex(1)->point() << std::endl;
          std::cout << dh->vertex(2)->point() << std::endl;
          found = true;
        }

        CDT::Finite_edges_iterator eh = basic_viewer->select_edge(e, gs_selector);
        auto invalid_edge = CDT::Finite_edges_iterator();
        if(eh != invalid_edge)
        {
          std::cout << "Edge : ";
          std::cout << eh->first->vertex(eh->first->cw(eh->second))->point() << std::endl;
          std::cout << eh->first->vertex(eh->first->ccw(eh->second))->point() << std::endl;
          found = true;
        }

        CDT::Vertex_handle vh = basic_viewer->select_vertex(e, gs_selector);
        auto invalid_vertex = CDT::Vertex_handle();
        if(vh != invalid_vertex)
        {
          std::cout << "Vertex : ";
          std::cout << vh->point() << std::endl;
          found = true;
        }

        return found;
      }
      return false;
    };

    app.run();
  }

  #endif

  return 0;
}
