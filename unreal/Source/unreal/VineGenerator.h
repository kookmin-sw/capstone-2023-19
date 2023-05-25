// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VineGenerator.generated.h"

class LLetter;
class LSystem;
class AVine;

UCLASS()
class UNREAL_API AVineGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVineGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void Iterate();
	void Clear();

private:
	LSystem* System;
	FActorSpawnParameters SpawnParams;
	FRotator Rotator;
	FVector Position;

	std::vector<AVine*>* vines;
	float renderTime;
	float iterateCycle;
	int iterateCount;
	int tryCount;
};
