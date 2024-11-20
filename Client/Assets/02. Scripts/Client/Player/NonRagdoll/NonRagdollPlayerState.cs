using ExcelDataStructure;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class NonRagdollPlayerState : MonoBehaviour
{
    public GameObject playerMesh;
    public GameObject Head_Equipments;
    public GameObject Face_Equipments;
    public GameObject Back_Equipments;
    private Material skinMaterial;
    private Material faceMaterial;
    private GameObject headItem;
    private GameObject faceItem;
    private GameObject backItem;

    private void Awake()
    {
        if (skinMaterial == null || faceMaterial == null)
        {
            Material[] mat = playerMesh.GetComponent<SkinnedMeshRenderer>().materials;
            skinMaterial = mat[0];
            faceMaterial = mat[1];
        }
    }
    public void SetMaterial(Material skinMaterial, Material faceMaterial)
    {
        Material[] mat = new Material[] { skinMaterial, faceMaterial };
        playerMesh.GetComponent<SkinnedMeshRenderer>().materials = mat;
    }
    public void ChangeCustomizing(int itemCode)
    {
        ItemEntity tItem = Managers.Data.GetItemData(itemCode);
        switch (tItem.Item_Type)
        {
            //Skin
            case 10:
                {
                    skinMaterial = Resources.Load<Material>($"Materials/{tItem.File_Name}");
                    SetMaterial(skinMaterial, faceMaterial);
                }
                break;
            //Head Item
            case 21:
                {
                    if (headItem != null)
                    {
                        headItem.SetActive(false);
                    }
                    for (int i = 0; i < Head_Equipments.transform.childCount; ++i)
                    {
                        if (Head_Equipments.transform.GetChild(i).name == tItem.File_Name)
                        {
                            headItem = Head_Equipments.transform.GetChild(i).gameObject;
                            headItem.SetActive(true);
                            break;
                        }
                    }
                }
                break;
            //Face Item
            case 22:
                {
                    if (faceItem != null)
                    {
                        faceItem.SetActive(false);
                    }
                    for (int i = 0; i < Face_Equipments.transform.childCount; ++i)
                    {
                        if (Face_Equipments.transform.GetChild(i).name == tItem.File_Name)
                        {
                            faceItem = Face_Equipments.transform.GetChild(i).gameObject;
                            faceItem.SetActive(true);
                            break;
                        }
                    }
                }
                break;
            //Back Item
            case 23:
                {
                    if (backItem != null)
                    {
                        backItem.SetActive(false);
                    }
                    for (int i = 0; i < Back_Equipments.transform.childCount; ++i)
                    {
                        if (Back_Equipments.transform.GetChild(i).name == tItem.File_Name)
                        {
                            backItem = Back_Equipments.transform.GetChild(i).gameObject;
                            backItem.SetActive(true);
                            break;
                        }
                    }
                }
                break;
            //Face
            case 24:
                {
                    faceMaterial = Resources.Load<Material>($"Materials/{tItem.File_Name}");
                    SetMaterial(skinMaterial, faceMaterial);
                }
                break;
            default:
                {
                    Debug.Log("ERROR!!! ChangeCustomizing(): Wrong Item Type!!!");
                }
                break;
        }
    }
}
