import pygame
import random
import time
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import threading

# Initialize Pygame
pygame.init()

# Screen settings
WIDTH, HEIGHT = 800, 400
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("PID Ball Balance with Live Graph")

# Colors
WHITE = (255, 255, 255)
RED = (200, 50, 50)
GREEN = (50, 200, 50)
BLUE = (50, 50, 200)

# Ball properties
ball_radius = 15
ball_x = 100
ball_y = HEIGHT // 2
ball_speed = 0


# Kp = 0.1  # Proportional gain
# Ki = 0.01  # Integral gain
# Kd = 0.05  # Derivative gain


# PID parameters
Kp = 0.01  # Proportional gain
Ki = 0.01  # Integral gain
Kd = 0.0   # Derivative gain

# PID control variables
prev_error = 0
integral = 0
setpoint = WIDTH // 2  # The desired center position

# Data storage for graph
time_values = []
setpoint_values = []
control_values = []
frame_count = 0  # To track time

clock = pygame.time.Clock()

# Live Graph Setup
fig, ax = plt.subplots()
ax.set_title("Live PID Control Graph")
ax.set_xlabel("Time")
ax.set_ylabel("Control Input")
line_setpoint, = ax.plot([], [], label="Setpoint", color="blue")
line_control, = ax.plot([], [], label="Control Input", color="red")
ax.legend()

def update_plot(frame):
    """Updates the live graph with new data."""
    line_setpoint.set_data(time_values, setpoint_values)
    line_control.set_data(time_values, control_values)
    
    ax.set_xlim(max(0, frame - 50), frame + 10)  # Scroll x-axis
    ax.set_ylim(min(control_values, default=-10), max(control_values, default=10))  # Adjust y-axis
    
    return line_setpoint, line_control

def run_pygame():
    """Runs the Pygame simulation with PID control."""
    global ball_x, prev_error, integral, frame_count
    
    running = True
    while running:
        screen.fill(WHITE)

        # Apply random disturbance
        disturbance = random.choice([-1, 0, 1]) * 2
        ball_x += disturbance  # Simulate external push

        # PID Calculation
        error = setpoint - ball_x
        integral += error
        derivative = error - prev_error
        control_signal = Kp * error + Ki * integral + Kd * derivative

        # Apply PID control to move the ball back
        ball_x += control_signal
        prev_error = error

        # Store data for plotting
        time_values.append(frame_count)
        setpoint_values.append(setpoint)
        control_values.append(control_signal)
        frame_count += 1

        # Draw elements in Pygame
        pygame.draw.circle(screen, RED, (int(ball_x), ball_y), ball_radius)
        pygame.draw.line(screen, BLUE, (setpoint, 0), (setpoint, HEIGHT), 2)

        # Display P, I, D values
        font = pygame.font.Font(None, 30)
        text_p = font.render(f"P: {Kp * error:.2f}", True, GREEN)
        text_i = font.render(f"I: {Ki * integral:.2f}", True, GREEN)
        text_d = font.render(f"D: {Kd * derivative:.2f}", True, GREEN)
        screen.blit(text_p, (20, 20))
        screen.blit(text_i, (20, 50))
        screen.blit(text_d, (20, 80))

        pygame.display.flip()
        clock.tick(30)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
                pygame.quit()
                return

# Start Pygame in a separate thread
pygame_thread = threading.Thread(target=run_pygame)
pygame_thread.start()

# Start Matplotlib Animation
ani = animation.FuncAnimation(fig, update_plot, interval=100)
plt.show()
