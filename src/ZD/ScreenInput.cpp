#include "ScreenInput.hpp"

const MousePosition<long> ScreenMouse::position() const
{
  const double vscl_x = view_size.width() / screen.width;
  const double vscl_y = view_size.height() / screen.height;

  const int mx = position_view_space.x / vscl_x + 0.5;
  const int my = position_view_space.y / vscl_y + 0.5;

  const int sx = (((mx - screen.x / vscl_x) + (screen.width / 2 * screen.scale.x)) -
                  (screen.width / 2)) /
                 screen.scale.x;
  const int sy = (((my - screen.y / vscl_y) + (screen.height / 2 * screen.scale.y)) -
                  (screen.height / 2)) /
                 screen.scale.y;
  return { sx, sy };
}
