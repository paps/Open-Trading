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

#include <iostream>
#include <fmodex/fmod_errors.h>
#include "SoundSystem.hpp"
#include "logger/Logger.hpp"
#include "tools/ToString.hpp"

#define CLASS "[Sound/SoundSystem] "

namespace Sound
{
    SoundSystem::SoundSystem(bool debug, Logger::Logger const& logger) :
        _debug(debug), _logger(logger)
    {
        if (this->_debug)
            this->_logger.Log(CLASS "Sound system running in debug mode.");
        FMOD_RESULT result;
        if ((result = FMOD::System_Create(&this->_system)) != FMOD_OK)
        {
            this->_system = 0;
            this->_ShowError("Failed to create FMOD sound system.", result);
            return;
        }
        if ((result = this->_system->init(32, FMOD_INIT_NORMAL, 0)) != FMOD_OK)
        {
            this->_system = 0;
            this->_ShowError("Failed to initialize FMOD.", result);
            return;
        }
        if (this->_debug)
            this->_logger.Log(CLASS "FMOD sound system initialized.");
    }

    SoundSystem::~SoundSystem()
    {
        if (this->_system == 0)
            return;
        FMOD_RESULT result;
        if ((result = this->_system->release()) != FMOD_OK)
            this->_ShowError("Failed to release FMOD sound system.", result);
        else if (this->_debug)
            this->_logger.Log(CLASS "FMOD sound system released.");
    }

    void SoundSystem::_ShowError(std::string const& message, FMOD_RESULT code)
    {
        this->_logger.Log(CLASS + message + " Error " + Tools::ToString(code) + ": " + FMOD_ErrorString(code), Logger::Warning);
    }

    void SoundSystem::LoadSound(Sound& sound, std::string const& path)
    {
        if (this->_system == 0)
        {
            this->_logger.Log(CLASS "Sound \"" + path + "\" not loaded (sound system not initialized).", Logger::Warning);
            return;
        }
        FMOD_RESULT result;
        if ((result = this->_system->createSound(path.c_str(), FMOD_DEFAULT, 0, &(sound._sound))) != FMOD_OK)
        {
            this->_ShowError("Failed to load sound \"" + path + "\".", result);
            return;
        }
        if (this->_debug)
            this->_logger.Log(CLASS "Sound \"" + path + "\" loaded.");
    }

    void SoundSystem::UnloadSound(Sound& sound)
    {
        if (this->_system == 0 || sound._sound == 0)
            return;
        FMOD_RESULT result;
        if (this->_debug)
        {
            char nameBuf[512];
            sound._sound->getName(nameBuf, 512);
            std::string name(nameBuf);
            this->_logger.Log(CLASS "Sound \"" + name + "\" unloaded.");
        }
        if ((result = sound._sound->release()) != FMOD_OK)
        {
            char nameBuf[512];
            sound._sound->getName(nameBuf, 512);
            std::string name(nameBuf);
            this->_ShowError("Failed to unload sound \"" + name + "\".", result);
        }
        sound._sound = 0;
    }

    void SoundSystem::PlaySound(Sound& sound)
    {
        if (this->_system == 0 || sound._sound == 0)
            return;
        this->_system->playSound(FMOD_CHANNEL_FREE, sound._sound, false, 0);
    }
}
