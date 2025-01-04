# Boids

https://en.wikipedia.org/wiki/Boids

An implementation of boids in C with SDL. Each rule loops over the boids independently, to improve this combine to a single method.

<img src="media/example.gif" width=1280>

Just a bit of fun, not planning on doing anymore with this.

Adjust settings using:
```c
// Boid constants
#define BOIDS_COUNT 300
#define BOID_MIN_VELOCITY 2
#define BOID_MAX_VELOCITY 4.5
#define VELOCITY_START (BOID_MIN_VELOCITY + 1)
#define PROXIMITY_RANGE 20
#define VISIBLE_RANGE (PROXIMITY_RANGE + 50)
#define AVOIDANCE_FACTOR 0.05
#define MATCHING_FACTOR 0.03
#define COHESION_FACTOR 0.001
#define WINDOW_BOUNDARY 200
#define BOUNDARY_TURN_FACTOR 0.1
```
