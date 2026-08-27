#ifndef KASARIA_CONFIG_H
#define KASARIA_CONFIG_H

#include "omconfig.h"

class KasariaConfig : public SynthConfig
{
public:
    KasariaConfig(OMConfig *omconfig);

    void load() override;
    void store() override;
    json getJson() override;
    std::string name() override {
        return "Kasaria";
    }

    bool disableLogging        = false;
    bool antialiasedSamples    = true;
    bool fastDecay             = true;
    bool preResample           = false;
    bool audioLimiter          = true;
    uint8_t audioVol           = 100;
    uint32_t audioFrameSize    = 10;
    uint32_t controlRate       = 100;
    uint64_t voiceLimit        = 2026;
    uint32_t sampleRate        = 48000;
    bool noteVelSkipperEnabled = false;
    uint8_t lowVelRange        = 0;
    uint8_t highVelRange       = 32;
    uint64_t EvBufSize         = 25536;
    

private:
    json m_cfg;
};

#endif // XSYNTHCONFIG_H