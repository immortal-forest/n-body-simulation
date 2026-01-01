import sys

import numpy as np
import pandas as pd
import pygame

# --- Configuration ---
FILE_PATH = "data/output.csv"
WIDTH, HEIGHT = 1200, 800
FPS = 60
TRAIL_FADE = 40
SPEED_FACTOR = 1


BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
CYAN = (0, 255, 255)
MAGENTA = (255, 0, 255)


def main():
    print("Loading simulation data... (this might take a moment)")
    try:
        df = pd.read_csv(FILE_PATH)
    except FileNotFoundError:
        print("Error: data/output.csv not found.")
        sys.exit()

    # Pre-group data for O(1) access
    grouped = df.groupby("Step")
    total_steps = df["Step"].max()
    print("Data loaded. Starting render engine.")

    pygame.init()
    screen = pygame.display.set_mode((WIDTH, HEIGHT))
    pygame.display.set_caption("N-Body Gravity Simulation (Pan: Drag | Zoom: Scroll)")
    clock = pygame.time.Clock()

    # auto-scale to fit the galaxy initially
    max_pos = np.percentile(np.abs(df[["X", "Y"]].values), 95)
    scale = (HEIGHT / 2) / max_pos * 0.8  # Initial zoom
    offset_x, offset_y = WIDTH // 2, HEIGHT // 2  # Center of screen

    # Surface for trails (Semi-transparent black)
    # acts like an eraser that slowly fades old pixels
    fade_surface = pygame.Surface((WIDTH, HEIGHT))
    fade_surface.set_alpha(TRAIL_FADE)
    fade_surface.fill(BLACK)

    step = 0
    running = True
    dragging = False
    last_mouse_pos = (0, 0)
    paused = False

    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

            elif event.type == pygame.MOUSEWHEEL:
                zoom_speed = 0.1
                if event.y > 0:
                    scale *= 1 + zoom_speed
                else:
                    scale *= 1 - zoom_speed

            elif event.type == pygame.MOUSEBUTTONDOWN:
                if event.button == 1:  # Left click
                    dragging = True
                    last_mouse_pos = event.pos
            elif event.type == pygame.MOUSEBUTTONUP:
                if event.button == 1:
                    dragging = False
            elif event.type == pygame.MOUSEMOTION:
                if dragging:
                    mx, my = event.pos
                    dx = mx - last_mouse_pos[0]
                    dy = my - last_mouse_pos[1]
                    offset_x += dx
                    offset_y += dy
                    last_mouse_pos = (mx, my)

            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_SPACE:
                    paused = not paused

        if not paused:
            screen.blit(fade_surface, (0, 0))

            try:
                # Get current frame data
                data = grouped.get_group(step)

                # Convert real coordinates to screen coordinates
                pixel_x = (data["X"].values * scale) + offset_x
                pixel_y = (data["Y"].values * scale) + offset_y

                # Draw stars
                for px, py in zip(pixel_x, pixel_y):
                    # Check if point is on screen before drawing (Culling)
                    if 0 <= px < WIDTH and 0 <= py < HEIGHT:
                        pygame.draw.rect(screen, CYAN, (px, py, 2, 2))

                center_x = (data.iloc[0]["X"] * scale) + offset_x
                center_y = (data.iloc[0]["Y"] * scale) + offset_y
                pygame.draw.circle(screen, WHITE, (int(center_x), int(center_y)), 4)

                step += SPEED_FACTOR
                if step > total_steps:
                    step = 0  # Loop animation

            except KeyError:
                pass  # Frame missing or end of simulation

            font = pygame.font.SysFont("monospace", 16)
            text = font.render(
                f"Step: {step}/{total_steps} | Zoom: {scale:.2e}", True, WHITE
            )
            screen.blit(text, (10, 10))

            pygame.display.flip()
            clock.tick(FPS)

    pygame.quit()


if __name__ == "__main__":
    main()
