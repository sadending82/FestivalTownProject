using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.UI;

public class UI_LobbyPatterns : UI_Base
{
    enum GameObjects
    {
        Pattern1,
        Pattern2,
        Pattern3,
        End, // �������� ����� Ȯ���ϴ� ������ �߰�
    }

    bool isInitialized = false;

    /// <summary>
    /// ���� ���° �ε����� ������ ���� ���� �ֳ���?
    /// </summary>
    int _currentTop = 0;

    const float _startOpacity = 0.3f;
    float _currentOpacity = 0.3f;
    
    const float _maxOpacity = 0.3f;
    const float _minOpacity = 0.1f;

    bool _isOpacityIncreasing = false;
    const float _opacityIncreasingSpeed = 0.05f;

    const float _patternChangeY = 1799.0f;

    const float _moveSpeed = 20f;

    Vector3 _positionToMove = Vector3.zero;
    Color ColorToChange = Color.white;

    void Start()
    {
        if (!isInitialized)
        {
            Init();
        }
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        _currentOpacity = _startOpacity;
        isInitialized = true;
    }

    private void Update()
    {
        if (_isOpacityIncreasing)
        {
            _currentOpacity += _opacityIncreasingSpeed * Time.deltaTime;
            if (_currentOpacity > _maxOpacity)
            {
                _currentOpacity = _maxOpacity;
                _isOpacityIncreasing = false;
            }
        }
        else
        {
            _currentOpacity -= _opacityIncreasingSpeed * Time.deltaTime;
            if (_currentOpacity < _minOpacity)
            {
                _currentOpacity = _minOpacity;
                _isOpacityIncreasing = true;
            }
        }

        for(int i = 0; i < (int)GameObjects.End; ++i)
        {
            GameObject go = Get<GameObject>(i);
            _positionToMove = go.transform.localPosition;
            _positionToMove.y += _moveSpeed * Time.deltaTime;
            if(_positionToMove.y > _patternChangeY)
            {
                _positionToMove.y -= _patternChangeY * 3;
            }
            go.transform.localPosition = _positionToMove;     

            ColorToChange = Color.white;
            ColorToChange.a = _currentOpacity;

            go.GetComponent<Image>().color = ColorToChange;    
        }
        
    }
}
