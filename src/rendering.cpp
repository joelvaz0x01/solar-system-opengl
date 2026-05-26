#include "rendering.h"

#include <GLFW/glfw3.h>

#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

// Global state for VAOs if we want to keep them local to this translation unit,
// but they are managed in main.cpp for now.
// Let's keep the logic but maybe move the static VAOs here if possible.
static unsigned int sphereVAO = 0;
static GLsizei indexCount;
static unsigned int skyboxVAO = 0;

void renderSphere() {
  if (sphereVAO == 0) { /* first time initializing the sphere */
    glGenVertexArrays(1, &sphereVAO);

    /* vertex buffer object, element buffer object */
    unsigned int vbo, ebo;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    std::vector<glm::vec3> positions;  /* vertices */
    std::vector<glm::vec2> uv;         /* texture coordinates */
    std::vector<glm::vec3> normals;    /* normals */
    std::vector<unsigned int> indices;

    const float radius = 1.0f;  /* radius from center (0,0) */

    /* create sphere */
    for (unsigned int x = 0; x <= STEP; ++x) {
      for (unsigned int y = 0; y <= STEP; ++y) {
        /* calculate the UV coordinates (two-dimensional texture coordinates) */
        float xSegment = (float)x / (float)STEP;  /* u coordinate (horizontal) */
        float ySegment = (float)y / (float)STEP;  /* v coordinate (vertical) */

        /*
         * calculate the position of each vertex (same for normals)
         * see more at: https://mathinsight.org/spherical_coordinates
         */
        float xPos = radius * std::sin(ySegment * PI) * std::cos(xSegment * 2.0f * PI);
        float yPos = radius * std::sin(ySegment * PI) * std::sin(xSegment * 2.0f * PI);
        float zPos = radius * std::cos(ySegment * PI);

        /* add the elements to the end of each vector */
        positions.emplace_back(xPos, yPos, zPos);
        uv.emplace_back(xSegment, ySegment);
        normals.emplace_back(xPos, yPos, zPos);
      }
    }

    /*
     * generate indices
     * see more at: https://opentk.net/learn/chapter1/3-element-buffer-objects.html
     */
    bool oddRow = false;
    for (unsigned int y = 0; y < STEP; ++y) {
      if (!oddRow) {
        /* even rows move left to right */
        for (unsigned int x = 0; x <= STEP; ++x) {
          indices.push_back(y * (STEP + 1) + x);
          indices.push_back((y + 1) * (STEP + 1) + x);
        }
      } else {
        for (int x = STEP; x >= 0; --x) {
          indices.push_back((y + 1) * (STEP + 1) + x);
          indices.push_back(y * (STEP + 1) + x);
        }
      }
      oddRow = !oddRow;
    }

    /* calculate the number of indices (size of indices vector) */
    indexCount = static_cast<GLsizei>(indices.size());

    /* store all the data in one vector (positions, normals and uv) */
    std::vector<float> data;
    for (unsigned int i = 0; i < positions.size(); ++i) {
      data.push_back(positions[i].x);
      data.push_back(positions[i].y);
      data.push_back(positions[i].z);
      if (!normals.empty()) {
        data.push_back(normals[i].x);
        data.push_back(normals[i].y);
        data.push_back(normals[i].z);
      }
      if (!uv.empty()) {
        data.push_back(uv[i].x);
        data.push_back(uv[i].y);
      }
    }

    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    GLsizei stride = (3 + 2 + 3) * sizeof(float); /* 3 for position, 2 for uv, 3 for normal */

    /* vertex attribute */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)nullptr);
    glEnableVertexAttribArray(0);

    /* normal attribute */
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    /* texture attribute */
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

#ifdef _DEBUG
    std::cout << "New sphere created" << std::endl;
#endif
  }
  glBindVertexArray(sphereVAO);

  /*
   * GL_TRIANGLE_STRIP is to ensure that the triangles are all drawn with the same orientation
   * see more at: https://www.khronos.org/opengl/wiki/Primitive#Triangle_primitives
   */
  glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, nullptr);
}

