#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNodeBase.h"
#include "B1AnimNode_BoneGroupTransform.generated.h"

/**
 * Runtime anim-graph node struct stub matching inZOI's FB1AnimNode_BoneGroupTransform
 * from /Script/BlueClient.
 *
 * Place this node in your AnimGraph between the local-space input pose and the
 * ConvertLocalToComponent (or AnimDynamics) nodes. It passes the pose through
 * unchanged at the plugin level; at runtime inside the game binary the real
 * implementation inspects the owning UB1BonePostProcessAnimInstance and applies
 * the per-bone group transforms that drive body sliders/morphs.
 *
 * Graph wiring for combined jiggle + morphs:
 *   [Linked Input Pose]
 *        |  (local space)
 *   [B1 Bone Group Transform]   <- this node
 *        |  (local space, morphs applied)
 *   [Convert Local to Component Space]
 *        |  (component space)
 *   [AnimDynamics x7]
 *        |  (component space)
 *   [Convert Component to Local Space]
 *        |
 *   [Output Pose]
 *
 * The editor wrapper for this node (UAnimGraphNode_B1BoneGroupTransform) is
 * located in the BlueClientEditor module.
 */
USTRUCT(BlueprintInternalUseOnly)
struct BLUECLIENT_API FB1AnimNode_BoneGroupTransform : public FAnimNode_Base
{
    GENERATED_BODY()

    /** Incoming local-space pose to be processed. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Links)
    FPoseLink Source;

    // FAnimNode_Base interface - stub implementations pass the pose through.
    virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
    virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override;
    virtual void Update_AnyThread(const FAnimationUpdateContext& Context) override;
    virtual void Evaluate_AnyThread(FPoseContext& Output) override;
    virtual void GatherDebugData(FNodeDebugData& DebugData) override;
};
