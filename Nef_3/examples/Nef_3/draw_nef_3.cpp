#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Nef_polyhedron_3.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/draw_nef_3.h>

#include <fstream>
#include <iostream>

typedef CGAL::Exact_predicates_exact_constructions_kernel   Kernel;
typedef CGAL::Polyhedron_3<Kernel>                          Polyhedron;
typedef CGAL::Nef_polyhedron_3<Kernel>                      Nef_polyhedron;

int main(int argc, char *argv[])
{
  CGAL::Graphics_scene gs;

  CGAL::Graphics_scene_selector<Nef_polyhedron,
                                Nef_polyhedron::Vertex_const_handle,
                                Nef_polyhedron::Halfedge_const_handle,
                                Nef_polyhedron::Halffacet_const_handle,
                                void> gs_selector(true);

  // read OFF file into a polyhedron
  Polyhedron P1, P2;
  std::ifstream ifs1((argc > 1) ? argv[1] : CGAL::data_file_path("meshes/cross_quad.off"));
  ifs1 >> P1;
  std::ifstream ifs2((argc > 2) ? argv[2] : CGAL::data_file_path("meshes/beam.off"));
  ifs2 >> P2;

  // initialize nef from polyhedron
  Nef_polyhedron N1(P1);
  Nef_polyhedron N2(P2);

  Nef_polyhedron anef = N1 - N2;

  // draw Nef Polyhedron
  CGAL::add_to_graphics_scene(anef, gs, &gs_selector);

  #ifdef CGAL_USE_BASIC_VIEWER

  CGAL::Qt::QApplication_and_basic_viewer app(gs, "Small faces");
  if(app)
  {
    app.basic_viewer().on_mouse_pressed = [&gs_selector, &anef] (QMouseEvent* e, CGAL::Qt::Basic_viewer* basic_viewer) -> bool
    {
      if(e->button() == Qt::LeftButton)
      {
        bool selected = false;
        bool found = false;
        Nef_polyhedron::Halfedge_const_handle eh = basic_viewer->select_edge(e, gs_selector, selected);
        if(selected)
        {
          std::cout << eh->source()->point() << ", " << eh->target()->point() << std::endl;
          bool found = true;
        }

        Nef_polyhedron::Vertex_const_handle vh = basic_viewer->select_vertex(e, gs_selector, selected);
        if(selected)
        {
          std::cout << vh->point() << std::endl;
          bool found = true;
        }
        

        return true;
      }
      return false;
    };

    app.run();
  }

  #endif

  return EXIT_SUCCESS;
}
