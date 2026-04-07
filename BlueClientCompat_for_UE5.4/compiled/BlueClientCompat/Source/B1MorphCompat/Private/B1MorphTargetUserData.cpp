#include "B1MorphTargetUserData.h"

#include "Animation/AnimInstance.h"
#include "Dom/JsonObject.h"
#include "Engine/SkeletalMesh.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

namespace
{
    static const TCHAR* LogPrefix = TEXT("[BlueClientMorphTools]");

    static bool ParseVector3fField(const TSharedPtr<FJsonObject>& VertexObject, const TCHAR* FieldName, FVector3f& OutVector)
    {
        const TSharedPtr<FJsonObject>* VectorObjectPtr = nullptr;
        if (!VertexObject->TryGetObjectField(FieldName, VectorObjectPtr) || !VectorObjectPtr || !(*VectorObjectPtr).IsValid())
        {
            return false;
        }

        const TSharedPtr<FJsonObject>& VectorObject = *VectorObjectPtr;
        double X = 0.0;
        double Y = 0.0;
        double Z = 0.0;
        if (!VectorObject->TryGetNumberField(TEXT("X"), X) || !VectorObject->TryGetNumberField(TEXT("Y"), Y) || !VectorObject->TryGetNumberField(TEXT("Z"), Z))
        {
            return false;
        }

        OutVector = FVector3f(static_cast<float>(X), static_cast<float>(Y), static_cast<float>(Z));
        return true;
    }

    static bool ParseMorphTargetDatasFromFModelJson(const FString& JsonText, TArray<FB1MorphTargetData>& OutMorphTargetDatas, FString& OutMessage)
    {
        OutMorphTargetDatas.Reset();

        TArray<TSharedPtr<FJsonValue>> RootArray;
        const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);
        if (!FJsonSerializer::Deserialize(Reader, RootArray))
        {
            OutMessage = TEXT("Failed to parse JSON.");
            return false;
        }

        const TSharedPtr<FJsonObject>* B1Object = nullptr;
        for (const TSharedPtr<FJsonValue>& Entry : RootArray)
        {
            if (!Entry.IsValid() || Entry->Type != EJson::Object)
            {
                continue;
            }

            const TSharedPtr<FJsonObject>& Object = Entry->AsObject();
            FString Type;
            if (Object.IsValid() && Object->TryGetStringField(TEXT("Type"), Type) && Type == TEXT("B1MorphTargetUserData"))
            {
                B1Object = &Object;
                break;
            }
        }

        if (!B1Object || !(*B1Object).IsValid())
        {
            OutMessage = TEXT("No B1MorphTargetUserData object found in JSON.");
            return false;
        }

        const TSharedPtr<FJsonObject>* PropertiesObject = nullptr;
        if (!(*B1Object)->TryGetObjectField(TEXT("Properties"), PropertiesObject) || !PropertiesObject || !(*PropertiesObject).IsValid())
        {
            OutMessage = TEXT("B1MorphTargetUserData has no Properties object.");
            return false;
        }

        const TArray<TSharedPtr<FJsonValue>>* MorphDatasJson = nullptr;
        if (!(*PropertiesObject)->TryGetArrayField(TEXT("MorphTargetDatas"), MorphDatasJson) || !MorphDatasJson)
        {
            OutMessage = TEXT("Properties.MorphTargetDatas not found.");
            return false;
        }

