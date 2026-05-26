#include "text_renderer.h"

#include <ft2build.h>
#include <glad/gl.h>

#include <iostream>
#include FT_FREETYPE_H
#include "constants.h"
#include "planet_data.h"

static std::map<char, Character> Characters;
static unsigned int textVAO, textVBO;

int initTextRenderer(const std::string& fontPath) {
  /* load FreeType */
  FT_Library ft;
  if (FT_Init_FreeType(&ft)) {
    std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    return -1;
  }

  /* load font */
  FT_Face face;
  if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
    std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
    FT_Done_FreeType(ft);
    return -1;
  }

  /* set size to load glyphs as */
  FT_Set_Pixel_Sizes(face, 0, 48);

  /* disable byte-alignment restriction */
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  /* load the first 128 characters of ASCII set */
  for (unsigned char c = 0; c < 128; c++) {
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      std::cerr << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
      continue;
    }

    /* generate texture */
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RED,
      (GLsizei)face->glyph->bitmap.width,
      (GLsizei)face->glyph->bitmap.rows,
      0,
      GL_RED,
      GL_UNSIGNED_BYTE,
      face->glyph->bitmap.buffer
    );

    /* set texture options */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* store character for later use */
    Character character = {
      texture,
      glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
      glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
      static_cast<unsigned int>(face->glyph->advance.x)
    };
    Characters.insert(std::pair<char, Character>(c, character));
  }
  glBindTexture(GL_TEXTURE_2D, 0);

  /* destroy FreeType once we're finished */
  FT_Done_Face(face);
  FT_Done_FreeType(ft);

  /* configure textVAO/textVBO for texture quads */
  glGenVertexArrays(1, &textVAO);
  glGenBuffers(1, &textVBO);
  glBindVertexArray(textVAO);
  glBindBuffer(GL_ARRAY_BUFFER, textVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)nullptr);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  return 0;
}

void renderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color) {
  shader.use();
  shader.setVec3("textColor", color.x, color.y, color.z);
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(textVAO);

  /* iterate through all characters */
  for (std::string::const_iterator c = text.begin(); c != text.end(); c++) {
    Character ch = Characters[*c];

    float x_pos = x + (float)ch.bearing.x * scale;
    float y_pos = y - (float)(ch.size.y - ch.bearing.y) * scale;

    float w = (float)ch.size.x * scale;
    float h = (float)ch.size.y * scale;

    float vertices[6][4] = {  /* 2 for position, 2 for texture */
      {x_pos,     y_pos + h, 0.0f, 0.0f},  /* bottom left */
      {x_pos,     y_pos,     0.0f, 1.0f},  /* top left */
      {x_pos + w, y_pos,     1.0f, 1.0f},  /* top right */
      {x_pos,     y_pos + h, 0.0f, 0.0f},  /* bottom left */
      {x_pos + w, y_pos,     1.0f, 1.0f},  /* top right */
      {x_pos + w, y_pos + h, 1.0f, 0.0f}   /* bottom right */
    };

    /* render glyph texture over quad */
    glBindTexture(GL_TEXTURE_2D, ch.textureID);

    /* update content of VBO memory */
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);

    /* NOTE: use glBufferSubData and not glBufferData */
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /* render quad */
    glDrawArrays(GL_TRIANGLES, 0, 6);

    /*
     * advance cursors for the next glyph (NOTE: advance is number of 1/64 pixels)
     * 2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels)
     */
    x += (float)(ch.advance >> 6) * scale;  /* bitshift by 6 to get value in pixels */
  }
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

float charHeightScaled(float scale, bool isMaxHeight) {
  /* correction to apply when the character is not at the top of the screen */
  if (isMaxHeight) return HEIGHT - CHAR_HEIGHT_UP * scale;
  return CHAR_HEIGHT_DOWN * scale;
}

float charWidthScaled(float scale, std::string::size_type textLength, bool isMaxWidth) {
  /* correction to apply when the character is not at the top of the screen */
  if (isMaxWidth) return WIDTH - static_cast<float>(textLength) * CHAR_WIDTH_UP * scale;
  return CHAR_WIDTH_DOWN * scale;
}

void showPlanetInfo(Shader& shader, unsigned int planetIndex, glm::vec3 textColor, float textScale) {
  std::string planetInfoText[] = {
    "Name: " + planetsData[planetIndex].name,
    "Distance: " + planetsData[planetIndex].distance,
    "Radius: " + planetsData[planetIndex].radius,
    "Moons number: " + planetsData[planetIndex].moons,
    "Rotation duration: " + planetsData[planetIndex].rotationPeriod,
    "Translation duration: " + planetsData[planetIndex].orbitalPeriod,
  };

  for (int i = 0; i < 6; i++) {
    renderText(
      shader,
      planetInfoText[i],
      charWidthScaled(textScale, planetInfoText[i].length(), false),
      charHeightScaled(textScale, true) - ((float)i * 50.0f),
      textScale,
      textColor
    );
  }
}

void cleanupTextRenderer() {
  glDeleteVertexArrays(1, &textVAO);
  glDeleteBuffers(1, &textVBO);
  for (auto const& it : Characters)
    glDeleteTextures(1, &it.second.textureID);
  Characters.clear();
}
