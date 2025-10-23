#include "Com_Filter.h"

#define ALPHA 0.8
/* һ�׵�ͨ�ʲ� */
int16_t Com_Filter_LowPass(int16_t newData, int16_t lastData)
{
    return ALPHA * lastData + (1 - ALPHA) * newData;
}


/* �������˲����� */
KalmanFilter_Struct kfs[3] = {
    {0.02, 0, 0, 0, 0.001, 0.543},
    {0.02, 0, 0, 0, 0.001, 0.543},
    {0.02, 0, 0, 0, 0.001, 0.543}};
double Common_Filter_KalmanFilter(KalmanFilter_Struct *kf, double input)
{
    kf->Now_P = kf->LastP + kf->Q;
    kf->Kg    = kf->Now_P / (kf->Now_P + kf->R);
    kf->out   = kf->out + kf->Kg * (input - kf->out);
    kf->LastP = (1 - kf->Kg) * kf->Now_P;
    return kf->out;
}
