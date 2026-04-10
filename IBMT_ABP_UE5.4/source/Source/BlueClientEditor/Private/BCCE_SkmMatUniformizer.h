#pragma once

#include "CoreMinimal.h"

class USkeletalMesh;
class UMaterialInterface;

namespace BlueClientCompatSkmMaterialUniformizer
{
bool ApplyUniformMaterialsToAllSlots(USkeletalMesh* Mesh, UMaterialInterface* Material, const FString& SlotNameStr, FString& OutMessage);
}
