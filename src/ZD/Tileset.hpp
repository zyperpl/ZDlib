#pragma once

#include <climits>
#include <functional>
#include <list>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "Image.hpp"
#include "Painter.hpp"

struct Tile;
class Tilemap;
class Tileset;
class TilesetImageWrapper;

struct TileGridPosition
{
  TileGridPosition(int32_t x, int32_t y)
  : x { x }
  , y { y }
  {
  }
  int32_t x;
  int32_t y;
};

struct TileIndex
{
  TileIndex(uint8_t x, uint8_t y)
  : x { x }
  , y { y }
  {
  }
  uint8_t x;
  uint8_t y;

  bool operator==(const TileIndex &o) const { return o.x == x && o.y == y; }
  bool operator!=(const TileIndex &o) const { return o.x != x || o.y != y; }
};

struct TileIndexRange
{
  TileIndexRange(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
  : x1 { x1 }
  , y1 { y1 }
  , x2 { x2 }
  , y2 { y2 }
  {
  }

  TileIndexRange(TileIndex idx1, TileIndex idx2)
  : x1 { idx1.x }
  , y1 { idx1.y }
  , x2 { idx2.x }
  , y2 { idx2.y }
  {
  }
  uint8_t x1, y1;
  uint8_t x2, y2;

  bool contains(const TileIndex &idx) const
  {
    if (idx.x >= x1 && idx.x <= x2 && idx.y >= y1 && idx.y <= y2) return true;

    return false;
  }
};

struct Tile
{
  Tile(TileGridPosition position, TileIndex index)
  : position { position }
  , index { index }
  {
  }
  Tile(uint16_t grid_x, uint16_t grid_y, uint8_t idx_x, uint8_t idx_y)
  : position { grid_x, grid_y }
  , index { idx_x, idx_y }
  {
  }
  TileGridPosition position;
  TileIndex index;
};

struct TilesMask
{
  bool not_set { false };
  int min_x;
  int max_x;
  int min_y;
  int max_y;

  void set_minmax(int x, int y)
  {
    if (x <= min_x) min_x = x;
    if (y <= min_y) min_y = y;
    if (x >= max_x) max_x = x + 1;
    if (y >= max_y) max_y = y + 1;
    not_set = false;
  }

  void reset()
  {
    not_set = true;
    min_x = INT_MAX;
    min_y = INT_MAX;
    max_x = INT_MIN;
    max_y = INT_MIN;
  }

  bool contains(int x, int y) const
  {
    if (x >= min_x && x <= max_x && y >= min_y && y <= max_y) return true;

    return false;
  }
};

class Tilemap
{
public:
  inline int64_t key(const int32_t x, const int32_t y) const
  {
    return x + y * TILES_H;
  }

  void remove(const int32_t x, const int32_t y)
  {
    //printf("remove %d %d;key = %ld\n", x, y, key(x, y));
    auto it = tiles.find(key(x, y));
    if (it == tiles.end()) return;

    tiles.erase(it);

    rules_update(x, y);

    set_redraw(x, y);
  }

  void insert(Tile tile)
  {
    const int32_t x = tile.position.x;
    const int32_t y = tile.position.y;
    //printf("insert %d %d; key = %ld\n", x, y, key(x, y));
    if (x < 0) return;
    if (y < 0) return;

    if (tiles.find(key(x, y)) != tiles.end()) { remove(x, y); }
    tiles.insert(std::make_pair(key(x, y), tile));

    rules_update(x, y);

    set_redraw(x, y);
  }

  std::optional<Tile> get(const int32_t x, const int32_t y) const
  {
    auto t = tiles.find(key(x, y));
    if (t == tiles.end()) return {};

    return t->second;
  }

  typedef uint16_t SpatialMask;

  enum Spatial : SpatialMask
  {
    TopLeft = 0x1,
    TopMiddle = 0x2,
    TopRight = 0x4,
    CenterLeft = 0x8,
    CenterRight = 0x10,
    BottomLeft = 0x20,
    BottomMiddle = 0x40,
    BottomRight = 0x80
  };

  SpatialMask neighborhood(int32_t x, int32_t y) const
  {
    SpatialMask v = 0x0;
    if (get(x - 1, y - 1)) v |= TopLeft;
    if (get(x, y - 1)) v |= TopMiddle;
    if (get(x + 1, y - 1)) v |= TopRight;
    if (get(x - 1, y)) v |= CenterLeft;
    if (get(x + 1, y)) v |= CenterRight;
    if (get(x - 1, y + 1)) v |= BottomLeft;
    if (get(x, y + 1)) v |= BottomMiddle;
    if (get(x + 1, y + 1)) v |= BottomRight;
    return v;
  }

  const TilesMask get_redraw_mask() { return redraw_mask; }
  void reset_redraw_mask() { redraw_mask.reset(); }

  void add_rule(
    TileIndexRange range,
    std::function<
      TileIndex(const TileIndex &, const TileGridPosition &, const Tilemap &)>
      func)
  {
    rules.push_back({ range, func });
  };

protected:
  TilesMask redraw_mask;

