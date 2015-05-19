#ifndef ENGINEFILTERPANSINGLE_H
#define ENGINEFILTERPANSINGLE_H

#include <string.h>

#include "engine/engineobject.h"
#include "util/assert.h"

static const int numChannels = 2;

template<unsigned int SIZE>
class EngineFilterPanSingle {
  public:
    EngineFilterPanSingle()
            : m_delayFrame(0),
              m_doStart(false) {
        // Set the current buffers to 0
        memset(m_buf, 0, sizeof(m_buf));
    }

    virtual ~EngineFilterPanSingle() {};

    void pauseFilter() {
        // Set the current buffers to 0
        if (!m_doStart) {
            memset(m_buf, 0, sizeof(m_buf));
            m_doStart = true;
        }
    }

    virtual void process(const CSAMPLE* pIn, CSAMPLE* pOutput, double leftDelayFrames) {
        double delayLeftSourceFrame;
        double delayRightSourceFrame;
        if (leftDelayFrames > 0) {
            delayLeftSourceFrame = m_delayFrame + SIZE - leftDelayFrames;
            delayRightSourceFrame = m_delayFrame + SIZE;
        } else {
            delayLeftSourceFrame = m_delayFrame + SIZE;
            delayRightSourceFrame = m_delayFrame + SIZE + leftDelayFrames;
        }

        // put in samples into delay buffer:
        m_buf[m_delayFrame * 2] = pIn[0];
        m_buf[m_delayFrame * 2 + 1] = pIn[1];
        m_delayFrame = (m_delayFrame + 1) % SIZE;

        double modLeft = fmod(delayLeftSourceFrame, 1);
        double modRight = fmod(delayRightSourceFrame, 1);

        pOutput[0] = m_buf[(static_cast<int>(floor(delayLeftSourceFrame)) % SIZE) * 2] * (1 - modLeft);
        pOutput[1] = m_buf[(static_cast<int>(floor(delayRightSourceFrame)) % SIZE) * 2 + 1] * (1 - modRight);
        pOutput[0] += m_buf[(static_cast<int>(ceil(delayLeftSourceFrame)) % SIZE) * 2] * modLeft;
        pOutput[1] += m_buf[(static_cast<int>(ceil(delayRightSourceFrame)) % SIZE) * 2 + 1] * modRight;
        m_doStart = false;
    }

  protected:
    int m_delayFrame;
    CSAMPLE m_buf[SIZE * numChannels];
    bool m_doStart;
};

#endif // ENGINEFILTERPAN_H
