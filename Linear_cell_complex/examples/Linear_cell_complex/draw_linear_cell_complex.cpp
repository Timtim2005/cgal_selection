#include <CGAL/Linear_cell_complex_for_combinatorial_map.h>
#include <CGAL/draw_linear_cell_complex.h>

typedef CGAL::Linear_cell_complex_for_combinatorial_map<3> LCC;
typedef LCC::Dart_descriptor Dart_descriptor;
typedef LCC::Point           Point;

int main()
{
  LCC lcc;
  Dart_descriptor d1=
    lcc.make_hexahedron(Point(0,0,0), Point(5,0,0),
                        Point(5,5,0), Point(0,5,0),
                        Point(0,5,4), Point(0,0,4),
                        Point(5,0,4), Point(5,5,4));
  Dart_descriptor d2=
    lcc.make_hexahedron(Point(5,0,0), Point(10,0,0),
                        Point(10,5,0), Point(5,5,0),
                        Point(5,5,4), Point(5,0,4),
                        Point(10,0,4), Point(10,5,4));

  lcc.sew<3>(lcc.beta(d1, 1, 1, 2), lcc.beta(d2, 2));

  lcc.display_characteristics(std::cout)<<", valid="
                                        <<lcc.is_valid()<<std::endl;

  CGAL::Graphics_scene gs;

  CGAL::Graphics_scene_selector<LCC,
                                LCC::Dart_const_handle,
                                LCC::Dart_const_handle,
                                LCC::Dart_const_handle> gss;

  CGAL::add_to_graphics_scene(lcc, gs, &gss);

  #ifdef CGAL_USE_BASIC_VIEWER

  CGAL::Qt::QApplication_and_basic_viewer app(gs, "Small faces");
  if(app)
  {
    app.basic_viewer().on_mouse_pressed = [&gss, &lcc] (QMouseEvent* e, CGAL::Qt::Basic_viewer* basic_viewer) -> bool
    {
      if(e->button() == Qt::LeftButton)
      {
        bool selected = false;
        LCC::Dart_const_handle dh = basic_viewer->select_face(e, gss, selected);
        if(!selected)
          return false;
        LCC::Dart_const_handle cur = dh;
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

  #endif

  return EXIT_SUCCESS;
}
