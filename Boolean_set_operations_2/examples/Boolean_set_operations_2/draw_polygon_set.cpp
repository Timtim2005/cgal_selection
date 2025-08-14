/*! \file draw_polygon_set.cpp
 * Drawing a polygon set.
 */

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/Polygon_set_2.h>
#include <CGAL/draw_polygon_set_2.h>

typedef CGAL::Exact_predicates_exact_constructions_kernel K;
typedef CGAL::Polygon_2<K>                                Polygon_2;
typedef CGAL::Polygon_with_holes_2<K>                     Polygon_with_holes_2;
typedef CGAL::Polygon_set_2<K>                            Polygon_set_2;
typedef CGAL::Point_2<K>                                  Point_2;

Polygon_2 rectangle(int l)
{
  // Create a rectangle with given side length.
  Polygon_2 P;
  P.push_back(Point_2(-l,-l));
  P.push_back(Point_2(l,-l));
  P.push_back(Point_2(l,l));
  P.push_back(Point_2(-l,l));

  return P;
}

int main()
{
  // Create a large rectangle A, with a hole and a smaller rectangle
  // B inside A's hole.
  Polygon_with_holes_2 A(rectangle(3));
  Polygon_2 H(rectangle(2));
  H.reverse_orientation();
  A.add_hole(H);
  Polygon_2 B(rectangle(1));

  // Add them to a polygon set and draw it.
  Polygon_set_2 S;
  S.insert(A);
  S.insert(B);

  CGAL::Graphics_scene gs;

  CGAL::Graphics_scene_selector<Polygon_2,
                                Polygon_2::Vertex_const_iterator,
                                Polygon_2::Vertex_const_iterator,
                                void*> gss;

  CGAL::add_to_graphics_scene(S, gs, &gss);

  #ifdef CGAL_USE_BASIC_VIEWER

  CGAL::Qt::QApplication_and_basic_viewer app(gs, "Small faces");
  if(app)
  {
    app.basic_viewer().on_mouse_pressed = [&gss, &S, &A, &B] (QMouseEvent* e, CGAL::Qt::Basic_viewer* basic_viewer) -> bool
    {
      if(e->button() == Qt::LeftButton)
      {
        bool found = false;
        bool next = true;
        bool selected = false;
        Polygon_2::Vertex_const_iterator vh = basic_viewer->select_vertex(e, gss, selected);
        if(selected)
        {
          std::cout << "Vertex : ";
          std::cout << *vh << std::endl;
          found = true;
        }

        Polygon_2::Vertex_const_iterator eh = basic_viewer->select_edge(e, gss, selected);
        if(selected)
        {
          std::cout << "Edge : ";
          std::vector<Polygon_with_holes_2> pwhs;
          S.polygons_with_holes(std::back_inserter(pwhs));
          for (const auto& pwh : pwhs)
          {
            if(!pwh.outer_boundary().is_empty() && eh == pwh.outer_boundary().vertices_begin())
            {
              std::cout << *eh << std::endl;
              std::cout << *pwh.outer_boundary().vertices_end() << std::endl;
              found = true;
              bool next = false;
            }
          }
          if(next)
          {
            std::cout << *eh << std::endl;
            --eh;
            std::cout << *eh << std::endl;
            found = true;
          }
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
