using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UI_WhiteBoard : UI_Base
{
    enum GameObjects
    {
        QButton,
        EButton,
        ExplainImages,
    }

    bool isInitialized = false;
    int currentImage = 0;
    int imageCount = 4;

    void Start()
    {
        if (!isInitialized) Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.QButton).BindEvent((PointerEventData) =>
        {
            MoveToPrevImage();
        });

        Get<GameObject>((int)GameObjects.EButton).BindEvent((PointerEventData) =>
        {
            MoveToNextImage();
        });

        isInitialized = true;
    }

    public void MoveToNextImage()
    {
        currentImage++;
        if(currentImage >= imageCount)
        {
            currentImage = 0;
        }

        SetCurrentImageOn();
        
    }

    public void MoveToPrevImage()
    {
        currentImage--;
        if (currentImage < 0)
        {
            currentImage = imageCount - 1;
        }

        SetCurrentImageOn();
    }

    public void SetCurrentImageOn()
    {
        AllOffImages();
        Get<GameObject>((int)GameObjects.ExplainImages).transform.GetChild(currentImage).gameObject.SetActive(true);
    }

    public void AllOffImages()
    {
        foreach (Transform tf in Get<GameObject>((int)GameObjects.ExplainImages).transform)
        {
            if (tf == Get<GameObject>((int)GameObjects.ExplainImages).transform) continue;
            tf.gameObject.SetActive(false);
        }
    }

    private void Update()
    {
        if(Input.GetKeyDown(KeyCode.Q))
        {
            MoveToPrevImage();
            Managers.Sound.Play("Sfx_Button_Click");
        }

        if(Input.GetKeyDown(KeyCode.E))
        {
            MoveToNextImage();
            Managers.Sound.Play("Sfx_Button_Click");
        }
    }
}
