#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/point_generators_3.h>
#include <CGAL/draw_triangulation_3.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_3<K>                   DT3;
typedef CGAL::Creator_uniform_3<double,K::Point_3>          Creator;

int main()
{
  std::vector<K::Point_3> points;
  CGAL::Random_points_in_sphere_3<K::Point_3,Creator> g(1.0);
  std::copy_n(g, 50, std::back_inserter(points));

  DT3 dt3(points.begin(), points.end());

  CGAL::Graphics_scene_selector<DT3,
                       typename DT3::Vertex_handle,
                       typename DT3::Finite_edges_iterator,
                       typename DT3::Finite_facets_iterator,
                       void> gss;

  CGAL::Graphics_scene gs;

  CGAL::add_to_graphics_scene(dt3, gs, &gss);

  #ifdef CGAL_USE_BASIC_VIEWER

  CGAL::Qt::QApplication_and_basic_viewer app(gs, "Small faces");
  if(app)
  {
    app.basic_viewer().on_mouse_pressed = [&gss, &dt3] (QMouseEvent* e, CGAL::Qt::Basic_viewer* basic_viewer) -> bool
    {
      if(e->button() == Qt::LeftButton)
      {
        bool found = false;
        bool selected = false;
        DT3::Finite_facets_iterator fh = basic_viewer->select_face(e, gss, selected);
        if(selected)
        {
          found = true;
          std::cout << fh->first->vertex(0)->point() << std::endl;
          std::cout << fh->first->vertex(1)->point() << std::endl;
          std::cout << fh->first->vertex(2)->point() << std::endl;
        }

        DT3::Finite_edges_iterator ed = basic_viewer->select_edge(e, gss, selected);
        if(selected)
        {
          found = true;
          std::cout << ed->first->vertex(ed->second)->point() << " - "
                    << ed->first->vertex(ed->third)->point() << std::endl;
        }

        DT3::Vertex_handle vh = basic_viewer->select_vertex(e, gss, selected);
        if(selected)
        {
          found = true;
          std::cout << vh->point() << std::endl;
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
