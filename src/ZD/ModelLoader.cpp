#include "ModelLoader.hpp"

#include <cstdio>
#include <cstdlib>
#include <string>

#define TINYOBJLOADER_IMPLEMENTATION 
#include "3rd/tiny_obj_loader.h"

std::optional<std::vector<ModelData>> ModelLoader::load(std::string_view file_name)
{
  std::vector<ModelData> models;

  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  std::string warn, err;

  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, file_name.data());

  if (!warn.empty()) 
  {
    printf("OBJ load warning: %s\n", warn.data());
  }

  if (!err.empty()) 
  {
    printf("OBJ load error: %s\n", err.data());
    return {};
  }

  if (!ret) 
  {
    return {};
  }

  for (const auto &shape : shapes)
  {
    ModelData md;
    size_t index_offset = 0;
    for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) 
    {
      int fv = shape.mesh.num_face_vertices[f];

      for (ssize_t v = 0; v < fv; v++) {
        tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

        tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index+0];
        tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index+1];
        tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index+2];
        md.vertices.push_back(vx);
        md.vertices.push_back(vy);
        md.vertices.push_back(vz);

        if (attrib.normals.size() > 0)
        {
          tinyobj::real_t nx = attrib.normals[3*idx.normal_index+0];
          tinyobj::real_t ny = attrib.normals[3*idx.normal_index+1];
          tinyobj::real_t nz = attrib.normals[3*idx.normal_index+2];
          md.normals.push_back(nx);
          md.normals.push_back(ny);
          md.normals.push_back(nz);
        }

        if (attrib.texcoords.size() > 0)
        {
          tinyobj::real_t tx = attrib.texcoords[2*idx.texcoord_index+0];
          tinyobj::real_t ty = attrib.texcoords[2*idx.texcoord_index+1];
          md.uvs.push_back(tx);
          md.uvs.push_back(1.0f - ty);
        }
      }
      index_offset += fv;
    }
    models.push_back(md);
  }

  return models;
}
