#ifndef WSSTRINGPROC_H
#define WSSTRINGPROC_H

#include <iostream>
#include "RtspSource.h"
#include "json.hpp"

class WSStringProc
{
    public:
        WSStringProc(websocketpp::connection_hdl hdl);
        virtual ~WSStringProc();
        void ProcWSData(std::string data);
    protected:
        std::string WSDataBuf;
        void OnWSData(nlohmann::json &root);

    private:
        RtspSource* rtspsource;
        websocketpp::connection_hdl Con_Hdl;
        void OpenSource();
};

#endif // WSSTRINGPROC_H
