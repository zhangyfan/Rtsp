#include "logger.h"

int main()
{
    Common::InitLogger();

    LOG_INFO("System starting ......");
    LOG_INFO("System exiting ......");
}