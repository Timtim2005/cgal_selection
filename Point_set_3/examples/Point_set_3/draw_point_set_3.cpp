#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/Point_set_3.h>
#include <CGAL/draw_point_set_3.h>

#include <fstream>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::FT FT;
typedef Kernel::Point_3 Point;
typedef Kernel::Vector_3 Vector;
typedef CGAL::Point_set_3<Point> Point_set;

int main (int argc, char** argv)
{
  CGAL::Graphics_scene_selector<Point_set,
                                  typename Point_set::Index,
                                  int,
                                  int,
                                  void> gss;

  const std::string filename = argc > 1 ? argv[1] : CGAL::data_file_path("points_3/sphere926.pwn");

  Point_set point_set;
  if(!CGAL::IO::read_point_set(filename, point_set))
  {
    std::cerr << "Can't read input file " << filename << std::endl;
    return EXIT_FAILURE;
  }

  CGAL::Graphics_scene gs;

  CGAL::add_to_graphics_scene(point_set, gs, &gss);

  #ifdef CGAL_USE_BASIC_VIEWER

  CGAL::Qt::QApplication_and_basic_viewer app(gs, "Small faces");
  if(app)
  {
    app.basic_viewer().on_mouse_pressed = [&gss, &point_set] (QMouseEvent* e, CGAL::Qt::Basic_viewer* basic_viewer) -> bool
    {
      if(e->button() == Qt::LeftButton)
      {
        bool selected = false;
        int dh = basic_viewer->select_face(e, gss, selected);
        if (selected)
        {
          std::cout << dh << std::endl;
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
