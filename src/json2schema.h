#include "schema.h"
#include "json.hpp"

using json = nlohmann::json;

int json_to_scene(json &jscene, Scene &s);
void printf_rgb(RGB &rgb);
void printf_vertex(Vertex &v);
void printf_vector(Vector &v);
void printf_material(Material &m);
void scene_to_cpp(Scene &s);

