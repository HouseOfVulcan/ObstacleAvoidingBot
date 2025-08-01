#include "logic.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Distance threshold and motion timing (in ms)
#define SAFE_DISTANCE_CM 15
#define TURN_90_MS       1000
#define BACKUP_MS        400
#define IDLE_MS          100

// FSM state definitions
typedef enum {
    STATE_FORWARD,
    STATE_BACKWARD,
    STATE_TURN_LEFT,
    STATE_TURN_RIGHT,
    STATE_ERROR
} ObstacleAvoidanceState;

// Track current FSM state
static ObstacleAvoidanceState current_state = STATE_FORWARD;

// set random seed or reset state if needed
void logic_init(void)
{
    srand(time(NULL)); // Random turn direction
    current_state = STATE_FORWARD;
}

// FSM logic
void obstacle_avoidance_fsm_step(void)
{
    switch (current_state)
    {
        case STATE_FORWARD:
        {
            uint32_t distance = get_distance_cm();

            if (distance == 0xFFFF)
		{
		   stop_motors();
		   printf("Sensor Error\n");
		   current_state = STATE_ERROR;
		   break;
		}

            if (distance < SAFE_DISTANCE_CM)
		{
		   stop_motors();
		   delay_ms(200);

		   move_backward();
		   delay_ms(BACKUP_MS);

		   current_state = (rand() % 2 == 0) ? STATE_TURN_LEFT : STATE_TURN_RIGHT; // randomly choose to go left or right
		}
            else
		{
		   move_forward();
		}
            break;
        }

        case STATE_TURN_LEFT:
            turn_left();
            delay_ms(TURN_90_MS);
            current_state = STATE_FORWARD;
            break;

        case STATE_TURN_RIGHT:
            turn_right();
            delay_ms(TURN_90_MS);
            current_state = STATE_FORWARD;
            break;

        case STATE_ERROR:
            stop_motors();
            // Could add retry/recovery logic here
            break;
    }

    delay_ms(IDLE_MS);
}
