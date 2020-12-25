// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Dom/JsonObject.h"
#include "JsonLibGameModeBase.generated.h"

typedef TSharedPtr<FJsonObject> JsonObjectPtr;
typedef TSharedPtr<FJsonValue> JsonValuePtr;

/**
 * 
 */
UCLASS()
class JSONLIB_API AJsonLibGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "Json")
	static bool WriteJsonData(FDateTime& SavedDate, FString CharName, TArray<AActor*> Items, TArray<int> Count);

	UFUNCTION(BlueprintCallable, Category = "Json")
	static bool ReadJsonData(FDateTime& SavedDate, FString& CharName, TArray<FString>& Items, TArray<int>& Count);

	UFUNCTION(BlueprintCallable, Category = "Json")
	static void GenerateBaseFile();

private:
	
	static bool WriteJsonFile(JsonObjectPtr object, FString FullPath, bool bCondensed = false);

	
	static JsonObjectPtr ReadJsonFile(FString FullPath);
};
