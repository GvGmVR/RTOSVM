-- 1. Hardware Node Declarations
CREATE NODE status_led AS LED (
    PIN = 13, 
    INITIAL_STATE = OFF
) ;

-- 2. RTOS Task Declaration
CREATE TASK MainBlinkTask (
    PRIORITY = 2, 
    STACK_SIZE = 1024
) ;

-- 3. FSM States (Graph Vertices)
CREATE STATE TurnLEDOn IN TASK MainBlinkTask (
    ACTION = SET status_led = ON
) ;

CREATE STATE TurnLEDOff IN TASK MainBlinkTask (
    ACTION = SET status_led = OFF
) ;

-- 4. FSM Transitions (Graph Edges with Timed Delays)
CONNECT TurnLEDOff TO TurnLEDOn AFTER 500 MS ;
CONNECT TurnLEDOn TO TurnLEDOff AFTER 500 MS ;