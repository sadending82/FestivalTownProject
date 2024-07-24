using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TestReceive : MonoBehaviour
{
    public float positionXOffeset;
    public float positionZOffeset;
    public float moveSpeed;
    private Vector3 moveDirection;

    public GameObject pelvis;
    private Transform pelvisTransform;
    private bool isMove;

    private void Awake()
    {
        SetIsMove(false);
    }
    private void Start()
    {
        pelvisTransform = pelvis.GetComponent<Transform>();
    }
    private void FixedUpdate()
    {
        if(isMove == true)
        {
            GetComponent<Rigidbody>().velocity = moveDirection * moveSpeed;
        }
    }

    public void SetTransform(Vector3 tranform)
    {
        this.transform.position = new Vector3(tranform.x + positionXOffeset, this.transform.position.y, tranform.z + positionZOffeset);
    }
    public void SetDirection(Vector3 direction)
    {
        moveDirection = direction;
    }
    public void SetIsMove(bool isMove)
    {
        this.isMove = isMove;
    }
}
