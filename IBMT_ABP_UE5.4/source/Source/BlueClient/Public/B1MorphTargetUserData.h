#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "B1MorphTargetUserData.generated.h"

class USkeletalMesh;
class UAnimInstance;

USTRUCT(BlueprintType)
struct FB1MorphTargetDelta
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "inZOI")
    int32 SourceIdx = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "inZOI")
    FVector3f PositionDelta = FVector3f::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "inZOI")
    FVector3f TangentZDelta = FVector3f::ZeroVector;

    // Equality is by SourceIdx only — used by AddUnique when expanding seam vertices
    bool operator==(const FB1MorphTargetDelta& Other) const { return SourceIdx == Other.SourceIdx; }
};

USTRUCT(BlueprintType)
struct FB1MorphTargetLODInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "inZOI")
    TArray<FB1MorphTargetDelta> Vertices;
};

USTRUCT(BlueprintType)
struct FB1MorphTargetData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "inZOI")
    FName Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "inZOI")
    TArray<FB1MorphTargetLODInfo> LODInfos;
};

UCLASS(BlueprintType)
class BLUECLIENT_API UB1MorphTargetUserData : public UAssetUserData
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "inZOI")
    TArray<FB1MorphTargetData> MorphTargetDatas;
};

UCLASS()
class BLUECLIENT_API UBlueClientMorphTools : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, CallInEditor, Category = "inZOI|Morph")
    static bool CopyB1MorphUserData(USkeletalMesh* SourceMesh, USkeletalMesh* TargetMesh, bool bCopyPostProcessAnimBlueprint);

    UFUNCTION(BlueprintCallable, CallInEditor, Category = "inZOI|Morph")
    static bool ImportB1MorphUserDataFromFModelJsonFile(USkeletalMesh* TargetMesh, const FString& JsonFilePath, FString& OutMessage);

    UFUNCTION(BlueprintCallable, CallInEditor, Category = "inZOI|Morph")
    static bool SetPostProcessAnimBlueprintByClassPath(USkeletalMesh* TargetMesh, const FString& AnimBlueprintGeneratedClassPath, FString& OutMessage);

    UFUNCTION(BlueprintCallable, Category = "inZOI|Morph")
    static bool ParseB1MorphDataFromJsonFile(const FString& JsonFilePath, TArray<FB1MorphTargetData>& OutMorphTargetDatas, FString& OutMessage);

    UFUNCTION(BlueprintCallable, Category = "inZOI|Morph")
    static UB1MorphTargetUserData* GetB1MorphUserData(USkeletalMesh* Mesh);

    /**
     * Import morph data from a JSON file exported by the blender_export_morph.py script.
     * Each Blender vertex position is matched against ALL render-buffer vertices on TargetMesh
     * at that position (within epsilon), so UV-seam split vertices all receive the same delta.
     * Works without a source mesh reference — the Blender file provides positions directly.
     * @param TargetMesh        The custom skeletal mesh to write morph data onto.
     * @param BlenderJsonPath   Path to the .json produced by blender_export_morph.py.
     * @param OutMessage        Human-readable result or error description.
     * @return true on success.
     */
    UFUNCTION(BlueprintCallable, CallInEditor, Category = "inZOI|Morph")
    static bool ImportMorphFromBlenderJson(USkeletalMesh* TargetMesh, const FString& BlenderJsonPath, FString& OutMessage);
};
