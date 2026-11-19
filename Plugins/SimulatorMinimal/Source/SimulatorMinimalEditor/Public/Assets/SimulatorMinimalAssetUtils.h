//------------------------------------------------------------------------------
// ModderAssetUtils.h
// Вспомогательные функции необходимые для импорта в ассет
//
// Copyright (c) 2022 Forward Development
// All rights reserved.
//
// Date:       06.11.26
// Author:     Кудрявцев Е.Б.
//------------------------------------------------------------------------------
#pragma once

#include "SimulatorMinimalAssetUtils.generated.h"

UCLASS()
class SIMULATORMINIMALEDITOR_API USimulatorMinimalAssetUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Import assets|UV")
	static void AddUVChannelTo(UStaticMesh* mesh, int uvChannel);

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Import assets|UV")
	static void GenerateUVsByVertexLocation(UStaticMesh* physicsMesh, int uvChannel);
};