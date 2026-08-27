#ifndef KASARIA_SETTINGS_H
#define KASARIA_SETTINGS_H

#include <QWidget>
#include "synthsettings.h"
#include "kasariaConfig.h"

namespace Ui {
class KasariaSettings;
}

class KasariaSettings : public QWidget, public SynthSettings
{
    Q_OBJECT

public:
    explicit KasariaSettings(QWidget *parent = nullptr, KasariaConfig *config = nullptr);
    ~KasariaSettings();
    void loadSettings() override;
    void storeSettings() override;
    QWidget *getWidget() override;

private:
    Ui::KasariaSettings *ui;
    KasariaConfig *m_cfg = nullptr;
};

#endif // XSYNTHSETTINGS_H