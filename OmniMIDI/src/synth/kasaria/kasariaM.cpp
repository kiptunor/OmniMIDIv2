



#include "kasaria.h"
#include "kasariaM.hpp"




// Symbol pointer declarations
Kasaria* (*OmniMIDI::KasariaSynth::ksr_init)(bool disable_logs) = nullptr;
int      (*OmniMIDI::KasariaSynth::ksr_init_audio)(Kasaria*, int) = nullptr;
int      (*OmniMIDI::KasariaSynth::ksr_start_audio)(Kasaria*) = nullptr;
int      (*OmniMIDI::KasariaSynth::ksr_stop_audio)(Kasaria*) = nullptr;
void     (*OmniMIDI::KasariaSynth::ksr_set_config)(Kasaria*, KasariaConfig) = nullptr;
void     (*OmniMIDI::KasariaSynth::ksr_set_max_voices)(Kasaria*, int) = nullptr;
int      (*OmniMIDI::KasariaSynth::ksr_load_soundfont_file)(Kasaria*, const char*, bool) = nullptr;
void     (*OmniMIDI::KasariaSynth::ksr_write_midi_ev)(Kasaria*, unsigned char, unsigned char, unsigned char) = nullptr;
void     (*OmniMIDI::KasariaSynth::ksr_write_midi_ev_packed)(Kasaria*, unsigned long) = nullptr;
void     (*OmniMIDI::KasariaSynth::ksr_write_sysex)(Kasaria*, unsigned char*, long) = nullptr;
void     (*OmniMIDI::KasariaSynth::ksr_shutdown)(Kasaria*) = nullptr;

bool OmniMIDI::KasariaSynth::LoadSynthModule()
{
    auto ptr = (LibImport *)KsrLibImp;
    KsrConfig = LoadSynthConfig<KasariaSettings>();

    if(KsrConfig == nullptr)
        return false;

    if(!KsrLib)
        KsrLib = new Lib("kasaria", nullptr, ErrLog, &ptr, KsrLibImpLen);

    if(KsrLib->IsOnline())
        return true;

    if(!KsrLib->LoadLib())
        return false;

    return true;
}

bool OmniMIDI::KasariaSynth::UnloadSynthModule()
{
    FreeSynthConfig(KsrConfig);
    
    if(!KsrLib)
        return true;

    if(!KsrLib->UnloadLib())
        return false;

    return true;
}

void OmniMIDI::KasariaSynth::LoadSoundFonts()
{
    if(_sfSystem.ClearList())
    {
        _sfVec = _sfSystem.LoadList();

        if(_sfVec == nullptr)
            return;

        auto &_sfVecIter = *_sfVec;
        
        if(_sfVecIter.size() < 1)
            return;

        // Try to load soundfonts in reversed order
        for(size_t i = _sfVecIter.size(); i-- > 0;)
        {
            auto item = _sfVecIter[i];
            auto sfPath = item.path.c_str();

            // Skip disabled soundfonts
            if(!item.enabled)
                continue;

            // Load the actual soundfont
            ksr_load_soundfont_file(ksr_synth_ctx, sfPath, true); // And preload its instruments
        }
    }
}

bool OmniMIDI::KasariaSynth::IsSynthInitialized()
{
    if(!KsrLib)
        return false;

    if(ksr_synth_ctx == nullptr)
        return false;

    return true;
}

bool OmniMIDI::KasariaSynth::StartSynthModule()
{
    if(IsSynthInitialized())
        return true;


    if(!KsrConfig)
        return false;

    

    // Initialize the Kasaria synth context
    ksr_synth_ctx = ksr_init(KsrConfig->disableSynthLogs);

    // Configure the synth with the following settings
    KasariaConfig ksr_settings =
    {
        .amplification      = KsrConfig->amplification,
        .voice_limit        = KsrConfig->voiceLimit,
        .sample_rate        = KsrConfig->sampleRate,
        .control_rate       = KsrConfig->controlRate,
        .low_note_velocity  = KsrConfig->lowVelocityThreshold,
        .high_note_velocity = KsrConfig->highVelocityThreshold,
        .audio_frame_size   = KsrConfig->framePeriodSize,
        .fast_decay         = KsrConfig->fastDecay,
        .antialiasing       = KsrConfig->antialiasingSamples,
        .pre_resample       = KsrConfig->preResample,
        .velocity_skipping  = KsrConfig->velocityNoteSkipping,
        .audio_compressor   = KsrConfig->audioLimiter,
    };

    // Override default settings with the current settings from ^^^
    ksr_set_config(ksr_synth_ctx, ksr_settings);

    // Load the enabled soundfonts
    LoadSoundFonts();

    _sfSystem.RegisterCallback(this);

    // Initialize the audio thread for raw MIDI events (Internal audio handler)
    ksr_init_audio(ksr_synth_ctx, RAW_MIDI_EVENTS);

    // Then start the audio thread
    ksr_start_audio(ksr_synth_ctx);

    //StartDebugOutput();

    return true;
}

void OmniMIDI::KasariaSynth::UPlayShortEvent(unsigned int ev)
{
    // This ain't good tbh
    ksr_write_midi_ev_packed(ksr_synth_ctx, ev);
}

void OmniMIDI::KasariaSynth::PlayShortEvent(unsigned int ev)
{
    if(!KsrLib->IsOnline() || !IsSynthInitialized())
        return;

    UPlayShortEvent(ev);
}

uint64_t OmniMIDI::KasariaSynth::GetActiveVoices()
{
    return ksr_get_active_voices(ksr_synth_ctx);
}

bool OmniMIDI::KasariaSynth::StopSynthModule()
{
    _sfSystem.RegisterCallback();

    if(IsSynthInitialized())
    {
        ksr_stop_audio(ksr_synth_ctx);
        ksr_shutdown(ksr_synth_ctx);
    }

    return true;
}