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
        [SerializeField]
        private bool updateUpperBodyAnimation;  // 상체가 아무런 동작을 하지않으면 끈다.
        [SerializeField]
        private bool useAnimationModule;        // 그로기 모드일때 꺼야한다.

        private void Awake()
        {
            updateUpperBodyAnimation = false;
            useAnimationModule = true;
        }
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
            if (useAnimationModule == true)
            {
                UpdateJointTargets();
            }
        }

        private void UpdateJointTargets()
        {
            // 전신에 애니메이션 적용
            if (updateUpperBodyAnimation == true)
            {
                for (int i = 0; i < _joints.Length; i++)
                {
                    ConfigurableJointExtensions.SetTargetRotationLocal(_joints[i], _animatedBones[i + 1].localRotation, _initialJointsRotation[i]);
                }
            }
            // 하체에만 애니메이션 적용
            else
            {
                for (int i = 20; i < _joints.Length; i++)
                {
                    ConfigurableJointExtensions.SetTargetRotationLocal(_joints[i], _animatedBones[i + 1].localRotation, _initialJointsRotation[i]);
                }
            }
        }

        // ------------------- GETTERS & SETTERS -------------------
        public void SetUpdateUpperBodyAnimation(bool updateUpperBodyAnimation)
        {
            this.updateUpperBodyAnimation = updateUpperBodyAnimation;
        }
        public void SetUseAnimationModule(bool useAnimationModule)
        {
            this.useAnimationModule = useAnimationModule;
        }
    }
}