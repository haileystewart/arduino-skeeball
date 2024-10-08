import time
import random

# Simulated serial communication with Arduino
class SimulatedSerial:
    def readline(self):
        # Simulating a random target hit from Arduino (5 targets with different scores)
        return random.choice([
            "Target 1 Hit!",  # 1000 points
            "Target 2 Hit!",  # 2000 points
            "Target 3 Hit!",  # 3000 points
            "Target 4 Hit!",  # 5000 points
            "Target 5 Hit!",  # 10000 points
            ""
        ])

# Initialize the simulated serial communication
ser = SimulatedSerial()

# Global variables for game score and high score tracking
player_score = 0
high_score = 0
balls_used = 0  # Track the number of balls used in a round

def update_score(target_hit):
    global player_score, balls_used
    if target_hit == "Target 1 Hit!":
        player_score += 1000  # Target 1: 1000 points
    elif target_hit == "Target 2 Hit!":
        player_score += 2000  # Target 2: 2000 points
    elif target_hit == "Target 3 Hit!":
        player_score += 3000  # Target 3: 3000 points
    elif target_hit == "Target 4 Hit!":
        player_score += 5000  # Target 4: 5000 points
    elif target_hit == "Target 5 Hit!":
        player_score += 10000  # Target 5: 10000 points
    print(f"Player Score: {player_score}")
    balls_used += 1  # Increment the ball counter

def play_round(round_number):
    global player_score, high_score, balls_used
    print(f"--- Round {round_number} Start! ---")
    
    player_score = 0  # Reset player score at the start of each round
    balls_used = 0  # Reset the ball counter
    start_time = time.time()

    # Simulate a single round of gameplay (game ends after 10 balls)
    while balls_used < 10:
        target_hit = ser.readline().strip()  # Simulate receiving data from Arduino
        if target_hit:
            print(f"{target_hit} received!")
            update_score(target_hit)

        time.sleep(1)  # Simulate time between readings

    print(f"--- Round {round_number} Over! Final Score: {player_score}")

    # Update the high score if the player score is greater
    if player_score > high_score:
        high_score = player_score
        print(f"New High Score! {high_score}")

    print(f"Highest Score so far: {high_score}")

def main():
    num_rounds = 3  # Set the number of rounds to play

    for round_number in range(1, num_rounds + 1):
        play_round(round_number)

    print(f"--- Game Over! Highest Score Achieved: {high_score} ---")

if __name__ == "__main__":
    main()
