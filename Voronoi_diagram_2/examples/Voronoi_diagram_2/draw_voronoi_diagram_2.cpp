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
          bool found = false;
          VD::Face_iterator fh = basic_viewer->select_face(e, gss);
          if(fh != VD::Face_iterator{})
          {
            if(!fh->is_unbounded())
            {
              found = true;
              std::cout << "Face: ";
              VD::Ccb_halfedge_circulator ec_start=fh->ccb();
              VD::Ccb_halfedge_circulator ec=ec_start;
              do
              {
                std::cout << ec->source()->point() << std::endl;
              }
              while (++ec!=ec_start);
            }
          }

          VD::Halfedge_iterator eh = basic_viewer->select_edge(e, gss);
          VD::Halfedge_iterator bh = basic_viewer->select_edge(e, gss);
          if(eh == bh)
          {
            std::cout << "Iterator is segment: " << (eh->is_segment() ? "yes" : "no") << std::endl;
            std::cout << "Has source: " << (eh->has_source() ? "yes" : "no") << std::endl;
            std::cout << "Has target: " << (eh->has_target() ? "yes" : "no") << std::endl;
            if(eh->is_segment())
            {
              found = true;
              std::cout << "Edge: ";
              std::cout << eh->source()->point() << " to " << eh->target()->point() << std::endl;
            }
          }

          VD::Vertex_iterator vh = basic_viewer->select_vertex(e, gss);
          if(vh != VD::Vertex_iterator())
          {
            found = true;
            std::cout << "Vertex: " << vh->point() << std::endl;
          }

          return found;
        }
        return false;
      };

      app.run();
    }

    #endif
  }

  return EXIT_SUCCESS;
}
