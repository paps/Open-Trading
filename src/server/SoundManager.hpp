// The Open Trading Project - open-trading.org
//
// Copyright (c) 2011 Martin Tapia - martin.tapia@open-trading.org
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef __SERVER_SOUNDMANAGER__
#define __SERVER_SOUNDMANAGER__

#include "CMakeFmodEx.hpp"

#ifdef CMAKE_FMODEX
    #include <map>
    #include <string>
    #include "sound/Sound.hpp"
#endif

namespace Sound
{
    class SoundSystem;
}

namespace Server
{
    class Gui;
    class Logger;

    class SoundManager
    {
        public:
            enum
            {
                SoundError,
                SoundWarning,
                SoundBarNext,
                SoundUp,
                SoundDown,
                SoundSuccess,
                SoundFailure,
            };
            SoundManager(Gui& gui);
            ~SoundManager();
            void PlaySound(int id);
            void EnableSound(int id, bool enable);
        private:
#ifdef CMAKE_FMODEX
            struct SoundEntity
            {
                Sound::Sound sound;
                bool enabled;
            };
            void _AddSound(int id, std::string const& path, bool enabled);
            Sound::SoundSystem* _soundSystem;
            std::map<int, SoundEntity*> _sounds;
#endif
            Gui& _gui;
            Logger* _soundLogger;
    };
}

#endif
