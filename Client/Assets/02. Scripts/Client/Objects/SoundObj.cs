using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SoundObj : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        StartCoroutine(DestroySelf());
    }

    IEnumerator DestroySelf()
    {
        yield return new WaitForSeconds(3.0f);

        Managers.Resource.Destroy(gameObject);
    }
}
