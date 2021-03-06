/*
 * Copyright 2019 Alex Andres
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "JNI_MediaDevices.h"
#include "JavaArrayList.h"
#include "JavaError.h"
#include "JavaObject.h"
#include "JavaString.h"
#include "media/Device.h"
#include "media/video/VideoCaptureCapability.h"

#include "api/scoped_refptr.h"
#include "api/task_queue/default_task_queue_factory.h"
#include "modules/audio_device/include/audio_device.h"
#include "modules/video_capture/video_capture_factory.h"

JNIEXPORT jobject JNICALL Java_dev_onvoid_webrtc_media_MediaDevices_getAudioRenderDevices
(JNIEnv * env, jclass caller)
{
	std::unique_ptr<webrtc::TaskQueueFactory> taskQueueFactory = webrtc::CreateDefaultTaskQueueFactory();

	if (!taskQueueFactory) {
		env->Throw(jni::JavaError(env, "Create TaskQueueFactory failed"));
		return nullptr;
	}

	rtc::scoped_refptr<webrtc::AudioDeviceModule> audioModule = webrtc::AudioDeviceModule::Create(
		webrtc::AudioDeviceModule::kPlatformDefaultAudio, taskQueueFactory.get());

	if (!audioModule) {
		env->Throw(jni::JavaError(env, "Create AudioDeviceModule failed"));
		return nullptr;
	}

	if (audioModule->Init() != 0) {
		env->Throw(jni::JavaError(env, "Initialize AudioDeviceModule failed"));
		return nullptr;
	}

	char name[webrtc::kAdmMaxDeviceNameSize];
	char guid[webrtc::kAdmMaxGuidSize];

	int16_t deviceCount = audioModule->PlayoutDevices();

	jni::JavaArrayList deviceList(env, deviceCount);

	for (int i = 0; i < deviceCount; ++i) {
		if (audioModule->PlayoutDeviceName(i, name, guid) == 0) {
			deviceList.add(jni::Device::toJavaAudioDevice(env, name, guid));
		}
	}

	return deviceList.listObject().release();
}

JNIEXPORT jobject JNICALL Java_dev_onvoid_webrtc_media_MediaDevices_getAudioCaptureDevices
(JNIEnv * env, jclass caller)
{
	std::unique_ptr<webrtc::TaskQueueFactory> taskQueueFactory = webrtc::CreateDefaultTaskQueueFactory();

	if (!taskQueueFactory) {
		env->Throw(jni::JavaError(env, "Create TaskQueueFactory failed"));
		return nullptr;
	}

	rtc::scoped_refptr<webrtc::AudioDeviceModule> audioModule = webrtc::AudioDeviceModule::Create(
		webrtc::AudioDeviceModule::kPlatformDefaultAudio, taskQueueFactory.get());

	if (!audioModule) {
		env->Throw(jni::JavaError(env, "Create AudioDeviceModule failed"));
		return nullptr;
	}

	if (audioModule->Init() != 0) {
		env->Throw(jni::JavaError(env, "Initialize AudioDeviceModule failed"));
		return nullptr;
	}

	char name[webrtc::kAdmMaxDeviceNameSize];
	char guid[webrtc::kAdmMaxGuidSize];

	int16_t deviceCount = audioModule->RecordingDevices();

	jni::JavaArrayList deviceList(env, deviceCount);

	for (int i = 0; i < deviceCount; ++i) {
		if (audioModule->RecordingDeviceName(i, name, guid) == 0) {
			deviceList.add(jni::Device::toJavaAudioDevice(env, name, guid));
		}
	}

	return deviceList.listObject().release();
}

JNIEXPORT jobject JNICALL Java_dev_onvoid_webrtc_media_MediaDevices_getVideoCaptureDevices
(JNIEnv * env, jclass caller)
{
	std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> info(webrtc::VideoCaptureFactory::CreateDeviceInfo());

	if (!info) {
		env->Throw(jni::JavaError(env, "Create video DeviceInfo failed"));
		return nullptr;
	}

	uint32_t deviceCount = info->NumberOfDevices();

	jni::JavaArrayList deviceList(env, deviceCount);

	if (deviceCount > 0) {
		const uint32_t size = webrtc::kVideoCaptureDeviceNameLength;

		for (uint32_t i = 0; i < deviceCount; ++i) {
			char name[size] = { 0 };
			char guid[size] = { 0 };

			if (info->GetDeviceName(i, name, size, guid, size) == 0) {
				deviceList.add(jni::Device::toJavaVideoDevice(env, name, guid));
			}
		}
	}

	return deviceList.listObject().release();
}

JNIEXPORT jobject JNICALL Java_dev_onvoid_webrtc_media_MediaDevices_getVideoCaptureCapabilities
(JNIEnv * env, jclass caller, jobject device)
{
	if (!device) {
		env->Throw(jni::JavaNullPointerException(env, "VideoDevice is null"));
		return nullptr;
	}

	std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> info(webrtc::VideoCaptureFactory::CreateDeviceInfo());

	if (!info) {
		env->Throw(jni::JavaError(env, "Create video DeviceInfo failed"));
		return nullptr;
	}

	jni::JavaObject obj(env, jni::JavaLocalRef<jobject>(env, device));

	const auto javaClass = jni::JavaClasses::get<jni::Device::JavaAudioDeviceClass>(env);
	const std::string guid = jni::JavaString::toNative(env, obj.getString(javaClass->guid));

	uint32_t capabilitiesCount = info->NumberOfCapabilities(guid.data());

	jni::JavaArrayList deviceList(env, capabilitiesCount);

	if (capabilitiesCount > 0) {
		for (uint32_t i = 0; i < capabilitiesCount; ++i) {
			webrtc::VideoCaptureCapability capability;

			if (info->GetCapability(guid.data(), i, capability) == 0) {
				deviceList.add(jni::VideoCaptureCapability::toJava(env, capability));
			}
		}
	}

	return deviceList.listObject().release();
}