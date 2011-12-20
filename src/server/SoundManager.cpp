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

#include "SoundManager.hpp"

#define CLASS "[Server/SoundManager] "

#ifdef CMAKE_FMODEX

#include "Gui.hpp"
#include "sound/SoundSystem.hpp"
#include "Logger.hpp"
#include "tools/ToString.hpp"
#include "Server.hpp"
#include "conf/Conf.hpp"

namespace Server
{
    SoundManager::SoundManager(Gui& gui) :
        _gui(gui)
    {
        Conf::Conf& conf = this->_gui.GetServer().GetConf();
        this->_soundLogger = new Logger(this->_gui, conf.Read<bool>("soundLogToUi", true), conf.Read<bool>("soundLogToStdOut", false));
        this->_soundSystem = new Sound::SoundSystem(conf.Read<bool>("debugSound", false), *this->_soundLogger);
        this->_soundLogger->Log(CLASS "Compiled with sound support (\"" CMAKE_FMODEX "\").");

        // Sounds       Id              Path                                                             Enabled
        this->_AddSound(SoundError,     conf.Read<std::string>("soundPathError", "error.wav"),           conf.Read<bool>("soundEnableError", true));
        this->_AddSound(SoundWarning,   conf.Read<std::string>("soundPathWarning", "warning.wav"),       conf.Read<bool>("soundEnableWarning", true));
        this->_AddSound(SoundUp,        conf.Read<std::string>("soundPathUp", "up.wav"),                 conf.Read<bool>("soundEnableUp", true));
    }

    SoundManager::~SoundManager()
    {
        std::map<int, SoundEntity*>::iterator it = this->_sounds.begin();
        std::map<int, SoundEntity*>::iterator itEnd = this->_sounds.end();
        for (; it != itEnd; ++it)
        {
            this->_soundSystem->UnloadSound(it->second->sound);
            delete it->second;
        }
        delete this->_soundSystem;
        delete this->_soundLogger;
    }

    void SoundManager::EnableSound(int id, bool enable)
    {
        std::map<int, SoundEntity*>::iterator it = this->_sounds.find(id);
        if (it == this->_sounds.end())
            this->_soundLogger->Log(CLASS "Invalid sound id " + Tools::ToString(id) + ".", ::Logger::Warning);
        else
            it->second->enabled = enable;
    }

    void SoundManager::PlaySound(int id)
    {
        std::map<int, SoundEntity*>::iterator it = this->_sounds.find(id);
        if (it == this->_sounds.end())
            this->_soundLogger->Log(CLASS "Invalid sound id " + Tools::ToString(id) + ".", ::Logger::Warning);
        else if (it->second->enabled)
            this->_soundSystem->PlaySound(it->second->sound);
    }

    void SoundManager::_AddSound(int id, std::string const& path, bool enabled)
    {
        SoundEntity* s = new SoundEntity;
        s->enabled = enabled;
        this->_soundSystem->LoadSound(s->sound, path);
        this->_sounds[id] = s;
    }
}

#else

#include "Server.hpp"
#include "Gui.hpp"
#include "Logger.hpp"
#include "conf/Conf.hpp"

namespace Server
{
    SoundManager::SoundManager(Gui& gui) :
        _gui(gui)
    {
        Conf::Conf& conf = this->_gui.GetServer().GetConf();
        this->_soundLogger = new Logger(this->_gui, conf.Read<bool>("soundLogToUi", true), conf.Read<bool>("soundLogToStdOut", false));
        this->_soundLogger->Log(CLASS "Not compiled with sound support.");
    }

    SoundManager::~SoundManager()
    {
        delete this->_soundLogger;
    }

    void SoundManager::PlaySound(int)
    {
    }

    void SoundManager::EnableSound(int, bool)
    {
    }
}

#endif
