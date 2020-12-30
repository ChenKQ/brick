#ifndef BRICK_PLAYMANAGER_H
#define BRICK_PLAYMANAGER_H

#include "ffmpegreader.h"

#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace brick
{
namespace media
{

/**
 * @brief The PlayManager class : it is used with FFMpegReader to manage
 * the video play process, including play and stop.
 * Make sure that the FFMpegReader is constructed befor this and
 * deconstructed after it is stopped and released
 */
class PlayManager final
{
public:
    PlayManager() = default;
    ~PlayManager();

    void play(FFMpegReader& reader, bool playnow = false);
    void stop(FFMpegReader& reader);

    inline bool stopped() { return atom_stop; }

private:
    std::thread t_producer;

    std::mutex mtx_play_sig;
    std::atomic<bool> atom_stop = {true};
    std::atomic<bool> atom_pause = {true};
    std::condition_variable cv_play;

    void producer_thread(FFMpegReader &reader);
};

}   //namespace media
}   //namespac brick

#endif // BRICK_PLAYMANAGER_H