void renderOrbit(float radius, unsigned int* VAO) {
  if (*VAO == 0) {  /* first time initializing the orbit */
    glGenVertexArrays(1, VAO);

    /* vertex buffer object, element buffer object */
    unsigned int vbo;
    glGenBuffers(1, &vbo);

    std::vector<glm::vec3> vertices;

    float angle = 360.0f / STEP;  /* angle between each vertex */

    /* create circle */
    for (unsigned int i = 0; i < STEP; i++) {
      float currentAngle = angle * (float)i;

      /*
       * calculate the position of each vertex
       * see more at: https://faun.pub/draw-circle-in-opengl-c-2da8d9c2c103
       */
      float x = radius * std::cos(glm::radians(currentAngle));
      float y = 0.0f;
      float z = radius * std::sin(glm::radians(currentAngle));

      /* infinite points in the circle correction */
      if (currentAngle == 90.0f || currentAngle == 270.0f) x = 0.0f;
      else if (currentAngle == 0.0f || currentAngle == 180.0f) z = 0.0f;

      /* add the elements to the end of each vector */
      vertices.emplace_back(x, y, z);
    }

    glBindVertexArray(*VAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    /* vertex attribute */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);
#ifdef _DEBUG
    std::cout << "New orbit created" << std::endl;
#endif
  }
  glBindVertexArray(*VAO);
  glDrawArrays(GL_LINE_LOOP, 0, STEP);
}

void renderSkybox(unsigned int skyboxCubeMap) {
  if (skyboxVAO == 0) {  /* first time initializing the skybox */
    float skyboxVertices[] = { /* cube to render skybox */
      /* bottom side */
      -0.5f, -0.5f, -0.5f,
      0.5f, -0.5f, -0.5f,
      0.5f, 0.5f, -0.5f,
      0.5f, 0.5f, -0.5f,
      -0.5f, 0.5f, -0.5f,
      -0.5f, -0.5f, -0.5f,

      /* upper side */
      -0.5f, -0.5f, 0.5f,
      0.5f, -0.5f, 0.5f,
      0.5f, 0.5f, 0.5f,
      0.5f, 0.5f, 0.5f,
      -0.5f, 0.5f, 0.5f,
      -0.5f, -0.5f, 0.5f,

      /* back side */
      -0.5f, 0.5f, 0.5f,
      -0.5f, 0.5f, -0.5f,
      -0.5f, -0.5f, -0.5f,
      -0.5f, -0.5f, -0.5f,
      -0.5f, -0.5f, 0.5f,
      -0.5f, 0.5f, 0.5f,

      /* front side */
      0.5f, 0.5f, 0.5f,
      0.5f, 0.5f, -0.5f,
      0.5f, -0.5f, -0.5f,
      0.5f, -0.5f, -0.5f,
      0.5f, -0.5f, 0.5f,
      0.5f, 0.5f, 0.5f,

      /* left side */
      -0.5f, -0.5f, -0.5f,
      0.5f, -0.5f, -0.5f,
      0.5f, -0.5f, 0.5f,
      0.5f, -0.5f, 0.5f,
      -0.5f, -0.5f, 0.5f,

      /* right side */
      -0.5f, 0.5f, -0.5f,
      0.5f, 0.5f, -0.5f,
      0.5f, 0.5f, 0.5f,
      0.5f, 0.5f, 0.5f,
      -0.5f, 0.5f, 0.5f,
      -0.5f, 0.5f, -0.5f,
    };

    /* vertex buffer object */
    unsigned int vbo;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &vbo);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    /* position attribute */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);
  }
  glDepthFunc(GL_LEQUAL);

  glBindVertexArray(skyboxVAO);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubeMap);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);

  glDepthFunc(GL_LESS);  /* reset depth function to default */
}

glm::mat4 planetCreator(float translation, float distance, float rotation, float scale, glm::vec3 centerModel) {
  glm::mat4 model = glm::translate(glm::mat4(1.0f), centerModel);
  model = glm::rotate(model, (float)glfwGetTime() * translation, glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::translate(model, glm::vec3(0.0f, 0.0f, distance));
  model = glm::rotate(model, (float)glfwGetTime() * rotation, glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::scale(model, glm::vec3(scale));
  return model;
}
