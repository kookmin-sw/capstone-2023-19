// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Trunk.generated.h"

UCLASS()
class UNREAL_API ATrunk : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrunk();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UStaticMeshComponent* Mesh;
};
