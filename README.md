# Assignment 4

Based on concepts of concurrency and process synchronization.

## Q1 - The refree-player problem

### Threads Involved
- Player Threads
- Refree Threads

### Functions used
- player_function() : main player function, called when player_thread is created
- refree_function() : main refree function, called when refree thread is created
- enterAcademy()
- meetOrganizer()
- enterCourt()
- warmUp()
- adjustEquipment()
- startGame()

### Mutex Locks used
- refree_lock : set when a refree has occupied organizer
- player1_lock : set when a player has occupied organizer as player 1
- player2_lock : set when a player has occupied organizer as player 2
- team_lock : used to set team variable
- ready_team_lock : used to set ready_team variable

### How to Run
- gcc q1.c -pthread
- ./a.out

### Input Format
- input is one line : n, where n is the number of games

## Q2 - The Voter-EVM problem

### Threads Involved
- Voter Threads
- EVM Threads

### Structures Involved
- Booth Structure
  - `int` active_evm_index
  - `int` booth_index
  - `int` total_voters
  - `int` left_voters
  - `mutex lock` booth_lock
  - `condition` booth_cond
  - `condition` ready_evm


- EVM Structure
  - `Booth*` booth
  - `int` evm_index


- Voter Structure
  - `Booth*` booth
  - `int`voter_index

### Functions used
- booth_init()
- voter_function()
- evm_function()
- polling_ready_evm()
- evm_voting()
- voter_wait_for_evm()
- voter_in_slot()
- check_waiting_evm()

### How to Run
- gcc q2.c -pthread
- ./a.out

## Q3 - Concurrent vs Normal mergesort

### Functions Involved
- Mergesort with concurrent processes
- Mergesort with concurrent Threads
- Non-concurrent mergesort

### How to Run
- gcc q3.c
- ./a.out

### Input Format
- First line : n, where n is the number of elements in the array
- Next n lines have the elements of the array

### Output
- Output has a report of the times taken by the three mergesorts
- The array sorted by processes is stored in *output3*
- The report is also appended to file *report3*
- If any array isn't actually sorted by any of the mergesorts, it is reported.
