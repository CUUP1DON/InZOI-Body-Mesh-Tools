#include "BCCE_SkmMatUniformizer.h"

#include "Engine/SkinnedAssetCommon.h"
#include "Engine/SkeletalMesh.h"
#include "Materials/MaterialInterface.h"
#include "SkeletalMeshTypes.h"

bool BlueClientCompatSkmMaterialUniformizer::ApplyUniformMaterialsToAllSlots(
    USkeletalMesh* Mesh,
    UMaterialInterface* Material,
    const FString& SlotNameStr,
    FString& OutMessage)
{
    if (!Mesh)
    {
        OutMessage = TEXT("Skeletal mesh is null.");
        return false;
    }
    if (!Material)
    {
        OutMessage = TEXT("No material selected (Materials > Uniform Section Material).");
        return false;
    }
    if (SlotNameStr.IsEmpty())
    {
        OutMessage = TEXT("Material slot name is empty.");
        return false;
    }

    const FName SlotName(*SlotNameStr);

    FScopedSkeletalMeshPostEditChange PostEditScope(Mesh);
    TArray<FSkeletalMaterial> Mats = Mesh->GetMaterials();
    const int32 Num = Mats.Num();
    if (Num == 0)
    {
        OutMessage = TEXT("Skeletal mesh has no material slots.");
        return false;
    }

    for (FSkeletalMaterial& M : Mats)
    {
        M.MaterialInterface = Material;
        M.MaterialSlotName = SlotName;
#if WITH_EDITORONLY_DATA
        M.ImportedMaterialSlotName = SlotName;
#endif
    }

    Mesh->SetMaterials(Mats);
    Mesh->MarkPackageDirty();

    OutMessage = FString::Printf(TEXT("Updated %d material slot(s)."), Num);
    return true;
}
