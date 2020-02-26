#include "Entity.hpp"

#include "3rd/glm/ext/vector_float3.hpp" // vec3
#include "3rd/glm/ext/matrix_float4x4.hpp" // mat4x4
#include "3rd/glm/ext/matrix_transform.hpp" // translate, rotate, scale, identity
#include "3rd/glm/ext/quaternion_float.hpp" // quat
#include "3rd/glm/gtc/quaternion.hpp"

Entity::Entity(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
: position { position }
, rotation { rotation }
, scale { scale }
{
  printf("Entity %p created.\n", this);
}

void Entity::update() { printf("Updating entity %p...\n", this); }

glm::mat4 Entity::get_model_matrix() const
{
  glm::mat4 translate_mat =
    glm::translate(glm::mat4(1), glm::vec3(position.x, position.y, position.z));
  glm::mat4 rotate_mat =
    glm::mat4(glm::quat(glm::vec3(rotation.x, rotation.y, rotation.z)));
  glm::mat4 scale_mat =
    glm::scale(glm::mat4(1), glm::vec3(scale.x, scale.y, scale.z));

  return translate_mat * rotate_mat * scale_mat;
}

void Entity::draw(const ShaderProgram &shader_program, const View &view)
{
  shader_program.use();

  auto model_matrix = get_model_matrix();
  auto projection_matrix = view.get_projection_matrix();
  auto view_matrix = view.get_view_matrix();

  auto m_location = shader_program.get_uniform("M");
  assert(m_location);
  glUniformMatrix4fv(m_location->index, 1, GL_FALSE, &model_matrix[0][0]);

  auto v_location = shader_program.get_uniform("V");
  assert(v_location);
  glUniformMatrix4fv(v_location->index, 1, GL_FALSE, &view_matrix[0][0]);

  auto p_location = shader_program.get_uniform("P");
  assert(p_location);
  glUniformMatrix4fv(p_location->index, 1, GL_FALSE, &projection_matrix[0][0]);

  for (const auto &model : models) { model->draw(shader_program); }
}
