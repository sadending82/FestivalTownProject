using ClientProtocol;
using System;
using UnityEditor;
using UnityEngine;

namespace ActiveRagdoll
{
    public class AnimationModule : Module
    {
        [Header("--- Body ---")]
        private Quaternion[] initialJointsRotation;
        private ConfigurableJoint[] joints;
        public ConfigurableJoint stabillizer;
        private Transform[] animatedBones;
        private Rigidbody[] rigidbodys;
        public Animator Animator { get; private set; }

        [Header("--- AnimationControll ---")]
        [SerializeField]
        private bool updateUpperBodyAnimation;  // 상체가 아무런 동작을 하지않으면 끈다.
        [SerializeField]
        private bool isGroggy;                  // 그로기 모드일때 꺼야한다.

        private void Awake()
        {
            updateUpperBodyAnimation = false;
            isGroggy = false;
        }
        private void Start()
        {
            joints = _activeRagdoll.Joints;
            animatedBones = _activeRagdoll.AnimatedBones;
            Animator = _activeRagdoll.AnimatedAnimator;
            rigidbodys = _activeRagdoll.Rigidbodies;

            initialJointsRotation = new Quaternion[joints.Length];
            for (int i = 0; i < joints.Length; i++)
            {
                initialJointsRotation[i] = joints[i].transform.localRotation;
            }
        }

        void FixedUpdate()
        {
            if (isGroggy == false)
            {
                UpdateJointTargets();
            }
        }

        private void UpdateJointTargets()
        {
            // 전신에 애니메이션 적용
            //if (updateUpperBodyAnimation == true)
            {
                for (int i = 0; i < joints.Length; i++)
                {
                    ConfigurableJointExtensions.SetTargetRotationLocal(joints[i], animatedBones[i + 1].localRotation, initialJointsRotation[i]);
                }
            }
            //// 하체에만 애니메이션 적용
            //else
            //{
            //    for (int i = 20; i < _joints.Length; i++)
            //    {
            //        ConfigurableJointExtensions.SetTargetRotationLocal(_joints[i], _animatedBones[i + 1].localRotation, _initialJointsRotation[i]);
            //    }
            //}
        }

        // ------------------- GETTERS & SETTERS -------------------
        public void SetUpdateUpperBodyAnimation(bool updateUpperBodyAnimation)
        {
            this.updateUpperBodyAnimation = updateUpperBodyAnimation;
        }
        public void GroggyOn()
        {
            Debug.Log("Groggy On");
            isGroggy = true;

            JointDrive driveJoint = new JointDrive();

            driveJoint.positionSpring = 50;
            driveJoint.positionDamper = 0;
            driveJoint.maximumForce = 10000;
            driveJoint.useAcceleration = false;
            for (int i = 0; i < joints.Length; ++i)
            {
                joints[i].angularXDrive = joints[i].angularYZDrive = driveJoint;
            }

            driveJoint.positionSpring = 0;
            driveJoint.positionDamper = 0;
            driveJoint.maximumForce = 1000;
            stabillizer.angularXDrive = stabillizer.angularYZDrive = driveJoint;

            // 그로기 상태에서 잘 끌려다니도록 mass 가볍게 설정
            foreach (Rigidbody rb in rigidbodys)
            {
                rb.mass = 0.5f;
            }
        }
        public void GroggyOff()
        {
            Debug.Log("Groggy Off");
            isGroggy = false;

            JointDrive driveJoint = new JointDrive();

            driveJoint.positionSpring = 1000;
            driveJoint.positionDamper = 0;
            driveJoint.maximumForce = 10000;
            driveJoint.useAcceleration = false;
            for (int i = 0; i < joints.Length; ++i)
            {
                if (joints[i].name == "tail01" ||
                    joints[i].name == "tail02" ||
                    joints[i].name == "tail03" ||
                    joints[i].name == "tail04" ||
                    joints[i].name == "tail05")
                {
                    driveJoint.positionSpring = 100;
                }
                joints[i].angularXDrive = joints[i].angularYZDrive = driveJoint;
                driveJoint.positionSpring = 1000;
            }

            driveJoint.positionDamper = 50;
            driveJoint.maximumForce = 1000;
            stabillizer.angularXDrive = stabillizer.angularYZDrive = driveJoint;

            // 다시 원래 무게로 설정
            foreach (Rigidbody rb in rigidbodys)
            {
                rb.mass = 1f;
            }
        }
    }
}