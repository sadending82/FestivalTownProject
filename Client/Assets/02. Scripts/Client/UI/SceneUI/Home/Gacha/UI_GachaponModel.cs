using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_GachaponModel : UI_Base
{
    enum GameObjects
    {
        Gachapon,
        GachaBall,
        GachaEffects,
        Blinder
    }

    bool isInitialized = false;

    private Animator animator;
    private const int MAX_GACHAPON_NUM = 5;

    private string nowAnimationState;

    private bool leverOn = false;
    public GameObject lever;
    private bool isGachaDone = false;
    void Start()
    {
        if (!isInitialized) Init();

        animator = Get<GameObject>((int)GameObjects.Gachapon).GetComponent<Animator>();

        SetGachapon();

        if (Camera.main.transform.GetChild(0).gameObject.activeInHierarchy)
        {
            transform.LookAt(Camera.main.transform.GetChild(0));
        }
    }
    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        isInitialized = true;
    }
    public void SetGachapon()
    {
        Get<GameObject>((int)GameObjects.Gachapon).transform.localRotation = Quaternion.identity;
        animator.SetTrigger("State");
        nowAnimationState = "State";

        transform.LookAt(Camera.main.transform.GetChild(0).position);
    }
    public void GachaponCustomizing(int value)
    {
        string gachaponColor = "Gachapon";
        Material[] gachaponMaterials = Get<GameObject>((int)GameObjects.Gachapon).transform.Find("GachaponA").GetComponent<SkinnedMeshRenderer>().materials;
        switch (value)
        {
            //냥냥 가챠
            case 100011:
                {
                    gachaponMaterials[2] = Resources.Load<Material>($"Materials/Gachapon/banner1");
                    gachaponColor += "2";
                }
                break;
            //울트라 냥냥가챠
            case 100012:
                {
                    gachaponMaterials[2] = Resources.Load<Material>($"Materials/Gachapon/banner2");
                    gachaponColor += "1";
                }
                break;
            //울트라충무공레전더리 냥냥가챠
            case 100013:
                {
                    gachaponMaterials[2] = Resources.Load<Material>($"Materials/Gachapon/banner3");
                    gachaponColor += Random.Range((int)1, (int)5);
                }
                break;
            //이모트 가챠
            case 100014:
                {
                    gachaponMaterials[2] = Resources.Load<Material>($"Materials/Gachapon/banner3");
                    gachaponColor += "5";
                }
                break;
            default:
                {
                    Debug.Log("ERROR!!! GachaponCustomizing(): Wrong GhachaType !!!");
                }
                break;
        }
        gachaponMaterials[0] = Resources.Load<Material>($"Materials/Gachapon/{gachaponColor}");
        Get<GameObject>((int)GameObjects.Gachapon).transform.Find("GachaponA").GetComponent<SkinnedMeshRenderer>().materials = gachaponMaterials;
    }

    private void ChangeAnimation(string state)
    {
        nowAnimationState = state;
        animator.SetTrigger(nowAnimationState);
    }

    public void GachaStart()
    {
        leverOn = true;
        StartCoroutine("LeverOff");
    }
    private void FixedUpdate()
    {
        if(leverOn)
        {
            lever.transform.Rotate(new Vector3(0.0f, 0.0f, Time.deltaTime * 150.0f));
        }
    }
    private void Update()
    {
        if (Input.GetMouseButtonUp(0) && isGachaDone == true)
        {
            var gachaCutScene = Managers.UI.GetCurrentSceneUI();
            gachaCutScene.GetComponent<UI_GachaCutScene>().ShowGachaRequest();
        }
    }
    IEnumerator LeverOff()
    {
        yield return new WaitForSeconds(1.5f);
        leverOn = false;
        ChangeAnimation("Shake");
        yield return new WaitForSeconds(1.5f);
        Get<GameObject>((int)GameObjects.GachaBall).transform.GetChild(Random.Range((int)0, (int)4)).gameObject.SetActive(true);
        var gachaCutScene = Managers.UI.GetCurrentSceneUI();
        Get<GameObject>((int)GameObjects.GachaEffects).transform.GetChild(gachaCutScene.GetComponent<UI_GachaCutScene>().GetResultItemGrade() - 1).gameObject.SetActive(true);
        Get<GameObject>((int)GameObjects.Blinder).transform.GetChild(0).gameObject.SetActive(true);
        isGachaDone = true;
        yield return null;
    }
}
