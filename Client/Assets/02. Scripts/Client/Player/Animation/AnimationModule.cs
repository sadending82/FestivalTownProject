using System;
using UnityEditor;
using UnityEngine;

namespace ActiveRagdoll
{

    public class AnimationModule : Module
    {
        [Header("--- Body ---")]
        private Quaternion[] _initialJointsRotation;
        private ConfigurableJoint[] _joints;
        private Transform[] _animatedBones;
        public Animator Animator { get; private set; }

        [Header("--- AnimationControll ---")]
        private bool useIK;
        private bool useAnimationModule;

        private void Start()
        {
            _joints = _activeRagdoll.Joints;
            _animatedBones = _activeRagdoll.AnimatedBones;
            Animator = _activeRagdoll.AnimatedAnimator;

            _initialJointsRotation = new Quaternion[_joints.Length];
            for (int i = 0; i < _joints.Length; i++)
            {
                _initialJointsRotation[i] = _joints[i].transform.localRotation;
            }
        }

        void FixedUpdate()
        {
            UpdateJointTargets();
        }

        private void UpdateJointTargets()
        {
            for (int i = 0; i < _joints.Length; i++)
            {
                ConfigurableJointExtensions.SetTargetRotationLocal(_joints[i], _animatedBones[i + 1].localRotation, _initialJointsRotation[i]);
            }
        }

        public void PlayAnimation(string animation, float speed = 1)
        {
            Animator.Play(animation);
            Animator.SetFloat("speed", speed);
        }

        // ------------------- GETTERS & SETTERS -------------------
        public void SetUseIK(bool useIK)
        {
            this.useIK = useIK;
        }
        public void SetUseAnimationModule(bool useAnimationModule)
        {
            this.useAnimationModule = useAnimationModule;
        }
    }
}