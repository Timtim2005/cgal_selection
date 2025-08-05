#include <CGAL/Periodic_2_Delaunay_triangulation_2.h>
#include <CGAL/Periodic_2_Delaunay_triangulation_traits_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/draw_periodic_2_triangulation_2.h>

#include <fstream>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Periodic_2_Delaunay_triangulation_traits_2<K> GT;
typedef CGAL::Periodic_2_Delaunay_triangulation_2<GT>       PDT;

typedef PDT::Point                                          Point;

int main(int argc, char* argv[])
{
  CGAL::Graphics_scene gs;

  CGAL::Graphics_scene_selector<PDT,
                                PDT::Periodic_point_iterator,
                                PDT::Periodic_segment_iterator,
                                PDT::Periodic_triangle_iterator,
                                void> gss;
  // Declare periodic triangulation 2D
  PDT T;

  // Read points and insert in T
  Point p;
  std::ifstream ifs((argc > 1) ? argv[1] : "data/data1.dt.cin");
  if (ifs)
  {
    while (ifs >> p)
    { T.insert(p); }

    if( T.is_triangulation_in_1_sheet())
    { T.convert_to_9_sheeted_covering(); }

    // Draw the periodic triangulation
    CGAL::add_to_graphics_scene(T, gs, &gss);

    #ifdef CGAL_USE_BASIC_VIEWER

    CGAL::Qt::QApplication_and_basic_viewer app(gs, "Small faces");
    if(app)
    {
      app.basic_viewer().on_mouse_pressed = [&gss, &T] (QMouseEvent* e, CGAL::Qt::Basic_viewer* basic_viewer) -> bool
      {
        if(e->button() == Qt::LeftButton)
        {
          
          PDT::Periodic_triangle_iterator dh = basic_viewer->select_face(e, gss);
          
          if(dh == PDT::Periodic_triangle_iterator())
            return false;
          typename PDT::Triangle tri(T.triangle(*dh));
          std::cout << tri[0] << std::endl;
          std::cout << tri[1] << std::endl;
          std::cout << tri[2] << std::endl;

          return true;
        }
        return false;
      };

      app.run();
    }

  #endif
  }

  return EXIT_SUCCESS;
}
