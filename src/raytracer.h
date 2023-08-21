#pragma once

#include <glm/glm.hpp>
#include "schema.h"

typedef glm::vec3 point3;
typedef glm::vec3 colour3;

const bool DISABLE_TOON_SHADING = true;

extern double fov;
extern colour3 background_colour;

float randomFloat();
void choose_scene(char const *fn);
bool ssTrace(const Vertex& e, const std::vector<Vector>& ss, RGB& colour, bool pick);
bool trace(const point3 &e, const point3 &s, colour3 &colour, bool pick);
