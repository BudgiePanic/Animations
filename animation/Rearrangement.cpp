#include "Rearrangement.h"
#include <list>

namespace anim {

std::map<int, int> RearrangeArmature(Armature& armature) {
    Pose& restPose = armature.GetRestPose();
    Pose& bindPose = armature.GetBindPose();
    unsigned int numbBones = restPose.Size();
    if (numbBones == 0) { return std::map<int, int>(); } // nothing to rearrange
    /*
    bone 0 - child bones
    bone 1 - child bones
    bone 2 - child bones
    */
    std::vector<std::vector<int>> boneHierarchy(numbBones);
    std::list<int> idk; // not sure what this is for yet, some kind of work queue
    for (unsigned int i = 0; i < numbBones; i++) {
        int parentBoneIdx = restPose.ParentIndexOf(i);
        if (parentBoneIdx >= 0) {
            std::vector<int>& parentBonesChildren = boneHierarchy[parentBoneIdx];
            parentBonesChildren.push_back((int)i);
        } else {
            // root bone
            idk.push_back((int)i);
        }
    }
    std::map<int, int> toNewBoneOrder; // forward (original bone idx -> new bone idx)
    std::map<int, int> toOldBoneOrder; // back (new bone idx -> original bone idx)
    int idx = 0;
    while (idk.size() > 0) {
        int bone = *idk.begin();
        idk.pop_front();
        std::vector<int>& boneChildren = boneHierarchy[bone];
        unsigned int numbChildren = boneChildren.size();
        for (unsigned int i = 0; i < numbChildren; i++) {
            idk.push_back(boneChildren[i]);
        }
        toNewBoneOrder[idx] = bone;
        toOldBoneOrder[bone] = idx;
        idx += 1;
    }
    // mapping for invalids
    toNewBoneOrder[-1] = -1;
    toOldBoneOrder[-1] = -1;
    // its now time to perform the in-place modification of the armature object to put the bones in the correct order
    Pose newRestPose(numbBones);
    Pose newBindPose(numbBones);
    std::vector<std::string> newBoneNames(numbBones);
    // copy the data across
    for (unsigned int i = 0; i < numbBones; i++) {
        int bone = toNewBoneOrder[i];
        newRestPose.SetLocalTransform(i, restPose.GetLocalTransform(bone));
        newBindPose.SetLocalTransform(i, bindPose.GetLocalTransform(bone)); 
        newBoneNames[i] = armature.GetBoneName(bone);
        int boneParent = toOldBoneOrder[bindPose.ParentIndexOf(bone)];
        newBindPose.SetParentIndex(i, boneParent);
        newRestPose.SetParentIndex(i, boneParent);
    }
    // do the thing!
    armature.Set(newRestPose, newBindPose, newBoneNames);
    return toOldBoneOrder;
}

void RearrangeMesh(render::Mesh& mesh, std::map<int, int>& boneRemapping) {
    std::vector<i4>& boneIndices = mesh.GetBoneIndices(); // which bones affect each vertex
    unsigned int numbBoneInfluences = (unsigned int)boneIndices.size();
    for (unsigned int i = 0; i < numbBoneInfluences; i++) {
        boneIndices[i].x = boneRemapping[boneIndices[i].x];
        boneIndices[i].y = boneRemapping[boneIndices[i].y];
        boneIndices[i].z = boneRemapping[boneIndices[i].z];
        boneIndices[i].w = boneRemapping[boneIndices[i].w];
    }
    mesh.SyncOpenGL();
}

void RearrangeClip(Clip& clip, std::map<int, int>& boneRemapping) {
    unsigned int numbClips = clip.Size();
    for (unsigned int i = 0; i < numbClips; i++) {
        int bone = (int)clip.GetTrackBoneIDAtIndex(i);
        unsigned int mappedBone = (unsigned int)boneRemapping[bone];
        clip.SetTrackBoneID(i, mappedBone);
    }
}

void RearrangeQuickClip(QuickClip& clip, std::map<int, int>& boneRemapping) {
    // this should work
    unsigned int numbClips = clip.Size();
    for (unsigned int i = 0; i < numbClips; i++) {
        int bone = (int)clip.GetTrackBoneIDAtIndex(i);
        unsigned int mappedBone = (unsigned int)boneRemapping[bone];
        clip.SetTrackBoneID(i, mappedBone);
    }
}

}
