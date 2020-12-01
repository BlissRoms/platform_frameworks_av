/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_IAUDIOFLINGER_H
#define ANDROID_IAUDIOFLINGER_H

#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

#include <utils/RefBase.h>
#include <utils/Errors.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <binder/Parcelable.h>
#include <media/AidlConversion.h>
#include <media/AudioClient.h>
#include <media/DeviceDescriptorBase.h>
#include <system/audio.h>
#include <system/audio_effect.h>
#include <system/audio_policy.h>
#include <utils/String8.h>
#include <media/MicrophoneInfo.h>
#include <string>
#include <vector>

#include "android/media/CreateEffectRequest.h"
#include "android/media/CreateEffectResponse.h"
#include "android/media/CreateRecordRequest.h"
#include "android/media/CreateRecordResponse.h"
#include "android/media/CreateTrackRequest.h"
#include "android/media/CreateTrackResponse.h"
#include "android/media/IAudioRecord.h"
#include "android/media/IAudioFlingerClient.h"
#include "android/media/IAudioTrack.h"
#include "android/media/IAudioTrackCallback.h"
#include "android/media/IEffect.h"
#include "android/media/IEffectClient.h"
#include "android/media/OpenInputRequest.h"
#include "android/media/OpenInputResponse.h"
#include "android/media/OpenOutputRequest.h"
#include "android/media/OpenOutputResponse.h"

namespace android {

// ----------------------------------------------------------------------------

class IAudioFlinger : public IInterface
{
public:
    DECLARE_META_INTERFACE(AudioFlinger);

    /* CreateTrackInput contains all input arguments sent by AudioTrack to AudioFlinger
     * when calling createTrack() including arguments that will be updated by AudioFlinger
     * and returned in CreateTrackOutput object
     */
    class CreateTrackInput {
    public:
        /* input */
        audio_attributes_t attr;
        audio_config_t config;
        AudioClient clientInfo;
        sp<IMemory> sharedBuffer;
        uint32_t notificationsPerBuffer;
        float speed;
        sp<media::IAudioTrackCallback> audioTrackCallback;
        std::string opPackageName;

        /* input/output */
        audio_output_flags_t flags;
        size_t frameCount;
        size_t notificationFrameCount;
        audio_port_handle_t selectedDeviceId;
        audio_session_t sessionId;

        ConversionResult<media::CreateTrackRequest> toAidl() const;
        static ConversionResult<CreateTrackInput> fromAidl(const media::CreateTrackRequest& aidl);
    };

    /* CreateTrackOutput contains all output arguments returned by AudioFlinger to AudioTrack
     * when calling createTrack() including arguments that were passed as I/O for update by
     * CreateTrackInput.
     */
    class CreateTrackOutput {
    public:
        /* input/output */
        audio_output_flags_t flags;
        size_t frameCount;
        size_t notificationFrameCount;
        audio_port_handle_t selectedDeviceId;
        audio_session_t sessionId;

        /* output */
        uint32_t sampleRate;
        size_t   afFrameCount;
        uint32_t afSampleRate;
        uint32_t afLatencyMs;
        audio_io_handle_t outputId;
        audio_port_handle_t portId;
        sp<media::IAudioTrack> audioTrack;

        ConversionResult<media::CreateTrackResponse> toAidl() const;
        static ConversionResult<CreateTrackOutput> fromAidl(const media::CreateTrackResponse& aidl);
    };

    /* CreateRecordInput contains all input arguments sent by AudioRecord to AudioFlinger
     * when calling createRecord() including arguments that will be updated by AudioFlinger
     * and returned in CreateRecordOutput object
     */
    class CreateRecordInput {
    public:
        /* input */
        audio_attributes_t attr;
        audio_config_base_t config;
        AudioClient clientInfo;
        String16 opPackageName;
        audio_unique_id_t riid;

        /* input/output */
        audio_input_flags_t flags;
        size_t frameCount;
        size_t notificationFrameCount;
        audio_port_handle_t selectedDeviceId;
        audio_session_t sessionId;

        ConversionResult<media::CreateRecordRequest> toAidl() const;
        static ConversionResult<CreateRecordInput> fromAidl(const media::CreateRecordRequest& aidl);
    };

    /* CreateRecordOutput contains all output arguments returned by AudioFlinger to AudioRecord
     * when calling createRecord() including arguments that were passed as I/O for update by
     * CreateRecordInput.
     */
    class CreateRecordOutput {
    public:
        /* input/output */
        audio_input_flags_t flags;
        size_t frameCount;
        size_t notificationFrameCount;
        audio_port_handle_t selectedDeviceId;
        audio_session_t sessionId;

