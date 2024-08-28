using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace eAnimationState
{
    // 서버에 상태 전달
    public enum UpperBodyAnimationState
    {
        NONE,
        ATTACK, POWERATTACK, HOLD, THROW, HEADATTACK, ROLL, FLYINGKICK
    }

    // 서버에 전달 X 클라가 관리
    public enum LowerBodyAnimationState
    {
        IDLE, WALK, RUN
    }
}