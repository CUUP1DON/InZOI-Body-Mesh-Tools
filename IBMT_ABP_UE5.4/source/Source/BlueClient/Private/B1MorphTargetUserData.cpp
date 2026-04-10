#include "B1MorphTargetUserData.h"

#include "Animation/AnimInstance.h"
#include "Dom/JsonObject.h"
#include "Engine/SkeletalMesh.h"
#include "Misc/FileHelper.h"
#include "Rendering/SkeletalMeshRenderData.h"
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

bool UBlueClientMorphTools::ParseB1MorphDataFromJsonFile(const FString& JsonFilePath, TArray<FB1MorphTargetData>& OutMorphTargetDatas, FString& OutMessage)
{
    FString JsonText;
    if (!FFileHelper::LoadFileToString(JsonText, *JsonFilePath))
    {
        OutMessage = FString::Printf(TEXT("Failed to read file: %s"), *JsonFilePath);
        return false;
    }
    return ParseMorphTargetDatasFromFModelJson(JsonText, OutMorphTargetDatas, OutMessage);
}

bool UBlueClientMorphTools::ImportMorphFromBlenderJson(USkeletalMesh* TargetMesh, const FString& BlenderJsonPath, FString& OutMessage)
{
    if (!TargetMesh)
    {
        OutMessage = TEXT("TargetMesh is null.");
        return false;
    }

    FString JsonText;
    if (!FFileHelper::LoadFileToString(JsonText, *BlenderJsonPath))
    {
        OutMessage = FString::Printf(TEXT("Failed to read Blender JSON: %s"), *BlenderJsonPath);
        return false;
    }

    TSharedPtr<FJsonObject> Root;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);
    if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
    {
        OutMessage = TEXT("Failed to parse Blender JSON.");
        return false;
    }

    FString MorphName;
    if (!Root->TryGetStringField(TEXT("morphName"), MorphName))
    {
        OutMessage = TEXT("Blender JSON missing 'morphName' field.");
        return false;
    }

    const TArray<TSharedPtr<FJsonValue>>* VerticesJson = nullptr;
    if (!Root->TryGetArrayField(TEXT("vertices"), VerticesJson) || !VerticesJson)
    {
        OutMessage = TEXT("Blender JSON missing 'vertices' array.");
        return false;
    }

    // ── Parse Blender vertex data ────────────────────────────────────────────
    // Blender exports world-space positions in meters (Z-up, -Y forward).
    // UE render buffer positions are in centimetres (Z-up, X-forward).
    // Conversion: UE.X = Blender.X * 100, UE.Y = -Blender.Y * 100, UE.Z = Blender.Z * 100

    struct FBlenderVertex
    {
        FVector3f Position;         // converted to UE cm
        FVector3f PositionDelta;    // converted to UE cm
        FVector3f TangentZDelta;    // deformedNormal - basisNormal (axis-converted)
    };

    TArray<FBlenderVertex> BlenderVerts;
    BlenderVerts.Reserve(VerticesJson->Num());

    auto ReadVec3 = [](const TSharedPtr<FJsonObject>& Obj, const TCHAR* Key, FVector3f& Out) -> bool
    {
        const TArray<TSharedPtr<FJsonValue>>* Arr = nullptr;
        if (!Obj->TryGetArrayField(Key, Arr) || !Arr || Arr->Num() < 3) return false;
        Out.X = static_cast<float>((*Arr)[0]->AsNumber());
        Out.Y = static_cast<float>((*Arr)[1]->AsNumber());
        Out.Z = static_cast<float>((*Arr)[2]->AsNumber());
        return true;
    };

    auto BlenderToUE = [](const FVector3f& B, bool bScale) -> FVector3f
    {
        // Blender (X, Y, Z) -> UE (X, -Y, Z), scale metres -> cm
        const float S = bScale ? 100.0f : 1.0f;
        return FVector3f(B.X * S, -B.Y * S, B.Z * S);
    };

    for (const TSharedPtr<FJsonValue>& Val : *VerticesJson)
    {
        if (!Val.IsValid() || Val->Type != EJson::Object) continue;
        const TSharedPtr<FJsonObject>& V = Val->AsObject();
        if (!V.IsValid()) continue;

        FVector3f BasisPos, Delta, BasisNormal, DeformedNormal;
        if (!ReadVec3(V, TEXT("basisPos"),       BasisPos))       continue;
        if (!ReadVec3(V, TEXT("delta"),           Delta))          continue;
        if (!ReadVec3(V, TEXT("basisNormal"),     BasisNormal))    continue;
        if (!ReadVec3(V, TEXT("deformedNormal"),  DeformedNormal)) continue;

        FBlenderVertex BV;
        BV.Position      = BlenderToUE(BasisPos,      /*bScale=*/true);
        BV.PositionDelta = BlenderToUE(Delta,          /*bScale=*/true);

        // TangentZDelta = deformed normal - basis normal (both unit, axis-converted)
        const FVector3f UEBasisN    = BlenderToUE(BasisNormal,    /*bScale=*/false);
        const FVector3f UEDeformedN = BlenderToUE(DeformedNormal, /*bScale=*/false);
        BV.TangentZDelta = UEDeformedN - UEBasisN;

        BlenderVerts.Add(BV);
    }

    if (BlenderVerts.Num() == 0)
    {
        OutMessage = TEXT("No valid vertex entries found in Blender JSON.");
        return false;
    }

    // ── Match Blender vertices to ALL UE render-buffer vertices ──────────────
    // Each Blender vertex position is matched against render-buffer vertices on
    // every LOD of TargetMesh by spatial proximity (within SeamEpsilon cm).
    // All co-located render verts (UV-seam splits) receive the same delta,
    // which keeps seams closed when the morph is applied.

    FSkeletalMeshRenderData* RD = TargetMesh->GetResourceForRendering();
    if (!RD || RD->LODRenderData.Num() == 0)
    {
        OutMessage = TEXT("Target mesh has no render data. Open the mesh in the editor first.");
        return false;
    }

    static constexpr float SeamEpsilon = 0.1f; // 1 mm — generous for seam splits
    static constexpr float CellSize    = 5.0f; // spatial hash cell size in cm

    // For each LOD, build a position -> renderVtxIndex spatial hash, then match
    const int32 NumLODs = RD->LODRenderData.Num();
    TArray<FB1MorphTargetLODInfo> LODInfos;
    LODInfos.SetNum(NumLODs);

    for (int32 LOD = 0; LOD < NumLODs; ++LOD)
    {
        const FSkeletalMeshLODRenderData& LODData = RD->LODRenderData[LOD];
        const int32 NumRenderVerts = LODData.StaticVertexBuffers.PositionVertexBuffer.GetNumVertices();

        // Build spatial hash: cell -> list of render vert indices
        TMap<FIntVector, TArray<int32>> Grid;
        Grid.Reserve(NumRenderVerts);
        for (int32 i = 0; i < NumRenderVerts; ++i)
        {
            const FVector3f& P = LODData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(i);
            const FIntVector Cell(
                FMath::FloorToInt(P.X / CellSize),
                FMath::FloorToInt(P.Y / CellSize),
                FMath::FloorToInt(P.Z / CellSize));
            Grid.FindOrAdd(Cell).Add(i);
        }

        TArray<FB1MorphTargetDelta>& Deltas = LODInfos[LOD].Vertices;
        Deltas.Reserve(BlenderVerts.Num() * 2); // 2x estimate for seam splits

        for (const FBlenderVertex& BV : BlenderVerts)
        {
            const FIntVector QueryCell(
                FMath::FloorToInt(BV.Position.X / CellSize),
                FMath::FloorToInt(BV.Position.Y / CellSize),
                FMath::FloorToInt(BV.Position.Z / CellSize));

            // Pass 1: find nearest render vert
            int32 BestIdx  = INDEX_NONE;
            float BestDist = FLT_MAX;
            for (int32 Radius = 0; Radius <= 8; ++Radius)
            {
                if (BestIdx != INDEX_NONE && static_cast<float>(Radius) * CellSize > BestDist)
                    break;
                for (int32 dx = -Radius; dx <= Radius; ++dx)
                for (int32 dy = -Radius; dy <= Radius; ++dy)
                for (int32 dz = -Radius; dz <= Radius; ++dz)
                {
                    if (FMath::Max3(FMath::Abs(dx), FMath::Abs(dy), FMath::Abs(dz)) != Radius)
                        continue;
                    const TArray<int32>* Bucket = Grid.Find(QueryCell + FIntVector(dx, dy, dz));
                    if (!Bucket) continue;
                    for (const int32 Cand : *Bucket)
                    {
                        const float D = FVector3f::Dist(BV.Position,
                            LODData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(Cand));
                        if (D < BestDist) { BestDist = D; BestIdx = Cand; }
                    }
                }
            }
            if (BestIdx == INDEX_NONE) continue;

            // Pass 2: collect ALL render verts within SeamEpsilon of BestDist
            const float Threshold = BestDist + SeamEpsilon;
            for (int32 Radius = 0; Radius <= 8; ++Radius)
            {
                if (static_cast<float>(Radius) * CellSize > Threshold + CellSize)
                    break;
                for (int32 dx = -Radius; dx <= Radius; ++dx)
                for (int32 dy = -Radius; dy <= Radius; ++dy)
                for (int32 dz = -Radius; dz <= Radius; ++dz)
                {
                    if (FMath::Max3(FMath::Abs(dx), FMath::Abs(dy), FMath::Abs(dz)) != Radius)
                        continue;
                    const TArray<int32>* Bucket = Grid.Find(QueryCell + FIntVector(dx, dy, dz));
                    if (!Bucket) continue;
                    for (const int32 Cand : *Bucket)
                    {
                        const float D = FVector3f::Dist(BV.Position,
                            LODData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(Cand));
                        if (D <= Threshold)
                        {
                            FB1MorphTargetDelta Delta;
                            Delta.SourceIdx     = Cand;
                            Delta.PositionDelta = BV.PositionDelta;
                            Delta.TangentZDelta = BV.TangentZDelta;
                            Deltas.AddUnique(Delta); // AddUnique by SourceIdx
                        }
                    }
                }
            }
        }
        Deltas.Shrink();
    }

    // ── Write to target mesh ─────────────────────────────────────────────────
    FB1MorphTargetData MorphData;
    MorphData.Name     = FName(*MorphName);
    MorphData.LODInfos = MoveTemp(LODInfos);

    TArray<FB1MorphTargetData> MorphArray;
    MorphArray.Add(MoveTemp(MorphData));

    TargetMesh->Modify();
    TargetMesh->RemoveUserDataOfClass(UB1MorphTargetUserData::StaticClass());

    UB1MorphTargetUserData* NewUD = NewObject<UB1MorphTargetUserData>(
        TargetMesh, UB1MorphTargetUserData::StaticClass(), NAME_None, RF_Public | RF_Transactional);
    NewUD->MorphTargetDatas = MoveTemp(MorphArray);
    TargetMesh->AddAssetUserData(NewUD);
    TargetMesh->MarkPackageDirty();

    int32 TotalDeltas = 0;
    for (const FB1MorphTargetLODInfo& LI : NewUD->MorphTargetDatas[0].LODInfos)
        TotalDeltas += LI.Vertices.Num();

    OutMessage = FString::Printf(
        TEXT("Blender import complete: %d Blender deltas -> %d render-buffer deltas across %d LODs written to %s."),
        BlenderVerts.Num(), TotalDeltas, NewUD->MorphTargetDatas[0].LODInfos.Num(), *TargetMesh->GetName());
    UE_LOG(LogTemp, Display, TEXT("%s %s"), LogPrefix, *OutMessage);
    return true;
}