        /* output */
        uint32_t sampleRate;
        audio_io_handle_t inputId;
        sp<IMemory> cblk;
        sp<IMemory> buffers;
        audio_port_handle_t portId;
        sp<media::IAudioRecord> audioRecord;

        ConversionResult<media::CreateRecordResponse> toAidl() const;
        static ConversionResult<CreateRecordOutput> fromAidl(const media::CreateRecordResponse& aidl);
    };

    /* create an audio track and registers it with AudioFlinger.
     * The audioTrack field will be null if the track cannot be created and the status will reflect
     * failure.
     */
    virtual status_t createTrack(const media::CreateTrackRequest& input,
                                 media::CreateTrackResponse& output) = 0;

    /* create an audio record and registers it with AudioFlinger.
     * The audioRecord field will be null if the track cannot be created and the status will reflect
     * failure.
     */
    virtual status_t createRecord(const media::CreateRecordRequest& input,
                                  media::CreateRecordResponse& output) = 0;

    // FIXME Surprisingly, format/latency don't work for input handles

    /* query the audio hardware state. This state never changes,
     * and therefore can be cached.
     */
    virtual     uint32_t    sampleRate(audio_io_handle_t ioHandle) const = 0;

    // reserved; formerly channelCount()

    virtual     audio_format_t format(audio_io_handle_t output) const = 0;
    virtual     size_t      frameCount(audio_io_handle_t ioHandle) const = 0;

    // return estimated latency in milliseconds
    virtual     uint32_t    latency(audio_io_handle_t output) const = 0;

    /* set/get the audio hardware state. This will probably be used by
     * the preference panel, mostly.
     */
    virtual     status_t    setMasterVolume(float value) = 0;
    virtual     status_t    setMasterMute(bool muted) = 0;

    virtual     float       masterVolume() const = 0;
    virtual     bool        masterMute() const = 0;

    virtual     status_t    setMasterBalance(float balance) = 0;
    virtual     status_t    getMasterBalance(float *balance) const = 0;

    /* set/get stream type state. This will probably be used by
     * the preference panel, mostly.
     */
    virtual     status_t    setStreamVolume(audio_stream_type_t stream, float value,
                                    audio_io_handle_t output) = 0;
    virtual     status_t    setStreamMute(audio_stream_type_t stream, bool muted) = 0;

    virtual     float       streamVolume(audio_stream_type_t stream,
                                    audio_io_handle_t output) const = 0;
    virtual     bool        streamMute(audio_stream_type_t stream) const = 0;

    // set audio mode
    virtual     status_t    setMode(audio_mode_t mode) = 0;

    // mic mute/state
    virtual     status_t    setMicMute(bool state) = 0;
    virtual     bool        getMicMute() const = 0;
    virtual     void        setRecordSilenced(audio_port_handle_t portId, bool silenced) = 0;

    virtual     status_t    setParameters(audio_io_handle_t ioHandle,
                                    const String8& keyValuePairs) = 0;
    virtual     String8     getParameters(audio_io_handle_t ioHandle, const String8& keys)
                                    const = 0;

    // Register an object to receive audio input/output change and track notifications.
    // For a given calling pid, AudioFlinger disregards any registrations after the first.
    // Thus the IAudioFlingerClient must be a singleton per process.
    virtual void registerClient(const sp<media::IAudioFlingerClient>& client) = 0;

    // retrieve the audio recording buffer size in bytes
    // FIXME This API assumes a route, and so should be deprecated.
    virtual size_t getInputBufferSize(uint32_t sampleRate, audio_format_t format,
            audio_channel_mask_t channelMask) const = 0;

    virtual status_t openOutput(const media::OpenOutputRequest& request,
                                media::OpenOutputResponse* response) = 0;
    virtual audio_io_handle_t openDuplicateOutput(audio_io_handle_t output1,
                                    audio_io_handle_t output2) = 0;
    virtual status_t closeOutput(audio_io_handle_t output) = 0;
    virtual status_t suspendOutput(audio_io_handle_t output) = 0;
    virtual status_t restoreOutput(audio_io_handle_t output) = 0;

    virtual status_t openInput(const media::OpenInputRequest& request,
                               media::OpenInputResponse* response) = 0;

