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
                  typename Triangulation::Vertex_handle,
                  typename Triangulation::Finite_edges_iterator,
                  typename Triangulation::Finite_faces_iterator,
                  void> gs_selector;

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
        auto dh = basic_viewer->select_face(e, gs_selector);
        if(dh != Triangulation::Finite_faces_iterator())
        {
          std::cout << dh->vertex(0)->point() << std::endl;
          std::cout << dh->vertex(1)->point() << std::endl;
          std::cout << dh->vertex(2)->point() << std::endl;
          return true;
        }
      }
      return false;
    };

    app.run();
  }

  #endif


  return EXIT_SUCCESS;
}
