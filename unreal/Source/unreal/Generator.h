// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Generator.generated.h"

class LLetter;
class LSystem;
class ATrunk;
class ALeaf;

enum class TrunkMaterialType : uint8;

UCLASS()
class UNREAL_API AGenerator : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void Iterate();
	void Clear();

private:
	//TrunkMaterialType TrunkType;

	LSystem* System;
	FActorSpawnParameters SpawnParams;
	FRotator Rotator;
	FVector Position;

	std::vector<ATrunk*>* trunks;
	std::vector<ALeaf*>* leaves;
	float renderTime;
	float iterateCycle;
	int iterateCount;
	int tryCount;
};
