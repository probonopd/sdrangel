///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Edouard Griffiths, F4EXB                                   //
//                                                                               //
// This program is free software; you can redistribute it and/or modify          //
// it under the terms of the GNU General Public License as published by          //
// the Free Software Foundation as version 3 of the License, or                  //
//                                                                               //
// This program is distributed in the hope that it will be useful,               //
// but WITHOUT ANY WARRANTY; without even the implied warranty of                //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  //
// GNU General Public License V3 for more details.                               //
//                                                                               //
// You should have received a copy of the GNU General Public License             //
// along with this program. If not, see <http://www.gnu.org/licenses/>.          //
///////////////////////////////////////////////////////////////////////////////////

#ifndef PLUGINS_SAMPLESOURCE_BLADERF2INPUT_BLADERF2INPUT_H_
#define PLUGINS_SAMPLESOURCE_BLADERF2INPUT_BLADERF2INPUT_H_

#include <QString>
#include <QByteArray>
#include <stdint.h>

#include "dsp/devicesamplesource.h"
#include "bladerf2/devicebladerf2shared.h"
#include "bladerf2inputsettings.h"

class DeviceSourceAPI;
class BladeRF2InputThread;
class FileRecord;
struct bladerf_gain_modes;
struct bladerf;

class BladeRF2Input : public DeviceSampleSource
{
public:
    class MsgConfigureBladeRF2 : public Message {
        MESSAGE_CLASS_DECLARATION

    public:
        const BladeRF2InputSettings& getSettings() const { return m_settings; }
        bool getForce() const { return m_force; }

        static MsgConfigureBladeRF2* create(const BladeRF2InputSettings& settings, bool force)
        {
            return new MsgConfigureBladeRF2(settings, force);
        }

    private:
        BladeRF2InputSettings m_settings;
        bool m_force;

        MsgConfigureBladeRF2(const BladeRF2InputSettings& settings, bool force) :
            Message(),
            m_settings(settings),
            m_force(force)
        { }
    };

    class MsgFileRecord : public Message {
        MESSAGE_CLASS_DECLARATION

    public:
        bool getStartStop() const { return m_startStop; }

        static MsgFileRecord* create(bool startStop) {
            return new MsgFileRecord(startStop);
        }

    protected:
        bool m_startStop;

        MsgFileRecord(bool startStop) :
            Message(),
            m_startStop(startStop)
        { }
    };

    class MsgStartStop : public Message {
        MESSAGE_CLASS_DECLARATION

    public:
        bool getStartStop() const { return m_startStop; }

        static MsgStartStop* create(bool startStop) {
            return new MsgStartStop(startStop);
        }

    protected:
        bool m_startStop;

        MsgStartStop(bool startStop) :
            Message(),
            m_startStop(startStop)
        { }
    };

    class MsgReportGainRange : public Message {
        MESSAGE_CLASS_DECLARATION

    public:
        int getMin() const { return m_min; }
        int getMax() const { return m_max; }
        int getStep() const { return m_step; }

        static MsgReportGainRange* create(int min, int max, int step) {
            return new MsgReportGainRange(min, max, step);
        }

    protected:
        int m_min;
        int m_max;
        int m_step;

        MsgReportGainRange(int min, int max, int step) :
            Message(),
            m_min(min),
            m_max(max),
            m_step(step)
        {}
    };

    struct GainMode
    {
        QString m_name;
        int m_value;
    };

    BladeRF2Input(DeviceSourceAPI *deviceAPI);
    virtual ~BladeRF2Input();
    virtual void destroy();

    virtual void init();
    virtual bool start();
    virtual void stop();
    BladeRF2InputThread *getThread() { return m_thread; }
    void setThread(BladeRF2InputThread *thread) { m_thread = thread; }

    virtual QByteArray serialize() const;
    virtual bool deserialize(const QByteArray& data);

    virtual void setMessageQueueToGUI(MessageQueue *queue) { m_guiMessageQueue = queue; }
    virtual const QString& getDeviceDescription() const;
    virtual int getSampleRate() const;
    virtual quint64 getCenterFrequency() const;
    virtual void setCenterFrequency(qint64 centerFrequency);

    void getFrequencyRange(uint64_t& min, uint64_t& max, int& step);
    void getSampleRateRange(int& min, int& max, int& step);
    void getBandwidthRange(int& min, int& max, int& step);
    void getGlobalGainRange(int& min, int& max, int& step);
    const std::vector<GainMode>& getGainModes() { return m_gainModes; }

    virtual bool handleMessage(const Message& message);

    virtual int webapiSettingsGet(
                SWGSDRangel::SWGDeviceSettings& response,
                QString& errorMessage);

    virtual int webapiSettingsPutPatch(
                bool force,
                const QStringList& deviceSettingsKeys,
                SWGSDRangel::SWGDeviceSettings& response, // query + response
                QString& errorMessage);

    virtual int webapiReportGet(
            SWGSDRangel::SWGDeviceReport& response,
            QString& errorMessage);

    virtual int webapiRunGet(
            SWGSDRangel::SWGDeviceState& response,
            QString& errorMessage);

    virtual int webapiRun(
            bool run,
            SWGSDRangel::SWGDeviceState& response,
            QString& errorMessage);

private:
    DeviceSourceAPI *m_deviceAPI;
    QMutex m_mutex;
    BladeRF2InputSettings m_settings;
    QString m_deviceDescription;
    bool m_running;
    DeviceBladeRF2Shared m_deviceShared;
    BladeRF2InputThread *m_thread;
    FileRecord *m_fileSink; //!< File sink to record device I/Q output
    std::vector<GainMode> m_gainModes;

    bool openDevice();
    void closeDevice();
    BladeRF2InputThread *findThread();
    void moveThreadToBuddy();
    bool applySettings(const BladeRF2InputSettings& settings, bool force = false);
    bool setDeviceCenterFrequency(struct bladerf *dev, int requestedChannel, quint64 freq_hz, int loPpmTenths);
    void webapiFormatDeviceSettings(SWGSDRangel::SWGDeviceSettings& response, const BladeRF2InputSettings& settings);
    void webapiFormatDeviceReport(SWGSDRangel::SWGDeviceReport& response);
};

#endif /* PLUGINS_SAMPLESOURCE_BLADERF2INPUT_BLADERF2INPUT_H_ */
