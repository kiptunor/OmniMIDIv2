/*
 * SPDX-License-Identifier: MIT
 *
 * OmniMIDI
 *
 * Copyright (c) 2024 Kiptunor
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the MIT License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * MIT License for more details.
 *
 * You should have received a copy of the MIT License along with this
 * program.  If not, see <https://opensource.org/license/mit/>.
 */

#include "kasaria.h"

#include "../SynthModule.hpp"



// The string used in the JSON config reference
#define KASARIA_STR "Kasaria"



namespace OmniMIDI
{
    class KasariaSettings : public SettingsModule
    {
    public:
        // Settings for Kasaria synth and the defaults
        bool     disableSynthLogs      = false;
        bool     audioLimiter          = true;
        bool     fastDecay             = true;
        bool     antialiasingSamples   = true;
        bool     preResample           = true;
        bool     velocityNoteSkipping  = true;
        uint8_t  lowVelocityThreshold  = 0;
        uint8_t  highVelocityThreshold = 20;
        int      framePeriodSize       = 10;
        int      voiceLimit            = 2026;
        int      amplification         = 100; // 100%
        int      sampleRate            = 48000;
        int      controlRate           = 125;
        int      audioBufferSize       = 32768;
        uint64_t evBufSize             = 25536;



        KasariaSettings(ErrorSystem::Logger *PErr) : SettingsModule(PErr) {}


        // Write JSON Config
        void RewriteSynthConfig()
        {
            nlohmann::json DefConfig =
            {
                ConfGetVal(disableSynthLogs),
                ConfGetVal(audioLimiter),
                ConfGetVal(fastDecay),
                ConfGetVal(antialiasingSamples),
                ConfGetVal(preResample),
                ConfGetVal(velocityNoteSkipping),
                ConfGetVal(lowVelocityThreshold),
                ConfGetVal(highVelocityThreshold),
                ConfGetVal(framePeriodSize),
                ConfGetVal(voiceLimit),
                ConfGetVal(amplification),
                ConfGetVal(sampleRate),
                ConfGetVal(controlRate),
                ConfGetVal(evBufSize),
            };

            if(AppendToConfig(DefConfig))
                WriteConfig();
    
            CloseConfig();
            InitConfig(false, KASARIA_STR, sizeof(KASARIA_STR));
        }

        // Read JSON Config
        void LoadSynthConfig()
        {
            if(InitConfig(false, KASARIA_STR, sizeof(KASARIA_STR)))
            {
                SynthSetVal(bool, disableSynthLogs);
                SynthSetVal(bool, audioLimiter);
                SynthSetVal(bool, fastDecay);
                SynthSetVal(bool, antialiasingSamples);
                SynthSetVal(bool, preResample);
                SynthSetVal(bool, velocityNoteSkipping);
                SynthSetVal(uint8_t, lowVelocityThreshold);
                SynthSetVal(uint8_t, highVelocityThreshold);
                SynthSetVal(int, framePeriodSize);
                SynthSetVal(int, voiceLimit);
                SynthSetVal(int, amplification);
                SynthSetVal(int, sampleRate);
                SynthSetVal(int, controlRate);
                SynthSetVal(uint64_t, evBufSize);
            }

            // If settings for Kasaria don't exist in the JSON file, append them
            if(IsConfigOpen() && !IsSynthConfigValid())
                RewriteSynthConfig();
        }
    };


class KasariaSynth : public SynthModule
{
    private:

    // Pointer symbols that will get populated at dynamic library loading
    static Kasaria* (*ksr_init)(bool disable_logs);
    static int      (*ksr_init_audio)(Kasaria*, int);
    static int      (*ksr_start_audio)(Kasaria*);
    static int      (*ksr_stop_audio)(Kasaria*);
    static int      (*ksr_get_active_voices)(Kasaria*);
    static void     (*ksr_set_config)(Kasaria*, KasariaConfig);
    static void     (*ksr_set_max_voices)(Kasaria*, int);
    static int      (*ksr_load_soundfont_file)(Kasaria*, const char*, bool);
    static void     (*ksr_write_midi_ev)(Kasaria*, unsigned char, unsigned char, unsigned char);
    static void     (*ksr_render_float)(Kasaria *, float *, long);
    static void     (*ksr_write_midi_ev_packed)(Kasaria*, unsigned long);
    static void     (*ksr_write_sysex)(Kasaria*, unsigned char*, long);
    static void     (*ksr_shutdown)(Kasaria*);
        
    Lib *KsrLib = nullptr;

    // Library symbols to load
    LibImport KsrLibImp[13] =
    {
        ImpFunc(ksr_init),
        ImpFunc(ksr_init_audio),
        ImpFunc(ksr_start_audio),
        ImpFunc(ksr_stop_audio),
        ImpFunc(ksr_get_active_voices),
        ImpFunc(ksr_set_config),
        ImpFunc(ksr_render_float),
        ImpFunc(ksr_set_max_voices),
        ImpFunc(ksr_load_soundfont_file),
        ImpFunc(ksr_write_midi_ev),
        ImpFunc(ksr_write_midi_ev_packed),
        ImpFunc(ksr_write_sysex),
        ImpFunc(ksr_shutdown)
    };

    size_t KsrLibImpLen = sizeof(KsrLibImp) / sizeof(KsrLibImp[0]);
    
    void UnloadSoundfonts();
    void EventsThread();
    bool ProcessEvBuf();
        

    KasariaSettings *KsrConfig = nullptr;
public:

    Kasaria *ksr_synth_ctx = nullptr;

    SoundFontSystem _sfSystem;
    int32_t sleepRate = -1;

    KasariaSynth(ErrorSystem::Logger *PErr) : SynthModule(PErr){}
    bool LoadSynthModule() override;
    bool UnloadSynthModule() override;
    bool StartSynthModule() override;
    bool StopSynthModule() override;
    bool SettingsManager(uint32_t setting, bool get, void *var, size_t size) override { return false; }
    uint64_t GetActiveVoices() override;
    uint32_t GetSampleRate() override { return 48000; }
    bool IsSynthInitialized() override;
    uint32_t SynthID() override { return 0x7C4E19A3; }
    void LoadSoundFonts() override;

    // Event handling system
    void PlayShortEvent(uint32_t ev) override;
    void UPlayShortEvent(uint32_t ev) override;

    // Also not supported in Kasaria
    SynthResult TalkToSynthDirectly(uint32_t evt, uint32_t chan, uint32_t param) override { return Ok; }
};
}