  void set_redraw(int32_t x, int32_t y)
  {
    redraw_mask.set_minmax(x, y);
  }

  std::optional<TileIndex> rules_index(const Tile &tile)
  {
    for (const auto &range_rule : rules)
    {
      const auto &range = range_rule.first;
      const auto &rule = range_rule.second;

      if (range.contains(tile.index))
      { return rule(tile.index, tile.position, *this); }
    }
    return {};
  }

  bool rules_update(int32_t x, int32_t y)
  {
    if (rules.empty()) return false;

    bool ret = false;
    for (int iy = -1; iy <= 1; iy++)
    {
      for (int ix = -1; ix <= 1; ix++)
      {
        if (auto t = get(x + ix, y + iy))
        {
          if (auto new_index = rules_index(t.value()))
          {
            if (t->index != new_index.value())
            {
              //printf("index %d.%d to %d.%d\n", t->index.x, t->index.y, new_index->x, new_index->y);
              tiles.at(key(x + ix, y + iy)).index = new_index.value();
              set_redraw(x + ix, y + iy);
              ret = true;
            }
          }
        }
      }
    }

    return ret;
  }

  const uint32_t TILES_H { 32768 };
  std::unordered_map<int64_t, Tile> tiles;

  std::list<std::pair<
    TileIndexRange,
    std::function<TileIndex(
      const TileIndex &, const TileGridPosition &, const Tilemap &)>>>
    rules;

  friend class TilesetImageWrapper;
  friend class Tileset;
  friend class TilesetRenderer;
};

struct TilesImagePosition
{
  int x;
  int y;
};

class Tileset
{
public:
  Tileset(std::shared_ptr<Image> source, int tile_width, int tile_height);

  inline int vertical_num() const { return source->width() / tile_width; }
  inline int horizontal_num() const { return source->height() / tile_height; }

  void draw_tiles(const Tilemap &, Painter &);
  void draw_tile(
    const Tile &tile, const int x_offset, const int y_offset, Painter &p)
  {
    draw_tile(
      -x_offset + tile.position.x * tile_width,
      -y_offset + tile.position.y * tile_height,
      tile.index.x,
      tile.index.y,
      p);
  }
  void draw_tile(int x, int y, int id_x, int id_y, Painter &);

  inline int get_tile_width() { return tile_width; }
  inline int get_tile_height() { return tile_height; }

  std::shared_ptr<Image> get_source() { return source; }

private:
  std::shared_ptr<Image> source;
  int tile_width { 0 };
  int tile_height { 0 };

  friend class TilesetImageWrapper;
  friend class TilesetRenderer;
};

class TilesetImageWrapper
{
public:
  TilesetImageWrapper(std::shared_ptr<Tileset> tileset, std::shared_ptr<Tilemap> tilemap)
  : tileset { tileset }
  , tilemap { tilemap }
  {
  }
  std::shared_ptr<Tilemap> get_tilemap() const { return tilemap; }
  std::shared_ptr<Tileset> get_tileset() const { return tileset; }

  bool redraw();
  void draw(Painter &painter);

private:
  int image_width() { return tileset->get_tile_width() * H_TILES_PER_IMAGE; }
  int image_height() { return tileset->get_tile_height() * V_TILES_PER_IMAGE; }
  int64_t image_key(int x, int y) { return x + H_IMAGES * y; }
  int64_t image_key(TilesImagePosition p) { return p.x + H_IMAGES * p.y; }
  TilesImagePosition key_to_image_position(int64_t key)
  {
    return { (int)(key % H_IMAGES), (int)(key / H_IMAGES) };
  }
  TilesImagePosition grid_position_to_image_position(int x, int y)
  {
    return { (int)(x / H_TILES_PER_IMAGE), (int)(y / V_TILES_PER_IMAGE) };
  }
  TilesImagePosition grid_position_to_image_position(TileGridPosition grid_p)
  {
    return grid_position_to_image_position(grid_p.x, grid_p.y);
  }
  std::shared_ptr<Image> image_for_tile(const Tile &tile)
  {
    auto key = image_key(grid_position_to_image_position(tile.position));
    return image_for_key(key);
  }
  std::shared_ptr<Image> image_for_key(int64_t key)
  {
    auto ptr = images.find(key);
    if (ptr == images.end())
    {
      auto new_image =
        Image::create(Size(image_width(), image_height()), PixelFormat::RGBA);
      images.insert({ key, new_image });
      return new_image;
    }
    return images.at(key);
  }
  std::unordered_map<int64_t, std::shared_ptr<Image>> images;
  std::shared_ptr<Tileset> tileset;
  std::shared_ptr<Tilemap> tilemap;

  static Painter painter;
  const uint32_t H_TILES_PER_IMAGE = 20 / 2;
  const uint32_t V_TILES_PER_IMAGE = 16 / 4;
  const uint32_t H_IMAGES = 8192;
  friend class TilesetRenderer;
};
