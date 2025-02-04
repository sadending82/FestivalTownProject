using System.Collections;
using System.Collections.Generic;
using System.Net.Http.Headers;
using UnityEngine;

public class LogarithmicRangeConverter
{
    public readonly float minValue;
    public readonly float maxValue;

    private readonly float _aValue;
    private readonly float _bValue;
    private readonly float _cValue;

    /// <summary>
    /// 최소값과 최댓값, 중앙값의 세 값을 주면 해당 값을 0에서 1사이의 값으로 정규화 해주는 로그 범위 변환기
    /// </summary>
    /// <param name="minValue">최솟값</param>
    /// <param name="centerValue">중앙값</param>
    /// <param name="maxValue">최댓값</param>
    public LogarithmicRangeConverter(float minValue, float centerValue, float maxValue)
    {
        this.minValue = minValue;
        this.maxValue = maxValue;

        _aValue = (minValue * maxValue - (centerValue * centerValue)) / (minValue - 2 * centerValue + maxValue);
        _bValue = ((centerValue - minValue) * (centerValue - minValue)) / (minValue - 2 * centerValue + maxValue);
        _cValue = 2 * Mathf.Log((maxValue - centerValue) / (centerValue - minValue));
    }

    /// <summary>
    /// 0에서 1사이 값을 최소값과 최댓값 사이의 값으로 변경해주는 녀석
    /// </summary>
    public float ToRealRange(float value)
    {
        return _aValue + _bValue * Mathf.Exp(_cValue * value);
    }

    /// <summary>
    /// 최솟값과 최댓값 사이의 값을 0에서 1의 정규화된 값으로 변경해주는 녀석
    /// </summary>
    public float ToNormaiized(float value)
    {
        return Mathf.Log((value - _aValue) / _bValue) / _cValue;
    }
        
}
