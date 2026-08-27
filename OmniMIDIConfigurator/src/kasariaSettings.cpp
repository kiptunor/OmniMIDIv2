#include "kasariaSettings.h"
#include "ui_kasariaSettings.h"
#include <QString>
#include "utils.h"

KasariaSettings::KasariaSettings(QWidget *parent, KasariaConfig *config)
    : QWidget(parent)
    , ui(new Ui::KasariaSettings)
    , m_cfg(config)
{
    ui->setupUi(this);
}

void KasariaSettings::loadSettings()
{
    ui->cb_disableSynthLogs->setCheckState(CCS(m_cfg->disableLogging));
    ui->cb_antialiasedSamples->setCheckState(CCS(m_cfg->antialiasedSamples));
    ui->cb_fastDecay->setCheckState(CCS(m_cfg->fastDecay));
    ui->cb_preResample->setCheckState(CCS(m_cfg->preResample));
    ui->cb_audioLimter->setCheckState(CCS(m_cfg->audioLimiter));
    ui->sl_audioVol->setValue(m_cfg->audioVol);
    ui->spb_framePeriodSize->setValue(m_cfg->audioFrameSize);
    ui->spb_ctrlRate->setValue(m_cfg->controlRate);
    ui->spb_voiceLimit->setValue(m_cfg->voiceLimit);
    ui->srs_smplRate->setRate(m_cfg->sampleRate);
    ui->cb_noteVelSkip->setCheckState(CCS(m_cfg->noteVelSkipperEnabled));
    ui->spb_lowVelRange->setValue(m_cfg->lowVelRange);
    ui->spb_highVelRange->setValue(m_cfg->highVelRange);
    ui->spb_evBufSize->setValue(m_cfg->EvBufSize);
}

void KasariaSettings::storeSettings()
{
    m_cfg->disableLogging        = ui->cb_disableSynthLogs->isChecked();
    m_cfg->antialiasedSamples    = ui->cb_antialiasedSamples->isChecked();
    m_cfg->fastDecay             = ui->cb_fastDecay->isChecked();
    m_cfg->preResample           = ui->cb_preResample->isChecked();
    m_cfg->audioLimiter          = ui->cb_audioLimter->isChecked();
    m_cfg->audioVol              = ui->sl_audioVol->value();
    m_cfg->audioFrameSize        = ui->spb_framePeriodSize->value();
    m_cfg->controlRate           = ui->spb_ctrlRate->value();
    m_cfg->voiceLimit            = ui->spb_voiceLimit->value();
    m_cfg->sampleRate            = ui->srs_smplRate->getRate();
    m_cfg->noteVelSkipperEnabled = ui->cb_noteVelSkip->isChecked();
    m_cfg->lowVelRange           = ui->spb_lowVelRange->value();
    m_cfg->highVelRange          = ui->spb_highVelRange->value();
    m_cfg->EvBufSize             = ui->spb_evBufSize->value();
    
}

QWidget *KasariaSettings::getWidget() {
    return this;
}

KasariaSettings::~KasariaSettings() {
    delete m_cfg;
    delete ui;
}
