#include "WSStringProc.h"
#include "json.hpp"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

using json = nlohmann::json;

WSStringProc::WSStringProc(websocketpp::connection_hdl hdl)
    : rtspsource(NULL)
    , Con_Hdl(hdl)
{
    //ctor
}

WSStringProc::~WSStringProc()
{
    if (rtspsource)
    {
        RtspSource* delsource = rtspsource;
        rtspsource = NULL;
        delete delsource;
    }
}

void WSStringProc::ProcWSData(std::string data)
{
    WSDataBuf += data;
    while(true)
    {
        std::string::size_type idx;
        idx = WSDataBuf.find("{");
        if (idx != std::string::npos)
        {
            if(idx > 0)
                WSDataBuf.erase(WSDataBuf.begin(),WSDataBuf.begin() + idx);
        }
        else
        {
            return;
        }

        try {
            json root = json::parse(WSDataBuf);

            boost::asio::io_service io;
            io.dispatch(boost::bind(&WSStringProc::OnWSData, this, root));
            io.run();
        } catch (json::exception &e) {
            std::cout << e.what() << std::endl;
        }

        WSDataBuf.erase(WSDataBuf.begin(), WSDataBuf.begin() + data.length());
    }
}

void WSStringProc::OnWSData(json &root)
{
    if (!root.contains("url")) {
        return;
    }
    if (rtspsource) {
        return;
    }

    std::string url = root["url"];
    rtspsource = new RtspSource(url, Con_Hdl);
    boost::thread opentread(&WSStringProc::OpenSource, this);
}

void WSStringProc::OpenSource()
{
    if (rtspsource->Open() == false)
    {
        RtspSource* delsource = rtspsource;
        rtspsource = NULL;
        delete delsource;
    }
}
