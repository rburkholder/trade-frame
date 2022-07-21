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
 * File:    PortAudio.hpp
 * Author:  raymond@burkholder.net
 * Project: lib/OUSound
 * Created: July 16, 2022 16:10:52
 */

#include <functional>

#include <portaudio.h>

namespace ou { // One Unified

class PortAudio {
public:

  using fStream_t = std::function<bool(unsigned long count,float* frames)>;

  PortAudio();
  ~PortAudio();

  double SampleRate() const { return m_dblSampleRate; }

  void Stream( fStream_t&& );

  void Enumerate();

  bool Active();
  bool Stopped();

  void Start();
  void Stop();

protected:
private:

  double m_dblSampleRate;

  PaStream* m_pStream;

  fStream_t m_fStream;

  static int CallBack_Lambda(
    const void* inputBuffer, void* outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void* userData
  );

};

} // namespace ou
