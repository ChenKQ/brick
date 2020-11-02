#include "media/playmanager.h"
#include "log/log.h"

namespace brick
{
namespace media
{

PlayManager::~PlayManager()
{
    LOGINFO << "deconstructor of playmanager...";
}

void PlayManager::play(FFMpegReader &reader, bool playnow)
{
    atom_pause = !playnow;
    if(atom_stop) // create a new thread
    {
        atom_stop = false;

        t_producer = std::thread([&reader, obj = this]()
        {
            obj->producer_thread(reader);
        });

        t_producer.detach();
    }
    else
    {
        if(!atom_pause) //carry on
        {
            cv_play.notify_one();
        }
    }
}

void PlayManager::stop(FFMpegReader& reader)
{
    atom_stop = true;
    cv_play.notify_all();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    mtx_play_sig.lock();
    mtx_play_sig.unlock();

//    mtx_play_sig.lock();
//    cv_play.notify_one();
//    reader.destroy();
//    mtx_play_sig.unlock();
}

void PlayManager::producer_thread(FFMpegReader &reader)
{
    int ret = 0;
    std::unique_lock<std::mutex> locker(mtx_play_sig);
    while(true)
    {
        if(atom_stop)
        {
            reader.reset();
            LOGINFO << "stop: producer thread exists.";
            return;
        }

        while(atom_pause)
        {
            if(atom_stop)
            {
                reader.reset();
                LOGINFO << "stop when pause: producer thread exists";
                return;
            }
            cv_play.wait(locker);
        }
        ret = reader.fetch();
        if(ret != ErrorCode::SUCCESS)
        {
            atom_stop = true;
            LOGWARN << "ERROR IN FETCH : " << ret << " and producer thread exists";
            reader.reset();
            return;
        }
    }
}


}   // namespace media
}   // namespace brick
