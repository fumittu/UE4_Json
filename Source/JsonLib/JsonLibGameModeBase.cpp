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
 * Json������쐬
 * @param SavedDate    �t�@�C�������o������
 * @param CharName     �L�����N�^�[��
 * @param ItemsActors  ���x����̃I�u�W�F�N�g
 * @param Count        ���l�̃J�E���g
 * @return �����o���ɐ���������
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
 * Json��������
 * @param SavedDate    �t�@�C�������o������
 * @param CharName     �L�����N�^�[��
 * @param ItemsActors  ���x����̃I�u�W�F�N�g
 * @param Count        ���l�̃J�E���g
 * @return ��͂ɂɐ���������
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
 * �l�����Json�t�@�C���쐬(Json�t�@�C���̐��^�̍쐬)
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
* Json�t�@�C�������o��
* @param object       Json�I�u�W�F�N�g
* @param FullPath     �����o����
* @param bCondensed   Json�𐬌`���ďo�͂��邩���Ȃ���
* @return�@�����o���ɐ���������
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
* Json�t�@�C���ǂݍ���
* @param FullPath     �ǂݍ��ݐ�̃p�X
* @return �ǂݍ���Json�I�u�W�F�N�g
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
