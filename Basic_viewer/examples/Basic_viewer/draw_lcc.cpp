#include <CGAL/Linear_cell_complex_for_combinatorial_map.h>
#include <CGAL/draw_linear_cell_complex.h>
#include <CGAL/Graphics_scene_selector.h>
#include <iostream>

using LCC=CGAL::Linear_cell_complex_for_combinatorial_map<3>;
using Point=LCC::Point;

int main()
{
  CGAL::Graphics_scene gs;

  CGAL::Graphics_scene_selector<LCC,
                                 LCC::Dart_const_handle,
                                 LCC::Dart_const_handle,
                                 LCC::Dart_const_handle,
                                 void> gss;

  LCC lcc;
  
  LCC::Dart_handle res;

  lcc.make_hexahedron(Point(0,0,0), Point(5,0,0),
                      Point(5,5,0), Point(0,5,0),
                      Point(0,5,4), Point(0,0,4),
                      Point(5,0,4), Point(5,5,4));
  CGAL::add_to_graphics_scene(lcc, gs, &gss);

  #ifdef CGAL_USE_BASIC_VIEWER

  CGAL::Qt::QApplication_and_basic_viewer app(gs, "Small faces");
  if(app)
  {
    app.basic_viewer().on_mouse_pressed = [&gss, &res, &lcc] (QMouseEvent* e, CGAL::Qt::Basic_viewer* basic_viewer) -> bool
    {
      if(e->button() == Qt::LeftButton)
      {
        LCC::Dart_const_handle dh = basic_viewer->select_face(e, gss);
        if(dh == LCC::null_descriptor)
          return false;
        LCC::Dart_const_handle cur = dh;
        do
        {
          std::cout << lcc.point(cur) << std::endl;
          cur = lcc.next(cur);
        } while (cur != dh);

        return true;
      }
      return false;
    };

    app.run();
  }

  #endif

  return EXIT_SUCCESS;
}
