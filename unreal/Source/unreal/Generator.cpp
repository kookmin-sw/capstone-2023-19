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
#include "Generator.h"

// Sets default values
AGenerator::AGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	System = new LSystem();
	Position = FVector::ZeroVector;
}

// Called when the game starts or when spawned
void AGenerator::BeginPlay()
{
	Super::BeginPlay();
	//TrunkType = TrunkMaterialType::Beech_I;

	System->SetWord("FF+(30)FF/(30)FF^(45)FF");

	std::vector<LSystem::UEModel>* models = new std::vector<LSystem::UEModel>();
	System->GetResultUEInfos(models);

	UWorld* world = GetWorld();

	for (const LSystem::UEModel& model : *models)
	{
		Position = FVector(-model.position.x, -model.position.z, model.position.y);
		FQuat q = FQuat(model.rotation.x, model.rotation.y, model.rotation.z, model.rotation.w);
		Rotator = FRotator(q);

		ATrunk* trunk = world->SpawnActor<ATrunk>(ATrunk::StaticClass(), Position, Rotator, SpawnParams);
		//trunk->SetTrunkMaterial(TrunkType);		// Set trunk material
	}
}

// Called every frame
void AGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
