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

#include <iostream>
#include <stdexcept>

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

  // http://portaudio.com/docs/v19-doxydocs/open_default_stream.html
  //std::cout << "Pa_OpenDefaultStream" << std::endl;
  pa_error = Pa_OpenDefaultStream( // Open an audio I/O stream.
    &m_pStream,
    0,          /* no input channels */
    2,          /* stereo output */
    paFloat32,  /* 32 bit floating point output */
    freqSampleRate,
    //256,        /* frames per buffer, i.e. the number
    paFramesPerBufferUnspecified, /*
                  of sample frames that PortAudio will
                  request from the callback. Many apps
                  may want to use
                  paFramesPerBufferUnspecified, which
                  tells PortAudio to pick the best,
                  possibly changing, buffer size.*/
    &CallBack, // this is your callback function
    &m_Frame   // This is a pointer that will be passed to your callback
    );

  if ( paNoError != pa_error ) {
    std::cerr << "PortAudio open error: " << Pa_GetErrorText( pa_error ) << std::endl;
    throw std::runtime_error( Pa_GetErrorText( pa_error ) );
  }

  // http://portaudio.com/docs/v19-doxydocs/utility_functions.html
  const PaStreamInfo* pInfo;
  //std::cout << "Pa_GetStreamInfo" << std::endl;
  pInfo = Pa_GetStreamInfo ( m_pStream );

  Start();

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

/* This routine will be called by the PortAudio engine when audio is needed.
 * It may called at interrupt level on some machines so don't do anything
 * that could mess up the system like calling malloc() or free().
*/
// http://portaudio.com/docs/v19-doxydocs/writing_a_callback.html
int PortAudio::CallBack(
  const void* inputBuffer, void* outputBuffer
, unsigned long framesPerBuffer
, const PaStreamCallbackTimeInfo* timeInfo
, PaStreamCallbackFlags statusFlags
, void* userData
) {
  /* Cast data passed through stream to our structure. */
  Frame* pFrame = reinterpret_cast<Frame*>( userData );
  float* pOutput = reinterpret_cast<float*>( outputBuffer );
  unsigned long i;
  //(void) inputBuffer; /* Prevent unused variable warning. */

  for( i = 0; i < framesPerBuffer; i++ ) {
      *(pOutput++) = pFrame->phaseLeft;
      *(pOutput++) = pFrame->phaseRight;
    /* Generate simple sawtooth phaser that ranges between -1.0 and 1.0. */
    pFrame->phaseLeft += 0.01f;
    /* When signal reaches top, drop back down. */
    if( pFrame->phaseLeft >= 1.0f ) pFrame->phaseLeft -= 2.0f;
    /* higher pitch so we can distinguish left and right. */
    pFrame->phaseRight += 0.03f;
    if( pFrame->phaseRight >= 1.0f ) pFrame->phaseRight -= 2.0f;
  }
  return 0;
}

void PortAudio::Enumerate() {
  // http://portaudio.com/docs/v19-doxydocs/querying_devices.html
  int numDevices = Pa_GetDeviceCount();
  if( numDevices <= 0 ) {
    std::cerr << "PortAudio: Pa_CountDevices returned " << numDevices << " devices" << std::endl;
  }
  else {
    const PaDeviceInfo* pDeviceInfo;
    for( int i = 0; i < numDevices; i++ ) {
      pDeviceInfo = Pa_GetDeviceInfo( i );
      std::cout
        << "PortAudio device " << pDeviceInfo->name
        << " sample rate " << pDeviceInfo->defaultSampleRate
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
    if( pa_error != paNoError ) {
      std::cerr << "PortAudio stop error: " << Pa_GetErrorText( pa_error ) << std::endl;
    }
  }
}

} // namespace ou
