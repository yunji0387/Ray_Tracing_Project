// Ray Tracer Assignment JSON->CPP utility
// Winter 2021

#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <glm/glm.hpp>

#include "schema.h"

#include "json2scene.h"

using json = nlohmann::json;

double fov = 60;
// colour3 background_colour(0, 0, 0);

json jscene;
Scene s;

/****************************************************************************/

void scene_to_cpp(Scene &s) {
  printf("Scene scene = {\n");
  printf("  // camera\n");
  printf("  { %f, ", s.camera.field);
  printf_rgb(s.camera.background);
  printf(" },\n");
  printf("  // objects\n");
  printf("  {\n");

  for (int i = 0; i < s.objects.size(); i++) {
    Object *o = s.objects[i];
    
    if (o->type == "sphere") {
      Sphere *s = (Sphere *)(o);
      printf("    new Sphere( ");
      printf_material(s->material);
      printf(", %f, ", s->radius);
      printf_vertex(s->position);
      printf(" )");

    } else if (o->type == "plane") {
      Plane *p = (Plane *)(o);
      printf("    new Plane( ");
      printf_material(p->material);
      printf(", ");
      printf_vertex(p->position);
      printf(", ");
      printf_vector(p->normal);
      printf(" )");
      
    } else if (o->type == "mesh") {
      Mesh *m = (Mesh *)(o);
      printf("    new Mesh( ");
      printf_material(m->material);
      printf(",\n");
      printf("      {\n");
      for (int j = 0; j < m->triangles.size(); j++) {
        printf("         { ");
        printf_vertex(m->triangles[j].vertices[0]);
        printf(", ");
        printf_vertex(m->triangles[j].vertices[1]);
        printf(", ");
        printf_vertex(m->triangles[j].vertices[2]);
        printf(" }");
        if (j < m->triangles.size() - 1) {
          printf(",\n");
        } else {
          printf("\n");
        }
      }
      printf("      } )");
    }
    
    if (i < s.objects.size() - 1) {
      printf(",\n");
    } else {
      printf("\n");
    }
  }

  printf("  },\n");
  printf("  // lights\n");
  printf("  {\n");

  for (int i = 0; i < s.lights.size(); i++) {
    Light *l = s.lights[i];
    
    if (l->type == "ambient") {
      AmbientLight *a = (AmbientLight *)(l);
      printf("    new AmbientLight( ");
      printf_rgb(a->color);
      printf(" )");
    } else if (l->type == "directional") {
      DirectionalLight *d = (DirectionalLight *)(l);
      printf("    new DirectionalLight( ");
      printf_rgb(d->color);
      printf(", ");
      printf_vector(d->direction);
      printf(" )");
    } else if (l->type == "point") {
      PointLight *p = (PointLight *)(l);
      printf("    new PointLight( ");
      printf_rgb(p->color);
      printf(", ");
      printf_vertex(p->position);
      printf(" )");
    } else if (l->type == "spot") {
      SpotLight *s = (SpotLight *)(l);
      printf("    new SpotLight( ");
      printf_rgb(s->color);
      printf(", ");
      printf_vertex(s->position);
      printf(", ");
      printf_vector(s->direction);
      printf(", %f )", s->cutoff);
    }
    
    if (i < s.lights.size() - 1) {
      printf(",\n");
    } else {
      printf("\n");
    }
  }

  printf("  }\n");
  printf("};\n");
}

/****************************************************************************/

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " scene.json\n";
    return -1;
  }
  
  std::fstream in(argv[1]);
  if (!in.is_open()) {
    std::cout << "Unable to open scene file " << argv[1] << std::endl;
    exit(EXIT_FAILURE);
  }

  in >> jscene;
  in.close();
  
  int result = json_to_scene(jscene, s);

  if (result < 0) {
    return result;
  }
  
  scene_to_cpp(s);
  
  return 0;
}