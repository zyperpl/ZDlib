#include "Entity.hpp"

#include "3rd/glm/ext/vector_float3.hpp" // vec3
#include "3rd/glm/ext/matrix_float4x4.hpp" // mat4x4
#include "3rd/glm/ext/matrix_transform.hpp" // translate, rotate, scale, identity
#include "3rd/glm/ext/quaternion_float.hpp" // quat
#include "3rd/glm/gtc/quaternion.hpp"
#include "3rd/glm/gtc/type_ptr.hpp" // value_ptr

#include "OpenGLRenderer.hpp"

namespace ZD
{
  Entity::Entity(glm::vec3 position, glm::quat rotation, glm::vec3 scale)
  : position { position }
  , rotation { rotation }
  , scale { scale }
  {
  }

  void Entity::update() { printf("Updating entity %p...\n", (void *)(this)); }

  glm::mat4 Entity::get_model_matrix() const
  {
    glm::mat4 translate_mat = glm::translate(glm::mat4(1), glm::vec3(position.x, position.y, position.z));
    glm::mat4 rotate_mat = glm::mat4(rotation);
    glm::mat4 scale_mat = glm::scale(glm::mat4(1), glm::vec3(scale.x, scale.y, scale.z));

    return translate_mat * rotate_mat * scale_mat;
  }

  void Entity::draw(const ShaderProgram &shader_program, const View &view)
  {
    shader_program.use();

    glm::mat4 model_matrix = get_model_matrix();
    glm::mat4 projection_matrix = view.get_projection_matrix();
    glm::mat4 view_matrix = view.get_view_matrix();

    auto v_location = shader_program.get_uniform("V");
    assert(v_location);
    glUniformMatrix4fv(v_location->location, 1, GL_FALSE, glm::value_ptr(view_matrix));

    auto m_location = shader_program.get_uniform("M");
    assert(m_location);
    glUniformMatrix4fv(m_location->location, 1, GL_FALSE, glm::value_ptr(model_matrix));

    auto p_location = shader_program.get_uniform("P");
    assert(p_location);
    glUniformMatrix4fv(p_location->location, 1, GL_FALSE, glm::value_ptr(projection_matrix));
    glCheckError();

    for (size_t i = 0; i < textures.size(); ++i)
    {
      const auto &texture = textures[i];
      texture->bind(shader_program, i, texture->get_name());
    }

    for (const auto &model : models)
    {
      model->draw(shader_program);
    }
  }
} // namespace ZD
