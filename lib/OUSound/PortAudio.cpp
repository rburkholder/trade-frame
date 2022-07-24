/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

/*
 * File:    PortAudio.cpp
 * Author:  raymond@burkholder.net
 * Project: lib/OUSound
 * Created: July 16, 2022 16:10:52
 */

// use libsndfile to load various files/file-formats for playback

#include <cassert>
#include <iostream>
#include <stdexcept>

#include "Music.hpp"
#include "PortAudio.hpp"

namespace {
  double freqSampleRate( 44100 );
}

namespace ou { // One Unified

PortAudio::PortAudio()
: m_pStream( nullptr )
{
  // http://portaudio.com/docs/v19-doxydocs/initializing_portaudio.html
  PaError pa_error;
  //std::cout << "Pa_Initialize" << std::endl;
  // many device probing messages will occur at this point on the console
  pa_error = Pa_Initialize();
  if ( paNoError != pa_error ) {
    std::cerr << "PortAudio construct error: " << Pa_GetErrorText( pa_error ) << std::endl;
    throw std::runtime_error( Pa_GetErrorText( pa_error ) );
  }

  int numDevices = Pa_GetDeviceCount();
  if( 0 >= numDevices ) {
    std::cerr << "PortAudio: Pa_CountDevices returned " << numDevices << " devices" << std::endl;
  }
  else {

    PaDeviceIndex index = Pa_GetDefaultOutputDevice();
    const PaDeviceInfo* pInfo = Pa_GetDeviceInfo( index );
    //const PaDeviceInfo* pInfo = Pa_GetDeviceInfo( 14 ); // "pulse"
    m_dblSampleRate = pInfo->defaultSampleRate;

    //std::cout
    //  << "sound default device '" << pInfo->name
    //  << "' channels " << pInfo->maxOutputChannels
    //  << " sample rate " << defaultSampleRate
    //  << std::endl;

    assert( 2 <= pInfo->maxOutputChannels );

    PaStreamParameters parameters;
    parameters.device = index;
    parameters.channelCount = 2;
    parameters.sampleFormat = paFloat32;
    parameters.suggestedLatency = pInfo->defaultHighOutputLatency;
    parameters.hostApiSpecificStreamInfo = nullptr;

    pa_error = Pa_OpenStream( // Open an audio I/O stream.
      &m_pStream,
      nullptr,          /* no input channels */
      &parameters,      /* output channel */
      m_dblSampleRate,
      paFramesPerBufferUnspecified,
      paClipOff|paDitherOff, //paNoFlag,
      &CallBack_Lambda,
      this              // reference self in the callback
      );

    if ( paNoError != pa_error ) {
      std::cerr << "PortAudio open error: " << Pa_GetErrorText( pa_error ) << std::endl;
      throw std::runtime_error( Pa_GetErrorText( pa_error ) );
    }

  }

  // http://portaudio.com/docs/v19-doxydocs/utility_functions.html
  //const PaStreamInfo* pInfo;
  //std::cout << "Pa_GetStreamInfo" << std::endl;
  //pInfo = Pa_GetStreamInfo( m_pStream );

}

PortAudio::~PortAudio() {

  PaError pa_error;

  // http://portaudio.com/docs/v19-doxydocs/start_stop_abort.html
  if ( m_pStream ) {
    // http://portaudio.com/docs/v19-doxydocs/terminating_portaudio.html
    pa_error = Pa_CloseStream( m_pStream );
    if ( paNoError != pa_error ) {
      std::cerr << "PortAudio close error: " << Pa_GetErrorText( pa_error ) << std::endl;
    }
  }

  // http://portaudio.com/docs/v19-doxydocs/initializing_portaudio.html
  pa_error = Pa_Terminate();
  if ( paNoError != pa_error ) {
    std::cerr << "PortAudio destruct error: " << Pa_GetErrorText( pa_error ) << std::endl;
  }
}

bool PortAudio::Active() {
  bool bActive( false );
  if ( m_pStream ) {
    bActive = Pa_IsStreamActive( m_pStream );
  }
  return bActive;
}

bool PortAudio::Stopped() {
  bool bStopped( true );
  if ( m_pStream ) {
    bStopped = Pa_IsStreamStopped( m_pStream );
  }
  return bStopped;
}

void PortAudio::Stream( fStream_t&& fStream ) {
  if ( Pa_IsStreamStopped( m_pStream ) ) {
    m_fStream = std::move( fStream );
  }
}

/* This routine will be called by the PortAudio engine when audio is needed.
 * It may called at interrupt level on some machines so don't do anything
 * that could mess up the system like calling malloc() or free().
*/
// http://portaudio.com/docs/v19-doxydocs/writing_a_callback.html

int PortAudio::CallBack_Lambda(
  const void* inputBuffer, void* outputBuffer
, unsigned long framesPerBuffer
, const PaStreamCallbackTimeInfo* timeInfo
, PaStreamCallbackFlags statusFlags
, void* userData
) {
  /* Cast data passed through stream to our structure. */
  PortAudio* self = reinterpret_cast<PortAudio*>( userData );
  float* pOutput = reinterpret_cast<float*>( outputBuffer );

  assert(self->m_fStream );
  bool bContinue = self->m_fStream( framesPerBuffer, pOutput );
  if ( bContinue ) {
    return  PaStreamCallbackResult::paContinue;
  }
  else {
    return  PaStreamCallbackResult::paComplete;
  }
}

void PortAudio::Enumerate() {
  // http://portaudio.com/docs/v19-doxydocs/querying_devices.html
  int numDevices = Pa_GetDeviceCount();
  if( 0 >= numDevices ) {
    std::cerr << "PortAudio: Pa_CountDevices returned " << numDevices << " devices" << std::endl;
  }
  else {
    const PaDeviceInfo* pDeviceInfo;
    for ( int i = 0; i < numDevices; i++ ) {
      pDeviceInfo = Pa_GetDeviceInfo( i );
      std::cout
        << "PortAudio device "
        << i
        << ": '" << pDeviceInfo->name
        << "' sample rate " << pDeviceInfo->defaultSampleRate
        << std::endl;
    }
  }
}

void PortAudio::Start() {
  PaError pa_error;
  // http://portaudio.com/docs/v19-doxydocs/start_stop_abort.html
  if ( m_pStream ) {
    //std::cout << "Pa_StartStream" << std::endl;
    pa_error = Pa_StartStream( m_pStream );
    if ( paNoError != pa_error ) {
      std::cerr << "PortAudio start error: " << Pa_GetErrorText( pa_error ) << std::endl;
      throw std::runtime_error( Pa_GetErrorText( pa_error ) );
    }
  }
}

void PortAudio::Stop() {
  PaError pa_error;
  if ( m_pStream ) {
    pa_error = Pa_StopStream( m_pStream );
    if ( paNoError != pa_error ) {
      std::cerr << "PortAudio stop error: " << Pa_GetErrorText( pa_error ) << std::endl;
    }
  }
}

} // namespace ou
