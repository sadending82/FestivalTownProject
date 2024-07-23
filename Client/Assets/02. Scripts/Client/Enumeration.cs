using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace eAnimationState
{
    public enum UpperBodyAnimationState
    {
        NONE,
        ATTACK, POWERATTACK, HOLD, THROW, HEADATTACK
    }

    public enum LowerBodyAnimationState
    {
        IDLE, WALK, RUN, JUMP, ROLL, FLYINGKICK
    }
}