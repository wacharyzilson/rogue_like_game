// Code in this file was written by proff Jeremy Sheaffer for reading
// in monster and item descriptions

// Contact @ sheaffer@iastate.edu

#include "dice.h"
#include "structures.h"
#include "dungeon_generator.h"

int32_t dice::roll(void) const
{
  int32_t total;
  uint32_t i;

  total = base;

  if (sides) {
    for (i = 0; i < number; i++) {
      total += random_number(1, sides);
    }
  }

  return total;
}

std::ostream &dice::print(std::ostream &o)
{
  return o << base << '+' << number << 'd' << sides;
}

std::ostream &operator<<(std::ostream &o, dice &d)
{
  return d.print(o);
}
