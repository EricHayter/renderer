#ifndef H_RENDERER
#define H_RENDERER

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include "model.h"
#include "vector.h"

constexpr int SCREEN_WIDTH = 900;
constexpr int SCREEN_HEIGHT = 900;
constexpr int DEPTH = 900;

// Color struct with 4 8-bit channels.
struct Color {
    int r, g, b, a;
};

// 2D point struct using floats
struct Point2D {
    float x, y;
};

struct VertexPair {
    Vector<3> pos;
    Vector<3> norm;
};

using Triangle = std::array<VertexPair, 3>;

/* Renderer Singleton Class
 *
 * The renderer singleton is responsible for the underlying SDL renderer and
 * window handles.
 */
class Renderer {
   public:
    // Singleton instance getter
    static Renderer* GetRenderer();

    // blacks out the entire screen and resets the value of z-buffer
    void clear_screen();

    // draws a point of given color on the screen
    void draw_point(int x, int y, const Color& clr);

    // render a triangular face with appropriate shading and coloring using
    // Phong shading.
    void draw_face(const Triangle& v1, const Color& clr);

    // render the given model
    void draw_model(const Model& model);

    // The Renderer should not be cloneable or assignable (singleton)
    Renderer(Renderer& other) = delete;
    void operator=(const Renderer&) = delete;

    // public fields
    Vector<3> light_dir{0.f, 0.f, -1.f};
    Vector<3> pos{0.f, 0.f, 255.f};
    float yaw{};
    float pitch{};

   private:
    Renderer();
    ~Renderer();
    static Renderer* renderer_;
    SDL_Renderer* sdl_renderer_{};
    SDL_Window* window_{};

    // zbuffer allows for keeping track of "layers" when printing multiple
    // colors at the same (x,y) pairs put different depths relative to the
    // camera.
    std::array<std::array<float, SCREEN_HEIGHT>, SCREEN_WIDTH> zbuffer_{};
};

bool InsideTriangle(const Triangle& triangle, float x, float y);

// given 3 points to define a plane return a function that finds a solution
// on the plane given some parameter of a point (x, y)
float GetDepthOnFace(const Triangle& triangle, float x, float y);

float triangleArea(const Triangle& triangle);
float triangleArea(const Vector<3>& v1,
                   const Vector<3>& v2,
                   const Vector<3>& v3);

// given a plane with 3 verticies with a normal for each vertex
// return a function to interpolate a normal for a given point (x,y) on the
// plane
Vector<3> findNormalSolution(const Triangle& triangle, float x, float y);

#endif
