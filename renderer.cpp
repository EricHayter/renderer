#include "renderer.h"
#include "model.h"
#include <algorithm>

// this is just a POC I'll be editting the values of the vertices in this
// function (will edit to remove this feature later)
void draw_model(const Model &model, SDL_Renderer *renderer)
{
	for (int i = 0; i < model.nfaces(); i++) {
		std::vector<FaceTuple> face = model.face(i);

		for (int j = 0; j < face.size(); j++) { // outer loop for start points
			Vertex start = model.vertex(face[j].vertex - 1); // indexes start at 1
			start.x = (start.x + 1.) * SCREEN_WIDTH / 2.;
			start.y = (start.y + 1.) * SCREEN_HEIGHT / 2.;
			for (int k = j + 1; k < face.size(); k++) {
				Vertex stop = model.vertex(face[k].vertex - 1);
				stop.x = (stop.x + 1.) * SCREEN_WIDTH / 2.;
				stop.y = (stop.y + 1.) * SCREEN_HEIGHT / 2.;
				draw_line({start, stop}, renderer, {255, 255, 255});
			}
		}
	}
}

void draw_line(Line l, SDL_Renderer *renderer, Color color)
{
    // save the current color
    Color prev_color {};
    uint8_t r, g, b, a; 
    SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a); // do a check for error in a sec
    prev_color.r = r;
    prev_color.g = g;
    prev_color.b = b;
    prev_color.a = a;

    SDL_SetRenderDrawColor(renderer, 
            color.r,
            color.g,
            color.b,
            color.a);

    int y {};
    bool is_steep {false};
    if (std::abs(l.to.y - l.from.y) > std::abs(l.to.x - l.from.x)) {
        std::swap(l.from.x, l.from.y);
        std::swap(l.to.x, l.to.y);
        is_steep = true;
    }

    if (l.to.x < l.from.x) {
        std::swap(l.to.x, l.from.x);
    }

    for (int x = l.from.x; x <= l.to.x; x++) {
        float t {static_cast<float>(x - l.from.x)/(l.to.x - l.from.x)};
        y = l.from.y + (l.to.y - l.from.y) * t;
        if (is_steep) {
            SDL_RenderDrawPoint(renderer, y, x);
        } else {
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }
}

