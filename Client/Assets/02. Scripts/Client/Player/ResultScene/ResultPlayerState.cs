using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ResultPlayerState : MonoBehaviour
{
    public GameObject playerMesh;
    public void SetStyle(Material skinMaterial, Material faceMaterial)
    {
        Material[] mat = new Material[] { skinMaterial, faceMaterial };
        playerMesh.GetComponent<SkinnedMeshRenderer>().materials = mat;
    }
}
