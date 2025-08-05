// standard includes
#include <fstream>

// includes for drawing the Voronoi Diagram
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Voronoi_diagram_2.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_traits_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_policies_2.h>
#include <CGAL/draw_voronoi_diagram_2.h>

// typedefs for defining the adaptor
typedef CGAL::Exact_predicates_inexact_constructions_kernel                  K;
typedef CGAL::Delaunay_triangulation_2<K>                                    DT;
typedef CGAL::Delaunay_triangulation_adaptation_traits_2<DT>                 AT;
typedef CGAL::Delaunay_triangulation_caching_degeneracy_removal_policy_2<DT> AP;
typedef CGAL::Voronoi_diagram_2<DT,AT,AP>                                    VD;

// typedef for the result type of the point location
typedef AT::Site_2                    Site_2;

int main(int argc, char* argv[])
{
  VD vd;
  std::ifstream ifs((argc>1)?argv[1]:"data/data4.dt.cin");
  if(ifs)
  {
    Site_2 t;
    while ( ifs >> t ) { vd.insert(t); }
    ifs.close();
    assert( vd.is_valid() );

    CGAL::Graphics_scene gs;
    CGAL::Graphics_scene_selector<VD,
                                  VD::Vertex_iterator,
                                  VD::Halfedge_iterator,
                                  VD::Face_iterator,
                                  void> gss(true);
    
    CGAL::add_to_graphics_scene(vd, gs, &gss);

    #ifdef CGAL_USE_BASIC_VIEWER

    CGAL::Qt::QApplication_and_basic_viewer app(gs, "Small faces");
    if(app)
    {
      app.basic_viewer().on_mouse_pressed = [&gss, &vd] (QMouseEvent* e, CGAL::Qt::Basic_viewer* basic_viewer) -> bool
      {
        if(e->button() == Qt::LeftButton)
        {
          VD::Face_iterator dh = basic_viewer->select_face(e, gss);
          if(dh == VD::Face_iterator())
            return false;
          VD::Ccb_halfedge_circulator ec_start=dh->ccb();
          VD::Ccb_halfedge_circulator ec=ec_start;
          do
          {
            std::cout << ec->source()->point() << std::endl;
          }
          while (++ec!=ec_start);

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
