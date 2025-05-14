#include "renderer.h"
#include "threadpool.h"
#include <SDL2/SDL_render.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <limits>
#include <memory>
#include <thread>
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
    for (int i = 0; i < static_cast<int>(zbuffer_.size()); i++) {
        for (int j = 0; j < static_cast<int>(zbuffer_[i].size()); j++) {
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
    for (int i = 0; i < static_cast<int>(zbuffer_.size()); i++) {
        for (int j = 0; j < static_cast<int>(zbuffer_[i].size()); j++) {
            zbuffer_[i][j] = -std::numeric_limits<float>::max();
        }
    }
}

void Renderer::draw_point(int x, int y, int z, const Color& clr) {
	std::lock_guard<std::mutex> lk(mut_);
	if (z < zbuffer_[x][y])
		return;
    zbuffer_[x][y] = z;
    SDL_SetRenderDrawColor(sdl_renderer_, clr.r, clr.g, clr.b, clr.a);
    SDL_RenderDrawPoint(sdl_renderer_, x, y);
}

//=============================================================================
// Rendering Models
//=============================================================================
void Renderer::draw_model(const Model& model) {
    Vector<3> z{view_vector(yaw, pitch)};                  // back-forward vec
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

	// create a threadpool for drawing faces
	ThreadPool tp;

    for (int i = 0; i < model.nfaces(); i++) {
        std::vector<FaceTuple> face = model.face(i);

        // transform the point and normal
        Vector<3> v1 =
            (transMatrix * model.vertex(face[0].vertex)).dehomogenize();
        Vector<3> v1n = (normalTransMatrix * model.normal(face[0].normal))
                            .dehomogenize()
                            .normalize();

        // triangle fan the face polygon (most of the time this is just a
        // triangle. outer loop for start points
        for (int j = 2; j < static_cast<int>(face.size()); j++) {
            // get the position and normal of the second point and transform
            // them
            Vector<3> v2 =
                (transMatrix * model.vertex(face[j - 1].vertex)).dehomogenize();
            Vector<3> v2n =
                (normalTransMatrix * model.normal(face[j - 1].normal))
                    .dehomogenize()
                    .normalize();

            // get the position and normal of the third point and transform them
            Vector<3> v3 =
                (transMatrix * model.vertex(face[j].vertex)).dehomogenize();
            Vector<3> v3n = (normalTransMatrix * model.normal(face[j].normal))
                                .dehomogenize()
                                .normalize();

            Triangle triangle{{{v1, v1n}, {v2, v2n}, {v3, v3n}}};
   //         draw_face(triangle, {255, 255, 255, 255});
			tp.add_task([this, triangle](){
            	draw_face(triangle, {255, 255, 255, 255});
			});
        }
    }
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(1s);
    SDL_RenderPresent(sdl_renderer_);
}

void Renderer::draw_face(const Triangle& triangle, const Color& clr) {
    Vector<3> v1 = triangle[0].pos;
    Vector<3> v2 = triangle[1].pos;
    Vector<3> v3 = triangle[2].pos;

    // create a bounding box around the triangle to be drawn
    int maxX = std::min({static_cast<int>(std::max({v1[X], v2[X], v3[X]})), SCREEN_WIDTH - 1});
 	int minX = std::max({static_cast<int>(std::min({v1[X], v2[X], v3[X]})), 0});
    int maxY = std::min({static_cast<int>(std::max({v1[Y], v2[Y], v3[Y]})), SCREEN_HEIGHT - 1});
    int minY = std::max({static_cast<int>(std::min({v1[Y], v2[Y], v3[Y]})), 0});

    // parametrize the plane created by the triangular face.
    //
    // Planes can be parametrized using the general equation:
    // ax + by + cz = d
    // where (a, b, c) is the norm of the plane. A norm can be trivially found 
	// by taking the cross product of two vectors on the plane.
	//
	// afterwards d can be solved for by substituting x, y, and z with a point
	// found on the plane. Now z can be created as a function of x, and y
	//
	// z = 1/c * (-ax -by + d)
	//
	// therefore we have: z(x, y) = 1/c * (-ax -by + d)
	//
	// there is a convenient property that
	// z(x + 1, y) = 1/c * (-a(x+1) - by + d)
	// 			   = 1/c * (-ax -a -by + d)
	// 			   = 1/c * (-ax -by + d) - a/c
	// 			   = z(x, y) - a/c
	//
	// using a similar technique we have
	// z(x, y + 1) = z(x, y) -b/c

    Vector<3> v2_delta = v2 - v1;
    Vector<3> v3_delta = v3 - v1;

    Vector<3> plane_norm = cross_product(v2_delta, v3_delta);

	// the plane is parallel with the z axis (don't render it)
	if(plane_norm[Z] == 0) {
		return;
	}

	float d = v1[X]*plane_norm[X] + v1[Y]*plane_norm[Y] + v1[Z]*plane_norm[Z];

	float delta_x = -plane_norm[X]/plane_norm[Z];
	float delta_y = -plane_norm[Y]/plane_norm[Z];

	// what is the z value at (minX, minY)?
	float z_row = (d - plane_norm[X] * minX - plane_norm[Y] * minY) / plane_norm[Z];

	Vector<3> norm = 1/3.f * (triangle[0].norm + triangle[1].norm + triangle[2].norm);
    for (int x = minX; x <= maxX; x++) {
		float z = z_row;
        for (int y = minY; y <= maxY; y++) {
            if (not InsideTriangle(triangle, static_cast<float>(x),
                                   static_cast<float>(y))) {
                continue;
            }
//            if (z >= zbuffer_[x][y]) {
//                zbuffer_[x][y] = z;
////                Vector<3> norm{findNormalSolution(
////                    triangle, static_cast<float>(x), static_cast<float>(y))};
//                float intensity{dot_product(light_dir, norm.normalize())};
//                if (intensity > 0) {
//                    draw_point(x, y,
//                               {static_cast<int>(clr.r * intensity),
//                                static_cast<int>(clr.g * intensity),
//                                static_cast<int>(clr.b * intensity), 255});
//                }
//            }

			// REMOVE THIS AFTER THIS IS FOR   TESTING
            {
//                Vector<3> norm{findNormalSolution(
//                    triangle, static_cast<float>(x), static_cast<float>(y))};
                float intensity{dot_product(light_dir, norm.normalize())};
                if (intensity > 0) {
					draw_point(x, y, z,
							   {static_cast<int>(clr.r * intensity),
								static_cast<int>(clr.g * intensity),
								static_cast<int>(clr.b * intensity), 255});
                }
            }


			z += delta_y;	
        }
		z_row += delta_x;
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
float GetDepthOnFace(const Triangle& triangle, float x, float y) {
    Vector<3> p1 = triangle[0].pos;
    Vector<3> p2 = triangle[1].pos;
    Vector<3> p3 = triangle[2].pos;

    Vector<3> normal{cross_product(p2 - p1, p3 - p1)};
    if (normal[Z] == 0) {
        return -std::numeric_limits<float>::max();  // normal has no z
    };

    float a{normal[X]};
    float b{normal[Y]};
    float c{normal[Z]};
    float x0{p1[X]};
    float y0{p1[Y]};
    float z0{p1[Z]};

    return (-a * (x - x0) - b * (y - y0)) / c + z0;
}

// find the area of a triangle in from a 2D perspective projected onto the X-Y
// plane.
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
