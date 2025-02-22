using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.PlayerLoop;

public class UI_PopUp : UI_Base
{
    public override void Init()
    {
        Managers.UI.SetCanvas(gameObject);
    }

    public virtual void ClosePopUpUI()
    {
        Managers.UI.ClosePopUpUI(this);
    }

    public virtual void Update()
    {
        if(Input.GetKeyDown(KeyCode.Escape))
        {
            if(Managers.UI.GetTopOfPopUPUI() == this)
            {
                Managers.UI.ClosePopUpUI(this);
            }
        }
    }
}
