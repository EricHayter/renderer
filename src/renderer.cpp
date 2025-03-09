#include "renderer.h"
#include <SDL2/SDL_render.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <limits>
#include <memory>
#include "model.h"
#include "vector.h"

//=============================================================================
// Renderer Object
//=============================================================================

Renderer* Renderer::renderer_ = nullptr;

Renderer* Renderer::GetRenderer() {
    if (renderer_ == nullptr) {
        renderer_ = new Renderer();
    }
    return renderer_;
}

Renderer::Renderer() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
        exit(-1);
    }

    window_ = SDL_CreateWindow("renderer", SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                               SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (window_ == NULL) {
        fprintf(stderr, "could not create window: %s\n", SDL_GetError());
        exit(-1);
    }

    sdl_renderer_ = SDL_CreateRenderer(window_, -1, 0);

    // set the zbuffer entries to be as far back as possible
    for (std::size_t i = 0; i < zbuffer_.size(); i++) {
        for (std::size_t j = 0; j < zbuffer_[i].size(); j++) {
            zbuffer_[i][j] = -std::numeric_limits<float>::max();
        }
    }
}

Renderer::~Renderer() {
    SDL_DestroyWindow(window_);
    SDL_Quit();
    delete renderer_;
}

//=============================================================================
// Utility Functions
//=============================================================================
void Renderer::clear_screen() {
    // make sure render color is black before clearing
    SDL_SetRenderDrawColor(sdl_renderer_, 0, 0, 0, 0);

    SDL_RenderClear(sdl_renderer_);
    for (std::size_t i = 0; i < zbuffer_.size(); i++) {
        for (std::size_t j = 0; j < zbuffer_[i].size(); j++) {
            zbuffer_[i][j] = -std::numeric_limits<float>::max();
        }
    }
}

void Renderer::draw_point(unsigned int x, unsigned int y, const Color& clr) {
    SDL_SetRenderDrawColor(sdl_renderer_, clr.r, clr.g, clr.b, clr.a);
    SDL_RenderDrawPoint(sdl_renderer_, static_cast<int>(x),
                        static_cast<int>(y));
}

//=============================================================================
// Rendering Models
//=============================================================================
void Renderer::draw_model(const Model& model) {
    Vector<3> z{view_vector(yaw, pitch).normalize()};      // back-forward vec
    Vector<3> x{cross_product({0, 1, 0}, z).normalize()};  // left-right vec
    Vector<3> y{cross_product(z, x).normalize()};          // up-down vec

    Matrix<4, 4> modelView{{x[X], x[Y], x[Z], pos[0]},
                           {y[X], y[Y], y[Z], pos[1]},
                           {z[X], z[Y], z[Z], pos[2]},
                           {0.f, 0.f, 0.f, 1.f}};

    // let's create our transformation matrix here
    Matrix<4, 4> projMatrix{{1.f, 0.f, 0.f, 0.f},
                            {0.f, 1.f, 0.f, 0.f},
                            {0.f, 0.f, 1.f, 0.f},
                            {0.f, 0.f, 1 / pos[Z], 1.f}};

    // this will scale our points to appropriate sizes for our screen
    Matrix<4, 4> viewPort{
        {SCREEN_WIDTH / 2.f, 0, 0, SCREEN_WIDTH / 2.f},
        {0, -static_cast<int>(SCREEN_HEIGHT) / 2.f, 0, SCREEN_HEIGHT / 2.f},
        {0, 0, DEPTH / 2.f, DEPTH / 2.f},
        {0, 0, 0, 1.f}};

    Matrix<4, 4> transMatrix{viewPort * projMatrix * modelView};
    Matrix<4, 4> normalTransMatrix{inverse(transMatrix).transpose()};

    for (int i = 0; i < model.nfaces(); i++) {
        std::vector<FaceTuple> face = model.face(i);

        // get v1 and transform it
        Vector<3> v1{model.vertex(face[0].vertex)};
        v1 = Vector<4>(transMatrix * v1.homogenize()).dehomogenize();

        // get normal and transform it
        Vector<3> v1n{model.normal(face[0].normal)};
        v1n = Vector<4>(normalTransMatrix * v1n.homogenize())
                  .dehomogenize()
                  .normalize();

        // triangle fan the face polygon (most of the time this is just a
        // triangle.
        for (std::size_t j = 2; j < face.size();
             j++) {  // outer loop for start points
            // get v2 and transform it
            Vector<3> v2{model.vertex(face[j - 1].vertex)};
            v2 = Vector<4>(transMatrix * v2.homogenize()).dehomogenize();

            Vector<3> v2n{model.normal(face[j - 1].normal)};
            v2n = Vector<4>(normalTransMatrix * v2n.homogenize())
                      .dehomogenize()
                      .normalize();

            // get v3 and transform it
            Vector<3> v3{model.vertex(face[j].vertex)};
            v3 = Vector<4>(transMatrix * v3.homogenize()).dehomogenize();

            Vector<3> v3n{model.normal(face[j].normal)};
            v3n = Vector<4>(normalTransMatrix * v3n.homogenize())
                      .dehomogenize()
                      .normalize();
            Triangle triangle{{
                {v1, v1n},
                {v2, v2n},
                {v3, v3n},
            }};
            draw_face(triangle, {255, 255, 255, 255});
        }
    }
    SDL_RenderPresent(sdl_renderer_);
}

