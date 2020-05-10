#pragma once

#include <unordered_map>
#include <vector>
#include <memory>

#include "Sprite.hpp"

enum AnimationLoopOption
{
  NoLoop,
  Loop
};

struct FrameSpan
{
  const int start_frame { 0 };
  const int end_frame { 0 };

  constexpr FrameSpan(const int start_frame, const int end_frame)
  : start_frame { start_frame }
  , end_frame { end_frame }
  {
  }

  constexpr FrameSpan(const int frame)
  : start_frame { frame }
  , end_frame { frame }
  {
  }
};

struct AnimationClip
{
  std::string key {};
  std::shared_ptr<Sprite> sprite { nullptr };
  int start_frame { 0 };
  int end_frame { 0 };
  double length { 0.0 };
  bool loop { true };
};

class Animation
{
public:
  constexpr Animation() = default;

  void add(
    const std::string key, std::shared_ptr<Sprite> sprite,
    const FrameSpan frame_span, double length = 0.0,
    AnimationLoopOption loop_option = Loop);

  void play(const std::string &key, bool force = false);
  void update(const double time_step = 0.03);

  std::shared_ptr<Sprite> get_sprite() const { return current.sprite; }
  const std::string get_current_key() const { return current.key; }

private:
  std::unordered_map<std::string, AnimationClip> clips {};
  AnimationClip current {};
  double animation_time { 0.0 };
};
