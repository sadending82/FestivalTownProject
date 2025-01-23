using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Cube : MonoBehaviour
{
    private float targetHeight;

    private Checker checker;

    private bool isFixed = false;

    private void Start()
    {
        if(checker == null)
        {
            checker = this.transform.GetChild(0).GetComponent<Checker>();
        }
    }
    private void FixedUpdate()
    {
        if (this.transform.position.y <= targetHeight && isFixed == false)
        {
            // 바닥에 도달했을때 더이상 움직이지 않도록 고정
            checker.SetGroundChecker(true);

            this.transform.position = new Vector3(this.transform.position.x, targetHeight, this.transform.position.z);
            Rigidbody rigidbody = this.GetComponent<Rigidbody>();
            rigidbody.constraints = RigidbodyConstraints.FreezePositionX |
                                    RigidbodyConstraints.FreezePositionY |
                                    RigidbodyConstraints.FreezePositionZ |
                                    RigidbodyConstraints.FreezeRotationX |
                                    RigidbodyConstraints.FreezeRotationY |
                                    RigidbodyConstraints.FreezeRotationZ;

            int soundNum = Random.Range(1, 4);
            Managers.Sound.Play3D($"Sfx_Block_Impact{soundNum}", gameObject);
            Vector3 tPosition = new Vector3(this.transform.position.x, this.transform.position.y - this.transform.localScale.y, this.transform.position.z);
            Managers.Effect.PlayEffect("CubeFalldown", tPosition);
            Managers.Map.UpdateMapColliders((int)this.transform.position.x, (int)this.transform.position.z);

            isFixed = true;
        }
    }
    public void SetTargetHeight(float targetHeight)
    {
        this.targetHeight = targetHeight;
    }
}