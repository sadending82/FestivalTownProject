using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Bomb : MonoBehaviour
{
    // test - 시간으로 인해 꺼지는거 꺼놓음
    public bool isTimeBomb = false;

    private float lifeTime;
    private float timer = 0;
    public GameObject bombPusher;

    private bool isPickUp = false;
    private int pickUpPlayerId;
    private Transform targetTransform;

    // ------ Server -------
    [SerializeField]
    private int id;

    private void OnEnable()
    {
        timer = 0;
        isPickUp = false;
        pickUpPlayerId = -1;
    }
    private void OnDisable()
    {
        SetRigidBodyBasic();
    }
    private void FixedUpdate()
    {
        if (isPickUp == true)
        {
            this.transform.position = targetTransform.position;
        }
    }
    private void Update()
    {
        if (isTimeBomb == true)
        {
            timer += Time.deltaTime;
            if (timer >= lifeTime)
            {
                Boom();
            }
        }
    }
    private void OnCollisionEnter(Collision collision)
    {
        // 바닥에 도달했을때 더이상 움직이지 않도록 고정
        if (collision.gameObject.tag == "Ground")
        {
            Rigidbody rigidbody = this.GetComponent<Rigidbody>();
            rigidbody.constraints = RigidbodyConstraints.FreezePosition |
                                    RigidbodyConstraints.FreezeRotation;
        }
    }
    public void Boom()
    {
        GameObject pusher = Instantiate(bombPusher) as GameObject;

        pusher.transform.position = new Vector3(this.transform.position.x, this.transform.position.y, this.transform.position.z);
        pusher.SetActive(true);

        if(isPickUp == true)
        {
            /// <summary>
            ///  게임 매니저 추가 되면 여기서 게임 매니저한테 죽은 플레이어 아이디 보내줌
            /// </summary>
            Debug.Log("Player " + pickUpPlayerId + " is Die");

            isPickUp = false;
            this.pickUpPlayerId = -1;
            this.targetTransform = null;
        }

        this.gameObject.SetActive(false);
    }

    public void SetLifeTime(float lifeTime)
    {
        this.lifeTime = lifeTime;
    }
    public bool GetIsPickUp()
    {
        return isPickUp;
    }

    public void PickUp(int pickUpPlayerId, Transform targetTransform)
    {
        isPickUp = true;
        this.pickUpPlayerId = pickUpPlayerId;
        this.targetTransform = targetTransform;
        SetRigidBodyPickUp();
        Debug.Log(this.name + " Pick Up to Player " + pickUpPlayerId);
    }
    private void SetRigidBodyPickUp()
    {
        Rigidbody rig = this.GetComponent<Rigidbody>();
        rig.constraints = RigidbodyConstraints.None;
        rig.useGravity = false;
        rig.mass = 0;
        rig.angularDrag = 0;
    }
    private void SetRigidBodyBasic()
    {
        Rigidbody rig = this.GetComponent<Rigidbody>();
        rig.constraints = RigidbodyConstraints.FreezeRotation;
        rig.useGravity = true;
        rig.mass = 1;
        rig.angularDrag = 0.05f;
    }
    
    public void SetId(int id)
    {
        this.id = id;
    }
    public int GetId()
    {
        return id;
    }
}
