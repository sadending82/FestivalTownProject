using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_GachaponModel : UI_Base
{
    enum GameObjects
    {
        Gachapon
    }

    bool isInitialized = false;

    private Animator animator;
    private const int MAX_GACHAPON_NUM = 5;

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

        transform.LookAt(Camera.main.transform.GetChild(0).position);
    }
    public void GachaponCustomizing(int value)
    {
        Material[] gachaponMaterials = Get<GameObject>((int)GameObjects.Gachapon).transform.Find("GachaponA").GetComponent<SkinnedMeshRenderer>().materials;
        switch (value)
        {
            //냥냥 가챠
            case 100011:
                {
                    gachaponMaterials[2] = Resources.Load<Material>($"Materials/Gachapon/banner1");
                }
                break;
            //울트라 냥냥가챠
            case 100012:
                {
                    gachaponMaterials[2] = Resources.Load<Material>($"Materials/Gachapon/banner2");
                }
                break;
            //울트라충무공레전더리 냥냥가챠
            case 100013:
                {
                    gachaponMaterials[2] = Resources.Load<Material>($"Materials/Gachapon/banner3");
                }
                break;
            default:
                {
                    Debug.Log("ERROR!!! GachaponCustomizing(): Wrong GhachaType !!!");
                }
                break;
        }
        string gachaponColor = "Gachapon" + Random.Range((int)1, (int)5);
        gachaponMaterials[0] = Resources.Load<Material>($"Materials/Gachapon/{gachaponColor}");
        Get<GameObject>((int)GameObjects.Gachapon).transform.Find("GachaponA").GetComponent<SkinnedMeshRenderer>().materials = gachaponMaterials;
    }
}
