//------------------------------------------------------------------------------
// VehicleSpecificationValue.h
// Vehicle specification value
//
// Copyright (c) 2026 Forward Development
// All rights reserved.
//
// Date:       27.10.26
// Author:     Гурко М.А.
//------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VehicleSpecificationValue.generated.h"


USTRUCT(BlueprintType)
struct FVehicleSpecificationBoundOption
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VehicleSpecificationValue")
	FString OptionName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VehicleSpecificationValue")
	FString OptionValue;
	
};

USTRUCT(BlueprintType)
struct FVehicleSpecificationValue
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VehicleSpecificationValue", meta = (DisplayPriority = 0))
	FText	DisplayName = NSLOCTEXT("VehicleSpecifications", "DefaultSpecValue", "DummyValue");
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VehicleSpecificationValue")
	bool	BoundToOption = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VehicleSpecificationValue", meta = (EditCondition = "BoundToOption", EditConditionHides))
	FVehicleSpecificationBoundOption BoundOption;
	
};

USTRUCT()
struct FVehicleSpecificationEngineType : public FVehicleSpecificationValue
{
	GENERATED_BODY()
	
	FVehicleSpecificationEngineType();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VehicleSpecificationValue", meta = (GetOptions = "VehicleSpecificationLibrary.GetFuelOptions", DisplayPriority = 1))
	FString	FuelType = "Gasoline";
	
};

USTRUCT(BlueprintType)
struct FCustomVehicleSpecification
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VehicleSpecificationValue")
	FText	Name = NSLOCTEXT("VehicleSpecifications", "DefaultSpecName", "Specification");
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VehicleSpecificationValue")
	FText	Value = NSLOCTEXT("VehicleSpecifications", "DefaultSpecValue", "Value");
	
};

USTRUCT(BlueprintType)
struct FCustomVehicleMultiSpecification
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VehicleSpecificationValue")
	FText	Name = NSLOCTEXT("VehicleSpecifications", "DefaultSpecName", "Specification");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VehicleSpecificationValue", meta = (TitleProperty = "DisplayName"))
	TArray<FVehicleSpecificationValue> Values = { FVehicleSpecificationValue() };
	
};

UCLASS(BlueprintType)
class UVehicleSpecifications : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UVehicleSpecifications();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VehicleSpecifications")
	FText Name = NSLOCTEXT("VehicleSpecifications", "VehicleName", "Vehicle");
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VehicleSpecifications")
	TSoftObjectPtr<UObject> Image;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VehicleSpecifications", meta = (GetOptions = "VehicleSpecificationLibrary.GetVehicleClassOptions"))
	FString Class = "Economy";
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VehicleSpecifications", meta = (TitleProperty = "DisplayName"))
	TArray<FVehicleSpecificationEngineType>	EngineTypes;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VehicleSpecifications", meta = (TitleProperty = "DisplayName"))
	TArray<FVehicleSpecificationValue>		TransmissionTypes;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VehicleSpecifications|Custom", meta = (TitleProperty = "Name"))
	TArray<FCustomVehicleSpecification>			CustomSpecifications;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VehicleSpecifications|Custom", meta = (TitleProperty = "Name"))
	TArray<FCustomVehicleMultiSpecification>	MultiSpecifications;
	
};

UCLASS()
class UVehicleSpecificationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "VehicleSpecifications")
	static TArray<FString> GetVehicleClassOptions();
	
	UFUNCTION(BlueprintPure, Category = "VehicleSpecifications")
	static TArray<FString> GetFuelOptions();
	
	UFUNCTION(BlueprintPure, Category = "VehicleSpecifications")
	static TArray<FString> GetTransmissionOptions();
	
	UFUNCTION(BlueprintPure, Category = "VehicleSpecifications")
	static FVehicleSpecificationEngineType GetDefaultEngineType();
	
	UFUNCTION(BlueprintPure, Category = "VehicleSpecifications")
	static FVehicleSpecificationValue GetDefaultTransmissionType();
	
	UFUNCTION(BlueprintCallable, Category = "VehicleSpecifications")
	static TArray<FCustomVehicleSpecification> CollectSingleSpecifications(UVehicleSpecifications* specifications);
	
	UFUNCTION(BlueprintCallable, Category = "VehicleSpecifications")
	static TArray<FCustomVehicleMultiSpecification> CollectMultiSpecifications(UVehicleSpecifications* specifications);
	
	UFUNCTION(BlueprintPure, Category = "VehicleSpecifications")
	static TArray<FText> CollectOptions(const TArray<FVehicleSpecificationValue>& specifications);
	
	UFUNCTION(BlueprintPure, Category = "VehicleSpecifications")
	static TArray<FText> CollectEngineOptions(const TArray<FVehicleSpecificationEngineType>& specifications);
	
};



