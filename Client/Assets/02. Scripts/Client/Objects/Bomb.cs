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

    private Vector3 throwDirection;
    private Rigidbody rigidbody;

    // ------ Server -------
    [SerializeField]
    private int id;

    private void Awake()
    {
        rigidbody = this.GetComponent<Rigidbody>();
    }
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
        if (collision.gameObject.tag == "Ground" && isPickUp == false)
        {
            rigidbody.constraints = RigidbodyConstraints.FreezePosition |
                                    RigidbodyConstraints.FreezeRotation;
        }
    }
    public void Boom()
    {
        GameObject pusher = Managers.Resource.Instantiate("BombPusher");

        pusher.transform.position = new Vector3(this.transform.position.x, this.transform.position.y, this.transform.position.z);
        pusher.SetActive(true);

        if(isPickUp == true)
        {
            /// <summary>
            ///  게임 매니저 추가 되면 여기서 게임 매니저한테 죽은 플레이어 아이디 보내줌
            /// </summary>
            Debug.Log("Player " + pickUpPlayerId + " is Die");

            PickUpOff();
        }

        Managers.Resource.Destroy(this.gameObject);
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
        Debug.Log("Bomb " + this.id + " Pick Up to Player " + pickUpPlayerId);
    }
    public void PickUpOff()
    {
        CharacterStatus pickUpPlayerStatus = Managers.Player.transform.GetChild(pickUpPlayerId).GetComponent<CharacterStatus>();
        pickUpPlayerStatus.SetIsHaveItem(false);

        isPickUp = false;
        this.pickUpPlayerId = -1;
        this.targetTransform = null;
    }

    public void Throw(Vector3 throwDirection, int throwStrength)
    {
        this.throwDirection = throwDirection;
        SetRigidBodyBasic();

        rigidbody.AddForce(throwDirection * throwStrength, ForceMode.Impulse);

        PickUpOff();
    }

    private void SetRigidBodyPickUp()
    {
        rigidbody.constraints = RigidbodyConstraints.None;
        rigidbody.useGravity = false;
        rigidbody.mass = 0;
        rigidbody.angularDrag = 0;
    }
    private void SetRigidBodyBasic()
    {
        rigidbody.constraints = RigidbodyConstraints.FreezeRotation;
        rigidbody.useGravity = true;
        rigidbody.mass = 1;
        rigidbody.angularDrag = 0.05f;
    }
    public void SetPosition(Vector3 position)
    {
        transform.position = position;
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
