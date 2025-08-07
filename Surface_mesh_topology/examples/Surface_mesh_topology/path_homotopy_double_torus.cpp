#include <CGAL/Linear_cell_complex_for_combinatorial_map.h>
#include <CGAL/Linear_cell_complex_constructors.h>
#include <CGAL/Curves_on_surface_topology.h>
#include <CGAL/Path_on_surface.h>
#include <CGAL/draw_face_graph_with_paths.h>

typedef CGAL::Linear_cell_complex_for_combinatorial_map<2,3> LCC_3_cmap;
using namespace CGAL::Surface_mesh_topology;

///////////////////////////////////////////////////////////////////////////////
void create_path_1(Path_on_surface<LCC_3_cmap>& p)
{
  p.push_back_by_index(14); // Its starting dart
  for (int i=0; i<7; ++i)
  { p.extend_positive_turn(2); } // Extend the path
}

///////////////////////////////////////////////////////////////////////////////
void create_path_2(Path_on_surface<LCC_3_cmap>& p)
{ p.push_back_by_index({202, 206, 335, 317, 322, 69, 62, 414}); }

///////////////////////////////////////////////////////////////////////////////
void create_path_3(Path_on_surface<LCC_3_cmap>& p)
{
  p.push_back_by_index(470); // Its starting dart
  for (int i=0; i<13; ++i)
  { p.extend_positive_turn(2); } // Extend the path
}

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
  bool draw=(argc>1?std::string(argv[1])=="-draw":false);
  LCC_3_cmap lcc;
  if (!CGAL::load_off(lcc, CGAL::data_file_path("meshes/double-torus-example.off").c_str()))
  {
    std::cout<<"ERROR reading file "<< CGAL::data_file_path("meshes/double-torus-example.off") << std::endl;
    exit(EXIT_FAILURE);
  }

  Curves_on_surface_topology<LCC_3_cmap> cst(lcc);
  Path_on_surface<LCC_3_cmap> p1(lcc), p2(lcc), p3(lcc);
  create_path_1(p1);
  create_path_2(p2);
  create_path_3(p3);

  bool res1=cst.is_contractible(p1);
  std::cout<<"Path p1 (pink) "<<(res1?"IS":"IS NOT")
           <<" contractible."<<std::endl;

  bool res2=cst.are_freely_homotopic(p1, p2);
  std::cout<<"Path p1 (pink) "<<(res2?"IS":"IS NOT")
           <<" homotopic with path p2 (green)."<<std::endl;

  bool res3=cst.are_freely_homotopic(p1, p3);
  std::cout<<"Path p1 (pink) "<<(res3?"IS":"IS NOT")
           <<" homotopic with path p3 (orange)."<<std::endl;

  CGAL::Graphics_scene gs;

  CGAL::Graphics_scene_selector<LCC_3_cmap,
                                CGAL::Get_map<LCC_3_cmap, LCC_3_cmap>::type::Dart_const_descriptor /*vh*/,
                                CGAL::Get_map<LCC_3_cmap, LCC_3_cmap>::type::Dart_const_descriptor /*eh*/,
                                CGAL::Get_map<LCC_3_cmap, LCC_3_cmap>::type::Dart_const_descriptor /*fh*/> gss(true);

  if (draw)
  {
    auto cycles={p1, p2, p3};
    CGAL::add_to_graphics_scene(lcc, gs, &gss, cycles);

    #ifdef CGAL_USE_BASIC_VIEWER

    CGAL::Qt::QApplication_and_basic_viewer app(gs, "Small faces");
    if(app)
    {
      app.basic_viewer().on_mouse_pressed = [&gss, &lcc] (QMouseEvent* e, CGAL::Qt::Basic_viewer* basic_viewer) -> bool
      {
        if(e->button() == Qt::LeftButton)
        {
          bool found = false;
          LCC_3_cmap::Dart_const_handle fh = basic_viewer->select_face(e, gss);
          if(fh != LCC_3_cmap::null_descriptor)
          {
            found = true;
            LCC_3_cmap::Dart_const_handle cur = fh;
            std::cout << "Face: ";
            do
            {
              std::cout << lcc.point(cur) << std::endl;
              cur = lcc.next(cur);
            } while (cur != fh);
            found = true;
          }

          
          LCC_3_cmap::Dart_const_handle eh = basic_viewer->select_edge(e, gss);
          if(eh != LCC_3_cmap::null_descriptor)
          {
            LCC_3_cmap::Dart_const_handle cur = eh;
            std::cout << "Edge: ";

            std::cout << lcc.point(cur) << std::endl;
            std::cout << lcc.point(lcc.other_extremity(cur)) << std::endl;
            found = true;
          }

          LCC_3_cmap::Dart_const_handle vh = basic_viewer->select_vertex(e, gss);
          if(vh != LCC_3_cmap::null_descriptor)
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

    #endif
  }

  return EXIT_SUCCESS;
}
///////////////////////////////////////////////////////////////////////////////

