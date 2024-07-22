using System;
using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

namespace ActiveRagdoll
{
    public class ActiveRagdoll : MonoBehaviour 
    {
        [Header("--- Body ---")]
        [SerializeField] private Transform _animatedTorso;
        [SerializeField] private Rigidbody _physicalTorso;
        public Transform AnimatedTorso { get { return _animatedTorso; } }
        public Rigidbody PhysicalTorso { get { return _physicalTorso; } }


        public Transform[] AnimatedBones { get; private set; }
        public ConfigurableJoint[] Joints { get; private set; }
        public Rigidbody[] Rigidbodies { get; private set; }

        [Header("--- Animators ---")]
        [SerializeField] private Animator _animatedAnimator;
        [SerializeField] private Animator _physicalAnimator;
        public Animator AnimatedAnimator 
        {
            get { return _animatedAnimator; }
            private set { _animatedAnimator = value; }
        }

        public bool SyncTorsoPositions { get; set; } = true;
        public bool SyncTorsoRotations { get; set; } = true;

        private void OnValidate() 
        {
            var animators = GetComponentsInChildren<Animator>();
            if (animators.Length >= 2)
            {
                if (_animatedAnimator == null) _animatedAnimator = animators[0];
                if (_physicalAnimator == null) _physicalAnimator = animators[1];
            
                if (_animatedTorso == null)
                    _animatedTorso = _animatedAnimator.GetBoneTransform(HumanBodyBones.Hips);
                if (_physicalTorso == null)
                    _physicalTorso = _physicalAnimator.GetBoneTransform(HumanBodyBones.Hips).GetComponent<Rigidbody>();
            }
        }

        private void Awake() 
        {
            if (AnimatedBones == null) AnimatedBones = _animatedTorso?.GetComponentsInChildren<Transform>();
            if (Joints == null) Joints = _physicalTorso?.GetComponentsInChildren<ConfigurableJoint>();
            if (Rigidbodies == null) Rigidbodies = _physicalTorso?.GetComponentsInChildren<Rigidbody>();
        }

        private List<ConfigurableJoint> TryGetJoints(params HumanBodyBones[] bones) 
        {
            List<ConfigurableJoint> jointList = new List<ConfigurableJoint>();
            foreach (HumanBodyBones bone in bones)
            {
                Transform boneTransform = _physicalAnimator.GetBoneTransform(bone);
                if (boneTransform != null && (boneTransform.TryGetComponent(out ConfigurableJoint joint)))
                {
                    jointList.Add(joint);
                }
            }
            return jointList;
        }

        private void FixedUpdate()
        {
            SyncAnimatedBody();
        }

        private void SyncAnimatedBody() 
        {
            if (SyncTorsoPositions)
                _animatedAnimator.transform.position = _physicalTorso.position
                                + (_animatedAnimator.transform.position - _animatedTorso.position);
            if (SyncTorsoRotations)
                _animatedAnimator.transform.rotation = _physicalTorso.rotation;
        }


        // ------------------- GETTERS & SETTERS -------------------
        public Transform GetAnimatedBone(HumanBodyBones bone)
        {
            return _animatedAnimator.GetBoneTransform(bone);
        }

        public Transform GetPhysicalBone(HumanBodyBones bone)
        {
            return _physicalAnimator.GetBoneTransform(bone);
        }
    }
}