/*
 * Copyright (C) 2012-2014 Open Source Robotics Foundation
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
 *
*/
#ifndef _IGNITION_COMMON_TIMER_HH_
#define _IGNITION_COMMON_TIMER_HH_

#include <ignition/common/Time.hh>

namespace ignition
{
  namespace common
  {
    /// \class Timer Timer.hh common/common.hh
    /// \brief A timer class, used to time things in real world walltime
    class Timer
    {
      /// \brief Constructor
      public: Timer();

      /// \brief Destructor
      public: virtual ~Timer();

      /// \brief Start the timer
      public: virtual void Start();

      /// \brief Stop the timer
      public: virtual void Stop();

      /// \brief Returns true if the timer is running.
      /// \return Tue if the timer has been started and not stopped.
      public: bool GetRunning() const;

      /// \brief Get the elapsed time
      /// \return The time
      public: Time GetElapsed() const;

      /// \brief Stream operator friendly
      public: friend std::ostream &operator<<(std::ostream &out,
                                              const ignition::common::Timer &t)
              {
                out << t.GetElapsed();
                return out;
              }

      /// \brief The time of the last call to Start
      private: Time start;

      /// \brief The time when Stop was called.
      private: Time stop;

      /// \brief True if the timer is running.
      private: bool running;
    };
  }
}
#endif

