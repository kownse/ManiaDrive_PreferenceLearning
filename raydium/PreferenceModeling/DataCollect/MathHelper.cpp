#include "PreferenceModeling/DataCollect/MathHelper.h"
#include <cmath>

///求向量内积
float inner_product_3f(float* vecA, float* vecB)
{
    float var = 0.0f;
    float lenA = 0.0f;
    float lenB = 0.0;

    for(int i=0; i<3; ++i)
    {
        var += vecA[i] * vecB[i];
        lenA += vecA[i] * vecA[i];
        lenB += vecB[i] * vecB[i];
    }

    lenA = sqrt(lenA);
    lenB = sqrt(lenB);

    return var / (lenA * lenB);
}
