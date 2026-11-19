//------------------------------------------------------------------------------
// ControlAssetData.h
// Control user asset data for forward some properties within actor component.
//
// Copyright (c) 2026 Forward Development
// All rights reserved.
//
// Date:       28.10.26
// Author:     Кудрявцев Е.Б.
//------------------------------------------------------------------------------
#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/SubclassOf.h"
#include "ControlAssetData.generated.h"

UCLASS(BlueprintType, Abstract, EditInlineNew, DefaultToInstanced, CollapseCategories)
class SIMULATORMINIMAL_API UControlPropertyBase : public UObject
{
	GENERATED_BODY()
};

UCLASS()
class SIMULATORMINIMAL_API UControlPropertyString : public UControlPropertyBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UserData|Control")
	FString Value;

};

UCLASS()
class SIMULATORMINIMAL_API UControlPropertyFloat : public UControlPropertyBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UserData|Control")
	float Value;

};

UCLASS()
class SIMULATORMINIMAL_API UControlPropertyInt : public UControlPropertyBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UserData|Control")
	int Value;

};

UCLASS()
class SIMULATORMINIMAL_API UControlPropertyBool : public UControlPropertyBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UserData|Control")
	bool Value;

};

UCLASS()
class SIMULATORMINIMAL_API UControlPropertyObject : public UControlPropertyBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UserData|Control")
	UObject* Value;

};

UCLASS()
class SIMULATORMINIMAL_API UControlPropertyClass : public UControlPropertyBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UserData|Control")
	UClass* Value;

};

UCLASS()
class SIMULATORMINIMAL_API UControlPropertyStringArray : public UControlPropertyBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UserData|Control", meta = (FullyExpand = true))
	TArray<FString> Values;

};

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced, CollapseCategories)
class SIMULATORMINIMAL_API UControlBlock : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "UserData|Control", meta = (DeterminesOutputType = "propertyClass", DynamicOutputParam = "outProperty"))
	bool GetProperty(FName Name, UPARAM(meta = (AllowAbstract = false)) TSubclassOf<UControlPropertyBase> propertyClass, UControlPropertyBase*& outProperty) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "UserData|Control")
	TMap<FName, UControlPropertyBase*> Properties;

};

UCLASS(BlueprintType, CollapseCategories)
class SIMULATORMINIMAL_API UControlAssetData : public UAssetUserData
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "UserData|Control")
	UControlBlock* GetControl(FName name) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "UserData|Control")
	TMap<FName, UControlBlock*> ControlBlocks;

};

UCLASS()
class SIMULATORMINIMAL_API UControlAssetDataStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "UserData|Control")
	static UControlAssetData* GetControlAssetData(UObject* object);

	UFUNCTION(BlueprintCallable, Category = "UserData|Control")
	static UControlBlock* GetControl(UObject* object, FName name);
};