        OutMorphTargetDatas.Reserve(MorphDatasJson->Num());
        for (const TSharedPtr<FJsonValue>& MorphValue : *MorphDatasJson)
        {
            if (!MorphValue.IsValid() || MorphValue->Type != EJson::Object)
            {
                continue;
            }

            const TSharedPtr<FJsonObject>& MorphObject = MorphValue->AsObject();
            if (!MorphObject.IsValid())
            {
                continue;
            }

            FB1MorphTargetData MorphData;

            FString MorphName;
            MorphObject->TryGetStringField(TEXT("Name"), MorphName);
            MorphData.Name = FName(*MorphName);

            const TArray<TSharedPtr<FJsonValue>>* LodInfosJson = nullptr;
            if (MorphObject->TryGetArrayField(TEXT("LODInfos"), LodInfosJson) && LodInfosJson)
            {
                MorphData.LODInfos.Reserve(LodInfosJson->Num());
                for (const TSharedPtr<FJsonValue>& LodValue : *LodInfosJson)
                {
                    if (!LodValue.IsValid() || LodValue->Type != EJson::Object)
                    {
                        continue;
                    }

                    const TSharedPtr<FJsonObject>& LodObject = LodValue->AsObject();
                    if (!LodObject.IsValid())
                    {
                        continue;
                    }

                    FB1MorphTargetLODInfo LodInfo;
                    const TArray<TSharedPtr<FJsonValue>>* VerticesJson = nullptr;
                    if (LodObject->TryGetArrayField(TEXT("Vertices"), VerticesJson) && VerticesJson)
                    {
                        LodInfo.Vertices.Reserve(VerticesJson->Num());
                        for (const TSharedPtr<FJsonValue>& VertexValue : *VerticesJson)
                        {
                            if (!VertexValue.IsValid() || VertexValue->Type != EJson::Object)
                            {
                                continue;
                            }

                            const TSharedPtr<FJsonObject>& VertexObject = VertexValue->AsObject();
                            if (!VertexObject.IsValid())
                            {
                                continue;
                            }

                            FB1MorphTargetDelta Delta;

                            int32 SourceIdx = 0;
                            if (VertexObject->TryGetNumberField(TEXT("SourceIdx"), SourceIdx))
                            {
                                Delta.SourceIdx = SourceIdx;
                            }

                            ParseVector3fField(VertexObject, TEXT("PositionDelta"), Delta.PositionDelta);
                            ParseVector3fField(VertexObject, TEXT("TangentZDelta"), Delta.TangentZDelta);

                            LodInfo.Vertices.Add(MoveTemp(Delta));
                        }
                    }

                    MorphData.LODInfos.Add(MoveTemp(LodInfo));
                }
            }

            OutMorphTargetDatas.Add(MoveTemp(MorphData));
        }

        OutMessage = FString::Printf(TEXT("Parsed %d MorphTargetDatas entries."), OutMorphTargetDatas.Num());
        return true;
    }
}

UB1MorphTargetUserData* UBlueClientMorphTools::GetB1MorphUserData(USkeletalMesh* Mesh)
{
    if (!Mesh)
    {
        return nullptr;
    }

    return Cast<UB1MorphTargetUserData>(Mesh->GetAssetUserDataOfClass(UB1MorphTargetUserData::StaticClass()));
}

bool UBlueClientMorphTools::CopyB1MorphUserData(USkeletalMesh* SourceMesh, USkeletalMesh* TargetMesh, bool bCopyPostProcessAnimBlueprint)
{
    if (!SourceMesh || !TargetMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s CopyB1MorphUserData failed: SourceMesh or TargetMesh is null."), LogPrefix);
        return false;
    }

    UB1MorphTargetUserData* SourceData = Cast<UB1MorphTargetUserData>(
        SourceMesh->GetAssetUserDataOfClass(UB1MorphTargetUserData::StaticClass()));

    if (!SourceData)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s CopyB1MorphUserData failed: source mesh has no UB1MorphTargetUserData."), LogPrefix);
        return false;
    }

    TargetMesh->Modify();
    TargetMesh->RemoveUserDataOfClass(UB1MorphTargetUserData::StaticClass());

    UB1MorphTargetUserData* NewData = NewObject<UB1MorphTargetUserData>(
        TargetMesh,
        UB1MorphTargetUserData::StaticClass(),
        NAME_None,
        RF_Public | RF_Transactional);

    NewData->MorphTargetDatas = SourceData->MorphTargetDatas;
    TargetMesh->AddAssetUserData(NewData);

    if (bCopyPostProcessAnimBlueprint)
    {
        TargetMesh->SetPostProcessAnimBlueprint(SourceMesh->GetPostProcessAnimBlueprint());
    }

    TargetMesh->MarkPackageDirty();
    UE_LOG(LogTemp, Display, TEXT("%s CopyB1MorphUserData success: copied %d MorphTargetDatas entries to %s."), LogPrefix, NewData->MorphTargetDatas.Num(), *TargetMesh->GetName());
    return true;
}

