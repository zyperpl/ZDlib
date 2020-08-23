#pragma once

#include "Image.hpp"
#include "Model.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "RenderTarget.hpp"

#include <memory>

#include "3rd/glm/glm.hpp"

namespace ZD
{
  class Sprite
  {
  public:
    Sprite(std::shared_ptr<Image> image);
    Sprite(std::shared_ptr<Image> image, const Size frame_size);
    Sprite(
      std::shared_ptr<ShaderProgram> shader_program,
      std::shared_ptr<Image> image, const Size frame_size);
    virtual ~Sprite() = default;

    int set_frame(int new_frame)
    {
      frame = new_frame % max_frames;
      return frame;
    }
    int increase_frame(int amount = 1)
    {
      frame += amount;
      frame %= max_frames;
      return frame;
    }
    int decrease_frame(int amount = 1)
    {
      frame -= amount;
      frame %= max_frames;
      return frame;
    }

    int get_frame() const { return frame; }
    int get_max_frames() const { return max_frames; }

    virtual void render(const RenderTarget &);

    std::shared_ptr<Image> get_image() const { return this->image; }

    /* 
   *  Changing image and texture to new image.
   *  New image size ought to be the same as old one.
   * */
    void swap_image(std::shared_ptr<Image> new_image)
    {
      image = new_image;
      texture->set_image(new_image);
    }

    glm::vec3 position { 0., 0., 0. };
    glm::vec2 scale { 1., 1. };
    float rotation { 0.0f };

  protected:
    std::shared_ptr<Image> image;
    int frame { 0 };
    const int max_frames { 1 };
    const Size frame_size { 0, 0 };

    void set_shader_uniforms(
      const RenderTarget &, std::shared_ptr<ShaderProgram> &);

    std::shared_ptr<Model> model;
    std::shared_ptr<Texture> texture;
    std::shared_ptr<ShaderProgram> shader_program;

  private:
  };

} // namespace ZD
