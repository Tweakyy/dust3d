#include "animationclipplayer.h"

AnimationClipPlayer::~AnimationClipPlayer()
{
    clear();
}

void AnimationClipPlayer::updateFrameMeshes(std::vector<std::pair<float, MeshLoader *>> &frameMeshes)
{
    clear();
    
    m_frameMeshes = frameMeshes;
    frameMeshes.clear();
    
    m_currentPlayIndex = 0;
    m_countForFrame.restart();
    
    if (!m_frameMeshes.empty())
        m_timerForFrame.singleShot(0, this, &AnimationClipPlayer::frameReadyToShow);
}

void AnimationClipPlayer::clear()
{
    freeFrames();
    delete m_lastFrameMesh;
    m_lastFrameMesh = nullptr;
}

void AnimationClipPlayer::freeFrames()
{
    for (auto &it: m_frameMeshes) {
        delete it.second;
    }
    m_frameMeshes.clear();
}

MeshLoader *AnimationClipPlayer::takeFrameMesh()
{
    if (m_currentPlayIndex >= (int)m_frameMeshes.size()) {
        if (nullptr != m_lastFrameMesh)
            return new MeshLoader(*m_lastFrameMesh);
        return nullptr;
    }
    int millis = m_frameMeshes[m_currentPlayIndex].first * 1000 - m_countForFrame.elapsed();
    if (millis > 0) {
        m_timerForFrame.singleShot(millis, this, &AnimationClipPlayer::frameReadyToShow);
        if (nullptr != m_lastFrameMesh)
            return new MeshLoader(*m_lastFrameMesh);
        return nullptr;
    }
    m_currentPlayIndex = (m_currentPlayIndex + 1) % m_frameMeshes.size();
    m_countForFrame.restart();

    MeshLoader *mesh = new MeshLoader(*m_frameMeshes[m_currentPlayIndex].second);
    m_timerForFrame.singleShot(m_frameMeshes[m_currentPlayIndex].first * 1000, this, &AnimationClipPlayer::frameReadyToShow);
    delete m_lastFrameMesh;
    m_lastFrameMesh = new MeshLoader(*mesh);
    return mesh;
}
