#include <CGAL/Linear_cell_complex_for_combinatorial_map.h>
#include <CGAL/Linear_cell_complex_constructors.h>
#include <fstream>
#include <CGAL/Curves_on_surface_topology.h>
#include <CGAL/Path_on_surface.h>
#include <CGAL/squared_distance_3.h>
#include <CGAL/draw_face_graph_with_paths.h>

using LCC_3          =CGAL::Linear_cell_complex_for_combinatorial_map<2, 3>;
using Path_on_surface=CGAL::Surface_mesh_topology::Path_on_surface<LCC_3>;

double cycle_length(const LCC_3& lcc, const Path_on_surface& cycle)
{ // Compute the length of the given cycle.
  double res=0;
  for (std::size_t i=0; i<cycle.length(); ++i)
  { res+=std::sqrt
      (CGAL::squared_distance(lcc.point(cycle[i]),
                              lcc.point(lcc.other_extremity(cycle[i])))); }
  return res;
}

void display_cycle_info(const LCC_3& lcc, const Path_on_surface& cycle)
{ // Display information about the given cycle.
  if (cycle.is_empty()) { std::cout<<"Empty."<<std::endl; return; }
  std::cout<<"Root: "<<lcc.point(cycle[0])<<"; "
           <<"Number of edges: "<<cycle.length()<<"; "
           <<"Length: "<<cycle_length(lcc, cycle)<<std::endl;
}

int main(int argc, char* argv[])
{
  std::string filename(argc==1?CGAL::data_file_path("meshes/3torus.off"):CGAL::data_file_path(argv[1]));
  bool draw=(argc<3?false:(std::string(argv[2])=="-draw"));
  LCC_3 lcc;
  if (!CGAL::load_off(lcc, filename.c_str())) // Load the off file.
  {
    std::cout<<"Cannot read file '"<<filename<<"'. Exiting program"<<std::endl;
    return EXIT_FAILURE;
  }
 std::cout<<"File '"<<filename<<"' loaded. Finding edge-width of the mesh..."<<std::endl;

  CGAL::Surface_mesh_topology::Curves_on_surface_topology<LCC_3> cst(lcc, true);

  Path_on_surface cycle1=cst.compute_edge_width(true);

  CGAL::Surface_mesh_topology::Euclidean_length_weight_functor<LCC_3> wf(lcc);
  Path_on_surface cycle2=cst.compute_shortest_non_contractible_cycle(wf, true);

  std::cout<<"Cycle 1 (pink): "; display_cycle_info(lcc, cycle1);
  std::cout<<"Cycle 2 (green): "; display_cycle_info(lcc, cycle2);
  if (draw)
  {
    auto cycles={cycle1, cycle2};

    CGAL::Graphics_scene gs;
    CGAL::Graphics_scene_selector<LCC_3,
                                       LCC_3::Dart_const_handle,
                                       LCC_3::Dart_const_handle,
                                       LCC_3::Dart_const_handle> gss;

    CGAL::add_to_graphics_scene(lcc, gs, &gss, cycles);

    CGAL::Qt::QApplication_and_basic_viewer app(gs, "Small faces");
    if(app)
    {
      app.basic_viewer().on_mouse_pressed = [&gss, &lcc] (QMouseEvent* e, CGAL::Qt::Basic_viewer* basic_viewer) -> bool
      {
        if(e->button() == Qt::LeftButton)
        {
          std::cout << "Face: ";
          bool found = false;
          bool selected = false;
          LCC_3::Dart_const_handle fh = basic_viewer->select_face(e, gss, selected);
          if(selected)
          {
            found = true;
            LCC_3::Dart_const_handle cur = fh;
            std::cout << "Face: ";
            do
            {
              std::cout << lcc.point(cur) << std::endl;
              cur = lcc.next(cur);
            } while (cur != fh);
            found = true;
          }
          
          LCC_3::Dart_const_handle eh = basic_viewer->select_edge(e, gss, selected);
          if(selected)
          {
            LCC_3::Dart_const_handle cur = eh;
            std::cout << "Edge: ";

            std::cout << lcc.point(cur) << std::endl;
            std::cout << lcc.point(lcc.other_extremity(cur)) << std::endl;
            found = true;
          }

          LCC_3::Dart_const_handle vh = basic_viewer->select_vertex(e, gss, selected);
          if(selected)
          {
            std::cout << "Vertex: ";
            std::cout << lcc.point(vh) << std::endl;
            found = true;
          }

          return found;
        }
        return false;
      };

      app.run();
    }
  }

  return EXIT_SUCCESS;
}
