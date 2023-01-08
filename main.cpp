#include <climits>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <optional>
#include <random>
#include <vector>

#include "vec2.hpp"

enum cell_state { wall, empty, calculated, flagged, path };

class cell {
public:
  cell_state state;
  unsigned int source;
  int fcost;

  cell(cell_state __state) { state = __state; }
};
std::ostream &operator<<(std::ostream &os, const cell &cell) {
  switch (cell.state) {
  case cell_state::wall:
    os << '#';
    break;
  case cell_state::empty:
    os << '.';
    break;
  case cell_state::calculated:
    os << '-';
    break;
  case cell_state::flagged:
    os << '+';
    break;
  case cell_state::path:
    os << 'p';
    break;
  }
  return os;
}

class world {
public:
  std::vector<cell> cells;
  unsigned int width;
  unsigned int height;
  vec2 start;
  vec2 end;

  world(unsigned int __width, unsigned int __height) {
    cells.reserve(__width * __height);
    width = __width;
    height = __height;
    start = vec2(0, 0);
    end = vec2(__width - 1, __height - 1);
  }

  void populate() {
    std::random_device rd;  // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, 11); // define the range

    for (int cy = 0; cy < height; cy++)
      for (int cx = 0; cx < width; cx++) {
        cell_state cell_type = cell_state::empty;
        if ((cy == start.y && cx == start.x) ||
            (cy == start.y && cx == start.x)) {
          cell_type = cell_state::empty;
        } else {
          if (distr(gen) > 9) {
            cell_type = cell_state::wall;
          } else {
            cell_type = cell_state::empty;
          }
        }

        cells.push_back(cell(cell_type));
      }
  }
  unsigned int index_from_coord(unsigned int x, unsigned int y) {
    return x + (y * width);
  }
  unsigned int index_from_vec2(vec2 t) { return index_from_coord(t.x, t.y); }
  vec2 coord_from_index(unsigned int index) {
    unsigned int x = index % width;
    unsigned int y = (index - x) / width;
    return vec2(x, y);
  }
  void calculate(std::vector<unsigned int> *calculated, vec2 target_pos,
                 vec2 source_pos) {
    unsigned int source_id = index_from_vec2(source_pos);
    unsigned int target_index = index_from_vec2(target_pos);

    if (cells[target_index].state == cell_state::empty) {
      auto gcost = get_dist(target_pos, start);
      auto hcost = get_dist(target_pos, end);
      auto fcost = gcost + hcost;

      cells[target_index].fcost = fcost;
      cells[target_index].state = cell_state::calculated;
      cells[target_index].source = source_id;

      (*calculated).push_back(target_index);
    }
  }
  void calculate_surrounding(std::vector<unsigned int> *calculated,
                             unsigned int x, unsigned int y) {
    if (x > 0)
      calculate(calculated, vec2(x - 1, y), vec2(x, y));

    if (x < width - 1)
      calculate(calculated, vec2(x + 1, y), vec2(x, y));

    if (y > 0)
      calculate(calculated, vec2(x, y - 1), vec2(x, y));

    if (y < height - 1)
      calculate(calculated, vec2(x, y + 1), vec2(x, y));

    // diagonal
    if (x > 0 && y > 0)
      calculate(calculated, vec2(x - 1, y - 1), vec2(x, y));

    if (x < width - 1 && y < height - 1)
      calculate(calculated, vec2(x + 1, y + 1), vec2(x, y));

    if (x > 0 && y < height - 1)
      calculate(calculated, vec2(x - 1, y + 1), vec2(x, y));

    if (x < width - 1 && y > 0)
      calculate(calculated, vec2(x + 1, y - 1), vec2(x, y));
  }
  std::optional<unsigned int> flag(std::vector<unsigned int> *calculated,
                                   std::vector<unsigned int> *flagged) {
    int min_cost = INT_MAX;
    std::optional<unsigned int> min_index;
    std::optional<unsigned int> min_source;
    for (auto &cell_index : *calculated) {
      auto cell = cells[cell_index];
      if (cell.state == cell_state::calculated) {
        if (cell.fcost < min_cost) {
          min_cost = cell.fcost;
          min_index = cell_index;
          min_source = cell.source;
        }
      }
    }

    if (min_index) {
      if (min_source) {
        cells[*min_index].state = cell_state::flagged;
        cells[*min_index].source = *min_source;
        (*flagged).push_back(*min_index);
        return *min_index;
      }
    }

    return std::nullopt;
  }
};
std::ostream &operator<<(std::ostream &os, const world &w) {
  int i = 0;
  for (auto &it : w.cells) {
    os << it;
    if ((i + 1) % w.width == 0)
      os << '\n';

    i++;
  }
  return os;
}

int main() {
  std::cout << "hello world\n";

  world w = world(50, 10);
  w.populate();

  std::cout << "\x1B[2J" << w;

  std::vector<unsigned int> calculated;
  std::vector<unsigned int> flagged;

  w.calculate_surrounding(&calculated, w.start.x, w.start.y);
  std::optional<unsigned int> flagged_id = w.flag(&calculated, &flagged);

  for (;;) {
    if (flagged_id) {
      if (*flagged_id == w.index_from_vec2(w.end))
        break;

      auto coord = w.coord_from_index(*flagged_id);
      w.calculate_surrounding(&calculated, coord.x, coord.y);

      flagged_id = w.flag(&calculated, &flagged);
    }
  }

  // trace source
  if (flagged_id) {
    std::vector<unsigned int> path;
    unsigned int checked_id = *flagged_id;
    for (;;) {
      auto cell = w.cells[checked_id];
      if (cell.state == cell_state::flagged) {
        path.push_back(checked_id);
        w.cells[checked_id].state = cell_state::path;
        checked_id = cell.source;
      } else {
        break;
      }
    }
  }

  std::cout << "\x1B[2J" << w;

  return 0;
}