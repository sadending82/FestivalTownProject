using System.Collections;
using UnityEngine;

public class Statue : MonoBehaviour
{
    public int Team = 0;

    public Define.StatueState _state = Define.StatueState.Fine;

    [SerializeField]
    GameObject[] AttackedOneTimeMesh;

    [SerializeField]
    GameObject[] AttackedTwoTimeMesh;

    [SerializeField]
    GameObject[] DestroyedMesh;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    public void SetTeam(int team)
    {
        Team = team;
    }

    // Update is called once per frame
    void Update()
    {
        if(Managers.Game.isInGame)
        {
            if (Managers.Game.TeamLife.TryGetValue(Team, out int life)) // 값이 없으면 null이라 문제가 되니까 묶지 말고 따로 해야함
            {
                if (life != (int)_state && life >= 0)
                {
                    StateChange((Define.StatueState)life);
                }
            }
        }
    }

    void StateChange(Define.StatueState state)
    {
        _state = state;

        AllObjectOff();
        Vector3 tPos = transform.position;

        switch(state)
        {
            case Define.StatueState.Fine:
                transform.GetChild(0).gameObject.SetActive(true);             
                break;
            case Define.StatueState.AttackedOneTime:
                transform.GetChild(1).gameObject.SetActive(true);
                StartCoroutine(AfterDestroy(0));
                Managers.Sound.Play3D("Sfx_Statue_Explosion", gameObject);
                tPos.y += 6.5f;
                Managers.Effect.PlayEffect("StatueExplosion", tPos);
                break;
            case Define.StatueState.AttackedTwoTime:
                transform.GetChild(2).gameObject.SetActive(true);
                StartCoroutine(AfterDestroy(1));
                Managers.Sound.Play3D("Sfx_Statue_Explosion", gameObject);
                tPos.y += 4.0f;
                Managers.Effect.PlayEffect("StatueExplosion", tPos);
                break;
            case Define.StatueState.Destroyed:
                transform.GetChild(3).gameObject.SetActive(true);
                StartCoroutine(AfterDestroy(2));
                Managers.Sound.Play3D("Sfx_Statue_Explosion", gameObject);
                tPos.y += 1.5f;
                Managers.Effect.PlayEffect("StatueExplosion", tPos);
                break;
            default:
                break;
        }

    }
      

    public IEnumerator AfterDestroy(int childIndex)
    {
        yield return new WaitForSeconds(3);

        switch(childIndex)
        {
            case 0:
                foreach(GameObject go in AttackedOneTimeMesh)
                {
                    go.SetActive(false);
                }
                break;
            case 1:
                foreach (GameObject go in AttackedTwoTimeMesh)
                {
                    go.SetActive(false);
                }
                break;
            case 2:
                foreach (GameObject go in DestroyedMesh)
                {
                    go.SetActive(false);
                }
                break;
        }
    }

    void AllObjectOff()
    {
        transform.GetChild(0).gameObject.SetActive(false);
        transform.GetChild(1).gameObject.SetActive(false);
        transform.GetChild(2).gameObject.SetActive(false);
        transform.GetChild(3).gameObject.SetActive(false);
    }
}
