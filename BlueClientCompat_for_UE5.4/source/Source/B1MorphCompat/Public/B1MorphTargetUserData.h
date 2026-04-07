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
class B1MORPHCOMPAT_API UB1MorphTargetUserData : public UAssetUserData
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "inZOI")
    TArray<FB1MorphTargetData> MorphTargetDatas;
};

UCLASS()
class B1MORPHCOMPAT_API UBlueClientMorphTools : public UBlueprintFunctionLibrary
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
    static UB1MorphTargetUserData* GetB1MorphUserData(USkeletalMesh* Mesh);
};
