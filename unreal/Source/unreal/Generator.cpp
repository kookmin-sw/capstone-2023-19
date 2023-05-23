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
#include "Vine.h"
#include "Leaf.h"
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

	// Mode
	bool vineMode = false;
	bool treeLeafMode = true;

	if (vineMode)
	{
		// vine.map
		System->SetAngleChange(22.5f);
		System->SetWord("F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]-[-F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]+F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]+F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]]+[+F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]-F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]-F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]]F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]-[-F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]+F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]+F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]]+[+F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]-F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]-F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]]-[-F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]-[-F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]+F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]+F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]]+[+F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]-F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]-F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]]+F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]-[-F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]+F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]+F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]]+[+F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]-F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]-F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]]+F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]-[-F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]+F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]+F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]]+[+F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]-F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]-F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]]]+[+F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]-[-F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]+F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]+F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]]+[+F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]-F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]-F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]]-F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]-[-F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]+F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]+F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]]+[+F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]-F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]-F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]]-F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]-[-F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]+F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]+F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]]+[+F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]-F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]-F(0.1875)F(0.1875)-[-F(0.1875)+F(0.1875)+F(0.1875)]+[+F(0.1875)-F(0.1875)-F(0.1875)]]]");

		std::vector<LSystem::UEModel>* models = new std::vector<LSystem::UEModel>();
		System->GetResultUEInfos(models);

		UWorld* world = GetWorld();

		for (const LSystem::UEModel& model : *models)
		{
			Position = FVector(-model.position.x, -model.position.z, model.position.y);
			FQuat q = FQuat(model.rotation.x, model.rotation.y, model.rotation.z, model.rotation.w);
			Rotator = FRotator(q);

			AVine* vine = world->SpawnActor<AVine>(AVine::StaticClass(), Position, Rotator, SpawnParams);
		}
	}
	else if (treeLeafMode)
	{
		// default.map
		System->SetWord("FF+(45)F[J][/(30)J]");
		//TrunkType = TrunkMaterialType::Beech_I;

		std::vector<LSystem::UEModel>* models = new std::vector<LSystem::UEModel>();
		System->GetResultUEInfos(models);

		UWorld* world = GetWorld();

		for (const LSystem::UEModel& model : *models)
		{
			Position = FVector(-model.position.x, -model.position.z, model.position.y);
			FQuat q = FQuat(model.rotation.x, model.rotation.y, model.rotation.z, model.rotation.w);
			Rotator = FRotator(q);

			if (model.type == 1)
			{
				ATrunk* trunk = world->SpawnActor<ATrunk>(ATrunk::StaticClass(), Position, Rotator, SpawnParams);
			}
			else
			{
				ALeaf* leaf = world->SpawnActor<ALeaf>(ALeaf::StaticClass(), Position, Rotator, SpawnParams);
			}
		}
	}
}

// Called every frame
void AGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
