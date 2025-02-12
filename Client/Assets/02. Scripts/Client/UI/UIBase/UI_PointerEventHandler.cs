using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class UI_PointerEventHandler : MonoBehaviour, IPointerClickHandler, IPointerDownHandler, IPointerUpHandler, IPointerEnterHandler, IPointerExitHandler
{
    public Action<PointerEventData> OnClickHandler = null;
    public Action<PointerEventData> OnPointerDownHandler = null;
    public Action<PointerEventData> OnPointerUpHandler = null;
    public Action<PointerEventData> OnPointerEnterHandler = null;
    public Action<PointerEventData> OnPointerExitHandler = null;

    bool _isButton = false;
    bool _isCancle = false;

    public void OnPointerClick(PointerEventData eventData)
    {
        if (OnClickHandler != null)
        {
            if (_isButton)
            {
                if (_isCancle)
                {
                    Managers.Sound.Play("Sfx_Button_Cancel");
                }
                else
                {
                    Managers.Sound.Play("Sfx_Button_Click");
                }
            }

            OnClickHandler.Invoke(eventData);
        }
    }

    public void OnPointerDown(PointerEventData eventData)
    {
        if (OnPointerDownHandler != null)
            OnPointerDownHandler.Invoke(eventData);
    }

    public void OnPointerUp(PointerEventData eventData)
    {
        if (OnPointerUpHandler != null)
            OnPointerUpHandler.Invoke(eventData);

    }

    public void OnPointerEnter(PointerEventData eventData)
    {
        if (_isButton)
        {
            Managers.Sound.Play("Sfx_Button_Hover");
        }

        if (OnPointerEnterHandler != null)
        {
            OnPointerEnterHandler.Invoke(eventData);
        }
    }

    public void OnPointerExit(PointerEventData eventData)
    {
        if (OnPointerExitHandler != null)
            OnPointerExitHandler.Invoke(eventData);
    }

    public void SetIsButton(bool isButton)
    {
        _isButton = isButton;
    }

    public void SetIsCancle(bool isCancle)
    {
        _isCancle = isCancle;
    }

}
