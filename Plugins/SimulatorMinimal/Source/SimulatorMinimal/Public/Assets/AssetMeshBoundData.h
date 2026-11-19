//------------------------------------------------------------------------------
// AssetMeshBoundData.h
// Вспомогательные данные хранящие границы сгенерированные функцией GenerateUVsByVertexLocation
//
// Copyright (c) 2024 Forward Development
// All rights reserved.
//
// Date:       12.03.24
// Author:     Кудрявцев Е.Б.
//------------------------------------------------------------------------------
#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "AssetMeshBoundData.generated.h"

class UStaticMesh;

UCLASS(BlueprintType)
class SIMULATORMINIMAL_API UAssetMeshBoundData : public UAssetUserData
{
	GENERATED_BODY()
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MeshBound")
	FVector2D MinBound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MeshBound")
	FVector2D MaxBound;

	UAssetMeshBoundData(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "MeshBound")
	static UAssetMeshBoundData* GetAssetMeshBoundUserData(UStaticMesh* staticMesh);
};