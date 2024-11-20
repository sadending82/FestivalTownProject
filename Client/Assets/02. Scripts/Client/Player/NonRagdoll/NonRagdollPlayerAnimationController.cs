using ClientProtocol;
using System.Collections;
using System.Collections.Generic;
using TMPro.Examples;
using UnityEngine;

public class NonRagdollPlayerAnimationController : MonoBehaviour
{
    public Animator animator;

    private void Awake()
    {
        if (animator == null)
        {
            animator = GetComponent<Animator>();
        }
    }
    public void SetWinAnimation()
    {
        int randomValue = (Random.Range(0, 3) + 1);
        ChangeAnimation("Win" + randomValue);
    }
    public void SetLoseAnimation()
    {
        int randomValue = (Random.Range(0, 3) + 1);
        ChangeAnimation("Lose" + randomValue);
    }
    private void ChangeAnimation(string state)
    {
        animator.SetTrigger(state);
    }
    public void ResetAnimation()
    {
        animator.SetTrigger("New State");
    }
}
