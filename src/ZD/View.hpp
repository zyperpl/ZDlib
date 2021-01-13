#pragma once

#include "3rd/glm/mat4x4.hpp"
#include "3rd/glm/vec3.hpp"
#include "3rd/glm/gtc/matrix_transform.hpp"

namespace ZD
{
  namespace Camera
  {
    enum class Projection
    {
      Perspective,
      Ortographic
    };

    struct Fov
    {
      constexpr Fov(float rad)
      : angle_rad { rad }
      {
      }

      static constexpr Fov from_degrees(float degrees)
      {
        return Fov(glm::radians(degrees));
      }

      static constexpr Fov from_radians(float radians) { return Fov(radians); }

      float angle_rad { 0 };
    };

    struct ClippingPlane
    {
      float near, far;

      constexpr ClippingPlane(float near, float far)
      : near { near }
      , far { far }
      {
      }
    };

    struct OrthographicBox
    {
      float left, right, bottom, top;

      constexpr OrthographicBox(
        float left, float right, float bottom, float top)
      : left { left }
      , right { right }
      , bottom { bottom }
      , top { top }
      {
      }
    };

    struct PerspectiveParameters
    {
      Fov fov;
      float aspect;
      ClippingPlane clipping_plane;

      constexpr PerspectiveParameters(
        Fov fov, float aspect, ClippingPlane plane)
      : fov { fov }
      , aspect { aspect }
      , clipping_plane { plane }
      {
      }
    };

    struct OrtographicParameters
    {
      OrthographicBox box;
      ClippingPlane clipping_plane;

      constexpr OrtographicParameters(OrthographicBox box, ClippingPlane plane)
      : box { box }
      , clipping_plane { plane }
      {
      }
    };
  } // namespace Camera

  class View
  {
  public:
    constexpr View(Camera::PerspectiveParameters params, glm::vec3 position)
    : projection { Camera::Projection::Perspective }
    , fov { params.fov }
    , aspect { params.aspect }
    , clipping_plane { params.clipping_plane }
    , position { position }
    {
      assert(projection == Camera::Projection::Perspective);
    }

    constexpr View(Camera::OrtographicParameters params, glm::vec3 position)
    : projection { Camera::Projection::Ortographic }
    , ortographic_box { params.box }
    , clipping_plane { params.clipping_plane }
    , position { position }
    {
      assert(projection == Camera::Projection::Ortographic);
    }

    glm::mat4 get_projection_matrix() const;
    glm::mat4 get_view_matrix() const;
    constexpr glm::vec3 get_position() const { return position; }
    constexpr glm::vec3 get_rotation() const { return rotation; }
    constexpr glm::vec3 get_scale() const { return scale; }
    constexpr glm::vec3 get_target() const { return target; }

    constexpr void set_position(glm::vec3 v) { position = v; }
    constexpr void set_rotation(glm::vec3 v) { rotation = v; }
    constexpr void set_scale(glm::vec3 v) { scale = v; }
    constexpr void set_target(glm::vec3 v) { target = v; }

    constexpr void set_fov(Camera::Fov new_fov) { fov = new_fov; }
    constexpr void set_aspect(float new_aspect) { aspect = new_aspect; }

  private:
    Camera::Projection projection;
    Camera::Fov fov { 180.0 };
    float aspect { 1.0 };
    Camera::OrthographicBox ortographic_box { -1.0, 1.0, 1.0, -1.0 };
    Camera::ClippingPlane clipping_plane { -10.0, 1000.0 };

    glm::vec3 position { 0.0, 0.0, 0.0 };
    glm::vec3 rotation { 0.0, 0.0, 0.0 };
    glm::vec3 scale { 1.0, 1.0, 1.0 };
    glm::vec3 target { 0.0, 0.0, 0.0 };
  };

} // namespace ZD
