// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectVersionBlueprint.h"

FString UProjectVersionBlueprint::GetProjectVersion()
{
	FString projectVersion;
	GConfig->GetString(
		TEXT("/Script/EngineSettings.GeneralProjectSettings"),
		TEXT("ProjectVersion"),
		projectVersion,
		GGameIni
	);
	return projectVersion;
}