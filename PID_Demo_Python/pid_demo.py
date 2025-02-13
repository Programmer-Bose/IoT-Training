import pygame
import sys
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import zmq
import time
import json


context = zmq.Context()
socket = context.socket(zmq.PUB)
socket.bind("tcp://127.0.0.1:5555")



# Constants
WIDTH, HEIGHT = 800, 600
BALL_RADIUS = 20
FPS = 30

# Colors
WHITE = (255, 255, 255)
RED = (255, 0, 0)

# PID Controller Constants
P = 0.01
I = 0.1
D = 0.02

# Initialize Pygame
pygame.init()

# Set up the display
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Ball Bouncing Game")

# Set up the clock
clock = pygame.time.Clock()

class Ball:
    def __init__(self, x, y, radius, color):
        self.x = x
        self.y = y
        self.radius = radius
        self.color = color
        self.y_speed = 0  # Initialize y_speed attribute


    def draw(self):
        pygame.draw.circle(screen, self.color, (int(self.x), int(self.y)), self.radius)

class PID:
    def __init__(self, kp, ki, kd, dt):
        self.kp = kp
        self.ki = ki
        self.kd = kd
        self.dt = dt
        self.prev_error = 0
        self.integral = 0

    def calculate(self, setpoint, process_input):
        error = setpoint - process_input
        self.integral += error * self.dt
        derivative = (error - self.prev_error) / self.dt
        self.prev_error = error

        output = self.kp * error + self.ki * self.integral + self.kd * derivative
        return output





# Create the first ball
ball1 = Ball(WIDTH //4, HEIGHT // 2, BALL_RADIUS, RED)

# Create the second ball (follower)
ball2 = Ball(3*WIDTH//4 , HEIGHT // 2, BALL_RADIUS, WHITE)

# Create PID controller
pid_controller = PID(P, I, D, 1 / FPS)
# Viewport position
viewport_x = 0
# Game loop
while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            sys.exit()
        elif event.type == pygame.MOUSEBUTTONDOWN:
            # Check if the click is inside the first ball
            mouse_x, mouse_y = pygame.mouse.get_pos()
            distance = ((ball1.x - mouse_x) ** 2 + (ball1.y - mouse_y) ** 2) ** 0.5
            if distance <= BALL_RADIUS:
                ball1.y_speed = -10  # Give the ball an upward velocity when clicked

    # Update the first ball's position
    ball1.y_speed += 0.5  # Add gravity
    ball1.y += ball1.y_speed

    # Bounce off the floor
    if ball1.y + ball1.radius >= HEIGHT:
        ball1.y = HEIGHT - ball1.radius
        ball1.y_speed *= -0.8  # Reverse velocity with some damping

    # Bounce off the ceiling
    if ball1.y - ball1.radius <= 0:
        ball1.y = ball1.radius
        ball1.y_speed *= -0.8  # Reverse velocity with some damping

    # Update the second ball (follower) using PID controller
    pid_output = pid_controller.calculate(ball1.y, ball2.y)
    ball2.y += pid_output

    # Draw the background
    screen.fill((80, 150, 200))
    timestamp=time.time()

    message = json.dumps({"ball1_y": ball1.y, "ball2_y": ball2.y, "timestamp": timestamp})

    socket.send_string(message)

    # Draw the balls
    ball1.draw()
    ball2.draw()




    # Update the display
    pygame.display.flip()

    # Cap the frame rate
    clock.tick(FPS)
# Quit Pygame and exit the program
pygame.quit()
sys.exit()