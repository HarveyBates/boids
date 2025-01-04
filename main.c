#include <SDL.h>
#include <stdbool.h>

// Window constants
#define WINDOW_SIZE_X 1280
#define WINDOW_SIZE_Y 720

// Boid constants
#define BOIDS_COUNT 300
#define BOID_MIN_VELOCITY 2
#define BOID_MAX_VELOCITY 4.5
#define VELOCITY_START (BOID_MIN_VELOCITY + 1)
#define PROXIMITY_RANGE 20
#define VISIBLE_RANGE (PROXIMITY_RANGE + 50)
#define AVOIDANCE_FACTOR 0.05
#define MATCHING_FACTOR 0.2
#define COHESION_FACTOR 0.001
#define WINDOW_BOUNDARY 200
#define BOUNDARY_TURN_FACTOR 0.1

// Window utilities
SDL_Renderer* renderer = NULL;
SDL_Window* window = NULL;

// Some 2D position
typedef struct {
    float x;  // X Position
    float y;  // Y Position
} Position_Typedef;

// Boid
typedef struct {
    Position_Typedef p;  // Position
    float vX;            // X velocity
    float vY;            // Y velocity
} Boid_Typedef;

Boid_Typedef boids[BOIDS_COUNT];

// Window function
bool window_init();
void window_kill();
bool window_mainloop();

// Draw functions
void draw_window();
void draw_boid(Boid_Typedef* boid);
void draw_boids(Boid_Typedef* boids);

// Boid functions
void boids_create();
void boid_keep_on_screen(Boid_Typedef* boid);
void boids_separate();
void boids_align();
void boids_cohesion();

int main() {
    if (!window_init()) {
        return 1;
    }

    boids_create();

    while (window_mainloop()) {
        boids_separate();
        boids_align();
        boids_cohesion();
        SDL_Delay(1);
    }

    window_kill();
    return 0;
}

void boids_cohesion() {
    for (int b = 0; b < BOIDS_COUNT; b++) {
        float av_posX = 0, av_posY = 0;
        int n_boids = 0;
        for (int i = 0; i < BOIDS_COUNT; i++) {
            double distance = sqrt(pow(boids[b].p.x - boids[i].p.x, 2) +
                                   pow(boids[b].p.y - boids[i].p.y, 2));
            if (b == i || distance > VISIBLE_RANGE) continue;
            av_posX += boids[i].p.x;
            av_posY += boids[i].p.y;
            n_boids++;
        }

        if (n_boids == 0) continue;

        av_posX /= (float)n_boids;
        av_posY /= (float)n_boids;

        boids[b].vX += (av_posX - boids[b].p.x) * (float)COHESION_FACTOR;
        boids[b].vY += (av_posY - boids[b].p.y) * (float)COHESION_FACTOR;
    }
}

void boids_align() {
    for (int b = 0; b < BOIDS_COUNT; b++) {
        float av_vX = 0, av_vY = 0;
        int n_boids = 0;
        for (int i = 0; i < BOIDS_COUNT; i++) {
            double distance = sqrt(pow(boids[b].p.x - boids[i].p.x, 2) +
                                   pow(boids[b].p.y - boids[i].p.y, 2));
            if (b == i || distance > VISIBLE_RANGE) continue;
            av_vX += boids[i].vX;
            av_vY += boids[i].vY;
            n_boids++;
        }

        if (n_boids == 0) continue;

        av_vX /= (float)n_boids;
        av_vY /= (float)n_boids;

        boids[b].vX += (av_vX - boids[b].vX) * (float)MATCHING_FACTOR;
        boids[b].vY += (av_vY - boids[b].vY) * (float)MATCHING_FACTOR;
    }
}

void boids_separate() {
    for (int b = 0; b < BOIDS_COUNT; b++) {
        float proximity_x = 0, proximity_y = 0;
        for (int i = 0; i < BOIDS_COUNT; i++) {
            double distance = sqrt(pow(boids[b].p.x - boids[i].p.x, 2) +
                                   pow(boids[b].p.y - boids[i].p.y, 2));
            if (b == i || distance > PROXIMITY_RANGE) continue;
            proximity_x += boids[b].p.x - boids[i].p.x;
            proximity_y += boids[b].p.y - boids[i].p.y;
        }

        boids[b].vX += proximity_x * AVOIDANCE_FACTOR;
        boids[b].vY += proximity_y * AVOIDANCE_FACTOR;
    }
}

