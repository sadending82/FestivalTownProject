using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SpectatorCameraManager : MonoBehaviour
{
    public PlayerCameraController[] cameraControllers;
    public Camera[] cameras;
    private int cameraCount = 0;
    private int myCameraNumber = -1;
    private int nowCameraNumber = -1;

    public void AddCameras()
    {
        GameObject players = Managers.Player.GetPlayers();
        cameraControllers = players.GetComponentsInChildren<PlayerCameraController>();
        cameras = players.GetComponentsInChildren<Camera>();

        for (int i = 0; i < cameraControllers.Length; ++i)
        {
            if (cameraControllers[i].GetAmIPlayer() == true)
            {
                myCameraNumber = i;
                nowCameraNumber = i;
                break;
            }
        }
        cameraCount = cameras.Length;
    }
    public void SwitchMyCamera()
    {
        if(nowCameraNumber == myCameraNumber)
        {
            return;
        }

        cameras[nowCameraNumber].enabled = false;
        cameraControllers[nowCameraNumber].SpectatorModeOff();

        cameras[myCameraNumber].enabled = true;
        cameraControllers[myCameraNumber].SpectatorModeOn();

        nowCameraNumber = myCameraNumber;
    }
    public void SwitchNextCamera()
    {
        int i = nowCameraNumber;
        while (true)
        {
            i++;
            i %= cameraCount;
            if (cameraControllers[i].GetIsPlayerDie() == false)
            {
                cameras[nowCameraNumber].enabled = false;
                cameraControllers[nowCameraNumber].SpectatorModeOff();
                cameras[i].enabled = true;
                cameraControllers[i].SpectatorModeOn();
                nowCameraNumber = i;
                break;
            }
            // 한바퀴 돌 동안 못찾은거니까 그냥 리턴
            if(nowCameraNumber == i)
            {
                break;
            }
        }
    }
    public void CheckSpectator(int id)
    {
        if(nowCameraNumber == id)
        {
            SwitchNextCamera();
        }
    }

    public void Clear()
    {
        cameraControllers = null;
        cameras = null;
        cameraCount = 0;
        myCameraNumber = -1;
        nowCameraNumber = -1;
    }
}
