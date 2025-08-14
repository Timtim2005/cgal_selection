#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>
#include <CGAL/draw_polyhedron.h>
#include <fstream>

typedef CGAL::Exact_predicates_inexact_constructions_kernel  Kernel;
typedef CGAL::Polyhedron_3<Kernel>                       Polyhedron;

int main(int argc, char* argv[])
{
  CGAL::Graphics_scene gs;

  CGAL::Graphics_scene_selector<Polyhedron, 
                               boost::graph_traits<Polyhedron>::vertex_descriptor, 
                               boost::graph_traits<Polyhedron>::edge_descriptor,
                               boost::graph_traits<Polyhedron>::face_descriptor,
                               void> gss;

  Polyhedron P;
  std::ifstream in1((argc>1)?argv[1]:CGAL::data_file_path("meshes/cross_quad.off"));
  in1 >> P;
  CGAL::add_to_graphics_scene(P, gs, &gss);

  #ifdef CGAL_USE_BASIC_VIEWER

  CGAL::Qt::QApplication_and_basic_viewer app(gs, "Small faces");
  if(app)
  {
    app.basic_viewer().on_mouse_pressed = [&gss, &P] (QMouseEvent* e, CGAL::Qt::Basic_viewer* basic_viewer) -> bool
    {
      if(e->button() == Qt::LeftButton)
      {
        bool found = false;
        auto point_pmap = get(CGAL::vertex_point, P);
        boost::graph_traits<Polyhedron>::face_descriptor fh = basic_viewer->select_face(e, gss);
        if(fh != boost::graph_traits<Polyhedron>::null_face())
        {
          std::cout << "Face vertices:" << std::endl;
          auto hd = fh->halfedge();
          const auto first_hd = hd;
          do
          {
            auto v = hd->vertex();
            std::cout << v->point() << std::endl;
            hd = hd->next();
          } while (hd != first_hd);
          found = true;
        }

        boost::graph_traits<Polyhedron>::edge_descriptor eh = basic_viewer->select_edge(e, gss);
        if(eh != boost::graph_traits<Polyhedron>::edge_descriptor())
        {
          std::cout << get(point_pmap, source(halfedge(eh, P), P)) << std::endl;
          std::cout << get(point_pmap, target(halfedge(eh, P), P)) << std::endl;
          found = true;
        }

        boost::graph_traits<Polyhedron>::vertex_descriptor vh = basic_viewer->select_vertex(e, gss);
        if(vh != boost::graph_traits<Polyhedron>::null_vertex())
        {
          std::cout << get(point_pmap, vh) << std::endl;
          found = true;
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
