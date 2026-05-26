#ifndef SOLAR_SYSTEM_TYPES_H
#define SOLAR_SYSTEM_TYPES_H

#include <glm/glm.hpp>
#include <string>

/**
 * @brief Store the properties of a planet
 */
struct planetProperties {
  float translation;  ///< translation around the sun
  float distance;     ///< distance from the sun
  float rotation;     ///< rotation around its own axis
  float scale;        ///< scale of the planet
};

/**
 * @brief Holds all state information relevant to a character as loaded using FreeType
 */
struct Character {
  unsigned int textureID;  ///< ID handle of the glyph texture
  glm::ivec2 size;         ///< Size of glyph
  glm::ivec2 bearing;      ///< Offset from baseline to left/top of glyph
  unsigned int advance;    ///< Horizontal offset to advance to next glyph
};

/**
 * @brief Struct for planet information
 */
struct planetInfo {
  std::string name;            ///< name of the planet
  std::string distance;        ///< distance from the sun to the planet
  std::string radius;          ///< radius of the planet
  std::string moons;           ///< number of moons the planet has
  std::string rotationPeriod;  ///< rotation period around its own axis
  std::string orbitalPeriod;   ///< orbital period around the sun
};

#endif  // SOLAR_SYSTEM_TYPES_H
