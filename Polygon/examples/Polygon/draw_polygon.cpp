#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/draw_polygon_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Polygon_2<K>                                  Polygon_2;
typedef CGAL::Point_2<K>                                    Point;

int main()
{
  // create a polygon and put some points in it
  Polygon_2 p;
  p.push_back(Point(0,0));
  p.push_back(Point(4,0));
  p.push_back(Point(4,4));
  p.push_back(Point(2,2));
  p.push_back(Point(0,4));

  CGAL::Graphics_scene gs;

  CGAL::Graphics_scene_selector<Polygon_2,
                                Polygon_2::Vertex_const_iterator,
                                Polygon_2::Vertex_const_iterator,
                                void*, void> gss(true);


  CGAL::add_to_graphics_scene(p, gs, &gss);

  #ifdef CGAL_USE_BASIC_VIEWER

  CGAL::Qt::QApplication_and_basic_viewer app(gs, "Small faces");
  if(app)
  {
    app.basic_viewer().on_mouse_pressed = [&gss, &p] (QMouseEvent* e, CGAL::Qt::Basic_viewer* basic_viewer) -> bool
    {
      if(e->button() == Qt::LeftButton)
      {
        Polygon_2::Vertex_const_iterator dh = basic_viewer->select_edge(e, gss);
        if(dh == Polygon_2::Vertex_const_iterator())
          return false;
        std::cout << *dh << std::endl;
        return true;
      }
      return false;
    };

    app.run();
  }

  #endif

  return EXIT_SUCCESS;
}
