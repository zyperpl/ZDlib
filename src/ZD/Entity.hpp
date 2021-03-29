#pragma once

#include <cstdio>
#include <cstdlib>
#include <memory>
#include <vector>

#include "Model.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "View.hpp"

#include "3rd/glm/mat4x4.hpp"
#include "3rd/glm/vec3.hpp"

namespace ZD
{
  class Entity
  {
  public:
    Entity() = default;
    Entity(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
    void update();
    void draw(const ShaderProgram &shader_program, const View &view);

    inline void add_model(std::shared_ptr<Model> model) { models.push_back(model); }
    inline void add_texture(std::shared_ptr<Texture> texture) { textures.push_back(texture); }

    glm::vec3 get_position() const { return position; }
    glm::vec3 get_rotation() const { return rotation; }
    glm::vec3 get_scale() const { return scale; }

    void set_position(glm::vec3 pos) { position = pos; }
    void set_rotation(glm::vec3 rot) { rotation = rot; }
    void set_scale(glm::vec3 scl) { scale = scl; }

    void move_position(glm::vec3 v) { position += v; }
    void add_rotation(glm::vec3 v) { rotation += v; }
    void multiply_scale(float a) { scale *= a; }

    glm::mat4 get_model_matrix() const;

  protected:
    glm::vec3 position { 0.0, 0.0, 0.0 };
    glm::vec3 rotation { 0.0, 0.0, 0.0 };
    glm::vec3 scale { 1.0, 1.0, 1.0 };
    std::vector<std::shared_ptr<Model>> models;
    std::vector<std::shared_ptr<Texture>> textures;
  };

} // namespace ZD
