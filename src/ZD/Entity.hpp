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
#include "3rd/glm/ext/quaternion_float.hpp" // quat

namespace ZD
{
  class Entity
  {
  public:
    Entity() = default;
    Entity(const glm::vec3 position, const glm::quat rotation, const glm::vec3 scale);
    virtual ~Entity() = default;

    virtual void render(const ShaderProgram &shader_program, const View &view);

    inline void add_model(const std::shared_ptr<Model> model) { models.push_back(model); }
    inline void add_texture(const std::shared_ptr<Texture> texture) { textures.push_back(texture); }

    glm::vec3 get_position() const { return position; }
    glm::quat get_rotation() const { return rotation; }
    glm::vec3 get_scale() const { return scale; }

    void set_position(const glm::vec3 pos) { position = pos; }
    void set_rotation(const glm::vec3 rot) { rotation = glm::quat(rot); }
    void set_rotation(const glm::quat q) { rotation = q; }
    void set_scale(const glm::vec3 scl) { scale = scl; }

    void move_position(const glm::vec3 v) { position += v; }
    void add_rotation(const glm::vec3 v) { rotation *= glm::quat(v); }
    void add_rotation(const glm::quat q) { rotation = glm::normalize(q) * glm::normalize(rotation); }
    void multiply_scale(const float a) { scale *= a; }

    glm::mat4 get_model_matrix() const;

  protected:
    glm::vec3 position { 0.0, 0.0, 0.0 };
    glm::quat rotation { 0.0, 0.0, 0.0, 0.0 };
    glm::vec3 scale { 1.0, 1.0, 1.0 };
    std::vector<std::shared_ptr<Model>> models;
    std::vector<std::shared_ptr<Texture>> textures;
  };

} // namespace ZD
