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
#include "Leaf.h"
#include "Generator.h"

// Sets default values
AGenerator::AGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	System = new LSystem();
	Position = FVector::ZeroVector;
	renderTime = -5.0f;
	trunks = new std::vector<ATrunk*>();
	leaves = new std::vector<ALeaf*>();
}

// Called when the game starts or when spawned
void AGenerator::BeginPlay()
{
	Super::BeginPlay();

	tryCount = 0;

	// default.map
	System->SetThickness(0.5f);
	System->SetDeltaThickness(0.92f);
	System->SetDistance(0.7f);
	System->SetWord("T(6)A");
	System->AddRule("A", "[+(30)FQA][+(30)^(120)FQA][+(30)&(120)FQA]");
	System->AddRule("F", "S^(5)F");
	System->AddRule("Q", "[/(66)J(5,1)]");
	System->AddRule("Q", "[\\(66)J(5,1)]");
	System->AddRule("S", "FQ");
	System->AddRule("T(t):t>0", "T(t - 1)");
	System->AddRule("T(t):t=0", "FFFF");
	//System->Iterate(7);
	iterateCount = 7;
	iterateCycle = 3.f;
	//TrunkType = TrunkMaterialType::Beech_I;
}

// Called every frame
void AGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	renderTime += DeltaTime;

	if (renderTime > iterateCycle)
	{
		renderTime = 0.f;
		// Destory before;
		Iterate();
	}
}

void AGenerator::Iterate()
{
	if (tryCount >= iterateCount)
	{
		return;
	}

	// Iterate 전 기존 rendering 초기화
	Clear();

	System->Iterate();

	std::vector<LSystem::UEModel>* models = new std::vector<LSystem::UEModel>();
	System->GetResultUEInfos(models);

	UWorld* world = GetWorld();

	for (LSystem::UEModel& model : *models)
	{
		Position = FVector(-model.position.x, -model.position.z, model.position.y);
		FQuat q = FQuat(model.rotation.x, model.rotation.y, model.rotation.z, model.rotation.w);
		Rotator = FRotator(q);

		if (model.type == 1)
		{
			ATrunk* trunk = world->SpawnActor<ATrunk>(ATrunk::StaticClass(), Position, Rotator, SpawnParams);
			//UE_LOG(LogTemp, Warning, TEXT("%f %f %f"), model.scale.x, model.scale.y, model.scale.z);
			if (model.scale.x < 0.000001f)
			{
				model.scale.x = 0.000001f;
				model.scale.y = 0.000001f;
			}
			FVector Scale = FVector(model.scale.x, model.scale.y, 1);
			trunk->SetActorScale3D(Scale);
			trunks->push_back(trunk);
		}
		else
		{
			ALeaf* leaf = world->SpawnActor<ALeaf>(ALeaf::StaticClass(), Position, Rotator, SpawnParams);
			leaves->push_back(leaf);
		}
	}

	tryCount++;
}

void AGenerator::Clear()
{
	for (ATrunk* trunk : *trunks)
	{
		trunk->Destroy();
	}

	for (ALeaf* leaf : *leaves)
	{
		leaf->Destroy();
	}
}