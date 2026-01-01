from multiprocessing import shared_memory

import numpy as np
import pygame

# Constants (MUST match shared.h)
SHM_NAME = "/shared_galaxy"
MAX_BODIES = 20000
WIDTH, HEIGHT = 1000, 1000


def main():
    pygame.init()
    screen = pygame.display.set_mode((WIDTH, HEIGHT))
    pygame.display.set_caption("Shared Memory Visualization")
    clock = pygame.time.Clock()

    print(f"Looking for {SHM_NAME}...")
    try:
        shm = shared_memory.SharedMemory(name=SHM_NAME)
    except FileNotFoundError:
        print("Error: Run the C simulation first!")
        return

    # Header is 2 integers (step, count) -> 8 bytes total
    # We read them as a numpy array of 2 ints
    header = np.ndarray((2,), dtype="int32", buffer=shm.buf, offset=0)

    # Body Data starts at offset 8. Shape is (MAX_BODIES, 2) floats.
    # map the WHOLE buffer, but we will only draw 'count' bodies.
    all_bodies = np.ndarray((MAX_BODIES, 2), dtype="float32", buffer=shm.buf, offset=8)

    print("Connected! Waiting for simulation...")

    running = True
    last_step = -1
    scale = 1e-9  # Adjust zoom
    offset_x, offset_y = WIDTH // 2, HEIGHT // 2

    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.MOUSEWHEEL:
                scale *= 1.1 if event.y > 0 else 0.9

        current_step = header[0]
        count = header[1]

        # Only redraw if C has computed a new step
        if current_step > last_step and count > 0:
            last_step = current_step

            # zero-copy!
            # Slice only the valid bodies
            valid_bodies = all_bodies[:count]

            screen.fill((0, 0, 0))

            screen_pos = (valid_bodies * scale) + [offset_x, offset_y]

            for x, y in screen_pos:
                if 0 <= x < WIDTH and 0 <= y < HEIGHT:
                    screen.set_at((int(x), int(y)), (255, 255, 255))

            pygame.display.set_caption(f"Step: {current_step} | Bodies: {count}")
            pygame.display.flip()

        # Limit Python speed so it doesn't eat 100% CPU checking for updates
        clock.tick(60)

    shm.close()
    pygame.quit()


if __name__ == "__main__":
    main()
