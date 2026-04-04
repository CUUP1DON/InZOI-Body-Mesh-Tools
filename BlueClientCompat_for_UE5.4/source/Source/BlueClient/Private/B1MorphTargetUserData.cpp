#include "B1MorphTargetUserData.h"

#include "Engine/SkeletalMesh.h"

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
        return false;
    }

    UB1MorphTargetUserData* SourceData = Cast<UB1MorphTargetUserData>(
        SourceMesh->GetAssetUserDataOfClass(UB1MorphTargetUserData::StaticClass()));

    if (!SourceData)
    {
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
    return true;
}