void Renderer::draw_face(const Triangle& triangle, const Color& clr) {
    Vector<3> v1 = triangle[0].pos;
    Vector<3> v2 = triangle[1].pos;
    Vector<3> v3 = triangle[2].pos;

    auto solFunc{findPlaneSolution(v1, v2, v3)};

    // create a bounding box around the triangle on the screen
    unsigned int maxX =
        std::min({static_cast<unsigned int>(std::max({v1[X], v2[X], v3[X]})),
                  SCREEN_WIDTH - 1});
    unsigned int minX = std::max(
        {static_cast<unsigned int>(std::min({v1[X], v2[X], v3[X]})), 0u});
    unsigned int maxY =
        std::min({static_cast<unsigned int>(std::max({v1[Y], v2[Y], v3[Y]})),
                  SCREEN_HEIGHT - 1});
    unsigned int minY = std::max(
        {static_cast<unsigned int>(std::min({v1[Y], v2[Y], v3[Y]})), 0u});

    for (unsigned int x = minX; x <= maxX; x++) {
        for (unsigned int y = minY; y <= maxY; y++) {
            if (not InsideTriangle(triangle, static_cast<float>(x),
                                   static_cast<float>(y)))
                continue;
            float z{solFunc(x, y)};
            if (z >= zbuffer_[x][y]) {
                zbuffer_[x][y] = z;
                Vector<3> norm{findNormalSolution(triangle,
                                                  static_cast<float>(x),
                                                  static_cast<float>(y))
                                   .normalize()};
                float intensity{dot_product(light_dir, norm)};
                if (intensity > 0) {
                    draw_point(x, y,
                               {(uint8_t)(clr.r * intensity),
                                (uint8_t)(clr.g * intensity),
                                (uint8_t)(clr.b * intensity), 255});
                }
            }
        }
    }
}

// given a triangle in a 2D space determine if a point (x, y) is contained
// inside of the triangle.
//
// This function is using method found in: A Parallel Algorithm for Polygon
// Rasterization by J. Pineda
bool InsideTriangle(const Triangle& triangle, float x, float y) {
    Vector<3> p1 = triangle[0].pos;
    Vector<3> p2 = triangle[1].pos;
    Vector<3> p3 = triangle[2].pos;

    bool ret{true};
    Vector<3> dp12{p1 - p2};
    Vector<3> dp23{p2 - p3};
    Vector<3> dp31{p3 - p1};

    ret &= ((y - p1[Y]) * dp12[X] - (x - p1[X]) * dp12[Y] >= 0);
    ret &= ((y - p2[Y]) * dp23[X] - (x - p2[X]) * dp23[Y] >= 0);
    ret &= ((y - p3[Y]) * dp31[X] - (x - p3[X]) * dp31[Y] >= 0);

    return ret;
}

// given 3 points to define a plane return a function for solutions of
// Z given some X, Y.
std::function<float(float x, float y)> findPlaneSolution(const Vector<3>& v1,
                                                         const Vector<3>& v2,
                                                         const Vector<3>& v3) {
    Vector<3> normal{cross_product(v3 - v1, v2 - v1)};
    if (normal[Z] == 0)
        return [](float /* x */, float /* y */) {
            return -std::numeric_limits<float>::max();  // normal has no z
                                                        // component
        };
    float a{normal[X]};
    float b{normal[Y]};
    float c{normal[Z]};
    float x0{v1[X]};
    float y0{v1[Y]};
    float z0{v1[Z]};
    return [a, b, c, x0, y0, z0](float x, float y) {
        return (-a * (x - x0) - b * (y - y0)) / c + z0;
    };
}

// find the area of a triangle
float triangleArea(const Vector<3>& p1,
                   const Vector<3>& p2,
                   const Vector<3>& p3) {
    // this function makes use of a special property of the determinant to find
    // the area of the triangle
    Matrix<3, 3> m{
        {p1[X], p2[X], p3[X]}, {p1[Y], p2[Y], p3[Y]}, {1.f, 1.f, 1.f}};
    return determinant(m) / 2;
}

// Find the area of a triangle
float triangleArea(const Triangle& triangle) {
    Vector<3> p1 = triangle[0].pos;
    Vector<3> p2 = triangle[1].pos;
    Vector<3> p3 = triangle[2].pos;

    return triangleArea(p1, p2, p3);
}

Vector<3> findNormalSolution(const Triangle& triangle, float x, float y) {
    // find the area of the triangle formed by v1, v2, v3 in the x-y plane.
    // we will use this with barycentric coordinates to extrapolate data
    // for values inside the triangles
    VertexPair v1 = triangle[0];
    VertexPair v2 = triangle[1];
    VertexPair v3 = triangle[2];
    float sarea{triangleArea(triangle)};

    float s1{triangleArea({x, y, 0.f}, v2.pos, v3.pos) / sarea};
    float s2{triangleArea(v1.pos, {x, y, 0.f}, v3.pos) / sarea};
    float s3{triangleArea(v1.pos, v2.pos, {x, y, 0.f}) / sarea};
    return s1 * v1.norm + s2 * v2.norm + s3 * v3.norm;
}
