




#include "kasariaConfig.h"




KasariaConfig::KasariaConfig(OMConfig *omconfig) : SynthConfig()
{
    m_cfg = omconfig->getSynthJson(name());
    load();
}

void KasariaConfig::load()
{
    if(m_cfg.is_null())
        return;

    try
    {
        audioVol              = m_cfg.value("amplification", audioVol);
        antialiasedSamples    = m_cfg.value("antialiasingSamples", antialiasedSamples);
        audioLimiter          = m_cfg.value("audioLimiter", audioLimiter);
        controlRate           = m_cfg.value("controlRate", controlRate);
        disableLogging        = m_cfg.value("disableSynthLogs", disableLogging);
        EvBufSize             = m_cfg.value("evBufSize", EvBufSize);
        fastDecay             = m_cfg.value("fastDecay", fastDecay);
        audioFrameSize        = m_cfg.value("framePeriodSize", audioFrameSize);
        highVelRange          = m_cfg.value("highVelocityThreshold", highVelRange);
        lowVelRange           = m_cfg.value("lowVelocityThreshold", lowVelRange);
        preResample           = m_cfg.value("preResample", preResample);
        sampleRate            = m_cfg.value("sampleRate", sampleRate);
        noteVelSkipperEnabled = m_cfg.value("velocityNoteSkipping", noteVelSkipperEnabled);
        voiceLimit            = m_cfg.value("voiceLimit", voiceLimit);
    }
    catch(const std::exception &e)
    {
        std::string s = "Error loading FluidSynth settings:";
        throw std::runtime_error(s + e.what());
    }
}

void KasariaConfig::store()
{
    if(m_cfg.is_null())
        m_cfg = json::parse("{}");

    m_cfg["amplification"]         = audioVol;
    m_cfg["antialiasingSamples"]   = antialiasedSamples;
    m_cfg["audioLimiter"]          = audioLimiter;
    m_cfg["controlRate"]           = controlRate;
    m_cfg["disableSynthLogs"]      = disableLogging;
    m_cfg["evBufSize"]             = EvBufSize;
    m_cfg["fastDecay"]             = fastDecay;
    m_cfg["framePeriodSize"]       = audioFrameSize;
    m_cfg["highVelocityThreshold"] = highVelRange;
    m_cfg["lowVelocityThreshold"]  = lowVelRange;
    m_cfg["preResample"]           = preResample;
    m_cfg["sampleRate"]            = sampleRate;
    m_cfg["velocityNoteSkipping"]  = noteVelSkipperEnabled;
    m_cfg["voiceLimit"]            = voiceLimit;
}

json KasariaConfig::getJson() {
    store();
    return m_cfg;
}