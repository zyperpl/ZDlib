#include "Animation.hpp"

void Animation::add(
  const std::string key, std::shared_ptr<Sprite> sprite,
  const FrameSpan frame_span, double length, AnimationLoopOption loop_option)
{
  /*
  printf(
    "%s: %p %d-%d %f %d\n",
    key.c_str(),
    &sprite,
    frame_span.start_frame,
    frame_span.end_frame,
    length,
    loop_option);
  // */

  AnimationClip clip;
  clip.key = key;
  clip.sprite = sprite;
  clip.start_frame = frame_span.start_frame;
  clip.end_frame = frame_span.end_frame;
  clip.length = length;
  clip.loop = loop_option == Loop;
  clips.insert(std::make_pair(key, std::move(clip)));

  if (!current.sprite)
  {
    current.sprite = sprite;
  }
}

void Animation::play(const std::string &key, bool force)
{
  if (clips.find(key) == clips.end())
  {
    assert(false);
    fprintf(stderr, "Animation key %s not found!\n", key.c_str());
    return;
  }

  if (current.key != key || force)
  {
    current = clips.at(key);
    animation_time = current.length;
    if (current.sprite)
    {
      current.sprite->set_frame(current.start_frame);
    }
  }
}

void Animation::update(const double time_step)
{
  if (current.length <= 0)
    return;

  if (current.sprite)
  {
    auto frame = current.sprite->get_frame();
    if (frame < current.start_frame || frame > current.end_frame)
    {
      frame = current.start_frame;
      animation_time = -0.0;
    }
  }

  if (animation_time <= 0.0)
  {
    animation_time = current.length;
    if (current.sprite)
    {
      auto frame = current.sprite->get_frame();
      if (!current.loop && frame == current.end_frame) {}
      else
      {
        frame = (frame + 1) % (current.end_frame + 1);
        if (frame < current.start_frame)
          frame = current.start_frame;
        current.sprite->set_frame(frame);
      }
    }
  }
  else
  {
    animation_time -= time_step;
  }
}