void boids_create() {
    srand(time(NULL));
    for (int i = 0; i < BOIDS_COUNT; i++) {
        boids[i].p.x = rand() % WINDOW_SIZE_X;
        boids[i].p.y = rand() % WINDOW_SIZE_Y;
        boids[i].vX = rand() % VELOCITY_START;
        boids[i].vY = rand() % VELOCITY_START;
    }
}

void boid_limit_speed(Boid_Typedef* boid) {
    double speed = sqrt(pow(boid->vX, 2) + pow(boid->vY, 2));
    if (speed == 0) return;

    if (speed > BOID_MAX_VELOCITY) {
        boid->vX = (boid->vX / speed) * BOID_MAX_VELOCITY;
        boid->vY = (boid->vY / speed) * BOID_MAX_VELOCITY;
    } else if (speed < BOID_MIN_VELOCITY) {
        boid->vX = (boid->vX / speed) * BOID_MIN_VELOCITY;
        boid->vY = (boid->vY / speed) * BOID_MIN_VELOCITY;
    }
}

void boid_keep_on_screen(Boid_Typedef* boid) {
    if (boid->p.x < WINDOW_BOUNDARY) {
        boid->vX += BOUNDARY_TURN_FACTOR;
    } else if (boid->p.x > WINDOW_SIZE_X - WINDOW_BOUNDARY) {
        boid->vX -= BOUNDARY_TURN_FACTOR;
    } else if (boid->p.y > WINDOW_SIZE_Y - WINDOW_BOUNDARY) {
        boid->vY -= BOUNDARY_TURN_FACTOR;
    } else if (boid->p.y < WINDOW_BOUNDARY) {
        boid->vY += BOUNDARY_TURN_FACTOR;
    }
}

void draw_boids(Boid_Typedef* _boids) {
    for (int i = 0; i < BOIDS_COUNT; i++) {
        boids[i].p.x += boids[i].vX;
        boids[i].p.y += boids[i].vY;
        boid_limit_speed(&boids[i]);
        boid_keep_on_screen(&boids[i]);
        draw_boid(&_boids[i]);
    }
}

void draw_boid(Boid_Typedef* boid) {
    // Get trajectory from velocity
    double angle = -atan2f(boid->vX, boid->vY);

    // Pre-calc vertices
    Position_Typedef v[3] = {{.x = boid->p.x + 0, .y = boid->p.y + 10},
                             {.x = boid->p.x + 5, .y = boid->p.y - 10},
                             {.x = boid->p.x - 5, .y = boid->p.y - 10}};

    SDL_Vertex triangle[3] = {
        // Note: removed first part of these equations as they are always zero
        {.position = {-(v[0].y - boid->p.y) * (float)sin(angle) + boid->p.x,
                      (v[0].y - boid->p.y) * (float)cos(angle) + boid->p.y},
         .color = {255, 0, 0, 255}},
        {.position = {(v[1].x - boid->p.x) * (float)cos(angle) -
                          (v[1].y - boid->p.y) * (float)sin(angle) + boid->p.x,
                      (v[1].x - boid->p.x) * (float)sin(angle) +
                          (v[1].y - boid->p.y) * (float)cos(angle) + boid->p.y},
         .color = {0, 255, 0, 255}},
        {.position = {(v[2].x - boid->p.x) * (float)cos(angle) -
                          (v[2].y - boid->p.y) * (float)sin(angle) + boid->p.x,
                      (v[2].x - boid->p.x) * (float)sin(angle) +
                          (v[2].y - boid->p.y) * (float)cos(angle) + boid->p.y},
         .color = {0, 0, 255, 255}}};

    SDL_RenderGeometry(renderer, NULL, triangle, 3, NULL, 0);
}

void draw_window() { draw_boids(boids); }

bool window_mainloop() {
    SDL_Event event;

    // Clear window
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // Check for exit event
    while (SDL_PollEvent(&event) != 0) {
        switch (event.type) {
            case SDL_QUIT:
                return false;
            default:
                break;
        }
    }

    draw_window();

    // Update window
    SDL_RenderPresent(renderer);

    return true;
}

bool window_init() {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("Error initialising SDL: %s\n", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow("Boids", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, WINDOW_SIZE_X,
                              WINDOW_SIZE_Y, SDL_WINDOW_SHOWN);

    if (!window) {
        printf("Error creating SDL window: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer) {
        printf("Error creating SDL renderer: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void window_kill() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}