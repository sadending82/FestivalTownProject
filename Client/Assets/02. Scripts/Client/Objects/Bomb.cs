using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Bomb : MonoBehaviour
{
    private bool isPickUp = false;
    private int pickUpPlayerId;
    private int lastPickUpPlayerId;
    private Transform targetTransform;

    private Vector3 throwDirection;
    private Rigidbody rig;

    // ------ Server -------
    [SerializeField]
    private int Id;
    private PacketManager packetManager;

    private void Awake()
    {
        isPickUp = false;
        pickUpPlayerId = -1;
        lastPickUpPlayerId = -1;
        rig = this.GetComponent<Rigidbody>();
    }
    private void Start()
    {
        packetManager = Managers.Network.GetPacketManager();
    }
    private void OnEnable()
    {
        isPickUp = false;
        pickUpPlayerId = -1;
        lastPickUpPlayerId = -1;
    }
    private void OnDisable()
    {
        SetRigidBodyBasic();
    }
    private void FixedUpdate()
    {
        if (isPickUp == true)
        {
            transform.position = targetTransform.position;
        }
        if (Managers.Player.GetIsHost() == true && this.transform.position.y <= -10f)
        {
            packetManager.SendBombExplosionPacket(transform.position, Id);
        }
    }
    private void OnCollisionEnter(Collision collision)
    {
        // 바닥에 도달했을때 더이상 움직이지 않도록 고정
        if (collision.gameObject.tag == "Ground" && isPickUp == false)
        {
            rig.constraints = RigidbodyConstraints.FreezePositionX |
                              RigidbodyConstraints.FreezePositionZ |
                              RigidbodyConstraints.FreezeRotation;
        }
    }
    public void Boom()
    {
        Managers.Sound.Stop3D(gameObject);

        GameObject pusher = Managers.Resource.Instantiate("BombPusher");

        pusher.transform.position = transform.position;
        pusher.SetActive(true);
        pusher.GetComponent<Pusher>().StartPush();

        GameObject sound = Managers.Resource.Instantiate("AudioSourceForDestroyingObj");

        Managers.Sound.Play3D("Sfx_Bomb_Explosion", sound);

        if(isPickUp == true)
        {
            /// <summary>
            ///  게임 매니저 추가 되면 여기서 게임 매니저한테 죽은 플레이어 아이디 보내줌
            /// </summary>
            Debug.Log("Player " + pickUpPlayerId + " is Die");

            PickUpOff();
        }
        rig.velocity = Vector3.zero;

        Managers.Resource.Destroy(this.gameObject);
    }
    public bool GetIsPickUp()
    {
        return isPickUp;
    }

    public void PickUp(int pickUpPlayerId, Transform targetTransform)
    {
        isPickUp = true;
        this.pickUpPlayerId = pickUpPlayerId;
        this.lastPickUpPlayerId = pickUpPlayerId;

        this.targetTransform = targetTransform;
        SetRigidBodyPickUp();
    }
    public void PickUpOff()
    {
        CharacterStatus pickUpPlayerStatus = Managers.Player.GetPlayers().transform.GetChild(pickUpPlayerId).GetComponent<CharacterStatus>();
        pickUpPlayerStatus.SetIsHaveBomb(false);

        isPickUp = false;
        this.pickUpPlayerId = -1;
        this.targetTransform = null;
    }

    public void Throw(Vector3 throwDirection, int throwStrength)
    {
        this.throwDirection = throwDirection;
        SetRigidBodyBasic();

        rig.AddForce(throwDirection * throwStrength, ForceMode.Impulse);

        PickUpOff();
    }

    private void SetRigidBodyPickUp()
    {
        rig.constraints = RigidbodyConstraints.FreezeRotation;
        rig.useGravity = false;
        rig.mass = 0;
        rig.angularDrag = 0;
    }
    private void SetRigidBodyBasic()
    {
        rig.constraints = RigidbodyConstraints.FreezeRotation;
        rig.useGravity = true;
        rig.mass = 1;
        rig.angularDrag = 0.05f;
    }
    public void SetPosition(Vector3 position)
    {
        transform.position = position;
    }
    public void SetId(int Id)
    {
        this.Id = Id;
    }
    public int GetId()
    {
        return Id;
    }
    public int GetLastPickUpPlayerId()
    {
        return lastPickUpPlayerId;
    }    
}
