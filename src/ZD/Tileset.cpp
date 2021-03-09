#include <vector>

#include "Tileset.hpp"

namespace ZD
{
  Tileset::Tileset(
    std::shared_ptr<Image> source, int tile_width, int tile_height)
  : source { std::move(source) }
  , tile_width { tile_width }
  , tile_height { tile_height }
  {
  }

  void Tileset::draw_tile(int x, int y, int id_x, int id_y, Painter &p)
  {
    if (id_x * tile_width >= source->width())
      return;
    if (id_y * tile_height >= source->height())
      return;
    if (id_x < 0)
      return;
    if (id_y < 0)
      return;

    const auto img_w = source->width();
    const auto source_data =
      source->get_data() + (id_x * tile_width + (id_y * tile_height) * img_w);

    for (ssize_t i = 0; i < tile_width * tile_height; ++i)
    {
      const int px = i % tile_width;
      const int py = i / tile_width;
      const auto color = source_data[px + py * img_w];
      p.set_pixel(x + px, y + py, color);
    }
  }

  void Tileset::draw_tiles(const Tilemap &map, Painter &p)
  {
    for (const auto &idx_tile : map.tiles)
    {
      const Tile &tile = idx_tile.second;
      draw_tile(
        tile.position.x * tile_width,
        tile.position.y * tile_height,
        tile.index.x,
        tile.index.y,
        p);
    }
  }

  Painter TilesetImageWrapper::painter;

  bool TilesetImageWrapper::redraw()
  {
    const auto &redraw_mask = tilemap->get_redraw_mask();
    if (redraw_mask.not_set)
      return false;

    for (auto &key_image : images)
    {
      const auto &key = key_image.first;
      const auto &image = key_image.second;

      const auto &position = key_to_image_position(key);

      const long x_offset = position.x * image_width();
      const long y_offset = position.y * image_height();

      const int x1 = redraw_mask.min_x * tileset->tile_width;
      const int y1 = redraw_mask.min_y * tileset->tile_height;
      const int x2 = redraw_mask.max_x * tileset->tile_width;
      const int y2 = redraw_mask.max_y * tileset->tile_height;

      TilesetImageWrapper::painter.set_target(image);
      TilesetImageWrapper::painter.clear_rectangle(
        x1 - x_offset, y1 - y_offset, x2 - x_offset, y2 - y_offset);
    }

    std::unordered_set<int64_t> nonempty_images;

    for (const auto &idx_tile : tilemap->tiles)
    {
      const Tile &tile = idx_tile.second;
      auto image_position = grid_position_to_image_position(tile.position);
      auto key = image_key(image_position);
      auto image = image_for_key(key);
      nonempty_images.insert(key);

      if (!redraw_mask.contains(tile.position.x, tile.position.y))
        continue;

      const long x_offset = image_position.x * image_width();
      const long y_offset = image_position.y * image_height();
      //printf("tile=%p; image=%p\n", &tile, &image);
      TilesetImageWrapper::painter.set_target(image);
      tileset->draw_tile(
        tile, x_offset, y_offset, TilesetImageWrapper::painter);
    }

    for (auto it = images.begin(); it != images.end();)
    {
      const auto &key = it->first;
      if (nonempty_images.find(key) == nonempty_images.end())
      {
        it = images.erase(it);
      }
      else
      {
        it++;
      }
    }

    tilemap->reset_redraw_mask();
    return true;
  }

  void TilesetImageWrapper::draw(Painter &painter)
  {
    for (const auto &key_image : images)
    {
      const auto &key = key_image.first;
      const auto &image = key_image.second;

      const auto pos = key_to_image_position(key);

      //printf("key=%lu; image=%p; pos=%d,%d\n", key, &image, pos.x, pos.y);
      painter.draw_image(pos.x * image_width(), pos.y * image_height(), *image);
    }
    //printf("Tiles::draw of %lu. images\n", images.size());
  }

} // namespace ZD
