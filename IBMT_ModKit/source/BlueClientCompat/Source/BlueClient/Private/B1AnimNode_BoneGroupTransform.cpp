#include "B1AnimNode_BoneGroupTransform.h"

// Stub implementations - pass the pose through unchanged.
// At runtime inside the game binary the real FB1AnimNode_BoneGroupTransform
// reads data from the owning UB1BonePostProcessAnimInstance and applies
// per-bone-group transforms that drive body sliders/morphs.

void FB1AnimNode_BoneGroupTransform::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
    FAnimNode_Base::Initialize_AnyThread(Context);
    Source.Initialize(Context);
}

void FB1AnimNode_BoneGroupTransform::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
    FAnimNode_Base::CacheBones_AnyThread(Context);
    Source.CacheBones(Context);
}

void FB1AnimNode_BoneGroupTransform::Update_AnyThread(const FAnimationUpdateContext& Context)
{
    Source.Update(Context);
}

void FB1AnimNode_BoneGroupTransform::Evaluate_AnyThread(FPoseContext& Output)
{
    Source.Evaluate(Output);
}

void FB1AnimNode_BoneGroupTransform::GatherDebugData(FNodeDebugData& DebugData)
{
    Source.GatherDebugData(DebugData);
}