bool UBlueClientMorphTools::ImportB1MorphUserDataFromFModelJsonFile(USkeletalMesh* TargetMesh, const FString& JsonFilePath, FString& OutMessage)
{
    UE_LOG(LogTemp, Display, TEXT("%s Import called. Target=%s JsonPath=%s"), LogPrefix, TargetMesh ? *TargetMesh->GetName() : TEXT("<null>"), *JsonFilePath);

    if (!TargetMesh)
    {
        OutMessage = TEXT("TargetMesh is null.");
        UE_LOG(LogTemp, Warning, TEXT("%s %s"), LogPrefix, *OutMessage);
        return false;
    }

    FString JsonText;
    if (!FFileHelper::LoadFileToString(JsonText, *JsonFilePath))
    {
        OutMessage = FString::Printf(TEXT("Failed to read file: %s"), *JsonFilePath);
        UE_LOG(LogTemp, Warning, TEXT("%s %s"), LogPrefix, *OutMessage);
        return false;
    }

    TArray<FB1MorphTargetData> ParsedMorphTargetDatas;
    if (!ParseMorphTargetDatasFromFModelJson(JsonText, ParsedMorphTargetDatas, OutMessage))
    {
        UE_LOG(LogTemp, Warning, TEXT("%s Parse failed: %s"), LogPrefix, *OutMessage);
        return false;
    }

    TargetMesh->Modify();
    TargetMesh->RemoveUserDataOfClass(UB1MorphTargetUserData::StaticClass());

    UB1MorphTargetUserData* NewData = NewObject<UB1MorphTargetUserData>(
        TargetMesh,
        UB1MorphTargetUserData::StaticClass(),
        NAME_None,
        RF_Public | RF_Transactional);

    NewData->MorphTargetDatas = MoveTemp(ParsedMorphTargetDatas);
    TargetMesh->AddAssetUserData(NewData);
    TargetMesh->MarkPackageDirty();

    OutMessage = FString::Printf(TEXT("Imported %d MorphTargetDatas entries into %s."), NewData->MorphTargetDatas.Num(), *TargetMesh->GetName());
    UE_LOG(LogTemp, Display, TEXT("%s %s"), LogPrefix, *OutMessage);

    return true;
}

bool UBlueClientMorphTools::SetPostProcessAnimBlueprintByClassPath(USkeletalMesh* TargetMesh, const FString& AnimBlueprintGeneratedClassPath, FString& OutMessage)
{
    if (!TargetMesh)
    {
        OutMessage = TEXT("TargetMesh is null.");
        UE_LOG(LogTemp, Warning, TEXT("%s %s"), LogPrefix, *OutMessage);
        return false;
    }

    // First try loading the class (works if the asset is in this project)
    UClass* LoadedClass = LoadObject<UClass>(nullptr, *AnimBlueprintGeneratedClassPath);

    if (LoadedClass)
    {
        if (!LoadedClass->IsChildOf(UAnimInstance::StaticClass()))
        {
            OutMessage = TEXT("Loaded class is not an AnimInstance class.");
            UE_LOG(LogTemp, Warning, TEXT("%s %s"), LogPrefix, *OutMessage);
            return false;
        }
        TargetMesh->Modify();
        TargetMesh->SetPostProcessAnimBlueprint(LoadedClass);
        TargetMesh->MarkPackageDirty();
        OutMessage = TEXT("PostProcessAnimBlueprint set successfully (loaded class).");
        UE_LOG(LogTemp, Display, TEXT("%s %s Path=%s"), LogPrefix, *OutMessage, *AnimBlueprintGeneratedClassPath);
        return true;
    }

    // Asset is not in this project (e.g. lives in the base game paks).
    // Write the soft reference directly via the property so it survives cooking.
    UE_LOG(LogTemp, Warning, TEXT("%s Class not found in project, writing raw soft reference: %s"), LogPrefix, *AnimBlueprintGeneratedClassPath);

    FProperty* Prop = USkeletalMesh::StaticClass()->FindPropertyByName(FName("PostProcessAnimBlueprint"));
    if (!Prop)
    {
        OutMessage = TEXT("Could not find PostProcessAnimBlueprint property via reflection.");
        UE_LOG(LogTemp, Error, TEXT("%s %s"), LogPrefix, *OutMessage);
        return false;
    }

    FSoftClassProperty* SoftClassProp = CastField<FSoftClassProperty>(Prop);
    if (!SoftClassProp)
    {
        OutMessage = TEXT("PostProcessAnimBlueprint is not a FSoftClassProperty - engine version mismatch?");
        UE_LOG(LogTemp, Error, TEXT("%s %s"), LogPrefix, *OutMessage);
        return false;
    }

    TargetMesh->Modify();

    void* ValuePtr = SoftClassProp->ContainerPtrToValuePtr<void>(TargetMesh);
    FSoftObjectPtr* SoftPtr = reinterpret_cast<FSoftObjectPtr*>(ValuePtr);
    *SoftPtr = FSoftObjectPath(AnimBlueprintGeneratedClassPath);

    TargetMesh->MarkPackageDirty();

    OutMessage = FString::Printf(TEXT("PostProcessAnimBlueprint set via soft path (will resolve at runtime): %s"), *AnimBlueprintGeneratedClassPath);
    UE_LOG(LogTemp, Display, TEXT("%s %s"), LogPrefix, *OutMessage);
    return true;
}
