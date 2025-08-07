#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_2.h>
#include <CGAL/draw_triangulation_2.h>
#include <fstream>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_2<K>                            Triangulation;
typedef Triangulation::Point                                Point;

int main(int argc, char* argv[]) {
  CGAL::Graphics_scene gs;

  CGAL::Graphics_scene_selector<Triangulation,
                  Triangulation::Vertex_handle,
                  Triangulation::Finite_edges_iterator,
                  Triangulation::Finite_faces_iterator> gs_selector;

  std::ifstream in((argc>1)?argv[1]:"data/triangulation_prog1.cin");
  std::istream_iterator<Point> begin(in);
  std::istream_iterator<Point> end;

  Triangulation t;
  t.insert(begin, end);

  CGAL::add_to_graphics_scene(t, gs, &gs_selector);

  #ifdef CGAL_USE_BASIC_VIEWER

  CGAL::Qt::QApplication_and_basic_viewer app(gs, "Small faces");
  if(app)
  {
    app.basic_viewer().on_mouse_pressed = [&gs_selector, &t] (QMouseEvent* e, CGAL::Qt::Basic_viewer* basic_viewer) -> bool
    {
      if(e->button() == Qt::LeftButton)
      {
        bool found = false;
        Triangulation::Finite_faces_iterator dh = basic_viewer->select_face(e, gs_selector);
        auto invalid_face = Triangulation::Finite_faces_iterator();
        if(dh != invalid_face)
        {
          std::cout << "Face : ";
          std::cout << dh->vertex(0)->point() << std::endl;
          std::cout << dh->vertex(1)->point() << std::endl;
          std::cout << dh->vertex(2)->point() << std::endl;
          found = true;
        }

        Triangulation::Finite_edges_iterator eh = basic_viewer->select_edge(e, gs_selector);
        Triangulation::Finite_edges_iterator bh = basic_viewer->select_edge(e, gs_selector);
        if(eh == bh)
        {
          std::cout << "Edge : ";
          std::cout << eh->first->vertex(eh->first->cw(eh->second))->point() << std::endl;
          std::cout << eh->first->vertex(eh->first->ccw(eh->second))->point() << std::endl;
          found = true;
        }
        
        Triangulation::Vertex_handle vh = basic_viewer->select_vertex(e, gs_selector);
        auto invalid_vertex = Triangulation::Vertex_handle();
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


  return EXIT_SUCCESS;
}
