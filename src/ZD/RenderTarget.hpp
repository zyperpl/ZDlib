#pragma once

class RenderTarget
{
public:
  virtual int get_width() const = 0;
  virtual int get_height() const = 0;
};
