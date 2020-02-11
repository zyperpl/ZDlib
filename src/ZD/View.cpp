#include "View.hpp"

#include <cstdio>

#include "3rd/glm/ext/vector_float3.hpp" // vec3
#include "3rd/glm/ext/matrix_float4x4.hpp" // mat4x4
#include "3rd/glm/ext/matrix_transform.hpp" // translate, rotate, scale, identity
#include "3rd/glm/ext/quaternion_float.hpp" // quat
#include "3rd/glm/gtc/quaternion.hpp"

View::View(Camera::PerspectiveParameters params, glm::vec3 position)
  : projection{Camera::Projection::Perspective}
  , fov{params.fov}
  , aspect{params.aspect}
  , clipping_plane{params.clipping_plane}
  , position{position}
{
  assert(projection == Camera::Projection::Perspective);
}

View::View(Camera::OrtographicParameters params, glm::vec3 position)
  : projection{Camera::Projection::Ortographic}
  , ortographic_box{params.box}
  , clipping_plane{params.clipping_plane}
  , position{position}
{
  assert(projection == Camera::Projection::Ortographic);
}

glm::mat4 View::get_projection_matrix() const
{
  switch (projection)
  {
    case Camera::Projection::Perspective:
      return glm::perspective(fov.angle_rad, aspect, clipping_plane.near, clipping_plane.far);
    case Camera::Projection::Ortographic:
      return glm::ortho(ortographic_box.left, ortographic_box.right, ortographic_box.bottom, ortographic_box.top, clipping_plane.near, clipping_plane.far);
  }
  assert(false);
}

glm::mat4 View::get_view_matrix() const
{
  return glm::lookAt(position, glm::vec3(0,0,0), glm::vec3(0,1,0));
}
