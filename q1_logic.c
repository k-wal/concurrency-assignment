/*

mutex locks for organizer, player1, player2, refree, to_wait, ready_team, team
index player1_index,player2_index,refree_index
int team=0,ready_team=0

startGame()
	while(true)
		if(ready_team==3)
			GAME HAS STARTED BY PLAYERS
			lock(team_lock)
			team=0
			free(team_lock)
			lock(ready_team_lock)
			ready_team=0
			free(ready_team_lock)
			free(player 1)
			free(player 2)
			free(refree)

for creation
	for(i=0; i<3n; i++)
		wait(random%3)
		assign player or refree
		
function meet_organizer
{
	lock(to_wait)
	waiting_players++
	free(to_wait)
		
	if player
		if waiting_players is even
			lock(player2)
			lock(team_lock)
			team++;
			free(team_lock)
			lock(to_wait)
			waiting_players--
			free(to_wait)
			player2_index = current process index

		if waiting_players is odd
			lock(player1)
			lock(team_lock)
			team++;
			free(team_lock)
			lock(to_wait)
			waiting_players--
			free(to_wait)
			player1_index = current process index
	
	if refree
		lock(refree)
		lock(team_lock)
		team++;
		free(team_lock)
		refree_index = current process index

	while(true)
		if(team==3)
			return

}

warmUp()
	sleep(1)
	lock(ready_team_lock)
	ready_team++
	free(ready_team_lock)

adjustEquipment()
	sleep(1)
	lock(ready_team_lock)
	ready_team++
	free(ready_team_lock)


function player_function(player_index)
{
	enterAcademy
	meetOrganizer
	enterCourt
	warmUp : 1 second
}

function refree_function(refree_index)
{
	enterAcademy
	meetOrganizer
	enterCourt
	adjustEquipment : 0.5 seconds
	startGame
}

*/
