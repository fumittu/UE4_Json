// Copyright Epic Games, Inc. All Rights Reserved.


#include "JsonLibGameModeBase.h"
#include "JsonUtilities/Public/JsonUtilities.h"
#include "Runtime/Json/Public/Serialization/JsonReader.h"
#include "Misc/DateTime.h"

static const FString RootName("InventoryObject");
static const FString WritePath(FPaths::ProjectSavedDir() / "Json");
static const FString FileName("JsonData.json");
static const FString FilePathFull(WritePath / FileName);

/**
 * Json文字列作成
 * @param SavedDate    ファイル書き出し日時
 * @param CharName     キャラクター名
 * @param ItemsActors  レベル上のオブジェクト
 * @param Count        数値のカウント
 * @return 書き出しに成功したか
 */
bool AJsonLibGameModeBase::WriteJsonData(FDateTime& SavedDate, FString CharName, TArray<AActor*> ItemsActors, TArray<int> Count)
{
	JsonObjectPtr JsonRootObject = MakeShareable(new FJsonObject);

	const FString SaveDate = FDateTime::UtcNow().ToString();
	JsonRootObject->SetStringField("SaveDate", SaveDate);

	JsonRootObject->SetStringField("Character", CharName);

	TArray<TSharedPtr<FJsonValue>> ItemObjects;

	for (AActor* obj : ItemsActors)
	{
		ItemObjects.Add(MakeShareable(new FJsonValueString(obj->GetFullName())));
	}
	JsonRootObject->SetArrayField("Items", ItemObjects);

	TArray<TSharedPtr<FJsonValue>> ItemCountArray;
	for (int i : Count)
	{
		ItemCountArray.Add(MakeShareable(new FJsonValueNumber(i)));
	}
	JsonRootObject->SetArrayField("ItemCount", ItemCountArray);

	return WriteJsonFile(JsonRootObject, FilePathFull);
}

/**
 * Json文字列解析
 * @param SavedDate    ファイル書き出し日時
 * @param CharName     キャラクター名
 * @param ItemsActors  レベル上のオブジェクト
 * @param Count        数値のカウント
 * @return 解析にに成功したか
 */
bool AJsonLibGameModeBase::ReadJsonData(FDateTime & SavedDate, FString & CharName, TArray<FString>& Items, TArray<int>& Count)
{
	JsonObjectPtr JsonRootObject = ReadJsonFile(FilePathFull);

	if(JsonRootObject)
	{
		FDateTime::Parse(JsonRootObject->GetStringField("SaveDate"), SavedDate);

		CharName = JsonRootObject->GetStringField("Character");

		for (TSharedPtr<FJsonValue> V : JsonRootObject->GetArrayField("Items"))
		{
			Items.Add(V->AsString());
		}

		for (TSharedPtr<FJsonValue> V : JsonRootObject->GetArrayField("ItemCount"))
		{
			Count.Add(V->AsNumber());
		}
		return true;
	}
	return false;
}

/**
 * 値が空のJsonファイル作成(Jsonファイルの雛型の作成)
 */
void AJsonLibGameModeBase::GenerateBaseFile()
{
	JsonObjectPtr root = MakeShareable(new FJsonObject);

	const FString SaveDate = FDateTime::UtcNow().ToString();
	root->SetStringField("SaveDate", SaveDate);

	root->SetStringField("Character", FString());

	TArray<TSharedPtr<FJsonValue>> ItemObjects;
	root->SetArrayField("Items", ItemObjects);

	TArray<TSharedPtr<FJsonValue>> ItemCountArray;
	root->SetArrayField("ItemCount", ItemCountArray);

	WriteJsonFile(root, FilePathFull);
}

/**
* Jsonファイル書き出し
* @param object       Jsonオブジェクト
* @param FullPath     書き出し先
* @param bCondensed   Jsonを成形して出力するかしないか
* @return　書き出しに成功したか
*/
bool AJsonLibGameModeBase::WriteJsonFile(JsonObjectPtr object, FString FullPath, bool bCondensed)
{
	FString OutPutString;

	if (bCondensed)
	{
		TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutPutString);
		FJsonSerializer::Serialize(object.ToSharedRef(), Writer);
	}
	else
	{
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutPutString);
		FJsonSerializer::Serialize(object.ToSharedRef(), Writer);
	}
	return FFileHelper::SaveStringToFile(OutPutString, *FullPath);
}

/**
* Jsonファイル読み込み
* @param FullPath     読み込み先のパス
* @return 読み込んだJsonオブジェクト
*/
JsonObjectPtr AJsonLibGameModeBase::ReadJsonFile(FString FullPath)
{
	FString RawData;

	bool bLoadedFile = FFileHelper::LoadFileToString(RawData, *FilePathFull);
	JsonObjectPtr JsonRootObject = MakeShareable(new FJsonObject());

	if (bLoadedFile)
	{
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(RawData);
		FJsonSerializer::Deserialize(JsonReader, JsonRootObject);
	}
	return JsonRootObject;
}
