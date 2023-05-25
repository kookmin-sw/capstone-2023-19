// Fill out your copyright notice in the Description page of Project Settings.

#include "Stdafx.h"
#include "CommonStructure.hpp"
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "LSystem.hpp"
#include "LRule.hpp"
#include "LLetter.hpp"
#include "Vine.h"
#include "VineGenerator.h"

// Sets default values
AVineGenerator::AVineGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	System = new LSystem();
	Position = FVector::ZeroVector;
	renderTime = -5.0f;
	vines = new std::vector<AVine*>();

}

// Called when the game starts or when spawned
void AVineGenerator::BeginPlay()
{
	Super::BeginPlay();

	tryCount = 0;

	// vine.map
	System->SetAngleChange(22.5f);
	System->SetWord("F(0.1875)");
	System->AddRule("F(t)", "F(t)F(t)-[-F(t)+F(t)+F(t)]+[+F(t)-F(t)-F(t)]");
	//System->Iterate(3);
	iterateCount = 3;
	iterateCycle = 5.f;
}

// Called every frame
void AVineGenerator::Tick(float DeltaTime)
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

void AVineGenerator::Iterate()
{

	if (tryCount >= iterateCount)
	{
		return;
	}

	Clear();
	System->Iterate();

	std::vector<LSystem::UEModel>* models = new std::vector<LSystem::UEModel>();
	System->GetResultUEInfos(models);

	UWorld* world = GetWorld();

	for (const LSystem::UEModel& model : *models)
	{
		Position = FVector(-model.position.x, -model.position.z, model.position.y);
		FQuat q = FQuat(model.rotation.x, model.rotation.y, model.rotation.z, model.rotation.w);
		Rotator = FRotator(q);

		AVine* vine = world->SpawnActor<AVine>(AVine::StaticClass(), Position, Rotator, SpawnParams);
		vines->push_back(vine);
	}
}

void AVineGenerator::Clear()
{
	for (AVine* vine : *vines)
	{
		vine->Destroy();
	}
}