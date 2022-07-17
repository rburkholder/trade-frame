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

#include <portaudio.h>

namespace ou { // One Unified

class PortAudio {
public:
  PortAudio();
  ~PortAudio();

  void Enumerate();

  void Start();
  void Stop();

protected:
private:

  struct Frame {
    float phaseLeft;
    float phaseRight;
    Frame(): phaseLeft {}, phaseRight {} {}
  };

  Frame m_Frame;

  PaStream* m_pStream;

  static int CallBack(
    const void* inputBuffer, void* outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void* userData
  );
};

} // namespace ou
