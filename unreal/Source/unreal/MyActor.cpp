// Fill out your copyright notice in the Description page of Project Settings.

#include "MyActor.h"
#include "Stdafx.h"
#include "CommonStructure.hpp"
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "LSystem.hpp"
#include "LRule.hpp"
#include "LLetter.hpp"
#include "Trunk.h"

// Sets default values
AMyActor::AMyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));

	system = new LSystem();

	Position = FVector::ZeroVector;
}

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
	Super::BeginPlay();
	
	system->SetWord("FF+(30)FF/(30)FF^(45)FF");
	//system->AddRule("F", "F[-&\\F][\\++&F]F[--&/F][+&F]");
	//system->SetWord("F+(45)\\(45)FF");
	//system->AddRule("f", "fffff");
	//system->AddRule("F(s):s>3", "FFRF(1)");
	//system->AddRule("F(s, t)", "F(s + 0.1, t + 0.01)");
	//system->AddRule("F(s, t) :t > 0.3", "F(s)");
	//system->AddRule("F(s, t, v)", "F(s + 0.1, t + 0.005, v)");
	//system->AddRule("F(s, t, v) :t", "0.15->F(s, t, 3)");
	//system->AddRule("Q", "F(0.1, 0.01)");
	//system->AddRule("R", "[/ (45) + (30)F(0.5, 0.01, 2)]");
	//system->AddRule("R", "[/ (90) + (30)F(0.5, 0.01, 2)]");
	//system->AddRule("R", "[/ (135) + (30)F(0.5, 0.01, 2)]");
	//system->AddRule("R", "[/ (180) + (30)F(0.5, 0.01, 2)]");
	//system->AddRule("R", "[/ (-45) + (30)F(0.5, 0.01, 2)]");
	//system->AddRule("R", "[/ (-90) + (30)F(0.5, 0.01, 2)]");
	//system->AddRule("R", "[/ (-135) + (30)F(0.5, 0.01, 2)]");
	//system->AddRule("R", "[/ (-180) + (30)F(0.5, 0.01, 2)]");
	//system->Iterate(1);

	std::vector<LSystem::UEModel>* models = new std::vector<LSystem::UEModel>();
	system->GetResultUEInfos(models);

	UWorld* world = GetWorld();

	for (const LSystem::UEModel& model : *models)
	{
		Position = FVector(-model.position.x, -model.position.z, model.position.y);
		FQuat q = FQuat(model.rotation.x, model.rotation.y, model.rotation.z, model.rotation.w);
		Rotator = FRotator(q);

		world->SpawnActor<ATrunk>(ATrunk::StaticClass(), Position, Rotator, SpawnParams);
	}
}

// Called every frame
void AMyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

