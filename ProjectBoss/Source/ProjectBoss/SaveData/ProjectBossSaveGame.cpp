// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectBossSaveGame.h"

UProjectBossSaveGame::UProjectBossSaveGame()
{
	MLParticipation = false;
	TotalGames = 0;
	PlayerWins = 0;
}

FString UProjectBossSaveGame::BuildSaveGameLogString(UProjectBossSaveGame* saveGame)
{
	FString output;
	// Check save game is valid
	if (IsValid(saveGame))
	{
		char NL = '\n';

		FString isParticipating = saveGame->MLParticipation ? "Yes" : "No";

		// Add ML participation
		output += "ML Participation: " + isParticipating;
		output += NL;
		// Add total games
		output += "Total Games: " + FString::FromInt(saveGame->TotalGames);
		output += NL;
		// Add player wins
		output += "Player Wins: " + FString::FromInt(saveGame->PlayerWins);
		output += NL;
	}
	// Return output
	return output;
}