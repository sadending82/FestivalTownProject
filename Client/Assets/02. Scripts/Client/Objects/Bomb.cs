using ExcelDataStructure;
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

    private float bombReadyCount1;
    private int bombReadyTime1;
    private float bombReadyCount2;
    private float elapsedTime;

    public Material bombMaterial1;
    public Material bombMaterial2;
    private Material[] bombMaterials;

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

        bombMaterials = GetComponent<MeshRenderer>().materials;
        bombMaterials[0] = bombMaterial1;
        GetComponent<MeshRenderer>().materials = bombMaterials;

        StartCoroutine("Blink");
    }
    private void Start()
    {
        packetManager = Managers.Network.GetPacketManager();
    }
    private void OnEnable()
    {
        bombMaterials = GetComponent<MeshRenderer>().materials;
        bombMaterials[0] = bombMaterial1;
        GetComponent<MeshRenderer>().materials = bombMaterials;

        isPickUp = false;
        pickUpPlayerId = -1;
        lastPickUpPlayerId = -1;
        elapsedTime = 0.0f;

        var data = Managers.Data.GetModeData(Managers.Game.mapIndex);
        FITHModeEntity fme = (FITHModeEntity)data;
        bombReadyCount1 = 1.0f / fme.Bomb_Ready_Count1;
        bombReadyTime1 = fme.Bomb_Ready_Time1;
        bombReadyCount2 = 1.0f / fme.Bomb_Ready_Count2;

        StartCoroutine("Blink");
    }
    private void OnDisable()
    {
        SetRigidBodyBasic();
        StopCoroutine("Blink");
    }
    private void FixedUpdate()
    {
        elapsedTime += Time.deltaTime;
        if (isPickUp == true)
        {
            transform.position = targetTransform.position;
        }
        else if (Managers.Player.GetIsHost() == true && this.transform.position.y <= -10f)
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
        Managers.Effect.PlayEffect("BombExplosion", this.transform.position);

        if(isPickUp == true)
        {
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
        GameObject tPlayer = Managers.Player.FindPlayerById(pickUpPlayerId);
        if(tPlayer == null)
        {
            Debug.Log("ERROR!! PickUpOff: Player is Null");
            return;
        }
        CharacterStatus pickUpPlayerStatus = tPlayer.GetComponent<CharacterStatus>();
        pickUpPlayerStatus.SetIsHaveBomb(false);

        isPickUp = false;
        this.pickUpPlayerId = -1;
        this.targetTransform = null;
    }
    public void Throw(Vector3 throwDirection, float throwPower)
    {
        this.throwDirection = throwDirection;
        SetRigidBodyBasic();
        rig.AddForce(throwDirection * throwPower, ForceMode.Impulse);
        PickUpOff();
    }
    public void Drop(Vector3 position)
    {
        SetRigidBodyBasic();
        this.transform.position = position;
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
    private IEnumerator Blink()
    {
        while (true)
        {
            if (elapsedTime < bombReadyTime1)
            {
                yield return new WaitForSeconds(bombReadyCount1);
            }
            else
            {
                yield return new WaitForSeconds(bombReadyCount2);
            }

            bombMaterials[0] = bombMaterial2;
            GetComponent<MeshRenderer>().materials = bombMaterials;
            yield return new WaitForSeconds(0.2f);

            bombMaterials[0] = bombMaterial1;
            GetComponent<MeshRenderer>().materials = bombMaterials;
        }
    }
}
