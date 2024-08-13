using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using eAnimationState;

public class CharacterStatus : MonoBehaviour
{
    [Header("--- Status ---")]
    [SerializeField]
    private int id;
    public int maxHp;
    public int hp;
    public float maxStamina;
    public float stamina;
    public float staminaRecoverySpeed;
    public int strength;
    public int speed;
    public float runSpeedRatio;
    private float rollSpeed;
    public bool isDie = false;
    public int groggyNum;
    public float groggyTime;
    public Camera myCamera;
    private bool amIPlayer;

    [Header("--- AnimationControll ---")]
    public bool isGroggy;
    public AnimationController animationController;
    private ActiveRagdoll.AnimationModule animationMoudule;
    private UpperBodyAnimationState upperBodyAnimationState;
    private LowerBodyAnimationState lowerBodyAnimationState;

    private void Awake()
    {
        amIPlayer = false;
        myCamera.enabled = false;
    }
    private void Start()
    {
    }
    private void Update()
    {
    }

    // ------------------- GETTERS & SETTERS -------------------
    public void SetAmIPlayer(bool amIPlayer)
    {
        this.amIPlayer = amIPlayer;
        this.GetComponent<PlayerController>().SetAmIPlayer(amIPlayer);
        this.GetComponent<PlayerCameraController>().SetAmIPlayer(amIPlayer);
        if(amIPlayer == true)
        {
            myCamera.enabled = true;
            Managers.UI.ShowSceneUI<UI_OneVersusOneSceneUI>();
        }
    }
    public void setUpperBodyAnimationState(UpperBodyAnimationState upperBodyAnimationState)
    {
        this.upperBodyAnimationState = upperBodyAnimationState;
        animationController.setUpperBodyAnimationState(upperBodyAnimationState);
        ///<summary>
        ///서버에 상태 전달하는 부분 여기에 추가
        ///</summary>
    }
    public void setLowerBodyAnimationState(LowerBodyAnimationState lowerBodyAnimationState)
    {
        if (lowerBodyAnimationState != LowerBodyAnimationState.ROLL &&
            lowerBodyAnimationState != LowerBodyAnimationState.FLYINGKICK)
        {
            this.lowerBodyAnimationState = lowerBodyAnimationState;
            animationController.setLowerBodyAnimationState(lowerBodyAnimationState);
            ///<summary>
            ///서버에 상태 전달하는 부분 여기에 추가
            ///</summary>
        }
    }
}