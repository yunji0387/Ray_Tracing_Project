#include "bvh.h"
#include "schema.h"

#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <algorithm>
#include <memory>
#include <tuple>


bool compareAxis(int axis, const std::tuple<Object*, glm::vec3, glm::vec3>& obj1, const std::tuple<Object*, glm::vec3, glm::vec3>& obj2) {
    float obj1Pos = 0.0f;
    float obj2Pos = 0.0f;
    if (std::get<0>(obj1)->type == "sphere") {
        Sphere* sphere = (Sphere*)(std::get<0>(obj1));
        obj1Pos = sphere->position[axis];
    }
    else if (std::get<0>(obj1)->type == "mTriangle") {
        MTriangle* mTriangle = (MTriangle*)(std::get<0>(obj1));
        obj1Pos = mTriangle->midPoint[axis];
    }

    if (std::get<0>(obj2)->type == "sphere") {
        Sphere* sphere = (Sphere*)(std::get<0>(obj2));
        obj2Pos = sphere->position[axis];
    }
    else if (std::get<0>(obj2)->type == "mTriangle") {
        MTriangle* mTriangle = (MTriangle*)(std::get<0>(obj2));
        obj2Pos = mTriangle->midPoint[axis];
    }
    
    return obj1Pos < obj2Pos;
}

BVHNode* subdivide(std::vector<std::tuple<Object*, glm::vec3, glm::vec3>> bvhObjects)
{
    BVHNode* ret = new BVHNode();

    // recurse base case
    if (bvhObjects.size() == 1)
    {
        ret->obj = std::get<0>(bvhObjects[0]);
        ret->aabbMinBound = std::get<1>(bvhObjects[0]);
        ret->aabbMaxBound = std::get<2>(bvhObjects[0]);
        return ret;
    }

    // calculate global bounding box
    Vector globalMinBound(float(1e30f), float(1e30f), float(1e30f));
    Vector globalMaxBound(float(-1e30f), float(-1e30f), float(-1e30f));
    for (auto& currObj : bvhObjects)
    {
        Vector currMinBound = std::get<1>(currObj);
        Vector currMaxBound = std::get<2>(currObj);

        globalMinBound.x = std::min(globalMinBound.x, currMinBound.x);
        globalMinBound.y = std::min(globalMinBound.y, currMinBound.y);
        globalMinBound.z = std::min(globalMinBound.z, currMinBound.z);
    
        globalMaxBound.x = std::max(globalMaxBound.x, currMaxBound.x);
        globalMaxBound.y = std::max(globalMaxBound.y, currMaxBound.y);
        globalMaxBound.z = std::max(globalMaxBound.z, currMaxBound.z);
    }
    ret->aabbMinBound = globalMinBound;
    ret->aabbMaxBound = globalMaxBound;

    Vector extent = globalMaxBound - globalMinBound;
    int longestAxis = 0;

    if (extent.x < extent.y && extent.z < extent.y)
    {
        longestAxis = 1;
    }
    else if(extent.x < extent.z && extent.y < extent.z)
    {
        longestAxis = 2;
    }

    // Sort the bvhObjects list based on longestAxis
    std::sort(bvhObjects.begin(), bvhObjects.end(),
        [longestAxis](const std::tuple<Object*, glm::vec3, glm::vec3>& obj1, const std::tuple<Object*, glm::vec3, glm::vec3>& obj2) {
            return compareAxis(longestAxis, obj1, obj2);
        });


    const auto half = bvhObjects.begin() + bvhObjects.size() / 2;
    auto leftHalfBVHNodes = std::vector<std::tuple<Object*, glm::vec3, glm::vec3>>(bvhObjects.begin(), half);
    auto rightHalfBVHNodes = std::vector<std::tuple<Object*, glm::vec3, glm::vec3>>(half, bvhObjects.end());
    
    ret->left = subdivide(leftHalfBVHNodes);
    ret->right = subdivide(rightHalfBVHNodes);

    return ret;
}

BVHNode* buildBVH(const std::vector<Object*>& objects)
{
    std::vector<std::tuple<Object*, glm::vec3, glm::vec3>> bvhObjects;
    // filter and calculate per-object bounding box, then push back to sorted
    for (auto object : objects)
    {
        if (object->type == "sphere")
        {
            const float radius = ((Sphere*)object)->radius;
            const glm::vec3& pos = ((Sphere*)object)->position;

            Vector minBound = Vector(pos.x - radius, pos.y - radius, pos.z - radius);
            Vector maxBound = Vector(pos.x + radius, pos.y + radius, pos.z + radius);

            bvhObjects.push_back(std::make_tuple(object, minBound, maxBound));
        }
        else if (object->type == "mesh")
        {
            Mesh* mesh = (Mesh*)(object);
            const auto& triangles = mesh->triangles;

            for (int i = 0; i < triangles.size(); i++)
            {
                MTriangle* mTriangle = new MTriangle(mesh->material, triangles[i]);
                Vector minBound = Vector(float(1e30f), float(1e30f), float(1e30f));
                Vector maxBound = Vector(float(-1e30f), float(-1e30f), float(-1e30f));

                minBound.x = std::min(triangles[i].vertices[0].x, minBound.x);
                minBound.x = std::min(triangles[i].vertices[1].x, minBound.x);
                minBound.x = std::min(triangles[i].vertices[2].x, minBound.x);

                minBound.y = std::min(triangles[i].vertices[0].y, minBound.y);
                minBound.y = std::min(triangles[i].vertices[1].y, minBound.y);
                minBound.y = std::min(triangles[i].vertices[2].y, minBound.y);

                minBound.z = std::min(triangles[i].vertices[0].z, minBound.z);
                minBound.z = std::min(triangles[i].vertices[1].z, minBound.z);
                minBound.z = std::min(triangles[i].vertices[2].z, minBound.z);

                maxBound.x = std::max(triangles[i].vertices[0].x, maxBound.x);
                maxBound.x = std::max(triangles[i].vertices[1].x, maxBound.x);
                maxBound.x = std::max(triangles[i].vertices[2].x, maxBound.x);

                maxBound.y = std::max(triangles[i].vertices[0].y, maxBound.y);
                maxBound.y = std::max(triangles[i].vertices[1].y, maxBound.y);
                maxBound.y = std::max(triangles[i].vertices[2].y, maxBound.y);

                maxBound.z = std::max(triangles[i].vertices[0].z, maxBound.z);
                maxBound.z = std::max(triangles[i].vertices[1].z, maxBound.z);
                maxBound.z = std::max(triangles[i].vertices[2].z, maxBound.z);

                mTriangle->midPoint = Vertex((maxBound.x + minBound.x) / 2.0f, (maxBound.y + minBound.y) / 2.0f, (maxBound.z + minBound.z) / 2.0f);

                bvhObjects.push_back(std::make_tuple(mTriangle, minBound, maxBound));
            }
        }
    }
    return subdivide(bvhObjects);
}
