using System.Collections;
using UnityEngine;

public class Statue : MonoBehaviour
{
    public int Team = 0;

    public Define.StatueState _state = Define.StatueState.Fine;

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

        switch(state)
        {
            case Define.StatueState.Fine:
                transform.GetChild(0).gameObject.SetActive(true);
                break;
            case Define.StatueState.AttackedOneTime:
                transform.GetChild(1).gameObject.SetActive(true);
                break;
            case Define.StatueState.AttackedTwoTime:
                transform.GetChild(2).gameObject.SetActive(true);
                break;
            case Define.StatueState.Destroyed:
                transform.GetChild(3).gameObject.SetActive(true);
                StartCoroutine(AfterDestroy(3));
                break;
            default:
                break;
        }

    }
      

    public IEnumerator AfterDestroy(int childIndex)
    {
        yield return new WaitForSeconds(3);

        transform.GetChild(childIndex).gameObject.SetActive(false);
    }

    void AllObjectOff()
    {
        transform.GetChild(0).gameObject.SetActive(false);
        transform.GetChild(1).gameObject.SetActive(false);
        transform.GetChild(2).gameObject.SetActive(false);
        transform.GetChild(3).gameObject.SetActive(false);
    }
}
