#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/draw_surface_mesh.h>
#include <fstream>

typedef CGAL::Simple_cartesian<double>                       Kernel;
typedef Kernel::Point_3                                      Point;
typedef CGAL::Surface_mesh<Point>                            Mesh;

int main(int argc, char* argv[])
{
  const std::string filename = (argc>1) ? argv[1] : CGAL::data_file_path("meshes/elephant.off");

  CGAL::Graphics_scene gs;
  CGAL::Graphics_scene_selector<Mesh,
                                 Mesh::Vertex_index,
                                 Mesh::Edge_index,
                                 Mesh::Face_index,
                                 void> gss;

  Mesh sm;
  if(!CGAL::IO::read_polygon_mesh(filename, sm))
  {
    std::cerr << "Invalid input file: " << filename << std::endl;
    return EXIT_FAILURE;
  }

  // Internal color property maps are used if they exist and are called "v:color", "e:color" and "f:color".
  auto vcm = sm.add_property_map<Mesh::Vertex_index, CGAL::IO::Color>("v:color").first;
  auto ecm = sm.add_property_map<Mesh::Edge_index, CGAL::IO::Color>("e:color").first;
  auto fcm = sm.add_property_map<Mesh::Face_index>("f:color", CGAL::IO::white() /*default*/).first;

  for(auto v : vertices(sm))
  {
    if(v.idx()%2)
    { put(vcm, v, CGAL::IO::black()); }
    else
    { put(vcm, v, CGAL::IO::blue()); }
  }

  for(auto e : edges(sm))
  { put(ecm, e, CGAL::IO::gray()); }

  put(fcm, *(sm.faces().begin()), CGAL::IO::red());

  // Draw!
  CGAL::add_to_graphics_scene(sm, gs, &gss);

  #ifdef CGAL_USE_BASIC_VIEWER

  CGAL::Qt::QApplication_and_basic_viewer app(gs, "Small faces");
  if(app)
  {
    app.basic_viewer().on_mouse_pressed = [&gss, &sm] (QMouseEvent* e, CGAL::Qt::Basic_viewer* basic_viewer) -> bool
    {
      if(e->button() == Qt::LeftButton)
      {
        bool found = false;
        bool selected = false;
        auto point_pmap = get(CGAL::vertex_point, sm);
        Mesh::Face_index fh = basic_viewer->select_face(e, gss, selected);
        if(selected)
        {
          auto hd = halfedge(fh, sm);
          const auto first_hd = hd;
          do
          {
            auto v = source(hd, sm);
            std::cout << get(point_pmap, v) << std::endl;
            hd = next(hd, sm);
          }
          while (hd != first_hd);
          found = true;
        }

        Mesh::Edge_index eh = basic_viewer->select_edge(e, gss, selected);
        if(selected)
        {
          std::cout << get(point_pmap, source(halfedge(eh, sm), sm)) << " to "
                    << get(point_pmap, target(halfedge(eh, sm), sm)) << std::endl;
          found = true;
        }


        Mesh::Vertex_index vh = basic_viewer->select_vertex(e, gss, selected);
        if(selected)
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
