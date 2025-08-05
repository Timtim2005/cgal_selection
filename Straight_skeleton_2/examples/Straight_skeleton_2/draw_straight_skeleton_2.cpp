#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/Polygon_2.h>
#include <CGAL/create_straight_skeleton_2.h>
#include <CGAL/draw_straight_skeleton_2.h>

#include <cassert>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K ;

typedef K::Point_2                   Point ;
typedef CGAL::Polygon_2<K>           Polygon_2 ;
typedef CGAL::Straight_skeleton_2<K> Ss ;

typedef std::shared_ptr<Ss> SsPtr ;

int main()
{
  CGAL::Graphics_scene gs;
  CGAL::Graphics_scene_selector<Ss, Ss::Vertex_const_handle,
                                Ss::Halfedge_const_handle,
                                Ss::Face_const_handle, void> gs_selector(true);

  Polygon_2 poly ;
  poly.push_back( Point(-1,-1) ) ;
  poly.push_back( Point(0,-12) ) ;
  poly.push_back( Point(1,-1) ) ;
  poly.push_back( Point(12,0) ) ;
  poly.push_back( Point(1,1) ) ;
  poly.push_back( Point(0,12) ) ;
  poly.push_back( Point(-1,1) ) ;
  poly.push_back( Point(-12,0) ) ;

  assert(poly.is_counterclockwise_oriented());

  // You can pass the polygon via an iterator pair
  SsPtr iss = CGAL::create_interior_straight_skeleton_2(poly.vertices_begin(), poly.vertices_end());

  CGAL::add_to_graphics_scene(*iss, gs, &gs_selector);

  #ifdef CGAL_USE_BASIC_VIEWER

  CGAL::Qt::QApplication_and_basic_viewer app(gs, "Small faces");
  if(app)
  {
    app.basic_viewer().on_mouse_pressed = [&gs_selector] (QMouseEvent* e, CGAL::Qt::Basic_viewer* basic_viewer) -> bool
    {
      if(e->button() == Qt::LeftButton)
      {
        Ss::Halfedge_const_handle dh = basic_viewer->select_edge(e, gs_selector);
        if(dh == Ss::Halfedge_const_handle())
          return false;

        std::cout << dh->opposite()->vertex()->point() << std::endl;
        std::cout << dh->vertex()->point() << std::endl;

        return true;
      }
      return false;
    };

    app.run();
  }

  #endif

  return 0;
}
