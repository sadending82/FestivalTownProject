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
    /// �ּҰ��� �ִ�, �߾Ӱ��� �� ���� �ָ� �ش� ���� 0���� 1������ ������ ����ȭ ���ִ� �α� ���� ��ȯ��
    /// </summary>
    /// <param name="minValue">�ּڰ�</param>
    /// <param name="centerValue">�߾Ӱ�</param>
    /// <param name="maxValue">�ִ�</param>
    public LogarithmicRangeConverter(float minValue, float centerValue, float maxValue)
    {
        this.minValue = minValue;
        this.maxValue = maxValue;

        _aValue = (minValue * maxValue - (centerValue * centerValue)) / (minValue - 2 * centerValue + maxValue);
        _bValue = ((centerValue - minValue) * (centerValue - minValue)) / (minValue - 2 * centerValue + maxValue);
        _cValue = 2 * Mathf.Log((maxValue - centerValue) / (centerValue - minValue));
    }

    /// <summary>
    /// 0���� 1���� ���� �ּҰ��� �ִ� ������ ������ �������ִ� �༮
    /// </summary>
    public float ToRealRange(float value)
    {
        return _aValue + _bValue * Mathf.Exp(_cValue * value);
    }

    /// <summary>
    /// �ּڰ��� �ִ� ������ ���� 0���� 1�� ����ȭ�� ������ �������ִ� �༮
    /// </summary>
    public float ToNormaiized(float value)
    {
        return Mathf.Log((value - _aValue) / _bValue) / _cValue;
    }
        
}
