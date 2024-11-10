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
        End, // 마지막이 어딘지 확인하는 용으로 추가
    }

    bool isInitialized = false;

    /// <summary>
    /// 현재 몇번째 인덱스의 패턴이 가장 위에 있나요?
    /// </summary>
    int _currentTop = 0;

    float _startOpacity = 0.3f;
    float _currentOpacity = 0.3f;
    
    float _maxOpacity = 0.3f;
    float _minOpacity = 0.1f;

    bool _isOpacityIncreasing = false;
    float _opacityIncreasingSpeed = 0.05f;

    float _patternChangeY = 1799.0f;

    float _moveSpeed = 20f;

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
            go.transform.localPosition = _positionToMove;

            ColorToChange = Color.white;
            ColorToChange.a = _currentOpacity;

            go.GetComponent<Image>().color = ColorToChange;
        }

        GameObject curTopGo = Get<GameObject>(_currentTop);

        if (curTopGo.transform.localPosition.y > _patternChangeY)
        {
            _positionToMove = curTopGo.transform.localPosition;
            _positionToMove.y -= _patternChangeY * 2.0f;
            curTopGo.transform.localPosition = _positionToMove;

            _currentTop++;
            
            if(_currentTop > (int)GameObjects.End)
            {
                _currentTop = 0;
            }
        }
        
    }
}
