#define _USE_MATH_DEFINES
#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 600

struct Circle
{
	double x, y, radius;
};

struct Ray
{
	double x_start, y_start;
	double angle;
	double x_end, y_end;
};

void fillCircle(SDL_Surface *surface, struct Circle *circle, Uint32 color)
{

	SDL_LockSurface(surface);
	for (double i = circle->x - circle->radius; i <= circle->x + circle->radius; i++)
	{
		for (double j = circle->y - circle->radius; j <= circle->y + circle->radius; j++)
		{
			double dx = i - circle->x;
			double dy = j - circle->y;
			if (dx * dx + dy * dy <= circle->radius * circle->radius)
			{
				if (i >= 0 && i < WIDTH && j >= 0 && j < HEIGHT)
				{
					SDL_Rect pixel = (SDL_Rect){i, j, 1, 1};
					SDL_FillRect(surface, &pixel, color);
				}
			}
		}
	}

	SDL_UnlockSurface(surface);
}

void generate_rays_to_circle(SDL_Surface *surface, struct Circle *source, struct Circle *target, Uint32 color)
{
	SDL_LockSurface(surface);

	double center_dx = target->x - source->x;
	double center_dy = target->y - source->y;
	double center_distance = sqrt(center_dx * center_dx + center_dy * center_dy);

	if (center_distance <= source->radius + target->radius)
	{
		SDL_UnlockSurface(surface);
		return;
	}

	double sin_theta = target->radius / center_distance;
	double cos_theta = sqrt(1 - sin_theta * sin_theta);

	double base_angle = atan2(center_dy, center_dx);

	double tangent_angle1 = base_angle + asin(sin_theta);
	double tangent_angle2 = base_angle - asin(sin_theta);

	int num_rays = 50;
	double angle_step = (tangent_angle1 - tangent_angle2) / num_rays;

	for (int ray = 0; ray <= num_rays; ray++)
	{
		double current_angle = tangent_angle2 + ray * angle_step;

		double start_x = source->x + source->radius * cos(current_angle);
		double start_y = source->y + source->radius * sin(current_angle);

		double step_size = 1.0;
		double ray_x = start_x;
		double ray_y = start_y;

		while (ray_x >= 0 && ray_x < WIDTH && ray_y >= 0 && ray_y < HEIGHT)
		{
			double dx_to_target = ray_x - target->x;
			double dy_to_target = ray_y - target->y;
			if (dx_to_target * dx_to_target + dy_to_target * dy_to_target <= target->radius * target->radius)
			{
				break;
			}

			if (ray_x >= 0 && ray_x < WIDTH && ray_y >= 0 && ray_y < HEIGHT)
			{
				((Uint32 *)surface->pixels)[(int)ray_y * surface->w + (int)ray_x] = color;
			}

			ray_x += step_size * cos(current_angle);
			ray_y += step_size * sin(current_angle);
		}
	}

	SDL_UnlockSurface(surface);
}

void generate_rays_from_center(SDL_Surface *surface, struct Circle *circle, Uint32 color)
{
	SDL_LockSurface(surface);

	int num_rays = 3600;
	double angle_step = 2 * M_PI / num_rays;

	for (int i = 0; i < num_rays; i++)
	{
		double angle = i * angle_step;
		double step_size = 1.0;

		double ray_x = circle->x + circle->radius * cos(angle);
		double ray_y = circle->y + circle->radius * sin(angle);

		while (ray_x >= 0 && ray_x < WIDTH && ray_y >= 0 && ray_y < HEIGHT)
		{
			((Uint32 *)surface->pixels)[(int)ray_y * surface->w + (int)ray_x] = color;

			ray_x += step_size * cos(angle);
			ray_y += step_size * sin(angle);
		}
	}

	SDL_UnlockSurface(surface);
}

int main()
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow("Raytracing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

	SDL_Surface *surface = SDL_GetWindowSurface(window);
	struct Circle circle = {200, 200, 50};
	struct Circle shadow = {600, 300, 140};

	int simulation_running = 1;
	SDL_Event event;
	while (simulation_running)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				simulation_running = 0;
			}
			if (event.type == SDL_MOUSEMOTION && event.motion.state != 0)
			{
				circle.x = event.motion.x;
				circle.y = event.motion.y;
			}
		}
		SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
		fillCircle(surface, &circle, SDL_MapRGB(surface->format, 255, 255, 255));
		fillCircle(surface, &shadow, SDL_MapRGB(surface->format, 0, 0, 255));
		generate_rays_to_circle(surface, &circle, &shadow, SDL_MapRGB(surface->format, 255, 255, 255));
		SDL_UpdateWindowSurface(window);
		SDL_Delay(10);
	}
}
