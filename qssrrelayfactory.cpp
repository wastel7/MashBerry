#include "qssrrelayfactory.h"
#include "qssrrelaykernel.h"
#include "qssrrelaygpio.h"

QSSRrelayFactory::QSSRrelayFactory(QObject *parent) :
    QObject(parent)
{
}

QSSRrelay* QSSRrelayFactory::GetSSRrelay(int ssr)
{
    int gpio;
    QFile ssrKernel(QString("/sys/devices/platform/ssr_plug.0/ssr_%1").arg(ssr));

    if(ssrKernel.open(QIODevice::WriteOnly))
    {
        ssrKernel.close();
        return new QSSRrelayKernel(ssr, this);
    }
    else
    {
        if(ssr == 1)
            gpio = 23;
        else
            gpio = 24;

        return new QSSRrelayGPIO(gpio, this);
    }
}
