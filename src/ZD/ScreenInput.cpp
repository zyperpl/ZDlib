#include "ScreenInput.hpp"

inline const MousePosition<long> ScreenMouse::position() const
{
  const int mx = position_view_space.x;
  const int my = position_view_space.y;

  const int sx = (((mx - screen.x) + (screen.width / 2 * screen.scale.x)) -
                  (screen.width / 2)) /
                 screen.scale.x;
  const int sy = (((my + screen.y) + (screen.height / 2 * screen.scale.y)) -
                  (screen.height / 2)) /
                 screen.scale.y;
  return { sx, sy };
}
