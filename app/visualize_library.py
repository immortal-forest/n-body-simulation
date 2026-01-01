import ctypes

import pygame


# c structs
class Vector(ctypes.Structure):
    _fields_ = [("x", ctypes.c_double), ("y", ctypes.c_double)]


class Body(ctypes.Structure):
    _fields_ = [
        ("mass", ctypes.c_double),
        ("position", Vector),
        ("velocity", Vector),
        ("force", Vector),
    ]


lib = ctypes.CDLL("./build/libgalaxy.so")

# return types of functions
lib.sim_get_bodies.restype = ctypes.POINTER(Body)

NUM_BODIES = 1000
WIDTH, HEIGHT = 1000, 1000


def main():
    # initialize C Simulation
    print(f"Initializing C simulation with {NUM_BODIES} bodies...")
    lib.sim_init(NUM_BODIES)

    # get the pointer to the memory array
    bodies_ptr = lib.sim_get_bodies()

    # pygame setup
    pygame.init()
    screen = pygame.display.set_mode((WIDTH, HEIGHT))
    clock = pygame.time.Clock()

    scale = 1e-9
    offset_x, offset_y = WIDTH // 2, HEIGHT // 2
    running = True

    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.MOUSEWHEEL:
                scale *= 1.1 if event.y > 0 else 0.9

        # compute one frame
        lib.sim_step()

        screen.fill((0, 0, 0))

        for i in range(NUM_BODIES):
            b = bodies_ptr[i]

            # simple projection
            px = int(b.position.x * scale + offset_x)
            py = int(b.position.y * scale + offset_y)

            if 0 <= px < WIDTH and 0 <= py < HEIGHT:
                screen.set_at((px, py), (255, 255, 255))

        pygame.display.flip()
        clock.tick(60)

    # Cleanup
    lib.sim_cleanup()
    pygame.quit()


if __name__ == "__main__":
    main()
