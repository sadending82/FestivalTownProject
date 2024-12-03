using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

public class UI_CharacterModel : UI_Base
{

    enum GameObjects
    {
        NonRagdollPlayer,
        RaycastCatcher,
    }

    bool isInitialized = false;
    float _clickTime = 0.0f;
    float _shortClickLimit = 0.4f;
    bool _isClicked = false;
    bool _isShortClick = true;

    void Start()
    {
        if (!isInitialized) Init();
        // 커스터마이징 설정
        // transform도 만지기 때문에, Start에서 해주는 것이 맞는 것으로 보임. 
        SetCustomizing();

        if(Camera.main.transform.GetChild(0).gameObject.activeInHierarchy) // 로비 카메라
        {
            transform.LookAt(Camera.main.transform.GetChild(0));
        }
        else // 커스터마이징 카메라
        {
            transform.LookAt(Camera.main.transform.GetChild(1));
        }
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));


        isInitialized = true;
    }

    public void SetCustomizing()
    {
        var customizeState = Get<GameObject>((int)GameObjects.NonRagdollPlayer).GetComponent<NonRagdollPlayerState>();
        var animationState = Get<GameObject>((int)GameObjects.NonRagdollPlayer).GetComponent<NonRagdollPlayerAnimationController>();   

        foreach (var customizeData in Managers.Data.PlayerCustomizingData)
        {
            customizeState.ChangeCustomizing(customizeData.Value.ItemCode);
        }

        animationState.SetLobbyAnimation();

        Get<GameObject>((int)GameObjects.NonRagdollPlayer).transform.localRotation = Quaternion.identity;

        // 홈 화면 용 카메라
        transform.LookAt(Camera.main.transform.GetChild(0).position);
    }

    public void SetInventoryLocalCustomizing()
    {
        var customizeState = Get<GameObject>((int)GameObjects.NonRagdollPlayer).GetComponent<NonRagdollPlayerState>();
        var animationState = Get<GameObject>((int)GameObjects.NonRagdollPlayer).GetComponent<NonRagdollPlayerAnimationController>();

        customizeState.CustomizingInitialize();

        foreach (var customizeData in Managers.Data.ClientLocalCustomizingDataDict)
        {
            customizeState.ChangeCustomizing(customizeData.Value.ItemCode);
        }

        animationState.SetLobbyAnimation();

        Get<GameObject>((int)GameObjects.NonRagdollPlayer).transform.localRotation = Quaternion.identity;

        // 인벤토리 용 카메라
        transform.LookAt(Camera.main.transform.GetChild(1).position);
    }

    void Update()
    {
        
        if (Input.GetMouseButtonDown(0)) // 마우스 왼쪽 클릭
        {
            GameObject cam = Camera.main.transform.GetChild(0).gameObject.activeInHierarchy ?
                Camera.main.transform.GetChild(0).gameObject : cam = Camera.main.transform.GetChild(1).gameObject;

            Ray ray = cam.GetComponent<Camera>().ScreenPointToRay(Input.mousePosition);
            RaycastHit hit;
            if (Physics.Raycast(ray, out hit))
            {

                if (!_isClicked)
                {
                    _isClicked = true;
                    _isShortClick = true;
                    _clickTime = 0.0f;
                }
            }

            
        }

        if (Input.GetMouseButtonUp(0))
        {
            _isClicked = false;
        }

        

        if(_isClicked)
        {
            _clickTime += Time.deltaTime;
            if (_clickTime > _shortClickLimit)
            {
                _isShortClick = false;
            }

            Get<GameObject>((int)GameObjects.NonRagdollPlayer).transform.Rotate(Vector3.up, Input.GetAxis("Mouse X") * -5.0f);

        }


        
    }



}