    virtual status_t closeInput(audio_io_handle_t input) = 0;

    virtual status_t invalidateStream(audio_stream_type_t stream) = 0;

    virtual status_t setVoiceVolume(float volume) = 0;

    virtual status_t getRenderPosition(uint32_t *halFrames, uint32_t *dspFrames,
                                    audio_io_handle_t output) const = 0;

    virtual uint32_t getInputFramesLost(audio_io_handle_t ioHandle) const = 0;

    virtual audio_unique_id_t newAudioUniqueId(audio_unique_id_use_t use) = 0;

    virtual void acquireAudioSessionId(audio_session_t audioSession, pid_t pid, uid_t uid) = 0;
    virtual void releaseAudioSessionId(audio_session_t audioSession, pid_t pid) = 0;

    virtual status_t queryNumberEffects(uint32_t *numEffects) const = 0;

    virtual status_t queryEffect(uint32_t index, effect_descriptor_t *pDescriptor) const = 0;

    virtual status_t getEffectDescriptor(const effect_uuid_t *pEffectUUID,
                                         const effect_uuid_t *pTypeUUID,
                                         uint32_t preferredTypeFlag,
                                         effect_descriptor_t *pDescriptor) const = 0;

    virtual status_t createEffect(const media::CreateEffectRequest& request,
                                  media::CreateEffectResponse* response) = 0;

    virtual status_t moveEffects(audio_session_t session, audio_io_handle_t srcOutput,
                                    audio_io_handle_t dstOutput) = 0;

    virtual void setEffectSuspended(int effectId,
                                    audio_session_t sessionId,
                                    bool suspended) = 0;

    virtual audio_module_handle_t loadHwModule(const char *name) = 0;

    // helpers for android.media.AudioManager.getProperty(), see description there for meaning
    // FIXME move these APIs to AudioPolicy to permit a more accurate implementation
    // that looks on primary device for a stream with fast flag, primary flag, or first one.
    virtual uint32_t getPrimaryOutputSamplingRate() = 0;
    virtual size_t getPrimaryOutputFrameCount() = 0;

    // Intended for AudioService to inform AudioFlinger of device's low RAM attribute,
    // and should be called at most once.  For a definition of what "low RAM" means, see
    // android.app.ActivityManager.isLowRamDevice().  The totalMemory parameter
    // is obtained from android.app.ActivityManager.MemoryInfo.totalMem.
    virtual status_t setLowRamDevice(bool isLowRamDevice, int64_t totalMemory) = 0;

    /* List available audio ports and their attributes */
    virtual status_t listAudioPorts(unsigned int *num_ports,
                                    struct audio_port *ports) = 0;

    /* Get attributes for a given audio port */
    virtual status_t getAudioPort(struct audio_port_v7 *port) = 0;

    /* Create an audio patch between several source and sink ports */
    virtual status_t createAudioPatch(const struct audio_patch *patch,
                                       audio_patch_handle_t *handle) = 0;

    /* Release an audio patch */
    virtual status_t releaseAudioPatch(audio_patch_handle_t handle) = 0;

    /* List existing audio patches */
    virtual status_t listAudioPatches(unsigned int *num_patches,
                                      struct audio_patch *patches) = 0;
    /* Set audio port configuration */
    virtual status_t setAudioPortConfig(const struct audio_port_config *config) = 0;

    /* Get the HW synchronization source used for an audio session */
    virtual audio_hw_sync_t getAudioHwSyncForSession(audio_session_t sessionId) = 0;

    /* Indicate JAVA services are ready (scheduling, power management ...) */
    virtual status_t systemReady() = 0;

    // Returns the number of frames per audio HAL buffer.
    virtual size_t frameCountHAL(audio_io_handle_t ioHandle) const = 0;

    /* List available microphones and their characteristics */
    virtual status_t getMicrophones(std::vector<media::MicrophoneInfo> *microphones) = 0;

    virtual status_t setAudioHalPids(const std::vector<pid_t>& pids) = 0;
};


// ----------------------------------------------------------------------------

class BnAudioFlinger : public BnInterface<IAudioFlinger>
{
public:
    virtual status_t    onTransact( uint32_t code,
                                    const Parcel& data,
                                    Parcel* reply,
                                    uint32_t flags = 0);

    // Requests media.log to start merging log buffers
    virtual void requestLogMerge() = 0;
};

// ----------------------------------------------------------------------------

}; // namespace android

#endif // ANDROID_IAUDIOFLINGER_H
